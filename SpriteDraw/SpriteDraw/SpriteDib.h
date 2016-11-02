#ifndef __SPRITE_DIB_H__
#define __SPRITE_DIB_H__

class SpriteDib
{
public:
	typedef struct stSprite
	{
		BYTE *pImage;
		int width;
		int height;
		int pitch;
		int centerPointX;
		int centerPointY;
	} stSprite;
	SpriteDib(int maxSprite, DWORD colorKey);
	virtual ~SpriteDib();

	BOOL LoadDibSprite(int spriteIndex, WCHAR *fileName, int centerPointX, int centerPointY); 
	void ReleaseSprite(int spriteIndex);
	void DrawSprite(int spriteIndex, int drawX, int drawY,
		BYTE *pDest, int destWidth, int destHeight, int destPitch, int drawLen = 100);
	void DrawImage(int spriteIndex, int drawX, int drawY,
		BYTE *pDest, int destWidth, int destHeight, int destPitch, int drawLen = 100);
	POINT GetPoint(void);

protected:
	int _maxSprite;
	stSprite *_stSprite;
	DWORD _colorKey;
	POINT _drawPos;
};

#endif