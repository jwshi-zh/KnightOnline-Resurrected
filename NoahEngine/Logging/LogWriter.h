#pragma once

#include <string>

class CLogWriter  
{
protected:
	static std::string s_szFileName;

public:
	static void Open(const std::string& szFN);
	static void Close();
	static void Write(const char* lpszFormat, ...);
	
	CLogWriter();
	virtual ~CLogWriter();
};
