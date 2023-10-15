#include "pch.h"
#include "UILogIn.h"
#include "GameProcLogIn.h"

#include "N3UIEdit.h"
#include "N3UIButton.h"
#include "N3UIList.h"
#include "N3UIString.h"
#include "N3UIImage.h"
#include "UIMessageBoxManager.h"

#include <algorithm>

CUILogIn::CUILogIn()
{
	m_pEdit_id = nullptr;
	m_pEdit_pw = nullptr;
	
	m_pBtn_LogIn = nullptr;
	m_pBtn_Connect = nullptr;
	m_pBtn_Cancel = nullptr;
	m_pBtn_Option = nullptr;
	m_pBtn_Notice1Ok = nullptr;
	m_pBtn_Notice2Ok = nullptr;
	m_pBtn_Notice3Ok = nullptr;

	m_pGroup_ServerList = nullptr;
	m_pGroup_Notice1 = nullptr;
	m_pGroup_Notice2 = nullptr;
	m_pGroup_Notice3 = nullptr;
	m_pGroup_LogIn = nullptr;
	
	m_bLogIn = false;
}

CUILogIn::~CUILogIn()
{

}

bool CUILogIn::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if(nullptr == pSender) return false;

	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == m_pBtn_Notice1Ok || pSender == m_pBtn_Notice2Ok || pSender == m_pBtn_Notice3Ok)
		{
			m_pGroup_ActiveNotice->SetVisible(false);

			CGameProcedure::s_pProcLogIn->MsgSend_GameServerList();
		}
		else if (pSender == m_pBtn_LogIn && m_pEdit_id && m_pEdit_pw)
		{
			CGameProcedure::s_pProcLogIn->MsgSend_AccountLogIn();
		}
		else if(pSender == m_pBtn_Connect)
		{
			CGameProcedure::s_pProcLogIn->ConnectToGameServer(); // Connect to the game server of your choice
		}
		else if (pSender == m_pBtn_Cancel)
		{
			PostQuitMessage(0);	// end...
		}
		else if(pSender == m_pBtn_Option) // option..
		{
			std::string szMsg;
			::_LoadStringFromResource(IDS_CONFIRM_EXECUTE_OPTION, szMsg);
			CGameProcedure::MessageBoxPost(szMsg, "", MB_YESNO, BEHAVIOR_EXECUTE_OPTION);
		}
	}
	else if(UIMSG_LIST_DBLCLK == dwMsg)
	{
		CGameProcedure::s_pProcLogIn->ConnectToGameServer(); // Connect to the game server of your choice
	}
	else if (dwMsg == UIMSG_EDIT_RETURN)
	{
		if(!m_bLogIn && m_pEdit_id && m_pEdit_pw)
		{
			const CN3UIBase* pMsgBox = CGameProcedure::s_pMsgBoxMgr->GetFocusMsgBox();
			if( !(pMsgBox && pMsgBox->IsVisible()) )
				CGameProcedure::s_pProcLogIn->MsgSend_AccountLogIn();
		}
	}

	return true;
}

bool CUILogIn::Load(HANDLE hFile)
{
	if(CN3UIBase::Load(hFile)==false) return false;

	m_pGroup_LogIn = GetChildByID("Group_LogIn"); __ASSERT(m_pGroup_LogIn, "NULL UI Component!!");
	m_pGroup_Notice1 = GetChildByID("Group_Notice_1"); __ASSERT(m_pGroup_Notice1, "NULL UI Component!!");
	m_pGroup_Notice2 = GetChildByID("Group_Notice_2"); __ASSERT(m_pGroup_Notice2, "NULL UI Component!!");
	m_pGroup_Notice3 = GetChildByID("Group_Notice_3"); __ASSERT(m_pGroup_Notice3, "NULL UI Component!!");
	m_pGroup_ServerList = GetChildByID("Group_ServerList_01"); __ASSERT(m_pGroup_ServerList, "NULL UI Component!!");

	if (m_pGroup_LogIn) {
		m_pGroup_LogIn->CenterInScreen();

		m_pBtn_LogIn = (CN3UIButton*)m_pGroup_LogIn->GetChildByID("btn_ok");		__ASSERT(m_pBtn_LogIn, "NULL UI Component!!");
		m_pBtn_Cancel = (CN3UIButton*)m_pGroup_LogIn->GetChildByID("btn_cancel");	__ASSERT(m_pBtn_Cancel, "NULL UI Component!!");
		m_pBtn_Option = (CN3UIButton*)m_pGroup_LogIn->GetChildByID("btn_option");	__ASSERT(m_pBtn_Option, "NULL UI Component!!");
		m_pBtn_Homepage = (CN3UIButton*)m_pGroup_LogIn->GetChildByID("btn_homepage");	__ASSERT(m_pBtn_Homepage, "NULL UI Component!!");

		m_pEdit_id = (CN3UIEdit*)m_pGroup_LogIn->GetChildByID("Edit_ID");			__ASSERT(m_pEdit_id, "NULL UI Component!!");
		m_pEdit_pw = (CN3UIEdit*)m_pGroup_LogIn->GetChildByID("Edit_PW");			__ASSERT(m_pEdit_pw, "NULL UI Component!!");
	}

	if (m_pGroup_Notice1) {
		m_pGroup_Notice1->CenterInScreen();
		m_pGroup_Notice1->SetVisible(false);

		m_pBtn_Notice1Ok = (CN3UIButton*)m_pGroup_Notice1->GetChildByID("btn_ok");		__ASSERT(m_pBtn_Notice1Ok, "NULL UI Component!!");
	}

	if (m_pGroup_Notice2) {
		m_pGroup_Notice2->CenterInScreen();
		m_pGroup_Notice2->SetVisible(false);

		m_pBtn_Notice2Ok = (CN3UIButton*)m_pGroup_Notice2->GetChildByID("btn_ok");		__ASSERT(m_pBtn_Notice2Ok, "NULL UI Component!!");
	}

	if (m_pGroup_Notice3) {
		m_pGroup_Notice3->CenterInScreen();
		m_pGroup_Notice3->SetVisible(false);

		m_pBtn_Notice3Ok = (CN3UIButton*)m_pGroup_Notice3->GetChildByID("btn_ok");		__ASSERT(m_pBtn_Notice3Ok, "NULL UI Component!!");
	}

	if (m_pGroup_ServerList) {
		m_pGroup_ServerList->CenterInScreen();
		m_pGroup_ServerList->SetVisible(false);

		for (auto i = 1; i <= 20; i++) {
			m_arrServerList[i - 1].m_pImage_Arrow = (CN3UIImage*)m_pGroup_ServerList->GetChildByID("img_arrow" + std::to_string(i));
			__ASSERT(m_arrServerList[i - 1].m_pImage_Arrow, "NULL UI Component!!");
			if (m_arrServerList[i - 1].m_pImage_Arrow) m_arrServerList[i - 1].m_pImage_Arrow->SetVisible(false);

			auto pGroupServerContainer = m_pGroup_ServerList->GetChildByID("server_" + std::to_string(i));
			__ASSERT(pGroupServerContainer, "NULL UI Component!!");

			if (pGroupServerContainer) {
				m_arrServerList[i - 1].m_pStr_ServerName = (CN3UIString*)pGroupServerContainer->GetChildByID("List_Server");
				__ASSERT(m_arrServerList[i - 1].m_pStr_ServerName, "NULL UI Component!!");
				if (m_arrServerList[i - 1].m_pStr_ServerName) m_arrServerList[i - 1].m_pStr_ServerName->SetVisible(false);

				for (auto j = 1; j <= 12; j++) {
					m_arrServerList[i - 1].m_vImage_Capacity[j - 1] = (CN3UIImage*)pGroupServerContainer->GetChildByID(std::to_string(j));
					__ASSERT(m_arrServerList[i - 1].m_vImage_Capacity[j - 1], "NULL UI Component!!");
					if (m_arrServerList[i - 1].m_vImage_Capacity[j - 1]) m_arrServerList[i - 1].m_vImage_Capacity[j - 1]->SetVisible(false);
				}
			}
		}
	}

	return true;
}

void CUILogIn::AccountIDGet(std::string& szID) const
{
	if(m_pEdit_id) 
		szID = m_pEdit_id->GetString();
	else
		szID = "";
}

void CUILogIn::AccountPWGet(std::string& szPW)
{
	if(m_pEdit_pw)
		szPW = m_pEdit_pw->GetString();
	else
		szPW = "";
}

void CUILogIn::ConnectButtonSetEnable(bool bEnable)
{
	const eUI_STATE eState1 = (bEnable ? UI_STATE_BUTTON_NORMAL : UI_STATE_BUTTON_DISABLE);
	
	if(m_pBtn_Connect) m_pBtn_Connect->SetState(eState1);
}

void CUILogIn::FocusToID()
{
	if(m_pEdit_id) m_pEdit_id->SetFocus();
}

void CUILogIn::FocusCircular()
{
	if(nullptr == m_pEdit_id || nullptr == m_pEdit_pw) return;

	if(m_pEdit_id->HaveFocus())
		m_pEdit_pw->SetFocus();
	else
		m_pEdit_id->SetFocus();
}

void CUILogIn::InitEditControls()
{
	if(m_pEdit_id)
	{
		m_pEdit_id->SetString("");
		m_pEdit_id->SetFocus();
	}
	
	if(m_pEdit_pw)
	{
		m_pEdit_pw->SetString("");
	}
}

bool CUILogIn::ServerInfoAdd(uint8_t iIndex, std::string szName, std::string szIP, uint16_t iConcurrentUserCount, uint16_t iConcurrentCapacity)
{
	m_arrServerList[iIndex].m_szName = szName;
	m_arrServerList[iIndex].m_szIP = szIP;
	m_arrServerList[iIndex].m_iConcurrentUserCount = iConcurrentUserCount;
	m_arrServerList[iIndex].m_iConcurrentUserCapacity = iConcurrentCapacity;

	auto visibleCapacityBarCount = max(ceilf((float)iConcurrentUserCount / (float)iConcurrentCapacity), 1);

	if (m_arrServerList[iIndex].m_pStr_ServerName) {
		m_arrServerList[iIndex].m_pStr_ServerName->SetVisible(true);
		m_arrServerList[iIndex].m_pStr_ServerName->SetString(szName);

		for (auto barIdx = 0; barIdx < 12; barIdx++) {
			m_arrServerList[iIndex].m_vImage_Capacity[barIdx]->SetVisible(barIdx < visibleCapacityBarCount);
		}
	}
	return true;
}

void CUILogIn::OpenServerList()
{
	SelectServer(0);
	if(m_pGroup_ServerList) m_pGroup_ServerList->SetVisible(true);
}

void CUILogIn::HideLoginSubview()
{
	if (m_pGroup_LogIn) m_pGroup_LogIn->SetVisible(false);
}

void CUILogIn::OpenNotice(uint8_t iNoticeCount, std::vector<std::string> vSzNotice)
{
	m_pGroup_Notice1->SetVisible(false);
	m_pGroup_Notice2->SetVisible(false);
	m_pGroup_Notice3->SetVisible(false);

	switch (iNoticeCount) {
	case 1:
		m_pGroup_ActiveNotice = m_pGroup_Notice1;
		break;
	case 2:
		m_pGroup_ActiveNotice = m_pGroup_Notice2;
		break;
	case 3:
		m_pGroup_ActiveNotice = m_pGroup_Notice3;
		break;
	}

	for (auto i = 0; i < iNoticeCount; i++) {
		auto pNoticeText = (CN3UIString*)m_pGroup_ActiveNotice->GetChildByID("text_notice_0" + std::to_string(i + 1));
		pNoticeText->SetString(vSzNotice[i]);
	}

	m_pGroup_ActiveNotice->SetVisible(true);
}

void CUILogIn::SelectServer(uint8_t idx)
{
	for (auto i = 0; i < 20; i++) {
		m_arrServerList[i].m_pStr_ServerName->SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));
	}

	m_iSelectedServerIdx = idx;

	m_arrServerList[m_iSelectedServerIdx].m_pStr_ServerName->SetColor(D3DCOLOR_RGBA(128, 255, 0, 255));
}

void CUILogIn::SetVisibleLogInUIs(bool bEnable)
{
	if(m_pGroup_LogIn) m_pGroup_LogIn->SetVisible(bEnable); // Hide your login.
}

bool CUILogIn::OnKeyPress(int iKey)
{
	if(!m_bLogIn)
	{
		switch(iKey)
		{
		case DIK_TAB:
			FocusCircular();
			return true;
		}
	}
	else if (m_pGroup_ActiveNotice && m_pGroup_ActiveNotice->IsVisible()) {
		switch (iKey)
		{
		case DIK_RETURN:
		case DIK_NUMPADENTER:
			m_pGroup_ActiveNotice->SetVisible(false);

			CGameProcedure::s_pProcLogIn->MsgSend_GameServerList();
			
			return true;
		}
	}
	else if(m_pGroup_ServerList && m_pGroup_ServerList->IsVisible())
	{
		switch(iKey)
		{
		case DIK_UP:
		{
			uint8_t iNewServerIdx{ 0 };

			if (m_iSelectedServerIdx == 0) {
				for (uint8_t i = 19; i >= 0; i++) {
					if (m_arrServerList[iNewServerIdx].m_szIP.size() > 0) {
						iNewServerIdx = i;
						break;
					}
				}
			}
			else {
				iNewServerIdx = m_iSelectedServerIdx - 1;
			}

			SelectServer(iNewServerIdx);

			return true;
		}
		case DIK_DOWN:
		{
			auto iNewServerIdx = m_iSelectedServerIdx + 1;

			if (iNewServerIdx > 19 || m_arrServerList[iNewServerIdx].m_szIP.size() == 0) {
				iNewServerIdx = 0;
			}

			SelectServer(iNewServerIdx);

			return true;
		}
		case DIK_NUMPADENTER:
		case DIK_RETURN:
			ReceiveMessage(m_pBtn_Connect, UIMSG_BUTTON_CLICK);
			return true;
		}
	}

	return CN3UIBase::OnKeyPress(iKey);
}
