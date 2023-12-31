#include "pch.h"
#include <stdio.h>
#include "LogWriter.h"

std::string CLogWriter::s_szFileName = "";

CLogWriter::CLogWriter()
{
}

CLogWriter::~CLogWriter()
{
}

void CLogWriter::Open(const std::string& szFN)
{
	if(szFN.empty()) return;

	s_szFileName = szFN;

	HANDLE hFile = CreateFile(s_szFileName.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		hFile = CreateFile(s_szFileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(INVALID_HANDLE_VALUE == hFile) return;
	}

	DWORD dwSizeHigh = 0;
const DWORD dwSizeLow = ::GetFileSize(hFile, &dwSizeHigh);
	if(dwSizeLow > 256000)  // If the file size is too large, delete it.
	{
		CloseHandle(hFile);
		::DeleteFile(s_szFileName.c_str());
		hFile = CreateFile(s_szFileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(INVALID_HANDLE_VALUE == hFile) return;
	}

	::SetFilePointer(hFile, 0, nullptr, FILE_END);

	char szBuff[1024];
	SYSTEMTIME time;
	GetLocalTime(&time);
	DWORD dwRWC = 0;

	sprintf(szBuff, "---------------------------------------------------------------------------\r\n", time.wMonth, time.wDay, time.wHour, time.wMinute);
	int iLength = lstrlen(szBuff);
	WriteFile(hFile, szBuff, iLength, &dwRWC, nullptr);

	sprintf(szBuff, "// Begin writing log... [%.2d/%.2d %.2d:%.2d]\r\n", time.wMonth, time.wDay, time.wHour, time.wMinute);
	iLength = lstrlen(szBuff);
	WriteFile(hFile, szBuff, iLength, &dwRWC, nullptr);

	CloseHandle(hFile);
}

void CLogWriter::Close()
{
	HANDLE hFile = CreateFile(s_szFileName.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		hFile = CreateFile(s_szFileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(INVALID_HANDLE_VALUE == hFile) hFile = nullptr;
	}

	if(hFile)
	{
		::SetFilePointer(hFile, 0, nullptr, FILE_END);

		char szBuff[1024];
		SYSTEMTIME time;
		GetLocalTime(&time);
		DWORD dwRWC = 0;

		sprintf(szBuff, "// End writing log... [%.2d/%.2d %.2d:%.2d]\r\n", time.wMonth, time.wDay, time.wHour, time.wMinute);
		int iLength = lstrlen(szBuff);
		WriteFile(hFile, szBuff, iLength, &dwRWC, nullptr);

		sprintf(szBuff, "---------------------------------------------------------------------------\r\n", time.wMonth, time.wDay, time.wHour, time.wMinute);
		iLength = lstrlen(szBuff);
		WriteFile(hFile, szBuff, iLength, &dwRWC, nullptr);

		CloseHandle(hFile);
		hFile = nullptr;
	}
}

void CLogWriter::Write(const char *lpszFormat, ...)
{
	if(s_szFileName.empty() || nullptr == lpszFormat) return;

	static char szFinal[1024];
	static SYSTEMTIME time;
	GetLocalTime(&time);
	szFinal[0] = NULL;

	DWORD dwRWC = 0;
	sprintf(szFinal, "    [%.2d:%.2d:%.2d] ", time.wHour, time.wMinute, time.wSecond);

	static char szBuff[1024];
	szBuff[0] = NULL;
	va_list argList;
	va_start(argList, lpszFormat);
	vsprintf(szBuff, lpszFormat, argList);
	va_end(argList);

	lstrcat(szFinal, szBuff);
	lstrcat(szFinal, "\r\n");
	const int iLength = lstrlen(szFinal);

	HANDLE hFile = CreateFile(s_szFileName.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		hFile = CreateFile(s_szFileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(INVALID_HANDLE_VALUE == hFile) hFile = nullptr;
	}


	if(hFile)
	{
		::SetFilePointer(hFile, 0, nullptr, FILE_END);

		WriteFile(hFile, szFinal, iLength, &dwRWC, nullptr);
		CloseHandle(hFile);
	}
}
