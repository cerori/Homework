#include "stdafx.h"
#include "RingBuffer.h"
#include "FileLog.h"

RingBuffer::RingBuffer(void)
{
	_data = new char[_max_size];
	_readPos = _writePos = 0;
	_useSize = _freeSize = 0;
}

RingBuffer::~RingBuffer(void)
{
	delete[] _data;
}

int RingBuffer::Enqueue(char *inData, int inDataSize)
{
	int rearSpareSize;
	int write = (_writePos + 1) % _max_size;

	if (inDataSize == 0)
		return 0;

	// �������� ����
	if (this->GetFreeSize() == 0)
		return 0;

	if (this->GetFreeSize() < inDataSize)
	{
		//return FALSE;
		inDataSize = this->GetFreeSize();
	}

	// ���� ������ ������� ���� ������ ����� ������ 
	// �ι��� ������ memcpy �۾��Ѵ�. 
	rearSpareSize = this->GetNotBrokenEnqueueSize();
	if (rearSpareSize >= inDataSize)
	{
		memcpy(_data + _writePos, inData, inDataSize);
	}
	else
	{
		memcpy(_data + _writePos, inData, rearSpareSize);
		memcpy(_data, inData + rearSpareSize, inDataSize - rearSpareSize);
	}
		
	// ������� ����
	_useSize += inDataSize;

	// ���� ��ġ �̵�
	_writePos = (_writePos + inDataSize) % _max_size;

	return inDataSize;
}

int RingBuffer::Dequeue(char *outData, int outDataSize)
{
	return this->DequeueWrap(outData, outDataSize);
}

int RingBuffer::DequeueWrap(char *outData, int outDataSize, int isMoveReadPos)
{
	int rearUseSize, frontSize;

	if (outDataSize == 0)
		return 0;

	if (this->GetUseSize() == 0)
		return FALSE;

	// �бⰡ ���� �ڿ� �ְ� ��û ������ ����� ���� ��ġ���� ���� ���
	if (_writePos == (_readPos + outDataSize))
	{
		memcpy(outData, _data + _readPos, outDataSize);
	}
	else if (_readPos < _writePos && (_readPos + outDataSize) < _writePos)
	{
		memcpy(outData, _data + _readPos, outDataSize);
	}
	else if (_readPos < _writePos && (_readPos + outDataSize) > _writePos)
	{
		frontSize = _writePos - _readPos;
		memcpy(outData, _data + _readPos, frontSize);
		outDataSize = frontSize;
	}
	else if (_readPos > _writePos && (_readPos + outDataSize) < _max_size)
	{
		memcpy(outData, _data + _readPos, outDataSize);
	}
	else
	{
		frontSize = _max_size - _readPos;

		memcpy(outData, _data + _readPos, frontSize);
		if (_writePos > outDataSize - frontSize)
		{
			memcpy(outData + frontSize, _data, outDataSize - frontSize);
		}
		else
		{
			memcpy(outData + frontSize, _data, _writePos);
			outDataSize = (_writePos + frontSize);
		}
	}

	if (isMoveReadPos)
	{
		// ������� ����
		_useSize -= outDataSize;

		// �б� ��ġ �̵�
		_readPos = (_readPos + outDataSize) % _max_size;
	}

	return outDataSize;
}

int RingBuffer::GetUseSize(void)
{
	return _useSize;
}

int RingBuffer::GetFreeSize(void)
{
	return (_max_size - 1) - this->GetUseSize();
}

int RingBuffer::Peek(char *outData, int outDataSize)
{
	return this->DequeueWrap(outData, outDataSize, FALSE);
}

int RingBuffer::GetNotBrokenEnqueueSize(void)
{
	int rearFreeSize, maxSize;

	maxSize = _max_size - 1;

	if (_writePos == 0)
	{
		rearFreeSize = maxSize;
	}
	else if (_readPos == _writePos)
	{
		rearFreeSize = _max_size - _writePos;
	}
	else if (_readPos < _writePos)
	{
		rearFreeSize = maxSize - _writePos;
	}
	else
	{
		rearFreeSize = _readPos - _writePos;
	}

	return rearFreeSize;
}

int RingBuffer::GetNotBrokenDequeueSize(void)
{
	int rearUseSize;
	int maxSize;

	maxSize = _max_size - 1;

	if (_readPos == _writePos)
	{
		rearUseSize = 0;
	}
	else if (_readPos >= _writePos)
	{
		rearUseSize = _max_size - _readPos;
	}
	else
	{
		rearUseSize = _writePos - _readPos;
	}

	return rearUseSize;
}

char * RingBuffer::GetBufferPtr(void)
{
	return _data;
}

char * RingBuffer::GetReadBufferPtr(void)
{
	return _data + _readPos;
}

char * RingBuffer::GetWriteBufferPtr(void)
{
	return _data + _writePos;
}

void RingBuffer::RemoveData(int iSize)
{

}

int RingBuffer::MoveWritePos(int iSize)
{
	_writePos = (_writePos + iSize - 1) % _max_size;

	return _writePos;
}

void RingBuffer::ClearBuffer(void)
{
	_readPos = _writePos = _useSize = _freeSize = 0;
}
