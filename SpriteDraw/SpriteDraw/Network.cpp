#include "stdafx.h"
#include "Protocol.h"
#include "Network.h"
#include "FileLog.h"
#include "RingBuffer.h"
#include "PlayerObject.h"
#include "EffectObject.h"

extern SOCKET g_sock;
extern BOOL g_isCanSendPacket;
extern BOOL g_successConnect;
extern RingBuffer RecvQ;
extern RingBuffer SendQ;
extern PlayerObject *g_my_player;
extern EffectObject *g_effect;
extern list<BaseObject *> g_list;
extern DWORD g_lineNum;

static int _recvLineNum = 0;
static int _sendLineNum = 0;

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
		return FALSE;
	}

	return TRUE;
}

BOOL RecvEvent(void)
{
	BOOL successPeek = FALSE;
	int retval, peekSize;
	char recvBuff[1000] = { 0, };
	char payloadBuff[10] = { 0, };
	char endCode;
	st_PACKET_HEADER header;

	retval = recv(g_sock, recvBuff, 1000, 0);
	if (retval == SOCKET_ERROR || retval == 0)
	{
		FileLog(LOG_FILENAME, "%4d Recvice Error : %d", g_lineNum++, retval);
		return FALSE;
	}

	FileLog(RECV_LOG_FILENAME, "%4d recv size %d", _recvLineNum++, retval);
	FileLog(RECV_LOG_FILENAME, "%4d\tqueue free size %d", _recvLineNum++, RecvQ.GetFreeSize());

	if (RecvQ.GetFreeSize() != 0)
		retval = RecvQ.Enqueue(recvBuff, retval);

	FileLog(RECV_LOG_FILENAME, "%4d\tEnqueue size %d", _recvLineNum++, retval);

	while (1)
	{
		if (RecvQ.GetUseSize() == 0 && RecvQ.GetUseSize() < sizeof(st_PACKET_HEADER))
			break;

		peekSize = RecvQ.Peek((char *)&header, sizeof(st_PACKET_HEADER));
		FileLog(RECV_LOG_FILENAME, "%4d\tcheck header size", _recvLineNum++);
		if (peekSize != sizeof(st_PACKET_HEADER))
		{
			FileLog(RECV_LOG_FILENAME, "%4d\t\tbreak!!!!!!!!!!!!!!!", _recvLineNum++);
			break;
		}

		FileLog(RECV_LOG_FILENAME, "%4d\tcheck header code", _recvLineNum++);
		if (header.code != PACKET_CODE)
		{
			RecvQ.RemoveData(peekSize);
			FileLog(RECV_LOG_FILENAME, "%4d\t\tbreak!!!!!!!!!!!!!!!!!", _recvLineNum++);
			break;
		}

		FileLog(RECV_LOG_FILENAME, "%4d\tpeek(%d) size remove", _recvLineNum++, peekSize);
		RecvQ.RemoveData(peekSize);
		// end get header

		// 페이로드 + endcode 만큼 데이터가 있는지 확인
		FileLog(RECV_LOG_FILENAME, "%4d\tpayload peek", _recvLineNum++);
		if (header.size + sizeof(NETWORK_PACKET_END) <= RecvQ.GetUseSize())
		{
			peekSize = RecvQ.Dequeue(payloadBuff, (int)header.size);
			FileLog(RECV_LOG_FILENAME, "%4d\tcheck payload size", _recvLineNum++);
			if (peekSize < 1 || peekSize != header.size)
			{
				FileLog(RECV_LOG_FILENAME, "%4d\t\t%d, %d different!!!!!!!!!!!", _recvLineNum++, peekSize, header.size);
				break;
			}

			peekSize = RecvQ.Dequeue(&endCode, sizeof(NETWORK_PACKET_END));
			FileLog(RECV_LOG_FILENAME, "%4d\tdequeue packet end", _recvLineNum++);
			if (peekSize <= 0 || peekSize != sizeof(NETWORK_PACKET_END))
			{
				FileLog(RECV_LOG_FILENAME, "%4d\t\t%d %dbreak!!!!!!!!!!!!!!", _recvLineNum++, peekSize, sizeof(NETWORK_PACKET_END));
				break;
			}

			FileLog(RECV_LOG_FILENAME, "%4d\tcheck packet end", _recvLineNum++);
			if (endCode != NETWORK_PACKET_END)
			{
				FileLog(RECV_LOG_FILENAME, "%4d\t\tbreak!!!!!!!!!!!!!!!!", _recvLineNum++);
			}

			FileLog(RECV_LOG_FILENAME, "%4d Packet Procedure %d", _recvLineNum++, header.type);
			PacketProc(header.type, payloadBuff);
		}
		else
		{
			FileLog(RECV_LOG_FILENAME, "%4d\t\tpayload peek fail!!!!!!!!!!!!", _recvLineNum++);
			break;
		}
	}


	return TRUE;
}

BOOL SendEvent(void)
{
	int peekSize, retval;
	char sendBuff[SEND_BUFFER_SIZE] = { 0, };

	if (g_isCanSendPacket == FALSE)
		return TRUE;

	FileLog(SEND_LOG_FILENAME, "%4d Send Event", _sendLineNum++);
	while (1)
	{
		// 패킷중 제일 작은 사이즈
		if (SendQ.GetUseSize() < sizeof(st_PACKET_SC_DAMAGE) - 1)
			break;

		peekSize = SendQ.Peek(sendBuff, SEND_BUFFER_SIZE);
		if (peekSize < 1)
			break;

		FileLog(SEND_LOG_FILENAME, "%4d\tpeek size : %d", _sendLineNum++, peekSize);

		retval = send(g_sock, sendBuff, peekSize, 0);
		if (retval == SOCKET_ERROR || retval == 0)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				FileLog(SEND_LOG_FILENAME, "%4d\t\tWSAEWOULDBLOCK!!!!!!!!", _sendLineNum++);
				g_isCanSendPacket = FALSE;
				return TRUE;
			}

			FileLog(SEND_LOG_FILENAME, "%4d\t\tSEND SOCKET ERROR!!!!!!!!", _sendLineNum++);
			return FALSE;
		}

		SendQ.RemoveData(retval);
		FileLog(SEND_LOG_FILENAME, "%4d\tremove size : %d", _sendLineNum++, retval);
	}

	FileLog(SEND_LOG_FILENAME, "%4d\tfree size : %d", _sendLineNum++, SendQ.GetFreeSize());
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
	default:
		FileLog(LOG_FILENAME, "%4d unknown pack type!!!!!!!!!!!!!!", g_lineNum++);
		break;
	}
}

void PacketProc_CreateMyCharacter(char * packet)
{
	st_PACKET_CHARACTER *st_player = (st_PACKET_CHARACTER *)packet;

	g_my_player = new PlayerObject();

	g_my_player->SetId(st_player->id);
	g_my_player->SetDriection(st_player->direction);
	g_my_player->SetPostion(st_player->x, st_player->y);
	g_my_player->SetHp(st_player->hp);
	g_my_player->SetObjectType(PLAYER);

	if (st_player->direction == PACKET_MOVE_DIR_LL)
	{
		g_my_player->SetSprite(PLAYER_STAND_L01, PLAYER_STAND_L_MAX, dfDELAY_STAND);
	}
	else
	{
		g_my_player->SetSprite(PLAYER_STAND_R01, PLAYER_STAND_R_MAX, dfDELAY_STAND);
	}

	g_list.push_back(g_my_player);
}

void PacketProc_CreateOtherCharacter(char * packet)
{
	st_PACKET_CHARACTER *st_player = (st_PACKET_CHARACTER *)packet;

	PlayerObject *player = new PlayerObject();

	player->SetId(st_player->id);
	player->SetHp(st_player->hp);
	player->SetDriection(st_player->direction);
	player->SetPostion(st_player->x, st_player->y);
	player->SetObjectType(ENEMY);

	if (st_player->direction == PACKET_MOVE_DIR_LL)
	{
		player->SetSprite(PLAYER_STAND_L01, PLAYER_STAND_L_MAX, dfDELAY_STAND);
	}
	else
	{
		player->SetSprite(PLAYER_STAND_R01, PLAYER_STAND_R_MAX, dfDELAY_STAND);
	}

	g_list.push_back(player);
}

void packetProc_DeleteCharacter(char * packet)
{
	st_PACKET_SC_DELETE *player = (st_PACKET_SC_DELETE *)packet;

	FileLog(LOG_FILENAME, "%4d Delete Character: %d", g_lineNum++, player->id);

	list<BaseObject *>::iterator iter;
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		if ((*iter)->GetObjectType() != EFFECT 
			&& (*iter)->GetId() == player->id)
		{
			delete (*iter);
			g_list.erase(iter);

			break;
		}
	}
}

void packetProc_MoveStart(char * packet)
{
	st_PACKET_SC_MOVE_START *fromServer = (st_PACKET_SC_MOVE_START *)packet;
	PlayerObject *player;

	FileLog(LOG_FILENAME, "%4d move start %d", g_lineNum++, fromServer->id);

	list<BaseObject *>::iterator iter;
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		if ((*iter)->GetId() == fromServer->id)
		{
			FileLog(LOG_FILENAME, "%4d\tFind!! : %d, %d, %d, %d", g_lineNum++,
				fromServer->id, fromServer->direction, fromServer->x, fromServer->y);

			player = (PlayerObject *)(*iter);

			player->ActionInput(fromServer->direction);
			player->Action();

			break;
		}
	}
}

void packetProc_MoveStop(char * packet)
{
	st_PACKET_SC_MOVE_STOP *fromServer= (st_PACKET_SC_MOVE_STOP *)packet;
	PlayerObject *player;

	FileLog(LOG_FILENAME, "%4d move stop %d", g_lineNum++, fromServer->id);

	list<BaseObject *>::iterator iter;
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		if ((*iter)->GetId() == fromServer->id)
		{
			FileLog(LOG_FILENAME, "%4d\tFind!! : %d, %d, %d, %d", g_lineNum++,
				fromServer->id, fromServer->direction, fromServer->x, fromServer->y);

			player = (PlayerObject *)(*iter);

			player->SetDriection(fromServer->direction);
			player->SetPostion(fromServer->x, fromServer->y);
			player->ActionInput(dfACTION_STAND);
			player->Action();

			break;
		}
	}
}

void packetProc_Attack1(char * packet)
{
	st_PACKET_SC_ATTACK *fromServer= (st_PACKET_SC_ATTACK *)packet;
	PlayerObject *player;

	FileLog(LOG_FILENAME, "%4d attack1", g_lineNum++);

	list<BaseObject *>::iterator iter;
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		if ((*iter)->GetId() == fromServer->id)
		{
			FileLog(LOG_FILENAME, "%4d\tFind!! %d, %d, %d, %d", g_lineNum++,
				fromServer->id, fromServer->direction, fromServer->x, fromServer->y);

			player = (PlayerObject *)(*iter);

			player->SetDriection(fromServer->direction);
			player->SetPostion(fromServer->x, fromServer->y);
			player->ActionInput(dfACTION_ATTACK1);
			player->Action();

			break;
		}
	}
}

void packetProc_Attack2(char * packet)
{
	st_PACKET_SC_ATTACK *fromServer= (st_PACKET_SC_ATTACK *)packet;
	PlayerObject *player;

	FileLog(LOG_FILENAME, "%4d attack2", g_lineNum++);

	list<BaseObject *>::iterator iter;
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		if ((*iter)->GetId() == fromServer->id)
		{
			FileLog(LOG_FILENAME, "%4d\tFind!! %d, %d, %d, %d", g_lineNum++,
				fromServer->id, fromServer->direction, fromServer->x, fromServer->y);

			player = (PlayerObject *)(*iter);

			player->SetDriection(fromServer->direction);
			player->SetPostion(fromServer->x, fromServer->y);
			player->ActionInput(dfACTION_ATTACK2);
			player->Action();

			break;
		}
	}
}

void packetProc_Attack3(char * packet)
{
	st_PACKET_SC_ATTACK *fromServer = (st_PACKET_SC_ATTACK *)packet;
	PlayerObject *player;

	FileLog(LOG_FILENAME, "%4d attack2", g_lineNum++);

	list<BaseObject *>::iterator iter;
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		if ((*iter)->GetId() == fromServer->id)
		{
			FileLog(LOG_FILENAME, "%4d\tFind!! %d, %d, %d, %d", g_lineNum++,
				fromServer->id, fromServer->direction, fromServer->x, fromServer->y);

			player = (PlayerObject *)(*iter);

			player->SetDriection(fromServer->direction);
			player->SetPostion(fromServer->x, fromServer->y);
			player->ActionInput(dfACTION_ATTACK3);
			player->Action();

			break;
		}
	}
}

void packetProc_Damage(char * packet)
{
	st_PACKET_SC_DAMAGE *fromServer= (st_PACKET_SC_DAMAGE *)packet;
	PlayerObject *player;
	int x, y;

	FileLog(LOG_FILENAME, "%4d damage", g_lineNum++);

	list<BaseObject *>::iterator iter;
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		if ((*iter)->GetId() == fromServer->toId)
		{
			FileLog(LOG_FILENAME, "%4d\tFind!! %d, %d, %d", g_lineNum++,
				fromServer->fromId, fromServer->toId, fromServer->hp);

			player = (PlayerObject *)(*iter);

			player->SetHp(fromServer->hp);

			x = player->GetCurX();
			y = player->GetCurX();

			g_effect->SetPostion(x, y + 1);
			g_effect->isEffectStart = TRUE;

			break;
		}
	}
}
