#include "stdafx.h"
#include "Protocol.h"
#include "Network.h"
#include "FileLog.h"
#include "RingBuffer.h"
#include "PlayerObject.h"

extern SOCKET g_sock;
extern BOOL g_isCanSendPacket;
extern BOOL g_successConnect;
extern RingBuffer RecvQ;
extern RingBuffer SendQ;
extern PlayerObject *g_my_player;
extern list<BaseObject *> g_list;

BOOL NetworkProc(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam) != 0)
		return FALSE;

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
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
		g_successConnect = TRUE;

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
	st_PACKET_HEADER header;

	retval = recv(g_sock, recvBuff, 1000, 0);
	if (retval == SOCKET_ERROR || retval == 0)
	{
		FileLog(LOG_FILENAME, "recv error : %d", retval);
		return FALSE;
	}

	if (RecvQ.GetFreeSize() != 0)
		RecvQ.Enqueue(recvBuff, retval);

	while (1)
	{
		if (RecvQ.GetUseSize() < sizeof(st_PACKET_HEADER))
			break;

		peekSize = RecvQ.Peek((char *)&header, sizeof(st_PACKET_HEADER));
		if (peekSize != sizeof(st_PACKET_HEADER))
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
	int peekSize, retval;
	char sendBuff[SEND_BUFFER_SIZE] = { 0, };

	if (g_isCanSendPacket != TRUE)
		return TRUE;

	while (1)
	{
		// 최소 패킷 사이즈 10 (header + st_PACKET_CS_MOVE + end)
		if (SendQ.GetUseSize() < 9)
			break;

		peekSize = SendQ.Peek(sendBuff, SEND_BUFFER_SIZE);
		retval = send(g_sock, sendBuff, peekSize, 0);
		if (retval == SOCKET_ERROR || retval == 0)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				g_isCanSendPacket = FALSE;
				return TRUE;
			}

			return FALSE;
		}

		SendQ.RemoveData(peekSize);
	}

	return TRUE;
}

void PacketProc(BYTE packetType, char *packet)
{
	//FileLog(LOG_FILENAME, "recv packet type: %d", packetType);

	switch (packetType)
	{
	case PACKET_SC_CREATE_MY_CHARACTER:
		PacketProc_CreateMyCharacter(packet);
		break;

	case PACKET_SC_CREATE_OTHER_CHARACTER:
		PacketProc_CreateOtherCharacter(packet);
		break;

	case PACKET_SC_DELETE_CHARACTER:
		packetProc_DeleteCharacter(packet);
		break;

	case PACKET_SC_MOVE_START:
		packetProc_MoveStart(packet);
		break;

	case PACKET_SC_MOVE_STOP:
		packetProc_MoveStop(packet);
		break;

	case PACKET_SC_ATTACK1:
		packetProc_Attack1(packet);
		break;

	case PACKET_SC_ATTACK2:
		packetProc_Attack2(packet);
		break;

	case PACKET_SC_ATTACK3:
		packetProc_Attack3(packet);
		break;

	case PACKET_SC_DAMAGE:
		packetProc_Damage(packet);
		break;
	}
}

void PacketProc_CreateMyCharacter(char * packet)
{
	st_PACKET_CHARACTER *st_player = (st_PACKET_CHARACTER *)packet;

	g_my_player = new PlayerObject();

	g_my_player->SetId(st_player->id);
	g_my_player->SetHp(st_player->hp);
	g_my_player->SetPostion(st_player->x, st_player->y);
	g_my_player->SetObjectType(PLAYER);

	if (st_player->direction == 0)
	{
		g_my_player->SetSprite(PLAYER_STAND_R01, PLAYER_STAND_R_MAX, dfDELAY_STAND);
	}
	else
	{
		g_my_player->SetSprite(PLAYER_STAND_L01, PLAYER_STAND_L_MAX, dfDELAY_STAND);
	}

	g_list.push_back(g_my_player);
}

void PacketProc_CreateOtherCharacter(char * packet)
{
	st_PACKET_CHARACTER *st_player = (st_PACKET_CHARACTER *)packet;

	PlayerObject *player = new PlayerObject();

	player->SetId(st_player->id);
	player->SetHp(st_player->hp);
	player->SetPostion(st_player->x, st_player->y);
	player->SetObjectType(ENEMY);

	if (st_player->direction == 0)
	{
		player->SetSprite(PLAYER_STAND_R01, PLAYER_STAND_R_MAX, dfDELAY_STAND);
	}
	else
	{
		player->SetSprite(PLAYER_STAND_L01, PLAYER_STAND_L_MAX, dfDELAY_STAND);
	}

	g_list.push_back(player);
}

void packetProc_DeleteCharacter(char * packet)
{
	st_PACKET_SC_DELETE *player = (st_PACKET_SC_DELETE *)packet;

	list<BaseObject *>::iterator iter;
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		if ((*iter)->GetId() == player->id)
		{
			g_list.remove(*iter);
			break;
		}
	}

}

void packetProc_MoveStart(char * packet)
{
	st_PACKET_SC_MOVE_START *player = (st_PACKET_SC_MOVE_START *)packet;

	FileLog(LOG_FILENAME, "move start: %d, %d, %d", player->direction, player->x, player->y);

	RecvQ.ClearBuffer();
}

void packetProc_MoveStop(char * packet)
{
}

void packetProc_Attack1(char * packet)
{
}

void packetProc_Attack2(char * packet)
{
}

void packetProc_Attack3(char * packet)
{
}

void packetProc_Damage(char * packet)
{
}
