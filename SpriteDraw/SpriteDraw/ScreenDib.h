#ifndef __SCREEN_DIB__
#define __SCREEN_DIB__

#include <Windows.h>

class ScreenDib
{
public:
	ScreenDib(int width, int height, int colorBit);
	~ScreenDib();

protected:
	void CreateDibBuffer(int width, int height, int colorBit);
	void ReleaseDibBuffer(void);

public:
	void DrawBuffer(HWND hWnd, int x = 0, int y = 0);
	BYTE *GetDibBuffer(void);
	int GetWidth(void);
	int GetHeight(void);
	int GetPitch(void);

protected:
	BITMAPINFO _stDibInfo;
	BYTE *_pBuffer;
	
	int _width;
	int _height;
	int _pitch;
	int _colorBit;
	int _bufferSize;
};

#endif