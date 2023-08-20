#include "pch.h"
#include "PlayerBase.h"

#include "N3WorldManager.h"

#include "N3ShapeExtra.h"
#include "DFont.h"
#include "N3SndObj.h"

#include "N3FXMgr.h"

static const __Vector3 s_vLightOffset = __Vector3(10.0f, 40.0f, 30.0f);
CN3SndObj* CPlayerBase::m_pSnd_MyMove = nullptr;

CPlayerBase::CPlayerBase()
{
	m_ePlayerType = PLAYER_BASE; // Player Type ... Base, NPC, OTher, MySelf

	m_Chr.PartAlloc(PART_POS_COUNT); // Basically parts and plug settings..
	m_Chr.PlugAlloc(PLUG_POS_COUNT);

	m_pLooksRef = nullptr;				// Basic appearance and basic information reference table
	memset(m_pItemPlugBasics, 0, sizeof(m_pItemPlugBasics)); // Weapons attached to the character.
	memset(m_pItemPlugExts, 0, sizeof(m_pItemPlugExts));
	memset(m_pItemPartBasics, 0, sizeof(m_pItemPartBasics)); // Armor attached to the character.
	memset(m_pItemPartExts, 0, sizeof(m_pItemPartExts)); // Armor attached to the character.

	m_iIDTarget = -1;
	m_iMagicAni = 0;
	m_bGuardSuccess = false;	// A flag for whether the defense was successful.
	m_bVisible = true;			// can you see??

	m_iDroppedItemID = 0;	// Items dropped after death

	m_Chr.m_nJointPartStarts[0] = 1;	// Upper body - Separate animations and process... Joint index.
	m_Chr.m_nJointPartEnds[0] = 15;	// upper body

	m_Chr.m_nJointPartStarts[1] = 16;	// lower body
	m_Chr.m_nJointPartEnds[1] = 23;	// lower body

	m_cvDuration.a = m_cvDuration.r = m_cvDuration.g = m_cvDuration.b = 1.0f; // persistent color value
	m_fDurationColorTimeCur = 0; // current time..
	m_fDurationColorTime = 0; // duration..

	m_fFlickeringFactor = 1.0f; // A flickering alpha value of 1.0 means no flickering....
	m_fFlickeringTime = 0; // Blink time...

	m_fTimeAfterDeath = 0; // Timer to take a dying motion - 5 seconds is appropriate?? If you get hit before that, you die immediately.

	m_eStateNext = m_eState = PSA_BASIC;
	m_eStateMove = PSM_STOP; // stop at the beginning..
	m_eStateDying = PSD_UNKNOWN; // how to die
	m_fTimeDying = 0; // The last time when I took the motion to die..

	m_fRotRadianPerSec = D3DXToRadian(270.0f); // Rotation radians per second
	m_fMoveSpeedPerSec = 0; // Movement value per second.. This value is the default value and is added or subtracted depending on the state (walking, running, back, cursing, etc.).
	m_fYawCur = 0; // Current rotation value..
	m_fYawToReach = 0;

	m_fGravityCur = 0;			// Gravity value...
	m_fYNext = 0;				// Height value according to object or terrain collision check.

	m_fScaleToSet = 1.0f;			// Gradually change the scale value..
	m_fScalePrev = 1.0f;

	m_pSnd_Move = nullptr;
	m_pSnd_Attack_0 = nullptr;
	// m_pSnd_Attack_1 = NULL;
	m_pSnd_Struck_0 = nullptr;
	// m_pSnd_Struck_1 = NULL;
	m_pSnd_Breathe_0 = nullptr;
	// m_pSnd_Breathe_1 = NULL;
	m_pSnd_Blow = nullptr;
	m_bSoundAllSet = false;

	m_InfoBase.Init();				// I don&#39;t know the racial value.

	m_pShapeExtraRef = nullptr;					// If this NPC is in the form of an object such as a gate or a house, this pointer is set and used.

	m_fCastFreezeTime = 0.0f;
	m_iSkillStep = 0;			// Non-zero if the current skill is being used...
	m_fAttackDelta = 1.0f;			// Attack speed that changes by skill or magic.. 1.0 is the default, and the higher it is, the faster it attacks.
	m_fMoveDelta = 1.0f;			// Movement speed that changes by skill or magic is 1.0 by default, and the higher it is, the faster it moves.

	m_vDirDying.Set(0, 0, 1); // The direction you move when you die.

	// shadow reset
	// By : Ecli666 ( On 2002-03-29 4:23:36 PM )
	/*
	m_pTexShadow = NULL;
	m_pTexShadow = s_MngTex.Get("Chr\\Shadow_Character.tga");
	m_vShadows[0].Set(-0.7f, 0, 0.7f, 0, 0);
	m_vShadows[1].Set( 0.7f, 0, 0.7f, 1, 0);
	m_vShadows[2].Set( 0.7f, 0,-0.7f, 1, 1);
	m_vShadows[3].Set(-0.7f, 0,-0.7f, 0, 1);
	*/
	// ~(By Ecli666 On Mar 29 2002 4:23:36 PM )

	// Font initialization... // Create separate fonts for information display and balloons..
	m_pIDFont = nullptr;
	m_pClanFont = nullptr;
	m_pInfoFont = nullptr;
	m_pBalloonFont = nullptr;
	m_fTimeBalloon = 0;

	m_bAnimationChanged = false;	// It is set only when the cued animation changes.

	m_pvVertex[0].Set(-SHADOW_PLANE_SIZE, 0.0f, SHADOW_PLANE_SIZE, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	m_pvVertex[1].Set(SHADOW_PLANE_SIZE, 0.0f, SHADOW_PLANE_SIZE, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	m_pvVertex[2].Set(SHADOW_PLANE_SIZE, 0.0f, -SHADOW_PLANE_SIZE, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	m_pvVertex[3].Set(-SHADOW_PLANE_SIZE, 0.0f, -SHADOW_PLANE_SIZE, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);

	unsigned short* pIdx = m_pIndex;

	// bottom side.
	*pIdx++ = 0;  *pIdx++ = 1;  *pIdx++ = 3;
	*pIdx++ = 2;  *pIdx++ = 3;  *pIdx++ = 1;

	m_N3Tex.Create(SHADOW_SIZE, SHADOW_SIZE, D3DFMT_A4R4G4B4, 0);
}

CPlayerBase::~CPlayerBase()
{
	// By : Ecli666 ( On 2002-03-29 4:24:14 PM )
	//
	// s_MngTex.Delete(m_pTexShadow);

	// ~(By Ecli666 On Mar 29 2002 4:24:14 PM )
	delete m_pClanFont; m_pClanFont = nullptr;
	delete m_pIDFont; m_pIDFont = nullptr;
	delete m_pInfoFont; m_pInfoFont = nullptr;
	delete m_pBalloonFont; m_pBalloonFont = nullptr;

	m_bSoundAllSet = false;
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Move);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_0);
	// CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_0);
	// CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_0);
	// CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Blow);

	if (m_pShapeExtraRef) m_pShapeExtraRef->m_bVisible = false; // make it invisible...
}

void CPlayerBase::Release()
{
	m_AnimationDeque.clear();

	m_Chr.Release();
	m_Chr.PartAlloc(PART_POS_COUNT); // Basically parts and plug settings..
	m_Chr.PlugAlloc(PLUG_POS_COUNT);

	m_bSoundAllSet = false;
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Move);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_0);
	// CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_0);
	// CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_0);
	// CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Blow);

	m_pLooksRef = nullptr;				// Basic appearance and basic information reference table
	memset(m_pItemPlugBasics, 0, sizeof(m_pItemPlugBasics)); // Weapons attached to the character.
	memset(m_pItemPlugExts, 0, sizeof(m_pItemPlugExts));
	memset(m_pItemPartBasics, 0, sizeof(m_pItemPartBasics)); // Armor attached to the character.
	memset(m_pItemPartExts, 0, sizeof(m_pItemPartExts)); // Armor attached to the character.

	m_iIDTarget = -1;
	m_bGuardSuccess = false; // A flag for whether the defense was successful.
	m_bVisible = true;			// can you see??

	m_cvDuration.a = m_cvDuration.r = m_cvDuration.g = m_cvDuration.b = 1.0f; // persistent color value
	m_fDurationColorTimeCur = 0; // current time..
	m_fDurationColorTime = 0; // duration..

	m_fFlickeringFactor = 1.0f; // A flickering alpha value of 1.0 means no flickering....
	m_fFlickeringTime = 0; // Blink time...

	m_fTimeAfterDeath = 0; // Timer to take a dying motion - 5 seconds is appropriate?? If you get hit before that, you die instantly.

	m_eStateNext = m_eState = PSA_BASIC;
	m_eStateMove = PSM_STOP; // stop at the beginning..
	m_eStateDying = PSD_UNKNOWN; // how to die
	m_fTimeDying = 0; // The last time when I took the motion to die..

	m_fRotRadianPerSec = D3DXToRadian(270.0f); // Rotation radians per second
	m_fMoveSpeedPerSec = 0; // Movement value per second.. This value is the default value and is added or subtracted depending on the state (walking, running, back, cursing, etc.).
	m_fYawCur = 0; // Current rotation value..
	m_fYawToReach = 0;

	m_fGravityCur = 0;			// Gravity value...
	m_fYNext = 0;				// Height value according to object or terrain collision check.

	m_fScaleToSet = 1.0f;			// Gradually change the scale value..
	m_fScalePrev = 1.0f;

	m_InfoBase.Init();				// I don&#39;t know the racial value.

	m_pShapeExtraRef = nullptr;					// If this NPC is in the form of an object such as a gate or a house, this pointer is set and used.

	m_fCastFreezeTime = 0.0f;
	m_iSkillStep = 0;			// Non-zero if the current skill is being used...
	m_fAttackDelta = 1.0f;		// Attack speed that changes by skill or magic.. 1.0 is the default, and the higher it is, the faster it attacks.
	m_fMoveDelta = 1.0f;			// Movement speed that changes by skill or magic is 1.0 by default, and the higher it is, the faster it moves.
	m_vDirDying.Set(0, 0, 1); // The direction you move when you die.

	m_bAnimationChanged = false;	// It is set only when the cued animation changes.

	if (m_pShapeExtraRef) m_pShapeExtraRef->m_bVisible = false; // make it invisible...

	CGameBase::Release();
}

void CPlayerBase::SetSoundAndInitFont()
{
	if (!m_pLooksRef) return;
	if (true == m_bSoundAllSet) return;

	m_bSoundAllSet = true;

	if (!m_pSnd_Move) m_pSnd_Move = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Move);
	if (!m_pSnd_Struck_0) m_pSnd_Struck_0 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Struck0);
	// if(!m_pSnd_Struck_1) m_pSnd_Struck_1 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Struck1);
	if (!m_pSnd_Breathe_0) m_pSnd_Breathe_0 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Breathe0);
	// if(!m_pSnd_Breathe_1) m_pSnd_Breathe_1 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Breathe1);
	if (!m_pSnd_Blow) m_pSnd_Blow = CN3Base::s_SndMgr.CreateObj(100);

	// The sound corresponding to the weapon...
	__TABLE_ITEM_BASIC* pItemBasic = m_pItemPlugBasics[PLUG_POS_RIGHTHAND];
	if (!pItemBasic) pItemBasic = m_pItemPlugBasics[PLUG_POS_LEFTHAND];
	this->SetSoundPlug(pItemBasic);

	// Font initialization..
	if (!m_pIDFont)
	{
		std::string szFontID; ::_LoadStringFromResource(IDS_FONT_ID, szFontID);
		m_pIDFont = new CDFont(szFontID, 12);
		m_pIDFont->InitDeviceObjects(s_lpD3DDev);
		m_pIDFont->RestoreDeviceObjects();

		m_pIDFont->SetText(m_InfoBase.szID.c_str(), D3DFONT_BOLD); // Assign text to fonts.
		m_pIDFont->SetFontColor(m_InfoBase.crID);
	}
}

void CPlayerBase::SetSoundPlug(__TABLE_ITEM_BASIC* pItemBasic)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_0);
	// CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_1);

	if (pItemBasic) // put the sound...
	{
		m_pSnd_Attack_0 = CN3Base::s_SndMgr.CreateObj(pItemBasic->dwSoundID0);
		// m_pSnd_Attack_1 = CN3Base::s_SndMgr.CreateObj(pItemBasic-&gt;dwSoundID1); // correct sound..
	}
	else
	{
		m_pSnd_Attack_0 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Attack0); // swinging sound.
		// m_pSnd_Attack_1 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef-&gt;iSndID_Attack1); // swinging sound.
	}
}

void CPlayerBase::ReleaseSoundAndFont()
{
	if (false == m_bSoundAllSet) return;
	m_bSoundAllSet = false;

	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Move);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_0);
	// CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_0);
	// CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_0);
	// CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Blow);

	delete m_pIDFont; m_pIDFont = nullptr;
	delete m_pInfoFont; m_pInfoFont = nullptr;
	delete m_pClanFont; m_pClanFont = nullptr;
	delete m_pBalloonFont; m_pBalloonFont = nullptr;
}

void CPlayerBase::InfoStringSet(const std::string& szInfo, D3DCOLOR crFont)
{
	if (szInfo.empty())
	{
		delete m_pInfoFont; m_pInfoFont = nullptr;
		return;
	}
	else
	{
		if (nullptr == m_pInfoFont)
		{
			// Font to display other information..
			std::string szFontInfo; ::_LoadStringFromResource(IDS_FONT_INFO, szFontInfo);
			m_pInfoFont = new CDFont(szFontInfo, 12);
			m_pInfoFont->InitDeviceObjects(s_lpD3DDev);
			m_pInfoFont->RestoreDeviceObjects();
		}
		m_pInfoFont->SetText(szInfo.c_str(), D3DFONT_BOLD); // Assign text to fonts.
		m_pInfoFont->SetFontColor(crFont);
	}
}

void CPlayerBase::BalloonStringSet(const std::string& szBalloon, D3DCOLOR crFont)
{
	if (szBalloon.empty())
	{
		delete m_pBalloonFont; m_pBalloonFont = nullptr;
		m_fTimeBalloon = 0; // Balloon display time...
		return;
	}
	else
	{
		if (nullptr == m_pBalloonFont)
		{
			// Font to display other information..
			std::string szFontBalloon; ::_LoadStringFromResource(IDS_FONT_BALLOON, szFontBalloon);
			m_pBalloonFont = new CDFont(szFontBalloon, 12);
			m_pBalloonFont->InitDeviceObjects(s_lpD3DDev);
			m_pBalloonFont->RestoreDeviceObjects();
		}

		m_fTimeBalloon = szBalloon.size() * 0.2f;
	}

	m_pBalloonFont->SetText(szBalloon.c_str(), 0); // Assign text to fonts.
	m_pBalloonFont->SetFontColor(crFont);
}

void CPlayerBase::IDSet(int iID, const std::string& szID, D3DCOLOR crID)
{
	m_InfoBase.iID = iID;
	m_InfoBase.szID = szID; // Replace ID with your name.
	m_InfoBase.crID = crID;

#ifdef _DEBUG
	m_Chr.m_szName = szID; // Enter the name for debugging. So you can distinguish it.
#endif
}

void CPlayerBase::KnightsInfoSet(int iID, const std::string& szName, int iGrade, int iRank)
{
	char szPlug[128] = "";
	if (iGrade > 0 && iGrade <= 5)
	{
		sprintf(szPlug, "Item\\ClanAddOn_%.3d_%d.n3cplug", m_InfoBase.eRace, iGrade); // Create a plug name by race and class.
	}

	CN3CPlugBase* pPlug = this->PlugSet(PLUG_POS_KNIGHTS_GRADE, szPlug, nullptr, nullptr);

	if (nullptr == pPlug) return;

	auto* pCPlug = (CN3CPlug*)pPlug;
	const __TABLE_FX* pFXClanRank = s_pTbl_FXSource->Find(FXID_CLAN_RANK_1);

	std::string szFXClanRank = "";
	std::string szEmpty = "";
	if (pFXClanRank)
	{
		if (iRank <= 5 && iRank >= 1)
			szFXClanRank = pFXClanRank->szFN;
	}
	pCPlug->InitFX(szFXClanRank, szEmpty, 0xffffffff);
}

/*
void CPlayerBase::RenderShadow()
{
	// By : Ecli666 ( On 2002-03-29 4:22:59 PM )
	/*
	LPDIRECT3DTEXTURE8 lpTex = NULL;
	if(m_pTexShadow) lpTex = m_pTexShadow->Get();

	__AlphaPrimitive* pAP = s_AlphaMgr.Add();
	if(pAP)
	{
		pAP->bUseVB				= FALSE;
		pAP->dwBlendDest		= D3DBLEND_INVSRCALPHA;
		pAP->dwBlendSrc			= D3DBLEND_SRCALPHA;
		pAP-&gt;dwFVF = FVF_XYZT1;
		pAP->nPrimitiveCount	= 2;
		pAP->ePrimitiveType		= D3DPT_TRIANGLEFAN;
		pAP->dwPrimitiveSize	= sizeof(__VertexXyzT1);
		pAP->fCameraDistance	= (s_CameraData.vEye - m_Chr.m_Matrix.Pos()).Magnitude();
		pAP->lpTex				= lpTex;
		pAP->nRenderFlags		= RF_NOTZWRITE;
		pAP->nVertexCount		= 4;
		// pAP->MtxWorld			= &m_Matrix;
		pAP->pVertices			= m_vShadows;
		pAP->pwIndices			= NULL;

		pAP->MtxWorld.Identity();
		pAP-&gt;MtxWorld.PosSet(m_Chr.m_Matrix.Pos());
		pAP->MtxWorld._42 = 0.05f;
	}

	for(int i = 0; i < 4; i++)
		m_vShadows[i].y = s_pTerrain->GetHeight(pAP->MtxWorld._41 + m_vShadows[i].x, pAP->MtxWorld._43 + m_vShadows[i].z);

	return; // Do not render.
	*/

	// ~(By Ecli666 On Mar 29 2002 4:22:59 PM )

	/*
	// Render shadows.
	// backup
	DWORD dwAlpha;
	s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);

	// set render state
	if(FALSE == dwAlpha) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	// set texture state (alpha)
	LPDIRECT3DTEXTURE8 lpTex = m_pTexShadow->Get();
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	s_lpD3DDev->SetTexture(0, lpTex);

	static __Matrix44 mtx;
	mtx.Identity();
	mtx.PosSet(m_Chr.m_Matrix.Pos());
	mtx._41 += 0.1f;
	mtx._42 = 0.05f;
	mtx._43 -= 0.1f;
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtx);
	s_lpD3DDev-&gt;SetFVF(FVF_XYZT1);

	for(int i = 0; i < 4; i++)
		m_vShadows[i].y = s_pTerrain->GetHeight(mtx._41 + m_vShadows[i].x, mtx._43 + m_vShadows[i].z);

	s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_vShadows, sizeof(__VertexXyzT1));

	// restore
	if(FALSE == dwAlpha) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);

}*/

void CPlayerBase::RenderChrInRect(CN3Chr* pChr, const RECT& Rect)
{
	if (nullptr == pChr) return;

	// February 15, 2002 Dino Re-edited..
	// The calculation is complicated because you cannot specify the viewport area from less than 0 (i.e. outside the area of the front buffer).

	RECT rcViewport;	// Recalculate the viewport area. (Do not exceed the size of the front buffer..)
	if (Rect.left < 0) rcViewport.left = 0;
	else rcViewport.left = Rect.left;
	if (Rect.top < 0) rcViewport.top = 0;
	else rcViewport.top = Rect.top;
	if (Rect.right > (int)(s_CameraData.vp.X + s_CameraData.vp.Width)) rcViewport.right = s_CameraData.vp.X + s_CameraData.vp.Width;
	else rcViewport.right = Rect.right;
	if (Rect.bottom > (int)(s_CameraData.vp.Y + s_CameraData.vp.Height)) rcViewport.bottom = s_CameraData.vp.Y + s_CameraData.vp.Height;
	else rcViewport.bottom = Rect.bottom;

	// set viewport
	D3DVIEWPORT9 vp;
	vp.X = rcViewport.left;
	vp.Y = rcViewport.top;
	vp.Width = rcViewport.right - rcViewport.left;
	vp.Height = rcViewport.bottom - rcViewport.top;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	if (vp.Width <= 0 ||
		vp.Height <= 0 ||
		vp.Width > s_CameraData.vp.Width ||
		vp.Height > s_CameraData.vp.Height) return;	// If the Width or Height is larger than the front buffer, it is not drawn.

	s_lpD3DDev->SetViewport(&vp);

	// set matrix
	__Matrix44 mtxProj, mtxView;
	const float fChrHeight = pChr->Height() + 0.2f;	// Add about 20cm to the character&#39;s height
	const float fVCenter = fChrHeight * 0.5f;
	const int iWidth = Rect.right - Rect.left;
	const int iHeight = Rect.bottom - Rect.top;
	const float fViewVolumeHeight = fChrHeight * vp.Height / iHeight;	// The character&#39;s height (if it is clipped, narrow it down according to the clipping ratio)
	const float fViewVolumeWidth = fChrHeight * vp.Width / iHeight;	// The width is adjusted according to the aspect ratio of pRect (if it is clipped, it is narrowed according to the clipped ratio).
	// Originally this: fChrHeight * iWidth / iHeight * vp.Width / iWidth;
	D3DXMatrixOrthoLH(&mtxProj, fViewVolumeWidth, fViewVolumeHeight, 0, 20);

	float fCameraMoveX = ((fChrHeight * iWidth / iHeight) - fViewVolumeWidth) / 2.0f;	// Amount of camera movement according to clipping
	float fCameraMoveY = (fChrHeight - fViewVolumeHeight) / 2.0f;
	if (rcViewport.left != Rect.left) fCameraMoveX = -fCameraMoveX;		// Move the camera to the right (because the camera faces the -Z axis, the right side of the camera is the -X side) since the left area needs to be cropped.
	if (rcViewport.top != Rect.top) fCameraMoveY = -fCameraMoveY;			// We need to draw the top area clipped, so move the camera down

	// D3DXMatrixLookAtLH( &amp;mtxView, &amp;D3DXVECTOR3( 0.0f + fCameraMoveX, fVCenter+2.0f + fCameraMoveY, 10.0f ), // where the view matrix is relative to the camera angle, distance has no effect on perspective.
	// &amp;D3DXVECTOR3( 0.0f + fCameraMoveX, fVCenter + fCameraMoveY, 0.0f ), // fVCenter: looking at the middle of a character&#39;s key
	// &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
	const __Vector3& vChrPos = pChr->Pos();
	const auto vEye = D3DXVECTOR3(vChrPos.x + fCameraMoveX, vChrPos.y + fVCenter + 2.0f + fCameraMoveY, vChrPos.z + 10.0f);
	const auto vAt = D3DXVECTOR3(vChrPos.x + fCameraMoveX, vChrPos.y + fVCenter + fCameraMoveY, vChrPos.z + 0.0f);
	const auto vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mtxView, &vEye, &vAt, &vUp);
	s_lpD3DDev->SetTransform(D3DTS_VIEW, &mtxView);
	s_lpD3DDev->SetTransform(D3DTS_PROJECTION, &mtxProj);

	// backup render state
	DWORD dwFog, dwZEnable;// , dwLighting;
	s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	// s_lpD3DDev-&gt;GetRenderState( D3DRS_LIGHTING, &amp;dwLighting ); // Let the lighting be controlled externally.
	s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);

	// set render state
	if (D3DZB_TRUE != dwZEnable) s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	// if (FALSE != dwLighting) s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE);
	if (FALSE != dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// render
	const D3DRECT rc = { Rect.left, Rect.top, Rect.right, Rect.bottom };
	s_lpD3DDev->Clear(1, &rc, D3DCLEAR_ZBUFFER, 0, 1.0f, 0); // Z Buffer Clear

	const int iLODPrev = CN3Chr::LODDelta();
	CN3Chr::LODDeltaSet(0);
	pChr->m_nLOD = 1;	// Make the LOD 0 (as detailed as possible..)
	pChr->Render();
	CN3Chr::LODDeltaSet(iLODPrev);

	// restore
	if (D3DZB_TRUE != dwZEnable) s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	// if (FALSE != dwLighting) s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, dwLighting);
	if (FALSE != dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
	s_lpD3DDev->SetTransform(D3DTS_PROJECTION, &(s_CameraData.mtxProjection));
	s_lpD3DDev->SetTransform(D3DTS_VIEW, &(s_CameraData.mtxView));
	s_lpD3DDev->SetViewport(&(s_CameraData.vp));
}

void CPlayerBase::DurationColorSet(const _D3DCOLORVALUE& color, float fDurationTime)
{
	m_fDurationColorTime = fDurationTime;
	m_fDurationColorTimeCur = 0;
	m_cvDuration = color;
}

void CPlayerBase::FlickerFactorSet(float fAlpha)
{
	if (fAlpha < 0.1f) fAlpha = 0.1f;
	if (fAlpha > 1.0f) fAlpha = 1.0f;

	m_fFlickeringFactor = fAlpha; // A flickering alpha value of 1.0 means no flickering...
	m_fFlickeringTime = 0; // Blink time...
}

void CPlayerBase::RotateTo(float fYaw, bool bImmediately)
{
	int iLot = (int)(fYaw / __PI2);
	if (iLot) fYaw -= iLot * __PI2; // Set between 0 and 360 degrees.

	iLot = (int)(fYaw / __PI);
	if (iLot) fYaw -= iLot * __PI2; // Set between -180 and 180.

	m_fYawToReach = fYaw;
	if (bImmediately) m_fYawCur = fYaw; // If you need to turn...
}

void CPlayerBase::RotateTo(CPlayerBase* pOther)
{
	if (nullptr == pOther || pOther == this) return;

	__Vector3 vDir = pOther->Position() - m_Chr.Pos();
	vDir.y = 0.0f;
	vDir.Normalize();

	const float fYaw = ::_Yaw2D(vDir.x, vDir.z);
	this->RotateTo(fYaw, false); // turn

}

void CPlayerBase::TickYaw()
{
	if (m_fYawCur != m_fYawToReach && this->IsAlive()) // If you have to rotate... the only one alive...
	{
		const float fYawDiff = m_fYawToReach - m_fYawCur; // rotational difference.
		float fYawDelta = m_fRotRadianPerSec * s_fSecPerFrm; // amount to rotate
		if (T_Abs(fYawDiff) <= fYawDelta)
		{
			m_fYawCur = m_fYawToReach; // If the amount to rotate is small.. set it right away
		}
		else
		{
			if (fYawDiff > 0)
			{
				if (fYawDiff < __PI) fYawDelta *= 1.0f;
				else if (fYawDiff >= __PI) fYawDelta *= -1.0f;
			}
			else
			{
				if (fYawDiff > -__PI) fYawDelta *= -1.0f;
				else if (fYawDiff <= -__PI) fYawDelta *= 1.0f;
			}

			m_fYawCur += fYawDelta; // rotation..

			if (T_Abs(m_fYawCur) > __PI)
			{
				int iLot = (int)(m_fYawCur / __PI);
				if (iLot) m_fYawCur -= iLot * __PI2; // Set between 0 and 360 degrees.

				iLot = (int)(m_fYawCur / __PI);
				if (iLot) m_fYawCur -= iLot * __PI2; // Set between -180 and 180.
			}
		}
	}

	// Apply rotation value..
	__Quaternion qtRot;
	qtRot.RotationAxis(0, 1, 0, m_fYawCur); // rotation ....
	m_Chr.RotSet(qtRot);
}

void CPlayerBase::TickAnimation()
{
	// Walking, running, animation, etc... apply speed
	float fAniSpeedDelta = 1.0f;
	if (PSM_STOP != m_eStateMove) fAniSpeedDelta = m_fMoveDelta; // If you are moving, speed is applied.
	else if (PSA_ATTACK == m_eState) fAniSpeedDelta = m_fAttackDelta; // If attacking, attack speed is applied.
	__ASSERT(fAniSpeedDelta >= 0.1f && fAniSpeedDelta < 10.0f, "Invalid Animation Speed Delta!!!");
	m_Chr.AniSpeedDeltaSet(fAniSpeedDelta); // Actual application of animation speed..
	m_Chr.Tick(); // Animation tick..

	m_bAnimationChanged = false;	// It is set only when the cued animation changes.
	if (m_Chr.IsAnimEnd())// After the animation...
	{
		m_bAnimationChanged = true;	// It is set only when the cued animation changes.
		if (m_AnimationDeque.empty()) // If the animation deck is empty...
		{
			CPlayerBase* pTarget = this->CharacterGetByID(m_iIDTarget, true);
			this->Action(m_eStateNext, true, pTarget); // change state...
		}
		else // If the animation deck is not empty and there is an action to be performed...
		{
			const e_Ani eAniToSet = m_AnimationDeque[0]; // Take one off the deck...
			m_AnimationDeque.pop_front();
			m_Chr.AniCurSet(eAniToSet);
			// TRACE("      Animation : %d\n", eAniToSet);
		}
	}
}

void CPlayerBase::TickDurationColor()
{
	if (m_fDurationColorTime <= 0) return;

	if (m_fDurationColorTimeCur > m_fDurationColorTime)
	{
		m_fDurationColorTime = 0;
		m_fDurationColorTimeCur = 0;

		CN3CPart* pPart = nullptr;
		const int iPC = m_Chr.m_Parts.size();
		for (int i = 0; i < iPC; i++)
		{
			pPart = m_Chr.m_Parts[i];
			pPart->m_Mtl = pPart->m_MtlOrg; // return to original color
		}
	}
	else
	{
		const float fD = m_fDurationColorTimeCur / m_fDurationColorTime;
		CN3CPart* pPart = nullptr;
		const int iPC = m_Chr.m_Parts.size();
		for (int i = 0; i < iPC; i++)
		{
			pPart = m_Chr.m_Parts[i];

			// pPart->m_Mtl.Ambient.a = pPart->m_MtlOrg.Ambient.a * fD + m_cvDuration.a * (1.0f - fD);
			pPart->m_Mtl.Ambient.r = pPart->m_MtlOrg.Ambient.r * fD + m_cvDuration.r * (1.0f - fD);
			pPart->m_Mtl.Ambient.g = pPart->m_MtlOrg.Ambient.g * fD + m_cvDuration.g * (1.0f - fD);
			pPart->m_Mtl.Ambient.b = pPart->m_MtlOrg.Ambient.b * fD + m_cvDuration.b * (1.0f - fD);

			// pPart->m_Mtl.Diffuse.a = pPart->m_MtlOrg.Diffuse.a * fD + m_cvDuration.a * (1.0f - fD);
			pPart->m_Mtl.Diffuse.r = pPart->m_MtlOrg.Diffuse.r * fD + m_cvDuration.r * (1.0f - fD);
			pPart->m_Mtl.Diffuse.g = pPart->m_MtlOrg.Diffuse.g * fD + m_cvDuration.g * (1.0f - fD);
			pPart->m_Mtl.Diffuse.b = pPart->m_MtlOrg.Diffuse.b * fD + m_cvDuration.b * (1.0f - fD);
		}
	}

	m_fDurationColorTimeCur += s_fSecPerFrm;
}

void CPlayerBase::TickSound()
{
	const __Vector3 vPos = this->Position();

	if (PSA_ATTACK == m_eState) // When attacking...
	{
		// if(m_pSnd_Attack_0 && m_Chr.NeedPlaySound0())
		// m_pSnd_Attack_0-&gt;Play(&amp;vPos); // During attack 1...
		// if(m_pSnd_Attack_1 && m_Chr.NeedPlaySound1())
		// m_pSnd_Attack_1-&gt;Play(&amp;vPos); // During attack 2...
		if (m_pSnd_Attack_0 && (m_Chr.NeedPlaySound0() || m_Chr.NeedPlaySound1()))
			m_pSnd_Attack_0->Play(&vPos); // During attack 1...
		// if(m_pSnd_Attack_1 && m_Chr.NeedPlaySound1())
		// m_pSnd_Attack_1-&gt;Play(&amp;vPos); // During attack 2...
	}

	if (PSM_STOP == m_eStateMove)
	{
		if (PSA_DYING == m_eState && m_Chr.NeedPlaySound0())
		{
			const int DeadSoundID = (rand() % 2) ? m_pLooksRef->iSndID_Dead0 : m_pLooksRef->iSndID_Dead1;
			CN3Base::s_SndMgr.PlayOnceAndRelease(DeadSoundID, &vPos);
		}
		else if (PSA_BASIC == m_eState && m_Chr.NeedPlaySound0())
		{
			// CN3SndObj* pSndBreath = (rand()%2) ? m_pSnd_Breathe_0 : m_pSnd_Breathe_1;
			// if(pSndBreath)
			// pSndBreath-&gt;Play(&amp;vPos);
			if (m_pSnd_Breathe_0)
				m_pSnd_Breathe_0->Play(&vPos);
		}
	}
	else if (PSM_WALK == m_eStateMove ||
		PSM_RUN == m_eStateMove ||
		PSM_WALK_BACKWARD == m_eStateMove)
	{
		if (m_pSnd_Move && m_pSnd_Move != m_pSnd_MyMove && m_Chr.NeedPlaySound0())
			m_pSnd_Move->Play(&vPos);
		if (m_pSnd_Move && m_pSnd_Move != m_pSnd_MyMove && m_Chr.NeedPlaySound1())
			m_pSnd_Move->Play(&vPos);
	}
}

void CPlayerBase::Tick()  // Rotation, handling specified animation ticks and color assignments.. etc..
{
	if (m_pShapeExtraRef) // If the object...
	{
		m_pShapeExtraRef->Tick(FRAME_SELFPLAY);
		return;
	}

	// Gravity value applied.
	__Vector3 vPos = this->Position();
	if (vPos.y > m_fYNext)
	{
		m_fGravityCur += 9.8f * CN3Base::s_fSecPerFrm;
		vPos.y -= m_fGravityCur;
		this->PositionSet(vPos, false);
	}
	if (vPos.y < m_fYNext)
	{
		vPos.y = m_fYNext;
		this->PositionSet(vPos, false);
		m_fGravityCur = 0;
	}

	// Gradually larger scale applied
	if (m_fScaleToSet != m_fScalePrev)
	{
		float fScale = m_Chr.Scale().y;
		if (m_fScaleToSet > m_fScalePrev) // should grow...
		{
			fScale += (m_fScaleToSet - m_fScalePrev) * s_fSecPerFrm;
			if (fScale > m_fScaleToSet)
			{
				m_fScalePrev = fScale = m_fScaleToSet;
			}
		}
		else // If you need to be small...
		{
			fScale -= (m_fScalePrev - m_fScaleToSet) * s_fSecPerFrm;
			if (fScale < m_fScaleToSet)
			{
				m_fScalePrev = fScale = m_fScaleToSet;
			}
		}
		m_Chr.ScaleSet(fScale, fScale, fScale);
	}

	this->TickYaw();
	this->TickAnimation();
	this->TickDurationColor();
	this->TickSound();

	// Dealing with dying...
	if (m_fTimeAfterDeath > 0) m_fTimeAfterDeath += s_fSecPerFrm; // Timer to take a dying motion - 5 seconds is appropriate?? If you get hit before that, you die immediately.
	if (PSA_DYING == m_eState)
	{
		if (PSD_DISJOINT == m_eStateDying || PSD_KNOCK_DOWN == m_eStateDying) // pushed back...
		{
			float fAD = 0;
			if (m_fTimeDying <= 0.2f) fAD = 5.0f;
			else if (m_fTimeDying < 0.4f) fAD = 5.0f * (0.4f - m_fTimeDying);

			if (fAD > 0)
			{
				const float fDelta = (fAD * (0.3f + 0.7f / m_Chr.Radius()));
				vPos -= m_vDirDying * (fDelta * s_fSecPerFrm); // It is pushed in half proportion to its size.
				vPos.y = ACT_WORLD->GetHeightWithTerrain(vPos.x, vPos.z);
				m_Chr.PosSet(vPos);
			}
		}
		else
		{
		}

		m_fTimeDying += s_fSecPerFrm; // time to make a dying move
	}

	// Balloon processing...
	if (m_fTimeBalloon > 0)
	{
		m_fTimeBalloon -= s_fSecPerFrm;
		if (m_fTimeBalloon < 0)
		{
			m_fTimeBalloon = 0;
			this->BalloonStringSet("", 0); // Time ‰榮~!!
		}
	}
}

void CPlayerBase::Render(float fSunAngle)
{
	if (m_Chr.m_nLOD < 0 || m_Chr.m_nLOD >= MAX_CHR_LOD) return;

#ifdef _DEBUG
	if (m_pShapeExtraRef) // If it&#39;s an object type...
	{
		m_pShapeExtraRef->RenderCollisionMesh();
		return;
	}
#endif






	float fFactorToApply = 1.0f;
	if (m_fTimeAfterDeath > TIME_CORPSE_REMAIN - TIME_CORPSE_REMOVE) // make it transparent...
		fFactorToApply = (TIME_CORPSE_REMAIN - m_fTimeAfterDeath) / TIME_CORPSE_REMOVE;
	else if (m_fFlickeringFactor != 1.0f)
	{
		m_fFlickeringTime += s_fSecPerFrm;
		if (m_fFlickeringTime < 0.5f)
			fFactorToApply = m_fFlickeringFactor;
		else if (m_fFlickeringTime < 1.0f)
			fFactorToApply = m_fFlickeringFactor * 0.5f;
		else
		{
			m_fFlickeringTime = 0;
			fFactorToApply = m_fFlickeringFactor;
		}
	}


	if (fFactorToApply != 0) // If transparency needs to be applied...
	{
		DWORD dwAlphaBlend, dwAlphaOP, dwAlphaArg1, dwTexFactor, dwSrcBlend, dwDestBlend; // , dwZEnable;

		// backup state
		s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlend);
		s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
		s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
		// s_lpD3DDev-&gt;GetRenderState(D3DRS_ZENABLE, &amp;dwZEnable);
		s_lpD3DDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTexFactor);	// alpha factor setting
		s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOP);
		s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);

		const DWORD dwFactorToApply = ((DWORD)(255.0f * fFactorToApply)) << 24; // Transparency Calculation..

		// Set render state
		s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		// s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
		s_lpD3DDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwFactorToApply);	// Alpha factor setting

		// Setting the texture state (alpha)
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

		m_Chr.Render();

		// restore state
		s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlend);
		s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
		s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
		// s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
		s_lpD3DDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTexFactor);	// alpha factor setting
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOP);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg1);
	}
	else
	{
		m_Chr.Render();
	}

	if (s_Options.iUseShadow) this->RenderShadow(fSunAngle);




















#ifdef _DEBUG
	// if(m_Chr.CollisionMesh()) // box for collision check..
	// {
	// s_lpD3DDev->SetTransform(D3DTS_WORLD, &(m_Chr.m_Matrix));
	// m_Chr.CollisionMesh()->Render(0xffff0000);
	// }

	__Vector3 vLine[3];
	vLine[0] = m_Chr.Pos(); vLine[0].y += 1.3f;
	vLine[1] = m_vPosFromServer; vLine[1].y += 1.3f;
	vLine[2] = vLine[1]; vLine[2].y += 3.0f;
	__Matrix44 mtx; mtx.Identity();
	CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtx);
	CN3Base::RenderLines(vLine, 2, 0xff00ffff);
#endif

	if (m_InfoBase.bRenderID && m_pIDFont)
	{
		const float fDist = (m_Chr.Pos() - s_CameraData.vEye).Magnitude();
		if (fDist < 48.0f)
		{
			__Vector3 vHead = this->HeadPosition();
			vHead.y += this->Height() / 10.0f;
			if (PSA_SITDOWN == m_eState) vHead.y += this->RootPosition().y - this->Height() / 2.0f; // If you sit...
			POINT pt = ::_Convert3D_To_2DCoordinate(vHead, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp.Width, s_CameraData.vp.Height);

			SIZE size = m_pIDFont->GetSize();
			pt.y -= size.cy + 5;
			D3DCOLOR crFont = m_pIDFont->GetFontColor();

			m_pIDFont->DrawText(pt.x - (size.cx / 2) - 1, pt.y - 1, 0xff000000, 0);
			m_pIDFont->DrawText(pt.x - (size.cx / 2) + 1, pt.y + 1, 0xff000000, 0);
			m_pIDFont->DrawText(pt.x - (size.cx / 2), pt.y, crFont, 0);

			// Knight &amp; clan render..
			if (m_pClanFont && m_pClanFont->IsSetText())
			{
				size = m_pClanFont->GetSize();
				pt.y -= size.cy + 5;
				crFont = m_pClanFont->GetFontColor();
				m_pClanFont->DrawText(pt.x - (size.cx / 2) - 1, pt.y - 1, 0xff000000, 0);
				m_pClanFont->DrawText(pt.x - (size.cx / 2) + 1, pt.y + 1, 0xff000000, 0);
				m_pClanFont->DrawText(pt.x - (size.cx / 2), pt.y, crFont, 0);
			}

			// Recruiting party...
			if (m_pInfoFont && m_pInfoFont->IsSetText())// ->GetFontHeight() > 0)
			{
				size = m_pInfoFont->GetSize();
				pt.y -= size.cy + 5;
				crFont = m_pInfoFont->GetFontColor();
				m_pInfoFont->DrawText(pt.x - (size.cx / 2) - 1, pt.y - 1, 0xff000000, 0);
				m_pInfoFont->DrawText(pt.x - (size.cx / 2) + 1, pt.y + 1, 0xff000000, 0);
				m_pInfoFont->DrawText(pt.x - (size.cx / 2), pt.y, crFont, 0);
			}

			// Balloon message...
			if (m_pBalloonFont && m_pBalloonFont->IsSetText())// ->GetFontHeight())
			{
				crFont = m_pBalloonFont->GetFontColor();
				if (m_fTimeBalloon < 2.0f) // slowly fade away...
				{
					DWORD crFont = m_pBalloonFont->GetFontColor();
					crFont = (crFont & 0x00ffffff) | ((DWORD)(255 * (m_fTimeBalloon / 2.0f)) << 24);
					m_pBalloonFont->SetFontColor(crFont);
				}

				size = m_pBalloonFont->GetSize();
				pt.y -= size.cy + 5;
				m_pBalloonFont->DrawText(pt.x - (size.cx / 2) - 1, pt.y - 1, 0xff000000, 0);
				m_pBalloonFont->DrawText(pt.x - (size.cx / 2) + 1, pt.y + 1, 0xff000000, 0);
				m_pBalloonFont->DrawText(pt.x - (size.cx / 2), pt.y, crFont, D3DFONT_BOLD);
			}
		}
	}
}

__Vector3 CPlayerBase::HeadPosition()
{
	__Vector3 vHead = m_Chr.Pos();
	vHead.y += m_Chr.Height();

	return vHead;
}

float CPlayerBase::Height()
{
	if (m_pShapeExtraRef)
	{
		return (m_pShapeExtraRef->Max().y - m_pShapeExtraRef->Min().y) * m_pShapeExtraRef->Scale().y;
	}
	else
	{
		return m_Chr.Height();
	}
}

float CPlayerBase::Radius()
{
	if (m_pShapeExtraRef)
	{
		return m_pShapeExtraRef->Radius();
	}
	else
	{
		return m_Chr.Radius();
	}
}

__Vector3 CPlayerBase::Direction() const
{
	__Vector3 vDir(0, 0, 1);
	const __Matrix44 mtxRot = m_Chr.Rot();
	vDir *= mtxRot;

	return vDir;
}

bool CPlayerBase::Action(e_StateAction eState, bool bLooping, CPlayerBase* pTarget, bool bForceSet)
{
	if (false == bForceSet)
	{
		// State Table Action
		static BOOL sTableAction[PSA_COUNT][PSA_COUNT] =
		{
			// ---------------------------------------------------------------------------------------------------------------------------
			// BASIC,	   ATTACK,		GUARD,	   STRUCK, 		DYING,		DEATH, SPELL_MAGIC,	  SITDOWN
			// ---------------------------------------------------------------------------------------------------------------------------
			{		1,			1,			1,			1,			1,			0,			1,			1 }, // PSA_BASIC		-> ??
			// ---------------------------------------------------------------------------------------------------------------------------
			{		1,			1,			0,			0, 			1,			0,			1,			0 }, // PSA_ATTACK		-> ??
			// ---------------------------------------------------------------------------------------------------------------------------
			{		1,			1,			1,			0, 			1,			0,			1,			0 }, // PSA_GUARD		-> ??
			// ---------------------------------------------------------------------------------------------------------------------------
			{		1,			1,			1,			1,			1,			0,			1,			0 }, // PSA_STRUCK		-> ??
			// ---------------------------------------------------------------------------------------------------------------------------
			{		0,			0,			0,			0,			0,			1,			0,			0 }, // PSA_DYING		-> ??
			// ---------------------------------------------------------------------------------------------------------------------------
			{		0,			0,			0,			0,			0,			0,			0,			0 }, // PSA_DEATH		-> ??
			// ---------------------------------------------------------------------------------------------------------------------------
			{		1,			0,			0,			0,			1,			1,			1,			0 }, // PSA_SPELL_MAGIC	-> ??
			// ---------------------------------------------------------------------------------------------------------------------------
			{		1,			0,			0,			0,			1,			0,			0,			0 } // PSA_SITDOWN -&gt; ??
			// ---------------------------------------------------------------------------------------------------------------------------
		};

		if (FALSE == sTableAction[m_eState][eState])
			return false;
	}

	// if(m_szName == &quot;Aladdin&quot;)
	// {
		// bool bBReak = true;
		// std::string szSt1 = "??";
		// if ( PSA_BASIC == m_eState ) szSt1 = &quot; FREE &quot; ;
		// else if(PSA_SPELLMAGIC == m_eState) szSt1 = "매직쇼~";
		// TRACE("%s(%.1f) - %s\n", m_szName.c_str(), CN3Base::TimeGet(), szSt1.c_str());
	// }

	const bool bNPC = (RACE_NPC == m_InfoBase.eRace ? true : false);
	bool bNeedUpperAnimationOnly = false; // Do you apply animation to the whole body... -1: entire body, 0: lower body 1: upper body
	bool bOnceAndFreeze = false;
	e_Ani eAni = ANI_UNKNOWN;
	e_Ani eAniToRestore = ANI_UNKNOWN;
	float fFreezeTime = 0.0f;
	const e_StateAction eStatePrev = m_eState;
	m_eStateNext = m_eState = eState; // Once the character is maintained...

	switch (eState)
	{
	case PSA_BASIC:
		if (PSM_STOP == m_eStateMove)
		{
			if (PSA_SITDOWN == eStatePrev) // If you are sitting...
			{
				eAni = ANI_STANDUP; // stand up
				if (PSM_STOP == m_eStateMove) eAniToRestore = this->JudgeAnimationBreath(); // If you are stopped, the basic posture.. Judgment.
				bOnceAndFreeze = true;
			}
			else eAni = this->JudgeAnimationBreath(); // If it is stopped, the basic posture.. Judgment.
		}
		else if (PSM_WALK == m_eStateMove) eAni = this->JudgeAnimationWalk(); // walking posture when walking
		else if (PSM_RUN == m_eStateMove) eAni = this->JudgeAnimationRun(); // If you&#39;re running...
		else if (PSM_WALK_BACKWARD == m_eStateMove) eAni = this->JudgeAnimationWalkBackward(); // If you walk backwards...
		else eAni = this->JudgeAnimationBreath(); // etc ??
		break;

	case PSA_ATTACK:
		if (pTarget)
		{
			if (pTarget->m_InfoBase.eNation == m_InfoBase.eNation) return false; // if same country

			if (!bNPC && IsMovingNow())	// If you&#39;re a player... and you&#39;re on the move...
			{
				if (m_pItemPlugBasics[0]) eAni = ANI_ATTACK_WITH_WEAPON_WHEN_MOVE;		// If you hold a weapon in your right hand... set the animation...
				else eAni = ANI_ATTACK_WITH_NAKED_WHEN_MOVE;						// Set the animation...
				bNeedUpperAnimationOnly = true;										// The upper body plays once and then stops.
			}
			else
			{
				eAni = this->JudgeAnimationAttack();			// Set the animation...
			}

			if (!bLooping) // Unless it&#39;s looping...
			{
				m_eStateNext = PSA_BASIC;	// remember the state to return to
			}
		}
		break;

	case PSA_GUARD:
	{
		m_eStateNext = PSA_BASIC;
		eAni = this->JudgeAnimationGuard();

		if (!bNPC) bNeedUpperAnimationOnly = true; // Unless it&#39;s an NPC... upper body
		fFreezeTime = 1.5f; // block a little longer
	}
	break;

	case PSA_STRUCK:
	{
		m_eStateNext = PSA_BASIC;
		eAni = this->JudgeAnimationStruck();
		if (!bNPC) bNeedUpperAnimationOnly = true; // Unless it&#39;s an NPC... upper body
	}
	break;

	case PSA_DYING:
		m_eStateNext = PSA_DEATH; // kill..!!
		m_eStateMove = PSM_STOP;
		eAni = this->JudgeAnimationDying();
		bOnceAndFreeze = true; // Die, stretch on the floor and stop...
		break;

	case PSA_DEATH:
		m_eStateNext = PSA_DEATH;
		if (true == bForceSet) // If you force me to kill you...
		{
			eAni = this->JudgeAnimationDying(); // death action...
			m_Chr.AniFixToLastFrame(eAni); // Freeze to last frame.
		}
		this->RegenerateCollisionMesh();  // Find the maximum and minimum values again and recreate the collision mesh. -&gt; This is to make dead number picking accurate.
		return true; // go back!!!

	case PSA_SITDOWN:
		eAni = ANI_SITDOWN;
		eAniToRestore = ANI_SITDOWN_BREATH;
		bOnceAndFreeze = true;
		break;

	case PSA_SPELLMAGIC:
		m_eStateNext = PSA_BASIC;
		fFreezeTime = m_fCastFreezeTime;
		eAni = this->JudgetAnimationSpellMagic();
		bOnceAndFreeze = true;
		break;

	default:
		return false;
	}

	float fBlendTime = FLT_MIN;
	if (bForceSet) // If forced...
	{
		bNeedUpperAnimationOnly = false; // Unless it&#39;s an NPC... upper body
		fBlendTime = 0;
		m_Chr.m_FrmCtrl.Init();
	}

	// Animation settings...
	this->AnimationClear(); // Clear the contents of the animation queue. Then the animation starts immediately.
	if (ANI_UNKNOWN != eAniToRestore) this->AnimationAdd(eAniToRestore, false); // If you have the next animation, put it in your deck.

	if (bNeedUpperAnimationOnly) // Upper body only..
		m_Chr.AniUpperSet(eAni, fFreezeTime);
	else // Just animation..
	{
		m_Chr.AniCurSet(eAni, bOnceAndFreeze, fBlendTime, fFreezeTime);
	}

	if (bForceSet && m_Chr.m_FrmCtrl.pAniData) // If forced...
	{
		m_Chr.m_FrmCtrl.fFrmCur = m_Chr.m_FrmCtrl.pAniData->fFrmEnd; // Force set to end frame.
	}

	return true;
}

bool CPlayerBase::ActionMove(e_StateMove eMove)
{
	if (this->IsDead()) return false;

	static int sStateTableMove[PSM_COUNT][PSM_COUNT] = // State Table Move
	{
		// ---------------------------------------------------------------------------------------------------------------------------------------
		// STOP,	WALK,	RUN,	WALK_BACKWARD
		// ---------------------------------------------------------------------------------------------------------------------------------------
		{	0,		1,		1,		1 }, // PSM_STOP
		// ---------------------------------------------------------------------------------------------------------------------------------------
		{	1,		0,		1,		1 }, // PSM_WALK
		// ---------------------------------------------------------------------------------------------------------------------------------------
		{	1,		1,		0,		1 }, // PSM_RUN
		// ---------------------------------------------------------------------------------------------------------------------------------------
		{	1,		1,		1,		0 } // PSM_WALK_BACKWARD
		// ---------------------------------------------------------------------------------------------------------------------------------------
	};

	if (FALSE == sStateTableMove[m_eStateMove][eMove]) return false;

	m_eStateNext = m_eState; // Force the return state to match the current state.
	m_eStateMove = eMove;

	e_Ani eAni = ANI_UNKNOWN;
	switch (eMove)
	{
	case PSM_STOP:
		eAni = this->JudgeAnimationBreath();
		break;

	case PSM_WALK:
		eAni = this->JudgeAnimationWalk();
		break;

	case PSM_RUN:
		eAni = this->JudgeAnimationRun();
		break;

	case PSM_WALK_BACKWARD:
		eAni = this->JudgeAnimationWalkBackward();
		break;

	default:
		return false;
	}

	this->Action(PSA_BASIC, true); // I bet you don&#39;t do anything else..

	// Animation settings...
	this->AnimationClear(); // Clear the contents of the animation queue. Then the animation starts immediately.
	m_Chr.AniCurSet(eAni, false, FLT_MIN, 0, false); // Walk or run without stopping upper body animation...

	return true;
}

void CPlayerBase::ActionDying(e_StateDying eSD, const __Vector3& vDir)
{
	this->ActionMove(PSM_STOP);
	this->Action(PSA_DYING, false); // kill...
	m_eStateDying = eSD;
	m_vDirDying = vDir; // The direction you move when you die.

	e_Ani eAni = ANI_DEAD_NEATLY;
	if (eSD == PSD_KEEP_POSITION)
	{
		if (RACE_NPC == m_InfoBase.eRace) eAni = ANI_NPC_DEAD0; // In case of NPC
		else eAni = ANI_DEAD_NEATLY; // As for the player...
	}
	else if (eSD == PSD_DISJOINT)
	{
		if (RACE_NPC == m_InfoBase.eRace) eAni = ANI_NPC_DEAD1; // In case of NPC
		else eAni = ANI_DEAD_ROLL; // In the case of a player.. the body turns around and dies..
	}
	else if (eSD == PSD_KNOCK_DOWN) // get pushed back and die.
	{
		if (RACE_NPC == m_InfoBase.eRace) eAni = ANI_NPC_DEAD1; // In case of NPC
		else eAni = ANI_DEAD_KNOCKDOWN; // As for the player...
	}
	else
	{
		if (RACE_NPC == m_InfoBase.eRace) eAni = ANI_NPC_DEAD0; // In case of NPC
		else eAni = ANI_DEAD_NEATLY; // As for the player...
	}

	m_Chr.AniCurSet(eAni, true, FLT_MIN, 0, true);
}

bool CPlayerBase::ProcessAttack(CPlayerBase* pTarget)
{
	if (PSA_ATTACK != m_eState && m_iSkillStep == 0) return false; // If it&#39;s not in an attack state and it&#39;s not in the middle of using a skill... it will go back.
	if (nullptr == pTarget)
		return false;

	bool bAttackSuccess = false;
	bool bStrike = false;
	if (m_Chr.NeedStrike0() || m_Chr.NeedStrike1()) bStrike = true; // The timing to start...

	if (bStrike) // When the timing is right...
	{
		__Vector3 vCol(0, 0, 0);
		CPlayerBase* pTarget = TargetPointerCheck(false);
		if (nullptr == pTarget) return false;

		if (false == this->CheckCollisionToTargetByPlug(pTarget, 0, &vCol)) // Check for collisions with plugs first.
		{
			if (false == this->CheckCollisionToTargetByPlug(pTarget, 1, &vCol)) // Check for collisions with plugs first.
			{
				const __Matrix44* pMtxMine = m_Chr.MatrixGet(0); // Target character with joint..
				const __Matrix44* pMtxTarget = pTarget->m_Chr.MatrixGet(0);

				__Vector3 v0, v1;
				if (pMtxMine)
				{
					CN3CPlug* pPlug = m_Chr.Plug(0); // The point of the weapon and...
					if (pPlug)
					{
						__Matrix44 mtx = *(m_Chr.MatrixGet(pPlug->m_nJointIndex));
						v0.Set(0.0f, pPlug->m_fTrace1, 0.0f);
						v0 *= pPlug->m_Matrix;
						v0 *= mtx;
						v0 *= m_Chr.m_Matrix;
					}
					else v0 = m_Chr.Pos() + pMtxMine->Pos();
				}
				else
				{
					v0 = m_Chr.Pos(); v0.y += m_Chr.Height() / 2.0f;
				}

				if (pMtxTarget)
				{
					v1 = pMtxTarget->Pos() + pTarget->Position();
					if (false == pTarget->CheckCollisionByBox(v0, v1, &vCol, nullptr)) // Check for collision with one straight line.
					{
						__Vector3 vDir = v0 - v1; vDir.Normalize();
						vCol = v1 + vDir * (pTarget->Height() / 3.0f); // If not, use the value between characters.
					}
				}
				else if (pTarget->m_pShapeExtraRef && pTarget->m_pShapeExtraRef->CollisionMesh())
				{
					__Vector3 vDir = this->Direction(); vDir.Normalize();
					v1 = v0 + (vDir * 256.0f);

					CN3VMesh* pVMesh = pTarget->m_pShapeExtraRef->CollisionMesh();
					if (nullptr == pVMesh || false == pVMesh->CheckCollision(pTarget->m_pShapeExtraRef->m_Matrix, v0, v1, &vCol)) // If it doesn&#39;t collide with the collision mesh...
					{
						v1 = pTarget->m_pShapeExtraRef->Pos(); v1.y += pTarget->Height() / 2.0f;
						vDir = v1 - v0; vDir.Normalize();
						vCol = v0 + vDir * Radius(); // If not, use the value between characters.
					}
				}
				else
				{
					return false;
				}
			}
		}

		if (pTarget->m_fTimeAfterDeath > 0 && false == pTarget->IsDead())
		{
			e_ItemClass eICR = this->ItemClass_RightHand(); // Depending on the weapon in your right hand...

			e_StateDying eSD = PSD_KEEP_POSITION;
			if (ITEM_CLASS_SWORD_2H == eICR || ITEM_CLASS_AXE_2H == eICR || ITEM_CLASS_MACE_2H == eICR || ITEM_CLASS_POLEARM == eICR) eSD = PSD_DISJOINT; // If it is a two-handed weapon, it will be cut to death.
			else if (ITEM_CLASS_SWORD == eICR || ITEM_CLASS_AXE == eICR || ITEM_CLASS_MACE == eICR || ITEM_CLASS_SPEAR == eICR) eSD = PSD_KNOCK_DOWN; // One-handed normal weapon

			__Vector3 vTarget = pTarget->Position();
			if (pTarget->m_pSnd_Blow) pTarget->m_pSnd_Blow->Play(&vTarget); // The sound of a gun hitting...

			// Depending on the attribute of the weapon, different effects...
			bool bAffected = false;
			if (m_pItemPlugExts[PLUG_POS_RIGHTHAND])
			{
				int iFXID = -1;
				if ((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageFire > 0)
					|| (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageFire >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_FIRE_TARGET;			// fire
				else if ((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageIce > 0)
					|| (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageIce >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_ICE_TARGET;			// chill
				else if ((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamagePoison > 0)
					|| (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamagePoison >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_POISON_TARGET;		// poison
				else if ((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageThuner > 0)
					|| (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageThuner >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_LIGHTNING_TARGET;		// electric shock

				if (iFXID >= 0)
				{
					bAffected = true;
					CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, iFXID, vCol);	// Blitz weapon...
				}
			}
			if (m_pItemPlugExts[PLUG_POS_LEFTHAND] && !bAffected)
			{
				int iFXID = -1;
				if ((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageFire > 0)
					|| (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageFire >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_FIRE_TARGET;			// fire
				else if ((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageIce > 0)
					|| (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageIce >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_ICE_TARGET;			// chill
				else if ((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamagePoison > 0)
					|| (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamagePoison >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_POISON_TARGET;		// poison
				else if ((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageThuner > 0)
					|| (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageThuner >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_LIGHTNING_TARGET;		// electric shock

				if (iFXID >= 0)
				{
					bAffected = true;
					CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, iFXID, vCol);	// Attributed weapon...
				}
			}
			if (!bAffected) CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_BLOOD, vCol);	// normal weapon...

			__Vector3 vDirDeath = this->Position() - vTarget;
			vDirDeath.Normalize();
			pTarget->ActionDying(eSD, vDirDeath); // die in place
		}
		else if (pTarget->IsAlive())
		{
			if (false == pTarget->m_bGuardSuccess) // Flag if the defense was successful.. if the defense failed..
			{
				__Vector3 vTarget = pTarget->Position();

				if (pTarget->m_pSnd_Blow) pTarget->m_pSnd_Blow->Play(&vTarget); // Temporary sounds...

				// Depending on the attribute of the weapon, different effects...
				bool bAffected = false;
				if (m_pItemPlugExts[PLUG_POS_RIGHTHAND])
				{
					if ((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageFire > 0)
						|| (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageFire >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_FIRE_TARGET, vCol);	// fire weapon...
					}
					else if ((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageIce > 0)
						|| (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageIce >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_ICE_TARGET, vCol);	// cold weapon...
					}
					else if ((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamagePoison > 0)
						|| (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamagePoison >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_POISON_TARGET, vCol);	// poison weapon...
					}
					else if ((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageThuner > 0)
						|| (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageThuner >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_LIGHTNING_TARGET, vCol);	// Blitz weapon...
					}
				}
				if (m_pItemPlugExts[PLUG_POS_LEFTHAND] && !bAffected)
				{
					if ((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageFire > 0)
						|| (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageFire >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_FIRE_TARGET, vCol);	// fire weapon...
					}
					else if ((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageIce > 0)
						|| (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageIce >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_ICE_TARGET, vCol);	// cold weapon...
					}
					else if ((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamagePoison > 0)
						|| (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamagePoison >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_POISON_TARGET, vCol);	// poison weapon...
					}
					else if ((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare == ITEM_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageThuner > 0)
						|| (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageThuner >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_LIGHTNING_TARGET, vCol);	// Blitz weapon...
					}
				}
				if (!bAffected) CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_BLOOD, vCol);	// normal weapon...

				D3DCOLORVALUE crHit = { 1.0f, 0.2f, 0.2f, 1.0f };
				pTarget->DurationColorSet(crHit, 0.3f); // For 0.3 seconds of red and dry color...

				// int iRand = rand()%2; // Get hit and moan...
				// if(iRand == 0) { if(pTarget->m_pSnd_Struck_0) pTarget->m_pSnd_Struck_0->Play(&vTarget); }
				// else if(iRand == 1) { if(pTarget->m_pSnd_Struck_1) pTarget->m_pSnd_Struck_1->Play(&vTarget); }
				if (pTarget->m_pSnd_Struck_0) pTarget->m_pSnd_Struck_0->Play(&vTarget);

				if (0 == pTarget->m_iSkillStep) // The skill is not being used..
					pTarget->Action(PSA_STRUCK, false); // If it&#39;s not dead, I&#39;ll get beaten and hit.
			}
			else // defense success.
			{
				// if (0 == pTarget-&gt;m_iSkillStep) // The skill is not in use..
				// pTarget->Action(PSA_GUARD, false);
			}
		}
	}

	return bAttackSuccess;
}

e_Ani CPlayerBase::JudgeAnimationAttack()
{
	e_Ani eAni = ANI_BREATH;

	if (RACE_NPC == m_InfoBase.eRace) // In case of NPC
	{
		eAni = (e_Ani)(ANI_NPC_ATTACK0 + rand() % 2);
	}
	else // As for the player...
	{
		if (-1 != m_iIDTarget) // If you have a target...
		{
			const e_ItemClass eICR = this->ItemClass_RightHand(); // Depending on your right hand weapon...
			if (ITEM_CLASS_STAFF == eICR) // In the case of a staff, it is a spear attack. ???
			{
				eAni = ANI_DAGGER_ATTACK_A0;
			}
			else
			{
				eAni = (e_Ani)(this->JudgeAnimationBreath() + 1 + rand() % 2);
			}
		}
	}

	return eAni;
}

e_Ani CPlayerBase::JudgeAnimationBreath()
{
	e_Ani eAni = ANI_BREATH;

	if (RACE_NPC == m_InfoBase.eRace) // In case of NPC
	{
		if (rand() % 10 != 0) eAni = ANI_NPC_BREATH;
		else eAni = (e_Ani)(ANI_NPC_TALK0 + rand() % 4);
	}
	else // As for the player...
	{
		const CPlayerBase* pTarget = TargetPointerCheck(false);

		if (pTarget && pTarget->m_InfoBase.eNation != m_InfoBase.eNation) // If there is a target and the country is different...
		{
			const e_ItemClass eICR = this->ItemClass_RightHand();
			const e_ItemClass eICL = this->ItemClass_LeftHand();

			float fIWR = 0; // , fIWL = 0; // Item Weight RightHand, LeftHand
			if (m_pItemPlugBasics[PLUG_POS_RIGHTHAND]) fIWR = m_pItemPlugBasics[PLUG_POS_RIGHTHAND]->siWeight / 10.f;
			// if(m_pItemPlugBasics[PLUG_POS_LEFTHAND]) fIWL = m_pItemPlugBasics[PLUG_POS_LEFTHAND]->siWeight / 10.f;

			if ((ITEM_CLASS_SWORD == eICR && ITEM_CLASS_SWORD == eICL) ||
				(ITEM_CLASS_AXE == eICR && ITEM_CLASS_AXE == eICL) ||
				(ITEM_CLASS_SWORD == eICR && ITEM_CLASS_AXE == eICL) ||
				(ITEM_CLASS_AXE == eICR && ITEM_CLASS_SWORD == eICL))
			{
				if (ITEM_CLASS_SWORD == eICR) // Judging weapon weight based on right hand..
				{
					if (fIWR < WEAPON_WEIGHT_STAND_SWORD) eAni = ANI_DUAL_BREATH_A;
					else eAni = ANI_DUAL_BREATH_B;
				}
				else // if(ITEM_CLASS_AXE == eICR) // Determine weapon weight based on right hand..
				{
					if (fIWR < WEAPON_WEIGHT_STAND_AXE) eAni = ANI_DUAL_BREATH_A;
					else eAni = ANI_DUAL_BREATH_B;
				}
			}
			else if (ITEM_CLASS_DAGGER == eICR) // dagger
				eAni = ANI_DAGGER_BREATH_A;
			else if (ITEM_CLASS_SWORD == eICR) // just a regular knife
			{
				if (fIWR < WEAPON_WEIGHT_STAND_SWORD) eAni = ANI_SWORD_BREATH_A;
				else eAni = ANI_SWORD_BREATH_B;
			}
			else if (ITEM_CLASS_SWORD_2H == eICR) // amniotic fluid
				eAni = ANI_SWORD2H_BREATH_A;
			else if (ITEM_CLASS_AXE == eICR)
			{
				if (fIWR < WEAPON_WEIGHT_STAND_AXE) eAni = ANI_AXE_BREATH_A;
				else eAni = ANI_AXE_BREATH_B;
			}
			else if (ITEM_CLASS_AXE_2H == eICR || ITEM_CLASS_MACE_2H == eICR) // A two-handed ax or blunt weapon.
				eAni = ANI_BLUNT2H_BREATH_A;
			else if (ITEM_CLASS_MACE == eICR)
			{
				if (fIWR < WEAPON_WEIGHT_STAND_BLUNT) eAni = ANI_BLUNT_BREATH_A;
				else eAni = ANI_BLUNT_BREATH_B;
			}
			else if (ITEM_CLASS_SPEAR == eICR) // window
				eAni = ANI_SPEAR_BREATH_A;
			else if (ITEM_CLASS_POLEARM == eICR)
				eAni = ANI_POLEARM_BREATH_A;
			else if (eICR == ITEM_CLASS_UNKNOWN && eICL == ITEM_CLASS_BOW) // Left-handed bow...
				eAni = ANI_BOW_BREATH;
			else if (eICR == ITEM_CLASS_BOW_CROSS && eICL == ITEM_CLASS_UNKNOWN) // Right-handed crossbow...
				eAni = ANI_CROSS_BOW_BREATH;
			else if (eICR == ITEM_CLASS_LAUNCHER && eICL >= ITEM_CLASS_UNKNOWN) // Right-handed window launcher..
				eAni = ANI_LAUNCHER_BREATH;
			else if (eICR == ITEM_CLASS_UNKNOWN && eICL >= ITEM_CLASS_SHIELD) // Left-handed shield...
			{
				eAni = ANI_SHIELD_BREATH_A;
			}
			else if (eICR == ITEM_CLASS_STAFF) // cane..
			{
				eAni = ANI_BREATH; // I just breathe..
			}
			else // no weapons...
			{
				eAni = ANI_NAKED_BREATH_A;
			}
		}
		else
		{
			eAni = ANI_BREATH;
		}
	}

	return eAni;
}

e_Ani CPlayerBase::JudgeAnimationWalk() // Determining the walking mode.. Returns different animation indexes depending on the item you have and whether there is a target.
{
	e_Ani eAni = ANI_WALK;

	if (RACE_NPC == m_InfoBase.eRace) // In case of NPC
	{
		eAni = ANI_NPC_WALK;
	}
	else // As for the player...
	{
		eAni = ANI_WALK; // Different animations are judged according to the weapon you have in this position.
	}

	return eAni;
}

e_Ani CPlayerBase::JudgeAnimationRun() // Determining the walking mode.. Returns different animation indexes depending on the item you have and whether there is a target.
{
	e_Ani eAni = ANI_RUN;

	if (RACE_NPC == m_InfoBase.eRace) // In case of NPC
	{
		eAni = ANI_NPC_RUN;
	}
	else // As for the player...
	{
		eAni = ANI_RUN; // Different animations are judged according to the weapon you have in this position.
	}

	return eAni;
}

e_Ani CPlayerBase::JudgeAnimationWalkBackward() // Determining the walking mode.. Returns different animation indexes depending on the item you have and whether there is a target.
{
	e_Ani eAni = ANI_WALK_BACKWARD;

	if (RACE_NPC == m_InfoBase.eRace) // In case of NPC
	{
		eAni = ANI_NPC_WALK_BACKWARD;
	}
	else // As for the player...
	{
		eAni = ANI_WALK_BACKWARD; // Different animations are judged according to the weapon you have in this position.
	}

	return eAni;
}

e_Ani CPlayerBase::JudgeAnimationStruck()
{
	if (RACE_NPC == m_InfoBase.eRace) return (e_Ani)(ANI_NPC_STRUCK0 + rand() % 3); // In case of NPC
	else return (e_Ani)(ANI_STRUCK0 + rand() % 3); // As for the player...
}

e_Ani CPlayerBase::JudgeAnimationGuard()
{
	if (RACE_NPC == m_InfoBase.eRace) return ANI_NPC_GUARD; // In case of NPC
	else return ANI_GUARD; // As for the player...
}

e_Ani CPlayerBase::JudgeAnimationDying()
{
	if (RACE_NPC == m_InfoBase.eRace) return (e_Ani)(ANI_NPC_DEAD0); // In case of NPC
	else return (e_Ani)(ANI_DEAD_NEATLY + rand() % 3); // As for the player...
}

e_Ani CPlayerBase::JudgetAnimationSpellMagic()
{
	if (RACE_NPC == m_InfoBase.eRace) return (e_Ani)(m_iMagicAni); // In case of NPC
	else return (e_Ani)(m_iMagicAni); // As for the player...
}

bool CPlayerBase::CheckCollisionByBox(const __Vector3& v0, const __Vector3& v1, __Vector3* pVCol, __Vector3* pVNormal)
{
	CN3VMesh* pvMesh = m_Chr.CollisionMesh();
	if (nullptr == pvMesh) return false;
	return pvMesh->CheckCollision(m_Chr.m_Matrix, v0, v1, pVCol, pVNormal);
}

bool CPlayerBase::CheckCollisionToTargetByPlug(CPlayerBase* pTarget, int nPlug, __Vector3* pVCol)
{
	if (nullptr == pTarget) return false;
	const CN3CPlug* pPlug = m_Chr.Plug(nPlug);
	if (nullptr == pPlug) return false; // Doesn&#39;t work if you don&#39;t have a weapon equipped.
	// berserk
	// if(pPlug->m_ePlugType == PLUGTYPE_CLOAK)	return false;
	// CN3CPlug *pPlugNormal = (CN3CPlug*)pPlug;
	// if(pPlugNormal-&gt;m_fTrace0 &gt;= pPlugNormal-&gt;m_fTrace1) return false; // If there is no information on the length of the weapon or it is abnormal, collision check is not performed.
	if (pPlug->m_fTrace0 >= pPlug->m_fTrace1) return false; // If there is no information on the length of the weapon or it is abnormal, the collision check is not performed.
	if (PSA_DYING == pTarget->State() || PSA_DEATH == pTarget->State()) return false; // Do not check for collisions with dead or fallen bodies.

	// 
	// Collision check is not performed unless the sword trajectory remains.
	// __AnimData* pAni = m_Chr.AniCur(0);
	// if(NULL == pAni) return false;
	// float fFrmCur = m_Chr.FrmCur(0);
	// if(fFrmCur < pAni->fFrmPlugTraceStart || fFrmCur > pAni->fFrmPlugTraceEnd) return false;
	// Collision check is not performed unless the sword trajectory remains.
	// 

	__Vector3 v1, v2, v3;
	const __Matrix44 mtx = *(m_Chr.MatrixGet(pPlug->m_nJointIndex));

	v1.Set(0.0f, pPlug->m_fTrace0, 0.0f);
	v2.Set(0.0f, pPlug->m_fTrace1, 0.0f);

	v1 *= pPlug->m_Matrix;
	v1 *= mtx;
	v1 *= m_Chr.m_Matrix;

	v2 *= pPlug->m_Matrix;
	v2 *= mtx;
	v2 *= m_Chr.m_Matrix;

	v2 += (v2 - v1) * 1.0f; // double the length

#ifdef _DEBUG
	/*	CN3Base::s_lpD3DDev->BeginScene();

		__Vector3 vLines[2] = { v1, v2 };
		__Matrix44 mtxTmp; mtxTmp.Identity();
		CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtxTmp);
		CN3Base::RenderLines(vLines, 1, (D3DCOLOR)0xffff8080); // Draw a line...

		if(pChrDestination && pChrDestination->CollisionMesh())
		{
			CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &(pChrDestination->m_Matrix));
			pChrDestination-&gt;CollisionMesh()-&gt;Render((D3DCOLOR)0xffff0000); // Draw the collision box.
		}
		CN3Base::s_lpD3DDev->EndScene();
		CN3Base::s_lpD3DDev->Present(NULL, NULL, CN3Base::s_hWndBase, NULL);
	*/
#endif

	if (m_pShapeExtraRef && m_pShapeExtraRef->CollisionMesh())
	{
		CN3VMesh* pVMesh = m_pShapeExtraRef->CollisionMesh();
		if (pVMesh)
		{
			return pVMesh->CheckCollision(m_pShapeExtraRef->m_Matrix, v1, v2, pVCol);
		}
	}

	return pTarget->CheckCollisionByBox(v1, v2, pVCol, nullptr);			// Character Collision Check Box and Collision Check..
}

CN3CPlugBase* CPlayerBase::PlugSet(e_PlugPosition ePos, const std::string& szFN, __TABLE_ITEM_BASIC* pItemBasic, __TABLE_ITEM_EXT* pItemExt)
{
	if (ePos < PLUG_POS_RIGHTHAND || ePos >= PLUG_POS_COUNT)
	{
		__ASSERT(0, "Invalid Plug Position");
		return nullptr;
	}

	int iJoint = 0;
	if (PLUG_POS_RIGHTHAND == ePos)
	{
		m_pItemPlugBasics[ePos] = pItemBasic;
		m_pItemPlugExts[ePos] = pItemExt;
		iJoint = m_pLooksRef->iJointRH;
	}
	else if (PLUG_POS_LEFTHAND == ePos)
	{
		m_pItemPlugBasics[ePos] = pItemBasic;
		m_pItemPlugExts[ePos] = pItemExt;
		if (pItemBasic)
		{
			if (pItemBasic->byClass == ITEM_CLASS_SHIELD) iJoint = m_pLooksRef->iJointLH2; // if it&#39;s a shield
			else iJoint = m_pLooksRef->iJointLH; // left end...
		}
	}
	else if (PLUG_POS_KNIGHTS_GRADE == ePos)
	{
		m_pItemPlugBasics[ePos] = pItemBasic;
		m_pItemPlugExts[ePos] = pItemExt;
	}
	else if (PLUG_POS_BACK == ePos)
	{
		// m_pItemPlugBasics[PLUG_POS_BACK] = pItem;
	}
	else { __ASSERT(0, "Invalid Plug Item position"); }


	CN3CPlugBase* pPlug = m_Chr.PlugSet(ePos, szFN);
	if (nullptr == pPlug) return nullptr;

	if (PLUG_POS_LEFTHAND == ePos || PLUG_POS_RIGHTHAND == ePos)
	{
		float fScale = m_Chr.Height() / 1.8f;  // Grow in proportion to your height. The default height is 1.8 meters.
		fScale *= pPlug->Scale().y / m_Chr.Scale().y;
		pPlug->ScaleSet(__Vector3(fScale, fScale, fScale));
		pPlug->m_nJointIndex = iJoint; // Determining where to stick...
	}
	// else if(PLUG_POS_BACK == ePos)
	// {
	// CN3CPlug_Cloak *pPlugCloak = (CN3CPlug_Cloak*)pPlug;
	// pPlugCloak->GetCloak()->SetPlayerBase(this);
	// }

	if (pPlug && nullptr == pItemBasic && nullptr == pItemExt) pPlug->TexOverlapSet(""); // If it&#39;s basic...

	// 
	//
	// Add a plug effect..^^
	if (pItemExt)
	{
		if ((pItemExt->byMagicOrRare == ITEM_UNIQUE && pItemExt->byDamageFire > 0) || (pItemExt->byDamageFire >= LIMIT_FX_DAMAGE)) // 17 Additional Damage - Fire
		{
			auto* pCPlug = (CN3CPlug*)pPlug;
			__TABLE_FX* pFXMain = s_pTbl_FXSource->Find(FXID_SWORD_FIRE_MAIN);
			__TABLE_FX* pFXTail = s_pTbl_FXSource->Find(FXID_SWORD_FIRE_TAIL);

			std::string szFXMain, szFXTail;
			if (pFXMain) szFXMain = pFXMain->szFN;
			else szFXMain = "";
			if (pFXTail) szFXTail = pFXTail->szFN;
			else szFXTail = "";
			pCPlug->InitFX(szFXMain, szFXTail, 0xffffff00);
		}
		else if ((pItemExt->byMagicOrRare == ITEM_UNIQUE && pItemExt->byDamageIce > 0) || (pItemExt->byDamageIce >= LIMIT_FX_DAMAGE))// 18 Extra Damage - Ice
		{
			auto* pCPlug = (CN3CPlug*)pPlug;
			__TABLE_FX* pFXMain = s_pTbl_FXSource->Find(FXID_SWORD_ICE_MAIN);
			__TABLE_FX* pFXTail = s_pTbl_FXSource->Find(FXID_SWORD_ICE_TAIL);

			std::string szFXMain, szFXTail;
			if (pFXMain) szFXMain = pFXMain->szFN;
			else szFXMain = "";
			if (pFXTail) szFXTail = pFXTail->szFN;
			else szFXTail = "";

			pCPlug->InitFX(szFXMain, szFXTail, 0xff0000ff);
		}
		else if ((pItemExt->byMagicOrRare == ITEM_UNIQUE && pItemExt->byDamageThuner > 0) || (pItemExt->byDamageThuner >= LIMIT_FX_DAMAGE))// 19 Additional Damage - Lightning
		{
			auto* pCPlug = (CN3CPlug*)pPlug;
			__TABLE_FX* pFXMain = s_pTbl_FXSource->Find(FXID_SWORD_LIGHTNING_MAIN);
			__TABLE_FX* pFXTail = s_pTbl_FXSource->Find(FXID_SWORD_LIGHTNING_TAIL);

			std::string szFXMain, szFXTail;
			if (pFXMain) szFXMain = pFXMain->szFN;
			else szFXMain = "";
			if (pFXTail) szFXTail = pFXTail->szFN;
			else szFXTail = "";

			pCPlug->InitFX(szFXMain, szFXTail, 0xffffffff);
		}
		else if ((pItemExt->byMagicOrRare == ITEM_UNIQUE && pItemExt->byDamagePoison > 0) || (pItemExt->byDamagePoison >= LIMIT_FX_DAMAGE))// 20 Extra Damage - Poison
		{
			auto* pCPlug = (CN3CPlug*)pPlug;
			__TABLE_FX* pFXMain = s_pTbl_FXSource->Find(FXID_SWORD_POISON_MAIN);
			__TABLE_FX* pFXTail = s_pTbl_FXSource->Find(FXID_SWORD_POISON_TAIL);

			std::string szFXMain, szFXTail;
			if (pFXMain) szFXMain = pFXMain->szFN;
			else szFXMain = "";
			if (pFXTail) szFXTail = pFXTail->szFN;
			else szFXTail = "";

			pCPlug->InitFX(szFXMain, szFXTail, 0xffff00ff);
		}
	}
	//
	// 

	return pPlug;
}

CN3CPart* CPlayerBase::PartSet(e_PartPosition ePos, const std::string& szFN, __TABLE_ITEM_BASIC* pItemBasic, __TABLE_ITEM_EXT* pItemExt)
{
	if (ePos < PART_POS_UPPER || ePos >= PART_POS_COUNT)
	{
		__ASSERT(0, "Invalid Item Position");
		return nullptr;
	}

	if (PART_POS_UPPER == ePos) // The upper body requires special treatment.
	{
		if (pItemBasic) // if coated
		{
			if (pItemBasic->byIsRobeType && m_Chr.Part(PART_POS_LOWER)) // It&#39;s a robe-type full top, and if you&#39;re wearing something underneath...
			{
				this->PartSet(PART_POS_LOWER, "", m_pItemPartBasics[PART_POS_LOWER], m_pItemPartExts[PART_POS_LOWER]); // empty the bottom.
			}
		}
		else // If you take off your upper body
		{
			if (m_pItemPartBasics[ePos] && m_pItemPartBasics[ePos]->byIsRobeType) // If you&#39;ve worn a robe before...
			{
				if (m_pItemPartBasics[PART_POS_LOWER]) // If an item is worn on the lower body...
				{
					std::string szFN2;
					e_PartPosition ePartPos2 = PART_POS_UNKNOWN;
					e_PlugPosition ePlugPos2 = PLUG_POS_UNKNOWN;

					CGameProcedure::MakeResrcFileNameForUPC(m_pItemPartBasics[PART_POS_LOWER], &szFN2, nullptr, ePartPos2, ePlugPos2);
					this->PartSet(PART_POS_LOWER, szFN2, m_pItemPartBasics[PART_POS_LOWER], m_pItemPartExts[PART_POS_LOWER]); // Put the previous clothes on the lower body.
				}
				else // If there are no items worn on the lower body...
				{
					const __TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks->Find(m_InfoBase.eRace);	// User Player Character Skin structure pointer..
					this->PartSet(PART_POS_LOWER, pLooks->szPartFNs[PART_POS_LOWER], nullptr, nullptr); // Wear basic clothes on the lower body.
				}
			}
		}
	}
	else if (PART_POS_LOWER == ePos) // If the lower body...
	{
		if (pItemBasic) // When to wear
		{
			if (m_pItemPartBasics[PART_POS_UPPER] && m_pItemPartBasics[PART_POS_UPPER]->byIsRobeType) // If the robe was worn on the upper body before...
			{
				m_pItemPartBasics[ePos] = pItemBasic;
				m_pItemPartExts[ePos] = pItemExt;
				return m_Chr.PartSet(ePos, ""); // The lower body is stripped (?) and returned.
			}
		}
	}

	CN3CPart* pPart = nullptr;
	if (szFN.empty()) // If there is no file name.. Default wear..
	{
		if (PART_POS_HAIR_HELMET == ePos)
		{
			this->InitHair();
			pPart = m_Chr.Part(ePos);
		}
		else if (PART_POS_FACE == ePos)
		{
			this->InitFace();
			pPart = m_Chr.Part(ePos);
		}
		else
		{
			const __TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks->Find(m_InfoBase.eRace);	// Player Character Skin structure pointer..
			if (pLooks)
			{
				pPart = m_Chr.PartSet(ePos, pLooks->szPartFNs[ePos]);
				if (pPart) pPart->TexOverlapSet("");
			}
		}
	}
	else
	{
		pPart = m_Chr.PartSet(ePos, szFN);
	}

	m_pItemPartBasics[ePos] = pItemBasic; // apply item
	m_pItemPartExts[ePos] = pItemExt;

	if (pPart && nullptr == pItemBasic && nullptr == pItemExt) pPart->TexOverlapSet(""); // If it&#39;s basic...

	return pPart;
}

void CPlayerBase::DurabilitySet(e_ItemSlot eSlot, int iDurability)
{
	e_PartPosition ePartPos = PART_POS_UNKNOWN;

	if (ITEM_SLOT_HAND_RIGHT == eSlot || ITEM_SLOT_HAND_LEFT == eSlot)
	{
		e_PlugPosition ePos = PLUG_POS_LEFTHAND;
		if (ITEM_SLOT_HAND_RIGHT == eSlot) ePos = PLUG_POS_RIGHTHAND;
		if (nullptr == m_pItemPlugBasics[ePos] || nullptr == m_pItemPlugExts[ePos])
		{
			if (0 != iDurability)
			{
				__ASSERT(0, "NULL Item");
			}
			return;
		}

		const int iDuMax = m_pItemPlugBasics[ePos]->siMaxDurability + m_pItemPlugExts[ePos]->siMaxDurability;
		if (iDuMax <= 0)
		{
			__ASSERT(0, "최대 내구력 없음");
			return;
		}

		CN3CPlug* pPlug = m_Chr.Plug(ePos);
		if (nullptr == pPlug)
		{
			__ASSERT(0, "NULL Plug");
			return;
		}

		const int iPercentage = iDurability * 100 / iDuMax;
		std::string szFN;
		if (iPercentage <= 30) szFN = "Misc\\Dust_Hard.dxt";
		else if (iPercentage <= 70) szFN = "Misc\\Dust_Soft.dxt";

		pPlug->TexOverlapSet(szFN);
	}
	else if (ITEM_SLOT_UPPER == eSlot) ePartPos = PART_POS_UPPER;
	else if (ITEM_SLOT_LOWER == eSlot) ePartPos = PART_POS_LOWER;
	else if (ITEM_SLOT_GLOVES == eSlot) ePartPos = PART_POS_HANDS;
	else if (ITEM_SLOT_SHOES == eSlot) ePartPos = PART_POS_FEET;
	else if (ITEM_SLOT_HEAD == eSlot) ePartPos = PART_POS_HAIR_HELMET;

	if (PART_POS_UNKNOWN != ePartPos)
	{
		CN3CPart* pPart = m_Chr.Part(ePartPos);
		if (pPart)
		{
			if (m_pItemPartBasics[ePartPos] && m_pItemPartExts[ePartPos])
			{
				const int iDuMax = m_pItemPartBasics[ePartPos]->siMaxDurability + m_pItemPartExts[ePartPos]->siMaxDurability; // Basic durability + extended durability
				const int iPercentage = iDurability * 100 / iDuMax;

				std::string szFN;
				if (iPercentage <= 30) szFN = "Misc\\Dust_Hard.dxt";
				else if (iPercentage <= 70) szFN = "Misc\\Dust_Soft.dxt";
				pPart->TexOverlapSet(szFN);
			}
			else
			{
				pPart->TexOverlapSet("");
			}
		}
		else
		{
			__ASSERT(0, "Invalid Item Position");
		}
	}
}

bool CPlayerBase::InitChr(__TABLE_PLAYER_LOOKS* pTbl)
{
	if (nullptr == pTbl) return false;

	m_pLooksRef = pTbl;

	m_Chr.JointSet(pTbl->szJointFN);
	m_Chr.AniCtrlSet(pTbl->szAniFN);

	if (RACE_NPC != m_InfoBase.eRace) // Upper and lower body ready to play separately..
	{
		m_Chr.JointPartSet(0, 16, 23); // lower body
		m_Chr.JointPartSet(1, 1, 15); // upper body
	}

	return true;
}


void CPlayerBase::RegenerateCollisionMesh() // Find the maximum and minimum values again and recreate the collision mesh.
{
	m_Chr.FindMinMax();
	__Matrix44 mtxInverse;
	D3DXMatrixInverse(&mtxInverse, nullptr, &(m_Chr.m_Matrix));
	if (m_Chr.CollisionMesh()) m_Chr.CollisionMesh()->CreateCube(m_Chr.Min() * mtxInverse, m_Chr.Max() * mtxInverse);
}

CPlayerBase* CPlayerBase::TargetPointerCheck(bool bMustAlive)
{
	CPlayerBase* pTarget = this->CharacterGetByID(m_iIDTarget, bMustAlive);
	// if(pTarget &amp;&amp; (PSA_DEATH == pTarget-&gt;State())) pTarget = NULL; // Prevent the character selected by picking from being arbitrarily modified in the middle

	// if(NULL == pTarget) { m_iIDTarget = -1; }
	return pTarget;
}

bool CPlayerBase::JointPosGet(int nJointIdx, __Vector3& vPos)
{
	const __Matrix44* pMtx = m_Chr.MatrixGet(nJointIdx);
	if (!pMtx) return false;

	vPos = pMtx->Pos();
	vPos *= m_Chr.m_Matrix;

	return true;
}

void CPlayerBase::AnimationAdd(e_Ani eAni, bool bImmediately)
{
	if (bImmediately)
	{
		this->AnimationClear(); // Erased from the deck...
		m_Chr.AniCurSet(eAni); // right setting..
	}
	else
	{
		m_AnimationDeque.push_back(eAni);
	}
}

const __Matrix44 CPlayerBase::CalcShadowMtxBasicPlane(__Vector3 vOffs)
{
	__Matrix44	mtx;
	mtx.Identity();

	mtx._21 = -vOffs.x / vOffs.y;
	mtx._22 = 0.0f;
	mtx._23 = -vOffs.z / vOffs.y;

	mtx._41 = vOffs.x / vOffs.y;
	mtx._42 = 1.0f;
	mtx._43 = vOffs.z / vOffs.y;

	return mtx;
}

void CPlayerBase::RenderShadow(float fAngle)
{
	if (nullptr == m_Chr.Joint()) return;
	if (m_Chr.m_nLOD < 0 || m_Chr.m_nLOD > MAX_CHR_LOD) return;

	int iLODTemp = m_Chr.m_nLOD; // Shadows are taken with less load.
	iLODTemp += 2;
	if (iLODTemp >= MAX_CHR_LOD) return;

	__Matrix44 mV, mVvar, mVBack, mtxRotX, mtxRotZ;	mV.Identity();
	__Vector3 vPosBack, vNom;
	CN3Base::s_lpD3DDev->GetTransform(D3DTS_WORLD, (_D3DMATRIX*)&mVBack);
	vPosBack = m_Chr.m_Matrix.Pos();
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &mV);
	ACT_WORLD->GetNormalWithTerrain(vPosBack.x, vPosBack.z, vNom);	vNom.Normalize();

	mV.PosSet(0.0f, 0.0f, 0.0f);
	float fXRad, fZRad;
	fZRad = asin(vNom.z);
	mtxRotZ.RotationX(fZRad);
	fXRad = asin(-vNom.x);
	mtxRotX.RotationZ(fXRad);
	mV *= mtxRotX;
	mV *= mtxRotZ;
	vPosBack.y += 0.1f;

	mV.PosSet(vPosBack);
	mVvar = mV;

	for (auto i = 0; i < SHADOW_SIZE; i++)
		m_bitset[i].Resize(SHADOW_SIZE);

	for (auto i = 0; i < 4; i++)
	{
		m_vTVertex[i] = m_pvVertex[i];
		m_vTVertex[i] *= mV;
	}

	// Scale according to the size of the character.
	float fScale = Height() * Radius();
	m_fShaScale = 1.0f;
	__Matrix44 mtPos;	mtPos.Identity(); mtPos.PosSet(mV.Pos());
	if (fScale > 6.6f)
	{
		m_fShaScale = 2.2f;
		for (auto i = 0; i < 4; i++)
		{
			m_vTVertex[i] = m_pvVertex[i];
			m_vTVertex[i] *= m_fShaScale;
			m_vTVertex[i] *= mtPos;
		}
	}
	else
	{
		for (auto i = 0; i < 4; i++)
		{
			m_vTVertex[i] = m_pvVertex[i];
			m_vTVertex[i] *= mtPos;
		}
	}

	// All you have to do is calculate the position of the sun and put it in.
	if ((fAngle < 4.0f) || (fAngle > 6.1f))
	{
		if (fAngle > 3.14f)
		{
			fAngle -= 3.14f;
			fAngle = 3.14f - fAngle;
		}
	}
	float fAngleDeg = D3DXToDegree(fAngle);

	float zVal = s_vLightOffset.Magnitude();
	int iDiv = (int)((int)fAngleDeg) % ((int)(180));
	fAngleDeg = (float)iDiv;

	if ((fAngleDeg >= 0.0f) && (fAngleDeg < 50.0f))
		fAngleDeg = 50.0f;
	else if ((fAngleDeg > 130.0f) && (fAngleDeg <= 180.0f))
		fAngleDeg = 130.0f;

	__Matrix44 mtxRZ; mtxRZ.RotationZ(D3DXToRadian(fAngleDeg));
	__Vector3 vLP; vLP.Set(-zVal, 0.0f, 0.0f);	vLP *= mtxRZ;	vLP.Normalize();

	int iPC = m_Chr.PartCount();
	for (auto i = 0; i < iPC; i++)
	{
		CalcPart(m_Chr.Part(i), iLODTemp, vLP);
	}

	iPC = m_Chr.PlugCount();
	for (auto i = 0; i < iPC; i++)
	{
		CalcPlug(m_Chr.Plug(i), m_Chr.MatrixGet(m_Chr.Plug(i)->m_nJointIndex), mVvar, vLP);
	}

	// Scale down before rendering.
	if (fScale > 6.6f)
	{
		for (auto i = 0; i < 4; i++)
		{
			m_vTVertex[i] = m_pvVertex[i];
			m_vTVertex[i] *= 0.82f;
			m_vTVertex[i] *= m_fShaScale;
			m_vTVertex[i] *= mV;
		}
	}
	else
	{
		for (auto i = 0; i < 4; i++)
		{
			m_vTVertex[i] = m_pvVertex[i];
			m_vTVertex[i] *= 0.5f;
			m_vTVertex[i] *= mV;
		}
	}

	D3DLOCKED_RECT LR;
	if (!m_N3Tex.Get()) return;
	HRESULT hr = m_N3Tex.Get()->LockRect(0, &LR, nullptr, 0);
	if (hr != D3D_OK)
		return;

	auto pDst16 = (LPWORD)LR.pBits;
	WORD dwColor = SHADOW_COLOR;
	dwColor = dwColor << 12;
	for (auto i = 0; i < SHADOW_SIZE; i++)
	{
		for (int j = 0; j < SHADOW_SIZE; j++)
		{
			if (m_bitset[i].On(j))
				pDst16[SHADOW_SIZE * i + j] = dwColor;
			else
				pDst16[SHADOW_SIZE * i + j] = 0x0000;
		}
	}

	m_N3Tex.Get()->UnlockRect(0);

	static DWORD dwAlpha, dwFog, dwCull, dwColorVertex, dwMaterial, dwZWrite,
		dwColorOp0, dwColorArg01, dwColorArg02, dwColorOp1, dwColorArg11, dwColorArg12,
		dwAlphaOp0, dwAlphaArg01, dwAlphaArg02, dwAlphaOp1, dwAlphaArg11, dwAlphaArg12, dwSrcBlend, dwDestBlend, dwBlendOp,
		dwMagFilter0, dwMinFilter0, dwMagFilter1, dwMinFilter1;

	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_COLORVERTEX, &dwColorVertex);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &dwMaterial);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWrite);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp0);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg01);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg02);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_COLOROP, &dwColorOp1);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_COLORARG1, &dwColorArg11);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_COLORARG2, &dwColorArg12);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp0);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg01);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg02);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_ALPHAOP, &dwAlphaOp1);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_ALPHAARG1, &dwAlphaArg11);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_ALPHAARG2, &dwAlphaArg12);
	CN3Base::s_lpD3DDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter0);
	CN3Base::s_lpD3DDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter0);
	CN3Base::s_lpD3DDev->GetSamplerState(1, D3DSAMP_MAGFILTER, &dwMagFilter1);
	CN3Base::s_lpD3DDev->GetSamplerState(1, D3DSAMP_MINFILTER, &dwMinFilter1);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_BLENDOP, &dwBlendOp);

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	CN3Base::s_lpD3DDev->SetTexture(0, m_N3Tex.Get());

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	CN3Base::s_lpD3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	CN3Base::s_lpD3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	CN3Base::s_lpD3DDev->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	CN3Base::s_lpD3DDev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	CN3Base::s_lpD3DDev->SetFVF(FVF_VNT1);
	CN3Base::s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, m_pIndex, D3DFMT_INDEX16, m_vTVertex, sizeof(__VertexT1));

	// ..
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp0);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg01);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorArg02);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, dwColorOp1);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, dwColorArg11);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, dwColorArg12);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp0);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg01);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwAlphaArg02);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, dwAlphaOp1);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, dwAlphaArg11);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG2, dwAlphaArg12);

	CN3Base::s_lpD3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMagFilter0);
	CN3Base::s_lpD3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMinFilter0);
	CN3Base::s_lpD3DDev->SetSamplerState(1, D3DSAMP_MAGFILTER, dwMagFilter1);
	CN3Base::s_lpD3DDev->SetSamplerState(1, D3DSAMP_MINFILTER, dwMinFilter1);

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_COLORVERTEX, dwColorVertex);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, dwMaterial);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWrite);

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, dwCull);

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_BLENDOP, dwBlendOp);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);

	CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, (_D3DMATRIX*)&mVBack);
}

void CPlayerBase::CalcPart(CN3CPart* pPart, int nLOD, __Vector3 vDir)
{
	if (nullptr == pPart) return;
	if (nLOD < 0 || nLOD > MAX_CHR_LOD) return;
	if (nullptr == pPart->Skins() || nullptr == pPart->Skin(nLOD) || pPart->Skin(nLOD)->VertexCount() <= 0) return;

	// #ifdef _DEBUG
	// CN3Base::s_RenderInfo.nChr_Part++; // Rendering Information Update...
	// CN3Base::s_RenderInfo.nChr_Polygon += pPart->Skin(nLOD)->FaceCount();
	// #endif

	const int iTotalCount = pPart->Skin(nLOD)->VertexCount();
	if (iTotalCount < 0 || iTotalCount >  10000) return;

	__Vector3 vec, A, B, C, vPick;
	const __VertexXyzNormal* pVDest = pPart->Skin(nLOD)->Vertices();
	float t, u, v, fx, fz;

	for (auto i = 0; i < iTotalCount; i++)
	{
		if (pVDest)
		{
			vec = pVDest[i];
			vec *= m_Chr.m_Matrix;

			int iX, iZ;

			A = m_vTVertex[m_pIndex[0]];			B = m_vTVertex[m_pIndex[1]];			C = m_vTVertex[m_pIndex[2]];
			if (::_IntersectTriangle(vec, vDir, A, B, C, t, u, v, &vPick))
			{
				fx = (vPick.x - C.x) / (SHADOW_PLANE_SIZE * 2.0f * m_fShaScale);
				fz = (vPick.z - C.z) / (SHADOW_PLANE_SIZE * 2.0f * m_fShaScale);
				iX = (int)(fx * SHADOW_SIZE);
				iZ = (int)(fz * SHADOW_SIZE);
				if ((iX > 0) && (iX < SHADOW_SIZE) && (iZ > 0) && (iZ < SHADOW_SIZE))
					m_bitset[iZ].Set(iX);
			}

			A = m_vTVertex[m_pIndex[3]];			B = m_vTVertex[m_pIndex[4]];			C = m_vTVertex[m_pIndex[5]];
			if (::_IntersectTriangle(vec, vDir, A, B, C, t, u, v, &vPick))
			{
				fx = (vPick.x - B.x) / (SHADOW_PLANE_SIZE * 2.0f * m_fShaScale);
				fz = (vPick.z - B.z) / (SHADOW_PLANE_SIZE * 2.0f * m_fShaScale);
				iX = (int)(fx * SHADOW_SIZE);
				iZ = (int)(fz * SHADOW_SIZE);
				if ((iX > 0) && (iX < SHADOW_SIZE) && (iZ > 0) && (iZ < SHADOW_SIZE))
					m_bitset[iZ].Set(iX);
			}
		}
	}
}

void CPlayerBase::CalcPlug(CN3CPlugBase* pPlug, const __Matrix44* pmtxJoint, __Matrix44& mtxMV, __Vector3 vDir)
{
	if (nullptr == pPlug || nullptr == pPlug->PMeshInst()) return;
	if (pPlug->PMeshInst()->GetNumVertices() <= 0) return;

	// #ifdef _DEBUG
	// CN3Base::s_RenderInfo.nChr_Plug++; // Rendering Information Update...
	// CN3Base::s_RenderInfo.nChr_Plug_Polygon += pPlug->PMeshInst()->GetNumIndices() / 3; // Rendering Information Update...
	// #endif

	static __Matrix44 mtx, mtxBack;
	__Vector3 vOf;
	mtx = pPlug->m_Matrix;
	mtx *= (*pmtxJoint);
	mtx *= m_Chr.m_Matrix;

	const int iTotalCount = pPlug->PMeshInst()->GetNumVertices();
	if (iTotalCount < 0 || iTotalCount >  10000) return;

	__Vector3 vec, A, B, C, vPick;
	float t, u, v, fx, fz;

#ifdef _USE_VERTEXBUFFER
	LPDIRECT3DVERTEXBUFFER9	pBuf = NULL;
	__VertexT1* pVerT1 = NULL;
	pBuf = pPlug->PMeshInst()->GetVertexBuffer();
	if (pBuf)
		pBuf->Lock(0, 0, (BYTE**)(&pVerT1), 0);
#else 
	__VertexT1* pVerT1 = nullptr;
	pVerT1 = pPlug->PMeshInst()->GetVertices();
#endif

	for (auto i = 0; i < iTotalCount; i++)
	{
		if (pVerT1)
		{
			vec = pVerT1[i];
			vec *= mtx;

			int iX, iZ;

			A = m_vTVertex[m_pIndex[0]];			B = m_vTVertex[m_pIndex[1]];			C = m_vTVertex[m_pIndex[2]];
			if (::_IntersectTriangle(vec, vDir, A, B, C, t, u, v, &vPick))
			{
				fx = (vPick.x - C.x) / (SHADOW_PLANE_SIZE * 2.0f * m_fShaScale);
				fz = (vPick.z - C.z) / (SHADOW_PLANE_SIZE * 2.0f * m_fShaScale);
				iX = (int)(fx * SHADOW_SIZE);
				iZ = (int)(fz * SHADOW_SIZE);
				if ((iX > 0) && (iX < SHADOW_SIZE) && (iZ > 0) && (iZ < SHADOW_SIZE))
					m_bitset[iZ].Set(iX);
			}

			A = m_vTVertex[m_pIndex[3]];			B = m_vTVertex[m_pIndex[4]];			C = m_vTVertex[m_pIndex[5]];
			if (::_IntersectTriangle(vec, vDir, A, B, C, t, u, v, &vPick))
			{
				fx = (vPick.x - B.x) / (SHADOW_PLANE_SIZE * 2.0f * m_fShaScale);
				fz = (vPick.z - B.z) / (SHADOW_PLANE_SIZE * 2.0f * m_fShaScale);
				iX = (int)(fx * SHADOW_SIZE);
				iZ = (int)(fz * SHADOW_SIZE);
				if ((iX > 0) && (iX < SHADOW_SIZE) && (iZ > 0) && (iZ < SHADOW_SIZE))
					m_bitset[iZ].Set(iX);
			}
		}
	}

#ifdef _USE_VERTEXBUFFER
	if (pBuf)
		pBuf->Unlock();
#endif
}

__Vector3	CPlayerBase::Max()
{
	if (m_pShapeExtraRef)
		return m_pShapeExtraRef->Max();

	return m_Chr.Max();
}

__Vector3	CPlayerBase::Min()
{
	if (m_pShapeExtraRef)
		return m_pShapeExtraRef->Min();

	return m_Chr.Min();
}

__Vector3	CPlayerBase::Center()
{
	__Vector3 vCenter;

	if (m_pShapeExtraRef)
	{
		vCenter = m_pShapeExtraRef->Min() + (m_pShapeExtraRef->Max() - m_pShapeExtraRef->Min()) * 0.5f;
		return vCenter;
	}

	return (m_Chr.Min() + (m_Chr.Max() - m_Chr.Min()) * 0.5f);
}
