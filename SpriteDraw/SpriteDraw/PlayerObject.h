#ifndef __PLAYER_OBJECT__
#define __PLAYER_OBJECT__

#include <Windows.h>
#include "BaseObject.h"

class PlayerObject : public BaseObject
{
public:
	PlayerObject();
	virtual void Action(void);
	virtual void Draw(BYTE *pDest, int destWidth, int destHeight, int destPitch);
	void ActionProc(void);
	void InputActionProc(void);
	BOOL IsPlayer(void);
	int GetHp(void);
	BYTE GetDirection(void);
	void SetHp(int hp);
	void SetDriection(BYTE direction);
	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();
	void SetActionStand();
	void SetActionMove();

protected:
	BOOL _player;
	int _hp;
	DWORD _actionCur;
	DWORD _actionPrev;
	BYTE _direction;
};


#endif