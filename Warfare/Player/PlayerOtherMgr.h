#pragma once

#include "PlayerOther.h"
#include "GameBase.h"
#include "GameProcedure.h"

class CPlayerOtherMgr : public CGameBase
{
public:
	// std::list<CPlayerNPC*>		m_NPCs;		// NPC
	// std::list<CPlayerOther*>	m_UPCs;		// User Player Character
	// std::list<CPlayerNPC*> m_Corpses; // dead guy.. dying animation and disappearing over time..
	std::map<int, CPlayerNPC*>		m_NPCs;		// NPC
	std::map<int, CPlayerOther*>	m_UPCs;		// User Player Character
	std::map<int, CPlayerNPC*>		m_Corpses;	// Dead man.. dying animation and disappearing over time..
	int								m_iChrCountToRender; // Character count rendered

public:
	bool				IsValidCharacter(CPlayerBase* pCharacter);
	void				Tick(const __Vector3& vPosPlayer);
	void				Render(float fSunAngle);

	void				NPCAdd(CPlayerNPC* pNPC);
	bool				NPCDelete(int iID);								// Remove NPCs matching the unique ID from the list.. and release resources
	void				UPCAdd(CPlayerOther* pPlayer);
	bool				UPCDelete(int iID);								// Remove NPCs matching the unique ID from the list.. and release resources

	// CPlayerOther* UPCGetByName(const char* szID); // Investigate the User Player Character and NPC and get the pointer.
	CPlayerOther*		UPCGetByID(int iID, bool bFromAliveOnly);		// Investigate the User Player Character and NPC to get a pointer.
	// CPlayerNPC* NPCGetByName(const char* szID); // Investigate the User Player Character and NPC and get the pointer.
	CPlayerNPC*			NPCGetByID(int iID, bool bFromAliveOnly);		// Investigate the User Player Character and NPC to get a pointer.
	CPlayerNPC*			NPCGetByPos(const __Vector3& vPos);
	CPlayerNPC*			CharacterGetByID(int iID, bool bFromAliveOnly); // User, NPC not covered and brought..
	CPlayerNPC*			CharacterGetByNearstEnemy(e_Nation eNation, const __Vector3& vPosPlayer); // Get the nearest enemy...
	bool				CharacterDelete(int iID); // Users, NPCs are not covered and erased..

	CPlayerBase*		CorpseGetByID(int iID);							// Examine the Player Character and NPCs in the corpses and get the pointers.
	void				CorpseRemove(CPlayerNPC* pCorpse, bool bRemoveImmediately = false);
	void				CorpseAdd(CPlayerNPC* pNPC);
	void				CorpseAdd(int iID);
	CPlayerNPC*			CorpseGetNearstNPC(bool bMustHaveItem, e_Nation eNation, const __Vector3& vPosPlayer); // Get the nearest enemy corpse...
	void				MoveToCorpsesForcely(CPlayerNPC* pNPC, bool bErase);			// If the ID overlaps, a body is forcibly created.


	// .. Picked PlayerOther Calculation..
	CPlayerNPC*			Pick(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick = nullptr);
	CPlayerNPC*			PickNPC(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick = nullptr);
	CPlayerOther*		PickUPC(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick = nullptr);
	CPlayerNPC*			PickPrecisely(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick = nullptr);
	CPlayerNPC*			PickNPCPrecisely(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick = nullptr);
	CPlayerOther*		PickUPCPrecisely(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick = nullptr);
	CPlayerNPC*			PickAllPrecisely(int ixScreen, int iyScreen, int &iIDResult, __Vector3* pvPick);
	CPlayerNPC*			PickCorpse(int ixScreen, int iyScreen, int& iIDResult); // Click on the corpse...
	static int			SortByCameraDistance(const void* pArg1, const void* pArg2);

	void				ReleaseUPCs();
	void				ReleaseNPCs();
	void				ReleaseCorpses();
	void				Release();

	CPlayerOtherMgr();
	virtual ~CPlayerOtherMgr();
};

inline CPlayerOther* CPlayerOtherMgr::UPCGetByID(int iID, bool bFromAliveOnly)
{
	const auto it = m_UPCs.find(iID);
	if(it != m_UPCs.end())
	{
		CPlayerOther* pUPC = it->second;
		if(bFromAliveOnly)
		{
			if(PSA_DEATH != pUPC->m_eState) return pUPC;
			else return nullptr;
		}
		else return pUPC;
	}
	else return nullptr;
}

inline CPlayerNPC* CPlayerOtherMgr::NPCGetByID(int iID, bool bFromAliveOnly)
{
	const auto it = m_NPCs.find(iID);
	if(it != m_NPCs.end())
	{
		CPlayerNPC* pNPC = it->second;
		if(bFromAliveOnly)
		{
			if(PSA_DEATH != pNPC->m_eState) return pNPC;
			else return nullptr;
		}
		else return pNPC;
	}
	else return nullptr;
}

inline CPlayerBase* CPlayerOtherMgr::CorpseGetByID(int iID)
{
	const auto it = m_Corpses.find(iID);
	if(it != m_Corpses.end()) return it->second;
	else return nullptr;
}

inline CPlayerNPC* CPlayerOtherMgr::CharacterGetByID(int iID, bool bFromAliveOnly)
{
	if(iID < 0) return nullptr;

	CPlayerNPC* pBPC = this->NPCGetByID(iID, bFromAliveOnly);		// First, look for NPCs.
	if(nullptr == pBPC) pBPC = this->UPCGetByID(iID, bFromAliveOnly);			// None Users are also searched..

	return pBPC;
}

inline void CPlayerOtherMgr::UPCAdd(CPlayerOther* pUPC)
{
	const auto it = m_UPCs.find(pUPC->IDNumber());
	if(it == m_UPCs.end()) // If there are no duplicates...
	{
		m_UPCs.insert(std::make_pair(pUPC->IDNumber(), pUPC));
	}
	else // If duplicate...
	{
		T_Delete(it->second);
		it->second = pUPC;
	}
}

inline bool CPlayerOtherMgr::UPCDelete(int iID)
{
	const auto it = m_UPCs.find(iID);
	if(it == m_UPCs.end()) return false;

	T_Delete(it->second);
	m_UPCs.erase(it);
	return true;
}

inline void CPlayerOtherMgr::NPCAdd(CPlayerNPC* pNPC)
{
	const auto it = m_NPCs.find(pNPC->IDNumber());
	if(it == m_NPCs.end()) // If there are no duplicates...
	{
		m_NPCs.insert(std::make_pair(pNPC->IDNumber(), pNPC));
	}
	else // If duplicate...
	{
		T_Delete(it->second);
		it->second = pNPC;
	}
}

inline bool CPlayerOtherMgr::NPCDelete(int iID)
{
	const auto it = m_NPCs.find(iID);
	if(it == m_NPCs.end()) return false;

	T_Delete(it->second);
	m_NPCs.erase(it);
	return true;
}
