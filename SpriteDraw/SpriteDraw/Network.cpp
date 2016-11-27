#include "stdafx.h"
#include "Protocol.h"
#include "Network.h"
#include "FileLog.h"
#include "RingBuffer.h"

extern SOCKET g_sock;
extern BOOL g_isCanSendPacket;
extern RingBuffer RecvQ;
extern RingBuffer SendQ;

BOOL NetworkProc(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam) != 0)
		return FALSE;

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		FileLog(LOG_FILENAME, "EVENT FD_READ");
		if ( ! RecvEvent())
			return FALSE;
		break;

	case FD_WRITE:
		FileLog(LOG_FILENAME, "EVENT FD_WRITE");
		g_isCanSendPacket = TRUE;
		if ( ! SendEvent())
			return FALSE;
		break;

	case FD_CONNECT:
		FileLog(LOG_FILENAME, "EVENT FD_CONNECT");
		break;

	case FD_CLOSE:
		FileLog(LOG_FILENAME, "EVENT FD_CLOSE");
		break;
	}

	return TRUE;;
}

BOOL RecvEvent(void)
{
	int retval, peekSize;
	char recvBuff[1000] = { 0, };
	char payloadBuff[10] = { 0, };
	char endCode;
	PACKET_HEADER header;

	retval = recv(g_sock, recvBuff, 1000, 0);
	if (retval == SOCKET_ERROR || retval == 0)
		return FALSE;

	if (RecvQ.GetFreeSize() != 0)
		RecvQ.Enqueue(recvBuff, retval);

	while (1)
	{
		if (RecvQ.GetUseSize() < sizeof(PACKET_HEADER))
			break;

		peekSize = RecvQ.Peek((char *)&header, sizeof(PACKET_HEADER));
		if (peekSize != sizeof(PACKET_HEADER))
			break;

		if (header.code != PACKET_CODE)
			break;

		RecvQ.RemoveData(peekSize);
		// end get header

		// 페이로드 + endcode 만큼 데이터가 있는지 확인
		if ((int)header.size + sizeof(NETWORK_PACKET_END) <= RecvQ.GetUseSize())
		{
			peekSize = RecvQ.Dequeue(payloadBuff, (int)header.size);
			if (peekSize != (int)header.size)
				break;

			peekSize = RecvQ.Dequeue(&endCode, sizeof(NETWORK_PACKET_END));
			if (peekSize != sizeof(NETWORK_PACKET_END))
				break;

			if (endCode != NETWORK_PACKET_END)
				break;

			PacketProc(header.type, payloadBuff);
		}
		else
		{
			break;
		}
	}


	return TRUE;
}

BOOL SendEvent(void)
{
	return TRUE;
}

void PacketProc(BYTE packetType, char *packet)
{
	switch (packetType)
	{
	case PACKET_SC_CREATE_MY_CHARACTER:
		PacketProc_CreateMyCharacter(packet);
		break;
	}
}

void PacketProc_CreateMyCharacter(char * packet)
{

}
