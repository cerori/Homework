#include "stdafx.h"
#include "SpriteDib.h"
#include "EffectObject.h"

extern SpriteDib g_SpriteDib;

EffectObject::EffectObject()
{
	isEffectStart = FALSE;
	_objectType = EFFECT;
}

void EffectObject::Action(void)
{
	if (this->isEffectStart)
		NextFrame();
}

void EffectObject::Draw(BYTE * pDest, int destWidth, int destHeight, int destPitch)
{
	if (IsEndFrame())
		this->isEffectStart = FALSE;

	g_SpriteDib.DrawSprite(GetSpriteNow(), GetCurX(), GetCurY(), pDest, destWidth, destHeight, destPitch);

}
