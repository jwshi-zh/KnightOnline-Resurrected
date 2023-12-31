#include "pch.h"

#include "PacketDef.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "GameProcMain.h"

#include "UIImageTooltipDlg.h"
#include "UIInventory.h"
#include "UITradeEditDlg.h"

#include "SubProcPerTrade.h"
#include "UIPerTradeDlg.h"

#include "N3UIEdit.h"

CUITradeEditDlg::CUITradeEditDlg()
{
	m_pSubProcPerTrade = nullptr;
	m_pArea = nullptr;
	m_pImageOfIcon = nullptr;
}

CUITradeEditDlg::~CUITradeEditDlg()
{

}

// 

void CUITradeEditDlg::Release()
{
	CN3UIBase::Release();
}

int	CUITradeEditDlg::GetQuantity() // Get integer value from "edit_trade" Edit Control.
{
	CN3UIEdit* pEdit = (CN3UIEdit*)this->GetChildByID("edit_trade");
	__ASSERT(pEdit, "NULL UI Component!!");

	return atoi(pEdit->GetString().c_str());
}

void CUITradeEditDlg::SetQuantity(int iQuantity) // In the "edit_trade" Edit Control, set the integer value as a string.
{
	CN3UIEdit* pEdit = (CN3UIEdit*)this->GetChildByID("edit_trade");
	__ASSERT(pEdit, "NULL UI Component!!");

	char szBuff[64] = "";
	sprintf(szBuff, "%d", iQuantity);
	pEdit->SetString(szBuff);
}

bool CUITradeEditDlg::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if(nullptr == pSender) return false;

	if (dwMsg == UIMSG_BUTTON_CLICK)					
	{
		if(pSender->m_szID == "btn_ok")
			m_pSubProcPerTrade->ItemCountEditOK();

		if(pSender->m_szID == "btn_cancel")
			m_pSubProcPerTrade->ItemCountEditCancel();
	}

	return true;
}

void CUITradeEditDlg::Open(bool bCountGold)
{
	std::string szMsg;
	if (bCountGold)
		::_LoadStringFromResource(IDS_EDIT_BOX_GOLD, szMsg);
	else
		::_LoadStringFromResource(IDS_EDIT_BOX_COUNT, szMsg);

	CN3UIString* pString = nullptr;
	pString = (CN3UIString*)this->GetChildByID("String_PersonTradeEdit_Msg");
	__ASSERT(pString, "NULL UI Component!!");
	if (pString)
		pString->SetString(szMsg);

	SetVisible(true);

	// this_ui_add_start
	CN3UIEdit* pEdit = (CN3UIEdit*)this->GetChildByID("edit_trade");
	__ASSERT(pEdit, "NULL UI Component!!");
	if(pEdit) pEdit->SetFocus();
	// this_ui_add_end

	RECT rc, rcThis;
	int iCX, iCY;

	this->SetQuantity(0);

	rc = CGameProcedure::s_pProcMain->m_pSubProcPerTrade->m_pUIPerTradeDlg->GetRegion();
	iCX = (rc.right+rc.left)/2;
	iCY = (rc.bottom+rc.top)/2;
	rcThis = GetRegion();
	SetPos(iCX-(rcThis.right-rcThis.left)/2, iCY-(rcThis.bottom-rcThis.top)/2);
}

void CUITradeEditDlg::Close()
{
	SetVisible(false);

	const CN3UIEdit* pEdit = GetFocusedEdit();
	if (pEdit) pEdit->KillFocus();
}

