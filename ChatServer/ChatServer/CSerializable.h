#ifndef __CSerializable__
#define __CSerializable__

#include <Windows.h>

class CAyaPacket
{
public:
	enum enAYA_PACKET
	{
		eBUFFER_DEFAULT = 512      // 패킷의 기본 버퍼 사이즈.
	};

	CAyaPacket();
	//CAyaPacket(int iBufferSize);

	//virtual   ~CAyaPacket();

	void   Initial(int iBufferSize = eBUFFER_DEFAULT);
	void   Release(void);
	void   Clear(void);


	int      GetBufferSize(void) { return m_iBufferSize; }
	int      GetDataSize(void) { return m_iDataSize; }


	char   *GetBufferPtr(void) { return m_chpReadPos; }

	int      MoveWritePos(int iSize);
	int      MoveReadPos(int iSize);



	CAyaPacket   &operator=(CAyaPacket &clSrcAyaPacket);


	CAyaPacket   &operator << (BYTE byValue);
	CAyaPacket   &operator << (char chValue);

	CAyaPacket   &operator << (short shValue);
	CAyaPacket   &operator << (WORD wValue);

	CAyaPacket   &operator << (int iValue);
	CAyaPacket   &operator << (DWORD dwValue);
	CAyaPacket   &operator << (float fValue);

	CAyaPacket   &operator << (__int64 iValue);
	CAyaPacket   &operator << (double dValue);


	CAyaPacket   &operator >> (BYTE &byValue);
	CAyaPacket   &operator >> (char &chValue);

	CAyaPacket   &operator >> (short &shValue);
	CAyaPacket   &operator >> (WORD &wValue);

	CAyaPacket   &operator >> (int &iValue);
	CAyaPacket   &operator >> (DWORD &dwValue);
	CAyaPacket   &operator >> (float &fValue);

	CAyaPacket   &operator >> (__int64 &iValue);
	CAyaPacket   &operator >> (double &dValue);



	int      GetData(char *chpDest, int iSize);

	int      PutData(char *chpSrc, int iSrcSize);




protected:

	//------------------------------------------------------------
	// 패킷버퍼 / 버퍼 사이즈.
	//------------------------------------------------------------
	char   m_chpBuffer[eBUFFER_DEFAULT];
	int      m_iBufferSize;

	//------------------------------------------------------------
	// 버퍼의 읽을 위치, 넣을 위치.
	//------------------------------------------------------------
	char   *m_chpReadPos;
	char   *m_chpWritePos;


	//------------------------------------------------------------
	// 현재 버퍼에 사용중인 사이즈.
	//------------------------------------------------------------
	int      m_iDataSize;


};

#endif