#include "pch.h"

#include "PacketDef.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "GameProcMain.h"
#include "PlayerMyself.h"
#include "N3UIWndBase.h"

#include "UIImageTooltipDlg.h"
#include "UIInventory.h"
#include "UIPerTradeDlg.h"

#include "SubProcPerTrade.h"
#include "CountableItemEditDlg.h"

#include "UIManager.h"
#include "N3UIButton.h"
#include "N3UIEdit.h"
#include "N3SndObj.h"

CUIPerTradeDlg::CUIPerTradeDlg()
{
	m_pSubProcPerTrade = nullptr;
	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )	m_pPerTradeMy[i] = nullptr;
	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )	m_pPerTradeOther[i] = nullptr;
	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )	m_iBackupiOrder[i] = -1;
	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )	m_pPerTradeInv[i] = nullptr;

	m_iBackupiCount = 0;
	m_pUITooltipDlg = nullptr;
	m_pStrMyGold    = nullptr;

	this->SetVisible(false);
}

CUIPerTradeDlg::~CUIPerTradeDlg()
{
	Release();
}

void CUIPerTradeDlg::Release()
{
	CN3UIBase::Release();

	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )
	{
		if ( m_pPerTradeMy[i] != nullptr)
		{
			delete m_pPerTradeMy[i];
			m_pPerTradeMy[i] = nullptr;
		}
	}

	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )
	{
		if ( m_pPerTradeOther[i] != nullptr)
		{
			delete m_pPerTradeOther[i];
			m_pPerTradeOther[i] = nullptr;
		}
	}

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pPerTradeInv[i] != nullptr)
		{
			delete m_pPerTradeInv[i];
			m_pPerTradeInv[i] = nullptr;
		}
	}
}

void CUIPerTradeDlg::Render()
{
	if (!m_bVisible) return;	// If not visible, don't render the children.
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
	m_pUITooltipDlg->DisplayTooltipsDisable();

	bool bTooltipRender = false;
	__IconItemSkill* spItem = nullptr;

	for(auto itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if ( (GetState() == UI_STATE_ICON_MOVING) && (pChild->UIType() == UI_TYPE_ICON) && (CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) &&
			((CN3UIIcon *)pChild == CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon) )	continue;
			pChild->Render();
		if ( (GetState() == UI_STATE_COMMON_NONE) && 
				(pChild->UIType() == UI_TYPE_ICON) && (pChild->GetStyle() & UISTYLE_ICON_HIGHLIGHT) )
		{
			bTooltipRender = true;
			spItem = GetHighlightIconItem( (CN3UIIcon* )pChild );
		}
	}

	// Displays the number of items to be displayed.
	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )
	{
		if ( m_pPerTradeMy[i] && ( (m_pPerTradeMy[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE) || (m_pPerTradeMy[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE_SMALL) ) )
		{
			// Get string...
			CN3UIString* pStr = GetChildStringByiOrder(i);
			if(pStr) 
			{
				pStr->SetVisible(true);
				pStr->SetStringAsInt(m_pPerTradeMy[i]->iCount);
				pStr->Render();
			}
		}
		else
		{
			// Get string...
			CN3UIString* pStr = GetChildStringByiOrder(i);
			if(pStr) 
				pStr->SetVisible(false);
		}
	}

	// Displays the number of items to be displayed.
	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )
	{
		if ( m_pPerTradeOther[i] && ( (m_pPerTradeOther[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE) || (m_pPerTradeOther[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE_SMALL) ) )
		{
			// Get string...
			CN3UIString* pStr = GetChildStringByiOrder(i+100);
			if(pStr) 
			{
				pStr->SetVisible(true);
				pStr->SetStringAsInt(m_pPerTradeOther[i]->iCount);
				pStr->Render();
			}
		}
		else
		{
			// Get string...
			CN3UIString* pStr = GetChildStringByiOrder(i+100);
			if(pStr) 
				pStr->SetVisible(false);
		}
	}

	// Displays the number of items to be displayed.
	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pPerTradeInv[i] && ( (m_pPerTradeInv[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE) || (m_pPerTradeInv[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE_SMALL) ) )
		{
			// Get string...
			CN3UIString* pStr = GetChildStringByiOrder(i+200);
			if(pStr) 
			{
				if ( (GetState() == UI_STATE_ICON_MOVING) && (m_pPerTradeInv[i] == CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) )
				{
					pStr->SetVisible(false);
				}
				else
				{
					if ( m_pPerTradeInv[i]->pUIIcon->IsVisible() )
					{
						pStr->SetVisible(true);
						pStr->SetStringAsInt(m_pPerTradeInv[i]->iCount);
						pStr->Render();
					}
					else
					{
						pStr->SetVisible(false);
					}
				}
			}
		}
		else
		{
			// Get string...
			CN3UIString* pStr = GetChildStringByiOrder(i+200);
			if(pStr) 
				pStr->SetVisible(false);
		}
	}

	if ( (GetState() == UI_STATE_ICON_MOVING) && (CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) )
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->Render();		

	if ( bTooltipRender && spItem )
		m_pUITooltipDlg->DisplayTooltipsEnable(ptCur.x, ptCur.y, spItem );
}

void CUIPerTradeDlg::InitIconWnd(e_UIWND eWnd)
{
	const __TABLE_UI_RESRC* pTblUI = CGameBase::s_pTbl_UI->Find(CGameBase::s_pPlayer->m_InfoBase.eNation);

	m_pUITooltipDlg = new CUIImageTooltipDlg();
	m_pUITooltipDlg->Init(this);
	m_pUITooltipDlg->LoadFromFile(pTblUI->szItemInfo);
	m_pUITooltipDlg->InitPos();
	m_pUITooltipDlg->SetVisible(FALSE);	

	CN3UIWndBase::InitIconWnd(eWnd);

	// My decision button in normal state..
	const std::string szFN = "btn_trade_my";
	CN3UIButton* pButton;
	pButton = (CN3UIButton* )GetChildButtonByName(szFN);
	if(pButton) pButton->SetState(UI_STATE_BUTTON_NORMAL);

	m_pStrMyGold    = (CN3UIString* )GetChildByID("string_money_inv"); __ASSERT(m_pStrMyGold, "NULL UI Component!!");
	if(m_pStrMyGold)
		m_pStrMyGold->SetString("0");
}

void CUIPerTradeDlg::InitIconUpdate()
{
}

__IconItemSkill* CUIPerTradeDlg::GetHighlightIconItem(CN3UIIcon* pUIIcon)
{
	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )
	{
		if ( (m_pPerTradeMy[i] != nullptr) && (m_pPerTradeMy[i]->pUIIcon == pUIIcon) )
			return m_pPerTradeMy[i];
	}

	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )
	{
		if ( (m_pPerTradeOther[i] != nullptr) && (m_pPerTradeOther[i]->pUIIcon == pUIIcon) )
			return m_pPerTradeOther[i];
	}

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( (m_pPerTradeInv[i] != nullptr) && (m_pPerTradeInv[i]->pUIIcon == pUIIcon) ) 
			return m_pPerTradeInv[i];
	}

	return nullptr;
}

void CUIPerTradeDlg::LeavePerTradeState()
{
	// variable clear..
	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )	m_iBackupiOrder[i] = -1;
	// My decision button in normal state..
	const std::string szFN = "btn_trade_my";
	CN3UIButton* pButton;
	pButton = (CN3UIButton* )GetChildButtonByName(szFN);
	if(pButton) pButton->SetState(UI_STATE_BUTTON_NORMAL);

	ItemMoveFromThisToInv();

	if (GetState() == UI_STATE_ICON_MOVING)
		IconRestore();
	SetState(UI_STATE_COMMON_NONE);
	CN3UIWndBase::AllHighLightIconFree();
}

void CUIPerTradeDlg::EnterPerTradeState()
{
	// Initialize variable...
	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )	m_iBackupiOrder[i] = -1;

	// My decision button in normal state..
	const std::string szFN = "btn_trade_my";
	CN3UIButton* pButton;
	pButton = (CN3UIButton* )GetChildButtonByName(szFN);
	if(pButton) pButton->SetState(UI_STATE_BUTTON_NORMAL);

	for(auto i = 0; i < MAX_ITEM_TRADE; i++ )
	{
		if ( m_pPerTradeMy[i] != nullptr)
		{
			if ( m_pPerTradeMy[i]->pUIIcon )
			{
				RemoveChild(m_pPerTradeMy[i]->pUIIcon);
				m_pPerTradeMy[i]->pUIIcon->Release();
				delete m_pPerTradeMy[i]->pUIIcon;
				m_pPerTradeMy[i]->pUIIcon = nullptr;
			}
			delete m_pPerTradeMy[i];	
			m_pPerTradeMy[i] = nullptr;
		}
	}

	for(auto i = 0; i < MAX_ITEM_TRADE; i++ )
	{
		if ( m_pPerTradeOther[i] != nullptr)
		{
			if ( m_pPerTradeOther[i]->pUIIcon )
			{
				RemoveChild(m_pPerTradeOther[i]->pUIIcon);
				m_pPerTradeOther[i]->pUIIcon->Release();
				delete m_pPerTradeOther[i]->pUIIcon;
				m_pPerTradeOther[i]->pUIIcon = nullptr;
			}
			delete m_pPerTradeOther[i];	
			m_pPerTradeOther[i] = nullptr;
		}
	}

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pPerTradeInv[i] != nullptr)
		{
			if ( m_pPerTradeInv[i]->pUIIcon )
			{
				RemoveChild(m_pPerTradeInv[i]->pUIIcon);
				m_pPerTradeInv[i]->pUIIcon->Release();
				delete m_pPerTradeInv[i]->pUIIcon;
				m_pPerTradeInv[i]->pUIIcon = nullptr;
			}
			delete m_pPerTradeInv[i];	
			m_pPerTradeInv[i] = nullptr;
		}
	}

	ItemMoveFromInvToThis();

	char szGold[32];
	sprintf(szGold, "%d", CGameBase::s_pPlayer->m_InfoExt.iGold);
	if(m_pStrMyGold)	m_pStrMyGold->SetString(szGold);
}

void CUIPerTradeDlg::GoldUpdate() const
{
	char szGold[32];
	sprintf(szGold, "%d", CGameBase::s_pPlayer->m_InfoExt.iGold);
	if(m_pStrMyGold)	m_pStrMyGold->SetString(szGold);
}

void CUIPerTradeDlg::ItemMoveFromInvToThis()
{
	CUIInventory* pInven = CGameProcedure::s_pProcMain->m_pUIInventory;
	if(!pInven) return;

	int i;
	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if(pInven->m_pMyInvWnd[i])
		{
			__IconItemSkill* spItem = pInven->m_pMyInvWnd[i];
			spItem->pUIIcon->SetParent(this);

			pInven->m_pMyInvWnd[i] = nullptr;
			CN3UIArea* pArea;

			pArea = GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_INV, i);
			if ( pArea )
			{
				spItem->pUIIcon->SetRegion(pArea->GetRegion());
				spItem->pUIIcon->SetMoveRect(pArea->GetRegion());
			}			

			m_pPerTradeInv[i] = spItem;
		}
	}
}

void CUIPerTradeDlg::ItemMoveFromThisToInv()
{
	CUIInventory* pInven = CGameProcedure::s_pProcMain->m_pUIInventory;
	if(!pInven) return;

	int i;
	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if(m_pPerTradeInv[i])
		{
			__IconItemSkill* spItem = m_pPerTradeInv[i];
			spItem->pUIIcon->SetParent(pInven);

			m_pPerTradeInv[i] = nullptr;

			CN3UIArea* pArea;

			pArea = pInven->GetChildAreaByiOrder(UI_AREA_TYPE_INV, i);
			if ( pArea )
			{
				spItem->pUIIcon->SetRegion(pArea->GetRegion());
				spItem->pUIIcon->SetMoveRect(pArea->GetRegion());
			}			

			pInven->m_pMyInvWnd[i] = spItem;
		}
	}
}

void CUIPerTradeDlg::ItemCountOK()
{
	const int iGold = CN3UIWndBase::m_pCountableItemEdit->GetQuantity();

	__IconItemSkill* spItem, *spItemNew = nullptr;
	spItem = m_pPerTradeInv[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder];

	if ( iGold <= 0 ) return;
	if ( iGold > spItem->iCount ) return;

	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer	= true;
	m_iBackupiCount = iGold;	// The number of arrows moved before..
	const int iOffset = spItem->iCount - iGold;

	// Sound..
	if (spItem) PlayItemSound(spItem->pItemBasic);

	if ( iOffset > 0 )
	{	
		// Number update...
		spItem->iCount = iOffset;

		// If there is an icon in that location...
		if ( m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] )
		{
			m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->iCount += iGold;
		}
	}
	else
	{
		// If there is an icon in that location...
		if ( m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] )
		{
			m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->iCount += iGold;
		}

		// Delete the icon from your inventory.
		__IconItemSkill* spItem;
		spItem = m_pPerTradeInv[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder];

		// Delete it from your inventory.
		m_pPerTradeInv[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = nullptr;

		// Delete my manager's item from the list with iOrder.
		RemoveChild(spItem->pUIIcon);

		// Delete Icon Resource...
		spItem->pUIIcon->Release();
		delete spItem->pUIIcon;
		spItem->pUIIcon = nullptr;
		delete spItem;
		spItem = nullptr;
	}

	// send to Server.
	SendToServerItemAddMsg(CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, 
		m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->pItemBasic->dwID+
		m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->pItemExt->dwID, iGold);
	CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;

	CN3UIWndBase::m_pCountableItemEdit->Close();
}

void CUIPerTradeDlg::ItemCountCancel()
{
	// Sound..
	if (CN3UIWndBase::m_sRecoveryJobInfo.pItemSource) PlayItemSound(CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic);

	for(auto i = 0; i < MAX_ITEM_PER_TRADE; i++ )
	{
		if( (m_pPerTradeMy[i]) && ( (m_pPerTradeMy[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE) || (m_pPerTradeMy[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE_SMALL) ) )
		{
			if ( m_pPerTradeMy[i]->iCount == 0 )
			{
				// Delete the icon from your inventory.
				__IconItemSkill* spItem;
				spItem = m_pPerTradeMy[i];

				// Delete it from your inventory.
				m_pPerTradeMy[i] = nullptr;

				// Delete my manager's item from the list with iOrder.
				RemoveChild(spItem->pUIIcon);

				// Delete Icon Resource...
				spItem->pUIIcon->Release();
				delete spItem->pUIIcon;
				spItem->pUIIcon = nullptr;
				delete spItem;
				spItem = nullptr;
			}
		}
	}

	// cancellation..
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= nullptr;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;

	CN3UIWndBase::m_pCountableItemEdit->Close();
}

void CUIPerTradeDlg::SendToServerItemAddMsg(byte pos, int itemID, int iCount)
{
	// send to the server.
	BYTE byBuff[16];											// Packet buffer...
	int iOffset=0;											// Packet Offset...

	// Creates and sends packets to the server.
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_PER_TRADE);			
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PER_TRADE_ADD);		
	CAPISocket::MP_AddByte(byBuff, iOffset, pos);		
	CAPISocket::MP_AddDword(byBuff, iOffset, itemID);		
	CAPISocket::MP_AddDword(byBuff, iOffset, iCount);		

	CGameProcedure::s_pSocket->Send(byBuff, iOffset);			// sent...
}

bool CUIPerTradeDlg::ReceiveIconDrop(__IconItemSkill* spItem, POINT ptCur)
{
#define FAIL_RETURN {	\
		CN3UIWndBase::AllHighLightIconFree();	\
		SetState(UI_STATE_COMMON_NONE);	\
		return false;	\
	}

	CN3UIArea* pArea;
	e_UIWND_DISTRICT eUIWnd = UIWND_DISTRICT_UNKNOWN;
	if (!m_bVisible) return false;

	// If it's not the icon I had...
	if (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd != m_eUIWnd)
		FAIL_RETURN
	if (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict != UIWND_DISTRICT_PER_TRADE_INV)
		FAIL_RETURN

	// If it's an icon I had... it checks if it's an npc area.
	int i, iDestiOrder = -1; 
	bool bFound = false;

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_MY, i);
		if ( (pArea) && (pArea->IsIn(ptCur.x, ptCur.y)) )
		{
			bFound = true;
			eUIWnd = UIWND_DISTRICT_PER_TRADE_MY;
			break;
		}
	}

	if (!bFound)	FAIL_RETURN

	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer					= true;
	m_pSubProcPerTrade->m_ePerTradeItemKindBackup						= PER_TRADE_ITEM_OTHER;

	CN3UIWndBase::m_sRecoveryJobInfo.pItemSource						= CN3UIWndBase::m_sSelectedIconInfo.pItemSelect;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWnd				= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict		= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder			= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWnd				= UIWND_PER_TRADE;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict		= UIWND_DISTRICT_PER_TRADE_MY;

	bFound = false;
	if( (CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE) || 
		(CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE_SMALL) )
	{
		// If it's an item like a bow or potion...
		// Find out if there is an icon in the face down slot.
		int i = 0;

		for(i = 0; i < MAX_ITEM_PER_TRADE; i++ )
		{
			if ( bFound )
				break;

			if( (m_pPerTradeMy[i]) && (m_pPerTradeMy[i]->pItemBasic->dwID == CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic->dwID) &&
				(m_pPerTradeMy[i]->pItemExt->dwID == CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt->dwID) )
			{
				bFound = true;
				iDestiOrder = i;
			}
		}

		// If you can't find it...
		if ( !bFound )
		{
			// Go find an empty slot.
			for(i = 0; i < MAX_ITEM_PER_TRADE; i++ )
			{
				if ( !m_pPerTradeMy[i] )
				{
					bFound = true;
					iDestiOrder = i;
					break;
				}
			}

			if ( !bFound )	// If you can't find an empty slot...
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= nullptr;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;
				FAIL_RETURN
			}
		}		

		CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder	= iDestiOrder;

		// Check if the icon is there or not.
		if ( !m_pPerTradeMy[iDestiOrder] )
		{
			__IconItemSkill *spItemNew = nullptr;
			spItem = CN3UIWndBase::m_sSelectedIconInfo.pItemSelect;

			// If there is no icon, set the number to 0 to create an icon.
			spItemNew				= new __IconItemSkill;
			spItemNew->pItemBasic	= spItem->pItemBasic;
			spItemNew->pItemExt		= spItem->pItemExt;
			spItemNew->szIconFN		= spItem->szIconFN; // Copy icon filename..
			spItemNew->iCount		= 0;
			spItemNew->iDurability	= spItem->iDurability;

			// Creating an icon resource...
			spItemNew->pUIIcon		= new CN3UIIcon;
			const float fUVAspect			= (float)45.0f/(float)64.0f;
			spItemNew->pUIIcon->Init(this); 
			spItemNew->pUIIcon->SetTex(spItemNew->szIconFN);
			spItemNew->pUIIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
			spItemNew->pUIIcon->SetUIType(UI_TYPE_ICON);
			spItemNew->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
			spItemNew->pUIIcon->SetVisible(true);
			CN3UIArea* pArea;
			pArea = GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_MY, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder);
			if ( pArea )
			{
				spItemNew->pUIIcon->SetRegion(pArea->GetRegion());
				// can't move...
				const RECT rect = { 0, 0, 0, 0 };
				spItemNew->pUIIcon->SetMoveRect(rect);
			}

			m_pPerTradeMy[iDestiOrder] = spItemNew;
		}

		m_iBackupiOrder[i] = CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder;

		// If it's an item like a bow or potion...
		CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer			= false;
		CN3UIWndBase::m_pCountableItemEdit->Open(UIWND_PER_TRADE, UIWND_DISTRICT_PER_TRADE_MY, false);
		FAIL_RETURN
	}
	else
	{
		// Check if the item can fit, calculate the space the item will fit in..
		for(i = 0; i < MAX_ITEM_PER_TRADE; i++ )
		{
			if (m_pPerTradeMy[i] == nullptr)	
			{
				bFound = true;
				break;
			}
		}

		if ( !bFound )	
		{
			CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer			= false;
			FAIL_RETURN	// I couldn't find it, so I failed...
		}

		CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder				= i;
		m_iBackupiOrder[i] = CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder;

		// Send to Server. The current number of items is 1..^^
		SendToServerItemAddMsg(m_iBackupiOrder[i], CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic->dwID+
			CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt->dwID, 1);
		CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;
	}

	// I set it up in my trading area...
	m_pPerTradeMy[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = spItem;
	// Clear in area window..
	m_pPerTradeInv[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = nullptr;

	pArea = GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_MY, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder);
	if ( pArea )
	{
		spItem->pUIIcon->SetRegion(pArea->GetRegion());
		spItem->pUIIcon->SetMoveRect(pArea->GetRegion());
	}

	CN3UIWndBase::AllHighLightIconFree();
	SetState(UI_STATE_COMMON_NONE);

	return true;
}

void CUIPerTradeDlg::CancelIconDrop(__IconItemSkill* spItem)
{
	CN3UIWndBase::AllHighLightIconFree();
	SetState(UI_STATE_COMMON_NONE);
}

void CUIPerTradeDlg::AcceptIconDrop(__IconItemSkill* spItem)
{
	CN3UIWndBase::AllHighLightIconFree();
	SetState(UI_STATE_COMMON_NONE);
}

void CUIPerTradeDlg::IconRestore()
{
	CN3UIArea* pArea;

	switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
	{
		case UIWND_DISTRICT_PER_TRADE_INV:
			if ( m_pPerTradeInv[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != nullptr)
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_INV, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
				if ( pArea )
				{
					m_pPerTradeInv[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pPerTradeInv[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
			}
			break; 
	}
}

DWORD CUIPerTradeDlg::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if ( !IsVisible() ) { dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);  return dwRet; }
	if (CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer) { dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);  return dwRet; }

	// Renew dragged icon..
	if ( (GetState() == UI_STATE_ICON_MOVING) && 
			(CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd == UIWND_PER_TRADE) )
	{
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetRegion(GetSampleRect());
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetMoveRect(GetSampleRect());
	}

	return CN3UIWndBase::MouseProc(dwFlags, ptCur, ptOld);
}

int	CUIPerTradeDlg::GetItemiOrder(__IconItemSkill* spItem, e_UIWND_DISTRICT eWndDist)
{
	const int iReturn = -1;
	int i;

	switch ( eWndDist )
	{
		case UIWND_DISTRICT_PER_TRADE_INV:
			for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
			{
				if ( (m_pPerTradeInv[i] != nullptr) && (m_pPerTradeInv[i] == spItem) )
					return i;
			}
			break;
	}

	return iReturn;
}

RECT CUIPerTradeDlg::GetSampleRect()
{
	RECT rect;
	CN3UIArea* pArea;
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
	pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_PER_TRADE_MY, 0);
	rect = pArea->GetRegion();
	float fWidth = rect.right - rect.left;
	float fHeight = rect.bottom - rect.top;
	fWidth *= 0.5f; fHeight *= 0.5f;
	rect.left = ptCur.x - (int)fWidth;  rect.right  = ptCur.x + (int)fWidth;
	rect.top  = ptCur.y - (int)fHeight; rect.bottom = ptCur.y + (int)fHeight;
	return rect;
}

bool CUIPerTradeDlg::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
// Temp Define
#define FAIL_CODE {		\
				SetState(UI_STATE_COMMON_NONE);	\
				return false;	\
			}

	if(nullptr == pSender) return false;

	if ( (dwMsg == UIMSG_BUTTON_CLICK) && (pSender->m_szID == "btn_close") && (m_pSubProcPerTrade != nullptr) && 
		((m_pSubProcPerTrade->m_ePerTradeState == PER_TRADE_STATE_NORMAL) || 
		(m_pSubProcPerTrade->m_ePerTradeState == PER_TRADE_STATE_MY_TRADE_DECISION_DONE)) )
			// It can be canceled only in the normal state and when I press the transaction decision button.
			m_pSubProcPerTrade->LeavePerTradeState(PER_TRADE_RESULT_MY_CANCEL);

	if ( (dwMsg == UIMSG_BUTTON_CLICK) && (pSender->m_szID == "btn_trade_my") && (m_pSubProcPerTrade != nullptr) && 
		(m_pSubProcPerTrade->m_ePerTradeState == PER_TRADE_STATE_NORMAL) )		// It can only be determined in normal conditions.
				m_pSubProcPerTrade->PerTradeMyDecision();

	if (dwMsg == UIMSG_BUTTON_CLICK)					
	{
		if(pSender->m_szID == "btn_gold")
		{
			// When the inventory is floating...
			CGameProcedure::s_pProcMain->m_pSubProcPerTrade->RequestItemCountEdit();
		}
	}

	__IconItemSkill* spItem = nullptr;
	int iOrder;

const DWORD dwBitMask = 0x000f0000;

	switch (dwMsg & dwBitMask)
	{
		case UIMSG_ICON_DOWN_FIRST:
			// ..
			CN3UIWndBase::AllHighLightIconFree();

			// Get Item..
			spItem = GetHighlightIconItem((CN3UIIcon* )pSender);

			// Save Select Info..
			CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd = UIWND_PER_TRADE;
			CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict = UIWND_DISTRICT_PER_TRADE_INV;
			iOrder = GetItemiOrder(spItem, UIWND_DISTRICT_PER_TRADE_INV);
			if ( iOrder == -1 )	FAIL_CODE
			CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder = iOrder;
			CN3UIWndBase::m_sSelectedIconInfo.pItemSelect = spItem;

			// Do Ops..
			((CN3UIIcon* )pSender)->SetRegion(GetSampleRect());
			((CN3UIIcon* )pSender)->SetMoveRect(GetSampleRect());
			// Sound..
			if (spItem) PlayItemSound(spItem->pItemBasic);
			break;

		case UIMSG_ICON_DOWN:
			if ( GetState()  == UI_STATE_ICON_MOVING )
			{
				CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetRegion(GetSampleRect());
				CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetMoveRect(GetSampleRect());
			}
			break;

		case UIMSG_ICON_UP:
			// Walk around and inspect the Icon Manager windows.
			if ( !CGameProcedure::s_pUIMgr->BroadcastIconDropMsg(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) )
				// Set the icon position back to normal.
				IconRestore();
			// Sound..
			if (CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) PlayItemSound(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic);			
			break;
	}

	return true;
}

CN3UIBase* CUIPerTradeDlg::GetChildButtonByName(const std::string& szFN)
{
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		auto* pChild = (CN3UIBase* )(*itor);
		if ( (pChild->UIType() == UI_TYPE_BUTTON) && (szFN.compare(pChild->m_szID) == 0) )
			return pChild;
	}

	return nullptr;
}

