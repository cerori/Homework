#ifndef __EFFECT__
#define __EFFECT__

#include "BaseObject.h"

class EffectObject : public BaseObject
{
public:
	BOOL isEffectStart;

public:
	EffectObject();
	virtual void Action(void);
	virtual void Draw(BYTE *pDest, int destWidth, int destHeight, int destPitch);
	void SetEffect(void);

private:
	DWORD _attackId;
};

#endif
