#ifndef __PACKET__
#define __PACKET__

#include "Protocol.h"
#include <Windows.h>

void SendPacket(st_PACKET_HEADER *header, char *packet, int packetSize);
void MakePacket_MoveStart(st_PACKET_HEADER *header, st_PACKET_CS_MOVE_START *packet, BYTE direction, DWORD x, DWORD y);
void MakePacket_MoveStop(st_PACKET_HEADER *header, st_PACKET_CS_MOVE_STOP *packet, BYTE direction, DWORD x, DWORD y);

#endif
