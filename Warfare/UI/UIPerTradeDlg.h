#pragma once

#include "N3UIBase.h"
#include "N3UIArea.h"
#include "GameProcedure.h"

#include "GameDef.h"
#include "N3UIWndBase.h"

class CGameProcMain;

class CUIPerTradeDlg    : public CN3UIWndBase  
{
	friend class CUIInventory;

public:
	CSubProcPerTrade*	m_pSubProcPerTrade;

	// Object reference pointer to be accessed directly
	__IconItemSkill*		m_pPerTradeMy[MAX_ITEM_PER_TRADE];
	__IconItemSkill*		m_pPerTradeOther[MAX_ITEM_PER_TRADE];
	__IconItemSkill*		m_pPerTradeInv[MAX_ITEM_INVENTORY];

	int						m_iBackupiOrder[MAX_ITEM_PER_TRADE];

	CUIImageTooltipDlg*		m_pUITooltipDlg;
	int						m_iBackupiCount;	// The number of arrows moved before..

	CN3UIString*			m_pStrMyGold;

protected:
	int					GetItemiOrder(__IconItemSkill* spItem, e_UIWND_DISTRICT eWndDist);
	RECT				GetSampleRect();

	void				ItemMoveFromInvToThis();
	void				ItemMoveFromThisToInv();

public:
	CUIPerTradeDlg();
	virtual ~CUIPerTradeDlg();
	void				Release();

	virtual DWORD		MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual bool		ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);
	void				Render();
	void				LeavePerTradeState();
	void				EnterPerTradeState();

	void				InitIconWnd(e_UIWND eWnd);	
	void				InitIconUpdate();

	__IconItemSkill*	GetHighlightIconItem(CN3UIIcon* pUIIcon);

	void				IconRestore();	

	bool				ReceiveIconDrop(__IconItemSkill* spItem, POINT ptCur);

	void				CancelIconDrop(__IconItemSkill* spItem);
	void				AcceptIconDrop(__IconItemSkill* spItem);

	void				SendToServerItemAddMsg(byte pos, int itemID, int iCount);

	CN3UIBase*			GetChildButtonByName(const std::string& szFN);

	// Item Count OK..
	void				ItemCountOK();
	void				ItemCountCancel();

	void				GoldUpdate() const;
};
