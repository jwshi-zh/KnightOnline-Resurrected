#pragma once

#include "GameDef.h"
#include "N3UIBase.h"
#include <list>

class CUIPartyOrForce : public CN3UIBase // It is used as the same class as the UI for the party and the troops.
{
protected:
	class CN3UIProgress*	m_pProgress_HPs[MAX_PARTY_OR_FORCE];		// As much as the number of members... HP Gauge
	class CN3UIProgress*	m_pProgress_HPReduce[MAX_PARTY_OR_FORCE];	// As much as the number of members... HP Reduce
	class CN3UIProgress*	m_pProgress_ETC[MAX_PARTY_OR_FORCE];		// As much as the number of members... Abnormal status
	class CN3UIStatic*		m_pStatic_IDs[MAX_PARTY_OR_FORCE];		// As many as the number of members... Names...
	class CN3UIArea*		m_pAreas[MAX_PARTY_OR_FORCE];		// As many as the number of members... Names...

	std::list<__InfoPartyOrForce>	m_Members; // party member
	int			m_iIndexSelected; // Currently selected member index..

public:
	int			m_iPartyOrForce; // is it a party? Is it a bag?? 1 is the party, 2 is the army..

public:
	bool OnKeyPress(int iKey);
	void Tick();
	void		MemberClassChange(int iID, e_Class eClass);
	void		MemberLevelChange(int iID, int iLevel);
	void		MemberHPChange(int iID, int iHP, int iHPMax);
	void		MemberStatusChange(int iID, e_PartyStatus ePS, bool bSuffer);

	void		MemberInfoReInit(); // When the composition of party members is changed.. Updating the order and various information..
	bool		TargetByIndex(int iIndex); // Catching targets in order.

	const __InfoPartyOrForce*	MemberInfoGetByID(int iID, int& iIndexResult);
	const __InfoPartyOrForce*	MemberInfoGetByIndex(int iIndex);
	const __InfoPartyOrForce*	MemberInfoGetSelected(); // Currently selected member index..
	const __InfoPartyOrForce*	MemberAdd(int iID, const std::string szID, int iLevel, e_Class eClass, int iHP, int iHPMax);
	class CPlayerOther*			MemberGetByNearst(const __Vector3& vPosPlayer);
	bool						MemberRemove(int iID);
	void						MemberDestroy();
	int							MemberCount() const { return m_Members.size(); }
	void						MemberSelect(int iMemberIndex) { if(iMemberIndex < 0 || iMemberIndex > m_Members.size()) return; m_iIndexSelected = iMemberIndex; }

	bool Load(HANDLE hFile);
	bool ReceiveMessage(class CN3UIBase* pSender, DWORD dwMsg);
	void Render();
	
	void Release();
	CUIPartyOrForce();
	virtual ~CUIPartyOrForce();
};
