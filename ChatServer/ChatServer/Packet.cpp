#include "stdafx.h"
#include "Protocol.h"
#include "CSerializable.h"
#include "lib.h"
#include "Packet.h"

extern map<DWORD, ROOM *> g_room_list;

void MakePacket_Res_Login(CLIENT *client, st_PACKET_HEADER *header, BYTE result)
{
	CAyaPacket pack;

	pack << (BYTE)result;
	pack << (DWORD)client->userNo;

	header->byCode = dfPACKET_CODE;
	header->wMsgType = df_RES_LOGIN;
	header->wPayloadSize = pack.GetDataSize();
	header->byCheckSum = MakeChecksum(df_RES_LOGIN, pack.GetBufferPtr(), pack.GetDataSize());

	client->SendQ.Enqueue((char *)header, sizeof(st_PACKET_HEADER));
	client->SendQ.Enqueue(pack.GetBufferPtr(), pack.GetDataSize());
}

void MakePacket_Res_RoomList(CLIENT *client, st_PACKET_HEADER *header)
{
	ROOM *room;
	CAyaPacket pack;
	WCHAR nick[dfNICK_MAX_LEN];

	pack << (WORD)g_room_list.size();

	map<DWORD, ROOM *>::iterator iter = g_room_list.begin();
	for (; iter != g_room_list.end(); iter++)
	{
		room = iter->second;

		pack << (DWORD)room->no;
		pack << (WORD)(wcslen(room->name) * sizeof(WCHAR));
		pack.PutData((char *)&room->name, wcslen(room->name) * sizeof(WCHAR));
		pack << (BYTE)room->users.size();

		list<CLIENT *>::iterator iter_user = room->users.begin();
		for ( ; iter_user != room->users.end(); iter_user++)
		{
			ZeroMemory(nick, sizeof(nick));

			wcsncpy_s(nick, (WCHAR *)(*iter_user)->nickName, sizeof(nick));
			pack.PutData((char *)nick, dfNICK_MAX_LEN);
		}
	}

	header->byCode = dfPACKET_CODE;
	header->wMsgType = df_RES_ROOM_LIST;
	header->wPayloadSize = pack.GetDataSize();
	header->byCheckSum = MakeChecksum(df_RES_ROOM_LIST, pack.GetBufferPtr(), pack.GetDataSize());

	client->SendQ.Enqueue((char *)header, sizeof(st_PACKET_HEADER));
	client->SendQ.Enqueue(pack.GetBufferPtr(), pack.GetDataSize());
}

void MakePacket_Res_EnterRoom(CLIENT * client, st_PACKET_HEADER * header, ROOM *room, BYTE result)
{
	CAyaPacket pack;
	char *p;

	p = pack.GetBufferPtr();

	pack << (BYTE)result;

	if (room != NULL)
	{
		// 방 번호
		pack << (DWORD)room->no;
		// 방 이름 길이
		pack << (WORD)(wcslen(room->name) * sizeof(WCHAR));
		// 방 이름
		pack.PutData((char *)room->name, (wcslen(room->name) * sizeof(WCHAR)));
		// 방에 들어있는 사람 수
		pack << (BYTE)room->users.size();

		list<CLIENT *>::iterator iter_user = room->users.begin();
		for (; iter_user != room->users.end(); iter_user++)
		{
			pack.PutData((char *)(*iter_user)->nickName, sizeof((*iter_user)->nickName));
			pack << (*iter_user)->userNo;
		}
	}

	header->byCode = dfPACKET_CODE;
	header->wMsgType = df_RES_ROOM_ENTER;
	header->wPayloadSize = pack.GetDataSize();
	header->byCheckSum = MakeChecksum(df_RES_ROOM_ENTER, pack.GetBufferPtr(), pack.GetDataSize());

	client->SendQ.Enqueue((char *)header, sizeof(st_PACKET_HEADER));
	client->SendQ.Enqueue(pack.GetBufferPtr(), pack.GetDataSize());
}

void MakePacket_Res_CreateRoom(CLIENT * client, st_PACKET_HEADER * header, ROOM * room, BYTE result)
{
	CAyaPacket pack;

	pack << (BYTE)result;

	pack << (DWORD)room->no;
	pack << (WORD)(wcslen(room->name) * sizeof(WCHAR));
	pack.PutData((char *)room->name, (wcslen(room->name) * sizeof(WCHAR)));

	header->byCode = dfPACKET_CODE;
	header->wMsgType = df_RES_ROOM_CREATE;
	header->wPayloadSize = pack.GetDataSize();
	header->byCheckSum = MakeChecksum(df_RES_ROOM_CREATE, pack.GetBufferPtr(), pack.GetDataSize());

	client->SendQ.Enqueue((char *)header, sizeof(st_PACKET_HEADER));
	client->SendQ.Enqueue(pack.GetBufferPtr(), pack.GetDataSize());
}

void MakePacket_Res_Room(CLIENT *client, st_PACKET_HEADER *header, ROOM * room)
{
	CAyaPacket pack;
	WCHAR nick[dfNICK_MAX_LEN];

	pack << (WORD)1;

	pack << (DWORD)room->no;
	pack << (WORD)(wcslen(room->name) * sizeof(WCHAR));
	pack.PutData((char *)&room->name, wcslen(room->name) * sizeof(WCHAR));
	pack << (BYTE)room->users.size();

	list<CLIENT *>::iterator iter_user = room->users.begin();
	for ( ; iter_user != room->users.end(); iter_user++)
	{
		ZeroMemory(nick, sizeof(nick));

		wcsncpy_s(nick, (WCHAR *)(*iter_user)->nickName, sizeof(nick));
		pack.PutData((char *)nick, dfNICK_MAX_LEN);
	}

	header->byCode = dfPACKET_CODE;
	header->wMsgType = df_RES_ROOM_LIST;
	header->wPayloadSize = pack.GetDataSize();
	header->byCheckSum = MakeChecksum(df_RES_ROOM_LIST, pack.GetBufferPtr(), pack.GetDataSize());

	client->SendQ.Enqueue((char *)header, sizeof(st_PACKET_HEADER));
	client->SendQ.Enqueue(pack.GetBufferPtr(), pack.GetDataSize());
}
