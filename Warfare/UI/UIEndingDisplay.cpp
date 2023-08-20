#include "pch.h"
#include "UIEndingDisplay.h"
#include "UIManager.h"
#include "GameEng.h"

CUIEndingDisplay::CUIEndingDisplay()
{
}

CUIEndingDisplay::~CUIEndingDisplay()
{
	Release();
}

void CUIEndingDisplay::Release()
{
	CN3UIBase::Release();
}

void CUIEndingDisplay::Render()
{
	// Align to the center of the screen.
	const RECT rc = this->GetRegion();
	const int iX = (s_CameraData.vp.Width - (rc.right - rc.left))/2;
	const int iY = (s_CameraData.vp.Height - (rc.bottom - rc.top))/2;
	this->SetPos(iX, iY);

	const D3DCOLOR crEnv = 0x00000000;
	CGameEng::Clear(crEnv); // the background is black
	CGameEng::s_lpD3DDev->BeginScene();			// Scene renders...

	CUIManager::RenderStateSet();
	CN3UIBase::Render();
	CUIManager::RenderStateRestore();
	
	CGameEng::s_lpD3DDev->EndScene();			// Start scene render...
	CGameEng::Present(CN3Base::s_hWndBase);
}