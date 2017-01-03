#pragma once
/*
�� ���� ���� ��
- �ִ��� ���� STL�� �Ȱ��� ¥�� ����(��� �� ������ ��ŭ�̶�) ��ǥ�� �Ͽ� ����.
- "my_list.h"�� ����ؼ� ������� ��� ���α׷��鿡 ���� ���߿� <list>�� ��ü�ϴ���
  �ƹ��� ���� ���� �Ȱ��� �۵��ϴ� ������������ ��ǥ�� �ؼ� ����� ������.
  (���� ���������� ������� ����. �ʹ� ����.)
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

		//������� Ŭ������ �⺻ �����ڰ� �ƿ� ���� ��쿡�� �����ϰ� ��ó�ϱ� ���� �׻� �Ʒ� ����ü �����ڸ� ��ġ�� ��.
		//��������ڴ� �� � ���� ���¿����� ������ �����Ѵٴ� ���� �̿��� ���.
		S_NODE(const DataType& copyData) : data(copyData) {}
	};

	//�켱, STL�� end()�� rend()�� �ݺ��ڿ� ����Ǿ� �۵��Ǵ� ����� �䳻���� ���ؼ� ���̹���� ä���Ͽ���.
	//�׸��� �Ʒ� ���� ������ �����ͺ����̱� ������ ���̳�带 ����Ű�� ���� ����� ���̴�.
	//���̸� ����Ű�� ������ ����� ä���� ������ �Ϲ� ��������� ���̸� ����� ���� ����� ������ �߻��ϱ� �����̴�.
	//���� ���縦 ���� �������->pPrev�� ���������->pNext�� �ǵ���� �ذ��Ѵ��ص�... ���� ����� �ʾ�����
	//���߿� ��Ƽ������ ȯ�濡�� �ݵ�� ������ �߻��� �����̵��. �ƹ�ư �׷��� �̷��� ���̿� �����ͺ����� �α�� �ߴ�.
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
		//���� ������ ������� ���� ���ε��� �ұ��ϰ� copyData�� ���� �����ϰ� �ִ�.
		//�ֳ��ϸ� DataType�� Ŭ������ ��� �װ��� �⺻ �����ڰ� ���� ���� �ֱ� �����̴�.
		//�ݸ� ��������ڴ� ������ �����Ѵ�. �� ���� �̿��� ���̴�.
		//���� ���� ����ü�� ���� ������ �� �ݺ���Ŭ������ ������� Ÿ���� void*�� �ٲٰ�
		//�װ��� �� ����Լ��� �ȿ��� ��Ȳ�� ���� ������ ĳ�����ش�� ����� �ְ�����
		//�׳� �Ʒ��� ���� ����� �ξ� �� �����ϰ� �����ϴٰ� �����Ѵ�.
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
			throw "list iterator not incrementable";	//�� �ݺ��ڸ� ������ų �� ���ٴ� �޼����� ����� �𸣰�����...

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
	�����غôµ�, ���ӿ��� ��ǰ������ �ٸ� ���ĵ麸�� ���� ��찡 ���� �� ����.

	1. �ֳ��ϸ� �̹� ������ �Ǿ� �ִ� ��찡 �ٹݻ��̰�
	2. ������ Ʋ�������� ������ ��峢���� Ʋ�����ų�
	3. �ϳ��� ��常 �ٸ� ��򰡷� �̵��ϴ� ��찡 ��κ��̱� �����̴�.
	(�׷����� ���� ���� ��Ȳ���� �ƴ϶�� ��ǰ�� ���� ���� ���̴�.....)

	3������ ��򰡷� �̵��ؾ� �� ��尡 begin�ʿ��� end������ ���� �����
	��ǰ�� ������ �����ϰ����� �ݴ��� ����� �׷��� ���ϹǷ�
	��ǰ�� ���� ������ begin�ʿ��� end��, �׸��� end�ʿ��� begin���� �����ư��� �Ǳ۰ŷ��� �� ���̴�.

	3���� ��� ��ǰ������ ��ȯ������ ���� �Ͼ��. �׷��� ������� �μ�Ʈ ���ĵ� ������ ���� �� ������
	3���� ��캸�� 1, 2���� ��찡 �е������� ���� �� ���� �׳� ��ǰ������ �� ���� �� ����.
	(���� �� �Լ��� ����� �ִ� TCPFighter������Ʈ������ sort�� ȣ��Ǵ� ������ 99%�̻��� 1, 2���� ����̴�.)
	*/
template <typename PredType>
	void sort(PredType Pred)
	{
		//������ �Ϸ��� ��尡 �ּ� 2���� �־����
		if (m_size < 2) return;


		S_NODE* pFront = m_pHead->pNext;
		S_NODE* pEnd = m_pTail->pPrev;

		S_NODE* pLeft = pFront;
		S_NODE* pRight = pEnd;

		bool bNormalDir = true;


		//���� ����
		while (1)
		{
			//������ �̹� �Ǿ��ִٰ� ����
			bool bDone = true;


			//��ǰ �߻� ��ġ ����
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


			//��ǰ ����Ű�� ����
			while (1)
			{
				//��輱�� ������ ��ǰ�̵� �Ϸ�
				if (bNormalDir && pEnd == pLeft)			break;
				else if (!bNormalDir && pFront == pRight)	break;


				bool bFrontIsChanged = false;
				bool bEndIsChanged = false;


				//���ĵ� ���°� �ƴ϶�� ��ȯ
				if (!Pred(pLeft->data, pRight->data))
				{
					if (pFront == pLeft)
						bFrontIsChanged = true;
					if (pEnd == pRight)
						bEndIsChanged = true;


					//Left ����
					pLeft->pPrev->pNext = pRight;
					pRight->pPrev = pLeft->pPrev;


					//Left �����ֱ�
					pLeft->pPrev = pRight;
					pLeft->pNext = pRight->pNext;

					pRight->pNext->pPrev = pLeft;
					pRight->pNext = pLeft;


					//Left, Right �ٽ� ����
					S_NODE* pTemp = pLeft;
					pLeft = pRight;
					pRight = pTemp;


					//Front, End �ٲ���ٸ� �ٽ� ����
					if (bFrontIsChanged)
						pFront = pLeft;
					if (bEndIsChanged)
						pEnd = pRight;


					//�ѹ��̶� ��ȯ�۾��� �Ͼ�ٸ� ���� �� �Ǿ� �ִ� ������ ����
					bDone = false;
				}


				//���� ��ǰ���� �̵�
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


			//��ǰ�̵��� �Ϸ�� ������ ��輱 ����������
			if (bNormalDir)
				pEnd = pEnd->pPrev;
			else
				pFront = pFront->pNext;


			//������ �����ٸ� �����ϰ� �׷��� �ʴٸ� ��ǰ����ų ���� �ٲٱ�
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