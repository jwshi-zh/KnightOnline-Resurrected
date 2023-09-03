#pragma once

#include "pch.h"
#include "N3UIIcon.h"
#include "N3UIWndBase.h"
#include "GameProcedure.h"
#include "UIImageTooltipDlg.h"

#ifdef _N3GAME
#include "N3UIWndBase.h"
#endif

CN3UIIcon::CN3UIIcon()
{
	CN3UIImage::CN3UIImage();
	m_eType = UI_TYPE_ICON;
}

CN3UIIcon::~CN3UIIcon()
{
}

DWORD CN3UIIcon::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;

	if (!m_bVisible) return dwRet;
	if ( (m_pParent->GetState() == UI_STATE_COMMON_NONE) || (m_pParent->GetState() == UI_STATE_ICON_MOVING) ) 
		SetStyle(GetStyle() & (~UISTYLE_ICON_HIGHLIGHT));	

#ifdef _N3GAME
	if ( CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer ) return dwRet;
#endif

	RECT rect = GetRegion();
	if ( ::PtInRect(&rect, ptCur) && (m_pParent->GetState() == UI_STATE_COMMON_NONE) )
		SetStyle(GetStyle() | UISTYLE_ICON_HIGHLIGHT);	

	rect = GetMoveRect();

	if(!::PtInRect(&rect, ptCur))		// out of range
	{
		dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
		return dwRet;
	}

	if((dwFlags & UI_MOUSE_LBCLICK) && !(dwFlags & UI_MOUSE_RBDOWN))  // When the left button is pressed
	{
		m_pParent->SetState(UI_STATE_ICON_MOVING);
		m_pParent->ReceiveMessage(this, UIMSG_ICON_DOWN_FIRST); // Notify parent on button click..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if((dwFlags & UI_MOUSE_LBCLICKED) && !(dwFlags & UI_MOUSE_RBDOWN)) // When the left button is released
	{
		if(m_pParent && m_pParent->GetState() == UI_STATE_ICON_MOVING) // If the previous state is the Down state of the button
		{
			m_pParent->SetState(UI_STATE_COMMON_NONE);
			m_pParent->ReceiveMessage(this, UIMSG_ICON_UP); // Notify parent on button click..
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
	}

	if ((dwFlags & UI_MOUSE_RBCLICK)  && !(dwFlags & UI_MOUSE_LBDOWN))	// right
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_RDOWN_FIRST); // Notify parent on button click..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if((dwFlags & UI_MOUSE_RBCLICKED)  && !(dwFlags & UI_MOUSE_LBDOWN))// when the right button is released
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_RUP); // Notify parent on button click..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if (dwFlags & UI_MOUSE_LBDOWN)	// left
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_DOWN); // Notify parent on button click..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}
/*
	else
	{
		if(m_pParent && m_pParent->GetState() == UI_STATE_ICON_MOVING) // If the previous state is the button Down state
		{
			m_pParent->ReceiveMessage(this, UIMSG_ICON_UP); // Notify parent when button is clicked..
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
	}
*/

	if (dwFlags & UI_MOUSE_LBDBLCLK)	// left double click
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_DBLCLK); // Notify parent on button click..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if (dwFlags & UI_MOUSE_RBDBLCLK)	// right double click
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_RDBLCLK); // Notify parent on button click..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

void CN3UIIcon::Render()
{
	if (!IsVisible()) return;

	CN3UIWndBase::m_pSelectionImage->SetVisible(false);

/*	if ( m_dwStyle & UISTYLE_ICON_HIGHLIGHT )
	{		
		CN3UIWndBase::m_pSelectionImage->SetVisible(true);
		CN3UIWndBase::m_pSelectionImage->SetRegion(GetRegion());
		CN3UIWndBase::m_pSelectionImage->Render();
		CN3UIWndBase::m_pSelectionImage->SetVisible(false);
	}*/

	CN3UIImage::Render();

	if (m_dwStyle & UISTYLE_DISABLE_SKILL)
	{
		CN3UIWndBase::m_pSelectionImage->SetVisible(true);
		CN3UIWndBase::m_pSelectionImage->SetRegion(GetRegion());
		m_dc = CN3UIWndBase::m_pSelectionImage->GetColor();
		CN3UIWndBase::m_pSelectionImage->SetColor(D3DCOLOR_RGBA(40,40, 40, 160));
		CN3UIWndBase::m_pSelectionImage->RenderIconWrapper();
		CN3UIWndBase::m_pSelectionImage->SetColor(m_dc);
		CN3UIWndBase::m_pSelectionImage->SetVisible(false);
	}

	if( m_dwStyle & UISTYLE_DURABILITY_EXHAUST )
	{
		CN3UIWndBase::m_pSelectionImage->SetVisible(true);
		CN3UIWndBase::m_pSelectionImage->SetRegion(GetRegion());
		m_dc = CN3UIWndBase::m_pSelectionImage->GetColor();
		CN3UIWndBase::m_pSelectionImage->SetColor(D3DCOLOR_RGBA(200, 20, 20, 100));
		CN3UIWndBase::m_pSelectionImage->RenderIconWrapper();
		CN3UIWndBase::m_pSelectionImage->SetColor(m_dc);
		CN3UIWndBase::m_pSelectionImage->SetVisible(false);
	}
}





