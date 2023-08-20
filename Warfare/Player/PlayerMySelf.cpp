#include "pch.h"
#include "PlayerMySelf.h"

#include "PacketDef.h"
#include "PlayerOtherMgr.h"

#include "N3WorldManager.h"
#include "GameProcMain.h"
#include "UIInventory.h"
#include "MagicSkillMng.h"

#include "GameEng.h"

#include "DFont.h"
#include "N3Camera.h"
#include "N3ShapeMgr.h"
#include "N3SndObj.h"

CPlayerMySelf::CPlayerMySelf()
{
	m_ePlayerType = PLAYER_MYSELF; // Player Type ... Base, NPC, OTher, MySelf

	m_bRunning = false;				// Are you running?
	m_bMoveContinous = false;		// Do you keep moving?
	m_bAttackContinous = false;		// Are you still under attack?
	m_bSitDown = false;				// Are you sitting...
	m_bRecruitParty = false;		// Looking for a party??
	
	m_bStun = false;				// faint..
	m_fStunTime = 0.0f;				// embarrassing time...


	m_fAttackTimeRecent = CN3Base::TimeGet();	// Last attack time.
	m_bTempMoveTurbo = false;

	m_InfoExt.Init();

	// Inventory..
	m_ChrInv.PartAlloc(PART_POS_COUNT);
	m_ChrInv.PlugAlloc(PLUG_POS_COUNT);

	m_iSendRegeneration = 0;	// Once sent, a flag that will not be seen until it dies again

	m_dwMagicID = 0xffffffff;
	m_fCastingTime = 0.0f;
	m_pObjectTarget = nullptr; // target object pointer.
}

CPlayerMySelf::~CPlayerMySelf()
{
}

void CPlayerMySelf::Release()
{
	m_bTargetOrPosMove = false;
	m_iMoveTarget = -1;

	m_bRunning = false;				// Are you running?
	m_bMoveContinous = false;		// Do you keep moving?
	m_bAttackContinous = false;		// Are you still under attack?
	m_bSitDown = false;				// Are you sitting...
	m_bRecruitParty = false;		// Looking for a party??

	m_bStun = false;				// faint..
	m_fStunTime = 0.0f;				// stunned time..

	m_fAttackTimeRecent = CN3Base::TimeGet();	// Last attack time.
	m_bTempMoveTurbo = false;

	m_InfoExt.Init();

	// Inventory..
	m_ChrInv.Release();
	m_ChrInv.PartAlloc(PART_POS_COUNT);
	m_ChrInv.PlugAlloc(PLUG_POS_COUNT);

	m_pObjectTarget = nullptr; // target object pointer.

	CPlayerBase::Release();
}

void CPlayerMySelf::SetMoveTargetID(int iID)
{
	m_bTargetOrPosMove	= true;
	m_iMoveTarget		= iID;
}

void CPlayerMySelf::SetMoveTargetPos(__Vector3 vPos)
{
	m_vTargetPos		= vPos;
	m_bTargetOrPosMove	= true;
	m_iMoveTarget		= -1;
}

void CPlayerMySelf::Tick()
{
	BOOL	bAnim = TRUE;

	if(	PSA_DEATH == m_eState)  // It&#39;s in a dying state... When the dying animation is over... // A flag that doesn&#39;t show until it dies again once it&#39;s sent.
	{
		if(0 == m_iSendRegeneration) 
		{
			// std::string szMsg; ::_LoadStringFromResource(IDS_REGENERATION, szMsg);
			// CGameProcedure::MessageBoxPost(szMsg, &quot;&quot;, MB_OK, BEHAVIOR_REGENERATION); // Send recreate message..
			// CLogWriter::Write("Dead!!!");
			m_iSendRegeneration = 1;
		}
		return;
	}

	if(m_fTimeAfterDeath > 0) // forced to kill...
	{
		if(m_fTimeAfterDeath > 3.0f)
			this->Action(PSA_DYING, false); // Dies after 3 seconds.
		CPlayerBase::Tick();  // Rotation, handling specified animation ticks and color assignments.. etc..
		return;
	}
	
	if(IsDead()) // If dead, return.
	{
		CGameProcedure::s_pProcMain->CommandEnableAttackContinous(false, nullptr);
		CPlayerBase::Tick();  // Rotation, handling specified animation ticks and color assignments.. etc..
		return;
	}

	// 
	// My player is treated differently because his movement is different from others..
	if(	PSM_WALK == m_eStateMove ||
		PSM_WALK_BACKWARD  == m_eStateMove ||
		PSM_RUN == m_eStateMove ) // If you walk or run back and forth.
	{
		this->MoveSpeedCalculationAndCheckCollision(); // Moving speed and collision check...
		if(0 == m_fMoveSpeedPerSec) // If there is no speed, the collision check result will not go...
		{
			this->ActionMove(PSM_STOP);
			CGameProcedure::s_pProcMain->MsgSend_Move(false, false); // Send stop packets...
		}

		const __Vector3 vPos = this->Position();
		CPlayerBase::m_pSnd_MyMove = m_pSnd_Move;
		if(m_pSnd_Move && m_Chr.NeedPlaySound0()) 
			m_pSnd_Move->Play(&vPos);
		if(m_pSnd_Move && m_Chr.NeedPlaySound1()) 
			m_pSnd_Move->Play(&vPos);

		TargetOrPosMove();
	}
	else 
	{
		CPlayerBase::m_pSnd_MyMove = nullptr;
		m_fMoveSpeedPerSec = 0;
	}
	// My player is treated differently because his movement is different from others..
	// 

	CPlayerBase* pTarget = nullptr;
	// if(true == m_bAttackContinuous || m_iSkillStep &gt; 0)
	if(true == m_bAttackContinous && m_iSkillStep <= 0)
	{
		pTarget = TargetPointerCheck(false);
		if(nullptr == pTarget)
		{
			CGameProcedure::s_pProcMain->CommandEnableAttackContinous(false, nullptr);
		}
		else
		{
			const float fTime = CN3Base::TimeGet();
			
			// It is handled with skills such as archery and crossbow shooting.
			if(	(m_pItemPlugBasics[PLUG_POS_LEFTHAND] && ITEM_CLASS_BOW == m_pItemPlugBasics[PLUG_POS_LEFTHAND]->byClass ) || 
				(m_pItemPlugBasics[PLUG_POS_LEFTHAND] && ITEM_CLASS_BOW_LONG == m_pItemPlugBasics[PLUG_POS_LEFTHAND]->byClass ) || 
				(m_pItemPlugBasics[PLUG_POS_RIGHTHAND] && ITEM_CLASS_BOW_CROSS == m_pItemPlugBasics[PLUG_POS_RIGHTHAND]->byClass) )
			{
				__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(102003); // Find the basic bow skill in the skill table
				if(pSkill)
				{
					if(pTarget->IsAlive())// As long as the temporary user is alive...
					{
						float fInterval = (pSkill->iCastTime / 10.f) + 0.15f;
						if(m_fAttackDelta > 0) fInterval /= m_fAttackDelta;
						if(fTime > m_fAttackTimeRecent + fInterval) // If the attack interval is exceeded... attack!
						{
							if(CGameProcedure::s_pProcMain->m_pMagicSkillMng->MsgSend_MagicProcess(m_iIDTarget, pSkill))
							{// If you succeed in sending skill packets.
								TRACE("%.2f\n", fTime - m_fAttackTimeRecent);
							}
							m_fAttackTimeRecent = fTime;	// Last attack time.
						}
					}
				}
			}
			else if( m_pItemPlugBasics[PLUG_POS_RIGHTHAND] && ITEM_CLASS_LAUNCHER == m_pItemPlugBasics[PLUG_POS_RIGHTHAND]->byClass ) // If it&#39;s a javelin item...
			{
				__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(102009); // Find the basic bow skill in the skill table
				if(pSkill && fTime > m_fAttackTimeRecent + (pSkill->iCastTime / 10.f)) // If the attack interval is exceeded... attack!
				{
					if(pTarget->IsAlive())// Only when the temporary user is alive
					{
						CGameProcedure::s_pProcMain->m_pMagicSkillMng->MsgSend_MagicProcess(m_iIDTarget, pSkill); // If you succeed in sending skill packets.
						TRACE("%.2f\n", fTime - m_fAttackTimeRecent);
						m_fAttackTimeRecent = fTime;	// Last attack time.
					}
				}
			}
			else // If it&#39;s an attack...
			{
				float fIntervalTable = 1.0f;
				if(m_pItemPlugBasics[PLUG_POS_RIGHTHAND] && m_pItemPlugExts[PLUG_POS_RIGHTHAND]) // Attack Interval Definition
				{
					fIntervalTable = (m_pItemPlugBasics[PLUG_POS_RIGHTHAND]->siAttackInterval / 100.0f)
						* (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->siAttackIntervalPercentage / 100.0f); // 100 is 1 second 110 goes to 1.1 elementary... percent...
				}
				
				float fInterval = fIntervalTable;
				if(m_fAttackDelta > 0) fInterval /= m_fAttackDelta; // Attack speed that changes by skill or magic.. 1.0 is the default, and the higher it is, the faster it attacks.

				if(	fTime > m_fAttackTimeRecent + fInterval) // If the attack interval is exceeded... attack!
				{
					const bool bCastingNow = CGameProcedure::s_pProcMain->m_pMagicSkillMng->IsCasting();
					if(false == bCastingNow) // Attack packets are not sent while casting.
					{
						const bool bAttackable = IsAttackableTarget(pTarget);
						if(bAttackable) // attack possible.
						{
							const float fDistance = s_pPlayer->DistanceExceptRadius(pTarget); // attack distance
							
							CGameProcedure::s_pProcMain->MsgSend_Attack(pTarget->IDNumber(), fIntervalTable, fDistance);
							if(m_iSkillStep == 0 && PSA_ATTACK != m_eState && m_fFlickeringFactor == 1.0f) // If you don&#39;t use skills and don&#39;t attack...
								this->Action(PSA_ATTACK, true, pTarget); // If you are not attacking, attack.
							
							m_fAttackTimeRecent = fTime;	// Last attack time.
						}
						else 
						{
							if(CGameProcedure::s_pEng->ViewPoint() == VP_THIRD_PERSON)
							{
								CGameProcedure::s_pPlayer->RotateTo(pTarget); // turn

								const float fDist = (pTarget->Position() - m_Chr.Pos()).Magnitude(); // Find the attack distance.
								const float fDistLimit = this->AttackableDistance(pTarget);

								if(fDist > fDistLimit && !m_bTargetOrPosMove)
								{
									CGameProcedure::s_pProcMain->CommandMove(MD_FOWARD, true);
									s_pPlayer->SetMoveTargetID(pTarget->IDNumber());
								}
							}
							if(m_iSkillStep == 0 && PSA_SITDOWN != m_eState) // I&#39;m not using skills, but unless I&#39;m sitting...
								this->Action(PSA_BASIC, true); // basic tax...
						}
					}
				}
			}

			if(fTime == m_fAttackTimeRecent) // If you attack...
				CGameProcedure::s_pProcMain->CommandSitDown(false, false); // Raise it up.. There is a bug in the sitting state..
		}
	}


	if(m_bStun) // If you faint...
	{
		m_fStunTime -= s_fSecPerFrm; // stun time handling..
		if(m_fStunTime < 0) this->StunRelease(); // Relieve fainting..
	}


	if(PSA_ATTACK == m_eState || m_iSkillStep != 0) // If you are attacking or using a skill...
	{
		if(!pTarget) pTarget = TargetPointerCheck(false); // Get target pointer.
		CPlayerBase::ProcessAttack(pTarget); // Routine handling of attacks. It only handles animation settings and collisions, but does not process packets.
	}

	if(m_dwMagicID != 0xffffffff) 
		m_fCastingTime += CN3Base::s_fSecPerFrm;

	CPlayerBase::Tick(); // Rotation, handling specified animation ticks and color assignments.. etc..
}

void CPlayerMySelf::Render(float fSunAngle)
{
	// Force LOD Level + 1
	m_Chr.m_nLOD--;
	if(m_Chr.m_nLOD < 0) m_Chr.m_nLOD = 0;

	const int iLODDeltaPrev = CN3Chr::LODDelta(); // Let&#39;s draw my character more elaborately..
	CN3Chr::LODDeltaSet(0);
	CPlayerBase::Render(fSunAngle);
	CN3Chr::LODDeltaSet(iLODDeltaPrev);
}




bool CPlayerMySelf::ToggleAttackContinous()
{
	if(!IsAlive()) return false;

	if(false == m_bAttackContinous) // See if there is a target.
	{
		const CPlayerNPC* pTarget = s_pOPMgr->CharacterGetByID(m_iIDTarget, true);
		if(pTarget) // &amp;&amp; !IsOutOfAttackRange(pTarget)) // Only when there is a target and is within range
		{
			this->m_bAttackContinous = true;
		}
	}
	else
	{
		m_bAttackContinous = false;
	}

	return m_bAttackContinous;
}

bool CPlayerMySelf::ToggleRunMode()
{
	m_bRunning = !m_bRunning; // Are you running?
	if(true == m_bRunning)
	{
		if(PSM_WALK == m_eStateMove) 
		{
			m_eStateMove = PSM_RUN;
			m_Chr.AniCurSet(ANI_RUN);
		}
	}
	else
	{
		if(PSM_RUN == m_eStateMove) 
		{
			m_eStateMove = PSM_WALK;
			m_Chr.AniCurSet(ANI_WALK);
		}
	}

	return m_bRunning;
}

void CPlayerMySelf::ToggleMoveMode()
{
	m_bMoveContinous = !m_bMoveContinous; // Do you keep moving?
	
	int nAni = ANI_BREATH;
	if(m_bMoveContinous) // If you need to keep moving...
	{
		if(m_bRunning) // If you run...
		{
			m_eStateMove = PSM_RUN;
			nAni = ANI_RUN;
		}
		else
		{
			m_eStateMove = PSM_WALK;
			nAni = ANI_WALK;
		}

		const float fSpeed = this->MoveSpeedCalculationAndCheckCollision(); // Moving speed and collision check...
		if(0 == fSpeed) // If you can&#39;t go...
		{
			m_bMoveContinous = false;
			m_eStateMove = PSM_STOP;
			m_eState = PSA_BASIC;
			nAni = this->JudgeAnimationBreath(); // if you have a target. Depending on the weapon you are holding, the basic animation is sold.. If not, just the basic animation
		}
	} 
	else
	{
		m_eStateMove = PSM_STOP;
		m_eState = PSA_BASIC;
		nAni = this->JudgeAnimationBreath(); // if you have a target. Depending on the weapon you are holding, the basic animation is sold.. If not, just the basic animation
	}

	this->AnimationClear();
	m_Chr.AniCurSet(nAni); // Whole walking, turning and stopping apply
}

__Vector3 CPlayerMySelf::NextPos(float fTimeAfter) const
{
	const __Matrix44 mtxRot = m_Chr.Rot();
	const __Vector3 vDir(0,0,1);
	__Vector3 vNextPos = m_Chr.Pos() + ((vDir * mtxRot) * m_fMoveSpeedPerSec * fTimeAfter);

	return vNextPos;
}

void CPlayerMySelf::RotAdd(const float fRotRadianPerSec)			// Enter the rotation speed per second around the y-axis.
{
	m_fYawCur += fRotRadianPerSec * s_fSecPerFrm;

	if(m_fYawCur >= D3DXToRadian(180.0f) * 2) m_fYawCur -= D3DXToRadian(180.0f) * 2;
	if(m_fYawCur <= -D3DXToRadian(180.0f) * 2) m_fYawCur += D3DXToRadian(180.0f) * 2;
	m_fYawToReach = m_fYawCur; // turn right...
}

void CPlayerMySelf::InventoryChrRender(const RECT& Rect)
{
/*
	DWORD dwUsefog = TRUE;
	CN3Base::s_lpD3DDev-&gt;GetRenderState( D3DRS_FOGENABLE , &amp;dwUsefog );

	DWORD dwUseLighting=TRUE;
	CN3Base::s_lpD3DDev->GetRenderState( D3DRS_LIGHTING, &dwUseLighting );

	int	bLight[8];
	for(auto i = 0; i < 8; i++ )	CN3Base::s_lpD3DDev->GetLightEnable(i, &bLight[i]);

	if (dwUseLighting) CN3Base::s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, TRUE );
	if (dwUsefog) CN3Base::s_lpD3DDev->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// set render states
	for(auto i = 1; i < 8; i++ )	CN3Base::s_lpD3DDev->LightEnable(i, FALSE);
	CN3Base::s_lpD3DDev->LightEnable(0, TRUE);

	D3DLIGHT9 lgt0;
	
	memset(&lgt0, 0, sizeof(D3DLIGHT9));
	lgt0.Type = D3DLIGHT_POINT;
	lgt0.Attenuation0 = 0.5f;
	lgt0.Range = 100.0f;
	lgt0.Position = __Vector3(0.0f, 2.0f, -10.0f);
	lgt0.Diffuse.r = 220/255.0f; lgt0.Diffuse.g = 255/255.0f; lgt0.Diffuse.b = 220/255.0f;
	CN3Base::s_lpD3DDev->SetLight(0, &lgt0);

	// Take a character...
	//
	__Matrix44 mtxproj, mtxview, mtxworld, mtxprojback, mtxviewback, mtxworldback;

	CN3Base::s_lpD3DDev->GetTransform( D3DTS_PROJECTION, &mtxprojback );
	CN3Base::s_lpD3DDev->GetTransform( D3DTS_VIEW, &mtxviewback );
	CN3Base::s_lpD3DDev->GetTransform( D3DTS_WORLD, &mtxworldback );

	D3DXMatrixOrthoLH(&amp;mtxproj, 12.0f, 9.0f, 0, 100);
    CN3Base::s_lpD3DDev->SetTransform( D3DTS_PROJECTION, &mtxproj );

    D3DXMatrixLookAtLH( &mtxview, &D3DXVECTOR3( 0.0f, 2.0f,-10.0f ),
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    CN3Base::s_lpD3DDev->SetTransform( D3DTS_VIEW, &mtxview );

	mtxworld.Identity();
	CN3Base::s_lpD3DDev->SetTransform( D3DTS_WORLD, &mtxworld );


	// 
	// Get 2d coordinates..

	// 2d -&gt; 3d..
	__Vector3 vPos;
	vPos.x = (float)((Rect.right - Rect.left)/2.0f + Rect.left);
	vPos.y = (float)(Rect.bottom) - (Rect.bottom - Rect.top)/16.0f;
	vPos.z = 0.11f;

	float fWidth, fHeight;
	fWidth = s_CameraData.vp.Width;	fHeight = s_CameraData.vp.Height;	fWidth *= 0.5f;	fHeight *= 0.5f;

	vPos.x = (vPos.x - fWidth)/fWidth;
	vPos.y = (fHeight - vPos.y)/fHeight;

	__Matrix44 mtxProjInv, mtxViewInv;
	D3DXMatrixInverse(&mtxProjInv, NULL, &mtxproj);
	D3DXMatrixInverse(&mtxViewInv, NULL, &mtxview);

	vPos *= mtxProjInv;
	vPos *= mtxViewInv;

	m_ChrInv.PosSet(vPos.x, vPos.y, 1.0f);
	__Quaternion qt;
	qt.RotationAxis(0.0f, 1.0f, 0.0f, D3DXToRadian(198.0f));
	m_ChrInv.RotSet(qt);

	CGameProcedure::s_pEng->ClearZBuffer(NULL);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_ChrInv.m_nLOD = 0;
	m_ChrInv.Render();

	CN3Base::s_lpD3DDev->SetTransform( D3DTS_PROJECTION, &mtxprojback );
	CN3Base::s_lpD3DDev->SetTransform( D3DTS_VIEW, &mtxviewback );
	CN3Base::s_lpD3DDev->SetTransform( D3DTS_WORLD, &mtxworldback );

	CN3Base::s_lpD3DDev-&gt;SetRenderState(D3DRS_AMBIENT, 0x00000000);

	CN3Base::s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, dwUseLighting );
	CN3Base::s_lpD3DDev->SetRenderState( D3DRS_FOGENABLE , dwUsefog );
	for(auto i = 0; i < 8; i++ )	CN3Base::s_lpD3DDev->LightEnable(i, bLight[i]);
	*/
	// fix dino down
	// backup render state
	DWORD dwLighting;
	D3DLIGHT9 BackupLight0;

	s_lpD3DDev->GetRenderState( D3DRS_LIGHTING, &dwLighting );
	BOOL bLight[8];
	for(auto i = 0; i < 8; ++i )	s_lpD3DDev->GetLightEnable(i, &(bLight[i]));
	s_lpD3DDev->GetLight(0, &BackupLight0);

	// set render state
	if (TRUE != dwLighting) s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, TRUE );
	for(auto i = 1; i < 8; ++i )	s_lpD3DDev->LightEnable(i, FALSE);
	s_lpD3DDev->LightEnable(0, TRUE);

	// 0 light setting
	D3DLIGHT9 Light0;
	memset(&Light0, 0, sizeof(D3DLIGHT9));
	Light0.Type = D3DLIGHT_POINT;
	Light0.Attenuation0 = 0.5f;
	Light0.Range = 100.0f;
	Light0.Position = __Vector3(0.0f, 2.0f, 10.0f);
	Light0.Diffuse.r = 220/255.0f; Light0.Diffuse.g = 255/255.0f; Light0.Diffuse.b = 220/255.0f;
	s_lpD3DDev->SetLight(0, &Light0);

	// Set character position and orientation
	m_ChrInv.PosSet(__Vector3(0,0,0));
	__Quaternion qt;
	qt.RotationAxis(0.0f, 1.0f, 0.0f, D3DXToRadian(18.0f));	// Standing slightly obliquely...
	m_ChrInv.RotSet(qt);

	// render
	RenderChrInRect(&m_ChrInv, Rect);

	// restore
	if (TRUE != dwLighting) s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, dwLighting );
	for (auto i = 0; i < 8; ++i )	s_lpD3DDev->LightEnable(i, bLight[i]);
	s_lpD3DDev->SetLight(0, &BackupLight0);
}

void CPlayerMySelf::InventoryChrTick()
{
	// Inventory Animation Tick..
	m_ChrInv.PosSet(m_Chr.Pos());
	m_ChrInv.Tick();
}



void CPlayerMySelf::InventoryChrAnimationInitialize()
{
	const int iAniTmp = m_ChrInv.AniCtrl()->Count() - 1;
	m_ChrInv.AniCurSet(iAniTmp, false, 0);
	m_ChrInv.AniCurSet(ANI_BREATH, false, 0);
	m_ChrInv.Tick(); // do it once
}

CN3CPlugBase* CPlayerMySelf::PlugSet(e_PlugPosition ePos, const std::string& szFN, __TABLE_ITEM_BASIC* pItemBasic, __TABLE_ITEM_EXT* pItemExt)
{
	int iJoint = 0;
	if(PLUG_POS_RIGHTHAND == ePos)
	{
		m_pItemPlugBasics[ePos] = pItemBasic;
		m_pItemPlugExts[ePos] = pItemExt;
		iJoint = m_pLooksRef->iJointRH;
	}
	else if(PLUG_POS_LEFTHAND == ePos)
	{
		m_pItemPlugBasics[ePos] = pItemBasic;
		m_pItemPlugExts[ePos] = pItemExt;
		if(pItemBasic)
		{
			if(pItemBasic->byClass == ITEM_CLASS_SHIELD) iJoint = m_pLooksRef->iJointLH2; // if it&#39;s a shield
			else iJoint = m_pLooksRef->iJointLH; // left end...
		}
	}
	else if (PLUG_POS_KNIGHTS_GRADE == ePos)
	{
		m_pItemPlugBasics[ePos] = pItemBasic;
		m_pItemPlugExts[ePos] = pItemExt;
		iJoint = m_pLooksRef->iJointRH - 2; // right cuff
	}
	else if (PLUG_POS_BACK == ePos)
	{
		// m_pItemBasicPlugRefs[PLUG_POS_BACK] = pItem;
	}
	else { __ASSERT(0, "Invalid Plug Item position"); }

	CN3CPlugBase* pPlug = m_ChrInv.PlugSet(ePos, szFN);
	if(nullptr == pPlug) return nullptr;

	if(PLUG_POS_LEFTHAND == ePos || PLUG_POS_RIGHTHAND == ePos) // Grow in proportion to your height. The default height is 1.8 meters.
	{
		float fScale = m_Chr.Height() / 1.8f;
		fScale *= pPlug->Scale().y / m_Chr.Scale().y;
		pPlug->ScaleSet(__Vector3(fScale, fScale, fScale));
		pPlug->m_nJointIndex = iJoint; // Joint number setting..
	}
	// else if(PLUG_POS_BACK == ePos)
	// {
		// CN3CPlug_Cloak *pPlugCloak = (CN3CPlug_Cloak*)pPlug;
		// pPlugCloak->GetCloak()->SetPlayerBase(this);
	// }

	this->SetSoundPlug(pItemBasic);
	return CPlayerBase::PlugSet(ePos, szFN, pItemBasic, pItemExt);
}

CN3CPart* CPlayerMySelf::PartSet(e_PartPosition ePos, const std::string& szFN, __TABLE_ITEM_BASIC* pItemBasic, __TABLE_ITEM_EXT* pItemExt)
{
	if(ePos < PART_POS_UPPER || ePos >= PART_POS_COUNT)
	{
		__ASSERT(0, "Invalid Item Position");
		return nullptr;
	}

	if(PART_POS_UPPER == ePos) // The upper body requires special treatment.
	{
		if(pItemBasic) // if coated
		{
			if(pItemBasic->byIsRobeType && m_Chr.Part(PART_POS_LOWER)) // It&#39;s a robe-type full top, and if you&#39;re wearing something underneath...
			{
				m_ChrInv.PartSet(PART_POS_LOWER, ""); // empty the bottom.
				m_Chr.PartSet(PART_POS_LOWER, "");
			}
		}
		else // If you take off your upper body
		{
			if(m_pItemPartBasics[ePos] && m_pItemPartBasics[ePos]->byIsRobeType) // If you&#39;ve worn a robe before...
			{
				if(m_pItemPartBasics[PART_POS_LOWER]) // If an item is worn on the lower body...
				{
					std::string szFN2;
					e_PartPosition ePartPos2 = PART_POS_UNKNOWN;
					e_PlugPosition ePlugPos2 = PLUG_POS_UNKNOWN;
					CGameProcedure::MakeResrcFileNameForUPC(m_pItemPartBasics[PART_POS_LOWER], &szFN2, nullptr, ePartPos2, ePlugPos2);
					
					m_ChrInv.PartSet(PART_POS_LOWER, szFN2); // Put the previous clothes on the lower body.
					m_Chr.PartSet(PART_POS_LOWER, szFN2);
				}
				else // If there are no items worn on the lower body...
				{
					const __TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks->Find(m_InfoBase.eRace);	// User Player Character Skin structure pointer..
					m_ChrInv.PartSet(PART_POS_LOWER, pLooks->szPartFNs[PART_POS_LOWER]); // Wear basic clothes on the lower body.
					m_Chr.PartSet(PART_POS_LOWER, pLooks->szPartFNs[PART_POS_LOWER]); // Wear basic clothes on the lower body.
				}
			}
		}
	}
	else if(PART_POS_LOWER == ePos) // If the lower body...
	{
		// Whether you wear the item or take it off...
		if(m_pItemPartBasics[PART_POS_UPPER] && m_pItemPartBasics[PART_POS_UPPER]->byIsRobeType) // If the robe was worn on the upper body before...
		{
			m_pItemPartBasics[ePos] = pItemBasic; // apply item
			m_pItemPartExts[ePos] = pItemExt;
			m_ChrInv.PartSet(ePos, "");
			m_Chr.PartSet(ePos, ""); // The lower body is stripped (?)..
			return nullptr; // go back
		}
	}

	CN3CPart* pPart = nullptr;
	if(szFN.empty()) // If there is no file name.. Default wear..
	{
		if(PART_POS_HAIR_HELMET == ePos)
		{
			this->InitHair();
			pPart = m_Chr.Part(ePos);
		}
		else if(PART_POS_FACE == ePos)
		{
			this->InitFace();
			pPart = m_Chr.Part(ePos);
		}
		else
		{
			const __TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks->Find(m_InfoBase.eRace);	// Player Character Skin structure pointer..
			if(pLooks)
			{
				m_ChrInv.PartSet(ePos, pLooks->szPartFNs[ePos]);
				pPart = m_Chr.PartSet(ePos, pLooks->szPartFNs[ePos]);
				if(pPart) pPart->TexOverlapSet("");
			}
		}
	}
	else
	{
		m_ChrInv.PartSet(ePos, szFN);
		pPart = m_Chr.PartSet(ePos, szFN);
	}

	m_pItemPartBasics[ePos] = pItemBasic; // apply item
	m_pItemPartExts[ePos] = pItemExt;

	return pPart;
}

bool CPlayerMySelf::InitChr(__TABLE_PLAYER_LOOKS* pTbl)
{
	CPlayerBase::InitChr(pTbl);

	m_ChrInv.JointSet(pTbl->szJointFN);
	m_ChrInv.AniCtrlSet(pTbl->szAniFN);

	const float fScale = 2.1f / m_Chr.Height();
	m_ChrInv.ScaleSet(fScale, fScale, fScale); // Reduce the size so that it fits in the inventory window.

	return true;
}

float CPlayerMySelf::AttackableDistance(CPlayerBase* pTarget)
{
	if(nullptr == pTarget) return 0;

	float fDistLimit = (m_Chr.Radius() + pTarget->Radius())/2.0f; // Attack distance limit.
	if(m_pItemPlugBasics[0])
		fDistLimit += m_pItemPlugBasics[0]->siAttackRange / 10.0f; // When holding an item...
	else if(m_pItemPlugBasics[1] && ITEM_POS_TWOHANDLEFT == m_pItemPlugBasics[1]->byAttachPoint) // If you hold the two-handed left (bow) item in your left hand...
		fDistLimit += m_pItemPlugBasics[1]->siAttackRange / 10.0f;

	return fDistLimit;
}

float CPlayerMySelf::DistanceExceptRadius(CPlayerBase* pTarget)
{
	if(nullptr == pTarget) return 0;

	const float fDist = (pTarget->Position() - m_Chr.Pos()).Magnitude();
	const float fDistRadius = (m_Chr.Radius() + pTarget->Radius())/2.0f; // Attack distance limit.

	return fDist - fDistRadius;
}

bool CPlayerMySelf::IsAttackableTarget(CPlayerBase* pTarget, bool bMesureAngle)
{
	if(m_fFlickeringFactor != 1.0f) return false;	// When revived and blinking, you cannot attack.
	if(nullptr == pTarget || pTarget->IsDead()) return false;  // A character who is dead cannot attack - except for a character who is about to die.
	if(pTarget->m_InfoBase.eNation == m_InfoBase.eNation) return false; // It&#39;s the same country.

	const float fDist = (pTarget->Position() - m_Chr.Pos()).Magnitude(); // Find the attack distance.
	const float fDistLimit = this->AttackableDistance(pTarget);
	if(fDist > fDistLimit) return false; // If the distance is more than a certain distance, it will return.

	if(bMesureAngle)// look at the angle...
	{
		__Vector3 vDir = this->Direction(); vDir.y = 0; vDir.Normalize();
		__Vector3 vDirTarget = pTarget->Position() - m_Chr.Pos(); vDirTarget.y = 0; vDirTarget.Normalize();
		const float fDot = vDir.Dot(vDirTarget);
		if(fDot > 0.7f) return true;
		else return false;
	}

	return true;
}

bool CPlayerMySelf::CheckCollision()
{
	float fSpeed = m_fMoveSpeedPerSec * CN3Base::s_fSecPerFrm; // Current moving speed.. This is the speed calculated according to the frame.
	if(0 == fSpeed) return false; // If it doesn&#39;t move, don&#39;t check for collision.

	const __Vector3 vPos = this->Position();

	__Vector3 vDir(0,0,1); // Find the rotation value.
	const __Matrix44 mtxRot = this->Rotation(); 
	vDir *= mtxRot; // Find the direction of rotation.
	if(fSpeed < 0)
	{
		fSpeed *= -1.0f;
		vDir *= -1.0f;
	}
	const __Vector3 vPosNext = vPos + (vDir * fSpeed); // Calculate next position...
	if ( false == ACT_WORLD->IsInTerrainWithTerrain(vPosNext.x, vPosNext.z, vPos) )
		return true; // If you are not within the boundaries...


	// 
	// Check with other players...
	float fHeightSum, fMag;
	auto it = s_pOPMgr->m_UPCs.begin(), itEnd = s_pOPMgr->m_UPCs.end();
	for(; it != itEnd; it++)
	{
		CPlayerOther* pUPC = it->second;

		if(pUPC->IsDead()) continue; // A dead character does not perform collision checks.
		if(m_InfoBase.eNation == pUPC->m_InfoBase.eNation) continue; // same country...
		
		fMag = (pUPC->Position() - vPos).Magnitude();
		if(fMag < 32.0f)
		{
			fHeightSum = (pUPC->Height() + m_Chr.Height()) / 2.5f;
			if(fMag < fHeightSum) // If the distance is less than the sum of the heights...
			{
				const float fMag2 = (pUPC->Position() - vPosNext).Magnitude(); // If the next location is closer.
				if(fMag2 < fMag)
					return true;
			}
		}
	}
	// Check with other players...
	// 

	// __TABLE_ZONE* pZoneInfo = s_pTbl_Zones->Find(m_InfoExt.iZoneCur);
	// if(pZoneInfo && pZoneInfo->bNPCCollisionCheck) //this_zone

	// The enemy NPC performs a collision check.
	auto it_N = s_pOPMgr->m_NPCs.begin(),	it_NEnd	= s_pOPMgr->m_NPCs.end();
	for(; it_N != it_NEnd; it_N++)
	{
		CPlayerNPC* pNPC = it_N->second;

		if(pNPC->m_pShapeExtraRef) continue; // If it is in the form of a castle gate, collision check is not performed.
		if(m_InfoBase.eNation == pNPC->m_InfoBase.eNation) continue; // same country...
		if(m_InfoBase.eNation == NATION_KARUS && pNPC->m_InfoBase.eNation != NATION_ELMORAD) continue; // The enemy NPC performs a collision check.
		if(m_InfoBase.eNation == NATION_ELMORAD && pNPC->m_InfoBase.eNation != NATION_KARUS) continue; // 

		fMag = (pNPC->Position() - vPos).Magnitude();
		if(fMag < 32.0f)
		{
			fHeightSum = (pNPC->Height() + m_Chr.Height()) / 2.5f;
			if(fMag < fHeightSum) // If the distance is less than the sum of the heights...
			{
				const float fMag2 = (pNPC->Position() - vPosNext).Magnitude(); // If the next location is closer.
				if(fMag2 < fMag)
					return true;
			}
		}
	}// for(


	// Collision check with object.
	__Vector3 vPos2 = vPos, vCol, vNormal;
	if (!s_pWorldMgr->IsIndoor())
		vPos2.y += 0.5f; // Collision checks 0.5 meters above the height of the character&#39;s feet.
	else
		vPos2.y += 0.6f; // Collision checks 0.6 meters above the height of the character&#39;s feet. The 0.6 written inside this function is PvsMgr&#39;s m_fVolumeOffs.. ^^
	const bool bColShape = ACT_WORLD->CheckCollisionWithShape(vPos2, vDir, fSpeed, &vCol, &vNormal);
	if( bColShape) return true; // If there is a collision value with the object

	// 
	// Get the height value of the ground and the object..
	const float fYTerrain = ACT_WORLD->GetHeightWithTerrain(vPosNext.x, vPosNext.z);		// The height of the ground...
	const float fYClimb = ACT_WORLD->GetHeightNearstPosWithShape(vPosNext, CN3Base::s_fSecPerFrm * 30.0f, &vNormal); // The height value of the collision check object..
	vNormal.y = 0; // This way you can get a normal slope.
	
	if (!s_pWorldMgr->IsIndoor())
	{
		if(fYClimb > fYTerrain && fYClimb < vPosNext.y + ((30.0f/CN3Base::s_fFrmPerSec) * 0.5f)) // The height value is applied only when there is a collision check object height value and it is higher than the terrain
		{
			if(vNormal.Magnitude() > MAX_INCLINE_CLIMB && vNormal.Dot(vDir) <= 0.0f) // Slope check...
			{
				return true;
			}
			m_fYNext = fYClimb;
		}
		else
		{
			// Check if the slope of the terrain is less than 45 degrees
			if(true == ACT_WORLD->CheckInclineWithTerrain(vPosNext, vDir, MAX_INCLINE_CLIMB))
			{
				return true;
			}
			m_fYNext = fYTerrain; // Position the following...
		}
	}
	else	// first..
	{
		if ((fYClimb > fYTerrain) && (fYClimb < vPosNext.y + 0.6f))
			m_fYNext = fYClimb;
		else
			m_fYNext = fYTerrain; // Position the following...

		if ((m_fYNext > vPos.y + 0.6f) || (m_fYNext < vPos.y - 0.6f*2.0f))
		{
			m_fYNext = vPos.y;
			return true;
		}
	}

	// else // If it is impossible to climb, check the slope with the terrain..
	// {
		// // Find the direction... and apply different speeds according to the slope
		// s_pTerrain-&gt;GetNormal(vPos.x, vPos.z, vNormal);
		// vNormal.Normalize();
		// vNormal.y = 0.0f;
		// float fM = vNormal.Magnitude();
		// float fD = vNormal.Dot(vDir);
		// if(fSpeed < 0) fD *= -1.0f;
		// if(fD &lt; 0) fSpeed *= 1.0f - (fM / 0.7071f); // Apply a factor according to the slope
		//
		// vPosNext = vPos + (vDir * fSpeed); // Calculate the next position...
		// m_fYNext = s_pTerrain->GetHeight(vPosNext.x, vPosNext.z);
	// }

	this->PositionSet(vPosNext, false);

	// 
	// Character collision check.
	// int iSize = s_pOPMgr->m_OPCs.size();
	// it_UPC it = s_pOPMgr->m_OPCs.begin();
	// for(auto i = 0; i < iSize; i++, it++ )
	// {
		// if ( ((*it)->Position() - vPosAfter).Magnitude() < 1.2f )
		// return vPosBefore;
	// }

	return false;
}


void CPlayerMySelf::InitFace()
{
	const __TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks->Find(m_InfoBase.eRace);
	if(pLooks && !pLooks->szPartFNs[PART_POS_FACE].empty()) // If there is an item and a face name...
	{
		char szBuff[256] = "", szDir[128] = "", szFName[128] = "", szExt[16] = "";
		::_splitpath(pLooks->szPartFNs[PART_POS_FACE].c_str(), nullptr, szDir, szFName, szExt);
		sprintf(szBuff, "%s%s%.2d%s", szDir, szFName, m_InfoExt.iFace, szExt);
		this->PartSet(PART_POS_FACE, szBuff, nullptr, nullptr);
	}
}

void CPlayerMySelf::InitHair()
{
	const __TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks->Find(m_InfoBase.eRace);
	if(pLooks && !pLooks->szPartFNs[PART_POS_HAIR_HELMET].empty()) // If there is an item and a face name...
	{
		char szBuff[256] = "", szDir[128] = "", szFName[128] = "", szExt[16] = "";
		::_splitpath(pLooks->szPartFNs[PART_POS_HAIR_HELMET].c_str(), nullptr, szDir, szFName, szExt);
		sprintf(szBuff, "%s%s%.2d%s", szDir, szFName, m_InfoExt.iHair, szExt);
		this->PartSet(PART_POS_HAIR_HELMET, szBuff, nullptr, nullptr);
	}
	else
	{
		m_Chr.PartSet(PART_POS_HAIR_HELMET, "");
		m_ChrInv.PartSet(PART_POS_HAIR_HELMET, "");
	}
}

void CPlayerMySelf::KnightsInfoSet(int iID, const std::string& szName, int iGrade, int iRank)
{
	CPlayerBase::KnightsInfoSet(iID, szName, iGrade, iRank);

	m_InfoExt.iKnightsID = iID;
	m_InfoExt.szKnights = szName;
	m_InfoExt.iKnightsGrade = iGrade;
	m_InfoExt.iKnightsRank = iRank;

	if(m_InfoExt.szKnights.empty()) { if(m_pClanFont) delete m_pClanFont; m_pClanFont = nullptr; }
	else
	{
		if(!m_pClanFont)
		{
			std::string szFontID; ::_LoadStringFromResource(IDS_FONT_ID, szFontID);
			m_pClanFont = new CDFont(szFontID, 12);
			m_pClanFont->InitDeviceObjects( s_lpD3DDev );
			m_pClanFont->RestoreDeviceObjects();
		}

		m_pClanFont->SetText(m_InfoExt.szKnights.c_str(), D3DFONT_BOLD); // Assign text to fonts.
		m_pClanFont->SetFontColor(KNIGHTS_FONT_COLOR);
	}
}

void CPlayerMySelf::SetSoundAndInitFont()
{
	CPlayerBase::SetSoundAndInitFont();
	
	if(m_InfoExt.szKnights.empty()) { delete m_pClanFont; m_pClanFont = nullptr; }
	else
	{
		if(!m_pClanFont)
		{
			std::string szFontID; ::_LoadStringFromResource(IDS_FONT_ID, szFontID);
			m_pClanFont = new CDFont(szFontID, 12);
			m_pClanFont->InitDeviceObjects( s_lpD3DDev );
			m_pClanFont->RestoreDeviceObjects();
		}

		m_pClanFont->SetText(m_InfoExt.szKnights.c_str(), D3DFONT_BOLD); // Assign text to fonts.
		m_pClanFont->SetFontColor(KNIGHTS_FONT_COLOR);
	}
}

float CPlayerMySelf::MoveSpeedCalculationAndCheckCollision() // Get the speed and do a collision check with that speed. If the return value is 0, it is a collision.
{
	m_fMoveSpeedPerSec = MOVE_SPEED_WHEN_WALK;
	if(PSM_RUN == m_eStateMove) m_fMoveSpeedPerSec *= MOVE_DELTA_WHEN_RUNNING;
	else if(PSM_STOP == m_eStateMove) m_fMoveSpeedPerSec = 0.0f;
	else if(PSM_WALK_BACKWARD == m_eStateMove) m_fMoveSpeedPerSec *= -1.0f;
	m_fMoveSpeedPerSec *= m_fMoveDelta; // According to the change in movement speed... Multiply.

	if(m_bTempMoveTurbo) // Flags for development...
	{
		m_fMoveSpeedPerSec *= 10.0f;
	}

	if(this->CheckCollision()) // Collision check...
	{
		if(CGameProcedure::s_pProcMain)
			CGameProcedure::s_pProcMain->CommandMove(MD_STOP, true);
		// m_fMoveSpeedPerSec = 0;
	}

	// Find the direction.. Apply different speeds according to the slope
	/*	__Vector3 vDir = this->Direction();
	__Vector3 vPos = m_Chr.Pos(), vNormal(0,1,0);
	s_pTerrain-&gt;GetNormal(vPos.x, vPos.z, vNormal);
	vNormal.Normalize();
	vNormal.y = 0.0f;
	float fM = vNormal.Magnitude();
	float fD = vNormal.Dot(vDir);
	if(fSpeed < 0) fD *= -1.0f;
	// if (fM > MAX_INCLINE_CLIMB && fD <= 0.0f )
	// {
	// }
	if(fD &lt; 0) fSpeed *= 1.0f - (fM / 0.7071f); // Apply a factor according to the slope
	*/	
	return m_fMoveSpeedPerSec;

}

void CPlayerMySelf::Stun(float fTime) // To stun for a certain amount of time.
{
	m_bStun = true;				// faint..
	m_fStunTime = fTime;		// embarrassing time...

	CGameProcedure::s_pProcMain->CommandEnableAttackContinous(false, nullptr); // stop attacking...
	CGameProcedure::s_pProcMain->CommandMove(MD_STOP, true); // stop moving...
}

void CPlayerMySelf::StunRelease() // Get rid of fainting..
{
	m_bStun = false;			// faint..
	m_fStunTime = 0.0f;			// embarrassing time...
}

void CPlayerMySelf::TargetOrPosMove()
{
	if(!m_bTargetOrPosMove) return;

	m_vTargetPos.y = m_Chr.Pos().y;

	if( m_iMoveTarget >= 0 )
	{
		const CPlayerNPC* pTarget = s_pOPMgr->CharacterGetByID(m_iMoveTarget, true);
		if(pTarget)
		{
			m_vTargetPos = pTarget->Position();
		}
		else
		{
			this->ActionMove(PSM_STOP);
			CGameProcedure::s_pProcMain->MsgSend_Move(false, false); // Send stop packets...
		}
	}

	__Vector3 vDir = m_vTargetPos - s_pPlayer->Position();
	vDir.y = 0.0f;
	vDir.Normalize();

	const float fYaw = ::_Yaw2D(vDir.x, vDir.z);
	this->RotateTo(fYaw, true); // turn

	if( m_iMoveTarget >= 0 )
	{
		CPlayerNPC* pTarget = s_pOPMgr->CharacterGetByID(m_iMoveTarget, true);

		const float fDist = (pTarget->Position() - m_Chr.Pos()).Magnitude(); // Find the attack distance.
		const float fDistLimit = this->AttackableDistance(pTarget);

		if(fDist < fDistLimit)
		{
			CGameProcedure::s_pProcMain->CommandEnableAttackContinous(true, pTarget);
			this->ActionMove(PSM_STOP);
			CGameProcedure::s_pProcMain->MsgSend_Move(false, false); // Send stop packets...
		}
	}
	else
	{
		const float fDist = (m_vTargetPos - Position()).Magnitude();
		if( fDist < 0.5f )
		{
			this->ActionMove(PSM_STOP);
			CGameProcedure::s_pProcMain->MsgSend_Move(false, false); // Send stop packets..
		}
	}
}
