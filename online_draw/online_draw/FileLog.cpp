#include "stdafx.h"
#include "FileLog.h"

#include <fstream>
#include <stdarg.h>
#include <direct.h>
#include <Windows.h>

using namespace std;

void ClearFileLog(const char* pszFileName)
{
	DeleteFileA(pszFileName);
}

void FileLog(const char* pszFileName, const char* pszLog, ...)
{
	fstream _streamOut;
	_streamOut.open(pszFileName, ios::out | ios::app);

	va_list argList;
	char cbuffer[1024];
	va_start(argList, pszLog);
	vsnprintf(cbuffer, 1024, pszLog, argList);
	va_end(argList);

	_streamOut << cbuffer << endl;
	_streamOut.close();
}