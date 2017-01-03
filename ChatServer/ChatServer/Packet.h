#ifndef __PACKET__
#define __PACKET__

#include <Windows.h>
#include "Protocol.h"

void MakePacket_Res_Login(CLIENT *client, st_PACKET_HEADER *header, BYTE result);
void MakePacket_Res_RoomList(CLIENT *client, st_PACKET_HEADER *header);
void MakePacket_Res_EnterRoom(CLIENT *client, st_PACKET_HEADER *header, ROOM *room, BYTE result);
void MakePacket_Res_CreateRoom(CLIENT *client, st_PACKET_HEADER *header, ROOM *room, BYTE result);
void MakePacket_Chat(st_PACKET_HEADER *header, BYTE msgType, BYTE userNo);
void MakePacket_LeaveRoom(st_PACKET_HEADER *header, BYTE msgType, BYTE userNo);
void MakePacket_DeleteRoom(st_PACKET_HEADER *header, BYTE msgType, BYTE userNo);
void MakePacket_EnterRoomOther(st_PACKET_HEADER *header, BYTE msgType, BYTE userNo);

void MakePacket_Res_Room(CLIENT *client, st_PACKET_HEADER *header, ROOM *room);

#endif