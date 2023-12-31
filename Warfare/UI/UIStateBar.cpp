#include "pch.h"
#include "GameDef.h"
#include "UIStateBar.h"
#include "GameProcedure.h"
#include "LocalInput.h"

#include "N3UIProgress.h"
#include "N3UIString.h"
#include "N3UIImage.h"
#include "GameProcMain.h"
#include "APISocket.h"
#include "PacketDef.h"
#include "PlayerMySelf.h"
#include "MagicSkillMng.h"
#include "UIManager.h"

#include "N3Texture.h"

#include "N3UIDBCLButton.h"

CUIStateBar::CUIStateBar()
{
	m_pText_Position = nullptr;
	m_pProgress_HP = nullptr;
	m_pProgress_MSP = nullptr;
	m_pProgress_ExpC = nullptr;
	m_pProgress_ExpP = nullptr;

	// mini map...
	m_pGroup_MiniMap = nullptr;
	m_pImage_Map = nullptr;
	m_pBtn_ZoomIn = nullptr;
	m_pBtn_ZoomOut = nullptr; 
	memset(m_vArrows, 0, sizeof(m_vArrows));
	m_fZoom = 6.0f;
	m_fMapSizeX = 0.0f;
	m_fMapSizeZ = 0.0f;
	m_fYawPlayer = 0;
	m_vPosPlayer.Zero();

	m_pMagic.clear();
}

CUIStateBar::~CUIStateBar()
{
	const auto ite = m_pMagic.end();	
	for(auto it = m_pMagic.begin(); it!=ite; it++)
	{
		const __DurationMagicImg* pMagicImg = (*it);
		if(pMagicImg)
		{
			delete pMagicImg->pIcon;
			delete pMagicImg;
		}
	}

	m_pMagic.clear();
}

void CUIStateBar::Release()
{
	const auto ite = m_pMagic.end();
	for(auto it = m_pMagic.begin(); it!=ite; it++)
	{
		const __DurationMagicImg* pMagicImg = (*it);
		if(pMagicImg)
		{
			delete pMagicImg->pIcon;
			delete pMagicImg;
		}
	}

	m_pMagic.clear();

	CN3UIBase::Release();

	m_pText_Position = nullptr;
	m_pProgress_HP = nullptr;
	m_pProgress_MSP = nullptr;
	m_pProgress_ExpC = nullptr;
	m_pProgress_ExpP = nullptr;

	// mini map...
	m_pGroup_MiniMap = nullptr;
	m_pImage_Map = nullptr;
	m_pBtn_ZoomIn = nullptr;
	m_pBtn_ZoomOut = nullptr; 
	memset(m_vArrows, 0, sizeof(m_vArrows));
	m_fZoom = 6.0f;
	m_fMapSizeX = 0.0f;
	m_fMapSizeZ = 0.0f;
	m_fYawPlayer = 0;
	m_vPosPlayer.Zero();

}

bool CUIStateBar::Load(HANDLE hFile)
{
	if(CN3UIBase::Load(hFile)==false) return false;

	CN3UIString* pText = (CN3UIString*)(this->GetChildByID("Text_Version")); __ASSERT(pText, "NULL UI Component!!");
	if(pText)
	{
		char szVersion[128];
		sprintf(szVersion, "Ver. %.3f", CURRENT_VERSION / 1000.0f);
		pText->SetString(szVersion);
	}
	m_pText_Position =	(CN3UIString*)(this->GetChildByID("Text_Position"));	__ASSERT(m_pText_Position, "NULL UI Component!!");

	m_pProgress_HP =	(CN3UIProgress*)(this->GetChildByID("Progress_HP"));	__ASSERT(m_pProgress_HP, "NULL UI Component!!");
	m_pProgress_MSP =	(CN3UIProgress*)(this->GetChildByID("Progress_MSP"));	__ASSERT(m_pProgress_MSP, "NULL UI Component!!");
	m_pProgress_ExpC =	(CN3UIProgress*)(this->GetChildByID("Progress_ExpC"));	__ASSERT(m_pProgress_ExpC, "NULL UI Component!!");
	m_pProgress_ExpP =	(CN3UIProgress*)(this->GetChildByID("Progress_ExpP"));	__ASSERT(m_pProgress_ExpP, "NULL UI Component!!");
	
	if(m_pProgress_HP) m_pProgress_HP->SetRange(0, 100);
	if(m_pProgress_MSP) m_pProgress_MSP->SetRange(0, 100);
	if(m_pProgress_ExpC) m_pProgress_ExpC->SetRange(0, 100);
	if(m_pProgress_ExpP) m_pProgress_ExpP->SetRange(0, 100);

	// MiniMap
	m_pGroup_MiniMap = GetChildByID("Group_MiniMap"); __ASSERT(m_pGroup_MiniMap, "NULL UI Component!!");
	if(m_pGroup_MiniMap)
	{
		m_pGroup_MiniMap->SetVisible(false);

		m_pImage_Map =		(CN3UIImage*)(m_pGroup_MiniMap->GetChildByID("Img_MiniMap"));	__ASSERT(m_pImage_Map, "NULL UI Component!!");
		m_pBtn_ZoomIn =		(CN3UIButton*)(m_pGroup_MiniMap->GetChildByID("Btn_ZoomIn"));	__ASSERT(m_pBtn_ZoomIn, "NULL UI Component!!");
		m_pBtn_ZoomOut =	(CN3UIButton*)(m_pGroup_MiniMap->GetChildByID("Btn_ZoomIn"));	__ASSERT(m_pBtn_ZoomOut, "NULL UI Component!!");
	}

	return true;
}

bool CUIStateBar::LoadMap(const std::string& szMiniMapFN, float fMapSizeX, float fMapSizeZ)
{
	m_fMapSizeX = fMapSizeX;
	m_fMapSizeZ = fMapSizeZ;
	if(nullptr == m_pImage_Map) return false;

	m_pImage_Map->SetTex(szMiniMapFN);
	return true;
}


void CUIStateBar::UpdateExp(int iExp, int iExpNext, bool bUpdateImmediately) const
{
	__ASSERT(iExpNext, "Next Exp is 0");
	if(iExpNext <= 0) return;
	if(nullptr == m_pProgress_ExpC || nullptr == m_pProgress_ExpP) return;

	const int iPercentage = 100 * iExp / iExpNext;

	if(iExpNext > 10)
	{
		const int iExpNext2 = iExpNext/10;
		const int iExp2 = iExp%iExpNext2;
		const int iPercentage2 = 100 * iExp2 / iExpNext2;

		if(bUpdateImmediately) m_pProgress_ExpC->SetCurValue(iPercentage2);	 // SetCurValue --> if set
		else m_pProgress_ExpC->SetCurValue(iPercentage2, 0.7f, 50.0f);
	}
	else
	{
		m_pProgress_ExpC->SetCurValue(0);	 // SetCurValue --> if set
	}

	if(bUpdateImmediately) m_pProgress_ExpP->SetCurValue(iPercentage);	 // SetCurValue --> if set
	else m_pProgress_ExpP->SetCurValue(iPercentage, 0.3f, 100.0f);
}

void CUIStateBar::UpdateMSP(int iMSP, int iMSPMax, bool bUpdateImmediately)
{
	__ASSERT(iMSPMax, "Max MP is 0");
	if(iMSPMax <= 0) return;
	if(nullptr == m_pProgress_MSP) return;

	const int iPercentage = 100 * iMSP / iMSPMax;

	if(bUpdateImmediately) m_pProgress_MSP->SetCurValue(iPercentage);	 // SetCurValue --> if set
	else m_pProgress_MSP->SetCurValue(iPercentage, 0.3f, 100.0f);
}

void CUIStateBar::UpdateHP(int iHP, int iHPMax, bool bUpdateImmediately)
{
	__ASSERT(iHPMax, "Max HP is 0");
	if(iHPMax <= 0) return;

	const int iPercentage = 100 * iHP / iHPMax;

	if(bUpdateImmediately) m_pProgress_HP->SetCurValue(iPercentage);	 // SetCurValue --> if set
	else m_pProgress_HP->SetCurValue(iPercentage, 0.3f, 100.0f);
}

void CUIStateBar::UpdatePosition(const __Vector3 &vPos, float fYaw)
{
	if(nullptr == m_pText_Position) return;

	char szPos[64];
	sprintf(szPos, "%.1f, %.1f", vPos.x, vPos.z);
	m_pText_Position->SetString(szPos);

	// minimap.
	m_vPosPlayer = vPos;
	m_fYawPlayer = fYaw;
}

void CUIStateBar::Render()
{
	if(false == m_bVisible) return;

	CN3UIBase::Render();

	if(nullptr == m_pGroup_MiniMap || false == m_pGroup_MiniMap->IsVisible()) return; // The minimap is not turned on, so I'm going back.
	if(nullptr == m_pImage_Map) return;
	if(m_fMapSizeX <= 0 || m_fMapSizeZ <= 0) return;

	__VertexTransformedColor vPositions[4], vOutLines[4];
 
	vPositions[0].Set(0, 0, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xffffffff);
	vPositions[1] = vPositions[0];
	vPositions[2] = vPositions[0];
	vPositions[3] = vPositions[0];
	vOutLines[0].Set(0, 0, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff000000);
	vOutLines[1] = vOutLines[0];
	vOutLines[2] = vOutLines[0];
	vOutLines[3] = vOutLines[0];
	
	__PositionInfo info;

	const RECT rc = m_pImage_Map->GetRegion();
	const float fWidth = rc.right - rc.left;
	const float fHeight = rc.bottom - rc.top;
	const float fCenterX = rc.left + fWidth / 2.0f;
	const float fCenterY = rc.top + fHeight / 2.0f;

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

	CN3Base::s_lpD3DDev->SetFVF(FVF_TRANSFORMEDCOLOR); // Displays the player's position and direction.

	__Vector3 vPos;
	auto it = m_Positions.begin(), itEnd = m_Positions.end();
	for(; it != itEnd; it++)
	{
		info = *it;
		
		vPos = m_vPosPlayer - info.vPos;
		// vPos.x = (float)((int)(fCenterX - m_fZoom * fWidth * (vPos.x / m_fMapSizeX)));
		// vPos.z = (float)((int)(fCenterY + m_fZoom * fHeight * (vPos.z / m_fMapSizeZ)));
		vPos.x = fCenterX - m_fZoom * fWidth * (vPos.x / m_fMapSizeX);
		vPos.y = fCenterY + m_fZoom * fHeight * (vPos.z / m_fMapSizeZ);

		vPositions[0].color = info.crType;
		vPositions[1].color = info.crType;
		vPositions[2].color = info.crType;
		vPositions[3].color = info.crType;

		vPositions[0].x = vPos.x - 1.0f;
		vPositions[0].y = vPos.y - 1.0f;
		vPositions[1].x = vPos.x + 1.0f;
		vPositions[1].y = vPos.y - 1.0f;
		vPositions[2].x = vPos.x + 1.0f;
		vPositions[2].y = vPos.y + 1.0f;
		vPositions[3].x = vPos.x - 1.0f;
		vPositions[3].y = vPos.y + 1.0f;

		vOutLines[0].x = vPos.x - 2.0f;
		vOutLines[0].y = vPos.y - 2.0f;
		vOutLines[1].x = vPos.x + 2.0f;
		vOutLines[1].y = vPos.y - 2.0f;
		vOutLines[2].x = vPos.x + 2.0f;
		vOutLines[2].y = vPos.y + 2.0f;
		vOutLines[3].x = vPos.x - 2.0f;
		vOutLines[3].y = vPos.y + 2.0f;

		CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vOutLines, sizeof(__VertexTransformedColor));
		CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vPositions, sizeof(__VertexTransformedColor));
	}

	CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, m_vArrows, sizeof(__VertexTransformedColor));

	it = m_PositionsTop.begin(), itEnd = m_PositionsTop.end(); // Something to draw later...
	for(; it != itEnd; it++)
	{
		info = *it;
		
		vPos = m_vPosPlayer - info.vPos;
		// vPos.x = (float)((int)(fCenterX - m_fZoom * fWidth * (vPos.x / m_fMapSizeX)));
		// vPos.z = (float)((int)(fCenterY + m_fZoom * fHeight * (vPos.z / m_fMapSizeZ)));
		vPos.x = fCenterX - m_fZoom * fWidth * (vPos.x / m_fMapSizeX);
		vPos.y = fCenterY + m_fZoom * fHeight * (vPos.z / m_fMapSizeZ);

		vPositions[0].color = info.crType;
		vPositions[1].color = info.crType;
		vPositions[2].color = info.crType;
		vPositions[3].color = info.crType;

		vPositions[0].x = vPos.x - 1.0f;
		vPositions[0].y = vPos.y - 1.0f;
		vPositions[1].x = vPos.x + 1.0f;
		vPositions[1].y = vPos.y - 1.0f;
		vPositions[2].x = vPos.x + 1.0f;
		vPositions[2].y = vPos.y + 1.0f;
		vPositions[3].x = vPos.x - 1.0f;
		vPositions[3].y = vPos.y + 1.0f;

		vOutLines[0].x = vPos.x - 2.0f;
		vOutLines[0].y = vPos.y - 2.0f;
		vOutLines[1].x = vPos.x + 2.0f;
		vOutLines[1].y = vPos.y - 2.0f;
		vOutLines[2].x = vPos.x + 2.0f;
		vOutLines[2].y = vPos.y + 2.0f;
		vOutLines[3].x = vPos.x - 2.0f;
		vOutLines[3].y = vPos.y + 2.0f;

		CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vOutLines, sizeof(__VertexTransformedColor));
		CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vPositions, sizeof(__VertexTransformedColor));
	}

	// Render..
	// m_pMagic.clear();
	const auto iteMagic = m_pMagic.end();
	for(auto itMagic = m_pMagic.begin(); itMagic!=iteMagic; itMagic++)
	{
		const __DurationMagicImg* pMagicImg = (*itMagic);
		pMagicImg->pIcon->Render();
	}
	
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

void CUIStateBar::Tick()
{
	if(!m_bVisible) return;

	CN3UIBase::Tick();

	TickMiniMap(); // map image...
	TickMagicIcon(); // Handling icons..
}

void CUIStateBar::TickMiniMap()
{
	if(nullptr == m_pImage_Map) return;
	if(m_fMapSizeX <= 0 || m_fMapSizeZ <= 0) return;
	if(m_vPosPlayer.x < 0 || m_vPosPlayer.x > m_fMapSizeX || m_vPosPlayer.z < 0 || m_vPosPlayer.z > m_fMapSizeZ) return;

	const float fOffset = (0.5f/m_fZoom);
	const float fX = (m_vPosPlayer.x / m_fMapSizeX); // 1/16 accumulation..
	const float fY = (m_vPosPlayer.z / m_fMapSizeZ);
	// m_pImage_Map->SetUVRect(fX - fOffset, fY - fOffset, fX + fOffset, fY + fOffset);
	m_pImage_Map->SetUVRect((fX - fOffset), 1.0f - (fY + fOffset), (fX + fOffset), 1.0f - (fY - fOffset));

	const RECT rc = m_pImage_Map->GetRegion();
	const float fH = (rc.bottom - rc.top)/30.0f;

	__Matrix44 mtxRot; mtxRot.RotationZ(m_fYawPlayer);
	mtxRot.PosSet(rc.left + (rc.right - rc.left)/2.0f, rc.top + (rc.bottom - rc.top)/2.0f, 0);

	// arrow setting...
	m_vArrows[0].Set(0, -fH, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
	m_vArrows[1].Set(0, fH/2.0f, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);
	m_vArrows[2].Set(-fH, fH, UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xff00ff00);

	m_vArrows[3] = m_vArrows[0];
	m_vArrows[4] = m_vArrows[2]; m_vArrows[4].x = -m_vArrows[4].x;
	m_vArrows[5] = m_vArrows[1];

	for(int i = 0; i < 6; i++) m_vArrows[i] *= mtxRot; // Position and rotation conversion..
}

void CUIStateBar::TickMagicIcon()
{
	const int cnt = m_pMagic.size();
	auto it = m_pMagic.begin();
	__TABLE_UPC_SKILL* pRemoveSkill = nullptr;
	for(int i=0;i<cnt;i++,it++)
	{
		__DurationMagicImg* pMagicImg = (*it);

		pMagicImg->fDuration -= CN3Base::s_fSecPerFrm;
		if(pMagicImg->fDuration<=0.0f)
		{
			pRemoveSkill = CGameBase::s_pTbl_Skill->Find(pMagicImg->dwSkillID);
			break;
		}
		
		if(pMagicImg->fDuration<0.5f) pMagicImg->pIcon->SetVisible(true);
		else if(pMagicImg->fDuration<1.0f) pMagicImg->pIcon->SetVisible(false);
		else if(pMagicImg->fDuration<1.5f) pMagicImg->pIcon->SetVisible(true);
		else if(pMagicImg->fDuration<2.0f) pMagicImg->pIcon->SetVisible(false);
		else if(pMagicImg->fDuration<2.5f) pMagicImg->pIcon->SetVisible(true);
		else if(pMagicImg->fDuration<3.0f) pMagicImg->pIcon->SetVisible(false);
		else if(pMagicImg->fDuration<3.5f) pMagicImg->pIcon->SetVisible(true);
		else if(pMagicImg->fDuration<4.0f) pMagicImg->pIcon->SetVisible(false);
		else if(pMagicImg->fDuration<4.5f) pMagicImg->pIcon->SetVisible(true);
		else if(pMagicImg->fDuration<5.0f) pMagicImg->pIcon->SetVisible(false);
		else if(pMagicImg->fDuration<5.5f) pMagicImg->pIcon->SetVisible(true);
		else if(pMagicImg->fDuration<6.0f) pMagicImg->pIcon->SetVisible(false);
		else if(pMagicImg->fDuration<6.5f) pMagicImg->pIcon->SetVisible(true);
		else if(pMagicImg->fDuration<7.0f) pMagicImg->pIcon->SetVisible(false);
		else if(pMagicImg->fDuration<7.5f) pMagicImg->pIcon->SetVisible(true);
		else if(pMagicImg->fDuration<8.0f) pMagicImg->pIcon->SetVisible(false);
		else if(pMagicImg->fDuration<8.5f) pMagicImg->pIcon->SetVisible(true);
		else if(pMagicImg->fDuration<9.0f) pMagicImg->pIcon->SetVisible(false);		
	}

	if(pRemoveSkill) DelMagic(pRemoveSkill);
}

bool CUIStateBar::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if(dwMsg==UIMSG_ICON_DBLCLK)
	{
		const auto ite = m_pMagic.end();	
		for(auto it = m_pMagic.begin(); it!=ite; it++)
		{
			__DurationMagicImg* pMagicImg = (*it);

			if(pSender==pMagicImg->pIcon)
			{
				const DWORD MagicID = pMagicImg->dwSkillID;
				const __TABLE_UPC_SKILL* pSkill = CGameBase::s_pTbl_Skill->Find(MagicID);
				if(!pSkill) return false;

				if(CGameProcedure::s_pProcMain->m_pMagicSkillMng->IsPositiveMagic(MagicID))
				{
					if(pSkill->dw1stTableType==3 || pSkill->dw2ndTableType==3) pMagicImg->fDuration = 0.0f;

					// get rid of it...
					BYTE byBuff[32];
					int iOffset=0;
					CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
					CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_CANCEL);
					CAPISocket::MP_AddDword(byBuff, iOffset, (int)MagicID);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)CGameBase::s_pPlayer->IDNumber());
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)CGameBase::s_pPlayer->IDNumber());

					CAPISocket::MP_AddShort(byBuff, iOffset, 0);
					CAPISocket::MP_AddShort(byBuff, iOffset, 0);
					CAPISocket::MP_AddShort(byBuff, iOffset, 0);

					CAPISocket::MP_AddShort(byBuff, iOffset, 0);
					CAPISocket::MP_AddShort(byBuff, iOffset, 0);
					CAPISocket::MP_AddShort(byBuff, iOffset, 0);

					CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
				}
			}
		}	
	}	
	return false;
}

void CUIStateBar::PositionInfoAdd(int iID, const __Vector3 &vPos, D3DCOLOR crID, bool bDrawTop)
{
	__PositionInfo info;
	info.iID = iID;
	info.vPos = vPos;
	info.crType = crID;

	if(bDrawTop) m_PositionsTop.push_back(info);
	else m_Positions.push_back(info);
}

void CUIStateBar::PositionInfoClear()
{
	m_Positions.clear();
	m_PositionsTop.clear();
}

void CUIStateBar::ZoomSet(float fZoom)
{
	m_fZoom = fZoom;
}

bool CUIStateBar::ToggleMiniMap()
{
	if(nullptr == m_pGroup_MiniMap) return false;

	const bool bVisible = m_pGroup_MiniMap->IsVisible();
	m_pGroup_MiniMap->SetVisible(!bVisible);

	return !bVisible;
}

void CUIStateBar::AddMagic(__TABLE_UPC_SKILL* pSkill, float fDuration)
{
	std::vector<char> buffer(256, NULL);
	sprintf(buffer.data(),	"UI\\skillicon_%.2d_%d.dxt", pSkill->dwID%100, pSkill->dwID/100);

	auto* pMagicImg = new __DurationMagicImg;
	pMagicImg->fDuration = fDuration;
	pMagicImg->pIcon = new CN3UIDBCLButton;
	pMagicImg->dwSkillID = pSkill->dwID;

	CN3UIDBCLButton* pIcon = pMagicImg->pIcon;
	pIcon->Init(this);
	pIcon->SetTex(buffer.data());
	pIcon->SetTooltipText(pSkill->szName.c_str());
	pIcon->SetUVRect(0,0,1,1);

	CN3Texture* pTex = pIcon->GetTex();
	if(!pTex)
	{
		delete pIcon;
		delete pMagicImg;
		return;
	}
	
	RECT rt;
	rt.left = rt.top = 0;
	rt.right = pTex->Width();
	rt.bottom = pTex->Height();
	const int PosX = CN3Base::s_CameraData.vp.Width - (rt.right)*(m_pMagic.size()+1);
	pIcon->SetRegion(rt);
	pIcon->SetPos(PosX,0);

	m_pMagic.push_back(pMagicImg);
}

void CUIStateBar::DelMagic(__TABLE_UPC_SKILL* pSkill)
{
	std::vector<char> buffer(256, NULL);
	sprintf(buffer.data(),	"UI\\skillicon_%.2d_%d.dxt", pSkill->dwID%100, pSkill->dwID/100);

	auto itRemove = m_pMagic.end();
	const auto ite = m_pMagic.end();
	for(auto it = m_pMagic.begin(); it!=ite; it++)
	{
		const __DurationMagicImg* pMagicImg = (*it);
		CN3UIDBCLButton* pIcon = pMagicImg->pIcon;
		const CN3Texture* pTex = pIcon->GetTex();
		if(pTex && lstrcmpi(pTex->FileName().c_str(), buffer.data())==0)
		{
			itRemove = it;
		}
		if(itRemove!=ite)
		{
			const POINT pos = pIcon->GetPos();
			const RECT rt = pIcon->GetRegion();			
			pIcon->SetPos(pos.x + (rt.right-rt.left),0);
		}
	}
	if(itRemove!=ite)
	{
		const __DurationMagicImg* pMagicImg = (*itRemove);
		const CN3UIDBCLButton* pIcon = pMagicImg->pIcon;
		delete pIcon;
		delete pMagicImg;
		m_pMagic.erase(itRemove);
	}	
}

void CUIStateBar::ClearMagic()
{
	const auto ite = m_pMagic.end();
	for(auto it = m_pMagic.begin(); it!=ite; it++)
	{
		const __DurationMagicImg* pMagicImg = (*it);
		const CN3UIDBCLButton* pIcon = pMagicImg->pIcon;
		delete pIcon;
		delete pMagicImg;		
	}
	m_pMagic.clear();
}

DWORD CUIStateBar::MouseProc(DWORD dwFlags, const POINT &ptCur, const POINT &ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;

	const int cnt = m_pMagic.size();
	auto it = m_pMagic.begin();
	for(int i=0;i<cnt;i++,it++)
	{
		const __DurationMagicImg* pMagicImg = (*it);

		dwRet |= pMagicImg->pIcon->MouseProc(CGameProcedure::s_pLocalInput->MouseGetFlag(),
									CGameProcedure::s_pLocalInput->MouseGetPos(),
									CGameProcedure::s_pLocalInput->MouseGetPosOld());
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);

	return dwRet;
}

bool CUIStateBar::OnKeyPress(int iKey)
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
