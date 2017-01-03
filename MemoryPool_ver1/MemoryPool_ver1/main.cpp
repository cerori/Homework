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
	static int s_index;	//다음 생성될 노드의 index 예약값. 전체 노드 생성횟수와 동일.
	int _index;			//노드 고유의 인덱스 값.

	DWORD _lifeTick;	//노드의 수명.

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

	//[정적모드, 동적모드 모두 테스트 가능. bPlacementNew는 true값만 테스트 가능.]
	//단, 현재 테스트에서 정적모드일 경우 블락개수를 초과하여 사용된 경우에 대하여선 테스트가 안 되고 있다.
	//이런 경우까지 테스트를 하고 싶다면 g_check의 크기를 df_MAX보다 더 크게 주고
	//생성 조건도 createCount <= df_MAX에서 df_MAX를 새로 바뀐 g_check의 크기로 주고
	//그 밑의 df_MAX값도 마찬가지로 주면 된다. 그런 다음 메모리할당 실패가 뜨지 않고(뜨면 정상)
	//g_maxUseCount가 allocCount보다 더 큰 결과가 산출되었다면 오버플로우가 되었다는 뜻이다.
	CMemoryPool<CTest> memPool(df_MAX, true);

	DWORD makeTick = (1000 * 5) + GetTickCount();

	while (1)
	{
		CTest* pTest;

		DWORD curTick = GetTickCount();

		//마지막에 생성된 인덱스의 +1상태일 것이므로 의미상 문제없다.
		int createCount = CTest::GetStaticIndex();



		if (curTick < makeTick && createCount <= df_MAX)
		{
			//75%의 확률로 1~14개 생성
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
							printf("메모리 할당 실패!\n");
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
					printf("메모리 해제 실패!\n");
					return false;
				}

				iter = testList.erase(iter);
			}
			else
				iter++;
		}

		//첫 시작부터 하나도 안 만들어져서 나가버리는 겨우 방지
		if (makeTick <= curTick || df_MAX < createCount)
		{
			//List가 모두 지워지면 나감
			if (testList.empty())
				break;
		}
	}

	//블럭 만들어 놓은 개수와 실제 사용중인 개수.
	//블럭 만들어 놓은 개수는 생성횟수보다 적어야하며 실제 사용중인 개수는 0개여야 한다.
	printf("[할당과 해제가 모두 끝난 직후 정보]\n");
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
			printf("\n[비정상] : 할당 해제 불일치\n");
			printf("생성한 횟수 : %d\n", createCount);
			printf("최대사용했던 노드개수 : %d\n", g_maxUseCount);
			return -1;
		}
	}

	printf("\n★[정상] : 할당 해제 일치★\n");
	printf("생성한 횟수 : %d\n", createCount);
	printf("최대사용했던 노드개수 : %d\n", g_maxUseCount);


	printf("\n\n");
	return 0;
}