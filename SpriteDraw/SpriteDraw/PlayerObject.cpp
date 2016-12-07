#include "stdafx.h"
#include "PlayerObject.h"
#include "SpriteDib.h"

extern SpriteDib g_SpriteDib;

PlayerObject::PlayerObject()
{
	_hp = 100;
	_direction = e_RIGHT;
	_actionCur = NULL;
	_actionPrev = NULL;
}

void PlayerObject::Action(void)
{
	NextFrame();
	ActionProc();

	_actionPrev = _actionInput;
}

void PlayerObject::Draw(BYTE *pDest, int destWidth, int destHeight, int destPitch)
{
	g_SpriteDib.DrawSprite(eGUAGE_HP, GetCurX() - 35, GetCurY() + 9, pDest, destWidth, destHeight, destPitch, GetHp());

	g_SpriteDib.DrawSprite(eSHADOW, GetCurX(), GetCurY(), pDest, destWidth, destHeight, destPitch);

	g_SpriteDib.DrawSprite(GetSpriteNow(), GetCurX(), GetCurY(), pDest, destWidth, destHeight, destPitch);
}

void PlayerObject::ActionProc(void)
{
	switch (_actionInput)
	{
	case dfACTION_ATTACK1:
		if (IsEndFrame())
		{
			_isEndFrame = FALSE;
			ActionInput(dfACTION_STAND);
			SetActionStand();
		}

		if (_actionCur != _actionInput && !IsEndFrame())
			SetActionAttack1();
		break;

	case dfACTION_ATTACK2:
		if (IsEndFrame())
		{
			_isEndFrame = FALSE;
			ActionInput(dfACTION_STAND);
			SetActionStand();
		}

		if (_actionCur != _actionInput && !IsEndFrame())
			SetActionAttack2();
		break;

	case dfACTION_ATTACK3:
		if (IsEndFrame())
		{
			_isEndFrame = FALSE;
			ActionInput(dfACTION_STAND);
			SetActionStand();
		}
		if (_actionCur != _actionInput && !IsEndFrame())
			SetActionAttack3();
		break;
	default:
		InputActionProc();

	}

	//if (_actionPrev = _actionInput && !IsEndFrame())
	//{
	//	_actionInput = _actionPrev;
	//	return;
	//}

}

void PlayerObject::InputActionProc(void)
{
	int x, y;

	//if (!IsPlayer())
	//	return;

	x = GetCurX();
	y = GetCurY();

	switch(_actionInput)
	{
	case dfACTION_MOVE_UU:
		if (y >= dfRANGE_MOVE_TOP)
			y -= dfSPEED_PLAYER_Y;
		
		SetPostion(x, y);
		if (IsEndFrame())
		{
			SetActionMove();
		}
		break;
	case dfACTION_MOVE_DD:
		if (y <= dfRANGE_MOVE_BOTTOM)
			y += dfSPEED_PLAYER_Y;

		SetPostion(x, y);
		if (IsEndFrame())
		{
			SetActionMove();
		}
		break;
	case dfACTION_MOVE_LL:
		if (GetCurX() >= dfRANGE_MOVE_LEFT)
			x -= dfSPEED_PLAYER_X;

		_direction = e_LEFT;
		SetPostion(x, y);
		if (IsEndFrame())
		{
			SetActionMove();
		}
		break;
	case dfACTION_MOVE_RR:
		if (GetCurX() <= dfRANGE_MOVE_RIGHT)
			x += dfSPEED_PLAYER_X;

		_direction = e_RIGHT;

		SetPostion(x, y);
		if (IsEndFrame())
		{
			SetActionMove();
		}
		break;

	// ´ë°¢¼±
	case dfACTION_MOVE_LU:
		if (GetCurX() >= dfRANGE_MOVE_LEFT)
			x -= dfSPEED_PLAYER_X;

		if (GetCurY() >= dfRANGE_MOVE_TOP)
			y -= dfSPEED_PLAYER_Y;

		_direction = e_LEFT;
		SetPostion(x, y);
		if (IsEndFrame())
		{
			SetActionMove();
		}
		break;

	case dfACTION_MOVE_LD:
		if (GetCurX() >= dfRANGE_MOVE_LEFT)
			x -= dfSPEED_PLAYER_X;

		if (GetCurY() <= dfRANGE_MOVE_BOTTOM)
			y += dfSPEED_PLAYER_Y;

		_direction = e_LEFT;

		SetPostion(x, y);
		if (IsEndFrame())
		{
			SetActionMove();
		}
		break;

	case dfACTION_MOVE_RU:
		if (GetCurX() <= dfRANGE_MOVE_RIGHT)
			x += dfSPEED_PLAYER_X;

		if (GetCurY() >= dfRANGE_MOVE_TOP)
			y -= dfSPEED_PLAYER_Y;

		_direction = e_RIGHT;

		SetPostion(x, y);
		if (IsEndFrame())
		{
			SetActionMove();
		}
		break;

	case dfACTION_MOVE_RD:
		if (GetCurX() <= dfRANGE_MOVE_RIGHT)
			x += dfSPEED_PLAYER_X;

		if (GetCurY() <= dfRANGE_MOVE_BOTTOM)
			y += dfSPEED_PLAYER_Y;

		_direction = e_RIGHT;
		SetPostion(x, y);
		if (IsEndFrame())
		{
			SetActionMove();
		}
		break;
	}

	if (IsEndFrame())
	{
		SetActionStand();
	}

}

BOOL PlayerObject::IsPlayer(void)
{
	if (_objectType == PLAYER)
		return TRUE;

	return FALSE;
}

int PlayerObject::GetHp(void)
{
	return _hp;
}

BYTE PlayerObject::GetDirection(void)
{
	return _direction;
}

void PlayerObject::SetHp(int hp)
{
	_hp = hp;
}

void PlayerObject::SetDriection(BYTE direction)
{
	_direction = direction;
}

void PlayerObject::SetActionAttack1()
{
	if (_direction == e_LEFT)
		SetSprite(PLAYER_ATTACK1_L01, PLAYER_ATTACK1_L_MAX, dfDELAY_ATTACK1);
	else
		SetSprite(PLAYER_ATTACK1_R01, PLAYER_ATTACK1_R_MAX, dfDELAY_ATTACK1);
}

void PlayerObject::SetActionAttack2()
{
	if (_direction == e_LEFT)
		SetSprite(PLAYER_ATTACK2_L01, PLAYER_ATTACK2_L_MAX, dfDELAY_ATTACK2);
	else
		SetSprite(PLAYER_ATTACK2_R01, PLAYER_ATTACK2_R_MAX, dfDELAY_ATTACK2);
}

void PlayerObject::SetActionAttack3()
{
	if (_direction == e_LEFT)
		SetSprite(PLAYER_ATTACK3_L01, PLAYER_ATTACK3_L_MAX, dfDELAY_ATTACK3);
	else
		SetSprite(PLAYER_ATTACK3_R01, PLAYER_ATTACK3_R_MAX, dfDELAY_ATTACK3);
}

void PlayerObject::SetActionStand()
{
	if (_direction == e_LEFT)
		SetSprite(PLAYER_STAND_L01, PLAYER_STAND_L_MAX, dfDELAY_STAND);
	else
		SetSprite(PLAYER_STAND_R01, PLAYER_STAND_R_MAX, dfDELAY_STAND);
}

void PlayerObject::SetActionMove()
{
	if (_direction == e_LEFT)
		SetSprite(PLAYER_MOVE_L01, PLAYER_MOVE_L_MAX, dfDELAY_MOVE);
	else
		SetSprite(PLAYER_MOVE_R01, PLAYER_MOVE_R_MAX, dfDELAY_MOVE);
}
