#pragma once
/*
ㅁ 제작 방향 ㅁ
- 최대한 실제 STL과 똑같이 짜는 것을(적어도 겉 껍데기 만큼이라도) 목표로 하여 만듦.
- "my_list.h"를 사용해서 만들었던 모든 프로그램들에 대해 나중에 <list>로 교체하더라도
  아무런 문제 없이 똑같이 작동하는 정도까지만을 목표로 해서 만들어 나간다.
  (역의 성립까지는 고려하지 않음. 너무 힘듦.)
*/


template <typename DataType>
class list
{
private:
	struct S_NODE
	{
		DataType data;

		S_NODE* pPrev;
		S_NODE* pNext;

		//사용자의 클래스에 기본 생성자가 아예 없을 경우에도 유연하게 대처하기 위해 항상 아래 구조체 생성자를 거치게 함.
		//복사생성자는 그 어떤 정의 형태에서도 언제나 존재한다는 것을 이용한 방법.
		S_NODE(const DataType& copyData) : data(copyData) {}
	};

	//우선, STL의 end()와 rend()가 반복자와 연계되어 작동되는 방식을 흉내내기 위해서 더미방식을 채택하였다.
	//그리고 아래 헤드와 테일은 포인터변수이긴 하지만 더미노드를 가리키는 데에 사용할 것이다.
	//더미를 가리키는 포인터 방식을 채택한 이유는 일반 멤버변수로 더미를 만들면 얕은 복사시 문제가 발생하기 때문이다.
	//깊은 복사를 통해 가장왼쪽->pPrev와 가장오른쪽->pNext를 건드려서 해결한다해도... 아직 배우진 않았지만
	//나중에 멀티쓰레드 환경에서 반드시 문제가 발생할 예감이든다. 아무튼 그래서 이렇게 더미용 포인터변수를 두기로 했다.
	S_NODE* m_pHead;
	S_NODE* m_pTail;

	size_t m_size;

	size_t m_refCount;


public:
	class iterator
	{
		friend class list;

	private:
		S_NODE* m_pNode;

	public:
		iterator(S_NODE* pNode = nullptr) { m_pNode = pNode; }

	public:
		bool operator==(iterator iter) { return (m_pNode == iter.m_pNode) ? true : false; }
		bool operator!=(iterator iter) { return (m_pNode != iter.m_pNode) ? true : false; }

		iterator operator--()
		{
			if (nullptr == m_pNode || nullptr == m_pNode->pPrev->pPrev)
				throw "list iterator not decrementable";

			m_pNode = m_pNode->pPrev;
			return iterator(m_pNode);
		}
		const iterator operator--(int)
		{
			if (nullptr == m_pNode || nullptr == m_pNode->pPrev->pPrev)
				throw "list iterator not decrementable";

			const iterator iter(m_pNode);
			m_pNode = m_pNode->pPrev;

			return iter;
		}

		iterator operator++() 
		{
			if (nullptr == m_pNode || nullptr == m_pNode->pNext)
				throw "list iterator not incrementable";

			m_pNode = m_pNode->pNext;
			return iterator(m_pNode);
		}
		const iterator operator++(int)
		{
			if (nullptr == m_pNode || nullptr == m_pNode->pNext)
				throw "list iterator not incrementable";

			const iterator iter(m_pNode);
			m_pNode = m_pNode->pNext;

			return iter;
		}

		DataType& operator*()
		{
			if (nullptr == m_pNode || nullptr == m_pNode->pNext)
				throw "list iterator not dereferencable";

			return m_pNode->data; 
		}
		DataType* operator->()
		{
			if (nullptr == m_pNode || nullptr == m_pNode->pNext)
				throw "list iterator not dereferencable";

			return &m_pNode->data;
		}

	};

	iterator begin() { return (nullptr == m_pHead) ? iterator() : iterator(m_pHead->pNext); }
	iterator end() { return (nullptr == m_pTail) ? iterator() : iterator(m_pTail); }
	

private:
	void AddFirstNode(S_NODE* pNewNode, const DataType& copyData)
	{
		//더미 노드들은 사용하지 않을 것인데도 불구하고 copyData를 통해 복사하고 있다.
		//왜냐하면 DataType이 클래스일 경우 그곳에 기본 생성자가 없을 수도 있기 때문이다.
		//반면 복사생성자는 언제나 존재한다. 그 점을 이용한 것이다.
		//더미 노드용 구조체를 따로 선언한 뒤 반복자클래스의 멤버변수 타입을 void*로 바꾸고
		//그것의 각 멤버함수들 안에서 상황에 따라 적절히 캐스팅해대는 방법도 있겠지만
		//그냥 아래와 같은 방법이 훨씬 더 심플하고 적절하다고 생각한다.
		m_pHead = new S_NODE(copyData);
		m_pTail = new S_NODE(copyData);

		m_pHead->pPrev = nullptr;
		m_pHead->pNext = pNewNode;

		pNewNode->pPrev = m_pHead;
		pNewNode->pNext = m_pTail;

		m_pTail->pPrev = pNewNode;
		m_pTail->pNext = nullptr;
	}
	void DeleteLastNode()
	{
		delete m_pHead->pNext;
		delete m_pHead;
		delete m_pTail;

		m_pHead = nullptr;
		m_pTail = nullptr;
	}
public:
	iterator insert(iterator iter, const DataType& copyData)
	{
		S_NODE* pNewNode = new S_NODE(copyData);

		if (nullptr == iter.m_pNode && nullptr == m_pHead)
		{
			AddFirstNode(pNewNode, copyData);
		}
		else
		{
			pNewNode->pPrev = iter.m_pNode->pPrev;
			pNewNode->pNext = iter.m_pNode;

			iter.m_pNode->pPrev->pNext = pNewNode;
			iter.m_pNode->pPrev = pNewNode;
		}

		m_size++;
		return iterator(pNewNode);
	}
	iterator erase(iterator& iter)
	{
		if ((nullptr == iter.m_pNode && nullptr == m_pHead) ||
			m_pTail == iter.m_pNode)
			throw "list iterator not incrementable";	//왜 반복자를 증가시킬 수 없다는 메세지를 뱉는진 모르겠지만...

		S_NODE* pNextNode;

		if (m_pHead == iter.m_pNode->pPrev && iter.m_pNode->pNext == m_pTail)
		{
			pNextNode = nullptr;

			DeleteLastNode();
		}
		else
		{
			pNextNode = iter.m_pNode->pNext;

			iter.m_pNode->pPrev->pNext = pNextNode;
			pNextNode->pPrev = iter.m_pNode->pPrev;

			delete iter.m_pNode;
		}

		m_size--;
		iter.m_pNode = nullptr;
		return iterator(pNextNode);
	}
public:
	void push_front(DataType data) { insert(iterator(begin()), data); }
	void push_back(DataType data) { insert(iterator(end()), data); }

	void pop_front() { erase(iterator(begin())); }
	void pop_back() { erase(iterator(--end())); }


public:
	/*
	생각해봤는데, 게임에선 거품정렬이 다른 정렬들보다 빠른 경우가 많은 것 같다.

	1. 왜냐하면 이미 정렬이 되어 있는 경우가 다반사이고
	2. 정렬이 틀어지더라도 인접한 노드끼리만 틀어지거나
	3. 하나의 노드만 다른 어딘가로 이동하는 경우가 대부분이기 때문이다.
	(그렇지만 위와 같은 상황들이 아니라면 거품은 쭉쭉 빠질 것이다.....)

	3번에서 어딘가로 이동해야 할 노드가 begin쪽에서 end쪽으로 가는 경우라면
	거품이 위력을 발휘하겠지만 반대의 경우라면 그렇지 못하므로
	거품의 진행 방향은 begin쪽에서 end쪽, 그리고 end쪽에서 begin쪽을 번갈아가며 뽀글거려야 할 것이다.

	3번의 경우 거품정렬은 교환연산이 많이 일어난다. 그래서 대안으로 인서트 정렬도 나쁘진 않을 것 같지만
	3번의 경우보단 1, 2번의 경우가 압도적으로 많을 것 같아 그냥 거품정렬이 더 좋을 것 같다.
	(현재 이 함수를 만들고 있는 TCPFighter프로젝트에서는 sort가 호출되는 시점의 99%이상이 1, 2번의 경우이다.)
	*/
template <typename PredType>
	void sort(PredType Pred)
	{
		//정렬을 하려면 노드가 최소 2개는 있어야함
		if (m_size < 2) return;


		S_NODE* pFront = m_pHead->pNext;
		S_NODE* pEnd = m_pTail->pPrev;

		S_NODE* pLeft = pFront;
		S_NODE* pRight = pEnd;

		bool bNormalDir = true;


		//정렬 시작
		while (1)
		{
			//정렬은 이미 되어있다고 가정
			bool bDone = true;


			//거품 발생 위치 지정
			if (bNormalDir)
			{
				pLeft = pFront;
				pRight = pFront->pNext;
			}
			else
			{
				pLeft = pEnd->pPrev;
				pRight = pEnd;
			}


			//거품 일으키기 시작
			while (1)
			{
				//경계선에 닿으면 거품이동 완료
				if (bNormalDir && pEnd == pLeft)			break;
				else if (!bNormalDir && pFront == pRight)	break;


				bool bFrontIsChanged = false;
				bool bEndIsChanged = false;


				//정렬된 상태가 아니라면 교환
				if (!Pred(pLeft->data, pRight->data))
				{
					if (pFront == pLeft)
						bFrontIsChanged = true;
					if (pEnd == pRight)
						bEndIsChanged = true;


					//Left 뜯어내기
					pLeft->pPrev->pNext = pRight;
					pRight->pPrev = pLeft->pPrev;


					//Left 끼워넣기
					pLeft->pPrev = pRight;
					pLeft->pNext = pRight->pNext;

					pRight->pNext->pPrev = pLeft;
					pRight->pNext = pLeft;


					//Left, Right 다시 설정
					S_NODE* pTemp = pLeft;
					pLeft = pRight;
					pRight = pTemp;


					//Front, End 바뀌었다면 다시 설정
					if (bFrontIsChanged)
						pFront = pLeft;
					if (bEndIsChanged)
						pEnd = pRight;


					//한번이라도 교환작업이 일어났다면 정렬 안 되어 있는 것으로 간주
					bDone = false;
				}


				//다음 거품으로 이동
				if (bNormalDir)
				{
					pLeft = pRight;
					pRight = pRight->pNext;
				}
				else
				{
					pRight = pLeft;
					pLeft = pLeft->pPrev;
				}
			}


			//거품이동이 완료될 때마다 경계선 좁혀나가기
			if (bNormalDir)
				pEnd = pEnd->pPrev;
			else
				pFront = pFront->pNext;


			//정렬이 끝났다면 종료하고 그렇지 않다면 거품일으킬 방향 바꾸기
			if (pFront == pEnd || bDone)
				break;
			else
				bNormalDir = !bNormalDir;
		}
	}


public:
	size_t size() const { return m_size; }
	bool empty() const { return (nullptr == m_pHead) ? true : false; }
	void clear()
	{
		for (iterator iter = begin(); iter != end(); )
			iter = erase(iter);

		m_size = 0;
	}


public:
	list()
	{
		m_pHead = nullptr;
		m_pTail = nullptr;

		m_size = 0;

		m_refCount = 0;
	}
	list(const list<DataType>& lt)
	{
		m_pHead = lt.m_pHead;
		m_pTail = lt.m_pTail;

		m_size = lt.m_size;

		m_refCount = lt.m_refCount + 1;
	}

	~list()
	{
		if (0 == m_refCount)
			clear();
	}
};