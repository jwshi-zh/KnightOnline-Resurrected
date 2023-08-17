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
	// ȭ�� ����� ���߱�..
	RECT rc = this->GetRegion();
	int iX = (s_CameraData.vp.Width - (rc.right - rc.left))/2;
	int iY = (s_CameraData.vp.Height - (rc.bottom - rc.top))/2;
	this->SetPos(iX, iY);

	D3DCOLOR crEnv = 0x00000000;
	CGameEng::Clear(crEnv); // ����� ������
	CGameEng::s_lpD3DDev->BeginScene();			// �� ���� ����...

	CUIManager::RenderStateSet();
	CN3UIBase::Render();
	CUIManager::RenderStateRestore();
	
	CGameEng::s_lpD3DDev->EndScene();			// �� ���� ����...
	CGameEng::Present(CN3Base::s_hWndBase);
}