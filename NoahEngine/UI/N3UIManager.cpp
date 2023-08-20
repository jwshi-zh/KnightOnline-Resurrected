#include "pch.h"
#include "N3UIManager.h"
#include "N3UITooltip.h"

CN3UIManager::__RenderStateForUI CN3UIManager::s_sRSFU; // RenderStateForUI

CN3UIManager::CN3UIManager()
{

}

CN3UIManager::~CN3UIManager()
{
}

void CN3UIManager::Release()
{
	CN3UIBase::Release();
}

DWORD CN3UIManager::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	m_dwMouseFlagsCur = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return m_dwMouseFlagsCur;

	if (s_pTooltipCtrl)	s_pTooltipCtrl->MouseProc(dwFlags, ptCur, ptOld);	// Send mouse messages to tooltips.

	// Send message to child
	for(auto itor = m_Children.begin(); m_Children.end() != itor; )
	{
		CN3UIBase* pChild = (*itor);
		const DWORD dwChildRet = pChild->MouseProc(dwFlags, ptCur, ptOld);
		if (UI_MOUSEPROC_DONESOMETHING & dwChildRet)
		{	// In this case, it is the case where the distance is focused.
			itor = m_Children.erase(itor);			// First off the list
			m_Children.push_front(pChild);	// put in front I want to make the order of drawing last and receive messages first.
			m_dwMouseFlagsCur |= (UI_MOUSEPROC_DONESOMETHING|UI_MOUSEPROC_CHILDDONESOMETHING);
			ReorderChildList();	// Reorder the child list (reorder because of the dialog that always floats on top)
			return m_dwMouseFlagsCur;
		}
		else if ( (	UI_MOUSE_LBCLICK & dwFlags) && (UI_MOUSEPROC_INREGION & dwChildRet) )
		{	// When I click inside the area, it says that the remote has done the work and returns.
			itor = m_Children.erase(itor);			// First off the list
			m_Children.push_front(pChild);	// put in front I want to make the order of drawing last and receive messages first.
			m_dwMouseFlagsCur |= (UI_MOUSEPROC_DIALOGFOCUS);
			ReorderChildList();	// Reorder the child list (reorder because of the dialog that always floats on top)
			return m_dwMouseFlagsCur;
		}
		else ++itor;
		// else if (UI_MOUSE_LBCLICKED|UI_MOUSE_MBCLICK|UI_MOUSE_MBCLICKED|UI_MOUSE_RBCLICK|UI_MOUSE_RBCLICKED)

		m_dwMouseFlagsCur |= dwChildRet;
	}

	return m_dwMouseFlagsCur;
}

void CN3UIManager::ReorderChildList()	// Rearrange dialog order
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

void CN3UIManager::Tick()
{
	CN3UIBase::Tick();
	if (s_pTooltipCtrl)	s_pTooltipCtrl->Tick();
}

void CN3UIManager::Render()
{
	this->RenderStateSet();

	CN3UIBase::Render();	// children render
	if (s_pTooltipCtrl) s_pTooltipCtrl->Render();	// tooltip render

	this->RenderStateRestore();
}

void CN3UIManager::RenderStateSet()
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

void CN3UIManager::RenderStateRestore()
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
