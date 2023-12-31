#include "pch.h"
#include "UIMessageBoxManager.h"
#include "GameProcedure.h"
#include "UIManager.h"

CUIMessageBoxManager::CUIMessageBoxManager()
{
	m_pMsgBoxLatestRef = nullptr;
}

CUIMessageBoxManager::~CUIMessageBoxManager()
{
	Release();
}

int CUIMessageBoxManager::GetCount() const
{
	return m_UBMs.size();
}

std::string CUIMessageBoxManager::MessageBoxPost(const std::string& szMsg, const std::string& szTitle, int iStyle, e_Behavior eBehavior)
{
	CUIMessageBox*		pMB		= nullptr;
	const __TABLE_UI_RESRC*	pTblUI	= nullptr;

	auto it		= m_UBMs.find(szMsg);
	auto it_e	= m_UBMs.end();

	if( it == it_e )
	{
		pMB = new CUIMessageBox();
		if( pMB == nullptr)
			return szMsg;

		pTblUI = CGameBase::s_pTbl_UI->Find(NATION_ELMORAD);

		pMB->LoadFromFile(pTblUI->szMessageBox );
		pMB->SetVisible(false);
		m_UBMs.insert(std::make_pair(szMsg, pMB));
	}
	else
	{
		pMB = it->second;
	}

	const RECT rt = pMB->GetRegion();
	POINT pt;
	pt.x = (CN3Base::s_CameraData.vp.Width - (rt.right - rt.left)) / 2;
	pt.y = (CN3Base::s_CameraData.vp.Height - (rt.bottom - rt.top)) / 2;
	
	pMB->SetBoxStyle(iStyle);
	if(BEHAVIOR_DELETE_CHR == eBehavior) pMB->SetVisibleEditControl(true); // Edit Control Visible
	else pMB->SetVisibleEditControl(false);


	pMB->m_eBehavior = eBehavior;
	pMB->SetText(szMsg);
	pMB->SetTitle(szTitle);
	
	pMB->SetPos(pt.x, pt.y);
	pMB->SetVisible(true);

	m_pMsgBoxLatestRef = pMB; // The dialog box that popped up last... Set this pointer for Z alignment.

	it		= m_UBMs.begin();
	it_e	= m_UBMs.end();
	while(it!=it_e)
	{
		pMB = it->second;
		if( pMB && !pMB->IsVisible() )
		{
			T_Delete(pMB);
			it = m_UBMs.erase(it);
			continue;
		}
		it++;
	}

	return szMsg;
}

void CUIMessageBoxManager::MessageBoxClose(const std::string& szMsg)
{
	const auto it_f = m_UBMs.find(szMsg);
	const auto it_e = m_UBMs.end();

	if(it_f != it_e)
	{
		if(m_pMsgBoxLatestRef == it_f->second)
			m_pMsgBoxLatestRef = nullptr;

		CUIMessageBox* pMB = it_f->second;
		if(pMB) pMB->SetVisible(false);
	}
}

void CUIMessageBoxManager::Render()
{
	auto it = m_UBMs.begin(), it_e = m_UBMs.end();
	for(; it != it_e; it++)
	{
		CUIMessageBox* pMB = it->second;
		if(pMB == nullptr || pMB == m_pMsgBoxLatestRef) continue;
		if(!pMB->IsVisible()) continue;
		CUIManager::RenderStateSet();
		pMB->Render(); // Render message box...
		CUIManager::RenderStateRestore();
	}

	if(m_pMsgBoxLatestRef && m_pMsgBoxLatestRef->IsVisible())
	{
		CUIManager::RenderStateSet();
		m_pMsgBoxLatestRef->Render(); // Render message box...
		CUIManager::RenderStateRestore();
	}
}

void CUIMessageBoxManager::MessageBoxCloseAll()
{
	auto it = m_UBMs.begin(), it_e = m_UBMs.end();
	for(; it != it_e; it++)
	{
		CUIMessageBox* pMB = it->second;
		if(pMB) pMB->SetVisible(false);
	}

	m_pMsgBoxLatestRef = nullptr;
}

DWORD CUIMessageBoxManager::MouseProcAndTick(DWORD &dwFlags, const POINT &ptCur, const POINT &ptOld)
{
	DWORD dwRet = 0;
	CUIMessageBox *pMB;

	if(m_pMsgBoxLatestRef && m_pMsgBoxLatestRef->IsVisible())
	{
		dwRet = m_pMsgBoxLatestRef->MouseProc(dwFlags, ptCur, ptOld);
		if(dwRet != UI_MOUSEPROC_NONE) dwFlags = 0;
		if(m_pMsgBoxLatestRef) m_pMsgBoxLatestRef->Tick();
	}

	auto it = m_UBMs.begin(), it_e = m_UBMs.end();
	for(; it != it_e; it++)
	{
		pMB = it->second;
		if(false == pMB->IsVisible())	continue;
		if(m_pMsgBoxLatestRef == pMB)	continue;

		dwRet = pMB->MouseProc(dwFlags, ptCur, ptOld);
		if(dwRet != UI_MOUSEPROC_NONE) dwFlags = 0;
		pMB->Tick();
	}

	return dwRet;
}

void CUIMessageBoxManager::Release()
{
	auto it = m_UBMs.begin(), it_e = m_UBMs.end();
	for(; it != it_e; it++)
	{
		T_Delete(it->second);
	}
	m_UBMs.clear();

	m_pMsgBoxLatestRef = nullptr;
}

CUIMessageBox* CUIMessageBoxManager::ReFocusMsgBox()
{
	for(auto itor = m_UBMs.rbegin(); m_UBMs.rend() != itor; ++itor)
	{
		CUIMessageBox* pMsgBox = itor->second;
		if(pMsgBox && pMsgBox->IsVisible())
		{
			m_pMsgBoxLatestRef = pMsgBox;
			return m_pMsgBoxLatestRef;
		}
	}

	m_pMsgBoxLatestRef = nullptr;
	return nullptr;
}
