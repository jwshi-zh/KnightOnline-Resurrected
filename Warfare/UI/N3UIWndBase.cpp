#include "pch.h"
#include "N3UIWndBase.h"

#include "GameProcMain.h"
#include "UIImageTooltipDlg.h"
#include "UITransactionDlg.h"
#include "UIManager.h"
#include "CountableItemEditDlg.h"

#include "N3UITooltip.h"
#include "N3SndObj.h"

__InfoSelectedIcon		CN3UIWndBase::m_sSelectedIconInfo;
__RecoveryJobInfo		CN3UIWndBase::m_sRecoveryJobInfo;
__SkillSelectInfo		CN3UIWndBase::m_sSkillSelectInfo;

CN3UIImage* CN3UIWndBase::m_pSelectionImage = nullptr;
CCountableItemEditDlg*	CN3UIWndBase::m_pCountableItemEdit = nullptr;

CN3SndObj* CN3UIWndBase::s_pSnd_Item_Etc = nullptr;
CN3SndObj* CN3UIWndBase::s_pSnd_Item_Weapon = nullptr;
CN3SndObj* CN3UIWndBase::s_pSnd_Item_Armor = nullptr;
CN3SndObj* CN3UIWndBase::s_pSnd_Gold = nullptr;
CN3SndObj* CN3UIWndBase::s_pSnd_Repair = nullptr;
int CN3UIWndBase::s_iRefCount = 0;

CN3UIWndBase::CN3UIWndBase()
{
	m_pSelectionImage = nullptr;
	m_pSelectionImage = new CN3UIImage;
	m_pSelectionImage->Init(CGameProcedure::s_pUIMgr);
	m_pSelectionImage->SetUVRect(0.0f, 0.0f, 1.0f, 1.0f);

	if(s_iRefCount == 0)
	{
		s_pSnd_Item_Etc		= CN3Base::s_SndMgr.CreateObj(ID_SOUND_ITEM_ETC_IN_INVENTORY);
		s_pSnd_Item_Weapon	= CN3Base::s_SndMgr.CreateObj(ID_SOUND_ITEM_WEAPON_IN_INVENTORY);
		s_pSnd_Item_Armor	= CN3Base::s_SndMgr.CreateObj(ID_SOUND_ITEM_ARMOR_IN_INVENTORY);
		s_pSnd_Gold		= CN3Base::s_SndMgr.CreateObj(ID_SOUND_GOLD_IN_INVENTORY);	
		s_pSnd_Repair	= CN3Base::s_SndMgr.CreateObj(ID_SOUND_ITEM_IN_REPAIR);	
	}
	s_iRefCount++; // reference count
}

CN3UIWndBase::~CN3UIWndBase()
{
	s_iRefCount--; // reference count
	if(s_iRefCount == 0)
	{
		CN3Base::s_SndMgr.ReleaseObj(&s_pSnd_Item_Etc);
		CN3Base::s_SndMgr.ReleaseObj(&s_pSnd_Item_Weapon);
		CN3Base::s_SndMgr.ReleaseObj(&s_pSnd_Item_Armor);
		CN3Base::s_SndMgr.ReleaseObj(&s_pSnd_Gold);
		CN3Base::s_SndMgr.ReleaseObj(&s_pSnd_Repair);
	}
}

void CN3UIWndBase::InitIconWnd(e_UIWND eWnd)
{
	m_eUIWnd = eWnd;
}

CN3UIArea* CN3UIWndBase::GetChildAreaByiOrder(eUI_AREA_TYPE eUAT, int iOrder)
{
	char pszID[32];
	sprintf(pszID, "%d",iOrder);

	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		auto* pChild = (CN3UIArea* )(*itor);
		if ( (pChild->UIType() == UI_TYPE_AREA) && (pChild->m_eAreaType == eUAT) )
		{
			if (pChild->m_szID == pszID) return pChild;
		}
	}

	return nullptr;
}

CN3UIString* CN3UIWndBase::GetChildStringByiOrder(int iOrder)
{
	char pszID[32];
	sprintf(pszID, "%d",iOrder);

	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		auto* pChild = (CN3UIString* )(*itor);
		if ( pChild->UIType() == UI_TYPE_STRING )
		{
			if (pChild->m_szID == pszID) return pChild;
		}
	}

	return nullptr;

}

void CN3UIWndBase::AllHighLightIconFree()
{
	for(auto itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if ( pChild->UIType() == UI_TYPE_ICON )
			pChild->SetStyle(pChild->GetStyle() & (~UISTYLE_ICON_HIGHLIGHT));	
	}
}

DWORD CN3UIWndBase::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	// A window with an icon should receive messages from its children even when it leaves its window area.

	// UI moving code
	if (UI_STATE_COMMON_MOVE == m_eState)
	{
		if (dwFlags&UI_MOUSE_LBCLICKED)
		{
			SetState(UI_STATE_COMMON_NONE);
		}
		else
		{
			MoveOffset(ptCur.x - ptOld.x, ptCur.y - ptOld.y);
		}
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if(false == IsIn(ptCur.x, ptCur.y))	// out of range
	{
		if(false == IsIn(ptOld.x, ptOld.y))
		{
			if ( GetState() != UI_STATE_ICON_MOVING )
				return dwRet;// If the previous coordinates are also outside the domain
		}
		dwRet |= UI_MOUSEPROC_PREVINREGION;	// The previous coordinates were inside the area.
	}
	else
	{
		// tool tip related
		if (s_pTooltipCtrl) s_pTooltipCtrl->SetText(m_szToolTip);
	}
	dwRet |= UI_MOUSEPROC_INREGION;	// This coordinate is inside the area.

	// Send message to child
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		const DWORD dwChildRet = pChild->MouseProc(dwFlags, ptCur, ptOld);
		if (UI_MOUSEPROC_DONESOMETHING & dwChildRet)
		{	
			// In this case, it is a case where the distance is focused.
			dwRet |= (UI_MOUSEPROC_CHILDDONESOMETHING|UI_MOUSEPROC_DONESOMETHING);
			return dwRet;
		}
	}

	// UI moving code
	if (UI_STATE_COMMON_MOVE != m_eState && 
			PtInRect(&m_rcMovable, ptCur) && (dwFlags&UI_MOUSE_LBCLICK) )
	{
		// If it's the inventory window and you're doing commerce...
		if ( (UIType() == UI_TYPE_ICON_MANAGER) && (m_eUIWnd == UIWND_INVENTORY) 
			&& (CGameProcedure::s_pProcMain->m_pUITransactionDlg->IsVisible()) )
			return dwRet;
		SetState(UI_STATE_COMMON_MOVE);
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	return dwRet;
}

void CN3UIWndBase::PlayItemEtcSound()
{
	if (s_pSnd_Item_Etc)	s_pSnd_Item_Etc->Play();
}

void CN3UIWndBase::PlayItemWeaponSound()
{
	if (s_pSnd_Item_Weapon)	s_pSnd_Item_Weapon->Play();
}

void CN3UIWndBase::PlayItemArmorSound()
{
	if (s_pSnd_Item_Armor)	s_pSnd_Item_Armor->Play();
}

void CN3UIWndBase::PlayGoldSound()
{
	if (s_pSnd_Gold)	s_pSnd_Gold->Play();
}

void CN3UIWndBase::PlayRepairSound()
{
	if (s_pSnd_Repair)	s_pSnd_Repair->Play();
}

void CN3UIWndBase::PlayItemSound(__TABLE_ITEM_BASIC* pBasic)
{
	if (!pBasic) return;
	switch (pBasic->dwID/100000000)
	{
		case 1:
			PlayItemWeaponSound();
			break;

		case 2:
			PlayItemArmorSound();
			break;

		default:
			PlayItemEtcSound();
			break;
	}
}