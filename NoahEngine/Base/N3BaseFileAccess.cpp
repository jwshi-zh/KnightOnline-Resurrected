#include "pch.h"
#include "N3BaseFileAccess.h"
#include <vector>

CN3BaseFileAccess::CN3BaseFileAccess()
{
	m_dwType |= OBJ_BASE_FILEACCESS;
	m_szFileName = "";
	m_iLOD = 0;
}

CN3BaseFileAccess::~CN3BaseFileAccess()
{
}

void CN3BaseFileAccess::Release()
{
	m_szFileName = "";
	m_iLOD = 0;
	CN3Base::Release();
}

void CN3BaseFileAccess::FileNameSet(const std::string& szFileName)
{
	std::string szTmpFN = szFileName;

	if(!szTmpFN.empty()) CharLower(&(szTmpFN[0]));
	const int iPos = szTmpFN.find(s_szPath);
	if(iPos >= 0) m_szFileName = szTmpFN.substr(s_szPath.size());
	else m_szFileName = szTmpFN;
}

bool CN3BaseFileAccess::Load(HANDLE hFile)
{
	m_szName = "";

	DWORD dwRWC = 0;
	int nL = 0;
	ReadFile(hFile, &nL, 4, &dwRWC, nullptr);
	if(nL > 0) 
	{
		std::vector<char> buffer(nL+1, NULL);
		ReadFile(hFile, buffer.data(), nL, &dwRWC, nullptr);
		m_szName = buffer.data();
	}

	return true;
}

bool CN3BaseFileAccess::LoadFromFile()
{
	if(m_szFileName.size() <= 0)
	{
#ifdef _N3GAME
		CLogWriter::Write("Can't open file (read)");
#endif
		return false;
	}

	std::string szFullPath;
	if(-1 != m_szFileName.find(':') || -1 != m_szFileName.find("\\\\") || -1 != m_szFileName.find("//"))
	{
		szFullPath = m_szFileName;
	}
	else
	{
		if(NULL != s_szPath.size() > 0) szFullPath = s_szPath;
		szFullPath += m_szFileName;
	}

	DWORD dwRWC = 0;
	const HANDLE hFile = ::CreateFile(szFullPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if(INVALID_HANDLE_VALUE == hFile)
	{
		std::string szErr = szFullPath + " - Can't open file (read)";
#ifdef _N3TOOL
		MessageBox(::GetActiveWindow(), szErr.c_str(), "File Handle error", MB_OK);
#endif
#ifdef _N3GAME 
		CLogWriter::Write(szErr.c_str());
#endif
		return false;
	}

	const bool bSuccess =	this->Load(hFile);

	CloseHandle(hFile);

	return bSuccess;
}

bool CN3BaseFileAccess::LoadFromFile(const std::string& szFileName)
{
	this->FileNameSet(szFileName);
	return this->LoadFromFile();
}

bool CN3BaseFileAccess::SaveToFile()
{
	if(m_szFileName.size() <= 0)
	{
		const std::string szErr = m_szName + " Can't open file (write) - NULL String";
		MessageBox(::GetActiveWindow(), szErr.c_str(), "File Open Error", MB_OK);
		return false;
	}

	std::string szFullPath;
	if(-1 != m_szFileName.find(':') || -1 != m_szFileName.find("\\\\") || -1 != m_szFileName.find("//"))
	{
		szFullPath = m_szFileName;
	}
	else
	{
		if(s_szPath.size() > 0) szFullPath = s_szPath;
		szFullPath += m_szFileName;
	}

	DWORD dwRWC = 0;
	const HANDLE hFile = ::CreateFile(szFullPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		const std::string szErr = szFullPath + " - Can't open file(write)";
		MessageBox(::GetActiveWindow(), szErr.c_str(), "File Handle error", MB_OK);
		return false;
	}

	this->Save(hFile);

	CloseHandle(hFile);
	return true;
}

bool CN3BaseFileAccess::SaveToFile(const std::string& szFileName)
{
	this->FileNameSet(szFileName);
	return this->SaveToFile();
}

bool CN3BaseFileAccess::Save(HANDLE hFile)
{
	DWORD dwRWC = 0;

	const int nL = m_szName.size();
	WriteFile(hFile, &nL, 4, &dwRWC, nullptr);
	if(nL > 0) WriteFile(hFile, m_szName.c_str(), nL, &dwRWC, nullptr);

	return true;
}
