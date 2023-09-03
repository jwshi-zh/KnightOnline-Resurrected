#include "pch.h"
#include "UIPartyOrForce.h"
#include "GameProcMain.h"
#include "PlayerOtherMgr.h"

#include "N3UIProgress.h"
#include "N3UIStatic.h"
#include "N3UIImage.h"
#include "N3UIArea.h"
#include "UIManager.h"

CUIPartyOrForce::CUIPartyOrForce()
{
	for(int i = 0; i < MAX_PARTY_OR_FORCE; i++)
	{
		m_pProgress_HPs[i]		= nullptr;	// As much as the number of members... HP Gauge
		m_pProgress_HPReduce[i] = nullptr;	// As much as the number of members... HP Reduce
		m_pProgress_ETC[i]		= nullptr;	// As much as the number of members... Abnormal status
		m_pStatic_IDs[i]		= nullptr;	// As many as the number of members... Names...
		m_pAreas[i]				= nullptr;
	}

	m_iIndexSelected = -1; // Currently selected member index..
}

CUIPartyOrForce::~CUIPartyOrForce()
{
}

void CUIPartyOrForce::Release()
{
	CN3UIBase::Release();

	m_Members.clear();
	m_iIndexSelected = -1; // Currently selected member index..
	for(int i = 0; i < MAX_PARTY_OR_FORCE; i++)
	{
		m_pProgress_HPs[i]		= nullptr;	// As much as the number of members... HP Gauge
		m_pProgress_HPReduce[i] = nullptr;	// As much as the number of members... HP Reduce
		m_pProgress_ETC[i]		= nullptr;	// As much as the number of members... Abnormal status
		m_pStatic_IDs[i]		= nullptr;	// As many as the number of members... Names...
		m_pAreas[i]				= nullptr;
	}

}

bool CUIPartyOrForce::Load(HANDLE hFile)
{
	if(CN3UIBase::Load(hFile)==false) return false;

	char szID[128] = "";
	for(int i = 0; i < MAX_PARTY_OR_FORCE; i++) // Let's find an empty place.
	{
		sprintf(szID, "progress_hp_%d", i); // 
		m_pProgress_HPs[i] = (CN3UIProgress*)(this->GetChildByID(szID)); __ASSERT(m_pProgress_HPs[i], "NULL UI Component!!!");
		if(m_pProgress_HPs[i])
		{
			m_pProgress_HPs[i]->SetVisible(false);
			m_pProgress_HPs[i]->SetRange(0, 100);
		}

		sprintf(szID, "progress_hp_%d_poison", i); // 
		m_pProgress_HPReduce[i] = (CN3UIProgress*)(this->GetChildByID(szID)); __ASSERT(m_pProgress_HPReduce[i], "NULL UI Component!!!");
		if(m_pProgress_HPReduce[i])
		{
			m_pProgress_HPReduce[i]->SetVisible(false);
			m_pProgress_HPReduce[i]->SetRange(0, 100);
		}

		sprintf(szID, "progress_hp_%d_curse", i); // 
		m_pProgress_ETC[i] = (CN3UIProgress*)(this->GetChildByID(szID)); __ASSERT(m_pProgress_ETC[i], "NULL UI Component!!!");
		if(m_pProgress_ETC[i])
		{
			m_pProgress_ETC[i]->SetVisible(false);
			m_pProgress_ETC[i]->SetRange(0, 100);
		}
		
		sprintf(szID, "static_name_%d", i);
		m_pStatic_IDs[i] = (CN3UIStatic*)(this->GetChildByID(szID)); __ASSERT(m_pStatic_IDs[i], "NULL UI Component!!!");
		if(m_pStatic_IDs[i]) 
		{
			m_pStatic_IDs[i]->SetVisible(false);
		}

		sprintf(szID, "Area_%d", i);
		m_pAreas[i] = (CN3UIArea*)(this->GetChildByID(szID)); __ASSERT(m_pAreas[i], "NULL UI Component!!!");
	}

	this->MemberInfoReInit();

	return true;
}

bool CUIPartyOrForce::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if( dwMsg == UIMSG_BUTTON_CLICK )
	{
		const __InfoPartyOrForce* pIP = nullptr;
		auto it = m_Members.begin(), itEnd = m_Members.end();
		for(int i = 0; it != itEnd && i < MAX_PARTY_OR_FORCE; it++, i++)
		{
			// if(m_pStatic_IDs[i] && pSender == m_pStatic_IDs[i])
			if(pSender == m_pAreas[i])
			{
				pIP = &(*it);
				m_iIndexSelected = i; // Currently selected member index..
				break;
			}
		}

		if(pIP) CGameProcedure::s_pProcMain->TargetSelect(pIP->iID, true);
		else CGameProcedure::s_pProcMain->TargetSelect(-1, false);
	}

	return true;
}

void CUIPartyOrForce::Render()
{
	if(false == m_bVisible) return;

	CN3UIBase::Render();

	if(	m_iIndexSelected < 0 ||
		m_iIndexSelected >= m_Members.size() ||
		m_iIndexSelected >= MAX_PARTY_OR_FORCE ) return;
	if(nullptr == m_pStatic_IDs[m_iIndexSelected] || nullptr == m_pProgress_HPs[m_iIndexSelected]) return;

	RECT rc1 = m_pStatic_IDs[m_iIndexSelected]->GetRegion();
	rc1.left -= 2; rc1.top -= 2; rc1.right += 2; rc1.bottom += 2;

	RECT rc2 = m_pProgress_HPs[m_iIndexSelected]->GetRegion();
	rc2.left -= 2; rc2.top -= 2; rc2.right += 2; rc2.bottom += 2;

	RECT rc;
	rc.left = (rc1.left < rc2.left) ? rc1.left : rc2.left;
	rc.top = (rc1.top < rc2.top) ? rc1.top : rc2.top;
	rc.right = (rc1.right > rc2.right) ? rc1.right : rc2.right;
	rc.bottom = (rc1.bottom > rc2.bottom) ? rc1.bottom : rc2.bottom;

	CN3Base::RenderLines(rc, 0xff00ff00); // Show selection...
}

bool CUIPartyOrForce::TargetByIndex(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_Members.size()) return false;

	auto it = m_Members.begin();
	for(int i = 0; i < iIndex; i++) it++;

	const __InfoPartyOrForce* pIP = &(*it);
	m_iIndexSelected = iIndex; // Currently selected member index..

	if(pIP) CGameProcedure::s_pProcMain->TargetSelect(pIP->iID, true);

	return true;
}

const __InfoPartyOrForce* CUIPartyOrForce::MemberInfoGetByID(int iID, int& iIndexResult)
{
	if(m_Members.empty()) return nullptr;

	auto it = m_Members.begin(), itEnd = m_Members.end();
	iIndexResult = 0;
	for(; it != itEnd; it++, iIndexResult++)
	{
		if(iID == it->iID)
		{
			return &(*it);
		}
	}

	iIndexResult = -1;
	return nullptr;
}

const __InfoPartyOrForce* CUIPartyOrForce::MemberInfoGetByIndex(int iIndex)
{
	if(m_Members.empty()) return nullptr;
	if(iIndex < 0 || iIndex > m_Members.size()) return nullptr;

	auto it = m_Members.begin(), itEnd = m_Members.end();
	for(int i = 0; i < iIndex; i++) it++;

	return &(*it);
}

CPlayerOther* CUIPartyOrForce::MemberGetByNearst(const __Vector3& vPosPlayer)
{
	if(m_Members.empty()) return nullptr;

	float fDistMin = FLT_MAX;
	CPlayerOther* pTarget = nullptr;

	auto it = m_Members.begin(), itEnd = m_Members.end();
	for(; it != itEnd; it++)
	{
		CPlayerOther* pUPC = CGameBase::s_pOPMgr->UPCGetByID(it->iID, false);
		if(nullptr == pUPC) continue;

		const float fDistTmp = pUPC->Distance(vPosPlayer);
		if(fDistTmp < fDistMin)
		{
			pTarget = pUPC;
			fDistMin = fDistTmp;
		}
	}

	return pTarget;
}

const __InfoPartyOrForce* CUIPartyOrForce::MemberAdd(int iID, const std::string szID, int iLevel, e_Class eClass, int iHP, int iHPMax)
{
	__InfoPartyOrForce InfoTmp;
	InfoTmp.iID = iID;
	InfoTmp.szID = szID;
	InfoTmp.iLevel = iLevel;
	InfoTmp.iHP = iHP;
	InfoTmp.iHPMax = iHPMax;
	InfoTmp.eClass = eClass;

	m_Members.push_back(InfoTmp);

	auto it = m_Members.end();
	it--;
	
	this->MemberInfoReInit();

	return &(*it);
}

bool CUIPartyOrForce::MemberRemove(int iID)
{
	if(m_Members.empty()) return false;

	auto it = m_Members.begin(), itEnd = m_Members.end();
	for(; it != itEnd; it++)
	{
		if(iID == it->iID)
		{
			m_Members.erase(it);
			this->MemberInfoReInit();
			return true;
		}
	}

	return false;
}

void CUIPartyOrForce::MemberDestroy()
{
	m_Members.clear();
	for(int i = 0; i < MAX_PARTY_OR_FORCE; i++) // Let's find an empty place.
	{
		if(m_pProgress_HPs[i])		m_pProgress_HPs[i]->SetVisible(false);
		if(m_pProgress_HPReduce[i]) m_pProgress_HPReduce[i]->SetVisible(false);
		if(m_pProgress_ETC[i])		m_pProgress_ETC[i]->SetVisible(false);

		if(m_pStatic_IDs[i]) m_pStatic_IDs[i]->SetVisible(false);
	}

	this->MemberInfoReInit();
}

void CUIPartyOrForce::MemberInfoReInit() // When the composition of party members is changed.. Updating the order and various information..
{
	auto it = m_Members.begin(), itEnd = m_Members.end();
	int i;

	for(i = 0; it != itEnd && i < MAX_PARTY_OR_FORCE; it++, i++)
	{
		const __InfoPartyOrForce* pIP = &(*it); // I did this to make debugging easier.
		if(pIP->iHPMax <= 0)
		{
			__ASSERT(0, "Invalid Party memeber HP");
			continue;
		}

		if(m_pProgress_HPs[i])
		{
			m_pProgress_HPs[i]->SetCurValue(pIP->iHP * 100 / pIP->iHPMax);
			// m_pProgress_HPs[i]->SetVisible(true);
		}
		if(m_pProgress_HPReduce[i])
		{
			m_pProgress_HPReduce[i]->SetCurValue(pIP->iHP * 100 / pIP->iHPMax);
			// m_pProgress_HPReduce[i]->SetVisible(false);
		}
		if(m_pProgress_ETC[i])
		{
			m_pProgress_ETC[i]->SetCurValue(pIP->iHP * 100 / pIP->iHPMax);
			// m_pProgress_ETC[i]->SetVisible(false);
		}
		if(m_pStatic_IDs[i])
		{
			m_pStatic_IDs[i]->SetString(pIP->szID);
			m_pStatic_IDs[i]->SetVisible(true);
		}
	}

	for(; i < MAX_PARTY_OR_FORCE; i++)
	{
		if(m_pProgress_HPs[i])		m_pProgress_HPs[i]->SetVisible(false);
		if(m_pProgress_HPReduce[i]) m_pProgress_HPReduce[i]->SetVisible(false);
		if(m_pProgress_ETC[i])		m_pProgress_ETC[i]->SetVisible(false);
		if(m_pStatic_IDs[i])		m_pStatic_IDs[i]->SetVisible(false);
	}

	if(m_Members.empty()) this->SetVisible(false); // Hide if there are no members.
	else this->SetVisible(true); // If there is a member, it is visible.
}

const __InfoPartyOrForce* CUIPartyOrForce::MemberInfoGetSelected()
{
	if(m_Members.empty()) return nullptr;
	if(m_iIndexSelected < 0 || m_iIndexSelected > m_Members.size()) return nullptr;

	auto it = m_Members.begin(), itEnd = m_Members.end();
	for(int i = 0; i < m_iIndexSelected; i++) it++;

	return &(*it);
}

void CUIPartyOrForce::MemberHPChange(int iID, int iHP, int iHPMax)
{
	auto it = m_Members.begin(), itEnd = m_Members.end();
	for(int i = 0; it != itEnd && i < MAX_PARTY_OR_FORCE; it++, i++)
	{
		__InfoPartyOrForce* pIP = &(*it); // I did this to make debugging easier.
		if(pIP->iID == iID)
		{
			pIP->iHP = iHP;
			pIP->iHPMax = iHPMax;

			if(m_pProgress_HPs[i])		m_pProgress_HPs[i]->SetCurValue(pIP->iHP * 100 / pIP->iHPMax, 0.7f, 50.0f);
			if(m_pProgress_HPReduce[i]) m_pProgress_HPReduce[i]->SetCurValue(pIP->iHP * 100 / pIP->iHPMax, 0.7f, 50.0f);
			if(m_pProgress_ETC[i])		m_pProgress_ETC[i]->SetCurValue(pIP->iHP * 100 / pIP->iHPMax, 0.7f, 50.0f);
			break;
		}
	}
}

void CUIPartyOrForce::MemberStatusChange(int iID, e_PartyStatus ePS, bool bSuffer)
{
	auto it = m_Members.begin(), itEnd = m_Members.end();
	for(int i = 0; it != itEnd && i < MAX_PARTY_OR_FORCE; it++, i++)
	{
		__InfoPartyOrForce* pIP = &(*it); // I did this to make debugging easier.
		if(pIP->iID == iID)
		{
			if(PARTY_STATUS_DOWN_HP == ePS)	pIP->bSufferDown_HP = bSuffer;
			else if(PARTY_STATUS_DOWN_ETC == ePS) pIP->bSufferDown_Etc = bSuffer;
			break;
		}
	}
}

void CUIPartyOrForce::MemberLevelChange(int iID, int iLevel)
{
	auto it = m_Members.begin(), itEnd = m_Members.end();
	for(int i = 0; it != itEnd && i < MAX_PARTY_OR_FORCE; it++, i++)
	{
		__InfoPartyOrForce* pIP = &(*it); // I did this to make debugging easier.
		if(pIP->iID == iID)
		{
			pIP->iLevel = iLevel;
			break;
		}
	}
}

void CUIPartyOrForce::MemberClassChange(int iID, e_Class eClass)
{
	auto it = m_Members.begin(), itEnd = m_Members.end();
	for(int i = 0; it != itEnd && i < MAX_PARTY_OR_FORCE; it++, i++)
	{
		__InfoPartyOrForce* pIP = &(*it); // I did this to make debugging easier.
		if(pIP->iID == iID)
		{
			pIP->eClass = eClass;
			break;
		}
	}
}

void CUIPartyOrForce::Tick()
{
	CN3UIBase::Tick();

	bool bBlink = false;
	DWORD dwTime = GetTickCount();

	dwTime = dwTime/1000;
	dwTime %= 2;

	if(dwTime == 1)	bBlink = true;

	auto it = m_Members.begin(), itEnd = m_Members.end();
	for(int i = 0; it != itEnd && i < MAX_PARTY_OR_FORCE; it++, i++)
	{
		const __InfoPartyOrForce* pIP = &(*it); // I did this to make debugging easier.
		if(m_pProgress_HPs[i])
		{
			if( pIP->bSufferDown_HP || pIP->bSufferDown_Etc )
				m_pProgress_HPs[i]->SetVisible(false);
			else
				m_pProgress_HPs[i]->SetVisible(true);
		}

		if( pIP->bSufferDown_HP && pIP->bSufferDown_Etc )
		{
			if(bBlink)
			{
				if(m_pProgress_HPReduce[i])	m_pProgress_HPReduce[i]->SetVisible(true);
				if(m_pProgress_ETC[i])		m_pProgress_ETC[i]->SetVisible(false);
			}
			else
			{
				if(m_pProgress_HPReduce[i])	m_pProgress_HPReduce[i]->SetVisible(false);
				if(m_pProgress_ETC[i])		m_pProgress_ETC[i]->SetVisible(true);
			}
		}
		else
		{
			if(m_pProgress_HPReduce[i])
			{
				if( pIP->bSufferDown_HP )
					m_pProgress_HPReduce[i]->SetVisible(true);
				else
					m_pProgress_HPReduce[i]->SetVisible(false);
			}
			if(m_pProgress_ETC[i])
			{
				if( pIP->bSufferDown_Etc )
					m_pProgress_ETC[i]->SetVisible(true);
				else
					m_pProgress_ETC[i]->SetVisible(false);
			}
		}
	}
}

bool CUIPartyOrForce::OnKeyPress(int iKey)
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
