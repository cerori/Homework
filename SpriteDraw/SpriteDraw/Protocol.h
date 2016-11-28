#ifndef __PROTOCOL__
#define __PROTOCOL__

#include <Windows.h>

#define WM_USER_SOCKET (WM_USER + 1)
#define NETWORK_PORT	5000

#define BUFFER_SIZE	1000
#define SEND_BUFFER_SIZE	BUFFER_SIZE
#define RECV_BUFFER_SIZE	BUFFER_SIZE


#pragma pack(1)
struct st_PACKET_HEADER
{
	BYTE code;
	BYTE size;
	BYTE type;
	BYTE temp;
};

struct st_PACKET_CHARACTER
{
	DWORD id;
	BYTE direction;
	WORD x;
	WORD y;
	BYTE hp;
};

struct st_PACKET_SC_DELETE
{
	DWORD id;
};

// START, STOP °â¿ë
typedef struct st_PACKET_CS_MOVE
{
	BYTE direction;
	WORD x;
	WORD y;
} st_PACKET_CS_MOVE_START, st_PACKET_CS_MOVE_STOP;

// START, STOP °â¿ë
typedef struct st_PACKET_SC_MOVE
{
	DWORD id;
	BYTE direction;
	WORD x;
	WORD y;
} st_PACKET_SC_MOVE_START, st_PACKET_SC_MOVE_STOP;

struct st_PACKET_CS_ATTACK
{
	BYTE direction;
	WORD x;
	WORD y;
};

struct st_PACKET_SC_ATTACK
{
	DWORD id;
	BYTE direction;
	WORD x;
	WORD y;
};

struct st_PACKET_SC_DAMAGE
{
	DWORD fromId;
	DWORD toId;
	BYTE hp;
};
#pragma pack()

#define PACKET_CODE	0x89
#define NETWORK_PACKET_END ((BYTE)0x79)

#define PACKET_SC_CREATE_MY_CHARACTER		0
#define PACKET_SC_CREATE_OTHER_CHARACTER	1
#define PACKET_SC_DELETE_CHARACTER			2
#define PACKET_CS_MOVE_START				10
#define PACKET_SC_MOVE_START				11
#define PACKET_CS_MOVE_STOP					12
#define PACKET_SC_MOVE_STOP					13
#define PACKET_CS_ATTACK1	20
#define PACKET_SC_ATTACK1	21
#define PACKET_CS_ATTACK2	22
#define PACKET_SC_ATTACK2	23
#define PACKET_CS_ATTACK3	24
#define PACKET_SC_ATTACK3	25
#define PACKET_SC_DAMAGE	30

#define PACKET_MOVE_DIR_LL	0
#define PACKET_MOVE_DIR_LU	1
#define PACKET_MOVE_DIR_UU	2
#define PACKET_MOVE_DIR_RU	3
#define PACKET_MOVE_DIR_RR	4
#define PACKET_MOVE_DIR_RD	5
#define PACKET_MOVE_DIR_DD	6
#define PACKET_MOVE_DIR_LD	7

#endif
