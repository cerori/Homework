#ifndef __ACCEPT__
#define __ACCEPT__

#include "Protocol.h"
#include "CSerializable.h"

void AcceptClient(void);
void AcceptLogin(CLIENT *client, st_PACKET_HEADER *header, CAyaPacket *packet);
void AcceptRoomList(CLIENT *client, st_PACKET_HEADER *header, CAyaPacket *packet);
void AcceptRoomCreate(CLIENT *client, st_PACKET_HEADER *header, CAyaPacket *packet);
void AcceptRoomEnter(CLIENT *client, st_PACKET_HEADER *header, CAyaPacket *packet);

#endif