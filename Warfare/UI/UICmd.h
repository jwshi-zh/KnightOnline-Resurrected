#pragma once

#include "N3UIBase.h"

class CUICmd : public CN3UIBase  
{
public:
	CN3UIBase*	m_pBtn_Exit;			// exit

	CN3UIBase*	m_pBtn_Act;			// action
	CN3UIBase*	m_pBtn_Act_Walk;	// walking
	CN3UIBase*	m_pBtn_Act_Run;		// running
	CN3UIBase*	m_pBtn_Act_Stop;	// clerk
	CN3UIBase*	m_pBtn_Act_Attack;	// attack
	
	CN3UIBase*	m_pBtn_Act_StandUp; // stand up
	CN3UIBase*	m_pBtn_Act_SitDown;	// sit down

	CN3UIBase*	m_pBtn_Camera;			// camera
	CN3UIBase*	m_pBtn_Inventory;		// item window
	CN3UIBase*	m_pBtn_Party_Invite;	// party invitation
	CN3UIBase*	m_pBtn_Party_Disband;	// leave the party
	CN3UIBase*  m_pBtn_Option;			// option
	CN3UIBase*	m_pBtn_Quest;			// quest
	CN3UIBase*	m_pBtn_Character;		// self information window
	CN3UIBase*	m_pBtn_Skill;			// Skill Tree or Magic Spear
	CN3UIBase*	m_pBtn_Belong;			// affiliated knights
	CN3UIBase*	m_pBtn_Map;				// mini map

public:
	bool OnKeyPress(int iKey);
	// void	SetVisibleOptButtons(bool bVisible);
	// void	SetVisibleActButtons(bool bVisible);

	bool	Load(HANDLE hFile);
	bool	ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);
	void	UpdatePartyButtons(bool bIAmLeader, bool bIAmMemberOfParty, int iMemberIndex, const class CPlayerBase* pTarget) const;

	CUICmd();
	virtual ~CUICmd();
};
