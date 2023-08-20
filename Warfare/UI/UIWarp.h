#pragma once

#include <string>
#include <list>
#include "N3UIBase.h"

struct __WarpInfo
{
	int iID; // Send this number to the server.
	std::string szName;
	std::string szAgreement; // An article that informs users and asks for their consent.
	int iZone;
	int iMaxUser; // Maximum number of people limited..
	int iGold;
	__Vector3 vPos;
};

class CUIWarp : public CN3UIBase  
{
protected:
	class CN3UIButton*		m_pBtn_Ok;
	class CN3UIButton*		m_pBtn_Cancel;
	
	class CN3UIList*		m_pList_Infos;
	class CN3UIString*		m_pText_Agreement; // Agreement...

	std::list<__WarpInfo> m_ListInfos;

public:
	bool OnKeyPress(int iKey);
	void SetVisible(bool bVisible);
	void Reset();
	CUIWarp();
	virtual ~CUIWarp();

	void	InfoAdd(const __WarpInfo& ZW);
	int		InfoGetCur(__WarpInfo& ZW);
	void	UpdateList();
	void	UpdateAgreement();

	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);
};
