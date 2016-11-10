// online_star.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define SERVERPORT	3000
#define SOCKETMAX	64

#define GETID		0
#define CONNECT		1
#define DISCONNECT	2
#define MOVE		3

struct PACKET
{
	int type;
	int id;
	int x;
	int y;
};

struct CURPOS
{
	BOOL is_change;
	int x;
	int y;
};

CURPOS g_xy = { FALSE, 0, 0 };

PACKET clients[FD_SETSIZE];

int my_id = -1;
int totalClient = 0;
char input_ip[20] = "127.0.0.1";

SOCKET sock;

void InitSock(void);
void GetId(void);
void ConnectServer(void);
void KeyProc(void);
void NetworkProc(void);
void Draw(void);
void AddClient(int id, int x, int y);
void RemoveClient(int id);
void ChangeClientInfo(int id, int x, int y);
void SetCurrentCursorPos(int x, int y);
void RemoveCursor(void);

void err_quit(char *msg);
void err_display(char *msg);

int main()
{
	//cout << "아이피를 입력하세요 :";
	//cin >> input_ip;
	RemoveCursor();

	printf("InitSock\n");
	InitSock();

	printf("GetId\n");
	GetId();

	printf("connect server\n");
	ConnectServer();

	printf("run\n");

	while (1)
	{
		KeyProc();
		NetworkProc();
		Draw();

		Sleep(50);
	}

	WSACleanup();

	return 0;
}

void InitSock(void)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, input_ip, &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");
}

void GetId(void)
{
	int retval;
	PACKET packet;

	// 아이디 할당
	packet.type = 0;
	retval = send(sock, (char *)&packet, sizeof(packet), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
	}

	retval = recv(sock, (char *)&packet, sizeof(packet), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
	}
	else if (retval == 0)
	{
		err_display("아이디 얻기 실패");
		return;
	}

	my_id = ((PACKET)packet).id;
}

void ConnectServer(void)
{
	int retval;
	PACKET packet;

	// 신규 접속
	packet.type = 1;
	retval = send(sock, (char *)&packet, sizeof(packet), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
	}

	retval = recv(sock, (char *)&packet, sizeof(packet), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
	}
	else if (retval == 0)
	{
		err_display("접속 실패");
		return;
	}

	g_xy.x = packet.x;
	g_xy.y = packet.y;

	AddClient(packet.id, packet.x, packet.y);
}

void KeyProc(void)
{
	int retval;
	PACKET packet;

	if (GetAsyncKeyState(VK_UP) & 0x8001)
	{
		g_xy.is_change = TRUE;
		g_xy.y -= 1;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8001)
	{
		g_xy.is_change = TRUE;
		g_xy.y += 1;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8001)
	{
		g_xy.is_change = TRUE;
		g_xy.x -= 1;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8001)
	{
		g_xy.is_change = TRUE;
		g_xy.x += 1;
	}

	//대각선
	if ((GetAsyncKeyState(VK_LEFT) & 0x8001) && (GetAsyncKeyState(VK_UP) & 0x8001))
	{
		g_xy.is_change = TRUE;
		g_xy.x -= 1;
		g_xy.y -= 1;
	}

	if ((GetAsyncKeyState(VK_LEFT) & 0x8001)&& (GetAsyncKeyState(VK_DOWN) & 0x8001))
	{
		g_xy.is_change = TRUE;
		g_xy.x -= 1;
		g_xy.y += 1;
	}

	if ((GetAsyncKeyState(VK_RIGHT) & 0x8001) && (GetAsyncKeyState(VK_UP) & 0x8001))
	{
		g_xy.is_change = TRUE;
		g_xy.x += 1;
		g_xy.y -= 1;
	}

	if ((GetAsyncKeyState(VK_RIGHT) & 0x8001) && (GetAsyncKeyState(VK_DOWN) & 0x8001))
	{
		g_xy.is_change = TRUE;
		g_xy.x += 1;
		g_xy.y += 1;
	}

	if (g_xy.is_change)
	{
		packet.type = 3;
		packet.id = my_id;
		packet.x = g_xy.x;
		packet.y = g_xy.y;

		retval = send(sock, (char *)&packet, sizeof(packet), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			exit(1);
		}

		retval = recv(sock, (char *)&packet, sizeof(packet), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			exit(1);
		}

		//printf("%d, %d, %d, %d, %d\n", retval, ((PACKET)packet).type, ((PACKET)packet).id, ((PACKET)packet).x, ((PACKET)packet).y);

		g_xy.is_change = FALSE;
	}
}

void NetworkProc(void)
{
	int retval;
	PACKET packet;
	timeval timeout = { 0, 0 };

	while (1)
	{
		FD_SET rset;
		FD_ZERO(&rset);
		FD_SET(sock, &rset);

		retval = select(0, &rset, NULL, NULL, &timeout);
		if (retval == SOCKET_ERROR) err_display("select()");

		if (FD_ISSET(sock, &rset) == 0)
			break;

		retval = recv(sock, (char *)&packet, sizeof(packet), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
		}

		switch (((PACKET)packet).type)
		{
		case GETID:
			printf("get id\n");
			my_id = ((PACKET)packet).id;
			break;
		case CONNECT:
			printf("connect\n");
			AddClient(((PACKET)packet).id, ((PACKET)packet).x, ((PACKET)packet).y);
			break;
		case DISCONNECT:
			printf("disconnect\n");
			RemoveClient(((PACKET)packet).id);
			break;
		case MOVE:
			printf("move %d\n", ((PACKET)packet).id);
			ChangeClientInfo(((PACKET)packet).id, ((PACKET)packet).x, ((PACKET)packet).y);
			break;
		}
	}
}

void Draw(void)
{
	int i;

	system("cls");

	printf("Connent Client: %d", totalClient);

	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].id != 0)
		{
			SetCurrentCursorPos(clients[i].x, clients[i].y);
			printf("*");
		}
	}
}

void AddClient(int id, int x, int y)
{
	int i;

	if (totalClient > FD_SETSIZE)
		return;

	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].id == 0)
		{
			totalClient++;

			clients[i].id = id;
			clients[i].x = x;
			clients[i].y = y;
			break;
		}
	}
}

void RemoveClient(int id)
{
	int i;

	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].id == id)
		{
			totalClient--;

			clients[i].id = 0;
			break;
		}
	}
}

void ChangeClientInfo(int id, int x, int y)
{
	int i;

	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].id == id)
		{
			clients[i].x = x;
			clients[i].y = y;
			break;
		}
	}
}

void SetCurrentCursorPos(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}


void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
