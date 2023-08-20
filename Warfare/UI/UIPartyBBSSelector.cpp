#include "pch.h"
#include "UIPartyBBSSelector.h"

#include "GameProcedure.h"
#include "PacketDef.h"
#include "APISocket.h"

CUIPartyBBSSelector::CUIPartyBBSSelector()
{
	m_pBtn_WantPartyMember	= nullptr;
	m_pBtn_WantParty		= nullptr;
}

CUIPartyBBSSelector::~CUIPartyBBSSelector()
{

}

bool CUIPartyBBSSelector::ReceiveMessage(CN3UIBase *pSender, DWORD dwMsg)
{
	if (dwMsg == UIMSG_BUTTON_CLICK)					
	{
		if(pSender->m_szID == "Btn_WantParty")	
		{
			if(m_pBtn_WantParty && m_pBtn_WantParty->IsVisible())
			{
				MsgSend_PartyBBSKind(WANT_PARTY);
			}
			SetVisible(false);
			return true;
		}

		if(pSender->m_szID == "Btn_WantPartyMember")
		{
			if(m_pBtn_WantPartyMember && m_pBtn_WantPartyMember->IsVisible())
			{
				MsgSend_PartyBBSKind(WANT_PARTY_MEMBER);
			}
			SetVisible(false);
			return true;
		}

		if(pSender->m_szID == "Btn_Cancel")	
		{
			SetVisible(false);
			return true;
		}
	}
	return true;
}

bool CUIPartyBBSSelector::Load(HANDLE hFile)
{
	if(CN3UIBase::Load(hFile)==false) return false;

	m_pBtn_WantPartyMember	= (CN3UIButton*)GetChildByID("Btn_WantParty");			__ASSERT(m_pBtn_WantPartyMember,	"NULL UI Component!!");;
	m_pBtn_WantParty		= (CN3UIButton*)GetChildByID("Btn_WantPartyMember");	__ASSERT(m_pBtn_WantParty,	"NULL UI Component!!");;

	return true;
}

void CUIPartyBBSSelector::MsgSend_PartyBBSKind(BYTE byKind)
{
	BYTE byBuff[4];											
	int iOffset=0;											

	CAPISocket::MP_AddByte(byBuff, iOffset, 1);		// I guess I need to set the packet
	CAPISocket::MP_AddByte(byBuff, iOffset, byKind);		// Looking for a party or looking for a party member...
	CGameProcedure::s_pSocket->Send(byBuff, iOffset);		// Send packets..
}
