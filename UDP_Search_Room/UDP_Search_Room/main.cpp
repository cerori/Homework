#pragma comment(lib, "ws2_32")

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include <Windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>

using namespace std;

#define REMOTEIP	"255.255.255.255"
#define MAXPORTLIST	10
#define SOCKETMAX	64

/*
프로토콜 - 0xff 0xee 0xdd 0xaa 0x00 0x99 0x77 0x55 0x33 0x11   ( 10 Byte )

포트범위 - 19001 ~ 19099
*/

BYTE send_data[] = { 0xff, 0xee, 0xdd, 0xaa, 0x00, 0x99, 0x77, 0x55, 0x33, 0x11 };

SOCKET sock;

void err_quit(char *msg);
void err_display(char *msg);

int main()
{
	int port, retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	BOOL bEnable = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&bEnable, sizeof(bEnable));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	timeval timeout = { 0, 20000 };
	FD_SET rset;
	WCHAR buf[100];
	WCHAR peer_ip[20];

	SOCKADDR_IN remoteaddr;
	SOCKADDR_IN peeraddr;
	int addrlen = sizeof(peeraddr);


	for (port = 19001; port <= 19099; port++)
	{
		ZeroMemory(&buf, sizeof(buf));
		
		// bind()
		ZeroMemory(&remoteaddr, sizeof(remoteaddr));
		remoteaddr.sin_family = AF_INET;
		inet_pton(AF_INET, REMOTEIP, &remoteaddr.sin_addr.s_addr);
		remoteaddr.sin_port = htons(port);

		// sendto
		retval = sendto(sock, (char *)send_data, 10, 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) err_display("sendto()");

		FD_ZERO(&rset);
		FD_SET(sock, &rset);

		retval = select(0, &rset, NULL, NULL, &timeout);
		if (retval == SOCKET_ERROR) err_display("select()");
		if (FD_ISSET(sock, &rset))
		{
			// recvfrom
			retval = recvfrom(sock, (char *)buf, 500, 0, (SOCKADDR *)&peeraddr, &addrlen);
			InetNtop(AF_INET, &peeraddr.sin_addr, peer_ip, 20);
			wprintf(L"%15s:%d ] %s\n", peer_ip, port, buf);
		}
	}

	WSACleanup();

	return 0;
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
