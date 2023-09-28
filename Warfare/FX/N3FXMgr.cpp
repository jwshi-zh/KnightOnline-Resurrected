#include "pch.h"
#include "N3FXMgr.h"
#include "GameBase.h"
#include "GameProcmain.h"
#include "GameProcedure.h"
#include "PlayerOtherMgr.h"
#include "PlayerNPC.h"
#include "PlayerMySelf.h"
#include "N3FXBundleGame.h"
#include "N3WorldManager.h"
#include "MagicSkillMng.h"
#include "APISocket.h"
#include "PacketDef.h"
#include "N3ShapeExtra.h"

CN3FXMgr::CN3FXMgr()
{
	m_fOriginLimitedTime = 60.0f;
}

CN3FXMgr::~CN3FXMgr()
{
	for(auto it=m_ListBundle.begin(); it!=m_ListBundle.end(); it++)
	{
		delete (*it);
	}
	m_ListBundle.clear();

	for(auto itOrigin=m_OriginBundle.begin(); itOrigin!=m_OriginBundle.end(); itOrigin++)
	{
		const LPFXBUNDLEORIGIN pSrc = itOrigin->second;
		delete pSrc->pBundle;
		delete pSrc;
	}
	m_OriginBundle.clear();	
}


//
//
//
void CN3FXMgr::TriggerBundle(int SourceID, int SourceJoint, int FXID, int TargetID, int Joint, int idx, int MoveType)
{
	const __TABLE_FX* pFX = s_pTbl_FXSource->Find(FXID);
	if(!pFX) return;

	char buff[MAX_PATH];
	sprintf(buff, pFX->szFN.c_str());
	_strlwr(buff);
	std::string strTmp = buff;


	const auto itOrigin = m_OriginBundle.find(strTmp);
	
	if(itOrigin != m_OriginBundle.end())	// has the same effect...
	{
		const LPFXBUNDLEORIGIN pSrc = itOrigin->second;

		auto* pBundle = new CN3FXBundleGame;
		
		pBundle->SetPreBundlePos(SourceID, Joint);
		pSrc->pBundle->Duplicate(pBundle);
		pBundle->m_iID = FXID;
		pBundle->m_iIdx = idx;
		pBundle->m_iMoveType = MoveType;
		pBundle->m_iSourceJoint = SourceJoint;

		pBundle->Trigger(SourceID, TargetID, Joint, pFX->dwSoundID);
		
		m_ListBundle.push_back(pBundle);
		pSrc->iNum++;
	}
	else	// doesn't have the same effect.
	{
		auto pSrc = new FXBUNDLEORIGIN;
		pSrc->pBundle = new CN3FXBundleGame;
		pSrc->pBundle->LoadFromFile(strTmp);

		auto* pBundle = new CN3FXBundleGame;

		pBundle->SetPreBundlePos(SourceID, Joint);
		pSrc->pBundle->Duplicate(pBundle);	
		pBundle->m_iID = FXID;
		pBundle->m_iIdx = idx;
		pBundle->m_iMoveType = MoveType;
		pBundle->m_iSourceJoint = SourceJoint;

		pBundle->Trigger(SourceID, TargetID, Joint, pFX->dwSoundID);
		
		m_ListBundle.push_back(pBundle);
		
		pSrc->iNum++;
		m_OriginBundle.insert(std::make_pair(strTmp, pSrc));
	}
}


//
//
//
void CN3FXMgr::TriggerBundle(int SourceID, int SourceJoint, int FXID, __Vector3 TargetPos, int idx, int MoveType)
{
	const __TABLE_FX* pFX = s_pTbl_FXSource->Find(FXID);
	if(!pFX) return; 

	char buff[MAX_PATH];
	sprintf(buff, pFX->szFN.c_str());
	_strlwr(buff);
	std::string strTmp = buff;

	const auto itOrigin = m_OriginBundle.find(strTmp);

	if(itOrigin != m_OriginBundle.end())	// has the same effect...
	{
		const LPFXBUNDLEORIGIN pSrc = itOrigin->second;
		auto* pBundle = new CN3FXBundleGame;
				
		pBundle->SetPreBundlePos(SourceID, SourceJoint);
		pSrc->pBundle->Duplicate(pBundle);
		pBundle->m_iID = FXID;
		pBundle->m_iIdx = idx;
		pBundle->m_iMoveType = MoveType;
		pBundle->m_iSourceJoint = SourceJoint;

		pBundle->Trigger(SourceID, TargetPos, pFX->dwSoundID);		
		m_ListBundle.push_back(pBundle);
		pSrc->iNum++;
	}
	else	// doesn't have the same effect.
	{
		auto pSrc = new FXBUNDLEORIGIN;
		pSrc->pBundle = new CN3FXBundleGame;
		pSrc->pBundle->LoadFromFile(pFX->szFN);

		auto* pBundle = new CN3FXBundleGame;

		pBundle->SetPreBundlePos(SourceID, SourceJoint);
		pSrc->pBundle->Duplicate(pBundle);	
		pBundle->m_iID = FXID;
		pBundle->m_iIdx = idx;
		pBundle->m_iMoveType = MoveType;
		pBundle->m_iSourceJoint = SourceJoint;

		pBundle->Trigger(SourceID, TargetPos, pFX->dwSoundID);
		m_ListBundle.push_back(pBundle);

		pSrc->iNum++;
		m_OriginBundle.insert(std::make_pair(strTmp, pSrc));
	}
}


//
//
//
void CN3FXMgr::Stop(int SourceID, int TargetID, int FXID, int idx, bool immediately)
{
	CN3FXBundleGame* pBundle;
	if(FXID < 0)
	{
		auto it = m_ListBundle.begin();	
		while(it!=m_ListBundle.end())
		{
			pBundle = (*it);
			if(!pBundle)
			{
				it = m_ListBundle.erase(it);
				continue;
			}
			if(pBundle->m_iSourceID==SourceID && pBundle->m_iIdx==idx)
			{
				pBundle->Stop(immediately);
			}
			it++;
		}
	}
	else
	{
		auto it = m_ListBundle.begin();	
		while(it!=m_ListBundle.end())
		{
			pBundle = (*it);
			if(!pBundle)
			{
				it = m_ListBundle.erase(it);
				continue;
			}
			if(pBundle->m_iSourceID==SourceID && pBundle->m_iID == FXID && pBundle->m_iIdx==idx)
			{
				pBundle->Stop(immediately);
			}
			it++;
		}
	}
}

//
//
//
void CN3FXMgr::SetBundlePos(int FXID, int idx, __Vector3& vPos)
{
	auto it = m_ListBundle.begin();
	CN3FXBundleGame* pBundle;
	while(it!=m_ListBundle.end())
	{
		pBundle = (*it);
		if(pBundle && pBundle->m_iID == FXID && pBundle->m_iIdx==idx)
		{
			pBundle->m_vDestPos = vPos;
			return;
		}
		it++;
	}
}

//
//
//
void CN3FXMgr::StopMine()
{
	CN3FXBundleGame* pBundle;

	auto it = m_ListBundle.begin();	
	while(it!=m_ListBundle.end())
	{
		pBundle = (*it);
		if(!pBundle)
		{
			it = m_ListBundle.erase(it);
			continue;
		}
		if(pBundle->m_iSourceID==CGameBase::s_pPlayer->IDNumber())
		{
			pBundle->Stop(true);
		}
		it++;
	}
}


//
//
//
void CN3FXMgr::Tick()
{
	auto itOrigin = m_OriginBundle.begin();
	while(itOrigin!=m_OriginBundle.end())
	{
		LPFXBUNDLEORIGIN pSrc = itOrigin->second;
		if(pSrc && pSrc->iNum<=0)
		{
			pSrc->fLimitedTime += CN3Base::s_fSecPerFrm;
			if(pSrc->fLimitedTime > m_fOriginLimitedTime)
			{
				if(pSrc->pBundle) delete pSrc->pBundle;
				delete pSrc;
				
				itOrigin = m_OriginBundle.erase(itOrigin);
				continue;
			}
		}
		itOrigin++;
	}
	// TRACE("Origin Bundle Count : %d \n", m_OriginBundle.size());
	// TRACE("Number of currently used effects: %d \n", m_ListBundle.size());

	auto it = m_ListBundle.begin();
	// std::list<CN3FXBundleGame*>::iterator it = m_ListBundle.begin();
	while(it!=m_ListBundle.end())
	{
		CN3FXBundleGame* pBundle = (*it);
		if(!pBundle)
		{
			it = m_ListBundle.erase(it);
			continue;
		}
		if(pBundle->m_dwState==FX_BUNDLE_STATE_DEAD)
		{
			auto itOrigin = m_OriginBundle.find(pBundle->FileName());
			if(itOrigin!=m_OriginBundle.end())	// has the same effect...
			{
				LPFXBUNDLEORIGIN pSrc = itOrigin->second;
				pSrc->iNum--;
			}
			// else
			// {
			// TRACE("Invalid Bundle-.- \n");
			// }
			
			delete pBundle;
			it = m_ListBundle.erase(it);
			continue;
		}		
		
		// I shot...
		// After checking if pBundle->m_iMoveType and alive
		// The field of view test is on hold...if so...not with the view frustum...
		// Get the distance between the player and the obj, and if it's over a certain distance, get rid of it.
		// collision check
/*	
		if(pBundle->m_iMoveType != FX_BUNDLE_MOVE_NONE &&
			pBundle->m_dwState==FX_BUNDLE_STATE_LIVE &&
			( s_pPlayer->IDNumber()==pBundle->m_iSourceID ||
			( s_pPlayer->IDNumber()==pBundle->m_iTargetID &&
			s_pOPMgr->UPCGetByID(pBundle->m_iSourceID, true)==NULL)))
*/
		if(pBundle->m_iMoveType != FX_BUNDLE_MOVE_NONE && pBundle->m_dwState==FX_BUNDLE_STATE_LIVE)
		{
			if(s_pOPMgr->UPCGetByID(pBundle->m_iSourceID, true)== nullptr &&
				s_pOPMgr->NPCGetByID(pBundle->m_iSourceID, true)== nullptr &&
				s_pPlayer->IDNumber()!=pBundle->m_iSourceID	)
			{
				pBundle->Stop();
			}

			DWORD dwToMe = 0; // If dwToMe==1, I shot.. If dwToMe==2, I am the target..
			if(s_pPlayer->IDNumber()==pBundle->m_iSourceID) dwToMe = 1;
			else if( s_pPlayer->IDNumber()==pBundle->m_iTargetID &&
					s_pOPMgr->UPCGetByID(pBundle->m_iSourceID, true)== nullptr) dwToMe = 2;
			
			if(dwToMe == 1 || dwToMe == 2)
			{
				__Vector3 vCol;
				float fHeight = 0.5f;

				// Collision check with npc or player..
				bool bCol = false;
				auto it = s_pOPMgr->m_UPCs.begin();
				auto itEnd = s_pOPMgr->m_UPCs.end();
				CPlayerOther* pUPC = nullptr;

				if( dwToMe==2 && ((pBundle->m_vPos - s_pPlayer->Position()).Magnitude()<16.0f) )
				{
					__Vector3 vCp = s_pPlayer->Center();
					if( true == s_pPlayer->CheckCollisionByBox(pBundle->m_vPos, pBundle->m_vPos + pBundle->m_vDir*pBundle->m_fVelocity*CN3Base::s_fSecPerFrm, &vCol, nullptr) )
					// if( true == CheckCollisionSphere(pBundle->m_vPos, pBundle->m_vPos + pBundle->m_vDir*pBundle->m_fVelocity*CN3Base::s_fSecPerFrm, vCp, s_pPlayer->Radius(), &vCol) )
					{
						bCol = true;
						pBundle->m_vPos = vCol;
						pBundle->Stop();
						int iMagicID = CGameProcedure::s_pProcMain->m_pMagicSkillMng->GetMagicID(pBundle->m_iIdx);
						
						BYTE byBuff[32];
						int iOffset=0;
						CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
						CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
						CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)s_pPlayer->IDNumber());

						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						
						CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);	// ?
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						
						CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

						iOffset=0;
						CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
						CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_FAIL);
						CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)s_pPlayer->IDNumber());

						CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.x);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.y);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.z);

						CAPISocket::MP_AddShort(byBuff, iOffset, (short)SKILLMAGIC_FAIL_KILLFLYING);			
						CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);					
						
						CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

						break;
					}
				}

				for(; it != itEnd; it++)
				{
					pUPC = it->second;
					if(dwToMe == 1 && pUPC->m_InfoBase.eNation==s_pPlayer->m_InfoBase.eNation) continue;
					
					if((pBundle->m_vPos - pUPC->Position()).Magnitude() > 16.0f) continue; // Being more than 16 meters away passes by.

					__Vector3 vCp = pUPC->Center();
					// if( true == CheckCollisionSphere(pBundle->m_vPos, pBundle->m_vPos + pBundle->m_vDir*pBundle->m_fVelocity*CN3Base::s_fSecPerFrm, vCp, pUPC->Radius(), &vCol) )
					if ( true == pUPC->CheckCollisionByBox(pBundle->m_vPos, pBundle->m_vPos + pBundle->m_vDir*pBundle->m_fVelocity*CN3Base::s_fSecPerFrm, &vCol, nullptr) )
					{
						bCol = true;
						pBundle->m_vPos = vCol;
						pBundle->Stop();
						int iMagicID = CGameProcedure::s_pProcMain->m_pMagicSkillMng->GetMagicID(pBundle->m_iIdx);
						
						BYTE byBuff[32];
						int iOffset=0;
						CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
						CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
						CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)pUPC->IDNumber());

						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						
						CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);	// ?
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						
						CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

						iOffset=0;
						CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
						CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_FAIL);
						CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)pUPC->IDNumber());

						CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.x);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.y);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.z);

						CAPISocket::MP_AddShort(byBuff, iOffset, (short)SKILLMAGIC_FAIL_KILLFLYING);			
						CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);					
						
						CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

						break;
					}
				}

				if(false == bCol)
				{
					__Vector3 vNext = pBundle->m_vPos + pBundle->m_vDir * (pBundle->m_fVelocity * CN3Base::s_fSecPerFrm * 1.2f);

					auto it2 = s_pOPMgr->m_NPCs.begin();
					auto itEnd2 = s_pOPMgr->m_NPCs.end();
					CPlayerNPC* pSNPC = s_pOPMgr->NPCGetByID(pBundle->m_iSourceID, FALSE);
					CPlayerNPC* pNPC = nullptr;
					for(; it2 != itEnd2; it2++)
					{
						pNPC = (*it2).second;

						if(dwToMe == 1 && pNPC->m_InfoBase.eNation==s_pPlayer->m_InfoBase.eNation) continue;
						else if(pSNPC && dwToMe == 2 && pSNPC->m_InfoBase.eNation == pNPC->m_InfoBase.eNation) continue;

						if((pBundle->m_vPos - pNPC->Position()).Magnitude() > 16.0f) continue; // Being more than 16 meters away passes by.

						if(pNPC->IDNumber()==pBundle->m_iTargetID && pNPC->m_pShapeExtraRef)
						{
							__Vector3 vMin = pNPC->m_pShapeExtraRef->Min();
							__Vector3 vMax = pNPC->m_pShapeExtraRef->Max();
							__Vector3 vDestPos = vMin + ((vMax-vMin)*0.5f);

							float fDistTmp = pBundle->m_fVelocity * CN3Base::s_fSecPerFrm * 1.2f;
							
							if((pBundle->m_vPos - vDestPos).Magnitude() <= fDistTmp)
							{
								bCol = true;
								pBundle->Stop();
								int iMagicID = CGameProcedure::s_pProcMain->m_pMagicSkillMng->GetMagicID(pBundle->m_iIdx);
								
								BYTE byBuff[32];
								int iOffset=0;
								CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
								CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
								CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
								CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
								CAPISocket::MP_AddShort(byBuff, iOffset, (short)pNPC->IDNumber());

								CAPISocket::MP_AddShort(byBuff, iOffset, 0);
								CAPISocket::MP_AddShort(byBuff, iOffset, 0);
								CAPISocket::MP_AddShort(byBuff, iOffset, 0);
								
								CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);	// ?
								CAPISocket::MP_AddShort(byBuff, iOffset, 0);
								CAPISocket::MP_AddShort(byBuff, iOffset, 0);
								
								CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
								
								iOffset=0;
								CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
								CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_FAIL);
								CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
								CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
								CAPISocket::MP_AddShort(byBuff, iOffset, (short)pNPC->IDNumber());

								CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_vPos.x);
								CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_vPos.y);
								CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_vPos.z);

								CAPISocket::MP_AddShort(byBuff, iOffset, (short)SKILLMAGIC_FAIL_KILLFLYING);			
								CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);
								CAPISocket::MP_AddShort(byBuff, iOffset, 0);					
								
								CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

								break;
							}
						}

						__Vector3 vCp = pNPC->Center();
						// if( true == CheckCollisionSphere(pBundle->m_vPos, vNext, vCp, pNPC->Radius(), &vCol) )
						if ( true == pNPC->CheckCollisionByBox(pBundle->m_vPos, vNext, &vCol, nullptr) )
						{
							bCol = true;
							pBundle->m_vPos = vCol;
							pBundle->Stop();
							int iMagicID = CGameProcedure::s_pProcMain->m_pMagicSkillMng->GetMagicID(pBundle->m_iIdx);
							
							BYTE byBuff[32];
							int iOffset=0;
							CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
							CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
							CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
							CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
							CAPISocket::MP_AddShort(byBuff, iOffset, (short)pNPC->IDNumber());

							CAPISocket::MP_AddShort(byBuff, iOffset, 0);
							CAPISocket::MP_AddShort(byBuff, iOffset, 0);
							CAPISocket::MP_AddShort(byBuff, iOffset, 0);
							
							CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);	// ?
							CAPISocket::MP_AddShort(byBuff, iOffset, 0);
							CAPISocket::MP_AddShort(byBuff, iOffset, 0);
							
							CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
							
							iOffset=0;
							CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
							CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_FAIL);
							CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
							CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
							CAPISocket::MP_AddShort(byBuff, iOffset, (short)pNPC->IDNumber());

							CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.x);
							CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.y);
							CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.z);

							CAPISocket::MP_AddShort(byBuff, iOffset, (short)SKILLMAGIC_FAIL_KILLFLYING);			
							CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);
							CAPISocket::MP_AddShort(byBuff, iOffset, 0);					
							
							CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

							break;						
						}
					}
				}
				
				// Collision check with object..
				if(bCol == false &&
					true == ACT_WORLD->CheckCollisionWithShape(pBundle->m_vPos, pBundle->m_vDir, pBundle->m_fVelocity * CN3Base::s_fSecPerFrm, &vCol))
				{
					bCol = true;
					pBundle->m_vPos = vCol;

					pBundle->Stop();
					int iMagicID = CGameProcedure::s_pProcMain->m_pMagicSkillMng->GetMagicID(pBundle->m_iIdx);
					
					BYTE byBuff[32];
					int iOffset=0;

					CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
					CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
					CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)-1);

					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.x);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.y);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.z);
					
					CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);	// ?
					CAPISocket::MP_AddShort(byBuff, iOffset, 0);
					CAPISocket::MP_AddShort(byBuff, iOffset, 0);

					CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

					iOffset=0;
					CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
					CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_FAIL);
					CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)-1);

					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.x);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.y);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.z);

					CAPISocket::MP_AddShort(byBuff, iOffset, (short)SKILLMAGIC_FAIL_KILLFLYING);			
					CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);
					CAPISocket::MP_AddShort(byBuff, iOffset, 0);	
					
					CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
				}
				// Terrain and collision check.
				if(bCol == false &&
					ACT_WORLD->CheckCollisionWithTerrain(pBundle->m_vPos, pBundle->m_vDir, pBundle->m_fVelocity, &vCol))
				{
					// crash...
					// I need to drop a packet here...
					bCol = true;
					pBundle->m_vPos = vCol;
					pBundle->Stop();
					int iMagicID = CGameProcedure::s_pProcMain->m_pMagicSkillMng->GetMagicID(pBundle->m_iIdx);
					
					BYTE byBuff[32];
					int iOffset=0;

					CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
					CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
					CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)-1);

					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.x);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.y);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.z);
					
					CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);	// ?
					CAPISocket::MP_AddShort(byBuff, iOffset, 0);
					CAPISocket::MP_AddShort(byBuff, iOffset, 0);

					CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

					iOffset=0;
					CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
					CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_FAIL);
					CAPISocket::MP_AddDword(byBuff, iOffset, iMagicID);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)pBundle->m_iSourceID);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)-1);

					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.x);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.y);
					CAPISocket::MP_AddShort(byBuff, iOffset, (short)vCol.z);

					CAPISocket::MP_AddShort(byBuff, iOffset, (short)SKILLMAGIC_FAIL_KILLFLYING);			
					CAPISocket::MP_AddShort(byBuff, iOffset, pBundle->m_iIdx);
					CAPISocket::MP_AddShort(byBuff, iOffset, 0);
								
					CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
				}
			}
		}			
		pBundle->Tick();
		it++;
	}
}


//
//
//
void CN3FXMgr::Render()
{
	// All sorts of renderstate settings...
	DWORD dwLgt, dwAlpha, dwZEnable; 
	DWORD dwSrcBlend, dwDestBlend;

	s_lpD3DDev->GetRenderState( D3DRS_LIGHTING, &dwLgt );
	s_lpD3DDev->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlpha );
	s_lpD3DDev->GetRenderState( D3DRS_SRCBLEND, &dwSrcBlend );
    s_lpD3DDev->GetRenderState( D3DRS_DESTBLEND, &dwDestBlend );
	s_lpD3DDev->GetRenderState( D3DRS_ZWRITEENABLE, &dwZEnable );

	s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE );
	s_lpD3DDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	
	__Matrix44 mtx;
	mtx.Identity();
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtx);

	const auto itBegin = m_ListBundle.begin();
	const auto itEnd = m_ListBundle.end();
	
	for(auto it=itBegin; it!=itEnd; it++) (*it)->Render();

	s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, dwLgt );
	s_lpD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlpha );
	s_lpD3DDev->SetRenderState( D3DRS_SRCBLEND, dwSrcBlend );
    s_lpD3DDev->SetRenderState( D3DRS_DESTBLEND, dwDestBlend );
	s_lpD3DDev->SetRenderState( D3DRS_ZWRITEENABLE, dwZEnable );
}

bool CN3FXMgr::CheckCollisionSphere(__Vector3 vSp, __Vector3 vEp, __Vector3 vCp, float fRadius, __Vector3* vCol)
{
	const __Vector3 vSpCp = vCp - vSp;
	const float DistSpCp = vSpCp.Magnitude();
	if(DistSpCp <= fRadius)
	{
		(*vCol) = vSp;
		return true;
	}

	__Vector3 vDir = vEp - vSp;
	const float DistSpEp = vDir.Magnitude();
	vDir.Normalize();

	__Vector3 vCross;
	vCross.Cross(vSpCp, vDir);
	const float DistCross = vCross.Magnitude();

	if(DistCross <= fRadius)
	{
		const float sqDistCross = DistCross*DistCross;
		const float DistSpCross = sqrt((DistSpCp*DistSpCp)-sqDistCross);

		if(DistSpCross < DistSpEp)
		{
			const float DistCol = DistSpCross - sqrt((fRadius*fRadius)-sqDistCross);
			
			(*vCol) = vSp + DistCol*vDir;
			return true;
		}
	}
	return false;	
}

void CN3FXMgr::ClearAll()
{
	for(auto it=m_ListBundle.begin(); it!=m_ListBundle.end(); it++)
	{
		const CN3FXBundleGame* pBundle = (*it);
		if(pBundle) delete pBundle;
	}
	m_ListBundle.clear();

	for(auto itOrigin=m_OriginBundle.begin(); itOrigin!=m_OriginBundle.end(); itOrigin++)
	{
		const LPFXBUNDLEORIGIN pSrc = itOrigin->second;
		if(pSrc)
		{
			if(pSrc->pBundle) delete pSrc->pBundle;
			delete pSrc;
		}
	}
	m_OriginBundle.clear();
}
