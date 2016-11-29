#ifndef __PACKET__
#define __PACKET__

#include "Protocol.h"
#include <Windows.h>

void SendPacket(st_PACKET_HEADER *header, char *packet, int packetSize);
void MakePacket_MoveStart(st_PACKET_HEADER *header, st_PACKET_CS_MOVE_START *packet, BYTE direction, WORD x, WORD y);
void MakePacket_MoveStop(st_PACKET_HEADER *header, st_PACKET_CS_MOVE_STOP *packet, BYTE direction, WORD x, WORD y);
void MakePacket_Attack1(st_PACKET_HEADER *header, st_PACKET_CS_ATTACK *packet, BYTE direction, WORD x, WORD y);
void MakePacket_Attack2(st_PACKET_HEADER *header, st_PACKET_CS_ATTACK *packet, BYTE direction, WORD x, WORD y);
void MakePacket_Attack3(st_PACKET_HEADER *header, st_PACKET_CS_ATTACK *packet, BYTE direction, WORD x, WORD y);

#endif
