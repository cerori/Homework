// SpriteDraw.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "MainFunc.h"
#include "SpriteDraw.h"
#include "ScreenDib.h"
#include "SpriteDib.h"
#include "PlayerObject.h"
#include "KeyMgr.h"
#include "Protocol.h"
#include "FileLog.h"
#include "RingBuffer.h"
#include "Network.h"

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

// �Լ� ����
void InitWSA(void);

void err_quit(WCHAR *msg);
void err_display(WCHAR *msg);

// ����� ����
SOCKET g_sock;
WCHAR g_ip[] = L"127.0.0.1";
BOOL g_isCanSendPacket;
BOOL g_successConnect;

RingBuffer RecvQ; 
RingBuffer SendQ;

list<BaseObject *> g_list;

PlayerObject *g_player;

// ������ Ȱ��ȭ üũ
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

void InitWSA(void)
{
	int retval;

	g_isCanSendPacket = FALSE;
	g_successConnect = FALSE;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// socket()
	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock == INVALID_SOCKET) err_quit(L"socket()");

	retval = WSAAsyncSelect(g_sock, g_hWnd, WM_USER_SOCKET, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);
	if (retval == SOCKET_ERROR) err_quit(L"WSAAsyncSelect()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	InetPton(AF_INET, g_ip, &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(NETWORK_PORT);

	retval = connect(g_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
}

/*
	����� �Լ�
*/

void InitialGame(void)
{
	ContentLoad();

	// �����
	g_player = new PlayerObject();
	g_player->SetHp(50);
	g_player->SetPostion(100, 200);
	g_player->SetObjectType(PLAYER);
	g_player->SetSprite(PLAYER_STAND_R01, PLAYER_STAND_R_MAX, dfDELAY_STAND);

	g_list.push_back(g_player);

	// ��1
	PlayerObject *g_player1 = new PlayerObject();
	g_player1->SetObjectType(ENEMY);
	g_player1->SetPostion(200, 200);
	g_player1->SetSprite(PLAYER_STAND_L01, PLAYER_STAND_L_MAX, dfDELAY_STAND);

	g_list.push_back(g_player1);

	// ��2
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

	Sleep(0.02f * 1000);
}

/*
1. �̵� �߿� ������ �ԷµǸ� ���߰� ������ �Ѵ�.
*/
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

	//�밢��
	if (g_keyMgr->StayKeyDown(VK_LEFT) && g_keyMgr->StayKeyDown(VK_UP))
		action = dfACTION_MOVE_LU;

	if (g_keyMgr->StayKeyDown(VK_LEFT) && g_keyMgr->StayKeyDown(VK_DOWN))
		action = dfACTION_MOVE_LD;

	if (g_keyMgr->StayKeyDown(VK_RIGHT) && g_keyMgr->StayKeyDown(VK_UP))
		action = dfACTION_MOVE_RU;

	if (g_keyMgr->StayKeyDown(VK_RIGHT) && g_keyMgr->StayKeyDown(VK_DOWN))
		action = dfACTION_MOVE_RD;

	// ����
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

	// ����Ʈ�� ��� ��ü�� ���� �׼��� �Ѵ�.
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		(*iter)->Action();
	}
}

void Draw(void)
{
	BYTE *pDest = g_ScreenDib.GetDibBuffer();
	int destWidth = g_ScreenDib.GetWidth();
	int destHeight = g_ScreenDib.GetHeight();
	int destPitch = g_ScreenDib.GetPitch();

	g_SpriteDib.DrawImage(0, 0, 0, pDest, 640, 480, 640 * 4);

	g_list.sort(compare_axle_y);

	list<BaseObject *>::iterator iter;

	// ����Ʈ�� ��� ��ü�� ���� �׼��� �Ѵ�.
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{		
		(*iter)->Draw(pDest, 640, 480, 640 * 4);
	}

	g_ScreenDib.DrawBuffer(g_hWnd);
}

void err_quit(WCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(WCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	wprintf(L"[%s] %s", msg, (LPWSTR)&lpMsgBuf);
	LocalFree(lpMsgBuf);
}

