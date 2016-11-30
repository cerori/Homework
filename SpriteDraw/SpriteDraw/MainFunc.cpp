#include "stdafx.h"
#include "MainFunc.h"
#include "Packet.h"
#include "BaseObject.h"
#include "PlayerObject.h"
#include "EffectObject.h"
#include "ScreenDib.h"
#include "SpriteDib.h"
#include "KeyMgr.h"

extern HWND g_hWnd;
extern SOCKET g_sock;
extern WCHAR g_ip[];
extern BOOL g_isCanSendPacket;
extern BOOL g_successConnect;
extern BOOL g_beforeAction;
extern BOOL g_currentMoveDirection;
extern BOOL g_isMoveChange;
extern BOOL g_isActiveApp;
extern cKey *g_keyMgr;

extern DWORD g_lineNum;

extern list<BaseObject *> g_list;

extern PlayerObject *g_my_player;
extern EffectObject *g_effect;

// 윈도우 활성화 체크

extern ScreenDib g_ScreenDib;
extern SpriteDib g_SpriteDib;

/*
사용자 함수
*/

void InitialGame(void)
{
	g_effect = new EffectObject();

	g_list.push_back(g_effect);

	//// 사용자
	//g_player = new PlayerObject();
	//g_player->SetHp(50);
	//g_player->SetPostion(100, 200);
	//g_player->SetObjectType(PLAYER);
	//g_player->SetSprite(PLAYER_STAND_R01, PLAYER_STAND_R_MAX, dfDELAY_STAND);

	//g_list.push_back(g_player);

	//// 적1
	//PlayerObject *g_player1 = new PlayerObject();
	//g_player1->SetObjectType(ENEMY);
	//g_player1->SetPostion(200, 200);
	//g_player1->SetSprite(PLAYER_STAND_L01, PLAYER_STAND_L_MAX, dfDELAY_STAND);

	//g_list.push_back(g_player1);

	//// 적2
	//PlayerObject *g_player2 = new PlayerObject();
	//g_player2->SetObjectType(ENEMY);
	//g_player2->SetPostion(300, 200);
	//g_player2->SetSprite(PLAYER_STAND_R01, PLAYER_STAND_R_MAX, dfDELAY_STAND);

	//g_list.push_back(g_player2);
}

void ContentLoad(void)
{
	g_SpriteDib.LoadDibSprite(0, L"SpriteData/_Map.bmp", 640, 480);
	g_SpriteDib.LoadDibSprite(1, L"SpriteData/Stand_L_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(2, L"SpriteData/Stand_L_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(3, L"SpriteData/Stand_L_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(4, L"SpriteData/Stand_R_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(5, L"SpriteData/Stand_R_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(6, L"SpriteData/Stand_R_03.bmp", 71, 90);

	// move
	g_SpriteDib.LoadDibSprite(7, L"SpriteData/Move_L_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(8, L"SpriteData/Move_L_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(9, L"SpriteData/Move_L_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(10, L"SpriteData/Move_L_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(11, L"SpriteData/Move_L_05.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(12, L"SpriteData/Move_L_06.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(13, L"SpriteData/Move_L_07.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(14, L"SpriteData/Move_L_08.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(15, L"SpriteData/Move_L_09.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(16, L"SpriteData/Move_L_10.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(17, L"SpriteData/Move_L_11.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(18, L"SpriteData/Move_L_12.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(19, L"SpriteData/Move_R_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(20, L"SpriteData/Move_R_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(21, L"SpriteData/Move_R_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(22, L"SpriteData/Move_R_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(23, L"SpriteData/Move_R_05.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(24, L"SpriteData/Move_R_06.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(25, L"SpriteData/Move_R_07.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(26, L"SpriteData/Move_R_08.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(27, L"SpriteData/Move_R_09.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(28, L"SpriteData/Move_R_10.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(29, L"SpriteData/Move_R_11.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(30, L"SpriteData/Move_R_12.bmp", 71, 90);

	// action 1
	g_SpriteDib.LoadDibSprite(31, L"SpriteData/Attack1_L_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(32, L"SpriteData/Attack1_L_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(33, L"SpriteData/Attack1_L_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(34, L"SpriteData/Attack1_R_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(35, L"SpriteData/Attack1_R_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(36, L"SpriteData/Attack1_R_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(37, L"SpriteData/Attack1_R_04.bmp", 71, 90);

	// attack 2
	g_SpriteDib.LoadDibSprite(38, L"SpriteData/Attack2_L_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(39, L"SpriteData/Attack2_L_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(40, L"SpriteData/Attack2_L_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(41, L"SpriteData/Attack2_L_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(42, L"SpriteData/Attack2_R_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(43, L"SpriteData/Attack2_R_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(44, L"SpriteData/Attack2_R_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(45, L"SpriteData/Attack2_R_04.bmp", 71, 90);

	// attack 3
	g_SpriteDib.LoadDibSprite(46, L"SpriteData/Attack3_L_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(47, L"SpriteData/Attack3_L_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(48, L"SpriteData/Attack3_L_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(49, L"SpriteData/Attack3_L_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(50, L"SpriteData/Attack3_L_05.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(51, L"SpriteData/Attack3_L_06.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(52, L"SpriteData/Attack3_R_01.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(53, L"SpriteData/Attack3_R_02.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(54, L"SpriteData/Attack3_R_03.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(55, L"SpriteData/Attack3_R_04.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(56, L"SpriteData/Attack3_R_05.bmp", 71, 90);
	g_SpriteDib.LoadDibSprite(57, L"SpriteData/Attack3_R_06.bmp", 71, 90);

	// 타격 효과
	g_SpriteDib.LoadDibSprite(58, L"SpriteData/xSpark_1.bmp", 70, 70);
	g_SpriteDib.LoadDibSprite(59, L"SpriteData/xSpark_2.bmp", 70, 70);
	g_SpriteDib.LoadDibSprite(60, L"SpriteData/xSpark_3.bmp", 70, 70);
	g_SpriteDib.LoadDibSprite(61, L"SpriteData/xSpark_4.bmp", 70, 70);

	g_SpriteDib.LoadDibSprite(62, L"SpriteData/HPGuage.bmp", 0, 0);
	g_SpriteDib.LoadDibSprite(63, L"SpriteData/Shadow.bmp", 32, 4);
}

void Update(void)
{
	if (g_isActiveApp)
		KeyProcess();

	Action();

	// Frame Skip 삽입 위치

	Draw();

	Sleep(20);
}

/*
1. 이동 중에 공격이 입력되면 멈추고 공격을 한다.
*/
void KeyProcess(void)
{
	st_PACKET_HEADER header;
	char packet[20];
	int packetSize = 0;
	DWORD action = dfACTION_STAND;

	if (g_my_player == NULL)
		return;

	if (g_keyMgr->StayKeyDown(VK_UP))
	{
		if (g_beforeAction != dfACTION_MOVE_UU)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_MoveStart(&header, (st_PACKET_CS_MOVE_START *)packet, dfACTION_MOVE_UU, g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_MOVE_UU;
	}

	if (g_keyMgr->StayKeyDown(VK_DOWN))
	{
		if (g_beforeAction != dfACTION_MOVE_DD)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_MoveStart(&header, (st_PACKET_CS_MOVE_START *)packet, dfACTION_MOVE_DD, g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_MOVE_DD;
	}

	if (g_keyMgr->StayKeyDown(VK_LEFT))
	{
		if (g_beforeAction != dfACTION_MOVE_LL)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_MoveStart(&header, (st_PACKET_CS_MOVE_START *)packet, dfACTION_MOVE_LL, g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_MOVE_LL;
	}

	if (g_keyMgr->StayKeyDown(VK_RIGHT))
	{
		if (g_beforeAction != dfACTION_MOVE_RR)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_MoveStart(&header, (st_PACKET_CS_MOVE_START *)packet, dfACTION_MOVE_RR, g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_MOVE_RR;
	}

	//대각선
	if (g_keyMgr->StayKeyDown(VK_LEFT) && g_keyMgr->StayKeyDown(VK_UP))
	{
		if (g_beforeAction != dfACTION_MOVE_LU)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_MoveStart(&header, (st_PACKET_CS_MOVE_START *)packet, dfACTION_MOVE_LU, g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_MOVE_LU;
	}

	if (g_keyMgr->StayKeyDown(VK_LEFT) && g_keyMgr->StayKeyDown(VK_DOWN))
	{
		if (g_beforeAction != dfACTION_MOVE_LD)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_MoveStart(&header, (st_PACKET_CS_MOVE_START *)packet, dfACTION_MOVE_LD, g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_MOVE_LD;
	}

	if (g_keyMgr->StayKeyDown(VK_RIGHT) && g_keyMgr->StayKeyDown(VK_UP))
	{
		if (g_beforeAction != dfACTION_MOVE_RU)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_MoveStart(&header, (st_PACKET_CS_MOVE_START *)packet, dfACTION_MOVE_RU, g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_MOVE_RU;
	}

	if (g_keyMgr->StayKeyDown(VK_RIGHT) && g_keyMgr->StayKeyDown(VK_DOWN))
	{
		if (g_beforeAction != dfACTION_MOVE_RD)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_MoveStart(&header, (st_PACKET_CS_MOVE_START *)packet, dfACTION_MOVE_RD, g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_MOVE_RD;
	}

	// 공격1
	if (g_keyMgr->OnceKeyDown(e_Z))
	{
		if (g_beforeAction != dfACTION_ATTACK1)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_Attack1(&header, (st_PACKET_CS_ATTACK *)packet, g_my_player->GetDirection(), g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_ATTACK1;
	}

	// 공격2
	if (g_keyMgr->OnceKeyDown(e_X))
	{
		if (g_beforeAction != dfACTION_ATTACK2)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_Attack2(&header, (st_PACKET_CS_ATTACK *)packet, g_my_player->GetDirection(), g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_ATTACK2;
	}

	// 공격3
	if (g_keyMgr->OnceKeyDown(e_C))
	{
		if (g_beforeAction != dfACTION_ATTACK3)
		{
			packetSize = sizeof(st_PACKET_CS_MOVE_START);
			MakePacket_Attack3(&header, (st_PACKET_CS_ATTACK *)packet, g_my_player->GetDirection(), g_my_player->GetCurX(), g_my_player->GetCurY());
		}

		action = dfACTION_ATTACK3;
	}

	// 기본 동작
	if (action == dfACTION_STAND)
	{
		packetSize = sizeof(st_PACKET_CS_MOVE_STOP);
		MakePacket_MoveStop(&header, (st_PACKET_CS_MOVE_STOP *)packet, g_my_player->GetDirection(), g_my_player->GetCurX(), g_my_player->GetCurY());
	}

	// 케릭터 행동
	g_my_player->ActionInput(action);

	// 패킷 전송
	if (g_beforeAction != action)
		SendPacket(&header, (char *)&packet, packetSize);

	g_beforeAction = action;
}

void Action(void)
{
	list<BaseObject *>::iterator iter;

	// 리스트의 모든 객체를 돌며 액션을 한다.
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		(*iter)->Action();
	}
}

void Draw(void)
{
	BYTE *pDest = g_ScreenDib.GetDibBuffer();
	int destWidth = g_ScreenDib.GetWidth();
	int destHeight = g_ScreenDib.GetHeight();
	int destPitch = g_ScreenDib.GetPitch();

	g_SpriteDib.DrawImage(0, 0, 0, pDest, 640, 480, 640 * 4);

	g_list.sort(compare_axle_y);

	list<BaseObject *>::iterator iter;

	// 리스트의 모든 객체를 돌며 액션을 한다.
	for (iter = g_list.begin(); iter != g_list.end(); iter++)
	{
		(*iter)->Draw(pDest, 640, 480, 640 * 4);
	}

	g_ScreenDib.DrawBuffer(g_hWnd);
}

bool compare_axle_y(BaseObject *first, BaseObject *second)
{
	if (second->GetObjectType() == EFFECT)
		return true;

	return (first->GetCurY() < second->GetCurY());
}

void err_quit(WCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(WCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	wprintf(L"[%s] %s", msg, (LPWSTR)&lpMsgBuf);
	LocalFree(lpMsgBuf);
}
