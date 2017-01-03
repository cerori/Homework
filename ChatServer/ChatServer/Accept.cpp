#include "stdafx.h"
#include "Protocol.h"
#include "CSerializable.h"
#include "Packet.h"
#include "Accept.h"

extern SOCKET g_sock;

extern SOCKET g_socket_list[dfMAX_CLIENT];
extern list<CLIENT *> g_client_list;
extern map<DWORD, ROOM *> g_room_list;

extern DWORD g_userSeq;
extern DWORD g_roomSeq;

void AcceptClient(void)
{
	SOCKADDR_IN clientaddr;
	int addrlen;

	SOCKET client_sock;

	addrlen = sizeof(clientaddr);
	client_sock = accept(g_sock, (SOCKADDR *)&clientaddr, &addrlen);
	if (client_sock == INVALID_SOCKET)
	{
		wprintf(L"accept error\n");
		return;
	}

	CLIENT *client = new CLIENT;

	client->sock = client_sock;
	client->enterRoomNo = 0;
	client->userNo = ++g_userSeq;

	// ����� �߰�
	g_client_list.push_back(client);
}

void AcceptLogin(CLIENT *client, st_PACKET_HEADER *header, CAyaPacket *packet)
{
	wcsncpy_s(client->nickName, (WCHAR *)packet->GetBufferPtr(), (int)header->wPayloadSize);
	
	// �г��� �ߺ� üũ
	list<CLIENT *>::iterator iter = g_client_list.begin();
	for (; iter != g_client_list.end(); iter++)
	{
		if ((*iter)->userNo == client->userNo)
			continue;

		if (wcscmp((*iter)->nickName, client->nickName) == 0)
		{
			MakePacket_Res_Login(client, header, df_RESULT_LOGIN_DNICK);
			return;
		}
	}

	MakePacket_Res_Login(client, header, df_RESULT_LOGIN_OK);
}

void AcceptRoomList(CLIENT * client, st_PACKET_HEADER * header, CAyaPacket * packet)
{
	MakePacket_Res_RoomList(client, header);
}

void AcceptRoomCreate(CLIENT * client, st_PACKET_HEADER * header, CAyaPacket * packet)
{
	WORD size = 0;
	ROOM *room = new ROOM;
	CAyaPacket pack;

	g_roomSeq++;

	// ������� �� ����
	packet->GetData((char *)&size, sizeof(size));

	// �� ��ȣ
	room->no = g_roomSeq;
	// �� �̸�
	wcsncpy_s(room->name, (WCHAR *)packet->GetBufferPtr(), size);
	// �� �߰�
	g_room_list.insert(map<DWORD, ROOM *>::value_type(g_roomSeq, room));

	// ���ӵ� Ŭ���̾�Ʈ �鿡�� ���� ������ �� ������ ����
	list<CLIENT *>::iterator iter = g_client_list.begin();
	for (; iter != g_client_list.end(); )
	{
		MakePacket_Res_Room(*iter, header, room);

		iter++;
	}
}

void AcceptRoomEnter(CLIENT * client, st_PACKET_HEADER * header, CAyaPacket * packet)
{
	DWORD roomNo;
	ROOM *room;

	*packet >> roomNo;

	map<DWORD, ROOM *>::iterator iter = g_room_list.find(roomNo);

	room = iter->second;
	room->users.push_back(client);

	MakePacket_Res_EnterRoom(client, header);
	

}
