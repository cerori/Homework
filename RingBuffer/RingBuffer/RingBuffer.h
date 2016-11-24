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
		_readPos = _writePos = 0;
		_useSize = _freeSize = 0;
	}
	~RingBuffer(void);
	int Enqueue(char *in, int size);
	int Dequeue(char *out, int size);
	int DequeueWrap(char *outData, int outDataSize, int isMoveReadPos = TRUE);
	int GetUseSize(void);
	int GetFreeSize(void);
	int Peek(char *out, int size);
	int	GetNotBrokenEnqueueSize(void);
	int	GetNotBrokenDequeueSize(void);
	char *GetBufferPtr(void);
	char *GetReadBufferPtr(void);
	char *GetWriteBufferPtr(void);
	void RemoveData(int iSize);
	int	MoveWritePos(int iSize);
	void ClearBuffer(void);

private:
	char *_data;
	int _readPos, _writePos;
	int _useSize, _freeSize;
	const int _max_size = 100;
};

#endif