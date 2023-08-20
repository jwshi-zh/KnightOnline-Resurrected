#include "pch.h"
#include "PlayerOtherMgr.h"

#include "N3ShapeExtra.h"

CPlayerOtherMgr::CPlayerOtherMgr()
{
	m_iChrCountToRender = 0; // Character count rendered
}

CPlayerOtherMgr::~CPlayerOtherMgr()
{
	ReleaseUPCs();
	ReleaseNPCs();
	ReleaseCorpses();
}

void CPlayerOtherMgr::ReleaseUPCs()
{
	auto it = m_UPCs.begin(), itEnd = m_UPCs.end();
	for(; it != itEnd; it++) T_Delete(it->second);
	m_UPCs.clear();
}

void CPlayerOtherMgr::ReleaseNPCs()
{
	auto it = m_NPCs.begin(), itEnd = m_NPCs.end();
	for(; it != itEnd; it++) T_Delete(it->second);
	m_NPCs.clear();
}

void CPlayerOtherMgr::ReleaseCorpses()
{
	auto it = m_Corpses.begin(), itEnd = m_Corpses.end();
	for(; it != itEnd; it++) T_Delete(it->second);
	m_Corpses.clear();
}

void CPlayerOtherMgr::Release()
{
	ReleaseUPCs();
	ReleaseNPCs();
	ReleaseCorpses();
}

void CPlayerOtherMgr::Tick(const __Vector3& vPosPlayer)
{
	// Automatic character LOD adjustment..
	int iLOD = 0;
	int iLODTotal = 0;

	auto it = m_UPCs.begin(), itEnd = m_UPCs.end();
	CPlayerNPC* pNPC = nullptr;
	for(; it != itEnd; )
	{
		pNPC = it->second;
		
		pNPC->Tick();
		iLOD = pNPC->LODLevel();
		if(iLOD >= 0 && iLOD < MAX_CHR_LOD) iLODTotal += MAX_CHR_LOD - iLOD; // Necessary value for automatic LOD calculation..

		const float fDist = pNPC->Distance(vPosPlayer);
		if(fDist < SOUND_RANGE_TO_SET) pNPC->SetSoundAndInitFont(); // If inside SOUND_RANGE.
		else if(fDist > SOUND_RANGE_TO_RELEASE) pNPC->ReleaseSoundAndFont();
		it++;
	}

	auto it2 = m_NPCs.begin(), itEnd2 = m_NPCs.end();
	for(; it2 != itEnd2; )
	{
		pNPC = it2->second;
		
		pNPC->Tick();
		iLOD = pNPC->LODLevel();
		if(iLOD >= 0 && iLOD < MAX_CHR_LOD) iLODTotal += MAX_CHR_LOD - iLOD; // Necessary value for automatic LOD calculation..

		if(PSA_DEATH == pNPC->State())
		{
			it2 = m_NPCs.erase(it2);
			this->CorpseAdd(pNPC);
		}
		else
		{
			const float fDist = pNPC->Distance(vPosPlayer);
			if(fDist < SOUND_RANGE_TO_SET) pNPC->SetSoundAndInitFont(); // If inside SOUND_RANGE.
			else if(fDist > SOUND_RANGE_TO_RELEASE) pNPC->ReleaseSoundAndFont();
			it2++;
		}
	}

	// Dealing with the dead...
	CPlayerBase* pCorpse = nullptr;
	auto it3 = m_Corpses.begin(), itEnd3 = m_Corpses.end();
	for(; it3 != itEnd3; )
	{
		pCorpse = it3->second;
		pCorpse->ReleaseSoundAndFont();

		pCorpse->Tick();
		iLOD = pCorpse->LODLevel();
		if(iLOD >= 0 && iLOD < MAX_CHR_LOD) iLODTotal += MAX_CHR_LOD - iLOD; // Necessary value for automatic LOD calculation..

		if(pCorpse->m_fTimeAfterDeath >= TIME_CORPSE_REMAIN) // After a certain amount of time has passed...
		{
			T_Delete(pCorpse);
			it3 = m_Corpses.erase(it3); // Erase the dead...
		}
		else
		{
			it3++;
		}
	}

	// Automatic character LOD adjustment..
	int iLODDelta = 0;
	if(iLODTotal >= 100) iLODDelta = 3;
	else if(iLODTotal >= 60) iLODDelta = 2;
	else if(iLODTotal >= 30) iLODDelta = 1;
	CN3Chr::LODDeltaSet(iLODDelta);
}



void CPlayerOtherMgr::Render(float fSunAngle)
{

	// CPlayerOther*	pUPC = NULL;
	// it_UPC it = m_UPCs.begin(), itEnd = m_UPCs.end();
	// for(; it != itEnd; it++)
	// {
		// pUPC = it->second;
		// 
		// if(pUPC->m_InfoBase.iAuthority == AUTHORITY_MANAGER)
			// continue;//this_authority
		//
		// pUPC->Render(true, fSunAngle);
	// }

	// Sort by camera distance
	const int iUPCSize = m_UPCs.size();
	if(iUPCSize > 0)
	{
		std::vector<CPlayerOther*> UPCs;
		UPCs.reserve(iUPCSize);
		auto it = m_UPCs.begin(), itEnd = m_UPCs.end();
		for(; it != itEnd; it++) UPCs.push_back(it->second);
		qsort(&(UPCs[0]), UPCs.size(), 4, SortByCameraDistance);

		for(int i = iUPCSize - 1; i >= 0; i--)
		{
			if(UPCs[i]->m_InfoBase.iAuthority == AUTHORITY_MANAGER)
				continue;// this_authority

			UPCs[i]->Render(fSunAngle);
		}
	}
	
	// CPlayerNPC* pNPC = NULL;
	// it_NPC it2 = m_NPCs.begin(), itEnd2 = m_NPCs.end();
	// for(; it2 != itEnd2; it2++)
	// {
		// pNPC = it2->second;
		// 
		// pNPC->Render(true, fSunAngle);
	// }
	// Sort by camera distance
	const int iNPCSize = m_NPCs.size();
	if(iNPCSize > 0)
	{
		std::vector<CPlayerNPC*> NPCs;
		NPCs.reserve(iNPCSize);
		auto it = m_NPCs.begin(), itEnd = m_NPCs.end();
		for(; it != itEnd; it++) NPCs.push_back(it->second);
		qsort(&(NPCs[0]), NPCs.size(), 4, SortByCameraDistance);

		for(int i = iNPCSize - 1; i >= 0; i--)
			NPCs[i]->Render(fSunAngle);
	}


	// Even the dead are rendered...
	// CPlayerBase* pCorpse = NULL;
	// it_NPC it3 = m_Corpses.begin(), itEnd3 = m_Corpses.end();
	// for(; it3 != itEnd3; it3++)
	// {
		// pCorpse = it3->second;
		// pCorpse->Render(false, fSunAngle);
	// }
	// Sort by camera distance
	const int iCorpseSize = m_Corpses.size();
	if(iCorpseSize > 0)
	{
		std::vector<CPlayerNPC*> Corpses;
		Corpses.reserve(iCorpseSize);
		auto it = m_Corpses.begin(), itEnd = m_Corpses.end();
		for(; it != itEnd; it++) Corpses.push_back(it->second);
		qsort(&(Corpses[0]), iCorpseSize, 4, SortByCameraDistance);

		for(int i = iCorpseSize - 1; i >= 0; i--)
		{
			Corpses[i]->m_InfoBase.bRenderID = false;
			Corpses[i]->Render(fSunAngle);
		}
	}
}

CPlayerNPC*	CPlayerOtherMgr::NPCGetByPos(const __Vector3& vPos)
{
	auto it = m_NPCs.begin(), itEnd = m_NPCs.end();
	for(; it != itEnd; it++)
	{
		CPlayerNPC* pNPC = it->second;

		if(pNPC)
		{
			if(pNPC->m_vPosFromServer.x == vPos.x && pNPC->m_vPosFromServer.z == vPos.z)
				return pNPC;
		}
	}

	return nullptr;
}

CPlayerNPC* CPlayerOtherMgr::Pick(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick)
{
	CPlayerNPC* pNPC = this->PickNPC(ixScreen, iyScreen, iIDResult, pvPick);
	if(pNPC) return pNPC;

	CPlayerOther* pUPC = this->PickUPC(ixScreen, iyScreen, iIDResult, pvPick);
	if(pUPC) return pUPC;

	return nullptr;
}

CPlayerOther* CPlayerOtherMgr::PickUPC(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick)
{
	iIDResult = -1;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);

	if(!m_UPCs.empty())
	{
		// Sort by camera distance
		std::vector<CPlayerOther*> UPCs;
		UPCs.reserve(m_UPCs.size());
		auto it = m_UPCs.begin(), itEnd = m_UPCs.end();
		for(; it != itEnd; it++) UPCs.push_back(it->second);
		qsort(&(UPCs[0]), UPCs.size(), 4, SortByCameraDistance);

		for(int i = 0; i < UPCs.size(); i++)
		{
			CPlayerOther* pUPC = UPCs[i];
			if(pUPC->LODLevel() < 0 || pUPC->LODLevel() >= MAX_CHR_LOD) continue; // Anything without Level Of Detail is passing.

			CN3VMesh* pvMesh = pUPC->m_Chr.CollisionMesh();
			if(nullptr != pvMesh && pvMesh->Pick(pUPC->m_Chr.m_Matrix, vPos, vDir, pvPick)) 
			{
				iIDResult = pUPC->IDNumber();
				return pUPC;
			}
		}
	}

	return nullptr;
}

CPlayerNPC* CPlayerOtherMgr::PickNPC(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick)
{
	iIDResult = -1;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);
	
	if(!m_NPCs.empty())
	{
		// Sort by camera distance
		std::vector<CPlayerNPC*> NPCs;
		auto it = m_NPCs.begin(), itEnd = m_NPCs.end();
		NPCs.reserve(m_NPCs.size());
		for(; it != itEnd; it++) NPCs.push_back(it->second);
		qsort(&(NPCs[0]), NPCs.size(), 4, SortByCameraDistance);

		// Let&#39;s take a picture of the NPC first...
		for(int i = 0; i < NPCs.size(); i++)
		{
			CPlayerNPC* pNPC = NPCs[i];
			if(pNPC->LODLevel() < 0 || pNPC->LODLevel() >= MAX_CHR_LOD) continue; // Anything without Level Of Detail is passing.

			CN3VMesh* pvMesh = nullptr;
			const __Matrix44* pMtx = nullptr;
			if(pNPC->m_pShapeExtraRef && pNPC->m_pShapeExtraRef->m_bVisible)
			{
				pvMesh = pNPC->m_pShapeExtraRef->CollisionMesh();
				pMtx = &(pNPC->m_pShapeExtraRef->m_Matrix);
			}
			if(nullptr == pvMesh)
			{
				pvMesh = pNPC->m_Chr.CollisionMesh();
				pMtx = &(pNPC->m_Chr.m_Matrix);
			}

			if(nullptr == pvMesh) continue;

			const bool bPick = pvMesh->Pick(*pMtx, vPos, vDir, pvPick);
			if(bPick)
			{
				iIDResult = pNPC->IDNumber();
				return pNPC;
			}
		}
	}

	return nullptr;
}

CPlayerNPC* CPlayerOtherMgr::PickCorpse(int ixScreen, int iyScreen, int& iIDResult)
{
	iIDResult = -1;
	if(m_Corpses.empty()) return nullptr;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);


	// Sort by camera distance
	std::vector<CPlayerNPC*> Corpses;
	Corpses.reserve(m_Corpses.size());
	auto it = m_Corpses.begin(), itEnd = m_Corpses.end();
	for(; it != itEnd; it++) Corpses.push_back(it->second);
	qsort(&(Corpses[0]), Corpses.size(), 4, SortByCameraDistance);

	for(int i = 0; i < Corpses.size(); i++)
	{
		CPlayerNPC* pCorpse = Corpses[i];
		if(pCorpse->LODLevel() < 0 || pCorpse->LODLevel() >= MAX_CHR_LOD) continue; // Anything without Level Of Detail is passing.

		CN3VMesh* pvMesh = pCorpse->m_Chr.CollisionMesh();
		if(nullptr != pvMesh && pvMesh->Pick(pCorpse->m_Chr.m_Matrix, vPos, vDir)) 
		{
			iIDResult = pCorpse->IDNumber();
			return pCorpse;
		}
	}

	return nullptr;
}

bool CPlayerOtherMgr::IsValidCharacter(CPlayerBase *pCharacter)
{
	auto it = m_UPCs.begin(), itEnd = m_UPCs.end();
	for(; it != itEnd; it++)
	{
		const CPlayerOther* pUPC = it->second;
		if(pCharacter == pUPC) return true;
	}

	auto it2 = m_NPCs.begin(), itEnd2 = m_NPCs.end();
	for(; it2 != itEnd2; it2++)
	{
		const CPlayerNPC* pNPC = it2->second;
		if(pCharacter == pNPC) return true;
	}

	auto it3 = m_Corpses.begin(), itEnd3 = m_Corpses.end();
	for(; it3 != itEnd3; it3++)
	{
		const CPlayerBase* pCorpse = it3->second;
		if(pCharacter == pCorpse) return true;
	}

	return false;
}


void CPlayerOtherMgr::CorpseRemove(CPlayerNPC *pCorpse, bool bRemoveImmediately)
{
	if(nullptr == pCorpse) return;
	if(pCorpse->m_fTimeAfterDeath >= TIME_CORPSE_REMAIN - TIME_CORPSE_REMOVE) return;

	if(bRemoveImmediately)
		pCorpse->m_fTimeAfterDeath = TIME_CORPSE_REMAIN; // Increases the dead time and removes it immediately.
	else
		pCorpse->m_fTimeAfterDeath = TIME_CORPSE_REMAIN - TIME_CORPSE_REMOVE; // Now it&#39;s time to disappear. The rest is done in Tick.
}

void CPlayerOtherMgr::CorpseAdd(CPlayerNPC* pNPC)
{
	if(nullptr == pNPC) return;
	const auto result = m_Corpses.insert(std::make_pair(pNPC->IDNumber(), pNPC));
	if(false == result.second) // If duplicate...
	{
		T_Delete(result.first->second); // Erase the previous one...
		result.first->second = pNPC; // Insert a new pointer...
	}
}

CPlayerNPC*	CPlayerOtherMgr::CorpseGetNearstNPC(bool bMustHaveItem, e_Nation eNation, const __Vector3& vPosPlayer) // Get the nearest enemy corpse...
{
	CPlayerNPC* pTarget = nullptr;
	float fDistMin = FLT_MAX;

	auto it2 = m_NPCs.begin(), itEnd2 = m_NPCs.end();
	for(; it2 != itEnd2; it2++)
	{
		CPlayerNPC* pNPC = it2->second;
		if(eNation == pNPC->m_InfoBase.eNation) continue;
		if(bMustHaveItem && pNPC->m_iDroppedItemID <= 0) continue;

		const float fDistTmp = pNPC->Distance(vPosPlayer);
		if(fDistTmp < fDistMin)
		{
			pTarget = pNPC;
			fDistMin = fDistTmp;
		}
	}

	return pTarget;
}

void CPlayerOtherMgr::MoveToCorpsesForcely(CPlayerNPC* pNPC, bool bErase)
{
	if(nullptr == pNPC) return;

	const int iID = pNPC->IDNumber();
	pNPC->Action(PSA_DEATH, false, nullptr, true); // forced to kill
	if(bErase) pNPC->m_fTimeAfterDeath = TIME_CORPSE_REMAIN - 10.0f; // Set dead time...
	else pNPC->m_fTimeAfterDeath = 0.1f;

	const auto it = m_UPCs.find(iID); // Search for User...
	if(it != m_UPCs.end())
	{
		if( bErase ) // When it is deleted, it becomes a corpse
		{// Due to duplication, the character becomes a corpse.
			CPlayerOther* pUPC = it->second;
			this->CorpseAdd(pUPC); // make a body...
			m_UPCs.erase(it); // erase it from the map
		}

		// CPlayerOther* pUPC = it->second;
		// this-&gt;CorpseAdd(pUPC); // make it a corpse...
		// m_UPCs. erase(it); // clear from map
	}
	else
	{
		const auto it2 = m_NPCs.find(iID);
		if(it2 != m_NPCs.end())
		{
			CPlayerNPC* pNPC = it2->second; 
			this->CorpseAdd(pNPC); // make a body...
			m_NPCs.erase(it2); // erase it from the map
		}
	}
}

CPlayerNPC*	CPlayerOtherMgr::CharacterGetByNearstEnemy(e_Nation eNation, const __Vector3& vPosPlayer) // Get the nearest enemy...
{
	CPlayerNPC* pTarget = nullptr;
	float fDistMin = FLT_MAX, fDistTmp = 0;

	auto it = m_UPCs.begin(), itEnd = m_UPCs.end();
	CPlayerNPC* pNPC = nullptr;
	for(; it != itEnd; it++)
	{
		pNPC = it->second;
		if(eNation == pNPC->m_InfoBase.eNation) continue;
		
		fDistTmp = pNPC->Distance(vPosPlayer);
		if(fDistTmp < fDistMin)
		{
			pTarget = pNPC;
			fDistMin = fDistTmp;
		}
	}

	auto it2 = m_NPCs.begin(), itEnd2 = m_NPCs.end();
	for(; it2 != itEnd2; it2++)
	{
		pNPC = it2->second;
		if(eNation == pNPC->m_InfoBase.eNation) continue;
		
		fDistTmp = pNPC->Distance(vPosPlayer);
		if(fDistTmp < fDistMin)
		{
			pTarget = pNPC;
			fDistMin = fDistTmp;
		}
	}

	return pTarget;
}

bool CPlayerOtherMgr::CharacterDelete(int iID) // Users, NPCs are not covered and erased..
{
	const auto it = m_UPCs.find(iID); // Search for User...
	if(it != m_UPCs.end())
	{
		const CPlayerOther* pUPC = it->second;
		delete pUPC;
		m_UPCs.erase(it); // erase it from the map
		return true;
	}

	const auto it2 = m_NPCs.find(iID);
	if(it2 != m_NPCs.end())
	{
		const CPlayerNPC* pNPC = it2->second; 
		delete pNPC;
		m_NPCs.erase(it2); // erase it from the map
		return true;
	}

	return false;
}


int CPlayerOtherMgr::SortByCameraDistance(const void* pArg1, const void* pArg2)
{
	const CPlayerBase* pPlayer1 = *((CPlayerBase**)pArg1);
	const CPlayerBase* pPlayer2 = *((CPlayerBase**)pArg2);

	const float fDist1 = (CN3Base::s_CameraData.vEye - pPlayer1->Position()).Magnitude();
	const float fDist2 = (CN3Base::s_CameraData.vEye - pPlayer2->Position()).Magnitude();

	if(fDist1 < fDist2) return -1; // true if close;
	else if(fDist1 > fDist2) return 1;
	else return 0;
}

void CPlayerOtherMgr::CorpseAdd(int iID)
{
	const auto it = m_UPCs.find(iID); // Search for User...
	if(it != m_UPCs.end())
	{
		CPlayerOther* pUPC = it->second;

		pUPC->Action(PSA_DEATH, false, nullptr, true); // forced to kill
		pUPC->m_fTimeAfterDeath = TIME_CORPSE_REMAIN - 10.0f; // Set dead time...

		this->CorpseAdd(pUPC); // make a body...
		m_UPCs.erase(it); // erase it from the map
	}
}

CPlayerNPC* CPlayerOtherMgr::PickAllPrecisely(int ixScreen, int iyScreen, int &iIDResult, __Vector3* pvPick)
{
	iIDResult = -1;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);


	CPlayerNPC* pNPC = nullptr; // Character pointer rather than NPC

	// Sort by camera distance
	std::vector<CPlayerNPC*> NPCs;
	std::vector<CPlayerNPC*> NUPCBufs;
	if(!m_NPCs.empty() || !m_UPCs.empty())
	{
		auto it = m_NPCs.begin(), itEnd = m_NPCs.end();
		auto it_u = m_UPCs.begin(), itEnd_u = m_UPCs.end();
		NPCs.reserve(m_NPCs.size()+m_UPCs.size());
		for(; it != itEnd; it++) NPCs.push_back(it->second);
		for(; it_u != itEnd_u; it_u++) NPCs.push_back(it_u->second);
		qsort(&(NPCs[0]), NPCs.size(), 4, SortByCameraDistance);
	}
	else
	{
		return nullptr;
	}


	int iCnt = 0;
	for(int i = 0; i < NPCs.size(); i++)
	{
		pNPC = NPCs[i];
		if(pNPC == nullptr) continue;
		if(pNPC->LODLevel() < 0 || pNPC->LODLevel() >= MAX_CHR_LOD) continue; // Anything without Level Of Detail is passing.

		CN3VMesh* pvMesh = nullptr;
		const __Matrix44* pMtx = nullptr;
		if(pNPC->m_pShapeExtraRef && pNPC->m_pShapeExtraRef->m_bVisible)
		{
			pvMesh = pNPC->m_pShapeExtraRef->CollisionMesh();
			pMtx = &(pNPC->m_pShapeExtraRef->m_Matrix);
			if(nullptr == pvMesh)
			{
				pvMesh = pNPC->m_Chr.CollisionMesh();
				pMtx = &(pNPC->m_Chr.m_Matrix);
			}

			if(nullptr == pvMesh) continue;
			const bool bPick = pvMesh->Pick(*pMtx, vPos, vDir, pvPick);
			if(bPick)
			{
				NUPCBufs.push_back(pNPC);
			}
			continue;
		}
		else
		{
			pvMesh = pNPC->m_Chr.CollisionMesh();
			pMtx = &(pNPC->m_Chr.m_Matrix);

			if(nullptr == pvMesh) continue;
			const bool bPick = pvMesh->Pick(*pMtx, vPos, vDir, pvPick);
			if(bPick)
			{
				NUPCBufs.push_back(pNPC);
			}
			continue;
		}
	}
	
	// 
	// If the NPC and UPC are separately checked for collision, even if the UPC is close to me,
	// Since the NPCs behind are found first, align the UPC and NPCs to the camera at the same time.
	// You need to do a collision check to find the correct character.
	// 

	const int iBufCnt = NUPCBufs.size();

	if(iBufCnt < 0) return nullptr;
	if(iBufCnt == 1)
	{
		pNPC = NUPCBufs[0];
		if(pNPC)
		{
			iIDResult = pNPC->IDNumber();
			return pNPC;
		}
	}

	for(auto i = 0; i < iBufCnt; i++)
	{
		pNPC = NUPCBufs[i];
		if(pNPC->LODLevel() < 0 || pNPC->LODLevel() >= MAX_CHR_LOD) continue; // Anything without Level Of Detail is passing.

		CN3VMesh* pvMesh = nullptr;
		__Matrix44* pMtx = nullptr;
		if(pNPC->m_pShapeExtraRef && pNPC->m_pShapeExtraRef->m_bVisible)
		{
			iIDResult = pNPC->IDNumber();
			return pNPC;
		}

		if(pNPC->m_Chr.CheckCollisionPrecisely(ixScreen ,iyScreen, pvPick) != -1)
		{
			iIDResult = pNPC->IDNumber();
			return pNPC;
		}
	}

	return nullptr;
}

CPlayerNPC* CPlayerOtherMgr::PickNPCPrecisely(int ixScreen, int iyScreen, int &iIDResult, __Vector3* pvPick)
{
	iIDResult = -1;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);
	
	if(!m_NPCs.empty())
	{
		// Sort by camera distance
		std::vector<CPlayerNPC*> NPCs;
		auto it = m_NPCs.begin(), itEnd = m_NPCs.end();
		NPCs.reserve(m_NPCs.size());
		for(; it != itEnd; it++) NPCs.push_back(it->second);
		qsort(&(NPCs[0]), NPCs.size(), 4, SortByCameraDistance);

		// Let&#39;s take a picture of the NPC first...
		for(int i = 0; i < NPCs.size(); i++)
		{
			CPlayerNPC* pNPC = NPCs[i];
			if(pNPC->LODLevel() < 0 || pNPC->LODLevel() >= MAX_CHR_LOD) continue; // Anything without Level Of Detail is passing.

			if(pNPC->m_pShapeExtraRef && pNPC->m_pShapeExtraRef->m_bVisible)
			{
				CN3VMesh* pvMesh = pNPC->m_pShapeExtraRef->CollisionMesh();
				const __Matrix44* pMtx = &(pNPC->m_pShapeExtraRef->m_Matrix);
				if(nullptr == pvMesh)
				{
					pvMesh = pNPC->m_Chr.CollisionMesh();
					pMtx = &(pNPC->m_Chr.m_Matrix);
				}

				if(nullptr == pvMesh) continue;
				const bool bPick = pvMesh->Pick(*pMtx, vPos, vDir, pvPick);
				if(bPick)
				{
					iIDResult = pNPC->IDNumber();
					return pNPC;
				}
				continue;
			}

			if(pNPC->m_Chr.CheckCollisionPrecisely(ixScreen ,iyScreen, pvPick) != -1)
			{
				iIDResult = pNPC->IDNumber();
				return pNPC;
			}
		}
	}

	return nullptr;
}

CPlayerOther* CPlayerOtherMgr::PickUPCPrecisely(int ixScreen, int iyScreen, int &iIDResult, __Vector3* pvPick)
{
	iIDResult = -1;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);

	if(!m_UPCs.empty())
	{
		// Sort by camera distance
		std::vector<CPlayerOther*> UPCs;
		UPCs.reserve(m_UPCs.size());
		auto it = m_UPCs.begin(), itEnd = m_UPCs.end();
		for(; it != itEnd; it++) UPCs.push_back(it->second);
		qsort(&(UPCs[0]), UPCs.size(), 4, SortByCameraDistance);

		for(int i = 0; i < UPCs.size(); i++)
		{
			CPlayerOther* pUPC = UPCs[i];
			if(pUPC->LODLevel() < 0 || pUPC->LODLevel() >= MAX_CHR_LOD) continue; // Anything without Level Of Detail is passing.

			if(pUPC->m_Chr.CheckCollisionPrecisely(ixScreen ,iyScreen, pvPick) != -1)
			{
				iIDResult = pUPC->IDNumber();
				return pUPC;
			}
		}
	}

	return nullptr;
}

CPlayerNPC* CPlayerOtherMgr::PickPrecisely(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick)
{
	CPlayerNPC* pNPC = this->PickAllPrecisely(ixScreen, iyScreen, iIDResult, pvPick);
	if(pNPC) return pNPC;

	return nullptr;
}
