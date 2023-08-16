#pragma once

#include "N3BaseFileAccess.h"
#include "N3FXDef.h"

#include <list>

class CN3FXGroup : public CN3BaseFileAccess  
{
public:
	int			m_iVersion;
	std::list<__FXBInfo*> FXBList;
	
public:
	int	GetCount() { return FXBList.size(); }
	__FXBInfo* GetFXBInfo(int idx);

	virtual bool	Load(HANDLE hFile); // �ڵ鿡�� �о����..
	bool	Save(HANDLE hFile);	// �����ϱ�..
	
	CN3FXGroup();	
	virtual ~CN3FXGroup();

#ifdef _N3TOOL	//�������� ���� �Լ���...
public:
	bool	DecodeScriptFile(const char* lpPathName);
#endif // end of _N3TOOL
};
