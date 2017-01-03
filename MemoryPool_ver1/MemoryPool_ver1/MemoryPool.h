#pragma once
#include <stdlib.h>
#include <new.h>


/*
[�����Լ����̺�]
sizeof(st_BLOCK)�� �����Լ����̺��������� ũ����� ����ؼ� ����üũ�⸦ �����ش�.
Placement New�� �����Լ����̺��������� ������ ä���ش�.
���� �����Լ����̺� ���������δ� �Ƚ��� �� �ִ�.
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
	�Ҹ����� ������ MemoryPool�� ����ϴ� ����ڰ� Alloc�� ���Ͽ� �Ҵ���� ��������
	�ٽ� Free�� ���Ͽ� ��� ��ȯ�� �־��ٴ� �����Ͽ� ����.
	���� �׷��� �ʾҴٸ� �װ� ������� �߸�.
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
				//[�ּ� 1] : ������ ���� �Ʒ� ����
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
		//Free�Լ��� �������� ������� ��� ó�������� ����.
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
	const bool _bDynamicMode;	//�������� ����ϰ� �ʹٸ� ���������� �� blockNum�� 0���� �ش�.
	const bool _bPlacementNew;	//MemoryPool�� Alloc �� Free�� �����ڿ� �ı��� ȣ�� ����

	//��� ���� ��������. ��������� ��쿡�� ���.
	st_BLOCK* _pBuffer;

	//������ Top�� ���� ������ �� ���̴�. Alloc�Լ��� Pop, Free�Լ��� Push�� �ؼ��ص� �����ϴ�.
	//���� ����� ��� Stack�� ����ִµ� Pop(Alloc)�� �� �� ���� ���� �ϳ��� ���� ���ش�.
	//�� �� ������� ������ ����ڰ� ����� ��ģ �� Push(Free)�� �־��ָ� ���ÿ� �߰��ȴ�.
	st_BLOCK* _pFreeNode;

	int _allocCount;	//�Ҵ�� ����� �� ����
	int _useCount;		//������� ����� ����
};



/*-----------------------------------------------------------------------------------------
���� �ּ� 1 ����
malloc�� �ƴ� new�� ����ϰ� �Ǹ� MemoryPool�� �Ҹ��ڿ���
�Ҵ�Ǿ��� ���� ������ �� free�� �ƴ� delete�� ����Ͽ� �����ϰ� �� �ٵ�
�׷��� �Ǹ� _bPlacementNew�� �������� ��� data�� �Ҹ��ڰ� ������ ���� 2�� ȣ���ϰ� �ȴ�.

1. MemoryPool�� Free�Լ����� ���� �Ҹ��� ȣ��.
2. MemoryPool�� �Ҹ��ڿ��� delete�� ���� �Ҹ��� ȣ��.

���� DataType�� �Ҹ��ڿ��� delete�� free���� �� ����ϰ� �־��ٸ�
������ �޸𸮸� �ٽ� �����Ϸ��ϰų� nullptr�� �����Ϸ��� ���� ������ ���� ���� �ִ�.

�� ������ �ذ��ϱ� ���ؼ��� MemoryPool�� �Ҹ��ڿ��� delete�ϱ� ��
Placement New�� ���� �����ڸ� ȣ�� �� delete�� �ؾ߸��Ѵ�.
�׷��� �� �ذ���� ���� �츮�� ���� ����� g_memoryPool�� ���� ���������� ����Ѵٸ�
���� ������ �� ������ ���� �츮�� ���� ���̺귯���� �ٸ� ����ڿ��� �־��� ��,
�� ����ڰ� Ư�� Scene��ü�� ��������� Ư�� �ֿ��Լ��� ���������ν� memoryPool������
�����ϰ� �ű�� DataType�� �����ڿ��� ���������� ����ƽ����, �ٸ� Ŭ������ ���������
������ ��ĥ �� �ִ� �Լ�ȣ�� ���� �� ��� ������ ���� �� �ִ�.

�ܼ��� ���� ���ڸ� �ù� class�� �����ڰ� ȣ�� �� ������ � ���������� ���� ��������
�� ���ÿ��� �¾�� �ùμ��� üũ�ߴٰ� �����غ���. �׸��� �� ����ڴ� �ڽ��� �ǵ����
memoryPool�� �۵���Ű�� ���� placementNew���� true�� �ִ� �� ���������� ó���Ͽ���.
�׷��� ������ memoryPool������ Ư�� Scene�� ���������������� �ϳ��� ������ ���� ��
Scene�� �ٲ�鼭 memoryPool�� �Ҹ��ڰ� ȣ��Ǹ� ������� �Ǿ���. �׸��� �ٸ� Scene�� ���۵Ǿ�
�� Scene���� ���ÿ��� �¾�� �ùμ��� ���س��� ���������� �����ϰ� �� �� �ùٸ� ���� ��������
���� ���̴�. memoryPool���� �Ҹ����� ������ ũ�⸸ŭ �����ڸ� �� �� �� ȣ���߱� �����̴�.

���� �츮�� g_memoryPool�� ���� ���������θ� ����ϱ� ������ ���� ������ ���� ����
���� �׳� ����ڰ� DataType�� �����ڳ� �Ҹ��ڿ��� ��������, ����ƽ����, �ٸ� Ŭ���� ����Լ�
���� ������� �ʰų� ����Ѵ� �ϴ��� �� �����͵��� memoryPool������ �Ҹ�� ���
�� �̻� ��ȿ�� ������ ���� �ʴ� �������� �ڵ带 ­�ٸ� �������� ���� ���̴�.

�׷����� �и� �̰� �����ϱ� �ϴ�. ����� ���忡�� Alloc�ÿ��� ������ ȣ��ǰ�
Free�ÿ��� �Ҹ��ڰ� ȣ��Ǳ⸦ ����ϸ� memoryPool�� �ι� ° ���ڿ� true���� ���� �ٵ�
���� �׷��� ������ ��쿡 ���� �̷� �� ������ ������ �� ������ �ִٶ�.....
������ ������ �� ���� ���̺귯�� �����ڴ� ����ڰ� �߸� ����ߴٸ� ſ�� ������ ������?

�׷��� ���� �׳� Alloc�Լ����� new ��� malloc + placement new ����� ä���Ͽ���.
�̷��� �� �ܰ�� �����ٰ� �ؼ� new�� ������� ������ ������ ������ �������ų� �� �� ���� �ʴ�.
�׸��� ������ ������忡�� ���� ������ �Ĵ� �۾��� �� ���� �ʹ����� �翬�ϰԵ�
������ new(���⼭ ���ϴ� �� Alloc�Լ��� new�� �ƴ� ��¥ ���� new)���� ������.
������ �̷� ��������� ��� MemoryPool�� ������ �����ϴ� �ñ�� ����� ������ �� ��
������ �󵵰� �������� ������ ���� ������ ���̴�.
-----------------------------------------------------------------------------------------*/