// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
#include <windowsx.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <list>

using namespace std;

// 로그
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#define dfACTION_MOVE_LL	0
#define dfACTION_MOVE_LU	1
#define dfACTION_MOVE_UU	2
#define dfACTION_MOVE_RU	3
#define dfACTION_MOVE_RR	4
#define dfACTION_MOVE_RD	5
#define dfACTION_MOVE_DD	6
#define dfACTION_MOVE_LD	7

#define dfACTION_ATTACK1	8
#define dfACTION_ATTACK2	9
#define dfACTION_ATTACK3	10

#define dfACTION_PUSH		11
#define dfACTION_STAND		12
#define dfACTION_MOVE		13

#define dfDELAY_STAND	5
#define dfDELAY_MOVE	4
#define dfDELAY_ATTACK1	3
#define dfDELAY_ATTACK2	4
#define dfDELAY_ATTACK3	4
#define dfDELAY_EFFECT	3

#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470

enum SPRITE_LIST
{
	MAP,
	PLAYER_STAND_L01,
	PLAYER_STAND_L02,
	PLAYER_STAND_L03,
	PLAYER_STAND_L_MAX = PLAYER_STAND_L03,
	PLAYER_STAND_R01,
	PLAYER_STAND_R02,
	PLAYER_STAND_R03,
	PLAYER_STAND_R_MAX = PLAYER_STAND_R03,

	PLAYER_MOVE_L01,
	PLAYER_MOVE_L02,
	PLAYER_MOVE_L03,
	PLAYER_MOVE_L04,
	PLAYER_MOVE_L05,
	PLAYER_MOVE_L06,
	PLAYER_MOVE_L07,
	PLAYER_MOVE_L08,
	PLAYER_MOVE_L09,
	PLAYER_MOVE_L10,
	PLAYER_MOVE_L11,
	PLAYER_MOVE_L12,
	PLAYER_MOVE_L_MAX = PLAYER_MOVE_L12,
	PLAYER_MOVE_R01,
	PLAYER_MOVE_R02,
	PLAYER_MOVE_R03,
	PLAYER_MOVE_R04,
	PLAYER_MOVE_R05,
	PLAYER_MOVE_R06,
	PLAYER_MOVE_R07,
	PLAYER_MOVE_R08,
	PLAYER_MOVE_R09,
	PLAYER_MOVE_R10,
	PLAYER_MOVE_R11,
	PLAYER_MOVE_R12,
	PLAYER_MOVE_R_MAX = PLAYER_MOVE_R12,

	PLAYER_ATTACK1_L01,
	PLAYER_ATTACK1_L02,
	PLAYER_ATTACK1_L04,
	PLAYER_ATTACK1_L_MAX = PLAYER_ATTACK1_L04,
	PLAYER_ATTACK1_R01,
	PLAYER_ATTACK1_R02,
	PLAYER_ATTACK1_R03,
	PLAYER_ATTACK1_R04,
	PLAYER_ATTACK1_R_MAX = PLAYER_ATTACK1_R04,

	PLAYER_ATTACK2_L01,
	PLAYER_ATTACK2_L02,
	PLAYER_ATTACK2_L03,
	PLAYER_ATTACK2_L04,
	PLAYER_ATTACK2_L_MAX = PLAYER_ATTACK2_L04,
	PLAYER_ATTACK2_R01,
	PLAYER_ATTACK2_R02,
	PLAYER_ATTACK2_R03,
	PLAYER_ATTACK2_R04,
	PLAYER_ATTACK2_R_MAX = PLAYER_ATTACK2_R04,

	PLAYER_ATTACK3_L01,
	PLAYER_ATTACK3_L02,
	PLAYER_ATTACK3_L03,
	PLAYER_ATTACK3_L04,
	PLAYER_ATTACK3_L05,
	PLAYER_ATTACK3_L06,
	PLAYER_ATTACK3_L_MAX = PLAYER_ATTACK3_L06,
	PLAYER_ATTACK3_R01,
	PLAYER_ATTACK3_R02,
	PLAYER_ATTACK3_R03,
	PLAYER_ATTACK3_R04,
	PLAYER_ATTACK3_R05,
	PLAYER_ATTACK3_R06,
	PLAYER_ATTACK3_R_MAX = PLAYER_ATTACK3_R06,

	eGUAGE_HP,
	eSHADOW
};

enum OBJECT_TYPE
{
	PLAYER,
	ENEMY,
	EFFECT
};

enum e_DIRECTION
{
	e_LEFT,
	e_RIGHT
};

enum e_KEY_MAP
{
	e_C = 0x43,
	e_X = 0x58,
	e_Z = 0x5A
};

