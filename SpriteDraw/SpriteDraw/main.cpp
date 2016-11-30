// SpriteDraw.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "MainFunc.h"
#include "SpriteDraw.h"
#include "ScreenDib.h"
#include "SpriteDib.h"
#include "PlayerObject.h"
#include "EffectObject.h"
#include "KeyMgr.h"
#include "Protocol.h"
#include "FileLog.h"
#include "RingBuffer.h"
#include "Network.h"
#include "Packet.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HWND g_hWnd;
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK    DialogProc(HWND, UINT, WPARAM, LPARAM);

// 사용자 전역
SOCKET g_sock;
WCHAR g_ip[] = L"192.168.10.103";
BOOL g_isCanSendPacket;
BOOL g_successConnect;
BOOL g_beforeAction;
BOOL g_currentMoveDirection;
BOOL g_isMoveChange = FALSE;

RingBuffer RecvQ; 
RingBuffer SendQ;

DWORD g_lineNum = 0;

list<BaseObject *> g_list;

PlayerObject *g_my_player;
EffectObject *g_effect;

// 윈도우 활성화 체크
BOOL g_isActiveApp;

ScreenDib g_ScreenDib(640, 480, 32);
SpriteDib g_SpriteDib(100, 0x00FFFFFF);

cKey *g_keyMgr = cKey::GetInst();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
	
	// TODO: 여기에 코드를 입력합니다.
	MSG msg;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SPRITEDRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	ContentLoad();
	InitialGame();
	InitWSA();

	// 기본 메시지 루프입니다.
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (g_successConnect && g_my_player != NULL)
				Update();
		}
	}

	return (int) msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPRITEDRAW));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   g_hWnd = CreateWindow(	szWindowClass, 
							szTitle, 
							WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
							CW_USEDEFAULT, 
							CW_USEDEFAULT, 
							640, 
							480, 
							NULL, 
							NULL,
							hInstance, 
							NULL);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);
   SetFocus(g_hWnd);

   RECT winRect;
   winRect.top = 0;
   winRect.left = 0;
   winRect.right = 640;
   winRect.bottom = 480;

   AdjustWindowRectEx(	&winRect,
						GetWindowStyle(g_hWnd),
						GetMenu(g_hWnd) != NULL,
						GetWindowExStyle(g_hWnd));

   //해상도 구하기
   int nResolutionX = GetSystemMetrics(SM_CXSCREEN);
   int nResolutionY = GetSystemMetrics(SM_CYSCREEN);

   MoveWindow(	g_hWnd, 
				nResolutionX / 2 - winRect.right / 2,
				nResolutionY / 2 - winRect.bottom / 2,
				winRect.right - winRect.left,
				winRect.bottom - winRect.top,
				TRUE);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_USER_SOCKET:
		if ( ! NetworkProc(wParam, lParam))
		{
			MessageBox(g_hWnd, L"접속이 종료되었습니다.", L"알림", MB_OK);
			PostQuitMessage(0);
			return 0;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.

		EndPaint(hWnd, &ps);
		break;
	case WM_ACTIVATEAPP:
		g_isActiveApp = (BOOL)wParam;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

BOOL CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hEditBox;

	switch (msg)
	{
	case WM_INITDIALOG:
		memset(g_ip, 0, sizeof(WCHAR) * 16);
		hEditBox = GetDlgItem(hWnd, IDC_EDIT1);
		SetWindowText(hEditBox, L"127.0.0.1");
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			GetDlgItemText(hWnd, IDC_EDIT1, g_ip, 16);
			EndDialog(hWnd, 99939);
			return TRUE;
		}
		return TRUE;
	}

	return FALSE;

}

