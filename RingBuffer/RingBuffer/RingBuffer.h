#ifndef __RING_BUFFER__
#define __RING_BUFFER__

#include <Windows.h>

class RingBuffer
{
public:
	RingBuffer(void);
	RingBuffer(int buffSize) : _max_size(buffSize)
	{
		_data = new char[_max_size];
		_read = _write = _data;
		_useSize = _freeSize = 0;
	}
	~RingBuffer(void);
	BOOL Enqueue(char *in, int size);
	BOOL Dequeue(char *out, int size);
	int GetUseSize(void);
	int GetFreeSize(void);
	int Peek(char *out, int size);

private:
	char *_data;
	char *_read, *_write;
	int _useSize, _freeSize;
	const int _max_size = 100;
};

#endif