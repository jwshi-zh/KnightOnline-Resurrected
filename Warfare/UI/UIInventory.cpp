#include "pch.h"

#include "PlayerMySelf.h"
#include "PacketDef.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "GameProcMain.h"

#include "UIInventory.h"
#include "UITransactionDlg.h"
#include "UIImageTooltipDlg.h"
#include "UIManager.h"

#include "SubProcPerTrade.h"
#include "UITradeEditDlg.h"
#include "UIPerTradeDlg.h"

#include "CountableItemEditDlg.h"
#include "UIRepairTooltipDlg.h"

#include "UIHotKeyDlg.h"
#include "UISkillTreeDlg.h"

#include "N3UIString.h"
#include "N3UIEdit.h"
#include "N3SndObj.h"

static bool g_bItemClassGroup[26][26] = {	// [item] [player]
	// { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0 } // initial group
	// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
	{ 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },	// Group 0
	{ 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1 },	// No. 1 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },	// No. 2 Group
	{ 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1 },	// No. 3 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },	// No. 4 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },	// No. 5 Group
	{ 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0 },	// Group 6
	{ 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },	// No. 7 Group
	{ 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },	// No. 8 Group
	{ 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1 },	// No. 9 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 },	// No. 10 Group
	{ 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1 },	// No. 11 Group
	{ 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1 },	// No. 12 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0 },	// No. 13 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },	// No. 14 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	// No. 15 Group
	{ 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0 },	// No. 16 Group
	{ 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 },	// No. 17 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },	// No. 18 Group
	{ 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0 },	// No. 19 Group
	{ 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0 },	// No. 20 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1 },	// No. 21 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1 },	// No. 22 Group
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1 },	// No. 23 Group
	{ 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0 },	// No. 24 Group
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };	// No. 25 Group (All Classes)

CUIInventory::CUIInventory()
{
	for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )	m_pMySlot[i] = nullptr;
	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )	m_pMyInvWnd[i] = nullptr;

	m_pUITooltipDlg = nullptr;
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer = false;

	m_bOpenningNow = false; // is opening...
	m_bClosingNow = false;	// it's closing...
	m_fMoveDelta = 0; // Floating point is used to calculate the current position to make it open and close smoothly.

	m_bDestoyDlgAlive	= false;
	m_pText_Weight = nullptr;

	m_iRBtnDownOffs = -1;
	m_bRBtnProcessing = false;
}

CUIInventory::~CUIInventory()
{
	Release();
}

void CUIInventory::Release()
{
	CN3UIBase::Release();

	for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( m_pMySlot[i] != nullptr)
		{
			delete m_pMySlot[i];
			m_pMySlot[i] = nullptr;
		}
	}

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pMyInvWnd[i] != nullptr)
		{
			delete m_pMyInvWnd[i];
			m_pMyInvWnd[i] = nullptr;
		}
	}

	m_bOpenningNow = false; // is opening...
	m_bClosingNow = false;	// it's closing...
	m_fMoveDelta = 0; // Floating point is used to calculate the current position to make it open and close smoothly.
	m_pText_Weight = nullptr;
}

bool CUIInventory::HasAnyItemInSlot() const
{
	for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( m_pMySlot[i] != nullptr)
			return true;
	}
	return false;	
}

void CUIInventory::ReleaseItem()
{
	for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( m_pMySlot[i] != nullptr)
		{
			if ( m_pMySlot[i]->pUIIcon )
			{
				RemoveChild(m_pMySlot[i]->pUIIcon);
				m_pMySlot[i]->pUIIcon->Release();
				delete m_pMySlot[i]->pUIIcon;
				m_pMySlot[i]->pUIIcon = nullptr;
			}

			delete m_pMySlot[i];	
			m_pMySlot[i] = nullptr;
		}
	}

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pMyInvWnd[i] != nullptr)
		{
			if ( m_pMyInvWnd[i]->pUIIcon )
			{
				RemoveChild(m_pMyInvWnd[i]->pUIIcon);
				m_pMyInvWnd[i]->pUIIcon->Release();
				delete m_pMyInvWnd[i]->pUIIcon;
				m_pMyInvWnd[i]->pUIIcon = nullptr;
			}

			delete m_pMyInvWnd[i];
			m_pMyInvWnd[i] = nullptr;
		}
	}
}

void CUIInventory::Open(e_InvenState eIS)
{
	m_eInvenState = eIS;
	if ( eIS == INV_STATE_REPAIR )
	{
		CGameProcedure::SetGameCursor(CGameProcedure::s_hCursorPreRepair, true);
	}

	CN3UIString* pStatic = (CN3UIString* )GetChildByID("text_gold"); __ASSERT(pStatic, "NULL UI Component!!");
	if(pStatic)
	{
		pStatic->SetStringAsInt(CGameBase::s_pPlayer->m_InfoExt.iGold);
	}
	
	// It's open!!
	SetVisible(true);
	this->SetPos(CN3Base::s_CameraData.vp.Width, 10);
	m_fMoveDelta = 0;
	m_bOpenningNow = true;
	m_bClosingNow = false;

	m_iRBtnDownOffs = -1;
}

void CUIInventory::GoldUpdate()
{
	CN3UIString* pStatic = (CN3UIString* )GetChildByID("text_gold"); __ASSERT(pStatic, "NULL UI Component!!");
	if(pStatic)
	{
		pStatic->SetStringAsInt(CGameBase::s_pPlayer->m_InfoExt.iGold);
	}
}

void CUIInventory::Close(bool bByKey)
{
	if ( m_eInvenState == INV_STATE_REPAIR )
	{
		if (bByKey) return;
		CGameProcedure::RestoreGameCursor();

		if (CGameProcedure::s_pProcMain->m_pUIRepairTooltip->IsVisible())
		{
			CGameProcedure::s_pProcMain->m_pUIRepairTooltip->m_bBRender = false;
			CGameProcedure::s_pProcMain->m_pUIRepairTooltip->DisplayTooltipsDisable();
		}
	}

	m_eInvenState = INV_STATE_NORMAL;

	if (GetState() == UI_STATE_ICON_MOVING)
		IconRestore();
	SetState(UI_STATE_COMMON_NONE);
	CN3UIWndBase::AllHighLightIconFree();

	// It closes slowly..!!
	// SetVisible(false); // Do this after everything is closed.
	const RECT rc = this->GetRegion();
	this->SetPos(CN3Base::s_CameraData.vp.Width - (rc.right - rc.left), 10);
	m_fMoveDelta = 0;
	m_bOpenningNow = false;
	m_bClosingNow = true;

	if(m_pSnd_CloseUI) m_pSnd_CloseUI->Play(); // closing sound...

	m_iRBtnDownOffs = -1;
}

void CUIInventory::Tick()
{
	if (!m_bVisible) return;	// If not visible, don't tick children.

	if(m_bOpenningNow) // Swipe from right to left...if you need to open...
	{
		POINT ptCur = this->GetPos();
		const RECT rc = this->GetRegion();
		const auto fWidth = (float)(rc.right - rc.left);

		float fDelta = 5000.0f * CN3Base::s_fSecPerFrm;
		fDelta *= (fWidth - m_fMoveDelta) / fWidth;
		if(fDelta < 2.0f) fDelta = 2.0f;
		m_fMoveDelta += fDelta;

		const int iXLimit = CN3Base::s_CameraData.vp.Width - (int)fWidth;
		ptCur.x = CN3Base::s_CameraData.vp.Width - (int)m_fMoveDelta;
		if(ptCur.x <= iXLimit) // It's all open!!
		{
			ptCur.x = iXLimit;
			m_bOpenningNow = false;
		}

		this->SetPos(ptCur.x, ptCur.y);

		CN3UIWndBase::AllHighLightIconFree();
	}
	else if(m_bClosingNow) // Swipe from right to left...if you need to open...
	{
		POINT ptCur = this->GetPos();
		const RECT rc = this->GetRegion();
		const auto fWidth = (float)(rc.right - rc.left);

		float fDelta = 5000.0f * CN3Base::s_fSecPerFrm;
		fDelta *= (fWidth - m_fMoveDelta) / fWidth;
		if(fDelta < 2.0f) fDelta = 2.0f;
		m_fMoveDelta += fDelta;

		const int iXLimit = CN3Base::s_CameraData.vp.Width;
		ptCur.x = CN3Base::s_CameraData.vp.Width - (int)(fWidth - m_fMoveDelta);
		if(ptCur.x >= iXLimit) // Everything is closed..!!
		{
			ptCur.x = iXLimit;
			m_bClosingNow = false;

			this->SetVisibleWithNoSound(false, false, true); // It's all closed so you can't see it.
		}

		this->SetPos(ptCur.x, ptCur.y);

		CN3UIWndBase::AllHighLightIconFree();
	}

	CGameBase::s_pPlayer->InventoryChrTick();
	CN3UIBase::Tick();
	CGameBase::s_pPlayer->m_ChrInv.m_nLOD = 1;

	m_cItemRepairMgr.Tick();
}

void CUIInventory::Render()
{
	if (!m_bVisible) return;	// If not visible, don't render the children.
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
	m_pUITooltipDlg->DisplayTooltipsDisable();
	const RECT rUser = m_pArea_User->GetRegion();

	bool bTooltipRender = false;
	__IconItemSkill* spItem = nullptr;

	RECT rcRegion;
	SetRect(&rcRegion, rUser.left, rUser.top, rUser.right, rUser.bottom);			// zoning
	char strDummy[32];
	lstrcpy(strDummy, "elmo_ecli666");

	for(auto itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if ( (m_szID != "Base_Iteminfo") && (pChild->GetID() != "area_samma") )
		{
			if ( (GetState() == UI_STATE_ICON_MOVING) && (pChild->UIType() == UI_TYPE_ICON) && (CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) &&
				((CN3UIIcon *)pChild == CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon) )	continue;
			pChild->Render();
		}
		if ( pChild->m_szID == strDummy )
			CGameBase::s_pPlayer->InventoryChrRender(rcRegion);
		if ( (pChild->UIType() == UI_TYPE_ICON) && (pChild->GetStyle() & UISTYLE_ICON_HIGHLIGHT)  && (!m_bOpenningNow) && (!m_bClosingNow))
		{
			bTooltipRender = true;
			spItem = GetHighlightIconItem( (CN3UIIcon* )pChild );
		}
	}

	if (m_bDestoyDlgAlive)
		m_pArea_Destroy->Render();

	if ( (GetState() == UI_STATE_ICON_MOVING) && (CN3UIWndBase::m_sSelectedIconInfo.pItemSelect))
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->Render();		

	// Displays the number of items to be displayed.
	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pMyInvWnd[i] && ((m_pMyInvWnd[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE) || 
					(m_pMyInvWnd[i]->pItemBasic->byContable == UIITEM_TYPE_COUNTABLE_SMALL)) )
		{
			// Get string...
			CN3UIString* pStr = GetChildStringByiOrder(i);
			if(pStr) 
			{
				if ( (GetState() == UI_STATE_ICON_MOVING) && (m_pMyInvWnd[i] == CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) )
				{
					pStr->SetVisible(false);
				}
				else
				{
					if ( m_pMyInvWnd[i]->pUIIcon->IsVisible() )
					{
						pStr->SetVisible(true);
						pStr->SetStringAsInt(m_pMyInvWnd[i]->iCount);
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
			CN3UIString* pStr = GetChildStringByiOrder(i);
			if(pStr) 
				pStr->SetVisible(false);
		}
	}

	// If in repair mode.. return;
	if (m_eInvenState == INV_STATE_REPAIR) { CGameProcedure::s_pProcMain->m_pUIRepairTooltip->Render(); return;	}
	
	if ( bTooltipRender && spItem )
		m_pUITooltipDlg->DisplayTooltipsEnable(ptCur.x, ptCur.y, spItem );
}

void CUIInventory::InitIconWnd(e_UIWND eWnd)
{
	m_pArea_User = (CN3UIArea *)GetChildByID("area_char"); __ASSERT(m_pArea_User, "NULL UI Component!!");
	if(nullptr == m_pArea_User) return;

	m_pArea_Destroy = (CN3UIArea *)GetChildByID("area_samma"); __ASSERT(m_pArea_Destroy, "NULL UI Component!!");
	if(nullptr == m_pArea_Destroy) return;

	m_pText_Weight		= (CN3UIString*)GetChildByID("text_weight");	__ASSERT(m_pText_Weight	, "NULL UI Component!!");
	const __TABLE_UI_RESRC* pTblUI = CGameBase::s_pTbl_UI->Find(CGameBase::s_pPlayer->m_InfoBase.eNation);
	__ASSERT(pTblUI, "NULL Pointer UI Table");

	m_pUITooltipDlg = new CUIImageTooltipDlg();
	m_pUITooltipDlg->Init(this);
	m_pUITooltipDlg->LoadFromFile(pTblUI->szItemInfo);
	m_pUITooltipDlg->InitPos();
	m_pUITooltipDlg->SetVisible(FALSE);	

	CN3UIWndBase::InitIconWnd(eWnd);
}

void CUIInventory::UpdateWeight(std::string str)
{
	if(nullptr == m_pText_Weight) return;

	m_pText_Weight->SetString(str);
}

void CUIInventory::InitIconUpdate()
{
	CN3UIArea* pArea;
	const float fUVAspect = (float)45.0f/(float)64.0f;

	for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( m_pMySlot[i] != nullptr)
		{
			m_pMySlot[i]->pUIIcon = new CN3UIIcon;
			m_pMySlot[i]->pUIIcon->Init(this);
			m_pMySlot[i]->pUIIcon->SetTex(m_pMySlot[i]->szIconFN);
			m_pMySlot[i]->pUIIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
			m_pMySlot[i]->pUIIcon->SetUIType(UI_TYPE_ICON);
			m_pMySlot[i]->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, i);
			if ( pArea )
			{
				m_pMySlot[i]->pUIIcon->SetRegion(pArea->GetRegion());
				m_pMySlot[i]->pUIIcon->SetMoveRect(pArea->GetRegion());
			}
			if ( m_pMySlot[i]->iDurability == 0 )
				m_pMySlot[i]->pUIIcon->SetStyle(m_pMySlot[i]->pUIIcon->GetStyle() | UISTYLE_DURABILITY_EXHAUST);	
		}
	}

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( m_pMyInvWnd[i] != nullptr)
		{
			m_pMyInvWnd[i]->pUIIcon = new CN3UIIcon;
			m_pMyInvWnd[i]->pUIIcon->Init(this);
			m_pMyInvWnd[i]->pUIIcon->SetTex(m_pMyInvWnd[i]->szIconFN);
			m_pMyInvWnd[i]->pUIIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
			m_pMyInvWnd[i]->pUIIcon->SetUIType(UI_TYPE_ICON);
			m_pMyInvWnd[i]->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, i);
			if ( pArea )
			{
				m_pMyInvWnd[i]->pUIIcon->SetRegion(pArea->GetRegion());
				m_pMyInvWnd[i]->pUIIcon->SetMoveRect(pArea->GetRegion());
			}
			if ( m_pMyInvWnd[i]->iDurability == 0 )
				m_pMyInvWnd[i]->pUIIcon->SetStyle(m_pMyInvWnd[i]->pUIIcon->GetStyle() | UISTYLE_DURABILITY_EXHAUST);	
		}
	}
}

__IconItemSkill* CUIInventory::GetHighlightIconItem(CN3UIIcon* pUIIcon)
{
	for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( (m_pMySlot[i] != nullptr) && (m_pMySlot[i]->pUIIcon == pUIIcon) )
			return m_pMySlot[i];
	}

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( (m_pMyInvWnd[i] != nullptr) && (m_pMyInvWnd[i]->pUIIcon == pUIIcon) )
			return m_pMyInvWnd[i];
	}
	return nullptr;
}

e_UIWND_DISTRICT CUIInventory::GetWndDistrict(__IconItemSkill* spItem)
{
	for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if ( (m_pMySlot[i] != nullptr) && (m_pMySlot[i] == spItem) )
			return UIWND_DISTRICT_INVENTORY_SLOT;
	}

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( (m_pMyInvWnd[i] != nullptr) && (m_pMyInvWnd[i] == spItem) )
			return UIWND_DISTRICT_INVENTORY_INV;
	}
	return UIWND_DISTRICT_UNKNOWN;
}

int CUIInventory::GetItemiOrder(__IconItemSkill* spItem, e_UIWND_DISTRICT eWndDist)
{
	const int iReturn = -1;
	int i;

	switch ( eWndDist )
	{
		case UIWND_DISTRICT_INVENTORY_SLOT:
			for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )
			{
				if ( (m_pMySlot[i] != nullptr) && (m_pMySlot[i] == spItem) )
					return i;
			}
			break;

		case UIWND_DISTRICT_INVENTORY_INV:
			for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
			{
				if ( (m_pMyInvWnd[i] != nullptr) && (m_pMyInvWnd[i] == spItem) )
					return i;
			}
			break;
	}

	return iReturn;
}

RECT CUIInventory::GetSampleRect()
{
	RECT rect;
	CN3UIArea* pArea;
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
	pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, 0);
	rect = pArea->GetRegion();
	float fWidth = rect.right - rect.left;
	float fHeight = rect.bottom - rect.top;
	fWidth *= 0.5f; fHeight *= 0.5f;
	rect.left = ptCur.x - (int)fWidth;  rect.right  = ptCur.x + (int)fWidth;
	rect.top  = ptCur.y - (int)fHeight; rect.bottom = ptCur.y + (int)fHeight;
	return rect;
}

DWORD CUIInventory::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;
	if (CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer) { dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);  return dwRet; }

	// If in repair mode.. return;
	if (m_eInvenState == INV_STATE_REPAIR) { dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);  return dwRet; }

	if (m_bDestoyDlgAlive)	
	{
		const CN3UIImage* pImg = (CN3UIImage* )m_pArea_Destroy->GetChildByID("img_Destroy");
		__ASSERT(pImg, "NULL UI Component!!");

		if (!pImg) return dwRet;
		if (!pImg->IsIn(ptCur.x, ptCur.y))
		{
			// this_ui_add_start
			// When the window to destroy is open, if you move out of the window and click in the inventory window, the character moves to prevent it...
			if(IsIn(ptCur.x, ptCur.y))
				dwRet |= UI_MOUSEPROC_INREGION;
			// this_ui_add_end

			return dwRet;
		}

		for(auto i = 0; i < m_pArea_Destroy->GetChildrenCount(); i++ )
		{
			CN3UIBase* pChild = m_pArea_Destroy->GetChildByIndex(i);
			if (pChild)	
				dwRet |= pChild->MouseProc(dwFlags, ptCur, ptOld);  
		}
		return dwRet;
	}

	// Renew dragged icon..
	if ( (GetState() == UI_STATE_ICON_MOVING) && 
			(CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd == UIWND_INVENTORY) &&
			(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) )
	{
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetRegion(GetSampleRect());
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetMoveRect(GetSampleRect());
	}

	return CN3UIWndBase::MouseProc(dwFlags, ptCur, ptOld);
}

void CUIInventory::SendInvMsg(BYTE bDir, int iItemID, int SrcPos, int DestPos)
{
	BYTE byBuff[100];												// buffer..
	int iOffset=0;												// Offset...

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ITEM_MOVE);				// Item Move
	CAPISocket::MP_AddByte(byBuff, iOffset, bDir);						
	CAPISocket::MP_AddDword(byBuff, iOffset, iItemID);					
	CAPISocket::MP_AddByte(byBuff, iOffset, (byte)SrcPos);				
	CAPISocket::MP_AddByte(byBuff, iOffset, (byte)DestPos);						

	TRACE("Send Inv Move %d, %d, %d, %d \n", bDir, iItemID, SrcPos, DestPos );

	CGameProcedure::s_pProcMain->s_pSocket->Send(byBuff, iOffset);									// sent...
}

int CUIInventory::GetInvDestinationIndex(__IconItemSkill* spItem)
{
	if( CN3UIWndBase::m_sSelectedIconInfo.pItemSelect == nullptr)
		return false;

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if (!m_pMyInvWnd[i])
			return i;
	}

	return -1;
}

int	CUIInventory::GetArmDestinationIndex(__IconItemSkill* spItem)
{
	if( CN3UIWndBase::m_sSelectedIconInfo.pItemSelect == nullptr)
		return false;

	__TABLE_ITEM_BASIC*		pItem;
	pItem = CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic;

	e_PartPosition ePart;
	e_PlugPosition ePlug;
	const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, nullptr, ePart, ePlug); // Create a file name according to the item
	if(ITEM_TYPE_UNKNOWN == eType) return false;

	if ( IsValidRaceAndClass(pItem, CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt) )
	{
		switch ( pItem->byAttachPoint )
		{
			case ITEM_ATTACH_POS_DUAL:
				if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT] && m_pMySlot[ITEM_SLOT_POS_HAND_LEFT])	// If both sides...
					return ITEM_SLOT_POS_HAND_RIGHT;				// Right if you have both.
				if (!m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT])	// If you're not on the right...
					return ITEM_SLOT_POS_HAND_RIGHT;
				else
				{
					if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]->pItemBasic->byAttachPoint == ITEM_ATTACH_POS_TWOHAND_RIGHT)
						return ITEM_SLOT_POS_HAND_RIGHT;
					else
						return ITEM_SLOT_POS_HAND_LEFT;
				}
				return -1;

			case ITEM_ATTACH_POS_HAND_RIGHT:
				return ITEM_SLOT_POS_HAND_RIGHT;

			case ITEM_ATTACH_POS_HAND_LEFT:
				return ITEM_SLOT_POS_HAND_LEFT;

			case ITEM_ATTACH_POS_TWOHAND_RIGHT:				// When holding a two-handed sword in the right hand...
				if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT] && m_pMySlot[ITEM_SLOT_POS_HAND_LEFT])	// If both sides...
					return -1;
				else 
					return ITEM_SLOT_POS_HAND_RIGHT;

			case ITEM_ATTACH_POS_TWOHAND_LEFT:				// When holding a two-handed sword in the right hand...
				if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT] && m_pMySlot[ITEM_SLOT_POS_HAND_LEFT])	// If both sides...
					return -1;
				else 
					return ITEM_SLOT_POS_HAND_LEFT;

			case ITEM_ATTACH_POS_EAR:
				if (!m_pMySlot[ITEM_SLOT_POS_EAR_RIGHT])	// If you're not on the right...
					return ITEM_SLOT_POS_EAR_RIGHT;
				if (!m_pMySlot[ITEM_SLOT_POS_EAR_LEFT])		// If not on the left...
					return ITEM_SLOT_POS_EAR_LEFT;
				return ITEM_SLOT_POS_EAR_RIGHT;				// Right if you have both.

			case ITEM_ATTACH_POS_HEAD:		
				return ITEM_SLOT_POS_HEAD;

			case ITEM_ATTACH_POS_NECK:		
				return ITEM_SLOT_POS_NECK;

			case ITEM_ATTACH_POS_UPPER:
				return ITEM_SLOT_POS_UPPER;

			case ITEM_ATTACH_POS_CLOAK:
				return ITEM_SLOT_POS_SHOULDER;

			case ITEM_ATTACH_POS_BELT:
				return ITEM_SLOT_POS_BELT;

			case ITEM_ATTACH_POS_FINGER:
				if (!m_pMySlot[ITEM_SLOT_POS_RING_RIGHT])	// If you're not on the right...
					return ITEM_SLOT_POS_RING_RIGHT;
				if (!m_pMySlot[ITEM_SLOT_POS_RING_LEFT])		// If not on the left...
					return ITEM_SLOT_POS_RING_LEFT;
				return ITEM_SLOT_POS_RING_RIGHT;				// Right if you have both.

			case ITEM_ATTACH_POS_LOWER:
				return ITEM_SLOT_POS_LOWER;

			case ITEM_ATTACH_POS_ARM:
				return ITEM_SLOT_POS_GLOVES;

			case ITEM_ATTACH_POS_FOOT:
				return ITEM_SLOT_POS_SHOES;
			default:
				return -1;
		}
	}			

	return -1;
}

bool CUIInventory::CheckIconDropIfSuccessSendToServer(__IconItemSkill* spItem)
{
	// First check if the item can fit...
	bool  bFound = false;
	bool  bArm = true;
	CN3UIArea* pArea;
	int   i, iDestiOrder;

	if (!m_bRBtnProcessing)
	{
		const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
		for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )
		{
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, i);
			if ( pArea && pArea->IsIn(ptCur.x, ptCur.y) )
			{
				bFound = true;
				bArm = true;
				iDestiOrder = i;
				break;
			}
		}
		if ( !bFound )
		{
			for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, i);
				if ( pArea && pArea->IsIn(ptCur.x, ptCur.y) )
				{
					bFound = true;
					bArm = false;
					iDestiOrder = i;
					break;
				}
			}
		}

		if ( !bFound )		// If you can't find it, search the inventory character area.
		{
			if ( m_pArea_User->IsIn(ptCur.x, ptCur.y) )
			{	
				// If it is not an icon in the inventory area.. false return..
				if (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_INV)
				{
					iDestiOrder = GetArmDestinationIndex(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect);
					if (iDestiOrder != -1)
					{
						bFound = true;
						bArm = true;
					}
				}
			}
		}
	}
	else
	{
		if (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_INV)
		{
			iDestiOrder = GetArmDestinationIndex(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect);
			if (iDestiOrder != -1)
			{
				bFound = true;
				bArm = true;
			}
		}
		else
		{
			iDestiOrder = GetInvDestinationIndex(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect);
			if (iDestiOrder != -1)
			{
				bFound = true;
				bArm = false;
			}				
		}
	}

	if ( !bFound )	return false;	// I couldn't find it, so I failed...

	// Start using Recovery Info in earnest.
	// First, set WaitFromServer to On.. Copy Select Info to Recovery Info..
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer				= true;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemSource					= CN3UIWndBase::m_sSelectedIconInfo.pItemSelect;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWnd			= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict = CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder		= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder;
	CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWnd			= UIWND_INVENTORY;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= nullptr;
	// During inspection, create pItemTarget if necessary during Recovery Info, and return to original state when false is returned.

	// arm -> arm
	if ( (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_SLOT) && bArm )
	{
		// Check if there is an existing icon and if there is no existing item...
		if ( !m_pMySlot[iDestiOrder] )
		{
			if ( IsValidPosFromArmToArm(iDestiOrder) )
			{
				// If the icon can be entered.. Create backup information in case the server fails..
				// And write information about what to do if the server gives success.
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_SLOT;				
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

				// Send To Server..
				SendInvMsg(0x04, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
					CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
				return true;
			}
			else
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= nullptr;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;
				return false;
			}
		}
		// If you have an existing icon...
		else
		{
			// If you can put an icon...
			if ( IsValidPosFromArmToArm(iDestiOrder) )
			{
				// Set pItemTarget and UIWndTargetStart with existing icon information.
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[iDestiOrder];
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= iDestiOrder;
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;

				// If the opposite is possible...
				if ( IsValidPosFromArmToArmInverse(CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder) )
				{
					// If the icon can be entered.. Create backup information in case the server fails..
					// And write information about what to do if the server gives success.
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_SLOT;				
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_SLOT;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;

					// Send To Server..
					SendInvMsg(0x04, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
						CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
						CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
					return true;
				}
			}
			else
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= nullptr;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;
				return false;
			}
		}
	}
	// Arm -> Inv
	else if ( (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_SLOT) && !bArm )
	{
		// Check if there is an existing icon and if there is no existing item...
		if ( !m_pMyInvWnd[iDestiOrder] )
		{
			// Of course you can enter the icon.. ^^
			// If the icon can be entered.. Create backup information in case the server fails..
			// And write information about what to do if the server gives success.
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;				
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

			// Send To Server..
			SendInvMsg(0x02, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
			return true;
		}
		// If you have an existing icon...
		else
		{
			// Find an empty inventory slot.
			bFound = false;
			int i = 0;
			for(; i < MAX_ITEM_INVENTORY; i++ )
			{
				if ( !m_pMyInvWnd[i] )
				{
					bFound = true;
					break;
				}
			}

			if ( bFound )	// If you find an empty slot...
			{
				// If the icon can be entered.. Create backup information in case the server fails..
				// And write information about what to do if the server gives success.
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;				
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= i;

				// Send To Server..
				SendInvMsg(0x02, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
					CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, i);
				return true;
			}
			else
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= nullptr;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;
				return false;
			}
		}
	}
	// Inv -> Arm
	else if ( (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_INV) && bArm )
	{
		// In the case of mounting on a sword, there is no need to check if there is an existing icon.. Because the checking function does it..
		// Check if there is an existing icon and if there is no existing item...
		if ( !m_pMySlot[iDestiOrder] )
		{
			if ( IsValidPosFromInvToArm(iDestiOrder) )
			{
				// If the icon can be entered.. Create backup information in case the server fails..
				// And write information about what to do if the server gives success.
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_SLOT;				
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

				// Send To Server..
				SendInvMsg(0x01, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
					CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
				return true;
			}
			else
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= nullptr;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;
				return false;
			}
		}
		// If you have an existing icon...
		else
		{
			if ( IsValidPosFromInvToArm(iDestiOrder) )
			{
				// If the icon can be entered.. Create backup information in case the server fails..
				// And write information about what to do if the server gives success.
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_SLOT;				
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

				// Unless it's mounted on a sword...
				if ( (iDestiOrder != ITEM_SLOT_POS_HAND_RIGHT) && (iDestiOrder != ITEM_SLOT_POS_HAND_LEFT) )
				{
					CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[iDestiOrder];
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= iDestiOrder;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
				}
				// Send To Server..
				SendInvMsg(0x01, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
					CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
					CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
				return true;
			}
			else
			{
				CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= nullptr;
				CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;
				return false;
			}
		}
	}
	// Inv -> Inv
	else if ( (CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict == UIWND_DISTRICT_INVENTORY_INV) && !bArm )
	{
		// Check if there is an existing icon and if there is no existing item...
		if ( !m_pMyInvWnd[iDestiOrder] )
		{
			// If the icon can be entered.. Create backup information in case the server fails..
			// And write information about what to do if the server gives success.
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;				
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

			// Send To Server..
			SendInvMsg(0x03, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
			return true;
		}
		// If you have an existing icon...
		else
		{
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;				
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder			= iDestiOrder;

			CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMyInvWnd[iDestiOrder];
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_INV;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= iDestiOrder;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
			CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
			// Send To Server..
			SendInvMsg(0x03, CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic->dwID+
				CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemExt->dwID, 
				CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder, iDestiOrder);
			return true;
		}
	}
	
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= nullptr;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;
	return false;
}

inline	bool CUIInventory::InvOpsSomething(__IconItemSkill* spItem)
{
	if (!spItem) return false;
	const CN3UIArea* pArea = nullptr;

	// Check. If successful, send to server..
	if ( CheckIconDropIfSuccessSendToServer(spItem) )												
	{																								
		// Moving the icon.. Source.. If you move within the same icon.. You don't have to remove and add it, just move it..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )											
		{																							
			// eliminate..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict )				
			{																						
				case UIWND_DISTRICT_INVENTORY_SLOT:														
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = nullptr;		
					break;																			

				case UIWND_DISTRICT_INVENTORY_INV:													
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = nullptr;	
					break;																			
			}																						
		}																							
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )											
		{																							
			// eliminate..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict )				
			{																							
				case UIWND_DISTRICT_INVENTORY_SLOT:													
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder] = nullptr;		
					break;																			

				case UIWND_DISTRICT_INVENTORY_INV:													
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder] = nullptr;	
					break;																				
			}																						
		}																							

		// Moving the icon.. Source.. If you move within the same icon.. You don't have to remove and add it, just move it..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )											
		{																								
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource;				

			// addition..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict )				
			{																						
				case UIWND_DISTRICT_INVENTORY_SLOT:													
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder);	
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = spItem;												
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());				
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());			
					break;																													

				case UIWND_DISTRICT_INVENTORY_INV:																							
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder);	
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = spItem;											
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());			
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());			
					break;																													
			}																																
		}																																	

		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )																					
		{																																	
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget;														

			// addition..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict )														
			{																																
				case UIWND_DISTRICT_INVENTORY_SLOT:																								
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder);	
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder] = spItem;												
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());				
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());			
					break;																													

				case UIWND_DISTRICT_INVENTORY_INV:																							
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder);		
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder] = spItem;													
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());			
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());			
					break;																													
			}																																
		}																																	

		CN3UIWndBase::AllHighLightIconFree();																								
		SetState(UI_STATE_COMMON_NONE);																										
		return true;																														
	}																																		
	else																																	
	{																																		
		CN3UIWndBase::AllHighLightIconFree();																								
		SetState(UI_STATE_COMMON_NONE);																										
		return false;																														
	}			
}

bool CUIInventory::ReceiveIconDrop(__IconItemSkill* spItem, POINT ptCur)
{
	const CN3UIArea* pArea = nullptr;

	if (!m_bVisible) return false;
	if (!spItem) return false;

	// If it's not the icon I had...
	if ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd != m_eUIWnd )
		return false;

	// These are the icons I had and within the inventory i.e. Arm->Arm, Arm->Inv, Inv->Arm, Inv->Inv.
	// If it is the same as the selected icon...
	switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
	{
		case UIWND_DISTRICT_INVENTORY_SLOT:
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
			break;	

		case UIWND_DISTRICT_INVENTORY_INV:
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
			break;	
	}

	if ( (pArea) && (pArea->IsIn(ptCur.x, ptCur.y)) )
	{
		if (GetState()  == UI_STATE_ICON_MOVING)
		{
			if (spItem) PlayItemSound(spItem->pItemBasic);
			CN3UIWndBase::AllHighLightIconFree();																								
			SetState(UI_STATE_COMMON_NONE);																										
			return false;
		}
	}
	else if ( m_pArea_Destroy->IsIn(ptCur.x, ptCur.y) )
	{
		m_bDestoyDlgAlive = true;

		// can't move...
		const RECT rect = { 0, 0, 0, 0 };

		switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
		{
			case UIWND_DISTRICT_INVENTORY_SLOT:
				if ( m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != nullptr)
				{
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(m_pArea_Destroy->GetRegion());
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(rect);
				}
				break;

			case UIWND_DISTRICT_INVENTORY_INV:
				if ( m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != nullptr)
				{
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(m_pArea_Destroy->GetRegion());
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(rect);
				}
				break;
		}

		CN3UIWndBase::m_sRecoveryJobInfo.pItemSource					= CN3UIWndBase::m_sSelectedIconInfo.pItemSelect;
		CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWnd			= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd;
		CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict = CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict;
		CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder		= CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder;

		CN3UIWndBase::AllHighLightIconFree();																								
		SetState(UI_STATE_COMMON_NONE);	
		return true;
	}
	else
		return InvOpsSomething(spItem);

	return false;
}

void CUIInventory::ReceiveResultFromServer(BYTE bResult)
{
	const CN3UIArea* pArea = nullptr;

	if (bResult == 0x01)		// success..
	{
		// The icon has changed, so move the actual data..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )
		{
			const __IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource;
			e_ItemSlot eSlot = ITEM_SLOT_UNKNOWN;

			// eliminate..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					eSlot = (e_ItemSlot)CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
					if (!spItem) return;
					this->ItemDelete(spItem->pItemBasic, spItem->pItemExt, eSlot);
					break;
			}
		}

		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )
		{
			const __IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget;
			if (!spItem) return;
			e_ItemSlot eSlot = ITEM_SLOT_UNKNOWN;

			// eliminate..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					eSlot = (e_ItemSlot)CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder;
					if (!spItem) return;
					this->ItemDelete(spItem->pItemBasic,  spItem->pItemExt, eSlot);
					break;
			}
		}

		// The icon has changed, so move the actual data..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )
		{
			const __IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource;
			if (!spItem) return;

			// addition..
			e_ItemSlot eSlot = ITEM_SLOT_UNKNOWN;
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					eSlot = (e_ItemSlot)CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder;
					if (!spItem) return;
					ItemAdd(spItem->pItemBasic, spItem->pItemExt, eSlot);
					break;
			}
		}

		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )
		{
			const __IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget;
			if (!spItem) return;
			e_ItemSlot eSlot = ITEM_SLOT_UNKNOWN;

			// addition..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					eSlot = (e_ItemSlot)CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder;
					if (!spItem) return;
					ItemAdd(spItem->pItemBasic, spItem->pItemExt, eSlot);
					break;
			}
		}
	}
	else						// failure..
	{
		// Return the icon to its original state.
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )
		{
			// eliminate..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = nullptr;
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceEnd.iOrder] = nullptr;
					break;
			}
		}

		// Move Icon.. Target..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )
		{
			// eliminate..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder] = nullptr;
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder] = nullptr;
					break;
			}
		}

		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource )
		{
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource;

			// addition..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder);
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = spItem;
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder);
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = spItem;
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
					break;
			}
		}

		// Move Icon.. Target..
		if ( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget )
		{
			__IconItemSkill*	spItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget;

			// addition..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder);
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder] = spItem;
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder);
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder] = spItem;
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
					break;
			}
		}
	}

	CN3UIWndBase::AllHighLightIconFree();
	SetState(UI_STATE_COMMON_NONE);

	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer  = false;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemSource		= nullptr;
	CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget		= nullptr;

	if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
	if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();
}

void CUIInventory::CancelIconDrop(__IconItemSkill* spItem)
{
	CN3UIWndBase::AllHighLightIconFree();
	SetState(UI_STATE_COMMON_NONE);
}

void CUIInventory::AcceptIconDrop(__IconItemSkill* spItem)
{
	CN3UIWndBase::AllHighLightIconFree();
	SetState(UI_STATE_COMMON_NONE);
}

void CUIInventory::IconRestore()
{
	CN3UIArea* pArea;

	switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
	{
		case UIWND_DISTRICT_INVENTORY_SLOT:
			if ( m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != nullptr)
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
				if ( pArea )
				{
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
			}
			break;

		case UIWND_DISTRICT_INVENTORY_INV:
			if ( m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != nullptr)
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
				if ( pArea )
				{
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
			}
			break;
	}
}

int CUIInventory::GetIndexInArea(POINT pt)
{
	CN3UIArea* pArea;
	RECT	rect;

	for (int i = 0; i < ITEM_SLOT_COUNT; i++)
	{
		pArea = nullptr;
		pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, i);
		if (pArea)
		{
			rect = pArea->GetRegion();

			if ( (pt.x >= rect.left) && (pt.x <= rect.right) && (pt.y >= rect.top) && (pt.y <= rect.bottom) )
				return i;
		}
	}

	for (auto i = 0; i < MAX_ITEM_INVENTORY; i++)
	{
		pArea = nullptr;
		pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, i);
		if (pArea)
		{
			rect = pArea->GetRegion();

			if ( (pt.x >= rect.left) && (pt.x <= rect.right) && (pt.y >= rect.top) && (pt.y <= rect.bottom) )
				return i + ITEM_SLOT_COUNT;
		}
	}

	return -1;
}

bool CUIInventory::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
// Temp Define
#define FAIL_CODE {		\
				SetState(UI_STATE_COMMON_NONE);	\
				return false;	\
			}

	// Code Begin
	if(nullptr == pSender) return false;
	__IconItemSkill* spItem = nullptr;
	e_UIWND_DISTRICT eUIWnd;
	int iOrder;
	const DWORD dwBitMask = 0x0f1f0000;

	if (dwMsg == UIMSG_BUTTON_CLICK)					
	{
		if(pSender->m_szID == "btn_close")
		{
			// When the inventory is floating...
			Close();
		}
	}

	if (m_eInvenState == INV_STATE_REPAIR) FAIL_CODE
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();

	switch (dwMsg & dwBitMask)
	{
		case UIMSG_ICON_RDOWN_FIRST:
			m_iRBtnDownOffs = GetIndexInArea(ptCur);
			break;

		case UIMSG_ICON_RUP:
			{
				int iRBtn = GetIndexInArea(ptCur);
				if (iRBtn != -1 && m_iRBtnDownOffs != -1 && m_iRBtnDownOffs == iRBtn)
				{
					bool bSlot;
					if ((iRBtn - ITEM_SLOT_COUNT) >= 0)
					{
						bSlot = false;
						iRBtn -= ITEM_SLOT_COUNT;
					}
					else
						bSlot = true;
					
					// Get Item..
					spItem = GetHighlightIconItem((CN3UIIcon* )pSender);

					CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd = UIWND_INVENTORY;
					if (bSlot)
						CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
					else
						CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict = UIWND_DISTRICT_INVENTORY_INV;
					
					CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder = iRBtn;
					CN3UIWndBase::m_sSelectedIconInfo.pItemSelect = spItem;
					
					if (spItem) PlayItemSound(spItem->pItemBasic);

					// ..
					m_bRBtnProcessing = true;
					InvOpsSomething(spItem);
					m_bRBtnProcessing = false;
				}
			}
			break;

		case UIMSG_AREA_DOWN_FIRST:
			// I'm trading between individuals... If my ID is "area_gold"...
			// Call a function in SubProcPerTrade.. (the function is called.. if you are not editing)
			if ( (CGameProcedure::s_pProcMain->m_pSubProcPerTrade->m_ePerTradeState == PER_TRADE_STATE_NORMAL) &&
				(pSender->m_szID.compare("area_gold") == 0)	)
					CGameProcedure::s_pProcMain->m_pSubProcPerTrade->RequestItemCountEdit();
				break;
			break;

		case UIMSG_ICON_DOWN_FIRST:
			CN3UIWndBase::AllHighLightIconFree();

			// Get Item..
			spItem = GetHighlightIconItem((CN3UIIcon* )pSender);

			// Save Select Info..
			CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd = UIWND_INVENTORY;
			eUIWnd = GetWndDistrict(spItem);
			if ( eUIWnd == UIWND_DISTRICT_UNKNOWN )	FAIL_CODE
			CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict = eUIWnd;
			iOrder = GetItemiOrder(spItem, eUIWnd);
			if ( iOrder == -1 )	FAIL_CODE
			CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder = iOrder;
			CN3UIWndBase::m_sSelectedIconInfo.pItemSelect = spItem;
			// Do Ops..
			((CN3UIIcon* )pSender)->SetRegion(GetSampleRect());
			((CN3UIIcon* )pSender)->SetMoveRect(GetSampleRect());
			// Sound..
			if (spItem) PlayItemSound(spItem->pItemBasic);
			break;

		case UIMSG_ICON_UP:
			// Walk around and inspect the Icon Manager windows.
			if ( !CGameProcedure::s_pUIMgr->BroadcastIconDropMsg(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) )
				// Set the icon position back to normal.
				IconRestore();
			else
			{
				if (CN3UIWndBase::m_sSelectedIconInfo.pItemSelect) PlayItemSound(CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic);
			}
			SetState(UI_STATE_COMMON_NONE);
			break;

		case UIMSG_ICON_DOWN:
			if ( GetState()  == UI_STATE_ICON_MOVING )
			{
				CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetRegion(GetSampleRect());
				CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pUIIcon->SetMoveRect(GetSampleRect());
			}
			break;

		case UIMSG_ICON_DBLCLK:
			SetState(UI_STATE_COMMON_NONE);

			// Set the icon position back to normal.
			IconRestore();				
			break;			
	}

	return true;
}

bool CUIInventory::IsValidRaceAndClass(__TABLE_ITEM_BASIC* pItem, __TABLE_ITEM_EXT* pItemExt)
{
	int iNeedValue;

	char szBuf[256] = "";
	bool	bValid = false;

	if (!pItem) return false;

	// tribe..
	switch ( pItem->byNeedRace )
	{
		case 0:
			bValid = true;
			break;

		default:
			if ( pItem->byNeedRace == CGameBase::s_pPlayer->m_InfoBase.eRace )
				bValid = true;
			break;
	}

	const __InfoPlayerMySelf*	pInfoExt = nullptr;
	pInfoExt = &(CGameBase::s_pPlayer->m_InfoExt);
	if (!pInfoExt)	return false;

	std::string szMsg;
	if ( bValid )
	{
		// job..
		if (pItem->byNeedClass != 0)
		{
			switch (pItem->byNeedClass)
			{
				case CLASS_KINDOF_WARRIOR:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_WARRIOR:
						case CLASS_KA_BERSERKER:
						case CLASS_KA_GUARDIAN:
						case CLASS_EL_WARRIOR:
						case CLASS_EL_BLADE:
						case CLASS_EL_PROTECTOR:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_ROGUE:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_ROGUE:
						case CLASS_KA_HUNTER:
						case CLASS_KA_PENETRATOR:
						case CLASS_EL_ROGUE:
						case CLASS_EL_RANGER:
						case CLASS_EL_ASSASIN:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_WIZARD:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_WIZARD:
						case CLASS_KA_SORCERER:
						case CLASS_KA_NECROMANCER:
						case CLASS_EL_WIZARD:
						case CLASS_EL_MAGE:
						case CLASS_EL_ENCHANTER:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_PRIEST:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_PRIEST:
						case CLASS_KA_SHAMAN:
						case CLASS_KA_DARKPRIEST:
						case CLASS_EL_PRIEST:
						case CLASS_EL_CLERIC:
						case CLASS_EL_DRUID:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_ATTACK_WARRIOR:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_BERSERKER:
						case CLASS_EL_BLADE:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_DEFEND_WARRIOR:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_GUARDIAN:
						case CLASS_EL_PROTECTOR:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_ARCHER:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_HUNTER:
						case CLASS_EL_RANGER:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_ASSASSIN:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_PENETRATOR:
						case CLASS_EL_ASSASIN:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_ATTACK_WIZARD:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_SORCERER:
						case CLASS_EL_MAGE:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_PET_WIZARD:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_NECROMANCER:
						case CLASS_EL_ENCHANTER:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_HEAL_PRIEST:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_SHAMAN:
						case CLASS_EL_CLERIC:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				case CLASS_KINDOF_CURSE_PRIEST:
					switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
					{
						case CLASS_KA_DARKPRIEST:
						case CLASS_EL_DRUID:
							break;
						default:
							::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
							CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
							return false;
					}
					break;

				default:
					if (CGameBase::s_pPlayer->m_InfoBase.eClass != pItem->byClass)
					{
						::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_CLASS, szMsg);
						CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
						return false;
					}
					break;
			}					
		}

		if ( (pItem->byAttachPoint == ITEM_LIMITED_EXHAUST) && (CGameBase::s_pPlayer->m_InfoBase.iLevel < pItem->cNeedLevel+pItemExt->siNeedLevel) )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_LEVEL, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		if ( pInfoExt->iRank < pItem->byNeedRank+pItemExt->siNeedRank )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_RANK, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}
		
		if ( pInfoExt->iTitle < pItem->byNeedTitle+pItemExt->siNeedTitle )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_TITLE, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		iNeedValue = pItem->byNeedStrength;
		if (iNeedValue != 0)
			iNeedValue += pItemExt->siNeedStrength;
		if ( pInfoExt->iStrength < iNeedValue )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_POWER, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		iNeedValue = pItem->byNeedStamina;
		if (iNeedValue != 0)
			iNeedValue += pItemExt->siNeedStamina;
		if ( pInfoExt->iStamina < iNeedValue )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_STR, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		iNeedValue = pItem->byNeedDexterity;
		if (iNeedValue != 0)
			iNeedValue += pItemExt->siNeedDexterity;
		if ( pInfoExt->iDexterity < iNeedValue )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_DEX, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff1010);
			return false;
		}

		iNeedValue = pItem->byNeedInteli;
		if (iNeedValue != 0)
			iNeedValue += pItemExt->siNeedInteli;
		if ( pInfoExt->iIntelligence < iNeedValue )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_INT, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		iNeedValue = pItem->byNeedMagicAttack;
		if (iNeedValue != 0)
			iNeedValue += pItemExt->siNeedMagicAttack;
		if ( pInfoExt->iMagicAttak < iNeedValue )
		{
			::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_LOW_CHA, szMsg);
			CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
			return false;
		}

		return true;
	}
	else
	{
		::_LoadStringFromResource(IDS_MSG_VALID_CLASSNRACE_INVALID_RACE, szMsg);
		CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);
	}
	return false;
}

bool CUIInventory::IsValidPosFromInvToArm(int iOrder)
{
	if( CN3UIWndBase::m_sSelectedIconInfo.pItemSelect == nullptr)
		return false;

	__TABLE_ITEM_BASIC*		pItem;
	pItem = CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic;

	e_PartPosition ePart;
	e_PlugPosition ePlug;
	const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, nullptr, ePart, ePlug); // Create a file name according to the item
	if(ITEM_TYPE_UNKNOWN == eType) return false;

	if ( IsValidRaceAndClass(pItem, CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt) )
	{
		switch ( iOrder )
		{
			case ITEM_SLOT_POS_EAR_RIGHT:
			case ITEM_SLOT_POS_EAR_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_EAR:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HEAD:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_HEAD:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_NECK:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_NECK:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_UPPER:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_UPPER:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_SHOULDER:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_CLOAK:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_RIGHT:				// right hand..
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
					case ITEM_ATTACH_POS_HAND_RIGHT:
						// Did you know that there are two-handed weapons in one hand?
						if ( (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]) 
							&& (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]->pItemBasic->byAttachPoint == ITEM_ATTACH_POS_TWOHAND_LEFT) )
						{
							// Returns false if there is a weapon in the right hand.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]) 
								return false;
							// If there is no weapon in the right hand.. Left hand weapon is in the inventory.. ^^
							else
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_LEFT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_LEFT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
						}
						else	// If you don't have a two-handed weapon in your left hand...
						{
							// If there is a weapon in the right hand.. Item Exchange.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]) 
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_RIGHT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
							// If there is no weapon in the right hand.. Returns true..
							else
								return true;
						}
						break;

					case ITEM_ATTACH_POS_TWOHAND_RIGHT:				// When holding a two-handed sword in the right hand...
						if ( m_pMySlot[ITEM_SLOT_POS_HAND_LEFT] )	// He has a weapon in his left hand...
						{
							// Returns false if there is a weapon in the right hand.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]) 
								return false;
							// If there is no weapon in the right hand.. The weapon in the left hand goes to the inventory.. ^^
							else
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_LEFT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_LEFT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
						}
						else										// There is no weapon in the left hand.
						{
							// If there is a weapon in the right hand.. Item Exchange.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]) 
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_RIGHT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
							// If there is no weapon in the right hand.. Returns true..
							else
								return true;
						}
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
					case ITEM_ATTACH_POS_HAND_LEFT:
						// Did you know that there is a two-handed weapon in the right hand?
						if ( (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]) 
							&& (m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT]->pItemBasic->byAttachPoint == ITEM_ATTACH_POS_TWOHAND_RIGHT) )
						{
							// Returns false if there is a weapon in the left hand.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]) 
								return false;
							// If there is no weapon in the left hand.. the right hand weapon is in the inventory.. ^^
							else
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_RIGHT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
						}
						else	// If you don't have a two-handed weapon in your right hand...
						{
							// If there is a weapon in the left hand.. Item Exchange.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]) 
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_LEFT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_LEFT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
							// If there is no weapon in the left hand.. Returns true..
							else
								return true;
						}
						break;

					case ITEM_ATTACH_POS_TWOHAND_LEFT:				// When holding a two-handed sword in the left hand...
						if ( m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT] )	// He has a weapon in his right hand...
						{
							// Returns false if there is a weapon in the left hand.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]) 
								return false;
							// If there is no weapon in the left hand.. the right hand weapon is in the inventory.. ^^
							else
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_RIGHT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_RIGHT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
						}
						else	// If you don't have a two-handed weapon in your right hand...
						{
							// If there is a weapon in the left hand.. Item Exchange.. ^^
							if (m_pMySlot[ITEM_SLOT_POS_HAND_LEFT]) 
							{
								CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget					= m_pMySlot[ITEM_SLOT_POS_HAND_LEFT];
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.UIWndDistrict = UIWND_DISTRICT_INVENTORY_SLOT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetStart.iOrder		= ITEM_SLOT_POS_HAND_LEFT;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWnd			= UIWND_INVENTORY;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.UIWndDistrict	= UIWND_DISTRICT_INVENTORY_INV;
								CN3UIWndBase::m_sRecoveryJobInfo.UIWndTargetEnd.iOrder			= CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder;
								return true;
							}
							// If there is no weapon in the left hand.. Returns true..
							else
								return true;
						}
						break;
				}
				break;

			case ITEM_SLOT_POS_BELT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_BELT:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_RING_RIGHT:
			case ITEM_SLOT_POS_RING_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_FINGER:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_LOWER:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_LOWER:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_GLOVES:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_ARM:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_SHOES:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_FOOT:
						return true;
						break;
				}
				break;
		}
	}
	return false;
}

bool CUIInventory::IsValidPosFromArmToArm(int iOrder)
{
	if( CN3UIWndBase::m_sRecoveryJobInfo.pItemSource == nullptr)
		return false;

	__TABLE_ITEM_BASIC*		pItem;
	pItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemSource->pItemBasic;

	e_PartPosition ePart;
	e_PlugPosition ePlug;
	const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, nullptr, ePart, ePlug); // Create a file name according to the item
	if(ITEM_TYPE_UNKNOWN == eType) return false;

	if ( IsValidRaceAndClass(pItem, CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt) )
	{
		switch ( iOrder )
		{
			case ITEM_SLOT_POS_EAR_RIGHT:
			case ITEM_SLOT_POS_EAR_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_EAR:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_RIGHT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_RING_RIGHT:
			case ITEM_SLOT_POS_RING_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_FINGER:
						return true;
						break;
				}
				break;
		}
	}
	return false;
}

bool CUIInventory::IsValidPosFromArmToArmInverse(int iOrder)
{
	if( CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget == nullptr)
		return false;

	__TABLE_ITEM_BASIC*		pItem;
	pItem = CN3UIWndBase::m_sRecoveryJobInfo.pItemTarget->pItemBasic;

	e_PartPosition ePart;
	e_PlugPosition ePlug;
	const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, nullptr, ePart, ePlug); // Create a file name according to the item
	if(ITEM_TYPE_UNKNOWN == eType) return false;

	if ( IsValidRaceAndClass(pItem, CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt) )
	{
		switch (iOrder)
		{
			case ITEM_SLOT_POS_EAR_RIGHT:
			case ITEM_SLOT_POS_EAR_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_EAR:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_RIGHT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_HAND_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_DUAL:
						return true;
						break;
				}
				break;

			case ITEM_SLOT_POS_RING_RIGHT:
			case ITEM_SLOT_POS_RING_LEFT:
				switch ( pItem->byAttachPoint )
				{
					case ITEM_ATTACH_POS_FINGER:
						return true;
						break;
				}
				break;
		}
	}

	return false;
}

void CUIInventory::ItemAdd(__TABLE_ITEM_BASIC* pItem, __TABLE_ITEM_EXT* pItemExt, e_ItemSlot eSlot)
{
	std::string szFN;
	e_PartPosition ePart;
	e_PlugPosition ePlug;
	const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, &szFN, nullptr, ePart, ePlug); // Create a file name according to the item

	if(ITEM_TYPE_PLUG == eType)
	{
		if(ITEM_SLOT_HAND_LEFT == eSlot) ePlug = PLUG_POS_LEFTHAND;
		else if(ITEM_SLOT_HAND_RIGHT == eSlot) ePlug = PLUG_POS_RIGHTHAND;
		else { __ASSERT(0, "Invalid Item Plug Position"); }

		CGameBase::s_pPlayer->PlugSet(ePlug, szFN, pItem, pItemExt); // plug settings...
		CGameBase::s_pPlayer->DurabilitySet(eSlot, m_pMySlot[eSlot]->iDurability);
	}
	else if(ITEM_TYPE_PART == eType)
	{
		CGameBase::s_pPlayer->PartSet(ePart, szFN, pItem, pItemExt);
		CGameBase::s_pPlayer->DurabilitySet(eSlot, m_pMySlot[eSlot]->iDurability);
	}
}

void CUIInventory::ItemDelete(__TABLE_ITEM_BASIC* pItem, __TABLE_ITEM_EXT* pItemExt, e_ItemSlot eSlot)
{
	const __TABLE_PLAYER_LOOKS* pLooks = CGameBase::s_pTbl_UPC_Looks->Find(CGameBase::s_pPlayer->m_InfoBase.eRace);	// User Player Character Skin structure pointer..
	__ASSERT(pLooks, "NULL Basic Looks!");
	if(nullptr == pLooks) return;

	e_PartPosition ePart;
	e_PlugPosition ePlug;
	const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, nullptr, ePart, ePlug); // Create a file name according to the item
	
	if(pLooks)
	{
		if(ITEM_TYPE_PLUG == eType)
		{
			if(ITEM_SLOT_HAND_LEFT == eSlot) ePlug = PLUG_POS_LEFTHAND;
			else if(ITEM_SLOT_HAND_RIGHT == eSlot) ePlug = PLUG_POS_RIGHTHAND;

			CGameBase::s_pPlayer->PlugSet(ePlug, "", nullptr, nullptr); // plug settings...
		}
		else if(ITEM_TYPE_PART == eType)
		{
			if(PART_POS_HAIR_HELMET == ePart) CGameBase::s_pPlayer->InitHair();
			else CGameBase::s_pPlayer->PartSet(ePart, pLooks->szPartFNs[ePart], nullptr, nullptr);
		}
	}
}

void CUIInventory::DurabilityChange(e_ItemSlot eSlot, int iDurability)
{
	char szBuf[512];
	std::string szDur; 

	if ( eSlot < ITEM_SLOT_EAR_RIGHT || eSlot >= ITEM_SLOT_COUNT )
	{
		__ASSERT(0, "Durability Change Item Index Weird.");
		CLogWriter::Write("Durability Change Item Index Weird. Slot(%d) Durability(%d)", eSlot, iDurability);
		return;
	}

	if ( m_pMySlot[eSlot] )
	{
		m_pMySlot[eSlot]->iDurability = iDurability;
		if ( iDurability == 0 )
		{
			if(m_pMySlot[eSlot]->pUIIcon && m_pMySlot[eSlot]->pItemBasic) 
			{
				m_pMySlot[eSlot]->pUIIcon->SetStyle(m_pMySlot[eSlot]->pUIIcon->GetStyle() | UISTYLE_DURABILITY_EXHAUST);
				// message box output.
				::_LoadStringFromResource(IDS_DURABILITY_EXOAST, szDur);
				wsprintf(szBuf, szDur.c_str(), m_pMySlot[eSlot]->pItemBasic->szName.c_str() );
				CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffff3b3b);
			}
			else
			{
				__ASSERT(0, "Durability Change Item NULL icon or NULL item.");
				CLogWriter::Write("Durability Change Item NULL icon or NULL item. Slot(%d) Durability(%d)", eSlot, iDurability);
			}
		}
	}
	else
	{
		__ASSERT(0, "Durability Change Item NULL Slot.");
		CLogWriter::Write("Durability Change Item ... NULL Slot. Slot(%d) Durability(%d)", eSlot, iDurability);
	}
}

void CUIInventory::ReceiveResultFromServer(int iResult, int iUserGold)
{
	m_cItemRepairMgr.ReceiveResultFromServer(iResult, iUserGold);
}

int CUIInventory::GetCountInInvByID(int iID)
{
	int i;
	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if ( (m_pMyInvWnd[i] != nullptr) && (m_pMyInvWnd[i]->pItemBasic->dwID == (iID/1000*1000)) &&
				(m_pMyInvWnd[i]->pItemExt->dwID == (iID%1000)) )
		return m_pMyInvWnd[i]->iCount;
	}

	return 0;
}

void CUIInventory::ItemCountChange(int iDistrict, int iIndex, int iCount, int iID)
{
	__IconItemSkill* spItem;
	switch (iDistrict)
	{
		case 0x00:
			if (m_pMySlot[iIndex] && ( (m_pMySlot[iIndex]->pItemBasic->dwID+m_pMySlot[iIndex]->pItemExt->dwID) != iID ) )	// If you have the wrong item...
			{
				// Delete item.. current inventory window only..
				spItem = m_pMySlot[iIndex];

				// Also delete it from your inventory.
				m_pMySlot[iIndex] = nullptr;

				// Delete my manager's item from the list with iOrder.
				RemoveChild(spItem->pUIIcon);

				// Delete Icon Resource...
				spItem->pUIIcon->Release();
				delete spItem->pUIIcon;
				spItem->pUIIcon = nullptr;
				delete spItem;
				spItem = nullptr;

				// Create an item...
				__TABLE_ITEM_BASIC* pItem = nullptr;								// Item table structure pointer..
				__TABLE_ITEM_EXT* pItemExt = nullptr;								// Item table structure pointer..

				pItem = CGameProcedure::s_pTbl_Items_Basic->Find(iID/1000*1000);	// Get column data...
				if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
					pItemExt = CGameProcedure::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iID%1000);	// Get column data...
				if (nullptr == pItem || nullptr == pItemExt )
				{
					__ASSERT(0, "NULL Item");
					CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", iID);
					return;
				}

				e_PartPosition ePart;
				e_PlugPosition ePlug;
				std::string szIconFN;
				const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item
				if(ITEM_TYPE_UNKNOWN == eType) CLogWriter::Write("MyInfo - slot - Unknown Item");
				__ASSERT(ITEM_TYPE_UNKNOWN != eType, "Unknown Item");
				
				spItem = new __IconItemSkill;
				spItem->pItemBasic	= pItem;
				spItem->pItemExt	= pItemExt;
				spItem->szIconFN = szIconFN; // Copy icon filename..
				spItem->iCount	= iCount;
				spItem->iDurability = pItem->siMaxDurability + pItemExt->siMaxDurability;
				m_pMySlot[iIndex] = spItem;
				return;
			}
			else if (m_pMySlot[iIndex])
			{
				m_pMySlot[iIndex]->iCount = iCount;
				if (iCount == 0)
				{
					// Delete item.. current inventory window only..
					__IconItemSkill* spItem;
					spItem = m_pMySlot[iIndex];

					// Also delete it from your inventory.
					m_pMySlot[iIndex] = nullptr;

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
			else	// If there are no items...
			{
				// Create an item...
				__TABLE_ITEM_BASIC* pItem = nullptr;								// Item table structure pointer..
				__TABLE_ITEM_EXT* pItemExt = nullptr;								// Item table structure pointer..

				pItem = CGameProcedure::s_pTbl_Items_Basic->Find(iID/1000*1000);	// Get column data...
				if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
					pItemExt = CGameProcedure::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iID%1000);	// Get column data...
				if (nullptr == pItem || nullptr == pItemExt )
				{
					__ASSERT(0, "NULL Item");
					CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", iID);
					return;
				}

				e_PartPosition ePart;
				e_PlugPosition ePlug;
				std::string szIconFN;
				const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item
				if(ITEM_TYPE_UNKNOWN == eType) CLogWriter::Write("MyInfo - slot - Unknown Item");
				__ASSERT(ITEM_TYPE_UNKNOWN != eType, "Unknown Item");
				
				spItem = new __IconItemSkill;
				spItem->pItemBasic	= pItem;
				spItem->pItemExt	= pItemExt;
				spItem->szIconFN = szIconFN; // Copy icon filename..
				spItem->iCount	= iCount;
				spItem->iDurability = pItem->siMaxDurability + pItemExt->siMaxDurability;
				m_pMySlot[iIndex] = spItem;
				return;
			}
			break;

		case 0x01:
			if (m_pMyInvWnd[iIndex] && ( (m_pMyInvWnd[iIndex]->pItemBasic->dwID+m_pMyInvWnd[iIndex]->pItemExt->dwID) != iID ) )	// If you have the wrong item...
			{
				// Delete item.. current inventory window only..
				spItem = m_pMyInvWnd[iIndex];

				// Also delete it from your inventory.
				m_pMyInvWnd[iIndex] = nullptr;

				// Delete my manager's item from the list with iOrder.
				RemoveChild(spItem->pUIIcon);

				// Delete Icon Resource...
				spItem->pUIIcon->Release();
				delete spItem->pUIIcon;
				spItem->pUIIcon = nullptr;
				delete spItem;
				spItem = nullptr;

				// Create an item...
				__TABLE_ITEM_BASIC* pItem = nullptr;								// Item table structure pointer..
				__TABLE_ITEM_EXT* pItemExt = nullptr;								// Item table structure pointer..

				pItem = CGameProcedure::s_pTbl_Items_Basic->Find(iID/1000*1000);	// Get column data...
				if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
					pItemExt = CGameProcedure::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iID%1000);	// Get column data...
				if (nullptr == pItem || nullptr == pItemExt )
				{
					__ASSERT(0, "NULL Item");
					CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", iID);
					return;
				}

				e_PartPosition ePart;
				e_PlugPosition ePlug;
				std::string szIconFN;
				const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item
				if(ITEM_TYPE_UNKNOWN == eType) CLogWriter::Write("MyInfo - slot - Unknown Item");
				__ASSERT(ITEM_TYPE_UNKNOWN != eType, "Unknown Item");
				
				spItem = new __IconItemSkill;
				spItem->pItemBasic	= pItem;
				spItem->pItemExt	= pItemExt;
				spItem->szIconFN = szIconFN; // Copy icon filename..
				spItem->iCount	= iCount;
				spItem->iDurability = pItem->siMaxDurability + pItemExt->siMaxDurability;
				m_pMyInvWnd[iIndex] = spItem;
				return;
			}
			else if (m_pMyInvWnd[iIndex])
			{
				m_pMyInvWnd[iIndex]->iCount = iCount;
				if (iCount == 0)
				{
					// Delete item.. current inventory window only..
					__IconItemSkill* spItem;
					spItem = m_pMyInvWnd[iIndex];

					// Also delete it from your inventory.
					m_pMyInvWnd[iIndex] = nullptr;

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
			else	// If there are no items...
			{
				// Create an item...
				__TABLE_ITEM_BASIC* pItem = nullptr;								// Item table structure pointer..
				__TABLE_ITEM_EXT* pItemExt = nullptr;								// Item table structure pointer..

				pItem = CGameProcedure::s_pTbl_Items_Basic->Find(iID/1000*1000);	// Get column data...
				if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
					pItemExt = CGameProcedure::s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iID%1000);	// Get column data...
				if (nullptr == pItem || nullptr == pItemExt )
				{
					__ASSERT(0, "NULL Item");
					CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", iID);
					return;
				}

				e_PartPosition ePart;
				e_PlugPosition ePlug;
				std::string szIconFN;
				const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item
				if(ITEM_TYPE_UNKNOWN == eType) CLogWriter::Write("MyInfo - slot - Unknown Item");
				__ASSERT(ITEM_TYPE_UNKNOWN != eType, "Unknown Item");
				
				spItem = new __IconItemSkill;
				spItem->pItemBasic	= pItem;
				spItem->pItemExt	= pItemExt;
				spItem->szIconFN = szIconFN; // Copy icon filename..
				spItem->iCount	= iCount;
				spItem->iDurability = pItem->siMaxDurability + pItemExt->siMaxDurability;
				m_pMyInvWnd[iIndex] = spItem;

				m_pMyInvWnd[iIndex]->pUIIcon = new CN3UIIcon;
				m_pMyInvWnd[iIndex]->pUIIcon->Init(this);
				m_pMyInvWnd[iIndex]->pUIIcon->SetTex(m_pMyInvWnd[iIndex]->szIconFN);
				const float fUVAspect = (float)45.0f/(float)64.0f;
				m_pMyInvWnd[iIndex]->pUIIcon->SetUVRect(0,0, fUVAspect, fUVAspect);
				m_pMyInvWnd[iIndex]->pUIIcon->SetUIType(UI_TYPE_ICON);
				m_pMyInvWnd[iIndex]->pUIIcon->SetStyle(UISTYLE_ICON_ITEM|UISTYLE_ICON_CERTIFICATION_NEED);
				const CN3UIArea* pArea = nullptr;
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, iIndex);
				if ( pArea )
				{
					m_pMyInvWnd[iIndex]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMyInvWnd[iIndex]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
				return;
			}
			break;
	}
}

void CUIInventory::ItemDestroyOK()
{
	m_bDestoyDlgAlive = false;

	BYTE byBuff[32];															// Packet buffer...
	int iOffset=0;																// Packet Offset...

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ITEM_DESTROY);					// Game start packet commands..

	switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
	{
		case UIWND_DISTRICT_INVENTORY_SLOT:
			CAPISocket::MP_AddByte(byBuff, iOffset, 0x01);						// ID Length Packet..
			break;
		case UIWND_DISTRICT_INVENTORY_INV:
			CAPISocket::MP_AddByte(byBuff, iOffset, 0x02);						// ID Length Packet..
			break;
	}
	CAPISocket::MP_AddByte(byBuff, iOffset, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);	// ID Length Packet..
	CAPISocket::MP_AddDword(byBuff, iOffset, CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemBasic->dwID+
		CN3UIWndBase::m_sSelectedIconInfo.pItemSelect->pItemExt->dwID);	// Put the ID string in the packet..

	CGameProcedure::s_pSocket->Send(byBuff, iOffset);	

	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer	= true;
}

void CUIInventory::ItemDestroyCancel()
{
	m_bDestoyDlgAlive = false;
	const CN3UIArea* pArea = nullptr;

	switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
	{
		case UIWND_DISTRICT_INVENTORY_SLOT:
			if ( m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != nullptr)
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
				if ( pArea )
				{
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
			}
			break;

		case UIWND_DISTRICT_INVENTORY_INV:
			if ( m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != nullptr)
			{
				pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
				if ( pArea )
				{
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
					m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
				}
			}
			break;
	}
}

void CUIInventory::ReceiveResultItemRemoveFromServer(int iResult)
{
	const CN3UIArea* pArea = nullptr;
	__IconItemSkill* spItem = nullptr;
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer	= false;

	switch (iResult)
	{
		case 0x01:			// success..
			switch ( CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					spItem = m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder];
					// Erase my area too..
					m_pMySlot[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = nullptr;
					this->ItemDelete(spItem->pItemBasic, spItem->pItemExt, (e_ItemSlot)CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder);
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					spItem = m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder];
					// Erase my area too..
					m_pMyInvWnd[CN3UIWndBase::m_sRecoveryJobInfo.UIWndSourceStart.iOrder] = nullptr;
					break;
			}

			// Delete my manager's item from the list with iOrder.
			if (!spItem) return;
			RemoveChild(spItem->pUIIcon);

			// Delete Icon Resource...
			spItem->pUIIcon->Release();
			delete spItem->pUIIcon;
			spItem->pUIIcon = nullptr;
			delete spItem;
			spItem = nullptr;
			break;

		case 0x00:			// failure..
			switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWndDistrict )
			{
				case UIWND_DISTRICT_INVENTORY_SLOT:
					if ( m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != nullptr)
					{
						pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SLOT, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
						if ( pArea )
						{
							m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
							m_pMySlot[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
						}
					}
					break;

				case UIWND_DISTRICT_INVENTORY_INV:
					if ( m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder] != nullptr)
					{
						pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_INV, CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder);
						if ( pArea )
						{
							m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
							m_pMyInvWnd[CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
						}
					}
					break;
			}
			break;
	}

	if (CGameProcedure::s_pProcMain->m_pUISkillTreeDlg) CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->UpdateDisableCheck();
	if (CGameProcedure::s_pProcMain->m_pUIHotKeyDlg) CGameProcedure::s_pProcMain->m_pUIHotKeyDlg->UpdateDisableCheck();
}

bool CUIInventory::CheckWeightValidate(__IconItemSkill* spItem)
{
	std::string szMsg;

	if (!spItem)	return false;

	const __InfoPlayerMySelf*	pInfoExt = &(CGameBase::s_pPlayer->m_InfoExt);
	if ( (pInfoExt->iWeight + spItem->pItemBasic->siWeight) > pInfoExt->iWeightMax)
	{	 
		::_LoadStringFromResource(IDS_ITEM_WEIGHT_OVERFLOW, szMsg);	
		CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff3b3b);	
		return false;	
	}

	return true;
}

// this_ui_add_start
bool CUIInventory::OnKeyPress(int iKey)
{
	if(m_bDestoyDlgAlive && m_pArea_Destroy)
	{
		switch(iKey)
		{
		case DIK_RETURN:
			{
				CN3UIButton* pBtnDestroyOk = (CN3UIButton* )m_pArea_Destroy->GetChildByID("btn_Destroy_ok");
				if(pBtnDestroyOk) m_pArea_Destroy->ReceiveMessage(pBtnDestroyOk, UIMSG_BUTTON_CLICK);
				else return false;
			}
			return true;
		case DIK_ESCAPE:
			{
				CN3UIButton* pBtnDestroyCancel = (CN3UIButton* )m_pArea_Destroy->GetChildByID("btn_Destroy_cancel");
				if(pBtnDestroyCancel) m_pArea_Destroy->ReceiveMessage(pBtnDestroyCancel, UIMSG_BUTTON_CLICK);
				else return false;
			}
			return true;
		}
	}
	else
	{
		switch(iKey)
		{
		case DIK_ESCAPE:
			if(!m_bClosingNow) this->Close();
			if(m_pUITooltipDlg) m_pUITooltipDlg->DisplayTooltipsDisable();
			return true;
		}
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CUIInventory::SetVisible(bool bVisible)
{
	CN3UIBase::SetVisible(bVisible);
	if(bVisible)
		CGameProcedure::s_pUIMgr->SetVisibleFocusedUI(this);
	else
		CGameProcedure::s_pUIMgr->ReFocusUI();

	m_iRBtnDownOffs = -1;
}

void CUIInventory::SetVisibleWithNoSound(bool bVisible, bool bWork, bool bReFocus)
{
	CN3UIBase::SetVisibleWithNoSound(bVisible, bWork, bReFocus);

	if(bWork)
	{
		if ( m_eInvenState == INV_STATE_REPAIR )
		{
			CGameProcedure::RestoreGameCursor();

			if (CGameProcedure::s_pProcMain->m_pUIRepairTooltip->IsVisible())
			{
				CGameProcedure::s_pProcMain->m_pUIRepairTooltip->m_bBRender = false;
				CGameProcedure::s_pProcMain->m_pUIRepairTooltip->DisplayTooltipsDisable();
			}
		}

		m_eInvenState = INV_STATE_NORMAL;

		if (GetState() == UI_STATE_ICON_MOVING)
			IconRestore();
		SetState(UI_STATE_COMMON_NONE);
		CN3UIWndBase::AllHighLightIconFree();

		if(m_pUITooltipDlg) m_pUITooltipDlg->DisplayTooltipsDisable();
	}

	if(bReFocus)
	{
		if(bVisible)
			CGameProcedure::s_pUIMgr->SetVisibleFocusedUI(this);
		else
			CGameProcedure::s_pUIMgr->ReFocusUI();
	}

	m_iRBtnDownOffs = -1;
}

int CUIInventory::GetIndexItemCount(DWORD dwIndex)
{
	int iCnt = 0;

	for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )
	{
		if(m_pMySlot[i] && m_pMySlot[i]->pItemBasic)
		{
			if(m_pMySlot[i]->pItemBasic->dwID == dwIndex)
				iCnt += m_pMySlot[i]->iCount;
		}
	}

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )
	{
		if(m_pMyInvWnd[i] && m_pMyInvWnd[i]->pItemBasic)
		{
			if(m_pMyInvWnd[i]->pItemBasic->dwID == dwIndex)
				iCnt += m_pMyInvWnd[i]->iCount;
		}
	}

	return iCnt;
}
// this_ui_add_end
