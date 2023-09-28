#include "pch.h"

#include "SubProcPerTrade.h"
#include "GameProcedure.h"
#include "GameProcMain.h"
#include "PlayerMySelf.h"
#include "PlayerOtherMgr.h"

#include "UIManager.h"
#include "UIMessageBox.h"

#include "UIInventory.h"
#include "UIDroppedItemDlg.h"

#include "PacketDef.h"
#include "APISocket.h"

#include "UIPerTradeDlg.h"
#include "UITradeEditDlg.h"

#include "N3UIIcon.h"
#include "UIHotKeyDlg.h"
#include "UISkillTreeDlg.h"

#include "N3UIString.h"
#include "N3UIEdit.h"
#include "N3UIButton.h"

CSubProcPerTrade::CSubProcPerTrade()
{
	m_pUIPerTradeDlg = nullptr;
	m_pUITradeEditDlg = nullptr;
	m_ePerTradeState = PER_TRADE_STATE_NONE;
	m_iOtherID		 = -1;
	m_iGoldOffsetBackup = 0;
	m_szMsg = ""; // MessagBox key
}

CSubProcPerTrade::~CSubProcPerTrade()
{

}


// 

void CSubProcPerTrade::Release()
{
	if (m_pUIPerTradeDlg)	
	{
		m_pUIPerTradeDlg->Release();
		delete m_pUIPerTradeDlg;
		m_pUIPerTradeDlg = nullptr;
	}

	if (m_pUITradeEditDlg)
	{
		m_pUITradeEditDlg->Release();
		delete m_pUITradeEditDlg;
		m_pUITradeEditDlg = nullptr;
	}

	m_szMsg = "";// MessageBox key
}


// 

void CSubProcPerTrade::InitPerTradeDlg(CUIManager* pUIManager)
{
	const int iW = CN3Base::s_CameraData.vp.Width;
	RECT rc;

	const e_Nation eNation = s_pPlayer->m_InfoBase.eNation;		// nation....
	const __TABLE_UI_RESRC* pTbl = s_pTbl_UI->Find(eNation);

	// UIPerTradeDlg.. ^^
	m_pUIPerTradeDlg = new CUIPerTradeDlg();
	m_pUIPerTradeDlg->Init(pUIManager);
	m_pUIPerTradeDlg->m_pSubProcPerTrade = this;
	m_pUIPerTradeDlg->LoadFromFile(pTbl->szPersonalTrade);
	rc = m_pUIPerTradeDlg->GetRegion();
	m_pUIPerTradeDlg->SetPos(iW - (rc.right-rc.left), 10);
	m_pUIPerTradeDlg->SetVisible(false);	
	m_pUIPerTradeDlg->InitIconWnd(UIWND_PER_TRADE);
	m_pUIPerTradeDlg->SetUIType(UI_TYPE_ICON_MANAGER);
	m_pUIPerTradeDlg->SetState(UI_STATE_COMMON_NONE);

	// OUTradeEditDlg.. ^^
	m_pUITradeEditDlg = new CUITradeEditDlg();
	m_pUITradeEditDlg->Init(pUIManager);
	m_pUITradeEditDlg->m_pSubProcPerTrade = this;
	m_pUITradeEditDlg->LoadFromFile(pTbl->szPersonalTradeEdit);
	m_pUITradeEditDlg->SetStyle(UISTYLE_ALWAYSTOP);
	// position calculation ..
	int iXPos, iYPos;
	const int iH = CN3Base::s_CameraData.vp.Height;
	iXPos = (iW/2) - (m_pUITradeEditDlg->GetRegion().right - m_pUITradeEditDlg->GetRegion().left)/2;
	iYPos = (iH/2) - (m_pUITradeEditDlg->GetRegion().bottom - m_pUITradeEditDlg->GetRegion().top)/2;
	m_pUITradeEditDlg->SetPos(iXPos, iYPos);	
	m_pUITradeEditDlg->Close();
	m_pUITradeEditDlg->SetUIType(UI_TYPE_BASE);
	m_pUITradeEditDlg->SetState(UI_STATE_COMMON_NONE);

	// First of all, fix it with the money icon.. ^^
	m_pUITradeEditDlg->m_pArea = (CN3UIArea *)m_pUITradeEditDlg->GetChildByID("area_trade_icon");	__ASSERT(m_pUITradeEditDlg->m_pArea, "NULL UI Component!!");

	m_pUITradeEditDlg->m_pImageOfIcon = new CN3UIImage;
	m_pUITradeEditDlg->m_pImageOfIcon->Init(m_pUITradeEditDlg);
	// Money icon string search.. There is only a function to search by ID.. ㅠ.ㅠ
	__TABLE_ITEM_BASIC*	pItem = nullptr;										// Item table structure pointer..
	pItem = s_pTbl_Items_Basic->Find(dwGold);	// Get column data...

	std::string szIconFN;
	e_PartPosition ePart;
	e_PlugPosition ePlug;
	CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item
	m_pUITradeEditDlg->m_pImageOfIcon->SetTex(szIconFN);
	const float fUVAspect = (float)45.0f/(float)64.0f;
	m_pUITradeEditDlg->m_pImageOfIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
	m_pUITradeEditDlg->m_pImageOfIcon->SetRegion(m_pUITradeEditDlg->m_pArea->GetRegion());
	m_pUITradeEditDlg->m_pImageOfIcon->SetMoveRect(m_pUITradeEditDlg->m_pArea->GetRegion());	
}


// 

void CSubProcPerTrade::EnterWaitMsgFromServerStatePerTradeReq()
{
	CPlayerOther* pTarget = s_pOPMgr->UPCGetByID(s_pPlayer->m_iIDTarget, false);
	if(nullptr == pTarget) return;

	m_ePerTradeState = PER_TRADE_STATE_WAIT_FOR_REQ;

	// Show message box text..
	char szBuff[256] = "";
	std::string szFmt; ::_LoadStringFromResource(IDS_PERSONAL_TRADE_FMT_WAIT, szFmt);
	sprintf(szBuff, szFmt.c_str(), s_pPlayer->IDString().c_str(), pTarget->IDString().c_str());
	m_szMsg = CGameProcedure::MessageBoxPost(szBuff, "", MB_CANCEL, BEHAVIOR_PERSONAL_TRADE_FMT_WAIT);

	SecureCodeBegin();
}

void CSubProcPerTrade::EnterWaitMsgFromServerStatePerTradeReq(std::string szName)
{
	m_ePerTradeState = PER_TRADE_STATE_WAIT_FOR_REQ;

	// Show message box text..
	char szBuff[256] = "";
	std::string szFmt; ::_LoadStringFromResource(IDS_PERSONAL_TRADE_FMT_WAIT, szFmt);
	sprintf(szBuff, szFmt.c_str(), s_pPlayer->IDString().c_str(), szName.c_str());
	m_szMsg = CGameProcedure::MessageBoxPost(szBuff, "", MB_CANCEL, BEHAVIOR_PERSONAL_TRADE_FMT_WAIT);

	SecureCodeBegin();
}

void CSubProcPerTrade::EnterWaitMyDecisionToPerTrade(int iOtherID)			// The state in which I received an item transaction request from someone else..
{
	CPlayerOther* pTarget = s_pOPMgr->UPCGetByID(iOtherID, false);
	if(nullptr == pTarget) return;

	m_iOtherID = iOtherID;
	m_ePerTradeState = PER_TRADE_STATE_WAIT_FOR_MY_DECISION_AGREE_OR_DISAGREE;

	// Show message box text..
	char szBuff[256] = "";
	std::string szFmt; ::_LoadStringFromResource(IDS_PERSONAL_TRADE_PERMIT, szFmt);
	sprintf(szBuff, szFmt.c_str(), s_pPlayer->IDString().c_str(), pTarget->IDString().c_str());
	m_szMsg = CGameProcedure::MessageBoxPost(szBuff, "", MB_YESNO, BEHAVIOR_PERSONAL_TRADE_PERMIT);

	SecureCodeBegin();
}


// 

void CSubProcPerTrade::SecureCodeBegin() const
{
	// 1. If you are in a commercial transaction, you cannot apply for a personal transaction. -> The relevant part.. ok
	// 2. If you receive an individual transaction request during commercial transactions, reject it.. -> That part.. ok

	// 3. If the user is moving, it stops.
	if ( s_pPlayer->IsMovingNow() )	s_pPlayer->ToggleMoveMode();				// Auto advance toggle..

	// 4. Close all Icon Manager windows.
	// In the inventory window...
	if ( CGameProcedure::s_pProcMain->m_pUIInventory->IsVisible() )
		CGameProcedure::s_pProcMain->m_pUIInventory->Close();

	// In the Drop Item window...
	if ( CGameProcedure::s_pProcMain->m_pUIDroppedItemDlg->IsVisible() )
		CGameProcedure::s_pProcMain->m_pUIDroppedItemDlg->LeaveDroppedState();

	// 5. Block input.. -> That part.. ok (key input and message..)

	// 6. Clear the value of edit control in the trading window..
	CN3UIString* pStrMy = (CN3UIString* )m_pUIPerTradeDlg->GetChildByID("string_money_my");			__ASSERT(pStrMy, "NULL UI Component!!");
	CN3UIString* pStrOther = (CN3UIString* )m_pUIPerTradeDlg->GetChildByID("string_money_other");	__ASSERT(pStrOther, "NULL UI Component!!");
	pStrMy->SetString("0");
	pStrOther->SetString("0");

	// 7. Chuck buttons in personal trade window back to normal..
	CN3UIButton* pButtonMy = (CN3UIButton* )m_pUIPerTradeDlg->GetChildByID("btn_trade_my");			__ASSERT(pButtonMy, "NULL UI Component!!");
	pButtonMy->SetState(UI_STATE_BUTTON_NORMAL);
	CN3UIButton* pButtonOther = (CN3UIButton* )m_pUIPerTradeDlg->GetChildByID("btn_trade_other");	__ASSERT(pButtonOther, "NULL UI Component!!");
	pButtonOther->SetState(UI_STATE_BUTTON_NORMAL);

	// 8. The counterparty transaction button cannot be clicked. uif itself function..
}


// 

void CSubProcPerTrade::FinalizePerTrade()
{
	m_ePerTradeState = PER_TRADE_STATE_NONE;

	if ( m_pUIPerTradeDlg->IsVisible() )
		m_pUIPerTradeDlg->SetVisible(false);

	CGameProcedure::MessageBoxClose(m_szMsg);
	m_szMsg = "";

	if ( m_pUITradeEditDlg->IsVisible() )
	{
		m_pUITradeEditDlg->SetQuantity(0);
		m_pUITradeEditDlg->Close();
	}
}

void CSubProcPerTrade::PerTradeCompleteSuccess()						// Personal transaction final success..
{
	// Deleting items in the personal trade window..
	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )
	{
		if (m_pUIPerTradeDlg->m_pPerTradeMy[i] != nullptr)	
		{
			__IconItemSkill* spItem;
			spItem = m_pUIPerTradeDlg->m_pPerTradeMy[i];

			// Removed from manager..
			m_pUIPerTradeDlg->RemoveChild(spItem->pUIIcon);

			// Remove resource...
			spItem->pUIIcon->Release();
			delete spItem->pUIIcon;
			spItem->pUIIcon = nullptr;
			delete spItem;
			spItem = nullptr;
			m_pUIPerTradeDlg->m_pPerTradeMy[i] = nullptr;
		}

		if (m_pUIPerTradeDlg->m_pPerTradeOther[i] != nullptr)	
		{
			__IconItemSkill* spItem;
			spItem = m_pUIPerTradeDlg->m_pPerTradeOther[i];

			// Removed from manager..
			m_pUIPerTradeDlg->RemoveChild(spItem->pUIIcon);

			// Remove resource...
			spItem->pUIIcon->Release();
			delete spItem->pUIIcon;
			spItem->pUIIcon = nullptr;
			delete spItem;
			spItem = nullptr;
			m_pUIPerTradeDlg->m_pPerTradeOther[i] = nullptr;
		}
	}

	m_pUIPerTradeDlg->LeavePerTradeState();
}

void CSubProcPerTrade::PerTradeCompleteCancel()							// Cancellation of personal transaction..
{
	std::string str;
	int iGold,			// The value of the trading window..
		iMyMoney;		// Inventory value...

	if ( (int)m_ePerTradeState >= (int)PER_TRADE_STATE_NORMAL )
	{
		// Check the money first.
		// Get the current money in the transaction window.
		CN3UIString* pStrMy = (CN3UIString* )m_pUIPerTradeDlg->GetChildByID("string_money_my");		__ASSERT(pStrMy, "NULL UI Component!!");
		str = pStrMy->GetString();
		iGold = atoi(str.c_str());

		// I'm getting my money now...
		iMyMoney = s_pPlayer->m_InfoExt.iGold;
		// recover money...
		iMyMoney += iGold;
		s_pPlayer->m_InfoExt.iGold = iMyMoney;

		// Then check the item...
		// items as they were.
		for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )
		{
			if (m_pUIPerTradeDlg->m_pPerTradeMy[i] != nullptr)	
			{
				if( (m_pUIPerTradeDlg->m_pPerTradeMy[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE) ||  
					(m_pUIPerTradeDlg->m_pPerTradeMy[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE_SMALL) )
				{
					// If it's an item like a bow or potion...
					__IconItemSkill* spItem;
					spItem = m_pUIPerTradeDlg->m_pPerTradeMy[i];
					
					// If you already have an icon...
					if ( m_pUIPerTradeDlg->m_pPerTradeInv[m_pUIPerTradeDlg->m_iBackupiOrder[i]] )
					{
						// Updating the numbers...
						m_pUIPerTradeDlg->m_pPerTradeInv[m_pUIPerTradeDlg->m_iBackupiOrder[i]]->iCount 
							+= m_pUIPerTradeDlg->m_pPerTradeMy[i]->iCount;

						// Removed from manager..
						m_pUIPerTradeDlg->RemoveChild(spItem->pUIIcon);

						// Remove resource...
						spItem->pUIIcon->Release();
						delete spItem->pUIIcon;
						spItem->pUIIcon = nullptr;
						delete spItem;
						spItem = nullptr;
						m_pUIPerTradeDlg->m_pPerTradeMy[i] = nullptr;
					}
					// If not, move on.
					else
					{
						__IconItemSkill* spItem;
						spItem = m_pUIPerTradeDlg->m_pPerTradeMy[i];

						spItem->pUIIcon->SetParent(m_pUIPerTradeDlg);

						// Set it in the inventory window.
						m_pUIPerTradeDlg->m_pPerTradeInv[m_pUIPerTradeDlg->m_iBackupiOrder[i]] = spItem;

						// Clear in my trading window..
						m_pUIPerTradeDlg->m_pPerTradeMy[i] = nullptr;

						CN3UIArea* pArea;

						pArea = m_pUIPerTradeDlg->GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_INV, m_pUIPerTradeDlg->m_iBackupiOrder[i]);
						if ( pArea )
						{
							spItem->pUIIcon->SetRegion(pArea->GetRegion());
							spItem->pUIIcon->SetMoveRect(pArea->GetRegion());
						}
					}
				}
				else
				{
					__IconItemSkill* spItem;
					spItem = m_pUIPerTradeDlg->m_pPerTradeMy[i];

					spItem->pUIIcon->SetParent(m_pUIPerTradeDlg);

					// Set it in the inventory window.
					m_pUIPerTradeDlg->m_pPerTradeInv[m_pUIPerTradeDlg->m_iBackupiOrder[i]] = spItem;

					// Clear in my trading window..
					m_pUIPerTradeDlg->m_pPerTradeMy[i] = nullptr;

					CN3UIArea* pArea;

					pArea = m_pUIPerTradeDlg->GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_INV, m_pUIPerTradeDlg->m_iBackupiOrder[i]);
					if ( pArea )
					{
						spItem->pUIIcon->SetRegion(pArea->GetRegion());
						spItem->pUIIcon->SetMoveRect(pArea->GetRegion());
					}
				}
			}

			if (m_pUIPerTradeDlg->m_pPerTradeOther[i] != nullptr)	
			{
				__IconItemSkill* spItem;
				spItem = m_pUIPerTradeDlg->m_pPerTradeOther[i];

				// Removed from manager..
				m_pUIPerTradeDlg->RemoveChild(spItem->pUIIcon);

				// Remove resource...
				spItem->pUIIcon->Release();
				delete spItem->pUIIcon;
				spItem->pUIIcon = nullptr;
				delete spItem;
				spItem = nullptr;
				m_pUIPerTradeDlg->m_pPerTradeOther[i] = nullptr;
			}
		}

		m_pUIPerTradeDlg->LeavePerTradeState();
	}
}


// 

void CSubProcPerTrade::LeavePerTradeState(e_PerTradeResultCode ePTRC)	// Defines the condition in which an item transaction is cancelled.
{
	BYTE byBuff[4];											// Packet buffer...
	int iOffset=0;											// Packet Offset...

	char szBuf[256] = "";
	std::string szMsg; 

	switch ( ePTRC )
	{
		case PER_TRADE_RESULT_MY_CANCEL:								// I, who applied for a transaction, canceled the transaction request..
			// Creates and sends packets to the server.
			CAPISocket::MP_AddByte(byBuff, iOffset, N3_PER_TRADE);			
			CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PER_TRADE_CANCEL);
			CGameProcedure::s_pSocket->Send(byBuff, iOffset);								// sent...

			// back finish...
			PerTradeCompleteCancel();
			FinalizePerTrade();
			TRACE("I cancel the transaction. The other party and I are cancelled.. Sent to the server..\n");
			break;

		case PER_TRADE_RESULT_MY_DISAGREE:								// I, who received the transaction request, canceled the transaction request..
			// Creates and sends packets to the server.
			CAPISocket::MP_AddByte(byBuff, iOffset, N3_PER_TRADE);			
			CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PER_TRADE_AGREE);		
			CAPISocket::MP_AddByte(byBuff, iOffset, 0x00);		

			CGameProcedure::s_pSocket->Send(byBuff, iOffset);								// sent...

			// back finish...
			FinalizePerTrade();
			TRACE("I declined the transaction.. Canceled by the other party and I Send to the server..\n");
			break;

		case PER_TRADE_RESULT_OTHER_DISAGREE:							// The counterparty who received the transaction request cancels the transaction request.
			TRACE("The other party declines the transaction.. \n");
			// this_ui
			// Show message box text..
			::_LoadStringFromResource(IDS_OTHER_PER_TRADE_ID_NO, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			// ::_LoadStringFromResource(IDS_OTHER_PER_TRADE_NO, szMsg);
			// sprintf(szBuf, szMsg.c_str(),
			// (s_pOPMgr->UPCGetByID(s_pPlayer->m_iIDTarget, false))->IDString().c_str());
			// CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffff3b3b);
			// back finish...
			FinalizePerTrade();
			break;
	}
}

void CSubProcPerTrade::ProcessProceed(e_PerTradeProceedCode ePTPC)		// Defines the state in which item trading continues.
{
	BYTE byBuff[4];											// Packet buffer...
	int iOffset=0;											// Packet Offset...

	switch ( ePTPC )
	{
		case PER_TRADE_RESULT_MY_AGREE:									// I, who received the transaction request, accepted the transaction request..
			// Creates and sends packets to the server.
			CAPISocket::MP_AddByte(byBuff, iOffset, N3_PER_TRADE);			
			CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PER_TRADE_AGREE);		
			CAPISocket::MP_AddByte(byBuff, iOffset, 0x01);		

			CGameProcedure::s_pSocket->Send(byBuff, iOffset);				// sent...

			CGameProcedure::MessageBoxClose(m_szMsg);
			m_szMsg = "";

			// Trade Core Start..
			PerTradeCoreStart();
			break;

		case PER_TRADE_RESULT_OTHER_AGREE:								// The other party to whom the transaction was requested approves the transaction request.
			CGameProcedure::MessageBoxClose(m_szMsg);
			m_szMsg = "";

			// Trade Core Start..
			PerTradeCoreStart();
			break;	
	}
}


// 

void CSubProcPerTrade::PerTradeCoreStart()
{
	m_ePerTradeState = PER_TRADE_STATE_NORMAL;

	m_pUIPerTradeDlg->SetVisible(true);
	m_pUIPerTradeDlg->EnterPerTradeState();

	if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->IsVisible())
		CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->Close();
}

void CSubProcPerTrade::PerTradeCoreInvDisable()
{
	const RECT rect = { 0, 0, 0 ,0 };

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pUIPerTradeDlg->m_pPerTradeInv[i] != nullptr)
			m_pUIPerTradeDlg->m_pPerTradeInv[i]->pUIIcon->SetMoveRect(rect);
	}
}

// 

void CSubProcPerTrade::RequestItemCountEdit()
{
	if ( m_ePerTradeState != PER_TRADE_STATE_NORMAL ) return;
	m_ePerTradeState = PER_TRARE_STATE_EDITTING;

	m_pUITradeEditDlg->Open(true);
	m_pUIPerTradeDlg->PlayGoldSound();
}

void CSubProcPerTrade::ItemCountEditOK()
{
	char szGold[32];
	std::string str;
	int iGold,			// The value of the trading window..
		iGoldOffset,	// Value in edit window..
		iMyMoney;		// Inventory value...

	// Get the current money in the transaction window.
	CN3UIString* pStrMy = (CN3UIString* )m_pUIPerTradeDlg->GetChildByID("string_money_my");	 __ASSERT(pStrMy, "NULL UI Component!!");
	str = pStrMy->GetString();
	iGold = atoi(str.c_str());

	// by getting the value of the input window
	iGoldOffset = m_pUITradeEditDlg->GetQuantity();

	// Gold Offset Backup..
	m_iGoldOffsetBackup = iGoldOffset;

	// I'm getting my money now...
	iMyMoney = s_pPlayer->m_InfoExt.iGold;

	if ( iGoldOffset <= 0 ) return;
	if ( iGoldOffset > iMyMoney ) return;

	// reduce money.
	iMyMoney -= iGoldOffset;
	s_pPlayer->m_InfoExt.iGold = iMyMoney;

	// Show Money.. Inventory..
	sprintf(szGold, "%d", iMyMoney);
	CN3UIString* pString = nullptr;
	pString = (CN3UIString* )CGameProcedure::s_pProcMain->m_pUIInventory->GetChildByID("text_gold"); __ASSERT(pString, "NULL UI Component!!");
	if(pString)	pString->SetString(szGold);	
	if(m_pUIPerTradeDlg->m_pStrMyGold) m_pUIPerTradeDlg->m_pStrMyGold->SetString(szGold);

	// Show money.. Personal trading window..
	iGold += iGoldOffset;
	sprintf(szGold, "%d", iGold);
	pStrMy->SetString(szGold);

	// send to the server.
	BYTE byBuff[16];											// Packet buffer...
	int iOffset=0;											// Packet Offset...

	// Creates and sends packets to the server.
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_PER_TRADE);			
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PER_TRADE_ADD);		
	CAPISocket::MP_AddByte(byBuff, iOffset, 0xff);		
	CAPISocket::MP_AddDword(byBuff, iOffset, dwGold);		
	CAPISocket::MP_AddDword(byBuff, iOffset, iGoldOffset);		

	CGameProcedure::s_pSocket->Send(byBuff, iOffset);			// sent...

	m_pUITradeEditDlg->SetQuantity(0);

	// Change the state... close the window...
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer = true;
	m_ePerTradeState = PER_TRADE_STATE_NORMAL;
	m_ePerTradeItemKindBackup = PER_TRADE_ITEM_MONEY;
	m_pUITradeEditDlg->Close();

	m_pUIPerTradeDlg->PlayGoldSound();
}

void CSubProcPerTrade::ItemCountEditCancel()
{
	m_pUITradeEditDlg->SetQuantity(0);

	// Change the state... close the window...
	m_ePerTradeState = PER_TRADE_STATE_NORMAL;
	m_pUITradeEditDlg->Close();

	m_pUIPerTradeDlg->PlayGoldSound();
}


// 

void CSubProcPerTrade::PerTradeMyDecision()							// I decided to make a deal...
{
	const std::string szFN = "btn_trade_my";
	CN3UIButton* pButton;
	pButton = (CN3UIButton* )m_pUIPerTradeDlg->GetChildButtonByName(szFN);

	// send to the server.
	BYTE byBuff[4];											// Packet buffer...
	int iOffset=0;											// Packet Offset...

	// Creates and sends packets to the server.
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_PER_TRADE);			
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PER_TRADE_DECIDE);		

	CGameProcedure::s_pSocket->Send(byBuff, iOffset);			// sent...

	// My decision button Disable..
	if(pButton) pButton->SetState(UI_STATE_BUTTON_DISABLE);
	SecureJobStuffByMyDecision();
}

void CSubProcPerTrade::SecureJobStuffByMyDecision()
{
	m_ePerTradeState = PER_TRADE_STATE_MY_TRADE_DECISION_DONE;

	// 1. Inventory area icon movement prevention..
	PerTradeCoreInvDisable();

	// 2. Prevent clicking on the money input window.. That part..

	// 3. Prohibition of placing buttons.. The relevant part..

}

void CSubProcPerTrade::PerTradeOtherDecision()						// Someone else decided to make a deal.
{
	CN3UIButton* pButtonOther = (CN3UIButton* )m_pUIPerTradeDlg->GetChildByID("btn_trade_other");	 __ASSERT(pButtonOther, "NULL UI Component!!");
	pButtonOther->SetState(UI_STATE_BUTTON_DISABLE);
}


// 

void CSubProcPerTrade::ReceiveMsgPerTradeReq(int iOtherID)
{
	EnterWaitMyDecisionToPerTrade(iOtherID);							// The state in which I received an item transaction request from someone else..
}

void CSubProcPerTrade::ReceiveMsgPerTradeAgree(BYTE bResult)
{
	switch ( bResult )
	{
		case 0x01:														// success..
			ProcessProceed(PER_TRADE_RESULT_OTHER_AGREE);
			break;

		case 0x00:														// failure..
			LeavePerTradeState(PER_TRADE_RESULT_OTHER_DISAGREE);
			break;
	}
}

void CSubProcPerTrade::ReceiveMsgPerTradeAdd(BYTE bResult)
{
	// Change the state... close the window...
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer = false;

	char szGold[32];
	std::string str;
	int iGold,			// The value of the trading window..
		iMyMoney;		// Inventory value...
	
	switch ( bResult )
	{
		case 0x01:
			break;

		case 0x00:			// In case of failure.. Recover..
			// If the item type you worked on before was money...
			switch ( m_ePerTradeItemKindBackup )
			{
				case PER_TRADE_ITEM_MONEY:
					{
						// Get the current money in the transaction window.
						CN3UIString* pStrMy = (CN3UIString* )m_pUIPerTradeDlg->GetChildByID("string_money_my");	 __ASSERT(pStrMy, "NULL UI Component!!");
						str = pStrMy->GetString();
						iGold = atoi(str.c_str());

						// I'm getting my money now...
						iMyMoney = s_pPlayer->m_InfoExt.iGold;
						// increase money.
						iMyMoney += m_iGoldOffsetBackup;
						s_pPlayer->m_InfoExt.iGold = iMyMoney;

						// Show Money.. Inventory..
						sprintf(szGold, "%d", iMyMoney);
						CN3UIString* pString = nullptr;
						pString = (CN3UIString* )CGameProcedure::s_pProcMain->m_pUIInventory->GetChildByID("text_gold"); __ASSERT(pString, "NULL UI Component!!");
						if(pString)	pString->SetString(szGold);	
						if(m_pUIPerTradeDlg->m_pStrMyGold) m_pUIPerTradeDlg->m_pStrMyGold->SetString(szGold);

						// Show money.. Personal trading window..
						iGold -= m_iGoldOffsetBackup;
						sprintf(szGold, "%d", iGold);
						pStrMy->SetString(szGold);
					}
					break;

				case PER_TRADE_ITEM_OTHER:
					{
						// If the item type you worked on before is an item..
						if( (m_pUIPerTradeDlg->m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->
							pItemBasic->byContable == UIITEM_TYPE_COUNTABLE) ||
							(m_pUIPerTradeDlg->m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->
							pItemBasic->byContable == UIITEM_TYPE_COUNTABLE_SMALL) )
						{
							// If it's an item like a bow or potion...
							bool bFound = false;

							// If the item is missing in the inventory.. create it..
							if ( !m_pUIPerTradeDlg->m_pPerTradeInv[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] )
							{
								bFound = true;
								// Creates in the inventory and determines the number of items.
								__IconItemSkill *spItem, *spItemNew = nullptr;
								spItem = m_pUIPerTradeDlg->m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder];

								// If there is no icon, set the number to 0 to create an icon.
								spItemNew				= new __IconItemSkill;
								spItemNew->pItemBasic	= spItem->pItemBasic;
								spItemNew->pItemExt		= spItem->pItemExt;
								spItemNew->szIconFN		= spItem->szIconFN; // Copy icon filename..
								spItemNew->iCount		= m_pUIPerTradeDlg->m_iBackupiCount;
								spItemNew->iDurability	= spItem->iDurability;

								// Creating an icon resource...
								spItemNew->pUIIcon		= new CN3UIIcon;
								const float fUVAspect			= (float)45.0f/(float)64.0f;
								spItemNew->pUIIcon->Init(m_pUIPerTradeDlg); 
								spItemNew->pUIIcon->SetTex(spItemNew->szIconFN);
								spItemNew->pUIIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
								spItemNew->pUIIcon->SetUIType(UI_TYPE_ICON);
								spItemNew->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
								spItemNew->pUIIcon->SetVisible(true);
								CN3UIArea* pArea;
								pArea = m_pUIPerTradeDlg->GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_INV, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder);
								if ( pArea )
								{
									spItemNew->pUIIcon->SetRegion(pArea->GetRegion());
									spItemNew->pUIIcon->SetMoveRect(pArea->GetRegion());
								}

								m_pUIPerTradeDlg->m_pPerTradeInv[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = spItemNew;

								// If there are items in my trading window .. Update the number..
								if ( m_pUIPerTradeDlg->m_iBackupiCount != 
									m_pUIPerTradeDlg->m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->iCount )
								{
									m_pUIPerTradeDlg->m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->iCount
										-= m_pUIPerTradeDlg->m_iBackupiCount;
								}
							}

							// If the icon appears in the slot of my trading window.. Delete it..
							if ( m_pUIPerTradeDlg->m_iBackupiCount == 
								m_pUIPerTradeDlg->m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->iCount )
							{
								bFound = true;

								__IconItemSkill* spItem;
								spItem = m_pUIPerTradeDlg->m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder];

								// Removed from manager..
								m_pUIPerTradeDlg->RemoveChild(spItem->pUIIcon);

								// Remove resource...
								spItem->pUIIcon->Release();
								delete spItem->pUIIcon;
								spItem->pUIIcon = nullptr;
								delete spItem;
								spItem = nullptr;
								m_pUIPerTradeDlg->m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = nullptr;

								// If there is an item in the inventory.. Update the number..
								if ( m_pUIPerTradeDlg->m_pPerTradeInv[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] )
								{
									m_pUIPerTradeDlg->m_pPerTradeInv[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder]->iCount
										+= m_pUIPerTradeDlg->m_iBackupiCount;
								}
							}

							// If the number is not corrected above, only the number is corrected.
							if ( !bFound)
							{
								// It determines the number of items in my trading window and inventory.
								m_pUIPerTradeDlg->m_pPerTradeInv[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder]->iCount
									+= m_pUIPerTradeDlg->m_iBackupiCount;

								m_pUIPerTradeDlg->m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->iCount
									-= m_pUIPerTradeDlg->m_iBackupiCount;
							}
						}
						else
						{
							__IconItemSkill* spItem;
							spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource;
							spItem->pUIIcon->SetParent(m_pUIPerTradeDlg);

							// Set it in the inventory window.
							m_pUIPerTradeDlg->m_pPerTradeInv[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = spItem;

							// Clear in my trading window..
							m_pUIPerTradeDlg->m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = nullptr;

							CN3UIArea* pArea;
							pArea = m_pUIPerTradeDlg->GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_INV, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder);
							if ( pArea )
							{
								spItem->pUIIcon->SetRegion(pArea->GetRegion());
								spItem->pUIIcon->SetMoveRect(pArea->GetRegion());
							}
						}
					}
					break;
			}
			break;
	}
}

void CSubProcPerTrade::ReceiveMsgPerTradeOtherAdd(int iItemID, int iCount, int iDurability)
{
	char szGold[32];
	std::string str;
	int iGold, iDestiOrder;			// The value of the trading window..

	if ( iItemID == dwGold )
	{
		// Get another person's current money from the transaction window.
		CN3UIString* pStrOther = (CN3UIString* )m_pUIPerTradeDlg->GetChildByID("string_money_other");	 __ASSERT(pStrOther, "NULL UI Component!!");
		str = pStrOther->GetString();
		iGold = atoi(str.c_str());
		
		// After adding the money...
		iGold += iCount;

		// Show money.. Personal trading window..
		sprintf(szGold, "%d", iGold);
		pStrOther->SetString(szGold);
	}
	else
	{
		// Check if the item can fit, calculate the space the item will fit in..
		bool  bFound = false;

		// Create and add items.
		__TABLE_ITEM_BASIC*	pItem = nullptr;
		__TABLE_ITEM_EXT*	pItemExt = nullptr;
		pItem = s_pTbl_Items_Basic->Find(iItemID/1000*1000);	// Get column data...
		if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
			pItemExt = CGameBase::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iItemID%1000);
		if(nullptr == pItem || nullptr == pItemExt)
		{
			__ASSERT(0, "not in item pointer table!!");
			return;
		}

		if( (pItem->byContable == UIITEM_TYPE_COUNTABLE) || (pItem->byContable == UIITEM_TYPE_COUNTABLE_SMALL) )
		{
			int i;

			for(i = 0; i < MAX_ITEM_PER_TRADE; i++ )
			{
				if( (m_pUIPerTradeDlg->m_pPerTradeOther[i]) && (m_pUIPerTradeDlg->m_pPerTradeOther[i]->pItemBasic->dwID == pItem->dwID) )
				{
					bFound = true;
					iDestiOrder = i;
					break;
				}
			}

			// If you can't find it...
			if ( !bFound )
			{
				// Find an empty inventory slot.
				for(i = 0; i < MAX_ITEM_PER_TRADE; i++ )
				{
					if ( !m_pUIPerTradeDlg->m_pPerTradeOther[i] )
					{
						bFound = true;
						iDestiOrder = i;
						break;
					}
				}

				if ( !bFound )	// If you can't find an empty slot...
					return;
			}

			if ( m_pUIPerTradeDlg->m_pPerTradeOther[iDestiOrder] )	// If there is an icon in that location...
			{
				m_pUIPerTradeDlg->m_pPerTradeOther[iDestiOrder]->iCount += iCount;
			}
			else
			{
				std::string szIconFN;
				e_PartPosition ePart;
				e_PlugPosition ePlug;
				CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item

				__IconItemSkill* spItem;

				spItem				= new __IconItemSkill;
				spItem->pItemBasic	= pItem;
				spItem->pItemExt	= pItemExt;
				spItem->szIconFN	= szIconFN; // Copy icon filename..
				spItem->iCount		= iCount;
				spItem->iDurability = iDurability;
				const float fUVAspect		= (float)45.0f/(float)64.0f;
				spItem->pUIIcon		= new CN3UIIcon;
				spItem->pUIIcon->Init(m_pUIPerTradeDlg); 
				spItem->pUIIcon->SetTex(szIconFN);
				spItem->pUIIcon->SetUVRect(0,0,fUVAspect,fUVAspect);
				spItem->pUIIcon->SetUIType(UI_TYPE_ICON);
				spItem->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
				spItem->pUIIcon->SetVisible(true);

				const CN3UIArea* pArea = nullptr;
				pArea = m_pUIPerTradeDlg->GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_OTHER, i);
				if ( pArea )
				{
					spItem->pUIIcon->SetRegion(pArea->GetRegion());
					// can't move...
					const RECT rect = { 0, 0, 0, 0 };
					spItem->pUIIcon->SetMoveRect(rect);
				}

				m_pUIPerTradeDlg->m_pPerTradeOther[i] = spItem;
			}
		}
		else
		{
			int i;

			for(i = 0; i < MAX_ITEM_PER_TRADE; i++ )
			{
				if (m_pUIPerTradeDlg->m_pPerTradeOther[i] == nullptr)	
				{
					bFound = true;
					break;
				}
			}

			if ( !bFound )	return;	// I couldn't find it, so I failed...

			std::string szIconFN;
			e_PartPosition ePart;
			e_PlugPosition ePlug;
			CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item

			__IconItemSkill* spItem;

			spItem				= new __IconItemSkill;
			spItem->pItemBasic	= pItem;
			spItem->pItemExt	= pItemExt;
			spItem->szIconFN	= szIconFN; // Copy icon filename..
			spItem->iCount		= 1;
			spItem->iDurability = iDurability;
			const float fUVAspect		= (float)45.0f/(float)64.0f;
			spItem->pUIIcon		= new CN3UIIcon;
			spItem->pUIIcon->Init(m_pUIPerTradeDlg); 
			spItem->pUIIcon->SetTex(szIconFN);
			spItem->pUIIcon->SetUVRect(0,0,fUVAspect,fUVAspect);
			spItem->pUIIcon->SetUIType(UI_TYPE_ICON);
			spItem->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
			spItem->pUIIcon->SetVisible(true);

			const CN3UIArea* pArea = nullptr;
			pArea = m_pUIPerTradeDlg->GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_OTHER, i);
			if ( pArea )
			{
				spItem->pUIIcon->SetRegion(pArea->GetRegion());
				// can't move...
				const RECT rect = { 0, 0, 0, 0 };
				spItem->pUIIcon->SetMoveRect(rect);
			}

			if ( spItem->iDurability == 0 )
				spItem->pUIIcon->SetStyle(spItem->pUIIcon->GetStyle() | UISTYLE_DURABILITY_EXHAUST);	

			m_pUIPerTradeDlg->m_pPerTradeOther[i] = spItem;
		}
	}
}

void CSubProcPerTrade::ReceiveMsgPerTradeOtherDecide()
{
	PerTradeOtherDecision();
}

void CSubProcPerTrade::ReceiveMsgPerTradeDoneSuccessBegin(int iTotalGold)
{
	char szGold[32];
	sprintf(szGold, "%d", iTotalGold);

	CN3UIString* pString = nullptr;
	pString = (CN3UIString* )CGameProcedure::s_pProcMain->m_pUIInventory->GetChildByID("text_gold"); __ASSERT(pString, "NULL UI Component!!");
	pString->SetString(szGold);	

	s_pPlayer->m_InfoExt.iGold = iTotalGold;
}

void CSubProcPerTrade::ReceiveMsgPerTradeDoneItemMove(BYTE bItemPos, int iItemID, int iCount, int iDurability)
{
	__IconItemSkill* spItem;

	// Added to item inventory INV area..
	// Check if the item can fit..
	if ( m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos] != nullptr)
	{
		if ( iItemID != (m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos]->pItemBasic->dwID/1000*1000)+
			(m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos]->pItemExt->dwID%1000) )
		{
			// Clear the existing icon..
			spItem =  m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos];

			// Removed from manager..
			m_pUIPerTradeDlg->RemoveChild(spItem->pUIIcon);

			// Remove resource...
			spItem->pUIIcon->Release();
			delete spItem->pUIIcon;
			spItem->pUIIcon = nullptr;
			delete spItem;
			spItem = nullptr;
			m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos] = nullptr;

			// Create an icon with the ID given by the server.
			goto Make_Icon;	
		}

		switch (m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos]->pItemBasic->byContable)
		{
			case UIITEM_TYPE_COUNTABLE:						 // Kind of like an arrow.
				m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos]->iCount += iCount;
				if (m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos]->iCount > UIITEM_COUNT_MANY)
					m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos]->iCount = UIITEM_COUNT_MANY;
				return;
				break;

			case UIITEM_TYPE_COUNTABLE_SMALL:			// Kind of like a potion...
				m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos]->iCount += iCount;
				if (m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos]->iCount > UIITEM_COUNT_FEW)
					m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos]->iCount = UIITEM_COUNT_FEW;
				return;
				break;
		}		
	}

Make_Icon:

	// Create and add items.
	__TABLE_ITEM_BASIC*	pItem = nullptr;
	__TABLE_ITEM_EXT*	pItemExt = nullptr;
	pItem = s_pTbl_Items_Basic->Find(iItemID/1000*1000);	// Get column data...
	if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
		pItemExt = CGameBase::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iItemID%1000);
	if(nullptr == pItem || nullptr == pItemExt)
	{
		__ASSERT(0, "not in item pointer table!!");
		return;
	}

	std::string szIconFN;
	e_PartPosition ePart;
	e_PlugPosition ePlug;
	CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item

	spItem				= new __IconItemSkill;
	spItem->pItemBasic	= pItem;
	spItem->pItemExt	= pItemExt;
	spItem->szIconFN	= szIconFN; // Copy icon filename..
	spItem->iCount		= iCount;
	spItem->iDurability = iDurability;

	const float fUVAspect		= (float)45.0f/(float)64.0f;
	spItem->pUIIcon		= new CN3UIIcon;
	spItem->pUIIcon->Init(m_pUIPerTradeDlg); 
	spItem->pUIIcon->SetTex(szIconFN);
	spItem->pUIIcon->SetUVRect(0,0,fUVAspect,fUVAspect);
	spItem->pUIIcon->SetUIType(UI_TYPE_ICON);
	spItem->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
	spItem->pUIIcon->SetVisible(true);

	const CN3UIArea* pArea = nullptr;
	pArea = m_pUIPerTradeDlg->GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_INV, bItemPos);
	if ( pArea )
	{
		spItem->pUIIcon->SetRegion(pArea->GetRegion());
		spItem->pUIIcon->SetMoveRect(pArea->GetRegion());
	}

	if ( spItem->iDurability == 0 )
		spItem->pUIIcon->SetStyle(spItem->pUIIcon->GetStyle() | UISTYLE_DURABILITY_EXHAUST);	

	m_pUIPerTradeDlg->m_pPerTradeInv[bItemPos] = spItem;	
}

void CSubProcPerTrade::ReceiveMsgPerTradeDoneSuccessEnd()
{
	// back finish...
	PerTradeCompleteSuccess();
	FinalizePerTrade();

	if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
	if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();
}

void CSubProcPerTrade::ReceiveMsgPerTradeDoneFail()
{
	char szBuf[256] = "";
	std::string szMsg; 

	// Show message box text..
	if (s_pOPMgr->UPCGetByID(m_iOtherID, false) != nullptr)
	{
		::_LoadStringFromResource(IDS_PER_TRADE_FAIL, szMsg);
		sprintf(szBuf, szMsg.c_str());			
		CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffffffff);

		// Show message box text..
		::_LoadStringFromResource(IDS_ITEM_TOOMANY_OR_HEAVY, szMsg);
		CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
	}

	// back finish...
	PerTradeCompleteCancel();
	FinalizePerTrade();
}

void CSubProcPerTrade::ReceiveMsgPerTradeCancel()
{
	char szBuf[256] = "";
	std::string szMsg; 

	// Show message box text..
	if (s_pOPMgr->UPCGetByID(m_iOtherID, false) != nullptr)
	{
		::_LoadStringFromResource(IDS_OTHER_PER_TRADE_CANCEL, szMsg);
		sprintf(szBuf, szMsg.c_str(), 
			(s_pOPMgr->UPCGetByID(m_iOtherID, false))->IDString().c_str());
		CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffff3b3b);
	}

	// back finish...
	PerTradeCompleteCancel();
	FinalizePerTrade();
}

void CSubProcPerTrade::ItemCountOK()
{
	m_pUIPerTradeDlg->ItemCountOK();
}

void CSubProcPerTrade::ItemCountCancel()
{
	m_pUIPerTradeDlg->ItemCountCancel();
}
