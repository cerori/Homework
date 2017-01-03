#ifndef __LIB__
#define __LIB__

#include <Windows.h>
#include "Protocol.h"
#include "CSerializable.h"

BOOL Checksum(st_PACKET_HEADER *header, CAyaPacket *pack);
BYTE MakeChecksum(WORD msgType, char *data, int size);


#endif