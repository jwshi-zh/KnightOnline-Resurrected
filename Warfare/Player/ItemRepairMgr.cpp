#include "pch.h"
#include "ItemRepairMgr.h"

#include "PacketDef.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "UIInventory.h"
#include "GameProcMain.h"
#include "PlayerMySelf.h"

#include "UIRepairTooltipDlg.h"

#include "N3UIString.h"

CItemRepairMgr::CItemRepairMgr()
{
	m_pspItemBack = nullptr;	
}

CItemRepairMgr::~CItemRepairMgr()
{

}

void CItemRepairMgr::Tick()
{
	const CUIInventory *pInv	= CGameProcedure::s_pProcMain->m_pUIInventory;
	if (!pInv) return;
	if (!pInv->IsVisible())	return;
	if (pInv->GetInvState() != INV_STATE_REPAIR) return;
	if (CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer) return;

	CUIRepairTooltipDlg *pDlg = CGameProcedure::s_pProcMain->m_pUIRepairTooltip;
	if (pDlg)	pDlg->m_bBRender = false;

	const POINT ptCur			= CGameProcedure::s_pLocalInput->MouseGetPos();

	// find a location
	int i;	int iArm = 0x00; int iOrder = -1; __IconItemSkill* spItem = nullptr;
	for (auto i = 0; i < ITEM_SLOT_COUNT; i++)
	{
		if (spItem) break;
		if (pInv->m_pMySlot[i])
		{
			if (pInv->m_pMySlot[i]->pUIIcon->IsIn(ptCur.x, ptCur.y))
			{
				iArm = 0x01;
				spItem = pInv->m_pMySlot[i];
				iOrder = i;
			}
		}
	}

	if (!spItem)
	{
		for (auto i = 0; i < MAX_ITEM_INVENTORY; i++)
		{
			if (spItem) break;
			if (pInv->m_pMyInvWnd[i])
			{
				if (pInv->m_pMyInvWnd[i]->pUIIcon->IsIn(ptCur.x, ptCur.y))
				{
					iArm = 0x02;
					spItem = pInv->m_pMyInvWnd[i];
					iOrder = i;
				}
			}
		}
	}

	// If you are on the icon...
	int iRepairGold = 0;
	if (spItem)
	{
		iRepairGold = CalcRepairGold(spItem);

		// Show repair price tooltip..
		if (pDlg)
		{
			pDlg->m_bBRender		= true;
			pDlg->m_iBxpos			= ptCur.x;
			pDlg->m_iBypos			= ptCur.y;
			pDlg->m_pBspItem		= spItem;
			pDlg->m_iBRequiredGold	= iRepairGold;
		}

		// If the repair cost is more than the money I have...
		if (iRepairGold > s_pPlayer->m_InfoExt.iGold)
		{
			// marked red...
			if (pDlg)
				pDlg->m_bBHaveEnough = false;
		}
		else
		{
			// If child, the original color..
			if (pDlg)
				pDlg->m_bBHaveEnough = true;
		}
	}

	const DWORD dwMouseFlags	= CGameProcedure::s_pLocalInput->MouseGetFlag();	// Mouse button flags - see LocalInput.h
	if (dwMouseFlags & MOUSE_LBCLICK)		// Press the left button...
	{
		m_pspItemBack	= spItem;
		m_iArm			= iArm;
		m_iiOrder		= iOrder;
	}
	else if(dwMouseFlags & MOUSE_LBCLICKED)
	{
		if (m_pspItemBack && spItem && (m_pspItemBack == spItem) )
		{
			// Send To Server..
			if (iRepairGold > 0)										// If there is a repair price...
			{
				// If the repair cost is more than the money I have...
				if (iRepairGold > s_pPlayer->m_InfoExt.iGold)
				{
					// Display message without sending to server..
					std::string szMsg; ::_LoadStringFromResource(IDS_REPAIR_LACK_GOLD, szMsg);
					CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff00ff);
				}
				else
				{
					BYTE byBuff[8];															// Packet buffer...
					int iOffset=0;															// Packet Offset...

					CAPISocket::MP_AddByte(byBuff, iOffset,  N3_ITEM_REPAIR_REQUEST);			// Game start packet commands..
					CAPISocket::MP_AddByte(byBuff, iOffset,  iArm);							// ID Length Packet..
					CAPISocket::MP_AddByte(byBuff, iOffset,  iOrder);							// ID Length Packet..
					CAPISocket::MP_AddDword(byBuff, iOffset, spItem->pItemBasic->dwID+spItem->pItemExt->dwID);	// Put the ID string in the packet..

					CGameProcedure::s_pSocket->Send(byBuff, iOffset);	

					// waiting for reply...
					CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer = true;

					// Change To Cursor..
					CGameProcedure::SetGameCursor(CGameProcedure::s_hCursorNowRepair, true);
				}
			}
		}
	}
}

void CItemRepairMgr::ReceiveResultFromServer(int iResult, int iUserGold)
{
	CUIRepairTooltipDlg *pDlg = CGameProcedure::s_pProcMain->m_pUIRepairTooltip;
	CUIInventory *pInv	= CGameProcedure::s_pProcMain->m_pUIInventory;
	if (!pInv) return;
	if (!m_pspItemBack) return;

	// If successful, the durability of the npc area is set to the maximum value.
	if(iResult == 0x01)
	{
		m_pspItemBack->iDurability = m_pspItemBack->pItemBasic->siMaxDurability+m_pspItemBack->pItemExt->siMaxDurability;

		switch (m_iArm)
		{
			case 0x01: // equipped item
				pInv->m_pMySlot[m_iiOrder] = m_pspItemBack;
				s_pPlayer->DurabilitySet((e_ItemSlot)m_iiOrder, m_pspItemBack->iDurability); // Restores durability.
				break;

			case 0x02: // Items in your inventory...
				pInv->m_pMyInvWnd[m_iiOrder] = m_pspItemBack;
				break;
		}

		// If the icon state is UISTYLE_DURABILITY_EXHAUST...
		m_pspItemBack->pUIIcon->SetStyle(m_pspItemBack->pUIIcon->GetStyle() & (~UISTYLE_DURABILITY_EXHAUST));
		
		if (pDlg)	pDlg->m_iBRequiredGold	= 0;
		pInv->PlayRepairSound();
	}

	// money update.
	UpdateUserTotalGold(iUserGold);

	// Disable wait for response..
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer = false;

	// Change To Cursor..
	CGameProcedure::SetGameCursor(CGameProcedure::s_hCursorPreRepair, true);
}

void CItemRepairMgr::UpdateUserTotalGold(int iGold)
{
	char szGold[32];

	// money update.
	s_pPlayer->m_InfoExt.iGold = iGold;
	sprintf(szGold, "%d", iGold);
	CN3UIString* pStatic = (CN3UIString*)CGameProcedure::s_pProcMain->m_pUIInventory->GetChildByID("text_gold"); __ASSERT(pStatic, "NULL UI Component!!");
	if(pStatic)
	{
		pStatic->SetString(szGold);
	}
}

int CItemRepairMgr::CalcRepairGold(__IconItemSkill* spItem)
{
	const int iMaxDurability = spItem->pItemBasic->siMaxDurability + spItem->pItemExt->siMaxDurability;
	if (iMaxDurability <= 0) return 0;
	const float fAllPrice = (float)spItem->pItemBasic->iPrice*(float)spItem->pItemExt->siPriceMultiply;
	const float fTemp = ((fAllPrice-10.0f)/(10000.0f))+pow(fAllPrice, 0.75f);
	const float fValue = fTemp*( (float)(iMaxDurability - spItem->iDurability) / (float)(iMaxDurability) );

	return (int)fValue;
}
