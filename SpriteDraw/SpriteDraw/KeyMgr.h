#pragma once

#include <Windows.h>

const int   KEYMAX = 256;
// �˳��ϰ� 255�� ������� 109Ű Ű����� ���콺 2��ư�� ������ 111�� ��Ƶ� ����
class cKey
{
private:
	static  cKey*       m_pInstance;            // Ŭ���� ������
	bool        m_bKeyDown[KEYMAX];     // Ű�� ���ȴ��� üũ�� �迭
	bool        m_bKeyUp[KEYMAX];       // Ű�� �������� üũ�� �迭

private:
	cKey(void);

public:
	~cKey(void);
	static  cKey*       GetInst(void);          // �ν��Ͻ� ����
	void        FreeInst(void);         // �ν��Ͻ� �ı�
	bool        StayKeyDown(INT nKey);// Ű�� ������ �ִ��� üũ
	bool        OnceKeyDown(INT nKey);// Ű�� �ѹ� ���ȴ��� üũ
	bool        OnceKeyUp(INT nKey);  // Ű�� �ѹ� ���ȴ� �������� üũ
	bool        IsToggleKey(INT nKey);// �ѹ� ���������� on off�� �ٲ�
};
