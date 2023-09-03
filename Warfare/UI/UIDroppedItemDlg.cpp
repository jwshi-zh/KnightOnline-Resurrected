#include "pch.h"

#include "UIDroppedItemDlg.h"

#include "PacketDef.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "GameProcMain.h"
#include "PlayerMyself.h"

#include "N3UIWndBase.h"
#include "UIImageTooltipDlg.h"
#include "UIInventory.h"
#include "UITransactionDlg.h"

#include "SubProcPerTrade.h"
#include "PlayerOtherMgr.h"
#include "PlayerNPC.h"

#include "UIHotKeyDlg.h"
#include "UISkillTreeDlg.h"

#include "N3UIArea.h"

CUIDroppedItemDlg::CUIDroppedItemDlg()
{
	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )	m_pMyDroppedItem[i] = nullptr;
	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )	m_bSendedIconArray[i] = false;

	m_iItemBundleID = 0;
	m_pUITooltipDlg = nullptr;
}

CUIDroppedItemDlg::~CUIDroppedItemDlg()
{
	Release();
}

void CUIDroppedItemDlg::Release()
{
	CN3UIBase::Release();

	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )
	{
		if ( m_pMyDroppedItem[i] != nullptr)
		{
			delete m_pMyDroppedItem[i];
			m_pMyDroppedItem[i] = nullptr;
		}
	}
}
void CUIDroppedItemDlg::Init(CN3UIBase* pParent)
{
	CN3UIBase::Init(pParent);

}

void CUIDroppedItemDlg::Render()
{
	if (!m_bVisible) return;	// If not visible, don't render the children.
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
	m_pUITooltipDlg->DisplayTooltipsDisable();

	bool bTooltipRender = false;
	__IconItemSkill* spItem = nullptr;

	for(auto itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->Render();
		if ( (pChild->UIType() == UI_TYPE_ICON) && (pChild->GetStyle() & UISTYLE_ICON_HIGHLIGHT) )
		{
			bTooltipRender = true;
			spItem = GetHighlightIconItem( (CN3UIIcon* )pChild );
		}
	}

	if ( bTooltipRender && spItem )
		m_pUITooltipDlg->DisplayTooltipsEnable(ptCur.x, ptCur.y, spItem );

	// Displays the number of items to be displayed.
	CN3UIString *pStr = nullptr;
	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )
	{
		if ( m_pMyDroppedItem[i] && ( (m_pMyDroppedItem[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE) || 
			(m_pMyDroppedItem[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE_SMALL) ) )
		{
			// Get string...
			pStr = GetChildStringByiOrder(i);
			if(pStr) 
			{
				if ( (GetState() == UI_STATE_ICON_MOVING) && (m_pMyDroppedItem[i] == CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) )
				{
					pStr->SetVisible(false);
				}
				else
				{
					if ( m_pMyDroppedItem[i]->pUIIcon->IsVisible() )
					{
						pStr->SetVisible(true);
						pStr->SetStringAsInt(m_pMyDroppedItem[i]->iCount);
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
			pStr = GetChildStringByiOrder(i);
			if(pStr) 
				pStr->SetVisible(false);
		}
	}
}

void CUIDroppedItemDlg::InitIconWnd(e_UIWND eWnd)
{
	const __TABLE_UI_RESRC* pTblUI = CGameBase::s_pTbl_UI->Find(CGameBase::s_pPlayer->m_InfoBase.eNation);

	m_pUITooltipDlg = new CUIImageTooltipDlg();
	m_pUITooltipDlg->Init(this);
	m_pUITooltipDlg->LoadFromFile(pTblUI->szItemInfo);
	m_pUITooltipDlg->InitPos();
	m_pUITooltipDlg->SetVisible(FALSE);	

	CN3UIWndBase::InitIconWnd(eWnd);
}

void CUIDroppedItemDlg::InitIconUpdate()
{
	CN3UIArea* pArea;
	const float fUVAspect = (float)45.0f/(float)64.0f;
	int i;

	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )
	{
		if ( m_pMyDroppedItem[i] )
		{
			m_pMyDroppedItem[i]->pUIIcon = new CN3UIIcon;
			m_pMyDroppedItem[i]->pUIIcon->Init(this);
			m_pMyDroppedItem[i]->pUIIcon->SetTex(m_pMyDroppedItem[i]->szIconFN);
			m_pMyDroppedItem[i]->pUIIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
			m_pMyDroppedItem[i]->pUIIcon->SetUIType(UI_TYPE_ICON);
			m_pMyDroppedItem[i]->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_DROP_ITEM, i);
			if ( pArea )
			{
				m_pMyDroppedItem[i]->pUIIcon->SetRegion(pArea->GetRegion());
				m_pMyDroppedItem[i]->pUIIcon->SetMoveRect(pArea->GetRegion());
			}
		}
	}
}

__IconItemSkill* CUIDroppedItemDlg::GetHighlightIconItem(CN3UIIcon* pUIIcon)
{
	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )
	{
		if ( (m_pMyDroppedItem[i] != nullptr) && (m_pMyDroppedItem[i]->pUIIcon == pUIIcon) )
			return m_pMyDroppedItem[i];
	}
	return nullptr;
}

void CUIDroppedItemDlg::EnterDroppedState(int xpos, int ypos)
{
	if ( !IsVisible() )
		SetVisible(true);

	SetPos(xpos, ypos-150);

	int i;
	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )
	{
		m_bSendedIconArray[i] = false;

		if ( m_pMyDroppedItem[i] )
		{
			if ( m_pMyDroppedItem[i]->pUIIcon )
			{
				RemoveChild(m_pMyDroppedItem[i]->pUIIcon);
				m_pMyDroppedItem[i]->pUIIcon->Release();
				delete m_pMyDroppedItem[i]->pUIIcon;
				m_pMyDroppedItem[i]->pUIIcon = nullptr;
			}
			delete m_pMyDroppedItem[i];	
			m_pMyDroppedItem[i] = nullptr;
		}
	}

	CN3UIWndBase::AllHighLightIconFree();
}


void CUIDroppedItemDlg::LeaveDroppedState()
{
	if ( IsVisible() )
		SetVisible(false);

	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )	m_bSendedIconArray[i] = false;
}

DWORD CUIDroppedItemDlg::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if ( !IsVisible() ) { dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);  return dwRet; }

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

void CUIDroppedItemDlg::AddToItemTable(int iItemID, int iItemCount, int iOrder)
{
	__IconItemSkill*	spItem;
	__TABLE_ITEM_BASIC*	pItem = nullptr;								// Item table structure pointer..
	__TABLE_ITEM_EXT*	pItemExt = nullptr;							// Item table structure pointer..
	std::string szIconFN;

	pItem = CGameBase::s_pTbl_Items_Basic->Find(iItemID/1000*1000);	// Get column data...
	if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
		pItemExt = CGameBase::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iItemID%1000);
	if(nullptr == pItem || nullptr == pItemExt)
	{
		__ASSERT(0, "not in item pointer table!!");
		CLogWriter::Write("CUIDroppedItemDlg::AddToItemTable - Invalidate ItemID : %d", iItemID);
		return;
	}

	TRACE("Dropped item from server to ItemDlg %d \n", iItemID);
	e_PartPosition ePart;
	e_PlugPosition ePlug;
	const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item
	if ( ITEM_TYPE_UNKNOWN == eType ) return;
	
	spItem = new __IconItemSkill;
	spItem->pItemBasic = pItem;
	spItem->pItemExt = pItemExt;
	spItem->szIconFN = szIconFN; // Copy icon filename..
	spItem->iCount	= iItemCount;
	spItem->iDurability = pItem->siMaxDurability+pItemExt->siMaxDurability;

	m_pMyDroppedItem[iOrder] = spItem;
}

void CUIDroppedItemDlg::AddToItemTableToInventory(int iItemID, int iItemCount, int iOrder)
{
	CN3UIArea* pArea;
	__IconItemSkill*	spItem;
	__TABLE_ITEM_BASIC*	pItem = nullptr;								// Item table structure pointer..
	__TABLE_ITEM_EXT*	pItemExt = nullptr;							// Item table structure pointer..
	std::string szIconFN;
	const float fUVAspect = (float)45.0f/(float)64.0f;

	pItem = CGameBase::s_pTbl_Items_Basic->Find(iItemID/1000*1000);	// Get column data...
	if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
		pItemExt = CGameBase::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iItemID%1000);
	if(nullptr == pItem || nullptr == pItemExt)
	{
		__ASSERT(0, "not in item pointer table!!");
		CLogWriter::Write("CUIDroppedItemDlg::AddToItemTableToInventory - Invalidate ItemID : %d", iItemID);
		return;
	}

	TRACE("Dropped item from server to ItemDlg %d \n", iItemID);
	e_PartPosition ePart;
	e_PlugPosition ePlug;
	const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item
	if ( ITEM_TYPE_UNKNOWN == eType ) return;
	
	spItem = new __IconItemSkill;
	spItem->pItemBasic = pItem;
	spItem->pItemExt = pItemExt;
	spItem->szIconFN = szIconFN; // Copy icon filename..
	spItem->iCount	= iItemCount;
	spItem->iDurability = pItem->siMaxDurability+pItemExt->siMaxDurability;

	spItem->pUIIcon = new CN3UIIcon;
	spItem->pUIIcon->Init(CGameProcedure::s_pProcMain->m_pUIInventory);
	spItem->pUIIcon->SetTex(spItem->szIconFN);
	spItem->pUIIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
	spItem->pUIIcon->SetUIType(UI_TYPE_ICON);
	spItem->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
	spItem->pUIIcon->SetVisible(true);
	pArea = nullptr;
	pArea = CGameProcedure::s_pProcMain->m_pUIInventory->GetChildAreaByiOrder(UI_AREA_TYPE_INV, iOrder);
	if ( pArea )
	{
		spItem->pUIIcon->SetRegion(pArea->GetRegion());
		spItem->pUIIcon->SetMoveRect(pArea->GetRegion());
	}

	CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[iOrder] = spItem;
	PlayItemSound(pItem);
}

bool CUIDroppedItemDlg::ReceiveIconDrop(__IconItemSkill* spItem, POINT ptCur)
{
	CN3UIArea* pArea = nullptr;

	if (!m_bVisible) return false;

	// It should check and notify the window with the selected icon the result.
	switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd )
	{
		// If it's from the inventory window...
		case UIWND_INVENTORY:
			CGameProcedure::s_pProcMain->m_pUIInventory->CancelIconDrop(spItem);
			break;

		// If it's from the commerce window...
		case UIWND_TRANSACTION:
			CGameProcedure::s_pProcMain->m_pUITransactionDlg->CancelIconDrop(spItem);
			break;
	}

	return false;
}

int CUIDroppedItemDlg::GetInventoryEmptyInviOrder(__IconItemSkill* spItem)
{
	int i;
	if (spItem == nullptr)
	{
		for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
		{
			if ( !CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[i] )
				return i;
		}
	}
	else
	{
		for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
		{
			if ( CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[i] && 
				(CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[i]->pItemBasic->dwID == spItem->pItemBasic->dwID) )
				return i;
		}

		for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
		{
			if ( !CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[i] )
				return i;
		}
	}

	return -1;
}

int	CUIDroppedItemDlg::GetItemiOrder(__IconItemSkill* spItem) const
{
	int i;

	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )
	{
		if ( (m_pMyDroppedItem[i] != nullptr) && (m_pMyDroppedItem[i] == spItem) )
			return i;
	}

	return -1;
}

bool CUIDroppedItemDlg::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
// Temp Define
#define FAIL_CODE {		\
				CN3UIWndBase::m_sSelectedIconInfo.eIconSelectState = UIICON_SELECT_NONE;	\
				return false;	\
			}

// Code Begin
	if(nullptr == pSender) return false;

	int iOrder, iOrderInv = -1;

const DWORD dwBitMask = 0x000f0000;

	__TABLE_ITEM_BASIC*	pItem;
	__IconItemSkill*	spItem;
	std::string szIconFN;
	e_PartPosition ePart;
	e_PlugPosition ePlug;
	e_ItemType eType;

	// Since it is an item that has not been sent to the server... it is sent to the server...
	BYTE byBuff[16];
	int iOffset = 0;

	switch (dwMsg & dwBitMask)
	{
		case UIMSG_ICON_DOWN_FIRST:
			spItem = nullptr;
			spItem = GetHighlightIconItem((CN3UIIcon* )pSender);
			if ( !spItem )	break;
			m_iBackupiOrder = GetItemiOrder( spItem );
			break;

		case UIMSG_ICON_UP:
			SetState(UI_STATE_COMMON_NONE);

			// Checks if an item is money.
			pItem = nullptr;								// Item table structure pointer..
			spItem = nullptr;
			spItem = GetHighlightIconItem((CN3UIIcon* )pSender);
			if ( !spItem )	break;
			pItem = CGameBase::s_pTbl_Items_Basic->Find(spItem->pItemBasic->dwID);	// Get column data...
			if ( pItem == nullptr)
			{
				__ASSERT(0, "NULL Item!!!");
				CLogWriter::Write("CUIDroppedItemDlg::ReceiveMessage - UIMSG_ICON_UP - NULL Icon : %d", spItem->pItemBasic->dwID);
				break;
			}

			eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item

			// Compare with the sent icon array.
			iOrder = GetItemiOrder( spItem );
			if ( m_iBackupiOrder != iOrder )	break;

			if ( m_bSendedIconArray[iOrder] == true )	// If the packet has been sent once, break it.
				break;
			else
				m_bSendedIconArray[iOrder] = true;

			CAPISocket::MP_AddByte(byBuff, iOffset, N3_ITEM_DROPPED_GET);
			CAPISocket::MP_AddDword(byBuff, iOffset, m_iItemBundleID);

			// If it's not money, add it to the inventory list...
			if ( ITEM_TYPE_GOLD != eType ) 
			{
				CAPISocket::MP_AddDword(byBuff, iOffset, spItem->pItemBasic->dwID+spItem->pItemExt->dwID);
				// CAPISocket::MP_AddDword(byBuff, iOffset, (BYTE)iOrderInv);
			}
			else
			{
				CAPISocket::MP_AddDword(byBuff, iOffset, spItem->pItemBasic->dwID);
				// CAPISocket::MP_AddDword(byBuff, iOffset, 0xff);
			}
			CGameProcedure::s_pSocket->Send(byBuff, iOffset);

			// Set sent icon information..
			CN3UIWndBase::m_sRecoveryJobInfo.pItemSource = spItem;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWnd = UIWND_DROPITEM;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict = UIWND_DISTRICT_DROPITEM;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder = iOrder;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWnd = UIWND_INVENTORY;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict = UIWND_DISTRICT_INVENTORY_INV;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder = iOrderInv;
			break;

		case UIMSG_ICON_DOWN:
			break;

		case UIMSG_ICON_DBLCLK:
			SetState(UI_STATE_COMMON_NONE);
			break;
	}

	return true;
}

void CUIDroppedItemDlg::GetItemByIDToInventory(BYTE bResult, int iItemID, int iGold, int iPos, int iItemCount, int iStrLen, 
											   std::string strString)
{
	// Find an item in the item list.
	bool				bFound = false;
	const __TABLE_ITEM_BASIC*	pItem = nullptr;									// Item table structure pointer..
	const __TABLE_ITEM_EXT*	pItemExt = nullptr;
	__IconItemSkill*	spItem = nullptr;
	int i;
	char szMsg[32];
	CN3UIString* pStatic = nullptr;
	__InfoPlayerMySelf*	pInfoExt = nullptr;
	std::string stdMsg;

	if (bResult == 0x00)			// failure..
	{
		const int iOrderInv = GetInventoryEmptyInviOrder();
		if ( iOrderInv == -1 )
		{
			// If the inventory is full.. break.. ^^
			char szBuff[128] = "";
			std::string stdMsg;
			::_LoadStringFromResource(IDS_INV_ITEM_FULL, stdMsg);
			CGameProcedure::s_pProcMain->MsgOutput(stdMsg, 0xff9b9bff);
		}

		if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
		if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();

		return;
	}
	
	if (bResult == 0x02)		// Get Noah in party state.
	{
		// money update.
		pStatic = nullptr;
		pInfoExt = &(CGameBase::s_pPlayer->m_InfoExt);

		// money update.
		::_LoadStringFromResource(IDS_DROPPED_NOAH_GET, stdMsg);
		sprintf(szMsg, stdMsg.c_str(), iGold - pInfoExt->iGold);
		CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xff9b9bff);

		pInfoExt->iGold = iGold;
		TRACE("돈 업데이트 %d \n", iGold);
		pStatic = (CN3UIString* )CGameProcedure::s_pProcMain->m_pUIInventory->GetChildByID("text_gold"); __ASSERT(pStatic, "NULL UI Component!!");
		if(pStatic) pStatic->SetStringAsInt(iGold);

		if (!IsVisible()) 
			return;

		// Remove the money icon if present.
		bFound = false;
		int i = 0;
		for(; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )
		{
			if( m_pMyDroppedItem[i] && m_pMyDroppedItem[i]->pItemBasic->dwID == dwGold )
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)	return;

		spItem = m_pMyDroppedItem[i];
		if (!spItem)	return;
		// Removed from manager..
		RemoveChild(spItem->pUIIcon);

		// Remove resource...
		spItem->pUIIcon->Release();
		delete spItem->pUIIcon;
		spItem->pUIIcon = nullptr;
		delete spItem;
		spItem = nullptr;
		m_pMyDroppedItem[i] = nullptr;

		PlayGoldSound();

		if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
		if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();
	}

	if (bResult == 0x03)		// In party state, I acquire an item..
	{
		pItem = nullptr;
		pItemExt = nullptr;
		pItem = CGameBase::s_pTbl_Items_Basic->Find(iItemID/1000*1000);	// Get column data...
		if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
			pItemExt = CGameBase::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iItemID%1000);
		if(nullptr == pItem || nullptr == pItemExt)
		{
			__ASSERT(0, "not in item pointer table!!");
			CLogWriter::Write("CUIDroppedItemDlg::GetItemByIDToInventory - NULL Icon : %d", iItemID);
			return;
		}

		char szBuff[128] = "";
		::_LoadStringFromResource(IDS_PARTY_ITEM_GET, stdMsg);
		sprintf(szBuff, stdMsg.c_str(), strString.c_str(), pItem->szName.c_str());
		CGameProcedure::s_pProcMain->MsgOutput(szBuff, 0xff9b9bff);

		if (!IsVisible()) 
			return;

		// Remove item icon if present.
		bFound = false;
		int i = 0;
		for(; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )
		{
			if( (m_pMyDroppedItem[i]) && (m_pMyDroppedItem[i]->pItemBasic->dwID+m_pMyDroppedItem[i]->pItemExt->dwID == iItemID) )
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)	return;

		if (m_pMyDroppedItem[i])
			PlayItemSound(m_pMyDroppedItem[i]->pItemBasic);

		spItem = m_pMyDroppedItem[i];
		if (!spItem)	return;
		// Removed from manager..
		RemoveChild(spItem->pUIIcon);

		// Remove resource...
		spItem->pUIIcon->Release();
		delete spItem->pUIIcon;
		spItem->pUIIcon = nullptr;
		delete spItem;
		spItem = nullptr;
		m_pMyDroppedItem[i] = nullptr;

		if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
		if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();

		return;
	}

	if (bResult == 0x04)		// In the party state, another member acquires an item..
	{
		spItem = m_pMyDroppedItem[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder];
		if (spItem)
		{
			// Removed from manager..
			RemoveChild(spItem->pUIIcon);

			// Remove resource...
			spItem->pUIIcon->Release();
			delete spItem->pUIIcon;
			spItem->pUIIcon = nullptr;
			delete spItem;
			spItem = nullptr;
		}

		m_pMyDroppedItem[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = nullptr;

		if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
		if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();

		return;	
	}

	if (bResult == 0x05)		// Acquire common items in party state..
	{
		if (iItemID == dwGold)
		{
			__ASSERT(0, "Invalidate Item ID From Server.. ");
			CLogWriter::Write("CUIDroppedItemDlg::GetItemByIDToInventory - ID Pos : %d", iPos);
			return;
		}

		if ( (iPos < 0) || (iPos > (MAX_ITEM_INVENTORY-1)) )
		{
			__ASSERT(0, "Invalidate Item Pos From Server.. ");
			CLogWriter::Write("CUIDroppedItemDlg::GetItemByIDToInventory - Invalidate Pos : %d", iPos);
			return;
		}

		__IconItemSkill	*spItemDest = CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[iPos];
		if (spItemDest)		// There is an item...
		{
			if ( iItemID != (spItemDest->pItemBasic->dwID+spItemDest->pItemExt->dwID) )
			{
				// Clear the existing items..
				if (!spItemDest) return;
				RemoveChild(spItemDest->pUIIcon);

				// Delete Icon Resource...
				spItemDest->pUIIcon->Release();
				delete spItemDest->pUIIcon;
				spItemDest->pUIIcon = nullptr;
				delete spItemDest;
				spItemDest = nullptr;

				CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[iPos] = nullptr;

				// Create a new item. Set the number of items..
				AddToItemTableToInventory(iItemID, iItemCount, iPos);
			}
			else
			{
				// Set the number of...
				CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[iPos]->iCount = iItemCount;
				PlayItemSound(CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[iPos]->pItemBasic);
			}
		}
		else
		{
			// If there is no item.. Create a new one.. Set the number of items..
			AddToItemTableToInventory(iItemID, iItemCount, iPos);
		}

		pItem = nullptr;
		pItem = CGameBase::s_pTbl_Items_Basic->Find(iItemID/1000*1000);	// Get column data...
		if(nullptr == pItem)
		{
			__ASSERT(0, "not in item pointer table!!");
			CLogWriter::Write("CUIDroppedItemDlg::GetItemByIDToInventory - NULL Icon : %d", iItemID);
			return;
		}

		::_LoadStringFromResource(IDS_ITEM_GET_BY_RULE, stdMsg);
		sprintf(szMsg, stdMsg.c_str(), pItem->szName.c_str());
		CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xff9b9bff);

		if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
		if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();
	}

	if (bResult == 0x06)
	{
		// Show message box text..
		std::string szMsg; ::_LoadStringFromResource(IDS_ITEM_TOOMANY_OR_HEAVY, szMsg);
		CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
	}

	if (bResult == 0x07)
	{
		// Show message box text..
		std::string szMsg; ::_LoadStringFromResource(IDS_INV_ITEM_FULL, szMsg);
		CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
	}

	if (bResult == 0x01)
	{
		if (iItemID != dwGold)
		{
			if ( (iPos < 0) || (iPos > (MAX_ITEM_INVENTORY-1)) )
			{
				__ASSERT(0, "Invalidate Item Pos From Server.. ");
				CLogWriter::Write("CUIDroppedItemDlg::GetItemByIDToInventory - Invalidate Pos : %d", iPos);
				return;
			}

			spItem = nullptr;
			__IconItemSkill	*spItemDest = CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[iPos];
			if (spItemDest)		// There is an item...
			{
				if ( iItemID != (spItemDest->pItemBasic->dwID+spItemDest->pItemExt->dwID) )
				{
					// Clear the existing items..
					if (!spItemDest) return;
					RemoveChild(spItemDest->pUIIcon);

					// Delete Icon Resource...
					spItemDest->pUIIcon->Release();
					delete spItemDest->pUIIcon;
					spItemDest->pUIIcon = nullptr;
					delete spItemDest;
					spItemDest = nullptr;

					CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[iPos] = nullptr;

					// Create a new item. Set the number of items..
					AddToItemTableToInventory(iItemID, iItemCount, iPos);
				}
				else
				{
					// Set the number of...
					CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[iPos]->iCount = iItemCount;
					PlayItemSound(CGameProcedure::s_pProcMain->m_pUIInventory->m_pMyInvWnd[iPos]->pItemBasic);
				}
			}
			else
			{
				// If there is no item.. Create a new one.. Set the number of items..
				AddToItemTableToInventory(iItemID, iItemCount, iPos);
			}

			spItem = m_pMyDroppedItem[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder];
			if (spItem)
			{
				// Removed from manager..
				RemoveChild(spItem->pUIIcon);

				// Remove resource...
				spItem->pUIIcon->Release();
				delete spItem->pUIIcon;
				spItem->pUIIcon = nullptr;
				delete spItem;
				spItem = nullptr;
			}
			m_pMyDroppedItem[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = nullptr;
		}
		else
		{
			pStatic = nullptr;
			pInfoExt = &(CGameBase::s_pPlayer->m_InfoExt);

			// money update.
			::_LoadStringFromResource(IDS_DROPPED_NOAH_GET, stdMsg);
			sprintf(szMsg, stdMsg.c_str(), iGold - pInfoExt->iGold);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xff9b9bff);

			pInfoExt->iGold = iGold;
			TRACE("돈 업데이트 %d \n", iGold);
			pStatic = (CN3UIString* )CGameProcedure::s_pProcMain->m_pUIInventory->GetChildByID("text_gold"); __ASSERT(pStatic, "NULL UI Component!!");
			if(pStatic) pStatic->SetStringAsInt(iGold);

			spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource;
			if (nullptr == spItem ) return;

			// Removed from manager..
			RemoveChild(spItem->pUIIcon);

			// Remove resource...
			spItem->pUIIcon->Release();
			delete spItem->pUIIcon;
			spItem->pUIIcon = nullptr;
			delete spItem;
			spItem = nullptr;
			m_pMyDroppedItem[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = nullptr;

			PlayGoldSound();
		}

		if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
		if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();
	}

	bFound = false;
	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )
	{
		if( m_pMyDroppedItem[i] != nullptr)
			bFound = true;
	}

	if ( !bFound )
		LeaveDroppedState();
}

// this_ui_add_start
void CUIDroppedItemDlg::SetVisibleWithNoSound(bool bVisible, bool bWork, bool bReFocus)
{
	CN3UIBase::SetVisibleWithNoSound(bVisible, bWork, bReFocus);

	for(auto i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++ )	m_bSendedIconArray[i] = false;
}
// this_ui_add_end