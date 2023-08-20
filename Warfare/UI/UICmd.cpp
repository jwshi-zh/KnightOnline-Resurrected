#include "pch.h"
#include "UICmd.h"
#include "GameProcMain.h"
#include "PlayerOtherMgr.h"
#include "PlayerMyself.h"
#include "UITransactionDlg.h"
#include "N3UIButton.h"
#include "UIManager.h"

CUICmd::CUICmd()
{
	m_pBtn_Exit = nullptr;			// exit

	m_pBtn_Act = nullptr;			// action
	m_pBtn_Act_Walk = nullptr;	// walking
	m_pBtn_Act_Run = nullptr;		// running
	m_pBtn_Act_Stop = nullptr;	// clerk
	m_pBtn_Act_Attack = nullptr;	// attack
	
	m_pBtn_Act_StandUp = nullptr; // stand up
	m_pBtn_Act_SitDown = nullptr;	// sit down

	m_pBtn_Camera = nullptr;			// camera
	m_pBtn_Inventory = nullptr;		// item window
	m_pBtn_Party_Invite = nullptr;	// party invitation
	m_pBtn_Party_Disband = nullptr;	// leave the party
	m_pBtn_Option = nullptr;			// option
	m_pBtn_Quest = nullptr;			// quest
	m_pBtn_Character = nullptr;		// self information window
	m_pBtn_Skill = nullptr;			// Skill Tree or Magic Spear
	m_pBtn_Belong = nullptr;			// affiliated knights
	m_pBtn_Map = nullptr;				// mini map
}

CUICmd::~CUICmd()
{
	DestroyTooltip();
}

bool CUICmd::Load(HANDLE hFile)
{
	if(CN3UIBase::Load(hFile)==false) return false;

	m_pBtn_Act =			GetChildByID("btn_control");	// __ASSERT(m_pBtn_Act, "NULL UI Component!!");
	m_pBtn_Act_Walk =		GetChildByID("btn_walk");		// __ASSERT(m_pBtn_Act_Walk, "NULL UI Component!!");
	m_pBtn_Act_Run =		GetChildByID("btn_run");		// __ASSERT(m_pBtn_Act_Run, "NULL UI Component!!");
	m_pBtn_Act_Stop =		GetChildByID("btn_stop");		// __ASSERT(m_pBtn_Act_Stop, "NULL UI Component!!");
	m_pBtn_Act_Attack =		GetChildByID("btn_attack");		// __ASSERT(m_pBtn_Act_Attack, "NULL UI Component!!");
	m_pBtn_Act_StandUp =	GetChildByID("btn_stand");		// __ASSERT(m_pBtn_Act_StandUp, "NULL UI Component!!");
	m_pBtn_Act_SitDown =	GetChildByID("btn_sit");		// __ASSERT(m_pBtn_Act_SitDown, "NULL UI Component!!");

	if(m_pBtn_Act_StandUp) m_pBtn_Act_StandUp->SetVisible(false); // Kill the wake button in advance.
	
	m_pBtn_Character =		GetChildByID("btn_character");	// __ASSERT(m_pBtn_Character, "NULL UI Component!!");
	m_pBtn_Inventory =		GetChildByID("btn_inventory");	// __ASSERT(m_pBtn_Inventory, "NULL UI Component!!");

	m_pBtn_Option =			GetChildByID("btn_option");		// __ASSERT(m_pBtn_Option, "NULL UI Component!!");
	m_pBtn_Camera =			GetChildByID("btn_camera");		// __ASSERT(m_pBtn_Camera, "NULL UI Component!!");
	m_pBtn_Party_Invite =	GetChildByID("btn_invite");		// __ASSERT(m_pBtn_Party_Invite, "NULL UI Component!!");
	m_pBtn_Party_Disband =	GetChildByID("btn_disband");	// __ASSERT(m_pBtn_Party_Disband, "NULL UI Component!!");
	m_pBtn_Skill =			GetChildByID("btn_skill");		// __ASSERT(m_pBtn_Skill, "NULL UI Component!!");
	m_pBtn_Exit	=			GetChildByID("btn_Exit");		// __ASSERT(m_pBtn_Exit, "NULL UI Component!!");

	m_pBtn_Quest =		GetChildByID("btn_quest");			// __ASSERT(m_pBtn_Quest, "NULL UI Component!!");
	m_pBtn_Belong =		GetChildByID("btn_knight");			// __ASSERT(m_pBtn_Belong, "NULL UI Component!!");

	m_pBtn_Map =		GetChildByID("btn_map");			// __ASSERT(m_pBtn_Map, "NULL UI Component!!");

	// this->SetVisibleActButtons(true);
	// this->SetVisibleOptButtons(false);

	return true;
}

bool CUICmd::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if ( CGameProcedure::s_pProcMain->m_pUITransactionDlg->IsVisible() )	
			return true;

	if (dwMsg == UIMSG_BUTTON_CLICK)					
	{
		// if(pSender == m_pBtn_Option)
		// {
			// this->SetVisibleActButtons(false);
			// this->SetVisibleOptButtons(true);
		// }

		if(pSender == m_pBtn_Act)
		{
			// this->SetVisibleActButtons(true);
			// this->SetVisibleOptButtons(false);
		}

		else if(pSender == m_pBtn_Act_Walk)
		{
			CGameProcedure::s_pProcMain->CommandToggleWalkRun();
		}
			
		else if(pSender == m_pBtn_Act_Run)
		{
			CGameProcedure::s_pProcMain->CommandToggleWalkRun();
		}

		else if(pSender == m_pBtn_Act_Attack)
		{
			CGameProcedure::s_pProcMain->CommandToggleAttackContinous();
		}

		else if(pSender == m_pBtn_Inventory)
		{
			CGameProcedure::s_pProcMain->CommandToggleUIInventory();
		}
		
		else if(pSender == m_pBtn_Character)
		{
			CGameProcedure::s_pProcMain->CommandToggleUIState();
		}

		else if(pSender == m_pBtn_Exit) // Go to the character selection screen.
		{
			std::string szMsg; ::_LoadStringFromResource(IDS_EXIT, szMsg);
			// CGameProcedure::MessageBoxPost(szMsg, &quot;&quot;, MB_YESNO, BEHAVIOR_RESTART_GAME); // Restart the game..??
			CGameProcedure::MessageBoxPost(szMsg, "", MB_YESNO, BEHAVIOR_EXIT); // Restart the game..??
		}

		else if(pSender == m_pBtn_Camera)
		{
			CGameProcedure::s_pProcMain->CommandCameraChange(); // Change the camera perspective...
		}

		else if(pSender == m_pBtn_Party_Invite)
		{
			CPlayerOther* pUPC = CGameProcedure::s_pOPMgr->UPCGetByID(CGameProcedure::s_pPlayer->m_iIDTarget, true);
			if(pUPC && pUPC->m_InfoBase.eNation == CGameProcedure::s_pPlayer->m_InfoBase.eNation) // country check
			{
				CGameProcedure::s_pProcMain->MsgSend_PartyOrForceCreate(0, pUPC->IDString()); // Invite a party...
			}
		}

		else if(pSender == m_pBtn_Party_Disband)
		{
			CGameProcMain* pMain = CGameProcedure::s_pProcMain;
			CPlayerMySelf* pPlayer = CGameBase::s_pPlayer;

			bool bIAmLeader = false, bIAmMemberOfParty = false;
			int iMemberIndex = -1;
			CPlayerBase* pTarget = nullptr;
			pMain->PartyOrForceConditionGet(bIAmLeader, bIAmMemberOfParty, iMemberIndex, pTarget); // Look at the status of the party...
			
			std::string szMsg;
			if(bIAmLeader) // If I&#39;m the leader...
			{
				if(iMemberIndex > 0)
				{
					::_LoadStringFromResource(IDS_PARTY_CONFIRM_DISCHARGE, szMsg); // Shall I kick you out of the party?
					szMsg = pTarget->IDString() + szMsg;
				}
				else ::_LoadStringFromResource(IDS_PARTY_CONFIRM_DESTROY, szMsg); // Shall we have a party?
			}
			else if(bIAmMemberOfParty)
			{
				::_LoadStringFromResource(IDS_PARTY_CONFIRM_LEAVE, szMsg); // Shall we leave the party?
			}

			if(!szMsg.empty()) CGameProcedure::MessageBoxPost(szMsg, "", MB_YESNO, BEHAVIOR_PARTY_DISBAND); // Disbanding, expulsion, leaving the party... OK
		}

		else if(pSender == m_pBtn_Act_SitDown)
		{
			CGameProcedure::s_pProcMain->CommandSitDown(true, true);
		}
		
		else if(pSender == m_pBtn_Act_StandUp)
		{
			CGameProcedure::s_pProcMain->CommandSitDown(true, false);
		}

		else if(pSender == m_pBtn_Skill)
		{
			CGameProcedure::s_pProcMain->CommandToggleUISkillTree();
		}

		else if(pSender == m_pBtn_Map)
		{
			CGameProcedure::s_pProcMain->CommandToggleUIMiniMap();
		}
	}

	return true;
}

/*
void CUICmd::SetVisibleActButtons(bool bVisible)
{
	// action
	if(m_pBtn_Act_Walk) m_pBtn_Act_Walk->SetVisible(bVisible); 
	if(m_pBtn_Act_Run) m_pBtn_Act_Run->SetVisible(bVisible);
	if(m_pBtn_Act_Stop) m_pBtn_Act_Stop->SetVisible(bVisible);
	if(m_pBtn_Act_StandUp) m_pBtn_Act_StandUp->SetVisible(bVisible);
	if(m_pBtn_Act_SitDown) m_pBtn_Act_SitDown->SetVisible(bVisible);
	if(m_pBtn_Act_Attack) m_pBtn_Act_Attack->SetVisible(bVisible);
}
*/

/*
void CUICmd::SetVisibleOptButtons(bool bVisible)
{
	// option
	if(m_pBtn_Opt_Quest) m_pBtn_Opt_Quest->SetVisible(bVisible);
	if(m_pBtn_Character) m_pBtn_Character->SetVisible(bVisible);	
	if(m_pBtn_Skill) m_pBtn_Skill->SetVisible(bVisible);
	if(m_pBtn_Opt_Knight) m_pBtn_Opt_Knight->SetVisible(bVisible);
	if(m_pBtn_Inventory) m_pBtn_Inventory->SetVisible(bVisible);	
}
*/

void CUICmd::UpdatePartyButtons(bool bIAmLeader, bool bIAmMemberOfParty, int iMemberIndex, const CPlayerBase* pTarget) const
{
	bool bInvite = true;
	if(bIAmLeader) // If I&#39;m the leader...
	{
		if(pTarget) // I have a target...
		{
			if(iMemberIndex > 0) bInvite = false; // If the target is a party member... it can be expelled.
			else bInvite = true;
		}
		else
		{
			bInvite = false; // Leaders can also...
		}
	}
	else
	{
		if(bIAmMemberOfParty) bInvite = false; // If you are not a leader but are in a party... you can leave.
		else bInvite = true; // If you&#39;re not in the party, you can invite...
	}

	if(m_pBtn_Party_Invite) m_pBtn_Party_Invite->SetVisible(bInvite);
	if(m_pBtn_Party_Disband) m_pBtn_Party_Disband->SetVisible(!bInvite);
}

bool CUICmd::OnKeyPress(int iKey)
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
