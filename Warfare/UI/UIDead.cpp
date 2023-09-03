#include "pch.h"
#include "N3UITooltip.h"
#include "N3UIString.h"
#include "PacketDef.h"
#include "APISocket.h"
#include "GameProcedure.h"
#include "GameProcMain.h"
#include "UIManager.h"
#include "PlayerMySelf.h"
#include "UIDead.h"
#include "UIInventory.h"
#include "UIStateBar.h"
#include "MagicSkillMng.h"
#include "N3FXMgr.h"

#define CHILD_UI_REVIVE_MSG				1
#define CHILD_UI_LACK_LIVE_STONE_MSG	2
#define CHILD_UI_LOW_LEVEL				3

#define TIMES_LIFE_STONE				3
#define LIFE_STONE_INDEX				379006000

#define REVIVAL_TYPE_RETURN_TOWN		1
#define REVIVAL_TYPE_LIFE_STONE			2

CUIDead::CUIDead()
{
	m_pTextAlive	= nullptr;
	m_pTextTown		= nullptr;
	m_bProcessing	= false;
}

CUIDead::~CUIDead()
{

}

bool CUIDead::Load(HANDLE hFile)
{
	if(false == CN3UIBase::Load(hFile)) return false;

	m_pTextAlive	= (CN3UIString*)(this->GetChildByID("Text_Alive"));	__ASSERT(m_pTextAlive, "NULL UI Component!!!");
	m_pTextTown		= (CN3UIString*)(this->GetChildByID("Text_Town"));	__ASSERT(m_pTextTown, "NULL UI Component!!!");


	std::string szMsg;
	::_LoadStringFromResource(IDS_DEAD_REVIVAL, szMsg);
	if(m_pTextAlive) m_pTextAlive->SetString(szMsg);

	::_LoadStringFromResource(IDS_DEAD_RETURN_TOWN, szMsg);
	if(m_pTextTown) m_pTextTown->SetString(szMsg);

	const __TABLE_UI_RESRC* pTblUI = CGameBase::s_pTbl_UI->Find(NATION_ELMORAD);

	m_MsgBox.LoadFromFile(pTblUI->szMessageBox);

	const RECT rt = m_MsgBox.GetRegion();
	POINT pt;
	pt.x = (CN3Base::s_CameraData.vp.Width - (rt.right - rt.left)) / 2;
	pt.y = (CN3Base::s_CameraData.vp.Height - (rt.bottom - rt.top)) / 2;
	m_MsgBox.SetPos(pt.x, pt.y);

	return true;
}

bool CUIDead::ReceiveMessage(CN3UIBase *pSender, DWORD dwMsg)
{
	if( dwMsg == UIMSG_STRING_LCLICK )
	{
		if(pSender == m_pTextAlive)
		{
			int iItemCnt = 0;
			if(CGameProcedure::s_pProcMain->m_pUIInventory)
				iItemCnt = CGameProcedure::s_pProcMain->m_pUIInventory->GetIndexItemCount(LIFE_STONE_INDEX);

			const int iLevel = CGameProcedure::s_pPlayer->m_InfoBase.iLevel;
			const int iNeedItemCnt = iLevel * TIMES_LIFE_STONE;
			char szBuf[256] = "";
			std::string szMsg;

			if(iLevel < 6)
			{
				::_LoadStringFromResource(IDS_DEAD_LOW_LEVEL, szMsg);
				sprintf(szBuf, szMsg.c_str(), iNeedItemCnt);

				m_MsgBox.SetBoxStyle(MB_OK);
				m_MsgBox.m_eBehavior = BEHAVIOR_NOTHING;
				m_MsgBox.SetTitle("");
				m_MsgBox.SetText(szBuf);
				m_MsgBox.ShowWindow(CHILD_UI_LOW_LEVEL,this);
			}
			else if(iItemCnt >= iNeedItemCnt)
			{
				::_LoadStringFromResource(IDS_DEAD_REVIVAL_MESSAGE, szMsg);
				sprintf(szBuf, szMsg.c_str(), iNeedItemCnt);

				m_MsgBox.SetBoxStyle(MB_YESNO);
				m_MsgBox.m_eBehavior = BEHAVIOR_NOTHING;
				m_MsgBox.SetTitle("");
				m_MsgBox.SetText(szBuf);
				m_MsgBox.ShowWindow(CHILD_UI_REVIVE_MSG,this);
			}
			else
			{
				::_LoadStringFromResource(IDS_DEAD_LACK_LIFE_STONE, szMsg);

				m_MsgBox.SetBoxStyle(MB_OK);
				m_MsgBox.m_eBehavior = BEHAVIOR_NOTHING;
				m_MsgBox.SetTitle("");
				m_MsgBox.SetText(szMsg);
				m_MsgBox.ShowWindow(CHILD_UI_LACK_LIVE_STONE_MSG,this);
			}
		}
		else if(pSender == m_pTextTown)
		{
			MsgSend_Revival(REVIVAL_TYPE_RETURN_TOWN);
		}
	}

	return true;
}

DWORD CUIDead::MouseProc(DWORD dwFlags, const POINT &ptCur, const POINT &ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	// UI moving code
	if (UI_STATE_COMMON_MOVE == m_eState)
	{
		if (dwFlags&UI_MOUSE_LBCLICKED)
		{
			SetState(UI_STATE_COMMON_NONE);
		}
		else
		{
			MoveOffset(ptCur.x - ptOld.x, ptCur.y - ptOld.y);
		}
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if(false == IsIn(ptCur.x, ptCur.y))	// out of range
	{
		if(false == IsIn(ptOld.x, ptOld.y))
		{
			return dwRet;// If the previous coordinates are also outside the domain
		}
		dwRet |= UI_MOUSEPROC_PREVINREGION;	// The previous coordinates were inside the area.
	}
	else
	{
		// tool tip related
		if (s_pTooltipCtrl) s_pTooltipCtrl->SetText(m_szToolTip);
	}
	dwRet |= UI_MOUSEPROC_INREGION;	// This coordinate is inside the area.

	if(m_pChildUI && m_pChildUI->IsVisible())
		return dwRet;

	// Send message to child
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);

		DWORD dwChildRet = pChild->MouseProc(dwFlags, ptCur, ptOld);
		if( pChild->IsVisible() && UI_TYPE_STRING == pChild->UIType() )
		{
			if(pChild->IsIn(ptCur.x, ptCur.y) && (dwFlags & UI_MOUSE_LBCLICKED) )	
			{
				dwChildRet |= UI_MOUSEPROC_DONESOMETHING;
			}

			if(pChild->IsIn(ptCur.x, ptCur.y) && (dwFlags & UI_MOUSE_LBDBLCLK) )	
			{
				dwChildRet |= UI_MOUSEPROC_DONESOMETHING;
			}
		}

		if (UI_MOUSEPROC_DONESOMETHING & dwChildRet)
		{	// In this case, it is a case where the distance is focused.
			// (The code below must be done in the place where the dialog is managed. Therefore, it is blocked)
			// m_Children.erase(itor); // first remove from list
			// m_Children. push_front(pChild); // put in front I want to draw the drawing order last

			dwRet |= (UI_MOUSEPROC_CHILDDONESOMETHING|UI_MOUSEPROC_DONESOMETHING);
			return dwRet;
		}
	}

	// UI moving code
	if (UI_STATE_COMMON_MOVE != m_eState && 
			PtInRect(&m_rcMovable, ptCur) && (dwFlags&UI_MOUSE_LBCLICK) )
	{
		SetState(UI_STATE_COMMON_MOVE);
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	return dwRet;
}

void CUIDead::CallBackProc(int iID, DWORD dwFlag)
{
	TRACE("OnButton ID:%d Btn %d\n",iID, dwFlag);

	if(iID == CHILD_UI_REVIVE_MSG)
	{
		if(dwFlag == 1)// OK
		{
			MsgSend_Revival(REVIVAL_TYPE_LIFE_STONE);
		}
	}
	else if(iID == CHILD_UI_LACK_LIVE_STONE_MSG)
	{
	}
	else if(iID == CHILD_UI_LOW_LEVEL)
	{
	}
}

void CUIDead::MsgSend_Revival(BYTE byType)
{
	if(m_bProcessing) return;

	if(CGameProcedure::s_pPlayer->m_iSendRegeneration >= 2) return; // Once sent, a flag that will not be seen until it dies again

	BYTE byBuff[4];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_REGENE);
	CAPISocket::MP_AddByte(byBuff, iOffset, byType);
	CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

	CLogWriter::Write("Send Regeneration");
	CGameProcedure::s_pPlayer->m_iSendRegeneration = 2; // Once sent, a flag that will not be seen until it dies again
	TRACE("sent - come back to life\n");

	m_bProcessing = true;
}

void CUIDead::MsgRecv_Revival(DataPack *pDataPack, int &iOffset)
{
	m_bProcessing = false;

	__Vector3 vPosPlayer;
	vPosPlayer.x = (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	vPosPlayer.z = (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	vPosPlayer.y = (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;
	
	CGameProcedure::s_pProcMain->InitPlayerPosition(vPosPlayer); // Initialize the player position.. Raise him up and make him take the basic action.
	CGameProcedure::s_pPlayer->RegenerateCollisionMesh(); // Recreate the collision mesh.

	CGameProcedure::s_pPlayer->m_iSendRegeneration = 0; // Once sent, a flag that will not be seen until it dies again
	CGameProcedure::s_pPlayer->m_fTimeAfterDeath = 0; // Once sent, a flag that will not be seen until it dies again
	TRACE("received - come back to life(%.1f, %.1f)\n", vPosPlayer.x, vPosPlayer.z);

	//
	// Magic & Effect Reset..
	if(CGameProcedure::s_pProcMain->m_pUIStateBarAndMiniMap) 
		CGameProcedure::s_pProcMain->m_pUIStateBarAndMiniMap->ClearMagic();
	if(CGameProcedure::s_pProcMain->m_pMagicSkillMng) 
		CGameProcedure::s_pProcMain->m_pMagicSkillMng->ClearDurationalMagic();
	if(CGameProcedure::s_pProcMain->CGameProcedure::s_pFX) 
		CGameProcedure::s_pFX->StopMine();

	CLogWriter::Write("Receive Regeneration");

	const int iID = CGameProcedure::s_pPlayer->IDNumber();
	if(CGameProcedure::s_pPlayer->Nation()==NATION_KARUS) 
		CGameProcedure::s_pFX->TriggerBundle(iID, -1, FXID_REGEN_KARUS, iID, -1);
	else if(CGameProcedure::s_pPlayer->Nation()==NATION_ELMORAD) 
		CGameProcedure::s_pFX->TriggerBundle(iID, -1, FXID_REGEN_ELMORAD, iID, -1);

	SetVisible(false);
}

void CUIDead::SetVisible(bool bVisible)
{
	CN3UIBase::SetVisible(bVisible);
	if(bVisible)
		CGameProcedure::s_pUIMgr->SetVisibleFocusedUI(this);
	else
		CGameProcedure::s_pUIMgr->ReFocusUI();// this_ui
}

void CUIDead::SetVisibleWithNoSound(bool bVisible, bool bWork, bool bReFocus)
{
	if(!bWork)
	{
		CN3UIBase::SetVisibleWithNoSound(bVisible, bWork, bReFocus);
	}
}
