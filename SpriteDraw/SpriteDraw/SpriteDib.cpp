#include "stdafx.h"
#include "SpriteDib.h"

SpriteDib::SpriteDib(int maxSprite, DWORD colorKey)
{
	_maxSprite = maxSprite;
	_colorKey = colorKey;

	_stSprite = new stSprite[_maxSprite];
}

SpriteDib::~SpriteDib()
{
	int i;

	for (i = 0; i < _maxSprite; i++)
	{
		ReleaseSprite(i);
	}
}

BOOL SpriteDib::LoadDibSprite(int spriteIndex, WCHAR *fileName, int centerPointX, int centerPointY)
{
	HANDLE hFile;
	DWORD read;
	DWORD imageSize;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	BYTE *pBuffImage, *pTurnImage, *pRef;
	int i, pitch;

	hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	ReadFile(hFile, &fileHeader, sizeof(BITMAPFILEHEADER), &read, NULL);
	// BMP 파일인지 확인

	ReadFile(hFile, &infoHeader, sizeof(BITMAPINFOHEADER), &read, NULL);
	// 컬러비트 확인

	pitch = infoHeader.biWidth * 4;

	_stSprite[spriteIndex].centerPointX = centerPointX;
	_stSprite[spriteIndex].centerPointY = centerPointY;
	_stSprite[spriteIndex].height = infoHeader.biHeight;
	_stSprite[spriteIndex].width = infoHeader.biWidth;
	_stSprite[spriteIndex].pitch = pitch;

	imageSize = infoHeader.biHeight * infoHeader.biWidth * 4;

	_stSprite[spriteIndex].pImage = new BYTE[imageSize];
	pRef = _stSprite[spriteIndex].pImage;

	pBuffImage = new BYTE[imageSize];
	ReadFile(hFile, pBuffImage, imageSize, &read, NULL);

	// 마지막 줄로 이동
	pTurnImage = pBuffImage + pitch * (infoHeader.biHeight - 1);

	for (i = 0; i < infoHeader.biHeight; i++)
	{
		memcpy(pRef, pTurnImage, pitch);
		pRef += pitch;
		pTurnImage -= pitch;
	}

	delete[] pBuffImage;
	CloseHandle(hFile);

	return TRUE;
}

void SpriteDib::ReleaseSprite(int spriteIndex)
{
	//delete _stSprite[spriteIndex].pImage;
}

void SpriteDib::DrawSprite(int spriteIndex, int drawX, int drawY,
	BYTE *pDest, int destWidth, int destHeight, int destPitch, int drawLen)
{
	int x, y;
	int spriteWidth, spriteHeight, spritePitct;
	int centerX, centerY;

	stSprite *sprite = &_stSprite[spriteIndex];

	BYTE *pSpriteImage;
	BYTE *spriteOrigin;
	BYTE *destOrigin;

	DWORD *dwDest;
	DWORD *dwSprite;

	if (spriteIndex >= _maxSprite)
		return;

	//실제 이미지가 있는지 확인

	spriteWidth = (int)((long)_stSprite[spriteIndex].width * ((long)drawLen * 0.01));
	spriteHeight = _stSprite[spriteIndex].height;
	spritePitct = _stSprite[spriteIndex].pitch;
	pSpriteImage = _stSprite[spriteIndex].pImage;

	centerX = _stSprite[spriteIndex].centerPointX;
	centerY = _stSprite[spriteIndex].centerPointY;

	// 중점 이동
	drawX -= _stSprite[spriteIndex].centerPointX;
	drawY -= _stSprite[spriteIndex].centerPointY;

	dwDest = (DWORD *)pDest;
	dwSprite = (DWORD *)_stSprite[spriteIndex].pImage;

	// 클리핑
	// 상
	if (drawY < 0)
	{
		spriteHeight += drawY;
		dwSprite = (DWORD *)(sprite->pImage + (sprite->pitch * (-drawY)));
		drawY = 0;
	}

	// 좌
	if (drawX < 0)
	{
		spriteWidth = spriteWidth - (-drawX);
		dwSprite = (DWORD *)((BYTE *)dwSprite + (-drawX));
		drawX = 0;
	}

	// 하
	if ((drawY + sprite->height) > destHeight)
	{
		spriteHeight -= drawY + spriteHeight - destHeight;
	}

	// 우
	if ((drawX + sprite->width) > destWidth)
	{
		spriteWidth -= drawX + sprite->width - destWidth;
	}

	_drawPos.x = drawX;
	_drawPos.y = drawY;

	dwDest = (DWORD *)((BYTE *)(dwDest + drawX) + (destPitch * drawY));

	spriteOrigin = (BYTE *)dwSprite;
	destOrigin = (BYTE *)dwDest;

	for (y = 0; y < spriteHeight; y++)
	{
		for (x = 0; x < spriteWidth; x++)
		{
			if (_colorKey != (*dwSprite & 0x00ffffff))
				*dwDest = *dwSprite;

			dwSprite++;
			dwDest++;
		}

		spriteOrigin = spriteOrigin + spritePitct;
		destOrigin = destOrigin + destPitch;

		dwSprite = (DWORD *)spriteOrigin;
		dwDest = (DWORD *)destOrigin;
	}
}

void SpriteDib::DrawImage(int spriteIndex, int drawX, int drawY,
	BYTE *pDest, int destWidth, int destHeight, int destPitch, int drawLen)
{
	int y;
	int spriteWidth, spriteHeight, spritePitct;
	int centerX, centerY;

	stSprite *sprite = &_stSprite[spriteIndex];

	DWORD *dwDest;
	DWORD *dwSprite;

	if (spriteIndex >= _maxSprite)
		return;

	//실제 이미지가 있는지 확인

	spriteWidth = _stSprite[spriteIndex].width;
	spriteHeight = _stSprite[spriteIndex].height;
	spritePitct = _stSprite[spriteIndex].pitch;

	centerX = _stSprite[spriteIndex].centerPointX;
	centerY = _stSprite[spriteIndex].centerPointY;

	// 중점 이동
//	drawX -= _stSprite[spriteIndex].centerPointX;
//	drawY -= _stSprite[spriteIndex].centerPointY;

	dwDest = (DWORD *)pDest;
	dwSprite = (DWORD *)_stSprite[spriteIndex].pImage;

	// 클리핑
	// 상
	if (drawY < 0)
	{
		spriteHeight += drawY;
		dwSprite = (DWORD *)(sprite->pImage + (sprite->pitch * (-drawY)));
		drawY = 0;
	}

	// 좌
	if (drawX < 0)
	{
		spriteWidth = spriteWidth - (-drawX);
		dwSprite = (DWORD *)((BYTE *)dwSprite + (-drawX));
		drawX = 0;
	}

	// 하
	if ((drawY + sprite->height) > destHeight)
	{
		spriteHeight -= drawY + spriteHeight - destHeight;
	}

	// 우
	if ((drawX + sprite->width) > destWidth)
	{
		spriteWidth -= drawX + sprite->width - destWidth;
	}

	dwDest = (DWORD *)((BYTE *)(dwDest + drawX) + (destPitch * drawY));

	for (y = 0; y < spriteHeight; y++)
	{
		memcpy(dwDest, dwSprite, spriteWidth * 4);

		dwSprite = (DWORD *)((BYTE *)dwSprite + sprite->pitch);
		dwDest = (DWORD *)((BYTE *)dwDest + destPitch);
	}
}

POINT SpriteDib::GetPoint(void)
{
	return _drawPos;
}
