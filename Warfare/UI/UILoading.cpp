#include "pch.h"
#include "UILoading.h"
#include "GameDef.h"
#include "GameEng.h"
#include "GameProcedure.h"
#include "UIManager.h"

#include "N3UIProgress.h"
#include "N3UIString.h"

CUILoading::CUILoading()
{
	m_pText_Info = nullptr;
	m_pText_Version = nullptr;
	m_pProgress_Loading = nullptr;
}

CUILoading::~CUILoading()
{
	CUILoading::Release();
}

void CUILoading::Release()
{
	CN3UIBase::Release();

	m_pText_Info = nullptr;
	m_pText_Version = nullptr;
	m_pProgress_Loading = nullptr;
}

bool CUILoading::Load(HANDLE hFile)
{
	if(CN3UIBase::Load(hFile)==false) return false;

	m_pText_Version = (CN3UIString*)(CN3UIBase::GetChildByID("Text_Version")); __ASSERT(m_pText_Version, "NULL UI Component!!");
	if(m_pText_Version)
	{
		char szVersion[128];
		sprintf(szVersion, "Ver. %.3f", CURRENT_VERSION/1000.0f);
		m_pText_Version->SetString(szVersion);
	}
	m_pText_Info = (CN3UIString*)(CN3UIBase::GetChildByID("Text_Info")); __ASSERT(m_pText_Info, "NULL UI Component!!");
	m_pProgress_Loading = (CN3UIProgress*)(CN3UIBase::GetChildByID("Progress_Loading")); __ASSERT(m_pProgress_Loading, "NULL UI Component!!");

	this->SetPosCenter(); // fit in the middle...
	m_pText_Version->SetPos(10, 10); // Version is displayed at the top.
	
	if(m_pProgress_Loading) m_pProgress_Loading->SetRange(0, 100);

	return true;
}

void CUILoading::Render(const std::string& szInfo, int iPercentage)
{
	if(m_pText_Info) m_pText_Info->SetString(szInfo);
	if(m_pProgress_Loading) m_pProgress_Loading->SetCurValue(iPercentage);

	const D3DCOLOR crEnv = 0x00000000;
	CGameProcedure::s_pEng->Clear(crEnv); // the background is black
	CN3Base::s_lpD3DDev->BeginScene();			// Scene renders...
	
	CN3UIBase::Tick();
	CUIManager::RenderStateSet();
	CN3UIBase::Render();
	CUIManager::RenderStateRestore();
	
	CN3Base::s_lpD3DDev->EndScene();			// Start scene render...
	CGameProcedure::s_pEng->Present(CN3Base::s_hWndBase);
}
