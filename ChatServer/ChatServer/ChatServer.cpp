// ChatServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Protocol.h"
#include "RingBuffer.h"
#include "CSerializable.h"
#include "lib.h"
#include "Packet.h"
#include "Accept.h"

using namespace std;

SOCKET g_sock;

SOCKET g_socket_list[dfMAX_CLIENT];
list<CLIENT *> g_client_list;

map<DWORD, ROOM *> g_room_list;

DWORD g_userSeq = 0;
DWORD g_roomSeq = 0;

void InitSock(void);
void NetworkProcess(void);
void PacketProc(CLIENT *client);
void SendProc(CLIENT *client);
void DisconnectClient(CLIENT *client);

int main()
{
	InitSock();

	while (1)
	{
//		AcceptClient();
		NetworkProcess();
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
	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock == INVALID_SOCKET)
		return;

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(dfNETWORK_PORT);
	retval = bind(g_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		wprintf(L"bind error\n");
		return;
	}

	retval = listen(g_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		wprintf(L"listen error\n");
		return;
	}

	wprintf(L"Init Socket: port %d\n", dfNETWORK_PORT);
}

void NetworkProcess(void)
{
	int retval;

	FD_SET rset, wset;

	TIMEVAL timeval = { 0, 0 };

	FD_ZERO(&rset);
	FD_ZERO(&wset);

	FD_SET(g_sock, &rset);
	for (list<CLIENT *>::iterator iter = g_client_list.begin(); iter != g_client_list.end(); )
	{
		FD_SET((*iter)->sock, &rset);
		FD_SET((*iter)->sock, &wset);

		iter++;
	}

	retval = select(NULL, &rset, &wset, NULL, &timeval);
	if (retval == SOCKET_ERROR)
	{
		wprintf(L"select error %d\n", GetLastError());
		return;
	}

	if (retval > 0)
	{
		if (FD_ISSET(g_sock, &rset))
		{
			AcceptClient();
		}

		for (list<CLIENT *>::iterator iter = g_client_list.begin(); iter != g_client_list.end(); )
		{
			if (FD_ISSET((*iter)->sock, &rset))
			{
				PacketProc(*iter);
			}

			if (FD_ISSET((*iter)->sock, &wset))
			{
				SendProc(*iter);
			}

			iter++;
		}


	}

	//for (i = 1; i < dfMAX_CLIENT; i++)
	//{
	//	if (g_socket_list[i] != 0)
	//	{
	//		FD_SET(g_socket_list[i], &rset);
	//	}
	//}

}

void PacketProc(CLIENT *client)
{
	int retval;
	char buff[100];
	st_PACKET_HEADER header;
	CAyaPacket packet;

	retval = recv(client->sock, buff, 100, 0);
	if (retval == SOCKET_ERROR || retval == 0)
	{
		//DisconnectClient(client);
	}

	packet.PutData(buff, retval);

	// 헤더 추출
	packet.GetData((char *)&header, sizeof(st_PACKET_HEADER));

	if (header.byCode == dfPACKET_CODE 
		&& Checksum(&header, &packet))
	{
		wprintf(L"msgType : %d\n", header.wMsgType);
		switch (header.wMsgType)
		{
		case df_REQ_LOGIN:
			wprintf(L"login\n");
			AcceptLogin(client, &header, &packet);
			break;

		case df_REQ_ROOM_LIST:
			wprintf(L"room list\n");
			AcceptRoomList(client, &header, &packet);
			break;

		case df_REQ_ROOM_CREATE:
			wprintf(L"room list\n");
			AcceptRoomCreate(client, &header, &packet);
			break;

		case df_REQ_ROOM_ENTER:
			break;

		default:
			wprintf(L"알수 없는 메세지 타입 %d\n", header.wMsgType);
			break;
		}
	}
}

void SendProc(CLIENT * client)
{
	int retval;

	if (client->SendQ.GetUseSize() == 0)
		return;

	retval = send(client->sock, client->SendQ.GetReadBufferPtr(), client->SendQ.GetUseSize(), 0);
	if (retval == SOCKET_ERROR || retval == 0)
	{
		wprintf(L"send error %d\n", GetLastError());
		return;
	}

	client->SendQ.RemoveData(retval);
}

void DisconnectClient(CLIENT * client)
{
	closesocket(client->sock);
	g_client_list.remove(client);

	delete client;
}

