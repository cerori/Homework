#ifndef __CSerializable__
#define __CSerializable__

#include <Windows.h>

class CAyaPacket
{
public:
	enum enAYA_PACKET
	{
		eBUFFER_DEFAULT = 512      // ��Ŷ�� �⺻ ���� ������.
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
	// ��Ŷ���� / ���� ������.
	//------------------------------------------------------------
	char   m_chpBuffer[eBUFFER_DEFAULT];
	int      m_iBufferSize;

	//------------------------------------------------------------
	// ������ ���� ��ġ, ���� ��ġ.
	//------------------------------------------------------------
	char   *m_chpReadPos;
	char   *m_chpWritePos;


	//------------------------------------------------------------
	// ���� ���ۿ� ������� ������.
	//------------------------------------------------------------
	int      m_iDataSize;


};

#endif