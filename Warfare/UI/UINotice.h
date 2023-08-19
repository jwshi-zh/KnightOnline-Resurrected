#pragma once

#include "N3UIBase.h"
#include <list>
#include <string>

class CUINotice : public CN3UIBase  
{
public:
	class CN3UIString*		m_pText_Notice;
	class CN3UIScrollBar*	m_pScrollBar;
	class CN3UIButton*		m_pBtn_OK;

	std::list<std::string>	m_Texts;

public:
	void RemoveNotice();
	void SetVisible(bool bVisible);
	bool OnKeyPress(int iKey);
	void GenerateText();
	void Release();

	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);

	CUINotice();
	virtual ~CUINotice();

};
