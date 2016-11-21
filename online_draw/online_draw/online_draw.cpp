// online_draw.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "online_draw.h"

#define SERVERPORT	25000
#define WM_SOCKET (WM_USER + 1)

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
HWND hWnd;
WCHAR *ClassName = L"MyClass";
WCHAR g_ip[16];

SOCKET g_sock;

#pragma pack(1)
struct st_DRAW_PACKET
{
	unsigned short	Len;
	int		iStartX;
	int		iStartY;
	int		iEndX;
	int		iEndY;
};
#pragma pack()

void InitSock(void);

void err_quit(WCHAR *msg);
void err_display(WCHAR *msg);

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK    DialogProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);

	WNDCLASSEXW wcex;

	hInst = hInstance;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = ClassName;
	wcex.hIconSm = NULL;

	RegisterClassExW(&wcex);

	hWnd = CreateWindowW(ClassName, NULL, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	// ���� �ʱ�ȭ
	InitSock();
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

    // �⺻ �޽��� �����Դϴ�.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

void InitSock(void)
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// socket()
	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock == INVALID_SOCKET) err_quit(L"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	InetPton(AF_INET, g_ip, &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(SERVERPORT);

	retval = WSAAsyncSelect(g_sock, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);
	retval = connect(g_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
}


//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	static int x;
	static int y;
	static BOOL nowDraw = FALSE;
	st_DRAW_PACKET packet;
	int retval;


	switch (message)
	{
	case WM_SOCKET:
		if (WSAGETSELECTERROR(lParam) != WSAEWOULDBLOCK)
		{
			switch (WSAGETSELECTEVENT(lParam))
			{
			case FD_READ:
				// recvQ �� �ֱ� ���Ͽ� ���� ���� �ʿ�
				retval = recv(wParam, (char *)&packet, sizeof(packet), 0);
				if (retval == SOCKET_ERROR)
					break;

				// recvQ.put

				// while (1)
				{
					hdc = GetDC(hWnd);
					MoveToEx(hdc, packet.iStartX, packet.iStartY, NULL);
					LineTo(hdc, packet.iEndX, packet.iEndY);
					ReleaseDC(hWnd, hdc);
				}
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		nowDraw = TRUE;
	case WM_MOUSEMOVE:
		hdc = GetDC(hWnd);
		if (nowDraw == TRUE)
		{
			packet.Len = 16;
			packet.iStartX = x;
			packet.iStartY = y;

//			MoveToEx(hdc, x, y, NULL);
			x = LOWORD(lParam);
			y = HIWORD(lParam);

			packet.iEndX = x;
			packet.iEndY = y;

//			LineTo(hdc, x, y);
//			ReleaseDC(hWnd, hdc);

			retval = send(g_sock, (char *)&packet, sizeof(packet), 0);
			//if (retval == SOCKET_ERROR)
			//	break;

		}
		break;
	case WM_LBUTTONUP:
		nowDraw = FALSE;
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
		hEditBox = GetDlgItem(hWnd, IDC_EDIT2);
		SetWindowText(hEditBox, L"127.0.0.1");
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			GetDlgItemText(hWnd, IDC_EDIT2, g_ip, 16);
			EndDialog(hWnd, 99939);
			return TRUE;
		}
		return TRUE;
	}

	return FALSE;

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

