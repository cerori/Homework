// online_draw.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "online_draw.h"
#include "RingBuffer.h"
#include "FileLog.h"

#define SERVERPORT	25000
#define WM_USER_SOCKET (WM_USER + 1)
#define PACKET_LENGTH	16
#define LOG_FILENAME	"online_draw.log"

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
HWND hWnd;
WCHAR *ClassName = L"MyClass";
WCHAR g_ip[16];

int count = 0;

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

struct st_CLIENT
{
	SOCKET sock;
	RingBuffer recvQ;
	RingBuffer sendQ;
	BOOL isCanSend = TRUE;
};

st_CLIENT g_client;

void InitSock(void);
void EnqueuePacket(int len, int startX, int startY, int endX, int endY);
void SendProc(void);

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

	// 소켓 초기화
	InitSock();
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

    // 기본 메시지 루프입니다.
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

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// socket()
	g_client.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_client.sock == INVALID_SOCKET) err_quit(L"socket()");

	retval = WSAAsyncSelect(g_client.sock, hWnd, WM_USER_SOCKET, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);
	if (retval == SOCKET_ERROR) err_quit(L"WSAAsyncSelect()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	InetPton(AF_INET, g_ip, &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(SERVERPORT);

	retval = connect(g_client.sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
}

void EnqueuePacket(int packet_len, int startX, int startY, int endX, int endY)
{
	int retval;
	st_DRAW_PACKET packet;

	packet.Len = packet_len;
	packet.iStartX = startX;
	packet.iStartY = startY;
	packet.iEndX = endX;
	packet.iEndY = endY;

	retval = g_client.sendQ.Enqueue((char *)&packet, sizeof(packet));
}

void SendProc()
{
	int retval, peekSize;
	char buff[1000];

	while (1)
	{
		if (g_client.sendQ.GetUseSize() == 0 || g_client.isCanSend == FALSE)
			break;

		peekSize = g_client.sendQ.Peek(buff, 1000);
		if (peekSize == 0)
			break;
		
		retval = send(g_client.sock, buff, peekSize, 0);
		if (retval == SOCKET_ERROR || retval == 0)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				g_client.isCanSend = FALSE;

			break;
		}
		//FileLog(LOG_FILENAME, "%d send size %d", count++, peekSize);

		g_client.sendQ.RemoveData(peekSize);
	}
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	static int x;
	static int y;
	static BOOL nowDraw = FALSE;
	st_DRAW_PACKET packet;
	int recvSize, endX, endY;
	int enqueueSize, peekSize;
	
	switch (message)
	{
	case WM_USER_SOCKET:
		if (WSAGETSELECTERROR(lParam) == WSAEWOULDBLOCK)
			return 0;
		
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			//FileLog(LOG_FILENAME, "%d FD_READ", count++);

			char buff[1000];
			
			recvSize = recv(wParam, buff, 1000, 0);
			if (recvSize == SOCKET_ERROR || recvSize == 0)
				break;

			enqueueSize = g_client.recvQ.Enqueue(buff, recvSize);

			//FileLog(LOG_FILENAME, "%d recv(%d, %d, %d, %d)",
			//	count++, packet.iStartX, packet.iStartY, packet.iEndX, packet.iEndY);

			while (1)
			{
				peekSize = g_client.recvQ.Peek((char *)&packet, sizeof(packet));
				if (peekSize == 0 || peekSize != sizeof(packet))
					break;

				// 시작 점과 끝점이 같으면 그려지지 않는다!!
				hdc = GetDC(hWnd);
				MoveToEx(hdc, packet.iStartX, packet.iStartY, NULL);
				LineTo(hdc, packet.iEndX, packet.iEndY);
				ReleaseDC(hWnd, hdc);

				g_client.recvQ.RemoveData(peekSize);
			}

			break;
		case FD_WRITE:
			//FileLog(LOG_FILENAME, "%d FD_WRITE", count++);

			g_client.isCanSend = TRUE;
			//SendProc(wParam, lParam);
			break;
		case FD_CONNECT:
			//FileLog(LOG_FILENAME, "%d FD_CONNECT", count++);

			break;
		case FD_CLOSE:
			//FileLog(LOG_FILENAME, "%d FD_CLOSE", count++);

			break;
		}
		break;

	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		nowDraw = TRUE;
		return 0;

	case WM_MOUSEMOVE:
		if (nowDraw == TRUE)
		{
			//packet.Len = 16;
			//packet.iStartX = x;
			//packet.iStartY = y;

			//MoveToEx(hdc, x, y, NULL);
			endX = LOWORD(lParam);
			endY = HIWORD(lParam);

			//packet.iEndX = x;
			//packet.iEndY = y;

			//LineTo(hdc, x, y);
			//ReleaseDC(hWnd, hdc);

			EnqueuePacket(PACKET_LENGTH, x, y, endX, endY);
			SendProc();

			x = endX;
			y = endY;
			//retval = send(g_sock, (char *)&packet, sizeof(packet), 0);
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

// 소켓 함수 오류 출력
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

