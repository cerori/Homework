#include "stdafx.h"
#include "CSerializable.h"

CAyaPacket::CAyaPacket()
{
	m_iBufferSize = eBUFFER_DEFAULT;
	m_chpReadPos = m_chpBuffer;
	m_chpWritePos = m_chpBuffer;
	m_iDataSize = 0;
}

void   CAyaPacket::Initial(int iBufferSize)
{

}

void   CAyaPacket::Release(void)
{

}

void   CAyaPacket::Clear(void)
{
	m_chpReadPos = m_chpBuffer;
	m_chpWritePos = m_chpBuffer;
}

int      CAyaPacket::MoveWritePos(int iSize)
{
	m_chpWritePos += iSize;
	return iSize;
}

int      CAyaPacket::MoveReadPos(int iSize)
{
	m_chpReadPos += iSize;
	return iSize;
}

int      CAyaPacket::GetData(char *chpDest, int iSize)
{
	int dataSize = m_iDataSize;
	if (dataSize == 0) return 0;

	int size = iSize;
	if (iSize > dataSize) size = dataSize;

	memcpy_s(chpDest, size, m_chpReadPos, size);

	m_iDataSize -= size;
	m_chpReadPos += size;

	return size;
}

int      CAyaPacket::PutData(char *chpSrc, int iSrcSize)
{
	int freeSize = m_iBufferSize - m_iDataSize;
	if (freeSize == 0) return 0;

	int size = iSrcSize;
	if (size > freeSize) size = freeSize;

	memcpy_s(m_chpWritePos, size, chpSrc, size);

	m_iDataSize += size;
	m_chpWritePos += size;
	return size;
}

CAyaPacket& CAyaPacket::operator=(CAyaPacket &clSrcAyaPacket)
{
	memcpy_s(m_chpBuffer, eBUFFER_DEFAULT, clSrcAyaPacket.m_chpBuffer, eBUFFER_DEFAULT);

	m_chpReadPos = clSrcAyaPacket.m_chpReadPos;
	m_chpWritePos = clSrcAyaPacket.m_chpWritePos;
	m_iDataSize = clSrcAyaPacket.m_iDataSize;

	return *this;
}

CAyaPacket& CAyaPacket::operator << (BYTE byValue)
{
	PutData((char*)&byValue, sizeof(BYTE));

	return *this;
}
CAyaPacket& CAyaPacket::operator << (char chValue)
{
	PutData((char*)&chValue, sizeof(char));

	return *this;
}

CAyaPacket& CAyaPacket::operator << (short shValue)
{
	PutData((char*)&shValue, sizeof(short));

	return *this;
}
CAyaPacket& CAyaPacket::operator << (WORD wValue)
{
	PutData((char*)&wValue, sizeof(WORD));

	return *this;
}

CAyaPacket& CAyaPacket::operator << (int iValue)
{
	PutData((char*)&iValue, sizeof(int));

	return *this;
}
CAyaPacket& CAyaPacket::operator << (DWORD dwValue)
{
	PutData((char*)&dwValue, sizeof(DWORD));

	return *this;
}
CAyaPacket& CAyaPacket::operator << (float fValue)
{
	PutData((char*)&fValue, sizeof(float));

	return *this;
}

CAyaPacket& CAyaPacket::operator << (__int64 iValue)
{
	PutData((char*)&iValue, sizeof(__int64));

	return *this;
}
CAyaPacket& CAyaPacket::operator << (double dValue)
{
	PutData((char*)&dValue, sizeof(double));

	return *this;
}

CAyaPacket& CAyaPacket::operator >> (BYTE &byValue)
{
	GetData((char*)byValue, sizeof(BYTE));

	return *this;
}
CAyaPacket& CAyaPacket::operator >> (char &chValue)
{
	GetData((char*)chValue, sizeof(char));

	return *this;
}

CAyaPacket& CAyaPacket::operator >> (short &shValue)
{
	GetData((char*)shValue, sizeof(short));

	return *this;
}
CAyaPacket& CAyaPacket::operator >> (WORD &wValue)
{
	GetData((char*)wValue, sizeof(WORD));

	return *this;
}

CAyaPacket& CAyaPacket::operator >> (int &iValue)
{
	GetData((char*)iValue, sizeof(int));

	return *this;
}
CAyaPacket& CAyaPacket::operator >> (DWORD &dwValue)
{
	GetData((char*)dwValue, sizeof(DWORD));

	return *this;
}
CAyaPacket& CAyaPacket::operator >> (float &fValue)
{
	float temp = fValue;
	GetData((char*)&temp, sizeof(float));

	return *this;
}

CAyaPacket& CAyaPacket::operator >> (__int64 &iValue)
{
	GetData((char*)iValue, sizeof(__int64));

	return *this;
}
CAyaPacket& CAyaPacket::operator >> (double &dValue)
{
	double temp = dValue;
	GetData((char*)&temp, sizeof(double));

	return *this;
}