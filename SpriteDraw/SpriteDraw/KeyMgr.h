#pragma once

#include <Windows.h>

const int   KEYMAX = 256;
// 넉넉하게 255를 잡았지만 109키 키보드와 마우스 2버튼을 포함한 111로 잡아도 무관
class cKey
{
private:
	static  cKey*       m_pInstance;            // 클래스 포인터
	bool        m_bKeyDown[KEYMAX];     // 키가 눌렸는지 체크할 배열
	bool        m_bKeyUp[KEYMAX];       // 키가 떼졌는지 체크할 배열

private:
	cKey(void);

public:
	~cKey(void);
	static  cKey*       GetInst(void);          // 인스턴스 생성
	void        FreeInst(void);         // 인스턴스 파괴
	bool        StayKeyDown(INT nKey);// 키가 눌리고 있는지 체크
	bool        OnceKeyDown(INT nKey);// 키가 한번 눌렸는지 체크
	bool        OnceKeyUp(INT nKey);  // 키가 한번 눌렸다 떼졌는지 체크
	bool        IsToggleKey(INT nKey);// 한번 눌릴때마다 on off로 바뀜
};
