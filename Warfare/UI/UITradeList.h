#pragma once

#include "GameDef.h"

#include "N3UIBase.h"
#include "N3UIButton.h"

class CUITradeList       : public CN3UIBase  
{
	CN3UIString*		m_pStr_List[9];
	std::string			m_sStr[40];
	CN3UIScrollBar*		m_pScrollbar;	// scrollbar ����������(���� m_Child�� ����)

public:
	CUITradeList();
	virtual ~CUITradeList();

	void Release();

	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);

	void Open(int iIDTarget = -1);
	void Close();

	void SetTopLine(int iTopLine);		// �� ������ �������ش�.
};
