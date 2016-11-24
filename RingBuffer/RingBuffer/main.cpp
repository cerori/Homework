// RingBuffer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "RingBuffer.h"
#include "FileLog.h"

RingBuffer *rb = new RingBuffer(203);

void PrintSize(void);

int main()
{
	int dataSize = 81, randSize, retPeekDataSize;
	BOOL retval;
	int remindSize, putSumSize, putSize;
	char *data = "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345";
	char tmp[100] = { 0, };
	char tmp1[100] = { 0, };
	char szLog[100] = { 0, };
	char logFilename[] = "log.txt";

	remindSize = putSumSize = putSize = 0;

	ClearFileLog(logFilename);

	for (int i=0; TRUE; i++)
	//while (1)
	{
		//randSize = rand() % (dataSize + 1);

		//if (rb->GetFreeSize() > 0)
		//{
		//	if (remindSize == 0)
		//	{
		//		putSize = rb->Enqueue(data, dataSize);
		//		remindSize = dataSize - putSize;
		//		printf("%d %d\n", putSize, remindSize);
		//	}
		//	else
		//	{

		//	}
		//}

		memset(tmp, 0, 100);
		memset(szLog, 0, 100);
		randSize = rand() % (dataSize + 1);

		//retPeekDataSize = rb->Peek(tmp1, randSize);
		retval = rb->DequeueWrap(tmp, randSize);
		//if (memcmp(tmp, tmp1, retPeekDataSize) != 0)
		//{
		//	printf("dequeue 와 peek 데이터가 다름\n");
		//	break;
		//}

		if (retval == FALSE)
		{
			rb->Enqueue(data, dataSize);
			continue;
			//printf("\nDequeue False.");
			//break;
		}

		//PrintSize();

		printf("%s", tmp);
		FileLog(logFilename, "%d", i);
	}

    return 0;
}

void PrintSize(void)
{
	printf("free size: %d\n", rb->GetFreeSize());
	printf("use size: %d\n", rb->GetUseSize());
	printf("GetNotBrokenEnqueueSize %d\n", rb->GetNotBrokenEnqueueSize());
	printf("GetNotBrokenDequeueSize %d\n", rb->GetNotBrokenDequeueSize());

}