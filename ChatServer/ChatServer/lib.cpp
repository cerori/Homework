#include "stdafx.h"
#include "lib.h"

BOOL Checksum(st_PACKET_HEADER *header, CAyaPacket *pack)
{
	UINT sum = 0;
	int i;
	char *buff;

	sum += header->wMsgType;
	buff = pack->GetBufferPtr();

	for (i = 0; i < pack->GetDataSize(); i++)
	{
		sum += (BYTE)*(buff + i);
	}

	if (sum % 256 == header->byCheckSum)
		return 1;
	else
		return 0;
}

BYTE MakeChecksum(WORD msgType, char * data, int dataSize)
{
	UINT sum = 0;
	int i;

	sum += msgType;

	for (i = 0; i < dataSize; i++)
	{
		sum += (BYTE)*(data + i);
	}

	return sum % 256;
}
