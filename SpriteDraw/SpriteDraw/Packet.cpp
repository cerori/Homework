#include "stdafx.h"
#include "Packet.h"
#include "Network.h"
#include "RingBuffer.h"
#include "FileLog.h"
#include "PlayerObject.h"

extern PlayerObject *g_my_player;
extern BOOL g_successConnect;
extern BOOL g_isCanSendPacket;
extern RingBuffer SendQ;

void SendPacket(st_PACKET_HEADER * header, char * packet, int packetSize)
{
	char end = (char)NETWORK_PACKET_END;

	if (g_successConnect != TRUE)
		return;

	FileLog(LOG_FILENAME, "send packet type: %d, %d, %d", header->type, g_my_player->GetCurX(), g_my_player->GetCurY());

	SendQ.Enqueue((char *)header, sizeof(st_PACKET_HEADER));
	SendQ.Enqueue((char *)packet, packetSize);
	SendQ.Enqueue(&end, sizeof(NETWORK_PACKET_END));

	SendEvent();
}

void MakePacket_MoveStart(st_PACKET_HEADER *header, st_PACKET_CS_MOVE_START *packet, BYTE direction, WORD x, WORD y)
{
	header->code = PACKET_CODE;
	header->type = PACKET_CS_MOVE_START;
	header->size = sizeof(st_PACKET_CS_MOVE_START);

	packet->direction = direction;
	packet->x = x;
	packet->y = y;
}

void MakePacket_MoveStop(st_PACKET_HEADER *header, st_PACKET_CS_MOVE_STOP *packet, BYTE direction, WORD x, WORD y)
{
	header->code = PACKET_CODE;
	header->type = PACKET_CS_MOVE_STOP;
	header->size = sizeof(st_PACKET_CS_MOVE_STOP);

	packet->direction = direction;
	packet->x = x;
	packet->y = y;
}

void MakePacket_Attack1(st_PACKET_HEADER * header, st_PACKET_CS_ATTACK * packet, BYTE direction, WORD x, WORD y)
{
	header->code = PACKET_CODE;
	header->type = PACKET_CS_ATTACK1;
	header->size = sizeof(st_PACKET_CS_ATTACK);

	packet->direction = direction;
	packet->x = x;
	packet->y = y;
}

void MakePacket_Attack2(st_PACKET_HEADER * header, st_PACKET_CS_ATTACK * packet, BYTE direction, WORD x, WORD y)
{
	header->code = PACKET_CODE;
	header->type = PACKET_CS_ATTACK2;
	header->size = sizeof(st_PACKET_CS_ATTACK);

	packet->direction = direction;
	packet->x = x;
	packet->y = y;
}

void MakePacket_Attack3(st_PACKET_HEADER * header, st_PACKET_CS_ATTACK * packet, BYTE direction, WORD x, WORD y)
{
	header->code = PACKET_CODE;
	header->type = PACKET_CS_ATTACK3;
	header->size = sizeof(st_PACKET_CS_ATTACK);

	packet->direction = direction;
	packet->x = x;
	packet->y = y;
}

