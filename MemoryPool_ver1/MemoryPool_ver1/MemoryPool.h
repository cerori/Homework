#pragma once
#include <stdlib.h>
#include <new.h>


/*
[가상함수테이블]
sizeof(st_BLOCK)은 가상함수테이블포인터의 크기까지 고려해서 구조체크기를 구해준다.
Placement New는 가상함수테이블포인터의 값까지 채워준다.
따라서 가상함수테이블 관련쪽으로는 안심할 수 있다.
*/


template <typename DataType>
class CMemoryPool
{
private:
	struct st_BLOCK
	{
		st_BLOCK* pNext;
		DataType data;

		st_BLOCK() :
			pNext(nullptr) {}
	};




public:
	CMemoryPool(int blockNum, bool bPlacementNew = true) :
		_bDynamicMode(!blockNum),
		_bPlacementNew(bPlacementNew)
	{
		_allocCount = blockNum;
		_useCount = 0;

		if (_bDynamicMode)
			_pFreeNode = nullptr;
		else
		{
			_pBuffer = (st_BLOCK*)malloc(sizeof(st_BLOCK) * blockNum);

			st_BLOCK* pBlock = _pBuffer;
			for (int i = 0; i < blockNum; i++)
			{
				pBlock->pNext = pBlock + 1;
				if (!_bPlacementNew)
					new (&pBlock->data) DataType;

				pBlock++;
			}
			pBlock--;
			pBlock->pNext = nullptr;

			_pFreeNode = _pBuffer;
		}
	}


	/*///////////////////////////////////////////////////////////////////////////////////
	소멸자의 로직은 MemoryPool을 사용하는 사용자가 Alloc을 통하여 할당받은 공간들을
	다시 Free를 통하여 모두 반환해 주었다는 가정하에 전개.
	만약 그러지 않았다면 그건 사용자의 잘못.
	///////////////////////////////////////////////////////////////////////////////////*/
	~CMemoryPool()
	{
		if (_bDynamicMode)
		{
			while (nullptr != _pFreeNode)
			{
				st_BLOCK* pBlock = _pFreeNode;
				_pFreeNode = _pFreeNode->pNext;

				if (!_bPlacementNew)
					pBlock->data.~DataType();

				free(pBlock);
			}
		}
		else
		{
			if (!_bPlacementNew)
			{
				st_BLOCK* pBlock = _pBuffer;
				for (int i = 0; i < _allocCount; i++)
				{
					pBlock->data.~DataType();
					pBlock++;
				}
			}

			free(_pBuffer);
		}
	}




public:
	DataType* Alloc()
	{
		DataType* pNewData;

		if (_bDynamicMode)
		{
			if (nullptr == _pFreeNode)
			{
				//[주석 1] : 페이지 가장 아래 참고
				st_BLOCK* pNewBlock = (st_BLOCK*)malloc(sizeof(st_BLOCK));
				pNewData = new (&pNewBlock->data) DataType;
				_allocCount++;
			}
			else
			{
				pNewData = &_pFreeNode->data;
				_pFreeNode = _pFreeNode->pNext;

				if (_bPlacementNew)
					new (pNewData) DataType;
			}
		}
		else
		{
			if (nullptr == _pFreeNode)
				return nullptr;

			pNewData = &_pFreeNode->data;
			_pFreeNode = _pFreeNode->pNext;

			if (_bPlacementNew)
				new (pNewData) DataType;
		}

		_useCount++;
		return pNewData;
	}
	bool Free(DataType* pData)
	{
		//Free함수는 동적모드와 정적모드 모두 처리로직이 같다.
		if (0 == _useCount)
			return false;

		st_BLOCK* pNewFreeNode = (st_BLOCK*)(((char*)pData) - sizeof(DataType*));

		pNewFreeNode->pNext = _pFreeNode;
		_pFreeNode = pNewFreeNode;

		if (_bPlacementNew)
			pData->~DataType();

		_useCount--;
		return true;
	}




public:
	int GetAllocCount() { return _allocCount; }
	int GetUseCount() { return _useCount; }




private:
	const bool _bDynamicMode;	//동적모드로 사용하고 싶다면 생성자인자 중 blockNum에 0값을 준다.
	const bool _bPlacementNew;	//MemoryPool의 Alloc 및 Free시 생성자와 파괴자 호출 여부

	//덩어리 블럭의 시작지점. 정적모드일 경우에만 사용.
	st_BLOCK* _pBuffer;

	//스택의 Top과 같은 역할을 할 것이다. Alloc함수를 Pop, Free함수를 Push로 해석해도 무방하다.
	//동적 모드일 경우 Stack이 비어있는데 Pop(Alloc)을 할 시 새로 공간 하나를 만들어서 내준다.
	//그 때 만들어진 공간은 사용자가 사용을 마친 후 Push(Free)로 넣어주면 스택에 추가된다.
	st_BLOCK* _pFreeNode;

	int _allocCount;	//할당된 블락의 총 개수
	int _useCount;		//사용중인 블락의 개수
};



/*-----------------------------------------------------------------------------------------
ㅁㅁ 주석 1 ㅁㅁ
malloc이 아닌 new를 사용하게 되면 MemoryPool의 소멸자에서
할당되었던 블럭을 해제할 때 free가 아닌 delete를 사용하여 해제하게 될 텐데
그렇게 되면 _bPlacementNew가 켜져있을 경우 data의 소멸자가 다음과 같이 2번 호출하게 된다.

1. MemoryPool의 Free함수에서 직접 소멸자 호출.
2. MemoryPool의 소멸자에서 delete로 인한 소멸자 호출.

만일 DataType의 소멸자에서 delete나 free같은 걸 사용하고 있었다면
해제한 메모리를 다시 해제하려하거나 nullptr를 해제하려는 등의 문제가 생길 수도 있다.

이 문제를 해결하기 위해서는 MemoryPool의 소멸자에서 delete하기 전
Placement New를 통해 생성자를 호출 후 delete를 해야만한다.
그러나 이 해결법은 지금 우리가 쓰는 방식인 g_memoryPool과 같이 전역변수로 사용한다면
전혀 문제될 게 없지만 만약 우리가 만든 라이브러리를 다른 사용자에게 주었을 때,
그 사용자가 특정 Scene객체의 멤버변수나 특정 주요함수의 지역변수로써 memoryPool변수를
선언하고 거기다 DataType의 생성자에서 전역변수나 스태틱변수, 다른 클래스의 멤버변수에
영향을 미칠 수 있는 함수호출 등을 할 경우 문제가 생길 수 있다.

단순한 예를 들어보자면 시민 class의 생성자가 호출 될 때마다 어떤 전역변수의 값을 증가시켜
그 도시에서 태어났던 시민수를 체크했다고 생각해보자. 그리고 이 사용자는 자신의 의도대로
memoryPool을 작동시키기 위해 placementNew값도 true로 주는 등 정상적으로 처리하였다.
그런데 문제는 memoryPool변수가 특정 Scene의 멤버변수였었어가지고 하나의 게임이 끝난 후
Scene이 바뀌면서 memoryPool도 소멸자가 호출되며 사라지게 되었다. 그리고 다른 Scene이 시작되어
그 Scene에서 도시에서 태어났던 시민수를 구해놓은 전역변수를 참고하게 될 때 올바른 값이 구해지지
않을 것이다. memoryPool에서 소멸직전 스택의 크기만큼 생성자를 한 번 더 호출했기 때문이다.

물론 우리는 g_memoryPool과 같이 전역변수로만 사용하기 때문에 전혀 문제될 것이 없고
또한 그냥 사용자가 DataType의 생성자나 소멸자에서 전역변수, 스태틱변수, 다른 클래스 멤버함수
등을 사용하지 않거나 사용한다 하더라도 그 데이터들을 memoryPool변수가 소멸될 경우
더 이상 유효한 값으로 보지 않는 관점으로 코드를 짯다면 문제되지 않을 것이다.

그렇지만 분명 이건 위험하긴 하다. 사용자 입장에선 Alloc시에만 생성자 호출되고
Free시에만 소멸자가 호출되기를 기대하며 memoryPool의 두번 째 인자에 true값을 넣을 텐데
실은 그렇지 않으며 경우에 따라 이런 점 때문에 문제가 될 여지가 있다라.....
문제가 생겼을 때 과연 라이브러리 제작자는 사용자가 잘못 사용했다며 탓할 권한이 있을까?

그래서 나는 그냥 Alloc함수에서 new 대신 malloc + placement new 방식을 채택하였다.
이렇게 두 단계로 나눈다고 해서 new만 사용했을 때보다 찜찜할 정도로 느려지거나 할 것 같지 않다.
그리고 어차피 동적모드에서 새로 공간을 파는 작업을 할 때는 너무나도 당연하게도
원래의 new(여기서 말하는 건 Alloc함수의 new가 아닌 진짜 단일 new)보다 느리다.
어차피 이런 동적모드의 경우 MemoryPool이 위력을 발휘하는 시기는 충분히 공간을 판 후
재사용의 빈도가 높아지기 시작할 때쯤 부터인 것이다.
-----------------------------------------------------------------------------------------*/