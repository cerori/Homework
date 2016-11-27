#ifndef __PROFILING__
#define __PROFILING__

#include <Windows.h>

typedef struct
{
	bool bFlag;
	int iProfileBegin;
	int iProfileOpen;

	char szName[64];
	LARGE_INTEGER lStartTime;
	double fAccumulatorTime;
	double fChildrenTime;
	int iParentsNum;
} PROFILE_SAMPLE;

typedef struct
{
	bool bFlag;
	char szName[64];

	double fAverage;
	double fMin;
	double fMax;
	
	double fRateAverage;
	double fRateMin;
	double fRateMax;

	__int64 iTCall;
} PROFILE_HISTORY;

#define MAX_PROFILE_SAMPLE	100

void ProfileInitial(void);
void ProfileBegin(char *szName);
void ProfileEnd(char *szName);
void ProfileDataOutText(char *szFileName);
void StoreProfileHistory(char *szName, double fTime, double fRate, int iCall);

PROFILE_HISTORY *GetProfileHistory(char *szName);


#endif