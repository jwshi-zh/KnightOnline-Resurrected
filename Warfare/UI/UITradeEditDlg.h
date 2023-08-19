#pragma once

#include "N3UIBase.h"
#include "N3UIArea.h"
#include "N3UIImage.h"
#include "GameProcedure.h"

#include "GameDef.h"

class CGameProcMain;

class CUITradeEditDlg : public CN3UIBase  
{
public:
	CSubProcPerTrade*	m_pSubProcPerTrade;
	CN3UIArea*			m_pArea;
	CN3UIImage*			m_pImageOfIcon;

public:
	CUITradeEditDlg();
	virtual ~CUITradeEditDlg();

	int					GetQuantity();
	void				SetQuantity(int iQuantity); // "edit_trade" Edit Control 에서 정수값을 문자열로 세팅한다..

	virtual void		Release();
	virtual bool		ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);

	void				Open(bool bCountGold);
	void				Close();
};
