// SpriteDraw.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "LinkList.h"
#include "MainFunc.h"
#include "SpriteDraw.h"
#include "ScreenDib.h"
#include "SpriteDib.h"
#include "PlayerObject.h"
#include "KeyMgr.h"

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
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

list<BaseObject *> g_list;

PlayerObject *g_player;

// 윈도우 활성화 체크
BOOL g_isActiveApp;

ScreenDib g_ScreenDib(640, 480, 32);
SpriteDib g_SpriteDib(100, 0x00FFFFFF);

cKey *g_keyMgr = cKey::GetInst();

bool compare_axle_y(BaseObject *first, BaseObject *second)
{
	return (first->GetCurY() < second->GetCurY());
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SPRITEDRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPRITEDRAW));

	InitialGame();

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
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SPRITEDRAW);
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
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
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

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

/*
	사용자 함수
*/

void InitialGame(void)
{
	ContentLoad();

	// 사용자
	g_player = new PlayerObject();
	g_player->SetHp(50);
	g_player->SetPostion(100, 200);
	g_player->SetObjectType(PLAYER);
	g_player->SetSprite(PLAYER_STAND_L01, PLAYER_STAND_L_MAX, dfDELAY_STAND);

	g_list.push_back(g_player);

	// 적1
	PlayerObject *g_player1 = new PlayerObject();
	g_player1->SetObjectType(ENEMY);
	g_player1->SetPostion(200, 200);
	g_player1->SetSprite(PLAYER_STAND_L01, PLAYER_STAND_L_MAX, dfDELAY_STAND);

	g_list.push_back(g_player1);

	// 적2
	PlayerObject *g_player2 = new PlayerObject();
	g_player2->SetObjectType(ENEMY);
	g_player2->SetPostion(300, 200);
	g_player2->SetSprite(PLAYER_STAND_R01, PLAYER_STAND_R_MAX, dfDELAY_STAND);

	g_list.push_back(g_player2);
}

void ContentLoad(void)
{
	g_SpriteDib.LoadDibSprite(0, L"SpriteData/_Map.bmp", 640, 480);
	g_SpriteDib.LoadDibSprite(1, L"SpriteData/Stand_L_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(2, L"SpriteData/Stand_L_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(3, L"SpriteData/Stand_L_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(4, L"SpriteData/Stand_R_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(5, L"SpriteData/Stand_R_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(6, L"SpriteData/Stand_R_03.bmp", 71, 90);

	// move
	g_SpriteDib.LoadDibSprite(7, L"SpriteData/Move_L_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(8, L"SpriteData/Move_L_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(9, L"SpriteData/Move_L_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(10, L"SpriteData/Move_L_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(11, L"SpriteData/Move_L_05.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(12, L"SpriteData/Move_L_06.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(13, L"SpriteData/Move_L_07.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(14, L"SpriteData/Move_L_08.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(15, L"SpriteData/Move_L_09.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(16, L"SpriteData/Move_L_10.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(17, L"SpriteData/Move_L_11.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(18, L"SpriteData/Move_L_12.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(19, L"SpriteData/Move_R_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(20, L"SpriteData/Move_R_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(21, L"SpriteData/Move_R_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(22, L"SpriteData/Move_R_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(23, L"SpriteData/Move_R_05.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(24, L"SpriteData/Move_R_06.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(25, L"SpriteData/Move_R_07.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(26, L"SpriteData/Move_R_08.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(27, L"SpriteData/Move_R_09.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(28, L"SpriteData/Move_R_10.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(29, L"SpriteData/Move_R_11.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(30, L"SpriteData/Move_R_12.bmp", 71, 90);

	// action 1
	g_SpriteDib.LoadDibSprite(31, L"SpriteData/Attack1_L_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(32, L"SpriteData/Attack1_L_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(33, L"SpriteData/Attack1_L_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(34, L"SpriteData/Attack1_R_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(35, L"SpriteData/Attack1_R_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(36, L"SpriteData/Attack1_R_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(37, L"SpriteData/Attack1_R_04.bmp", 71, 90);

	// attack 2
	g_SpriteDib.LoadDibSprite(38, L"SpriteData/Attack2_L_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(39, L"SpriteData/Attack2_L_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(40, L"SpriteData/Attack2_L_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(41, L"SpriteData/Attack2_L_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(42, L"SpriteData/Attack2_R_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(43, L"SpriteData/Attack2_R_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(44, L"SpriteData/Attack2_R_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(45, L"SpriteData/Attack2_R_04.bmp", 71, 90);

	// attack 3
	g_SpriteDib.LoadDibSprite(46, L"SpriteData/Attack3_L_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(47, L"SpriteData/Attack3_L_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(48, L"SpriteData/Attack3_L_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(49, L"SpriteData/Attack3_L_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(50, L"SpriteData/Attack3_L_05.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(51, L"SpriteData/Attack3_L_06.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(52, L"SpriteData/Attack3_R_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(53, L"SpriteData/Attack3_R_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(54, L"SpriteData/Attack3_R_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(55, L"SpriteData/Attack3_R_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(56, L"SpriteData/Attack3_R_05.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(57, L"SpriteData/Attack3_R_06.bmp", 71, 90);

	g_SpriteDib.LoadDibSprite(58, L"SpriteData/HPGuage.bmp", 0, 0);
	g_SpriteDib.LoadDibSprite(59, L"SpriteData/Shadow.bmp", 32, 4);
}

void Update(void)
{
	if (g_isActiveApp)
		KeyProcess();

	Action();

	Draw();

	Sleep(0.02f*1000);
}

void KeyProcess(void)
{
	DWORD action = dfACTION_STAND;

	if (g_keyMgr->StayKeyDown(VK_UP))
		action = dfACTION_MOVE_UU;

	if (g_keyMgr->StayKeyDown(VK_DOWN))
		action = dfACTION_MOVE_DD;

	if (g_keyMgr->StayKeyDown(VK_LEFT))
		action = dfACTION_MOVE_LL;

	if (g_keyMgr->StayKeyDown(VK_RIGHT))
		action = dfACTION_MOVE_RR;

	//대각선
	if (g_keyMgr->StayKeyDown(VK_LEFT) && g_keyMgr->StayKeyDown(VK_UP))
		action = dfACTION_MOVE_LU;

	if (g_keyMgr->StayKeyDown(VK_LEFT) && g_keyMgr->StayKeyDown(VK_DOWN))
		action = dfACTION_MOVE_LD;

	if (g_keyMgr->StayKeyDown(VK_RIGHT) && g_keyMgr->StayKeyDown(VK_UP))
		action = dfACTION_MOVE_RU;

	if (g_keyMgr->StayKeyDown(VK_RIGHT) && g_keyMgr->StayKeyDown(VK_DOWN))
		action = dfACTION_MOVE_RD;

	if (g_keyMgr->OnceKeyDown(e_Z))
		action = dfACTION_ATTACK1;

	if (g_keyMgr->OnceKeyDown(e_X))
		action = dfACTION_ATTACK2;

	if (g_keyMgr->OnceKeyDown(e_C))
		action = dfACTION_ATTACK3;

	g_player->ActionInput(action);
}

void Action(void)
{
	list<BaseObject *>::iterator iter;

	// 리스트의 모든 객체를 돌며 액션을 한다.
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		(*iter)->Action();
	}
}

void Draw(void)
{
	BaseObject *object;

	BYTE *pDest = g_ScreenDib.GetDibBuffer();
	int destWidth = g_ScreenDib.GetWidth();
	int destHeight = g_ScreenDib.GetHeight();
	int destPitch = g_ScreenDib.GetPitch();

	g_SpriteDib.DrawImage(0, 0, 0, pDest, 640, 480, 640 * 4);

	g_list.sort(compare_axle_y);

	list<BaseObject *>::iterator iter;

	// 리스트의 모든 객체를 돌며 액션을 한다.
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{		
		(*iter)->Draw(pDest, 640, 480, 640 * 4);
	}

	g_ScreenDib.DrawBuffer(g_hWnd);
}