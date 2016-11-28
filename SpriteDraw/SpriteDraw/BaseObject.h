#ifndef __BASEOBJECT_H__
#define __BASEOBJECT_H__

#include <Windows.h>

class BaseObject
{
public:
	~BaseObject();
	virtual void Action() = 0;
	virtual void Draw(BYTE *pDest, int destWidth, int destHeight, int destPitch) = 0;
	void ActionInput(DWORD actionInput);
	DWORD GetId(void);
	void SetId(DWORD id);
	int GetCurX(void);
	int GetCurY(void);
	int GetObjectId(void);
	OBJECT_TYPE GetObjectType(void);
	int GetSpriteNow(void);
	bool IsEndFrame(void);
	void NextFrame(void);
	void SetPostion(int x, int y);
	void SetObjectType(OBJECT_TYPE type);
	void SetSprite(int start, int max, int frameDelay);

protected:
	DWORD _id;
	bool _isEndFrame;
	DWORD _actionInput;
	int _curX;
	int _curY;
	int _delayCount;
	int _frameDelay;
	int _objectId;
	OBJECT_TYPE _objectType;
	int _spriteNow;
	int _spriteStart;
	int _spriteEnd;
};

#endif