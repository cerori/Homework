#include <stdio.h>
#include <string.h>
#include <Windows.h>

#include "my_list.h"
#include "MemoryPool.h"


#define df_MAX 100000

int g_check[df_MAX];
int g_maxUseCount;

class CTest
{
	static int s_index;	//���� ������ ����� index ���ప. ��ü ��� ����Ƚ���� ����.
	int _index;			//��� ������ �ε��� ��.

	DWORD _lifeTick;	//����� ����.

public:
	CTest() 
	{
		_index = s_index;
		s_index++;

		_lifeTick = rand() % (1000 * 5) + GetTickCount();
	}
	~CTest() 
	{
		g_check[_index]++;
	}


public:
	static int GetStaticIndex() { return s_index; }
	bool IsDeleteTime() { return (_lifeTick < GetTickCount()) ? true : false; }
};
int CTest::s_index = 0;


bool AllocAndFree()
{
	list<CTest*> testList;

	//[�������, ������� ��� �׽�Ʈ ����. bPlacementNew�� true���� �׽�Ʈ ����.]
	//��, ���� �׽�Ʈ���� ��������� ��� ��������� �ʰ��Ͽ� ���� ��쿡 ���Ͽ��� �׽�Ʈ�� �� �ǰ� �ִ�.
	//�̷� ������ �׽�Ʈ�� �ϰ� �ʹٸ� g_check�� ũ�⸦ df_MAX���� �� ũ�� �ְ�
	//���� ���ǵ� createCount <= df_MAX���� df_MAX�� ���� �ٲ� g_check�� ũ��� �ְ�
	//�� ���� df_MAX���� ���������� �ָ� �ȴ�. �׷� ���� �޸��Ҵ� ���а� ���� �ʰ�(�߸� ����)
	//g_maxUseCount�� allocCount���� �� ū ����� ����Ǿ��ٸ� �����÷ο찡 �Ǿ��ٴ� ���̴�.
	CMemoryPool<CTest> memPool(df_MAX, true);

	DWORD makeTick = (1000 * 5) + GetTickCount();

	while (1)
	{
		CTest* pTest;

		DWORD curTick = GetTickCount();

		//�������� ������ �ε����� +1������ ���̹Ƿ� �ǹ̻� ��������.
		int createCount = CTest::GetStaticIndex();



		if (curTick < makeTick && createCount <= df_MAX)
		{
			//75%�� Ȯ���� 1~14�� ����
			int loop = rand() % 20;
			if (5 < loop)
			{
				loop -= 5;

				if (createCount + loop <= df_MAX)
				{
					while (loop)
					{
						pTest = memPool.Alloc();
						if (nullptr == pTest)
						{
							printf("�޸� �Ҵ� ����!\n");
							return false;
						}
						testList.push_back(pTest);

						loop--;
					}
				}
			}
		}



		if (g_maxUseCount < memPool.GetUseCount())
			g_maxUseCount = memPool.GetUseCount();



		for (auto iter = testList.begin(); iter != testList.end(); )
		{
			if ((*iter)->IsDeleteTime())
			{
				if (false == memPool.Free((*iter)))
				{
					printf("�޸� ���� ����!\n");
					return false;
				}

				iter = testList.erase(iter);
			}
			else
				iter++;
		}

		//ù ���ۺ��� �ϳ��� �� ��������� ���������� �ܿ� ����
		if (makeTick <= curTick || df_MAX < createCount)
		{
			//List�� ��� �������� ����
			if (testList.empty())
				break;
		}
	}

	//�� ����� ���� ������ ���� ������� ����.
	//�� ����� ���� ������ ����Ƚ������ ������ϸ� ���� ������� ������ 0������ �Ѵ�.
	printf("[�Ҵ�� ������ ��� ���� ���� ����]\n");
	printf("allocCount : %d, useCound : %d\n", memPool.GetAllocCount(), memPool.GetUseCount());
	return true;
}


int main()
{
	srand(GetTickCount());

	memset(&g_check, 0, df_MAX);
	g_maxUseCount = 0;

	if (false == AllocAndFree())
		return -1;

	int createCount = CTest::GetStaticIndex();
	for (int i = 0; i < createCount; i++)
	{
		if (1 != g_check[i])
		{
			printf("\n[������] : �Ҵ� ���� ����ġ\n");
			printf("������ Ƚ�� : %d\n", createCount);
			printf("�ִ����ߴ� ��尳�� : %d\n", g_maxUseCount);
			return -1;
		}
	}

	printf("\n��[����] : �Ҵ� ���� ��ġ��\n");
	printf("������ Ƚ�� : %d\n", createCount);
	printf("�ִ����ߴ� ��尳�� : %d\n", g_maxUseCount);


	printf("\n\n");
	return 0;
}