#include "pch.h"
#include "UIKnightsOperation.h"
#include "GameProcMain.h"
#include "PacketDef.h"
#include "APISocket.h"

#include "N3UIButton.h"
#include "N3UIEdit.h"
#include "N3UIList.h"
#include "N3UIScrollBar.h"

CUIKnightsOperation::CUIKnightsOperation()
{
	m_iPageCur = 0;

	m_pBtn_Up	= nullptr;
	m_pBtn_Down	= nullptr;
	m_pBtn_Close	= nullptr;

	m_pList_Knights = nullptr;
	m_pBtn_Join = nullptr;
	m_pBtn_Create = nullptr;
	m_pBtn_Destroy = nullptr;
	m_pBtn_Withdraw = nullptr;
	m_pEdit_KnightsName = nullptr;
}

CUIKnightsOperation::~CUIKnightsOperation()
{

}

void CUIKnightsOperation::Release()
{
	CN3UIBase::Release();

	m_iPageCur = 0;
	m_KnightsListExt.clear();

	m_pBtn_Up	= nullptr;
	m_pBtn_Down	= nullptr;
	m_pBtn_Close	= nullptr;

	m_pList_Knights = nullptr;
	m_pBtn_Join = nullptr;
	m_pBtn_Create = nullptr;
	m_pBtn_Destroy = nullptr;
	m_pBtn_Withdraw = nullptr;
	m_pEdit_KnightsName = nullptr;
}

bool CUIKnightsOperation::Load(HANDLE hFile)
{
	if(false == CN3UIBase::Load(hFile)) return false;

	m_pBtn_Up = (CN3UIButton*)(this->GetChildByID("btn_up"));		__ASSERT(m_pBtn_Up, "NULL UI Component!!");
	m_pBtn_Down = (CN3UIButton*)(this->GetChildByID("btn_down"));	__ASSERT(m_pBtn_Down, "NULL UI Component!!");
	m_pBtn_Close = (CN3UIButton*)(this->GetChildByID("btn_close"));	__ASSERT(m_pBtn_Close, "NULL UI Component!!");

	m_pList_Knights =		(CN3UIList*)(this->GetChildByID("List_Knights"));		__ASSERT(m_pList_Knights, "NULL UI Component!!");
	m_pBtn_Join =			(CN3UIButton*)(this->GetChildByID("Btn_Join"));			__ASSERT(m_pBtn_Join, "NULL UI Component!!");
	m_pBtn_Create =			(CN3UIButton*)(this->GetChildByID("Btn_Create"));		__ASSERT(m_pBtn_Create, "NULL UI Component!!");
	m_pBtn_Destroy =		(CN3UIButton*)(this->GetChildByID("Btn_Destroy"));		__ASSERT(m_pBtn_Destroy, "NULL UI Component!!");
	m_pBtn_Withdraw =		(CN3UIButton*)(this->GetChildByID("Btn_Withdraw"));		__ASSERT(m_pBtn_Withdraw, "NULL UI Component!!");
	m_pEdit_KnightsName =	(CN3UIEdit*)(this->GetChildByID("Edit_KnightsName"));	__ASSERT(m_pEdit_KnightsName, "NULL UI Component!!");

	return true;
}

bool CUIKnightsOperation::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)					
	{
		POINT pt;
		::GetCursorPos(&pt);

		if(pSender == m_pBtn_Up)
		{
			m_iPageCur--;
			if(m_iPageCur < 0)
			{
				m_iPageCur = 0;
			}
			else
			{
				this->MsgSend_KnightsList(m_iPageCur);
			}
		}
		else if(pSender == m_pBtn_Up)
		{
			m_iPageCur--;
			if(m_iPageCur < 0)
			{
				m_iPageCur = 0;
			}
			else
			{
				this->MsgSend_KnightsList(m_iPageCur);
			}
		}
		else if(pSender == m_pBtn_Close) 
		{
			this->Close();
		}
		else if(pSender == m_pBtn_Create)
		{
			this->MsgSend_KnightsCreate();
		}
		else if(pSender == m_pBtn_Join) // join
		{
			this->MsgSend_KnightsJoin();
		}
		else if(pSender == m_pBtn_Destroy) // secession
		{
			std::string szMsg;
			::_LoadStringFromResource(IDS_KNIGHTS_DESTROY_CONFIRM, szMsg);
			CGameProcedure::MessageBoxPost(szMsg, "", MB_YESNO, BEHAVIOR_KNIGHTS_DESTROY); // Ask about disbanding the Knights...
		}
		else if(pSender == m_pBtn_Withdraw) // secession
		{
			std::string szMsg;
			::_LoadStringFromResource(IDS_KNIGHTS_WITHDRAW_CONFIRM, szMsg);
			CGameProcedure::MessageBoxPost(szMsg, "", MB_YESNO, BEHAVIOR_KNIGHTS_WITHDRAW); // Ask to leave the Knights...
		}
	}
	else if(dwMsg == UIMSG_LIST_SELCHANGE)
	{
		if(pSender == m_pList_Knights)
		{
		}
	}

	return false;
}

void CUIKnightsOperation::KnightsInfoDelete(int iID)
{
	const auto it = m_KnightsMapBase.find(iID);
	if(it != m_KnightsMapBase.end()) m_KnightsMapBase.erase(it);
}

void CUIKnightsOperation::KnightsInfoInsert(int iID, const std::string& szName)
{
	__KnightsInfoBase KIB;
	KIB.iID = iID;
	KIB.szName = szName;

	m_KnightsMapBase.insert(std::make_pair(iID, KIB));
}

__KnightsInfoBase* CUIKnightsOperation::KnightsInfoFind(int iID)
{
	const auto it = m_KnightsMapBase.find(iID);
	if(it != m_KnightsMapBase.end()) return &(it->second);
	return nullptr;
}

void CUIKnightsOperation::KnightsListAdd(	int iID,
											const std::string& szName,
											const std::string& szChiefName,
											int iMemberCount,
											int iPoint)
{
	__KnightsInfoExt KIE;
	KIE.iID = iID;
	KIE.szName = szName;
	KIE.szChiefName = szChiefName;
	KIE.iMemberCount = iMemberCount;
	KIE.iPoint = iPoint;

	m_KnightsListExt.push_back(KIE);
}

void CUIKnightsOperation::KnightsListUpdate()
{
	if(nullptr == m_pList_Knights) return;

	m_pList_Knights->ResetContent();

	char szBuff[80];
	auto it = m_KnightsListExt.begin(), itEnd = m_KnightsListExt.end();
	for(; it != itEnd; it++)
	{
		const auto* pKIE = (__KnightsInfoExt*)(&(*it));
		std::string szName = pKIE->szName;
		std::string szChiefName = pKIE->szChiefName;

		sprintf(szBuff, "%16s %12s %4d %8d", pKIE->szName.c_str(), pKIE->szChiefName.c_str(), pKIE->iMemberCount, pKIE->iPoint);
		m_pList_Knights->AddString(szBuff);
	}
}

void CUIKnightsOperation::KnightsListClear()
{
	m_KnightsListExt.clear();
	if(m_pList_Knights) m_pList_Knights->ResetContent();
}

void CUIKnightsOperation::ChangeUIByDuty(e_KnightsDuty eDuty) const
{
	if(eDuty == KNIGHTS_DUTY_CHIEF) // If you are the captain of the knights, you can disband the knights.
	{
		if(m_pBtn_Destroy) m_pBtn_Destroy->SetState(UI_STATE_BUTTON_NORMAL);
		if(m_pBtn_Withdraw) m_pBtn_Withdraw->SetState(UI_STATE_BUTTON_DISABLE); // The Knight Commander resigns????!!!!
		if(m_pBtn_Join) m_pBtn_Join->SetState(UI_STATE_BUTTON_DISABLE);
	}
	else 
	{
		if(m_pBtn_Destroy) m_pBtn_Destroy->SetState(UI_STATE_BUTTON_DISABLE);
		if(m_pBtn_Withdraw) m_pBtn_Withdraw->SetState(UI_STATE_BUTTON_NORMAL); // The Knight Commander resigns????!!!!
		if(m_pBtn_Join) m_pBtn_Join->SetState(UI_STATE_BUTTON_NORMAL);
	}
}

void CUIKnightsOperation::Open(e_KnightsDuty eDuty)
{
	m_iPageCur = 0;
	this->KnightsListClear(); // Knight information clear
	this->SetPosCenter(); // set it in the middle...
	this->SetVisible(true);

	this->ChangeUIByDuty(eDuty); // UI change according to permission..
}

void CUIKnightsOperation::Close()
{
	this->KnightsListClear(); // Knight information clear
	this->SetVisible(false);
	if(m_pEdit_KnightsName) m_pEdit_KnightsName->KillFocus(); // This way there won't be any problems elsewhere.
}

bool CUIKnightsOperation::MsgRecv_KnightsList(DataPack* pDataPack, int& iOffset)
{
	m_iPageCur = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const int iKC = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	int iID, iNameLength, iMemberCount, iPoint;
	std::string szName, szChiefName;
	for(int i = 0; i < iKC; i++)
	{
		iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		iNameLength = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szName, iNameLength);
		iMemberCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		iNameLength = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szChiefName, iNameLength);
		iPoint = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);

		this->KnightsListAdd(iID, szName, szChiefName, iMemberCount, iPoint); // Add to UI..
	}
	this->KnightsListUpdate(); // UI Update!!
	this->EnableKnightsUIs(true); // Disabled buttons are enabled.

	return true;
}

void CUIKnightsOperation::MsgSend_KnightsCreate()
{
	if(nullptr == m_pEdit_KnightsName) return;

	const std::string szKnightsName = m_pEdit_KnightsName->GetString();
	if(szKnightsName.empty()) // Error if no name.
	{
		std::string szMsg; ::_LoadStringFromResource(IDS_ERR_KNIGHTS_CREATE_FAILED_NAME_EMPTY, szMsg);
		CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff00ff);
		return;
	}

	int iOffset = 0;
	BYTE byBuff[128];

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_KNIGHTS);
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_KNIGHTS_CREATE); // Create Send - s1(Name Length) str1 | Recv - b1 (1: success 0: failure)
	CAPISocket::MP_AddShort(byBuff, iOffset, szKnightsName.size());
	CAPISocket::MP_AddString(byBuff, iOffset, szKnightsName);

	CGameProcedure::s_pSocket->Send(byBuff, iOffset);
}

void CUIKnightsOperation::MsgSend_KnightsDestroy()
{
	int iOffset = 0;
	BYTE byBuff[8];

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_KNIGHTS);
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_KNIGHTS_DESTROY); // Create Send - s1(Name Length) str1 | Recv - b1 (1: success 0: failure)

	CGameProcedure::s_pSocket->Send(byBuff, iOffset);
}

void CUIKnightsOperation::MsgSend_KnightsJoin()
{
	if(nullptr == m_pList_Knights) return;
	const int iCurSel = m_pList_Knights->GetCurSel();
	if(iCurSel < 0 && iCurSel >= m_KnightsListExt.size()) return;

	auto it = m_KnightsListExt.begin();
	for(int i = 0; i < iCurSel; i++, it++);
	const __KnightsInfoExt KIE = (*it);

	int iOffset = 0;
	BYTE byBuff[32];

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_KNIGHTS);
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_KNIGHTS_JOIN);
	CAPISocket::MP_AddShort(byBuff, iOffset, KIE.iID);

	CGameProcedure::s_pSocket->Send(byBuff, iOffset);
}

void CUIKnightsOperation::MsgSend_KnightsWithdraw()
{
	BYTE byBuff[32];
	int iOffset = 0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_KNIGHTS);
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_KNIGHTS_WITHDRAW);

	CGameProcedure::s_pSocket->Send(byBuff, iOffset);
}

void CUIKnightsOperation::MsgSend_KnightsList(int iPage)
{
	int iOffset = 0;
	BYTE byBuff[32];
	
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_KNIGHTS);
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_KNIGHTS_GRADE_CHANGE_ALL);
	CAPISocket::MP_AddShort(byBuff, iOffset, iPage);

	CGameProcedure::s_pSocket->Send(byBuff, iOffset);

	// Block the buttons when turning the page.
	this->EnableKnightsUIs(false);
}

void CUIKnightsOperation::EnableKnightsUIs(bool bEnable)
{
	eUI_STATE eState = UI_STATE_BUTTON_NORMAL;
	if(bEnable) eState = UI_STATE_BUTTON_NORMAL;
	else eState = UI_STATE_BUTTON_DISABLE;
	
	if(m_pBtn_Up) m_pBtn_Up->SetState(eState);
	if(m_pBtn_Down) m_pBtn_Down->SetState(eState);
	if(m_pBtn_Close) m_pBtn_Close->SetState(eState);

	if(m_pList_Knights) m_pList_Knights->SetState(eState);
	if(m_pBtn_Join) m_pBtn_Join->SetState(eState);
	if(m_pBtn_Create) m_pBtn_Create->SetState(eState);
	if(m_pBtn_Withdraw) m_pBtn_Withdraw->SetState(eState);
	if(m_pEdit_KnightsName) m_pEdit_KnightsName->SetState(eState);
}