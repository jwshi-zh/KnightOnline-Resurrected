#include "pch.h"
#include "LocalInput.h"
#include "N3UIWndBase.h"
#include "UIManager.h"
#include "GameProcedure.h"
#include "GameProcMain.h"
#include "UIInventory.h"
#include "UITransactionDlg.h"
#include "SubProcPerTrade.h"
#include "N3UIWndBase.h"
#include "CountableItemEditDlg.h"
#include "UIItemExchange.h"
#include "UIWareHouseDlg.h"

#include "N3UITooltip.h"

CUIManager::__RenderStateForUI CUIManager::s_sRSFU; // RenderStateForUI

CUIManager::CUIManager()
{
	m_dwMouseFlagsCur = 0;
	m_bEnableOperation = true;					// Is UI manipulation possible?
	m_pUIFocused = nullptr;

	m_bDoneSomething = false;					// I did the manipulation in the UI...
}

CUIManager::~CUIManager()
{
}

void CUIManager::Release()
{
	CN3UIBase::Release();
}

DWORD CUIManager::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	m_dwMouseFlagsCur = UI_MOUSEPROC_NONE;
	if (!m_bVisible || !m_bEnableOperation) return m_dwMouseFlagsCur;

	if (s_pTooltipCtrl)	s_pTooltipCtrl->MouseProc(dwFlags, ptCur, ptOld);	// Send mouse messages to tooltips.

	// Send message to child
	for(auto itor = m_Children.begin(); m_Children.end() != itor; )
	{
		CN3UIBase* pChild = (*itor);
		// When in commerce, only the icon manager window works.
		if ( CGameProcedure::s_pProcMain && CGameProcedure::s_pProcMain->m_pUITransactionDlg && 
			(CGameProcedure::s_pProcMain->m_pUITransactionDlg->IsVisible()))// && (pChild->UIType() != UI_TYPE_ICON_MANAGER) )
		{	
			if ( CN3UIWndBase::m_pCountableItemEdit->IsLocked() )
			{
				if ( pChild->m_szID.compare("base_tradeedit") != 0 )
					{	++itor; continue;	}
			}
		}
		// If it is stored in the library, only the icon manager window works.
		if ( CGameProcedure::s_pProcMain && CGameProcedure::s_pProcMain->m_pUIWareHouseDlg && 
			(CGameProcedure::s_pProcMain->m_pUIWareHouseDlg->IsVisible()))// && (pChild->UIType() != UI_TYPE_ICON_MANAGER) )
		{	
			if ( CN3UIWndBase::m_pCountableItemEdit->IsLocked() )
			{
				if ( pChild->m_szID.compare("base_tradeedit") != 0 )
					{	++itor; continue;	}
			}
		}
		// When trading between individuals, only the icon manager window works.. When inputting the amount of money or the number of arrows.. Only the input window works.
		if ( CGameProcedure::s_pProcMain && CGameProcedure::s_pProcMain->m_pSubProcPerTrade &&
			(CGameProcedure::s_pProcMain->m_pSubProcPerTrade->m_ePerTradeState != PER_TRADE_STATE_NONE) )
		{	
			if (CGameProcedure::s_pProcMain->m_pSubProcPerTrade->m_ePerTradeState == PER_TRARE_STATE_EDITTING) 
			{
				if ( pChild->m_szID.compare("base_tradeedit") != 0 )
					{	++itor; continue;	}
			}

			if ( CN3UIWndBase::m_pCountableItemEdit->IsLocked() )
			{
				if ( pChild->m_szID.compare("base_tradeedit") != 0 )
					{	++itor; continue;	}
			}
		}

		if(pChild->m_pChildUI && pChild->m_pChildUI->IsVisible())
		{
			const DWORD dwRet = pChild->m_pChildUI->MouseProc(dwFlags, ptCur, ptOld);
			if (UI_MOUSEPROC_DONESOMETHING & dwRet)
			{	// In this case, it is a case where the distance is focused.
				pChild->MouseProc(0, ptCur, ptOld);
				m_dwMouseFlagsCur |= (UI_MOUSEPROC_DONESOMETHING|UI_MOUSEPROC_CHILDDONESOMETHING);

				SetFocusedUI(pChild);// this_ui

				return m_dwMouseFlagsCur;
			}
			else if ( (	UI_MOUSE_LBCLICK & dwFlags) && (UI_MOUSEPROC_INREGION & dwRet) )
			{	// When you click inside the area, it says that the remote has worked and returns.
				pChild->MouseProc(0, ptCur, ptOld);
				m_dwMouseFlagsCur |= (UI_MOUSEPROC_DIALOGFOCUS);
				
				SetFocusedUI(pChild);// this_ui

				return m_dwMouseFlagsCur;
			}
		}

		const DWORD dwChildRet = pChild->MouseProc(dwFlags, ptCur, ptOld);
		if (UI_MOUSEPROC_DONESOMETHING & dwChildRet)
		{	// In this case, it is the case where the distance is focused.
			m_dwMouseFlagsCur |= (UI_MOUSEPROC_DONESOMETHING|UI_MOUSEPROC_CHILDDONESOMETHING);

			SetFocusedUI(pChild);// this_ui

			return m_dwMouseFlagsCur;
		}
		else if ( (	UI_MOUSE_LBCLICK & dwFlags) && (UI_MOUSEPROC_INREGION & dwChildRet) )
		{	// When you click inside the area, it says that the remote has worked and returns.
			m_dwMouseFlagsCur |= (UI_MOUSEPROC_DIALOGFOCUS);
			
			SetFocusedUI(pChild);// this_ui

			return m_dwMouseFlagsCur;
		}
		else ++itor;
		// else if (UI_MOUSE_LBCLICKED|UI_MOUSE_MBCLICK|UI_MOUSE_MBCLICKED|UI_MOUSE_RBCLICK|UI_MOUSE_RBCLICKED)

		m_dwMouseFlagsCur |= dwChildRet;
	}

	// if(UI_MOUSE_LBCLICK &amp; dwFlags) m_pUIFocused = NULL; // Record the focused UI.. If nothing is done.. null..

	return m_dwMouseFlagsCur;
}

void CUIManager::ReorderChildList()	// Rearrange dialog order
{
	const int iChildCount = m_Children.size();
	if (iChildCount<=0) return;
	auto** ppBuffer = new CN3UIBase*[iChildCount];
	int iAlwaysTopChildCount = 0;

	for(auto itor = m_Children.begin(); m_Children.end() != itor; )
	{
		CN3UIBase* pChild = (*itor);
		if (pChild->GetStyle() & UISTYLE_ALWAYSTOP)
		{
			itor = m_Children.erase(itor);			// First off the list
			ppBuffer[iAlwaysTopChildCount++] = pChild;
		}
		else ++itor;
	}
	int i;
	for (i=iAlwaysTopChildCount-1; i>=0; --i)
	{
		m_Children.push_front(ppBuffer[i]);	// put in front I want to make the order of drawing last and receive messages first.
	}
	delete [] ppBuffer;
}

void CUIManager::Tick()
{
	CN3UIBase::Tick();
	if (s_pTooltipCtrl)	s_pTooltipCtrl->Tick();
}

void CUIManager::Render()
{
	this->RenderStateSet();

	CN3UIBase::Render();	// children render
	if (s_pTooltipCtrl) s_pTooltipCtrl->Render();	// tooltip render

	this->RenderStateRestore();
}

void CUIManager::RenderStateSet()
{
	if(nullptr == s_lpD3DDev) return;

#ifdef _DEBUG
	__ASSERT(FALSE == s_sRSFU.bSet, "이전에 RenderStateSet()함수를 호출하고 RenderStateRestore()함수가 호출되지 않은 상태입니다.");
	s_sRSFU.bSet = TRUE;
#endif

	// back up old state
	s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &(s_sRSFU.dwZEnable));
	s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &(s_sRSFU.dwAlphaBlend));
	s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &(s_sRSFU.dwSrcBlend));
	s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &(s_sRSFU.dwDestBlend));
	s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &(s_sRSFU.dwFog));
	s_lpD3DDev->GetSamplerState(0, D3DSAMP_MAGFILTER,   &(s_sRSFU.dwMagFilter));
	s_lpD3DDev->GetSamplerState(0, D3DSAMP_MINFILTER,   &(s_sRSFU.dwMinFilter));
	s_lpD3DDev->GetSamplerState(0, D3DSAMP_MIPFILTER,   &(s_sRSFU.dwMipFilter));

	// set state
	if (D3DZB_FALSE != s_sRSFU.dwZEnable) s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	if (TRUE != s_sRSFU.dwAlphaBlend) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	if (D3DBLEND_SRCALPHA != s_sRSFU.dwSrcBlend) s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	if (D3DBLEND_INVSRCALPHA != s_sRSFU.dwDestBlend) s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	if (FALSE != s_sRSFU.dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE   , FALSE);	// 2d also eats fog ㅡ.ㅡ;
	if (D3DTEXF_POINT != s_sRSFU.dwMagFilter ) s_lpD3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER,   D3DTEXF_POINT);
	if (D3DTEXF_POINT != s_sRSFU.dwMinFilter ) s_lpD3DDev->SetSamplerState(0, D3DSAMP_MINFILTER,   D3DTEXF_POINT);
	if (D3DTEXF_NONE != s_sRSFU.dwMipFilter ) s_lpD3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER,   D3DTEXF_NONE);
}

void CUIManager::RenderStateRestore()
{
#ifdef _DEBUG
	__ASSERT(TRUE == s_sRSFU.bSet, "이전에 RenderStateSet()함수를 호출하지 않았습니다.");
	s_sRSFU.bSet = FALSE;
#endif

	// restore
	if (D3DZB_FALSE != s_sRSFU.dwZEnable) s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, s_sRSFU.dwZEnable);
	if (TRUE != s_sRSFU.dwAlphaBlend) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, s_sRSFU.dwAlphaBlend);
	if (D3DBLEND_SRCALPHA != s_sRSFU.dwSrcBlend) s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, s_sRSFU.dwSrcBlend);
	if (D3DBLEND_INVSRCALPHA != s_sRSFU.dwDestBlend) s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, s_sRSFU.dwDestBlend);
	if (FALSE != s_sRSFU.dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE   , s_sRSFU.dwFog);
	if (D3DTEXF_POINT != s_sRSFU.dwMagFilter ) s_lpD3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER,   s_sRSFU.dwMagFilter);
	if (D3DTEXF_POINT != s_sRSFU.dwMinFilter ) s_lpD3DDev->SetSamplerState(0, D3DSAMP_MINFILTER,   s_sRSFU.dwMinFilter);
	if (D3DTEXF_NONE != s_sRSFU.dwMipFilter ) s_lpD3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER,   s_sRSFU.dwMipFilter);
}

bool CUIManager::BroadcastIconDropMsg(__IconItemSkill* spItem)
{
	bool bFound = false;
	const POINT ptCur = CGameProcedure::s_pLocalInput->MouseGetPos();

	// Check the windows...
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		if ( bFound ) break;
		CN3UIBase* pChild = (*itor);
		if ( pChild->UIType() == UI_TYPE_ICON_MANAGER )
		{
			// If the window is visible (activated) and is within the window area...
			if ( ((CN3UIWndBase* )pChild)->IsVisible() && ((CN3UIWndBase* )pChild)->IsIn(ptCur.x, ptCur.y) )
				// Call the icon drop message function on the window.
				if ( ((CN3UIWndBase* )pChild)->ReceiveIconDrop(spItem, ptCur) )
					return true;
				else
					bFound = true;
		}
	}

	// If it does not belong to anyone&#39;s area, a Cancel message is sent to the window with the corresponding icon.
	if ( !bFound )
	{
		switch ( CN3UIWndBase::m_sSelectedIconInfo.UIWndSelect.UIWnd )
		{
			case UIWND_INVENTORY:
				CGameProcedure::s_pProcMain->m_pUIInventory->CancelIconDrop(spItem);
				break;

			case UIWND_TRANSACTION:
				CGameProcedure::s_pProcMain->m_pUITransactionDlg->CancelIconDrop(spItem);
				break;

			case UIWND_WARE_HOUSE:
				CGameProcedure::s_pProcMain->m_pUIWareHouseDlg->CancelIconDrop(spItem);
				break;

			case UIWND_EXCHANGE_REPAIR:
				CGameProcedure::s_pProcMain->m_pUIItemREDlg->CancelIconDrop(spItem);
				break;
		}
	}
	return false;
}

CN3UIBase* CUIManager::GetTopUI(bool bVisible)
{
	if(!bVisible) 
	{
		if(m_Children.empty()) return nullptr;
		else return *(m_Children.begin());
	}

	auto it = m_Children.begin(), itEnd = m_Children.end();
	for(; it != itEnd; it++)
	{
		CN3UIBase* pUI = *(it);
		if(pUI->IsVisible()) return pUI;
	}
	
	return nullptr;
}

void CUIManager::SetFocusedUI(CN3UIBase* pUI)
{
	if(nullptr == pUI)
	{
		m_pUIFocused = nullptr;
		return;
	}

	auto it = m_Children.begin(), itEnd = m_Children.end();
	it = m_Children.begin();
	for(; it != itEnd; it++)
	{
		if(pUI == *it) break;
	}
	if(it == itEnd) return;

	it = m_Children.erase(it);			// First off the list
	m_Children.push_front(pUI);	// put in front I want to make the order of drawing last and receive messages first.
	ReorderChildList();	// Reorder the child list (reorder because of the dialog that always floats on top)

	m_pUIFocused = this->GetTopUI(true);
}

CN3UIBase* CUIManager::ReFocusUI()
{
	SetVisibleFocusedUI(GetEnableFocusTopUI(true));
	return m_pUIFocused;
}

void CUIManager::SetVisibleFocusedUI(CN3UIBase *pUI)
{
	if(nullptr == pUI)
	{
		m_pUIFocused = nullptr;
		return;
	}

	if(!pUI->IsVisible())
		return;

	auto it = m_Children.begin(), itEnd = m_Children.end();

	DWORD dwUIStyle, dwUIHideStyle;
	CN3UIBase* pUIHide = nullptr;

	dwUIStyle = pUI->GetStyle();
	if(dwUIStyle & UISTYLE_SHOW_ME_ALONE)
	{
		for(; it != itEnd;)
		{
			pUIHide = *it;
			if(pUIHide == nullptr)
			{
				it = m_Children.erase(it);
				continue;
			}

			dwUIHideStyle = pUIHide->GetStyle();
			if(pUIHide->IsVisible() && pUI != pUIHide && !(dwUIHideStyle & UISTYLE_HIDE_UNABLE))
				pUIHide->SetVisibleWithNoSound(false, true);

			it++;
		}//
	}
	else if(!(dwUIStyle & UISTYLE_HIDE_UNABLE))
	{
		for(; it != itEnd;)
		{
			pUIHide = *it;
			if(pUIHide == nullptr)
			{
				it = m_Children.erase(it);
				continue;
			}

			dwUIHideStyle = pUIHide->GetStyle();
			if(pUIHide->IsVisible() && pUI != pUIHide && (dwUIHideStyle & UISTYLE_SHOW_ME_ALONE))
				pUIHide->SetVisibleWithNoSound(false, true);
/*
			if(pUIHide->IsVisible() && pUI != pUIHide)
			{
				if(dwUIHideStyle & UISTYLE_SHOW_ME_ALONE)
					pUIHide->SetVisibleWithNoSound(false, true);
				else if( (dwUIStyle & UISTYLE_POS_LEFT) && (dwUIHideStyle & UISTYLE_POS_LEFT) )
					pUIHide->SetVisibleWithNoSound(false, true);
				else if( (dwUIStyle & UISTYLE_POS_RIGHT) && (dwUIHideStyle & UISTYLE_POS_RIGHT) )
					pUIHide->SetVisibleWithNoSound(false, true);
			}
*/
			it++;
		}
	}

	it = m_Children.begin();
	itEnd = m_Children.end();

	for(; it != itEnd; it++)
	{
		if(pUI == *it) break;
	}
	if(it == itEnd) return;

	if(!(dwUIStyle & UISTYLE_FOCUS_UNABLE))
	{
		it = m_Children.erase(it);			// First off the list
		m_Children.push_front(pUI);	// put in front I want to make the order of drawing last and receive messages first.
		ReorderChildList();	// Reorder the child list (reorder because of the dialog that always floats on top)
	}

	m_pUIFocused = this->GetEnableFocusTopUI(true);
}

CN3UIBase* CUIManager::GetEnableFocusTopUI(bool bVisible)
{
	if(!bVisible) 
	{
		if(m_Children.empty()) return nullptr;
		else return *(m_Children.begin());
	}

	auto it = m_Children.begin(), itEnd = m_Children.end();
	for(; it != itEnd; it++)
	{
		CN3UIBase* pUI = *(it);
		if(pUI && pUI->IsVisible() &&	// visible
			!(pUI->GetStyle() & UISTYLE_FOCUS_UNABLE) && // can focus
			!(pUI->GetStyle() & UISTYLE_HIDE_UNABLE)) // can be closed
			return pUI;
	}
	
	return nullptr;
}

void CUIManager::UserMoveHideUIs()
{
	bool bHide = false;
	auto it = m_Children.begin(), itEnd = m_Children.end();
	for(; it != itEnd; it++)
	{
		CN3UIBase* pUI = *(it);

		if(pUI == nullptr) continue;

		if(pUI->IsVisible() && (pUI->GetStyle() & UISTYLE_USER_MOVE_HIDE))
		{
			bHide = true;
			pUI->SetVisibleWithNoSound(false, true);
		}
	}

	if(bHide) ReFocusUI();
}
