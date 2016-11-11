// server_online_star.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define SERVERPORT	3000

#define GETID		0
#define CONNECT		1
#define DISCONNECT	2
#define MOVE		3

#define INIT_POS_X	20
#define INIT_POS_Y	20

struct PACKET
{
	int type;
	int id;
	int x;
	int y;
};

struct CLIENT
{
	SOCKET sock;
	PACKET packet;
};

FD_SET g_rset;
CLIENT clients[FD_SETSIZE];

SOCKET g_listen_sock;
int g_connection_id = 1;
int totalClient = 0;

void InitSock(void);
void InitClient(void);

void AddClient(SOCKET sock, int id, int x, int y);
void RemoveClient(SOCKET sock);
void ChangeClientInfo(SOCKET sock, int x, int y);
void BroadcastConnectToMe(SOCKET sock, PACKET me);
void BroadcastConnectWithOutMe(PACKET pack);
void BroadcastMove(void);
void BroadcastDisconnect(PACKET packet);

void err_quit(char *msg);
void err_display(char *msg);

int main()
{
	InitSock();

	InitClient();

	timeval sock_timeout = { 0, 0 };

	int i, retval;
	int client_id;
	SOCKET client_sock;
	PACKET packet;

	while (1)
	{
		FD_ZERO(&g_rset);
		FD_SET(g_listen_sock, &g_rset);

		for (i = 0; i < FD_SETSIZE; i++)
		{
			if (clients[i].sock != INVALID_SOCKET)
			{
				FD_SET(clients[i].sock, &g_rset);
			}
		}

		retval = select(0, &g_rset, NULL, NULL, &sock_timeout);
		if (retval == SOCKET_ERROR) err_display("select()");

		// 새로운 클라이언트 확인
		if (FD_ISSET(g_listen_sock, &g_rset))
		{
			client_sock = accept(g_listen_sock, NULL, NULL);
			if (client_sock == SOCKET_ERROR) err_display("accept()");
			
			// 아이디 부여
			client_id = g_connection_id++;

			AddClient(client_sock, client_id, INIT_POS_X, INIT_POS_Y);

			packet.id = client_id;
			packet.type = GETID;

			// 아이디 전송
			retval = send(client_sock, (char *)&packet, sizeof(packet), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send id");
			}

			packet.type = CONNECT;
			packet.x = INIT_POS_X;
			packet.y = INIT_POS_Y;

			// 초기 좌표 전송
			retval = send(client_sock, (char *)&packet, sizeof(packet), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send new connect");
			}

			// 클라이언트에 새로운 접속 알림
			BroadcastConnectWithOutMe(packet);

			// 자신에게 기존 접속 알림
			BroadcastConnectToMe(client_sock, packet);
		}

		for (i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(clients[i].sock, &g_rset))
			{
				retval = recv(clients[i].sock, (char *)&packet, sizeof(packet), 0);
				if (retval == SOCKET_ERROR || retval == 0)
				{
					err_display("client recv");
					RemoveClient(clients[i].sock);
					break;
				}

				// 좌표 업데이트
				if (clients[i].packet.id == packet.id)
				{
					clients[i].packet.x = packet.x;
					clients[i].packet.y = packet.y;
				}

				BroadcastMove();

				break;
			}
		}

		system("cls");
		printf("Pack's online star\n");
		printf("Connect Client: %d\n", totalClient);
		for (i =0; i < FD_SETSIZE; i++)
		{
			if (clients[i].sock != INVALID_SOCKET)
			{
				printf("%d, %d, %d\n", clients[i].packet.id, clients[i].packet.x, clients[i].packet.y);
			}
		}
	}

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
	g_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(g_listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	retval = listen(g_listen_sock, FD_SETSIZE);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");
}

void InitClient(void)
{
	int i;

	for (i = 0; i < FD_SETSIZE; i++)
	{
		clients[i].sock = INVALID_SOCKET;
		clients[i].packet.id = -1;
	}
}

void AddClient(SOCKET sock, int id, int x, int y)
{
	int i;

	if (totalClient > FD_SETSIZE)
		return;

	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].packet.id == -1)
		{
			totalClient++;

			clients[i].sock = sock;
			clients[i].packet.id = id;
			clients[i].packet.x = x;
			clients[i].packet.y = y;

			break;
		}
	}
}

void RemoveClient(SOCKET sock)
{
	int i;

	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].sock == sock)
		{
			totalClient--;

			clients[i].sock = INVALID_SOCKET;
			closesocket(sock);

			BroadcastDisconnect(clients[i].packet);
			break;
		}
	}
}

void ChangeClientInfo(SOCKET sock, int x, int y)
{
	int i;

	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].sock == sock)
		{
			clients[i].packet.x = x;
			clients[i].packet.y = y;
			break;
		}
	}
}

void BroadcastMove(void)
{
	int i, j, retval;

	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].sock != INVALID_SOCKET)
		{
			for (j = 0; j < FD_SETSIZE; j++)
			{
				if (clients[j].sock != INVALID_SOCKET)
				{
					clients[j].packet.type = MOVE;

					retval = send(clients[i].sock, (char *)&clients[j].packet, sizeof(clients[j].packet), 0);
					if (retval == SOCKET_ERROR)
					{
						RemoveClient(clients[i].sock);
						err_display("connect()");
					}
				}
			}
		}
	}
}

void BroadcastDisconnect(PACKET packet)
{
	int i, retval;

	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].sock != INVALID_SOCKET)
		{
			packet.type = DISCONNECT;

			retval = send(clients[i].sock, (char *)&packet, sizeof(packet), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("disconnect");
			}
		}
	}
}

void BroadcastConnectWithOutMe(PACKET me)
{
	int i, retval;

	// 기존 접속자에게 새로운 접속자를 알려준다
	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].sock != INVALID_SOCKET && clients[i].packet.id != me.id)
		{
			me.type = CONNECT;

			retval = send(clients[i].sock, (char *)&me, sizeof(me), 0);
			if (retval == SOCKET_ERROR)
			{
				RemoveClient(clients[i].sock);
				err_display("connect()");
			}
		}
	}
}

void BroadcastConnectToMe(SOCKET sock, PACKET me)
{
	int i, retval;

	// 자신에게 기존 접속자를 알려준다
	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (clients[i].sock != INVALID_SOCKET && clients[i].packet.id != me.id)
		{
			clients[i].packet.type = CONNECT;

			retval = send(sock, (char *)&clients[i].packet, sizeof(clients[i].packet), 0);
			if (retval == SOCKET_ERROR)
			{
				RemoveClient(clients[i].sock);
				err_display("connect()");
			}
		}
	}
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

