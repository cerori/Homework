#include "stdafx.h"
#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include "profiling.h"

PROFILE_SAMPLE g_ProfileSample[MAX_PROFILE_SAMPLE];
PROFILE_HISTORY g_ProfileHistory[MAX_PROFILE_SAMPLE];

int g_iProfileCount;

LARGE_INTEGER g_lFrequency;

LARGE_INTEGER g_lProcessStartTime;
LARGE_INTEGER g_lFrameStartTime;

bool g_bFirestBegin = true;

void ProfileInitial(void)
{
	int iCount;

	for (iCount = 0; iCount < MAX_PROFILE_SAMPLE; iCount++)
	{
		g_ProfileSample[iCount].bFlag = FALSE;
		g_ProfileHistory[iCount].bFlag = FALSE;
	}

	g_iProfileCount = 0;

	QueryPerformanceFrequency(&g_lFrequency);
	QueryPerformanceCounter(&g_lProcessStartTime);
}

void ProfileBegin(char * szName)
{
	int iCount;
	int iParentNum = 0;
	LARGE_INTEGER lTime;

	QueryPerformanceCounter(&lTime);

	if (g_bFirestBegin)
	{
		g_lFrameStartTime = lTime;
		g_bFirestBegin = false;
	}

	for (iCount = 0; iCount < MAX_PROFILE_SAMPLE; iCount++)
	{
		if (g_ProfileSample[iCount].bFlag)
		{
			if (g_ProfileSample[iCount].iParentsNum > iParentNum &&
				g_ProfileSample[iCount].iProfileOpen > 0)
			{
				iParentNum = g_ProfileSample[iCount].iParentsNum;
			}
			else if (strcmp(g_ProfileSample[iCount].szName, szName) == 0)
			{
				g_ProfileSample[iCount].iProfileBegin++;
				g_ProfileSample[iCount].iProfileOpen++;
				g_ProfileSample[iCount].lStartTime = lTime;
				return;

			}
		}
	}

	for (iCount = 0; iCount < MAX_PROFILE_SAMPLE; iCount++)
	{
		if (!g_ProfileSample[iCount].bFlag)
		{
			strcpy_s(g_ProfileSample[iCount].szName, szName);
			g_ProfileSample[iCount].bFlag = TRUE;
			g_ProfileSample[iCount].iProfileBegin = 1;
			g_ProfileSample[iCount].iProfileOpen = 1;
			g_ProfileSample[iCount].lStartTime = lTime;
			g_ProfileSample[iCount].fAccumulatorTime = 0;
			g_ProfileSample[iCount].fChildrenTime = 0;
			g_ProfileSample[iCount].iParentsNum = iParentNum + 1;
			return;
		}
	}
}

void ProfileEnd(char * szName)
{
	int i, j;
	LARGE_INTEGER lTime;

	QueryPerformanceCounter(&lTime);

	for (i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		if (g_ProfileSample[i].bFlag)
		{
			if (strcmp(szName, g_ProfileSample[i].szName) == 0)
			{
				g_ProfileSample[i].iProfileOpen--;
				g_ProfileSample[i].fAccumulatorTime += (lTime.QuadPart - g_ProfileSample[i].lStartTime.QuadPart);

				if (g_ProfileSample[i].iParentsNum > 1)
				{
					int iParent = -1;
					LARGE_INTEGER lLately;
					lLately.QuadPart = 0x7fffffffffffffff;

					for (j = 0; j < MAX_PROFILE_SAMPLE; j++)
					{
						if (g_ProfileSample[j].bFlag &&
							g_ProfileSample[j].iProfileOpen > 0 &&
							g_ProfileSample[j].iParentsNum == g_ProfileSample[j].iParentsNum - 1 &&
							g_ProfileSample[j].lStartTime.QuadPart < lLately.QuadPart)
						{
							lLately.QuadPart = g_ProfileSample[j].lStartTime.QuadPart;
							iParent = j;
						}
					}

					if (iParent > -1)
					{
						g_ProfileSample[iParent].fChildrenTime += (lTime.QuadPart - g_ProfileSample[i].lStartTime.QuadPart);
					}
				}

				return;
			}
		}
	}
}

void ProfileDataOutText(char * szFileName)
{
	int i, j;
	char szSpace[64] = { 0, };

	PROFILE_HISTORY *pHistory;
	PROFILE_SAMPLE *pSample;

	LARGE_INTEGER lTime;
	double fRate;
	FILE *pFile;

	g_iProfileCount++;

	if (0 != fopen_s(&pFile, szFileName, "w"))
	{
		return;
	}
	QueryPerformanceCounter(&lTime);

	fprintf(pFile, " Average      |     Min      |     Max      |   F.Call / T.Call    |\n");
	fprintf(pFile, "====================================================================\n");
	pSample = g_ProfileSample;

	for (i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		if (pSample->bFlag)
		{
			if (strcmp(pSample->szName, "SLEEP") == 0)
				pSample = pSample;

			fRate = (pSample->fAccumulatorTime - pSample->fChildrenTime) /
				(double)(lTime.QuadPart - g_lFrameStartTime.QuadPart) * 100;

			StoreProfileHistory(pSample->szName, pSample->fAccumulatorTime, fRate, pSample->iProfileBegin);

			if (pHistory != NULL)
			{
				fprintf(pFile, "%9.4Lfs(%6.2f%%) | %9.4Lfs(6.2f%%) | %9.4Lfs(6.2f%%) | %6ld / %6ld |",
					(pHistory->fAverage / (double)g_lFrequency.QuadPart) / g_iProfileCount,
					pHistory->fRateAverage / g_iProfileCount,
					pHistory->fMin / (double)g_lFrequency.QuadPart,
					pHistory->fRateMin,
					pHistory->fMax / (double)g_lFrequency.QuadPart,
					pHistory->fRateMax,
					pSample->iProfileBegin, 
					pHistory->iTCall);
			}
			szSpace[0] = '\0';

			for (j = 0; j < pSample->iParentsNum; j++)
			{
				sprintf_s(szSpace, "%s  ", szSpace);
			}

			fprintf_s(pFile, "%s%s\r\n", szSpace, pSample->szName);
		}

		pSample++;
	}

	fprintf_s(pFile, "\r\n");
	fprintf_s(pFile, "Total Time : %11.2Lfs\n",
		(lTime.QuadPart - g_lProcessStartTime.QuadPart) / (double)g_lFrequency.QuadPart);
	fprintf_s(pFile, "Total Frame : %11ld \n", g_iProfileCount);
	fprintf_s(pFile, "1 Frame Time : %9.4Lfs\n",
		(lTime.QuadPart - g_lProcessStartTime.QuadPart) / (double)g_lFrequency.QuadPart / g_iProfileCount);

	for (i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		g_ProfileSample[i].bFlag = FALSE;
	}

	g_bFirestBegin = TRUE;

	fclose(pFile);
}

void StoreProfileHistory(char * szName, double fTime, double fRate, int iCall)
{
	int i;

	for (i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		if (g_ProfileHistory[i].bFlag)
		{
			if (strcmp(szName, g_ProfileHistory[i].szName) == 0)
			{
				g_ProfileHistory[i].fAverage += fTime;

				if (g_ProfileHistory[i].fMin > fTime)
					g_ProfileHistory[i].fMin = iCall;

				if (g_ProfileHistory[i].fMax < fTime)
					g_ProfileHistory[i].fMax = iCall;

				g_ProfileHistory[i].fRateAverage += fRate;

				if (g_ProfileHistory[i].fRateMin > fRate)
					g_ProfileHistory[i].fRateMin = fRate;

				if (g_ProfileHistory[i].fRateMax < fRate)
					g_ProfileHistory[i].fRateMax = iCall;

				return;
			}
		}
	}

	for (i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		if (!g_ProfileHistory[i].bFlag)
		{
			strcpy_s(g_ProfileHistory[i].szName, szName);
			g_ProfileHistory[i].bFlag = TRUE;
			g_ProfileHistory[i].fAverage = fTime;
			g_ProfileHistory[i].fMin = fTime;
			g_ProfileHistory[i].fMax = fTime;

			g_ProfileHistory[i].fRateAverage = fRate;
			g_ProfileHistory[i].fRateMin = fRate;
			g_ProfileHistory[i].fRateMax = fRate;

			g_ProfileHistory[i].iTCall = iCall;

			return;
		}
	}
}

PROFILE_HISTORY * GetProfileHistory(char * szName)
{
	int i;

	for (i = 0; i < MAX_PROFILE_SAMPLE; i++)
	{
		if (g_ProfileHistory[i].bFlag)
		{
			if (strcmp(szName, g_ProfileHistory[i].szName) == 0)
				return &g_ProfileHistory[i];
		}
	}

	return NULL;
}
