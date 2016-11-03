#include "stdafx.h"
#include "KeyMgr.h"

// ����ƽ �ɹ������� Ŭ���� ������ �ʱ�ȭ �� �� ���� �ۿ��� �ʱ�ȭ �ϵ��� �Ѵ�.
cKey *cKey::m_pInstance = NULL;

cKey::cKey(void)
{
	ZeroMemory(m_bKeyDown, KEYMAX);
	ZeroMemory(m_bKeyUp, KEYMAX);
	// Ű ���۵��� �ʱ�ȭ ��Ų��.
}

cKey::~cKey(void)
{
}

cKey* cKey::GetInst(void)
{
	// �̱��� ������ �����
	if (m_pInstance == NULL)
	{
		m_pInstance = new cKey;
		//RSCMGR->m_bKEY_MGR = true;
	}

	return m_pInstance;
}
void    cKey::FreeInst(void)
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
		//RSCMGR->m_bKEY_MGR = false;
	}
}

//  ��� ��������
bool    cKey::StayKeyDown(INT nKey)
{
	if (GetAsyncKeyState(nKey) & 0x8000)
	{
		return TRUE;
	}

	return FALSE;
}

//  �ѹ��� �������� üũ
bool    cKey::OnceKeyDown(INT nKey)
{
	if (GetAsyncKeyState(nKey) & 0x8000)
	{
		//  ������ ���� true
		if (m_bKeyDown[nKey] == false)
		{
			m_bKeyDown[nKey] = true;
			return TRUE;
		}
	}

	else
	{
		//  ���� ���� false
		m_bKeyDown[nKey] = false;
	}

	return FALSE;
}

//  �ѹ� ���ȴٰ� ��������� üũ
bool    cKey::OnceKeyUp(INT nKey)
{
	if (GetAsyncKeyState(nKey) & 0x8000)
	{
		m_bKeyUp[nKey] = true;
	}

	else
	{
		// ������ ���� true�� �ǹǷ� true�϶��� ����
		if (m_bKeyUp[nKey] == true)
		{
			//  Ű���¸� false�� ����� ������ �Ұ�
			m_bKeyUp[nKey] = false;

			return TRUE;
		}
	}

	return FALSE;
}

//  ���Ű
bool    cKey::IsToggleKey(INT nKey)
{
	if (GetKeyState(nKey) & 0x0001) // GetKeyState�� 0x0001 �� ������ ���ȴ°� �� üũ�Ѵ�.
	{
		return TRUE;
	}

	return FALSE;
}