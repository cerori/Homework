#include "stdafx.h"
#include "BaseObject.h"

BaseObject::~BaseObject()
{

}

void BaseObject::ActionInput(DWORD actionInput)
{
	_actionInput = actionInput;
}

DWORD BaseObject::GetId(void)
{
	return _id;
}

void BaseObject::SetId(DWORD id)
{
	_id = id;
}

int BaseObject::GetCurX(void)
{
	return _curX;
}

int BaseObject::GetCurY(void)
{
	return _curY;
}

int BaseObject::GetObjectId(void)
{
	return _objectId;
}

OBJECT_TYPE BaseObject::GetObjectType(void)
{
	return _objectType;
}

bool BaseObject::IsEndFrame(void)
{
	return _isEndFrame;
}

void BaseObject::NextFrame(void)
{
	if (_spriteStart < 0)
		return;

	_delayCount++;

	if (_delayCount > _frameDelay)
	{
		_delayCount = 0;

		_spriteNow++;
		
		if (_spriteNow > _spriteEnd)
		{
			_spriteNow = _spriteStart;
			_isEndFrame = TRUE;
		}
	}
}

void BaseObject::SetPostion(int x, int y)
{
	_curX = x;
	_curY = y;
}

void BaseObject::SetObjectType(OBJECT_TYPE type)
{
	_objectType = type;
}

void BaseObject::SetSprite(int start, int max, int frameDelay)
{
	_spriteStart = start;
	_spriteEnd = max;
	_frameDelay = frameDelay;

	_spriteNow = start;
	_delayCount = 0;
	_isEndFrame = FALSE;
}

int BaseObject::GetSpriteNow(void)
{
	return _spriteNow;
}

