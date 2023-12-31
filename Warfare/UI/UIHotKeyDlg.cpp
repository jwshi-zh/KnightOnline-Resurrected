#include "pch.h"
#include "UIHotKeyDlg.h"

#include "LocalInput.h"
#include "GameProcedure.h"
#include "GameProcMain.h"
#include "PlayerMySelf.h"
#include "UISkillTreeDlg.h"
#include "MagicSkillMng.h"
#include "UIManager.h"
#include "UIInventory.h"

CUIHotKeyDlg::CUIHotKeyDlg()
{
	m_iCurPage		= 0;
	m_iSelectIndex	= -1;
	m_iSelectPage	= -1;

	for(auto i = 0; i < MAX_SKILL_HOTKEY_PAGE; i++ )
		for(auto j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
			m_pMyHotkey[i][j] = nullptr;

	for(auto j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
	{
		m_pCountStr[j] = nullptr;
		m_pTooltipStr[j] = nullptr;
	}
}

CUIHotKeyDlg::~CUIHotKeyDlg()
{
	Release();	
}

void CUIHotKeyDlg::Release()
{
	for(auto i = 0; i < MAX_SKILL_HOTKEY_PAGE; i++ )
	{
		for(auto j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
		{
			if ( m_pMyHotkey[i][j] != nullptr)
			{
				delete m_pMyHotkey[i][j];
				m_pMyHotkey[i][j] = nullptr;
			}
		}
	}

	for(auto j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
	{
		m_pCountStr[j] = nullptr;
		m_pTooltipStr[j] = nullptr;
	}

	m_iCurPage		= 0;
	m_iSelectIndex	= -1;
	m_iSelectPage	= -1;

	CN3UIBase::Release();
}

void CUIHotKeyDlg::ReleaseItem()
{
	for(auto i = 0; i < MAX_SKILL_HOTKEY_PAGE; i++ )
	{
		for(auto j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
		{
			if ( m_pMyHotkey[i][j] != nullptr)
			{
				if ( m_pMyHotkey[i][j]->pUIIcon )
				{
					RemoveChild(m_pMyHotkey[i][j]->pUIIcon);
					m_pMyHotkey[i][j]->pUIIcon->Release();
					delete m_pMyHotkey[i][j]->pUIIcon;
					m_pMyHotkey[i][j]->pUIIcon = nullptr;
				}

				delete m_pMyHotkey[i][j];
				m_pMyHotkey[i][j] = nullptr;
			}
		}
	}

	m_iCurPage = 0;
}

DWORD CUIHotKeyDlg::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if ( !IsVisible() ) { dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);  return dwRet; }
	// don't actually use it...
	if (CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer) { dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);  return dwRet; }

	// Renew dragged icon..
	if ( GetState() == UI_STATE_ICON_MOVING ) 
	{
		if(CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo)
		{
			CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo->pUIIcon->SetRegion(GetSampleRect());
			CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo->pUIIcon->SetMoveRect(GetSampleRect());
		}
	}

	return CN3UIWndBase::MouseProc(dwFlags, ptCur, ptOld);
}

bool CUIHotKeyDlg::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	// ..
	if(pSender->m_szID == "btn_up")
		PageUp();
	if(pSender->m_szID == "btn_down")
		PageDown();

	__IconItemSkill* spSkill;

	const DWORD dwBitMask = 0x0f0f0000;
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();

	switch (dwMsg & dwBitMask)
	{
		case UIMSG_ICON_DOWN_FIRST:
			// Get Item..
			spSkill = GetHighlightIconItem((CN3UIIcon* )pSender);

			// Save Select Info..
			CN3UIWndBase::m_sSkillSelectInfo.UIWnd = UIWND_HOTKEY;
			CN3UIWndBase::m_sSkillSelectInfo.iOrder = GetAreaiOrder();
			CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo = spSkill;

			// Calc Move Rect Offset..
			if ( !CalcMoveOffset() )
				SetState(UI_STATE_COMMON_NONE);			
			break;

		case UIMSG_ICON_DOWN:
			if ( GetState()  == UI_STATE_ICON_MOVING )
			{
				CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo->pUIIcon->SetRegion(GetSampleRect());
				CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo->pUIIcon->SetMoveRect(GetSampleRect());
			}
			break;

		case UIMSG_ICON_RUP:
			// Inspect while moving around the Hot Key window.
			if ( IsIn(ptCur.x, ptCur.y) )
			{
				const int iOrder = GetAreaiOrder();
				if(m_pMyHotkey[m_iCurPage][iOrder])
				{
					m_iSelectIndex	= iOrder;
					m_iSelectPage	= m_iCurPage;
				}
			}
			SetState(UI_STATE_COMMON_NONE);
			break;

		case UIMSG_ICON_UP:
			// Inspect while moving around the Hot Key window.
			if ( IsIn(ptCur.x, ptCur.y) )
			{
				const int iOrder = GetAreaiOrder();
				if ( CN3UIWndBase::m_sSkillSelectInfo.iOrder == iOrder )	// execution..
				{
					CN3UIArea* pArea;
					pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SKILL_HOTKEY, iOrder);
					if (pArea) 
					{
						m_pMyHotkey[m_iCurPage][iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
						m_pMyHotkey[m_iCurPage][iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
					}	

					DoOperate(m_pMyHotkey[m_iCurPage][iOrder]);
				}
				else	
				{
					if ( iOrder == -1 )
					{
						// Resource Free..
						spSkill = CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo;

						// Removed from manager..
						RemoveChild(spSkill->pUIIcon);

						// Remove resource...
						spSkill->pUIIcon->Release();
						delete spSkill->pUIIcon;
						spSkill->pUIIcon = nullptr;
						delete spSkill;
						spSkill = nullptr;
						m_pMyHotkey[m_iCurPage][CN3UIWndBase::m_sSkillSelectInfo.iOrder] = nullptr;
						if(m_iCurPage == m_iSelectPage && CN3UIWndBase::m_sSkillSelectInfo.iOrder == m_iSelectIndex)
						{
							m_iSelectPage	= -1;
							m_iSelectIndex	= -1;
						}

						CloseIconRegistry();
					}
					else	// moving...
					{
						// If you have an existing icon...
						if ( m_pMyHotkey[m_iCurPage][iOrder] )
						{
							// Delete the existing icon.
							spSkill = m_pMyHotkey[m_iCurPage][iOrder];

							// Removed from manager..
							RemoveChild(spSkill->pUIIcon);

							// Remove resource...
							spSkill->pUIIcon->Release();
							delete spSkill->pUIIcon;
							spSkill->pUIIcon = nullptr;
							delete spSkill;
							spSkill = nullptr;
							m_pMyHotkey[m_iCurPage][iOrder] = nullptr;
						}

						spSkill = m_pMyHotkey[m_iCurPage][CN3UIWndBase::m_sSkillSelectInfo.iOrder];
						m_pMyHotkey[m_iCurPage][iOrder] = spSkill;
						m_pMyHotkey[m_iCurPage][CN3UIWndBase::m_sSkillSelectInfo.iOrder] = nullptr;

						if(m_iCurPage == m_iSelectPage && CN3UIWndBase::m_sSkillSelectInfo.iOrder == m_iSelectIndex)
						{
							m_iSelectPage	= -1;
							m_iSelectIndex	= -1;
						}

						CN3UIArea* pArea;
						pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SKILL_HOTKEY, iOrder);
						if (pArea) 
						{
							m_pMyHotkey[m_iCurPage][iOrder]->pUIIcon->SetRegion(pArea->GetRegion());
							m_pMyHotkey[m_iCurPage][iOrder]->pUIIcon->SetMoveRect(pArea->GetRegion());
						}

						CloseIconRegistry();
					}
				}
			}
			else		// delete..
			{
				// Resource Free..
				spSkill = CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo;

				// Removed from manager..
				RemoveChild(spSkill->pUIIcon);

				// Remove resource...
				spSkill->pUIIcon->Release();
				delete spSkill->pUIIcon;
				spSkill->pUIIcon = nullptr;
				delete spSkill;
				spSkill = nullptr;
				m_pMyHotkey[m_iCurPage][CN3UIWndBase::m_sSkillSelectInfo.iOrder] = nullptr;

				if(m_iCurPage == m_iSelectPage && CN3UIWndBase::m_sSkillSelectInfo.iOrder == m_iSelectIndex)
				{
					m_iSelectPage	= -1;
					m_iSelectIndex	= -1;
				}

				CloseIconRegistry();
			}
			CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo = nullptr;
			SetState(UI_STATE_COMMON_NONE);
			break;

		case UIMSG_ICON_DBLCLK:
			break;
	}

	return false;
}

void CUIHotKeyDlg::Render()
{
	bool bTooltipRender = false;
	__IconItemSkill* pSkill = nullptr;

	if (!m_bVisible) return;	// If not visible, don't render the children.
	DisableTooltipDisplay();
	DisableCountStrDisplay();

	for(auto itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if ( (GetState() == UI_STATE_ICON_MOVING) && (pChild->UIType() == UI_TYPE_ICON) && (CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo) &&
			((CN3UIIcon *)pChild == CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo->pUIIcon) )	continue;
			pChild->Render();
	}

	if ( (GetState() == UI_STATE_ICON_MOVING) && (CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo) )
		CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo->pUIIcon->Render();		

	if(m_iCurPage == m_iSelectPage && m_pMyHotkey[m_iSelectPage][m_iSelectIndex])
	{
		CN3UIIcon* pUIIcon = m_pMyHotkey[m_iSelectPage][m_iSelectIndex]->pUIIcon;
		if(pUIIcon)
			RenderSelectIcon(pUIIcon);
	}

	// on the current page
	CN3UIArea* pArea;
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();

	int k;

	for( k = 0; k < MAX_SKILL_IN_HOTKEY; k++ )
	{
		if (m_pMyHotkey[m_iCurPage][k] != nullptr) 
			DisplayCountStr(m_pMyHotkey[m_iCurPage][k]);
	}

	for( k = 0; k < MAX_SKILL_IN_HOTKEY; k++ )
	{
		if (m_pMyHotkey[m_iCurPage][k] != nullptr) 
		{
			pArea = nullptr;
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SKILL_HOTKEY, k);
			if (pArea && pArea->IsIn(ptCur.x, ptCur.y)) 
			{
				bTooltipRender = true;
				break;
			}
		}
	}

	pSkill = m_pMyHotkey[m_iCurPage][k];

	if (bTooltipRender && pSkill)
		DisplayTooltipStr(pSkill);
}

void CUIHotKeyDlg::Open()
{
	SetVisible(true);

}

void CUIHotKeyDlg::Close()
{
	SetVisible(false);

}

void CUIHotKeyDlg::InitIconWnd(e_UIWND eWnd)
{
	CN3UIWndBase::InitIconWnd(eWnd);
}

void CUIHotKeyDlg::InitIconUpdate()
{
	// Get Hotkey Data From Registry..
	// First, Getting Hotkey Data Count..

	for(auto j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
	{
		m_pTooltipStr[j] = GetTooltipStrControl(j);
		m_pCountStr[j] = GetCountStrControl(j);
	}

	int iHCount = 0;
	if (!CGameProcedure::RegGetSetting("Count", &iHCount, sizeof(int)) )
		return;

	if( (iHCount < 0) || (iHCount > 65) )
		return;

	char szSkill[32];
	int iSkillCount = 0;
	CHotkeyData HD;
	// DWORD bitMask;

	while(iHCount--)
	{
		std::string str = "Data";
		sprintf(szSkill, "%d", iSkillCount);
		str += szSkill;				
		if( CGameProcedure::RegGetSetting(str.c_str(), &HD, sizeof(CHotkeyData)) )
		{
			__TABLE_UPC_SKILL* pUSkill = nullptr;

			// If the Skill Tree Window does not have an ID, continue..
			if ( (HD.iID < UIITEM_TYPE_SONGPYUN_ID_MIN) &&  (!CGameProcedure::s_pProcMain->m_pUISkillTreeDlg->HasIDSkill(HD.iID)) )
				continue;

			pUSkill = CGameBase::s_pTbl_Skill->Find(HD.iID);
			if ( !pUSkill )
				continue;

			auto* spSkill = new __IconItemSkill();
			spSkill->pSkill = pUSkill;

			// Creating an icon name.. ^^
			std::vector<char> buffer(256, NULL);
			sprintf(buffer.data(),	"UI\\skillicon_%.2d_%d.dxt", HD.iID%100, HD.iID/100);
			spSkill->szIconFN = buffer.data();

			// Load the icon.. ^^
			spSkill->pUIIcon = new CN3UIIcon;
			spSkill->pUIIcon->Init(this);
			spSkill->pUIIcon->SetTex(spSkill->szIconFN);
			spSkill->pUIIcon->SetUVRect(0,0,1,1);
			spSkill->pUIIcon->SetUIType(UI_TYPE_ICON);
			spSkill->pUIIcon->SetStyle(UISTYLE_ICON_SKILL);

			const CN3UIArea* pArea = nullptr;
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SKILL_HOTKEY, HD.column);
			if ( pArea )
			{
				spSkill->pUIIcon->SetRegion(pArea->GetRegion());
				spSkill->pUIIcon->SetMoveRect(pArea->GetRegion());
			}

			// Save icon info..
			m_pMyHotkey[HD.row][HD.column] = spSkill;
		}
		iSkillCount++;
	}

	SetHotKeyPage(0);
}

void CUIHotKeyDlg::UpdateDisableCheck() const
{
	int i, j;
	DWORD bitMask;

	for(auto i = 0; i < MAX_SKILL_HOTKEY_PAGE; i++ )
	{
		for( j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
		{
			if ( m_pMyHotkey[i][j] != nullptr)
			{
				bitMask = UISTYLE_ICON_SKILL;
				if (!CGameProcedure::s_pProcMain->m_pMagicSkillMng->CheckValidSkillMagic(m_pMyHotkey[i][j]->pSkill))
					bitMask |= UISTYLE_DISABLE_SKILL;
				m_pMyHotkey[i][j]->pUIIcon->SetStyle(bitMask);
			}
		}
	}
}

void CUIHotKeyDlg::CloseIconRegistry()
{
	// Save Hotkey Data to Registry..
	// First, Saving Hotkey Data Count..

	int i, j;

	int iHCount = 0;
	for(auto i = 0; i < MAX_SKILL_HOTKEY_PAGE; i++ )
	{
		for( j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
		{
			if ( m_pMyHotkey[i][j] != nullptr)
				iHCount++;
		}
	}

	CGameProcedure::RegPutSetting("Count", &iHCount, sizeof(int) );

	char szSkill[32];
	int iSkillCount = 0;

	for(auto i = 0; i < MAX_SKILL_HOTKEY_PAGE; i++ )
	{
		for( j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
		{
			if ( m_pMyHotkey[i][j] != nullptr)
			{
				std::string str = "Data";
				sprintf(szSkill, "%d", iSkillCount);
				str += szSkill;				

				CHotkeyData HD(i, j, m_pMyHotkey[i][j]->pSkill->dwID);
				CGameProcedure::RegPutSetting(str.c_str(), &HD, sizeof(CHotkeyData) );
				iSkillCount++;
			}
		}
	}
}

__IconItemSkill* CUIHotKeyDlg::GetHighlightIconItem(CN3UIIcon* pUIIcon)
{
	for(auto k = 0; k < MAX_SKILL_IN_HOTKEY; k++ )
	{
		if ( (m_pMyHotkey[m_iCurPage][k] != nullptr) && 
			(m_pMyHotkey[m_iCurPage][k]->pUIIcon == pUIIcon) )
			return m_pMyHotkey[m_iCurPage][k];
	}

	return nullptr;
}

void CUIHotKeyDlg::AllFactorClear()
{
	__IconItemSkill *spSkill = nullptr;

	for(auto i = 0; i < MAX_SKILL_HOTKEY_PAGE; i++ )
	{
		for(auto j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
		{
			if ( m_pMyHotkey[i][j] != nullptr)
			{
				// Resource Free..
				spSkill = m_pMyHotkey[i][j];

				// Removed from manager..
				RemoveChild(spSkill->pUIIcon);

				// Remove resource...
				spSkill->pUIIcon->Release();
				delete spSkill->pUIIcon;
				spSkill->pUIIcon = nullptr;
				delete spSkill;
				spSkill = nullptr;
				m_pMyHotkey[i][j] = nullptr;
			}
		}
	}

	m_iCurPage = 0;
}

int	CUIHotKeyDlg::GetAreaiOrder()
{
	// First, search the area.
	CN3UIArea* pArea;
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();

	for(auto i = 0; i < MAX_SKILL_IN_HOTKEY; i++ )
	{
		pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SKILL_HOTKEY, i);
		if (pArea && pArea->IsIn(ptCur.x, ptCur.y))
			return i;
	}

	return -1;
}

bool CUIHotKeyDlg::IsSelectedSkillInRealIconArea()
{
	// First, search the area.
	CN3UIArea* pArea;
	bool bFound = false;
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();

	int i;

	for(i = 0; i < MAX_SKILL_IN_HOTKEY; i++ )
	{
		pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SKILL_HOTKEY, i);
		if (pArea && pArea->IsIn(ptCur.x, ptCur.y))
		{
			bFound = true;
			break;
		}
	}

	if (!bFound) return false;
	if (!CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo) return false;
	SetReceiveSelectedSkill(i);

	return bFound;
}

bool CUIHotKeyDlg::GetEmptySlotIndex(int &iIndex)
{
	__IconItemSkill *spSkill = nullptr;

	for(auto i = 0; i < MAX_SKILL_IN_HOTKEY; i++ )
	{
		if (!m_pMyHotkey[m_iCurPage][i])
		{
			iIndex = i;
			return true;
		}
	}

	return false;
}

void CUIHotKeyDlg::SetReceiveSelectedSkill(int iIndex)
{
	__IconItemSkill *spSkill = nullptr;

	if (m_pMyHotkey[m_iCurPage][iIndex] != nullptr)
	{
		// Resource Free..
		spSkill = m_pMyHotkey[m_iCurPage][iIndex];

		// Removed from manager..
		RemoveChild(spSkill->pUIIcon);

		// Remove resource...
		spSkill->pUIIcon->Release();
		delete spSkill->pUIIcon;
		spSkill->pUIIcon = nullptr;
		delete spSkill;
		spSkill = nullptr;
		m_pMyHotkey[m_iCurPage][iIndex] = nullptr;
	}

	CN3UIArea* pArea;
	pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SKILL_HOTKEY, iIndex);

	// Then... on the spot
	m_pMyHotkey[m_iCurPage][iIndex] = CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo;
	m_pMyHotkey[m_iCurPage][iIndex]->szIconFN = CN3UIWndBase::m_sSkillSelectInfo.pSkillDoneInfo->szIconFN;
	m_pMyHotkey[m_iCurPage][iIndex]->pUIIcon->SetRegion(pArea->GetRegion());
	m_pMyHotkey[m_iCurPage][iIndex]->pUIIcon->SetMoveRect(pArea->GetRegion());
	m_pMyHotkey[m_iCurPage][iIndex]->pUIIcon->SetParent(this);
}

RECT CUIHotKeyDlg::GetSampleRect()
{
	RECT rect;
	CN3UIArea* pArea;
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
	pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SKILL_HOTKEY, 0);
	rect = pArea->GetRegion();
	const float fWidth = rect.right - rect.left;
	const float fHeight = rect.bottom - rect.top;
	rect.left = ptCur.x - m_ptOffset.x;  rect.right  = rect.left + fWidth;
	rect.top  = ptCur.y - m_ptOffset.y;  rect.bottom = rect.top + fHeight;
	return rect;
}

void CUIHotKeyDlg::PageUp()
{
	if ( m_iCurPage <= 0 )
		return;

	SetHotKeyPage(--m_iCurPage);
}

void CUIHotKeyDlg::PageDown()
{
	if ( m_iCurPage >= (MAX_SKILL_HOTKEY_PAGE-1) )
		return;

	SetHotKeyPage(++m_iCurPage);
}

void CUIHotKeyDlg::SetHotKeyPage(int iPageNum)
{
	int i, j;
	
	for(auto i = 0; i < MAX_SKILL_HOTKEY_PAGE; i++ )
	{
		if ( i != iPageNum )
		{
			for( j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
				if ( m_pMyHotkey[i][j] != nullptr)
					m_pMyHotkey[i][j]->pUIIcon->SetVisible(false);
		}
		else
		{
			for( j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
				if ( m_pMyHotkey[i][j] != nullptr)
					m_pMyHotkey[i][j]->pUIIcon->SetVisible(true);
		}
	}

	m_iCurPage = iPageNum;
}

bool CUIHotKeyDlg::CalcMoveOffset()
{
	RECT rect;
	CN3UIArea* pArea;
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();
	const int iOrder = GetAreaiOrder();
	if ( iOrder == -1 ) return false;
	pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SKILL_HOTKEY, iOrder);
	rect = pArea->GetRegion();
	m_ptOffset.x = ptCur.x - rect.left;
	m_ptOffset.y = ptCur.y - rect.top;
	return true;
}

void CUIHotKeyDlg::EffectTriggerByHotKey(int iIndex)
{
	if(iIndex < 0 || iIndex >= 8) return;

	if ( m_pMyHotkey[m_iCurPage][iIndex] && m_pMyHotkey[m_iCurPage][iIndex]->pUIIcon->IsVisible() )
	{
		DoOperate(m_pMyHotkey[m_iCurPage][iIndex]);
	}
}

void CUIHotKeyDlg::DoOperate(__IconItemSkill*	pSkill)
{
	if(!pSkill) return;

	// char szBuf[512];
	// message box output.
	// wsprintf(szBuf, "Skill %s has been used.", pSkill->pSkill->szName.c_str() );
	// CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffffff00);

	PlayRepairSound();

	const int iIDTarget = CGameBase::s_pPlayer->m_iIDTarget;
	CGameProcedure::s_pProcMain->m_pMagicSkillMng->MsgSend_MagicProcess(iIDTarget, pSkill->pSkill);
}

void CUIHotKeyDlg::ClassChangeHotkeyFlush()
{
	__IconItemSkill* spSkill;

	for(auto i = 0; i < MAX_SKILL_HOTKEY_PAGE; i++ )
	{
		for(auto j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
		{
			if ( m_pMyHotkey[i][j] != nullptr)
			{
				// Resource Free..
				spSkill = m_pMyHotkey[i][j];

				// Removed from manager..
				RemoveChild(spSkill->pUIIcon);

				// Remove resource...
				spSkill->pUIIcon->Release();
				delete spSkill->pUIIcon;
				spSkill->pUIIcon = nullptr;
				delete spSkill;
				spSkill = nullptr;
				m_pMyHotkey[i][j] = nullptr;
			}
		}
	}
}

CN3UIString* CUIHotKeyDlg::GetTooltipStrControl(int iIndex)
{
	CN3UIString* pStr = nullptr;
	std::string str = "";
	char	cstr[4];
	sprintf(cstr, "%d", iIndex+10);	str += cstr;
	pStr = (CN3UIString* )GetChildByID(str);	 __ASSERT(pStr, "NULL UI Component!!");
	return pStr;
}

CN3UIString* CUIHotKeyDlg::GetCountStrControl(int iIndex)
{
	CN3UIString* pStr = nullptr;
	std::string str = "";
	char	cstr[4];
	sprintf(cstr, "%d", iIndex);	str += cstr;
	pStr = (CN3UIString* )GetChildByID(str);	 __ASSERT(pStr, "NULL UI Component!!");
	return pStr;
}

void CUIHotKeyDlg::DisplayTooltipStr(__IconItemSkill* spSkill)
{
	char pszDesc[256];

	const int iIndex = GetTooltipCurPageIndex(spSkill);
	if (iIndex != -1)
	{
		if (!m_pTooltipStr[iIndex]->IsVisible())	
			m_pTooltipStr[iIndex]->SetVisible(true);
		sprintf(pszDesc, "%s", spSkill->pSkill->szName.c_str());
		m_pTooltipStr[iIndex]->SetString(pszDesc);
		m_pTooltipStr[iIndex]->Render();
	}
}

void CUIHotKeyDlg::DisableTooltipDisplay()
{
	for(auto j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
	{
		if (m_pTooltipStr[j])	
			m_pTooltipStr[j]->SetVisible(false);
	}
}

void CUIHotKeyDlg::DisplayCountStr(__IconItemSkill* spSkill)
{
	char pszDesc[256];

	const int iIndex = GetCountCurPageIndex(spSkill);
	if (iIndex != -1)
	{
		if (!m_pCountStr[iIndex]->IsVisible())	
			m_pCountStr[iIndex]->SetVisible(true);
		sprintf(pszDesc, "%d", CGameProcedure::s_pProcMain->m_pUIInventory->GetCountInInvByID(spSkill->pSkill->dwExhaustItem));
		m_pCountStr[iIndex]->SetString(pszDesc);
		m_pCountStr[iIndex]->Render();
	}
}

void CUIHotKeyDlg::DisableCountStrDisplay()
{
	for(auto j = 0; j < MAX_SKILL_IN_HOTKEY; j++ )
	{
		if (m_pCountStr[j])	
			m_pCountStr[j]->SetVisible(false);
	}
}

int CUIHotKeyDlg::GetTooltipCurPageIndex(__IconItemSkill* pSkill)
{
	for(auto k = 0; k < MAX_SKILL_IN_HOTKEY; k++ )
	{
		if ( (m_pMyHotkey[m_iCurPage][k] != nullptr) && 
			(m_pMyHotkey[m_iCurPage][k] == pSkill) )
			return k;
	}

	return -1;
}

int CUIHotKeyDlg::GetCountCurPageIndex(__IconItemSkill* spSkill)
{
	if(spSkill->pSkill->dwExhaustItem <= 0)
		return -1;

	return GetTooltipCurPageIndex(spSkill);
}

bool CUIHotKeyDlg::ReceiveIconDrop(__IconItemSkill* spItem, POINT ptCur)
{
	bool bFound = false;
	// If it's not the icon I had...
	if ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd != UIWND_INVENTORY )
		return false;
	else
	{
		const CN3UIArea* pArea = nullptr;

		int iOrder;
		for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )
		{
			pArea = CN3UIWndBase::GetChildAreaByiOrder(UI_AREA_TYPE_SKILL_HOTKEY, i);
			if ( pArea && pArea->IsIn(ptCur.x, ptCur.y) )
			{
				bFound = true;
				iOrder = i;
				break;
			}
		}
		if ( !bFound )
			return false;

		__IconItemSkill* spSkill, *spItem;

		// If you have an existing icon...
		if ( m_pMyHotkey[m_iCurPage][iOrder] )
		{
			// Delete the existing icon.
			spSkill = m_pMyHotkey[m_iCurPage][iOrder];

			// Removed from manager..
			RemoveChild(spSkill->pUIIcon);

			// Remove resource...
			spSkill->pUIIcon->Release();
			delete spSkill->pUIIcon;
			spSkill->pUIIcon = nullptr;
			delete spSkill;
			spSkill = nullptr;
			m_pMyHotkey[m_iCurPage][iOrder] = nullptr;
		}

		spItem = CN3UIWndBase::m_sSelectedIconInfo.pItemSelect;

		__TABLE_UPC_SKILL* pUSkill = CGameBase::s_pTbl_Skill->Find(spItem->pItemBasic->dwEffectID1);
		if ( pUSkill == nullptr) return false;
		if ( pUSkill->dwID < UIITEM_TYPE_SONGPYUN_ID_MIN) return false;

		spSkill = new __IconItemSkill();
		spSkill->pSkill = pUSkill;

		// Creating an icon name.. ^^
		std::vector<char> buffer(256, NULL);
		sprintf(buffer.data(),	"UI\\skillicon_%.2d_%d.dxt", spItem->pItemBasic->dwEffectID1%100, spItem->pItemBasic->dwEffectID1/100);
		spSkill->szIconFN = buffer.data();

		// Load the icon.. ^^
		spSkill->pUIIcon = new CN3UIIcon;
		spSkill->pUIIcon->Init(this);
		spSkill->pUIIcon->SetTex(spSkill->szIconFN);
		spSkill->pUIIcon->SetUVRect(0,0,1.0f, 1.0f);
		spSkill->pUIIcon->SetUIType(UI_TYPE_ICON);
		spSkill->pUIIcon->SetStyle(UISTYLE_ICON_SKILL);

		DWORD bitMask = UISTYLE_ICON_SKILL;
		if (!CGameProcedure::s_pProcMain->m_pMagicSkillMng->CheckValidSkillMagic(spSkill->pSkill))
			bitMask |= UISTYLE_DISABLE_SKILL;
		spSkill->pUIIcon->SetStyle(bitMask);

		if ( pArea )
		{
			spSkill->pUIIcon->SetRegion(pArea->GetRegion());
			spSkill->pUIIcon->SetMoveRect(pArea->GetRegion());
		}

		m_pMyHotkey[m_iCurPage][iOrder] = spSkill;

		CloseIconRegistry();
	}

	return false;
}

bool CUIHotKeyDlg::EffectTriggerByMouse()
{
	if(m_iSelectIndex < 0 || m_iSelectIndex >= 8) return false;
	if(m_iSelectPage < 0 || m_iSelectPage >= 8) return false;

	if ( m_pMyHotkey[m_iSelectPage][m_iSelectIndex] )
	{
		const int iIDTarget = CGameBase::s_pPlayer->m_iIDTarget;
		return CGameProcedure::s_pProcMain->m_pMagicSkillMng->MsgSend_MagicProcess(iIDTarget, m_pMyHotkey[m_iSelectPage][m_iSelectIndex]->pSkill);
	}

	return false;
}

void CUIHotKeyDlg::RenderSelectIcon(CN3UIIcon* pUIIcon)
{
	if(!pUIIcon) return;

	const RECT rc = pUIIcon->GetRegion(); // check mark

	__VertexTransformedColor vLines[5];
	vLines[0].Set(rc.left, rc.top, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
	vLines[1].Set(rc.right, rc.top, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
	vLines[2].Set(rc.right, rc.bottom, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
	vLines[3].Set(rc.left, rc.bottom, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
	vLines[4] = vLines[0];

	DWORD dwZ, dwFog, dwAlpha, dwCOP, dwCA1, dwSrcBlend, dwDestBlend, dwVertexShader, dwAOP, dwAA1;
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &dwZ);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwCOP);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwCA1);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAOP);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAA1);
	CN3Base::s_lpD3DDev->GetFVF(&dwVertexShader); 

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

	CN3Base::s_lpD3DDev->SetFVF(FVF_TRANSFORMEDCOLOR);
	CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vLines, sizeof(__VertexTransformedColor));
	
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZ);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, dwCOP);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwCA1);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAOP);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAA1);
	CN3Base::s_lpD3DDev->SetFVF(dwVertexShader); 
}

// this_ui_add_start
bool CUIHotKeyDlg::OnKeyPress(int iKey)
{
	switch(iKey)
	{
	case DIK_ESCAPE:
		{	// When the hotkey is focused, other UI cannot be closed, so when DIK_ESCAPE comes in, focus again
			// Close other open children.
			CGameProcedure::s_pUIMgr->ReFocusUI();// this_ui
			CN3UIBase* pFocus = CGameProcedure::s_pUIMgr->GetFocusedUI();
			if(pFocus && pFocus != this) pFocus->OnKeyPress(iKey);
		}
		return true;
	}

	return CN3UIBase::OnKeyPress(iKey);
}
// this_ui_add_end
