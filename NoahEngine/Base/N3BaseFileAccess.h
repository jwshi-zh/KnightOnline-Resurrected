#pragma once

#include "N3Base.h"
#include <string>

class CN3BaseFileAccess : public CN3Base
{
protected:
	std::string		m_szFileName;

public:
	int m_iLOD;

public:
	const std::string& FileName() const { return m_szFileName; }
	void FileNameSet(const std::string& szFileName);

	bool LoadFromFile();
	virtual bool LoadFromFile(const std::string& szFileName);
	virtual bool Load(HANDLE hFile);

	virtual bool SaveToFile();
	virtual bool SaveToFile(const std::string& szFileName);
	virtual bool Save(HANDLE hFile);

public:
	void Release();

	CN3BaseFileAccess();
	virtual ~CN3BaseFileAccess();
};
