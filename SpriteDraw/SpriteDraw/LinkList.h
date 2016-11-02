#ifndef __LINKLIST__
#define __LINKLIST__

#include <Windows.h>

template<typename T>
class CList;

template<typename T>
class Node
{
	friend class CList<T>;
public:
	T data;
	Node<T> *prev;
	Node<T> *next;

	Node()
	{
		//data = prev = next = NULL; 
	}
};

template<typename T>
class CList {
private:
	Node<T> *head;
	Node<T> *tail;
	Node<T> *current;
public:
	CList<T>();
	void Add(T data);
	void MoveHead(void);
	BOOL HasNext(void);
	Node<T> *GetHead(void);
	BOOL GetNext(T *object);
};

template<typename T>
inline CList<T>::CList()
{
	head = new Node<T>();
	tail = new Node<T>();

	head->next = tail;
	tail->prev = head;

	current = head;
}

template<typename T>
inline void CList<T>::Add(T data)
{
	Node<T> *newNode = new Node<T>();

	newNode->data = data;

	newNode->prev = tail->prev->next;
	newNode->next = tail;

	tail->prev->next = newNode;
	tail->prev = newNode;
}

template<typename T>
inline void CList<T>::MoveHead(void)
{
	current = head;
}

template<typename T>
inline BOOL CList<T>::HasNext(void)
{
	if (current->next == tail)
		return FALSE;

	return TRUE;
}

template<typename T>
inline Node<T> *CList<T>::GetHead(void)
{
	return head;
}

template<typename T>
inline BOOL CList<T>::GetNext(T *object)
{
	if (current->next == tail)
		return FALSE;

	current = current->next;

	*object = current->data;

	return TRUE;
}

template<typename T>
class Iterate
{
public:
	Iterate(Node<T> *node);
	Node<T> *Current(void);
	Node<T> *Next(void);
	Node<T> *Prev(void);
	void MoveHead(void);
	void MoveAt(Node<T> *node);
	BOOL Swap(Node<T> *n1, Node<T> *n2);

protected:
	Node<T> *_head;
	Node<T> *_current;
};

template<typename T>
inline Iterate<T>::Iterate(Node<T> *node)
{
	_head = node;
	_current = _head;
}

template<typename T>
inline Node<T> *Iterate<T>::Current(void)
{
	if (_current->data == NULL)
		return NULL;

	return _current;
}

template<typename T>
inline Node<T> *Iterate<T>::Next(void)
{
	if (_current->next->data == NULL)
		return NULL;

	_current = _current->next;

	return _current;
}

template<typename T>
inline Node<T> *Iterate<T>::Prev()
{
	if (_current->prev->data == NULL)
		return NULL;

	_current = _current->prev;

	return _current;
}

template<typename T>
inline void Iterate<T>::MoveHead(void)
{
	_current = _head;
}

template<typename T>
inline void Iterate<T>::MoveAt(Node<T>* node)
{
	_current = node;
}

template<typename T>
inline BOOL Iterate<T>::Swap(Node<T> *node1, Node<T> *node2)
{
	T tmp1;

	if (node1 == NULL || node2 == NULL)
		return FALSE;

	tmp1 = node1->data;
	node1->data = node2->data;
	node2->data = tmp1;

	return TRUE;
}

#endif
