#include "stdafx.h"
#include "ScreenDib.h"

ScreenDib::ScreenDib(int width, int height, int colorBit)
{
	_width = width;
	_height = height;
	_colorBit = colorBit;
	_pitch = width * 4;

	CreateDibBuffer(width, height, colorBit);
}

ScreenDib::~ScreenDib()
{
	ReleaseDibBuffer();
}
void ScreenDib::CreateDibBuffer(int width, int height, int colorBit)
{
	memset(&_stDibInfo.bmiHeader, 0, sizeof(_stDibInfo.bmiHeader));

	_stDibInfo.bmiHeader.biSize = sizeof(_stDibInfo.bmiHeader);
	_stDibInfo.bmiHeader.biSizeImage = width *  height * 4;
	_stDibInfo.bmiHeader.biWidth = width;
	_stDibInfo.bmiHeader.biHeight = -1;
	_stDibInfo.bmiHeader.biPlanes = 1;
	_stDibInfo.bmiHeader.biBitCount = _colorBit;

	_pBuffer = new BYTE[width * height * 4];
}

void ScreenDib::ReleaseDibBuffer(void)
{
	delete[] _pBuffer;
}

void ScreenDib::DrawBuffer(HWND hWnd, int x, int y)
{
	HDC hdc = GetDC(hWnd);

	SetDIBitsToDevice(hdc, x, y, _width, _height,
		0, 0, 0, _height, _pBuffer, &_stDibInfo, DIB_RGB_COLORS);

	DeleteObject(hdc);
}

BYTE *ScreenDib::GetDibBuffer(void)
{
	return _pBuffer;
}

int ScreenDib::GetWidth(void)
{
	return _width;
}

int ScreenDib::GetHeight(void)
{
	return _height;
}

int ScreenDib::GetPitch(void)
{
	return _pitch;
}
