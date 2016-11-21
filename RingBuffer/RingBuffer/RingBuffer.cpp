#include "stdafx.h"
#include "RingBuffer.h"

RingBuffer::RingBuffer(void)
{
	_data = new char[_max_size];
	_read = _write = _size = 0;
}

RingBuffer::~RingBuffer(void)
{
	delete[] _data;
}

BOOL RingBuffer::Enqueue(char * in, int size)
{
	int write = (_write + 1) % _max_queue;

	// ≤À √°¿Ω
	if (write == _read)
		return FALSE;
		
	_size += size;

	// ¥Ÿ¿Ω¿∏∑Œ ¿Ãµø
	_write = write;

	return TRUE;
}

BOOL RingBuffer::Dequeue(char * out, int size)
{
	return 0;
}

int RingBuffer::GetUseSize(void)
{
	return 0;
}

int RingBuffer::GetFreeSize(void)
{
	return 0;
}

int RingBuffer::Peek(char * out, int size)
{
	return 0;
}
