// SpriteDraw.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
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

// ���� ����:
HWND g_hWnd;
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK    DialogProc(HWND, UINT, WPARAM, LPARAM);

// ����� ����
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

// ������ Ȱ��ȭ üũ
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
	
	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SPRITEDRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	ContentLoad();
	InitialGame();
	InitWSA();

	// �⺻ �޽��� �����Դϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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

   //�ػ� ���ϱ�
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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND	- ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT	- �� â�� �׸��ϴ�.
//  WM_DESTROY	- ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
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
			MessageBox(g_hWnd, L"������ ����Ǿ����ϴ�.", L"�˸�", MB_OK);
			PostQuitMessage(0);
			return 0;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.

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

