#include "pch.h"
#include "GameProcCharacterCreate.h"

#include "GameEng.h"
#include "UICharacterCreate.h"
#include "UIMessageBox.h"
#include "UIMessageBoxManager.h"
#include "PlayerMySelf.h"
#include "UIManager.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "PacketDef.h"
#include "GameCursor.h"

#include "N3UIString.h"

CGameProcCharacterCreate::CGameProcCharacterCreate()
{
	m_pUICharacterCreate = nullptr;

	s_pPlayer->m_InfoBase.eRace = RACE_UNKNOWN;
	s_pPlayer->m_InfoBase.eClass = CLASS_UNKNOWN;
}

CGameProcCharacterCreate::~CGameProcCharacterCreate()
{
	delete m_pUICharacterCreate;
}

void CGameProcCharacterCreate::Release()
{
	CGameProcedure::Release();

	delete m_pUICharacterCreate; m_pUICharacterCreate = nullptr;

	SetRect(&m_rcChr, 0, 0, 0, 0);
	m_Tbl_InitValue.Release();

	s_pPlayer->m_ChrInv.Release();
	s_pPlayer->m_ChrInv.PartAlloc(PART_POS_COUNT);
	s_pPlayer->m_ChrInv.PlugAlloc(PLUG_POS_COUNT);
}

void CGameProcCharacterCreate::Init()
{
	CGameProcedure::Init();

	SetRect(&m_rcChr, 0, 0, 0, 0);

	m_Tbl_InitValue.LoadFromFile("Data\\NewChrValue.tbl");

	s_pPlayer->m_InfoBase.eRace = RACE_UNKNOWN;
	s_pPlayer->m_InfoBase.eClass = CLASS_UNKNOWN;
	s_pPlayer->m_ChrInv.Release();
	s_pPlayer->m_ChrInv.PartAlloc(PART_POS_COUNT);
	s_pPlayer->m_ChrInv.PlugAlloc(PLUG_POS_COUNT);

	const __TABLE_UI_RESRC* pTblUI = s_pTbl_UI->Find(s_pPlayer->m_InfoBase.eNation);
	m_pUICharacterCreate = new CUICharacterCreate();
	m_pUICharacterCreate->Init(s_pUIMgr);
	m_pUICharacterCreate->LoadFromFile(pTblUI->szCharacterCreate);
}

void CGameProcCharacterCreate::Render()
{
	s_pEng->Clear(0); // clear..
	s_pEng->s_lpD3DDev->BeginScene();			// Scene renders...

	s_pUIMgr->Render();

	s_pPlayer->InventoryChrRender(m_rcChr); // Drawing a character...

	s_pMsgBoxMgr->Render(); // Draw a MessageBox.
	if(s_pGameCursor) s_pGameCursor->Render();

	s_pEng->s_lpD3DDev->EndScene();			// Start scene render...
	s_pEng->Present(CN3Base::s_hWndBase);
}

void CGameProcCharacterCreate::SetChr()
{
	const __InfoPlayerBase*	pInfoBase = &(s_pPlayer->m_InfoBase);
	__InfoPlayerMySelf*	pInfoExt = &(s_pPlayer->m_InfoExt);
	const __TABLE_NEW_CHR* pTbl = m_Tbl_InitValue.Find(pInfoBase->eRace);

	if(pTbl)
	{
		pInfoExt->iStrength =		pTbl->iStr;
		pInfoExt->iStamina =		pTbl->iSta;
		pInfoExt->iDexterity =		pTbl->iDex;
		pInfoExt->iIntelligence =	pTbl->iInt;
		pInfoExt->iMagicAttak =		pTbl->iMAP;
		m_pUICharacterCreate->m_iBonusPoint	=		pTbl->iBonus;
		m_pUICharacterCreate->m_iMaxBonusPoint =	pTbl->iBonus;
	}

	__TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks->Find(s_pPlayer->m_InfoBase.eRace);	// User Player Character Skin structure pointer..;
	if(nullptr == pLooks) return;

	s_pPlayer->InitChr(pLooks);
	s_pPlayer->m_ChrInv.ScaleSet(1,1,1); // Return the scale to its original size.

	if(pLooks) // Part setting...
	{
		for(int i = 0; i < PART_POS_COUNT; i++)
		{
			if(i == PART_POS_FACE) { s_pPlayer->InitFace(); continue; }
			if(i == PART_POS_HAIR_HELMET) { s_pPlayer->InitHair(); continue; }
			s_pPlayer->PartSet((e_PartPosition)i, pLooks->szPartFNs[i], nullptr, nullptr);
		}
	}
	
	m_pUICharacterCreate->Reset();
	m_pUICharacterCreate->UpdateRaceAndClassButtons(pInfoBase->eRace);
	m_pUICharacterCreate->UpdateClassButtons(pInfoBase->eClass);

	s_pPlayer->InventoryChrAnimationInitialize();
	s_pPlayer->Action(PSA_BASIC, true, nullptr, true);
}

void CGameProcCharacterCreate::Tick()
{
	// s_pLocalInput->Tick(); // Receive input from keyboard and mouse.
	// if(dwMouseFlags & MOUSE_LBDOWN) SetCursor(s_hCursorClick);
	// else SetCursor(s_hCursorNormal);

	CGameProcedure::Tick();

	const DWORD dwMouseFlags = s_pLocalInput->MouseGetFlag();
	m_pUICharacterCreate->Tick();
	m_pUICharacterCreate->MouseProc(dwMouseFlags, s_pLocalInput->MouseGetPos(), s_pLocalInput->MouseGetPosOld());

	s_pEng->s_SndMgr.Tick(); // Sound Engine...

	// Handling Network Msg
	while ( s_pSocket->m_qRecvPkt.size() > 0 )			// Is there a packet in the packet list????
	{
		int iOffset = 0;
		DataPack* pDataPack = s_pSocket->m_qRecvPkt.front();			// Copy the first one in the queue.
		if (false == ProcessPacket(pDataPack, iOffset)) break;		// It is not a situation to process packets.
		delete pDataPack;
		s_pSocket->m_qRecvPkt.pop();					// Remove packet from queue.
	}

	s_pPlayer->InventoryChrTick();
}

bool CGameProcCharacterCreate::MsgSendCharacterCreate()
{
	e_ErrorCharacterCreate eErrCode = ERROR_CHARACTER_CREATE_SUCCESS;
	const std::string& szID = s_pPlayer->IDString();

	const int iIDLength = szID.size();
	if(iIDLength <= 0)
	{
		eErrCode = ERROR_CHARACTER_CREATE_INVALID_NAME;
	}
	else if(RACE_UNKNOWN == s_pPlayer->m_InfoBase.eRace)
	{
		eErrCode = ERROR_CHARACTER_CREATE_INVALID_RACE;
	}
	// else if(RACE_KA_WRINKLETUAREK == s_pPlayer->m_InfoBase.eRace) // Wizard cannot be selected..
	// {
	// eErrCode = ERROR_CHARACTER_CREATE_NOT_SUPPORTED_RACE;
	// }
	else if(CLASS_UNKNOWN == s_pPlayer->m_InfoBase.eClass)
	{
		eErrCode = ERROR_CHARACTER_CREATE_INVALID_CLASS;
	}
	else if(m_pUICharacterCreate->m_iBonusPoint > 0)
	{
		eErrCode = ERROR_CHARACTER_CREATE_REMAIN_BONUS_POINT;
	}
	else
	{
		// Check if the name contains spaces or special characters
		bool bHasSpecialLetter = false;
		for(int i = 0; i < iIDLength; i++)
		{
			// CompadmreString(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH, id, strlen(id), pUser->m_UserId, strlen(pUser->m_UserId) ) == CSTR_EQUAL )
			if(	'~' == szID[i] || 
				'`' == szID[i] || 
				'!' == szID[i] || 
				'@' == szID[i] || 
				'#' == szID[i] || 
				'$' == szID[i] || 
				'%' == szID[i] || 
				'^' == szID[i] || 
				'&' == szID[i] || 
				'*' == szID[i] || 
				'(' == szID[i] || 
				')' == szID[i] || 
				// '_' == szID[i] ||
				'-' == szID[i] || 
				'+' == szID[i] || 
				'=' == szID[i] || 
				'|' == szID[i] || 
				'\\' == szID[i] || 
				'<' == szID[i] || 
				'>' == szID[i] || 
				',' == szID[i] || 
				'.' == szID[i] || 
				'?' == szID[i] || 
				'/' == szID[i] || 
				'{' == szID[i] || 
				'[' == szID[i] || 
				'}' == szID[i] || 
				']' == szID[i] ||
				'\"' == szID[i] ||
				'\'' == szID[i] ||
				' ' == szID[i] ) 
			{
				bHasSpecialLetter = true;
				eErrCode = ERROR_CHARACTER_CREATE_INVALID_NAME_HAS_SPECIAL_LETTER;
				break; 
			}
		}

		if(false == bHasSpecialLetter)
		{

			__InfoPlayerBase*	pInfoBase = &(s_pPlayer->m_InfoBase);
			const __InfoPlayerMySelf*	pInfoExt = &(s_pPlayer->m_InfoExt);

			BYTE byBuff[64];
			int iOffset = 0;
			CAPISocket::MP_AddByte(byBuff, iOffset,  N3_NEW_CHARACTER);					// command.
			CAPISocket::MP_AddByte(byBuff, iOffset, CGameProcedure::s_iChrSelectIndex);	// character index b
			CAPISocket::MP_AddShort(byBuff, iOffset, iIDLength);						// id length s
			CAPISocket::MP_AddString(byBuff, iOffset, s_pPlayer->IDString());			// ID string str
			CAPISocket::MP_AddByte(byBuff, iOffset, s_pPlayer->m_InfoBase.eRace);		// race b
			CAPISocket::MP_AddShort(byBuff, iOffset, s_pPlayer->m_InfoBase.eClass);		// job b
			CAPISocket::MP_AddByte(byBuff, iOffset, pInfoExt->iFace);					// face shape b
			CAPISocket::MP_AddByte(byBuff, iOffset, pInfoExt->iHair);					// hair style b
			CAPISocket::MP_AddByte(byBuff, iOffset, pInfoExt->iStrength);				// force b
			CAPISocket::MP_AddByte(byBuff, iOffset, pInfoExt->iStamina);				// endurance b
			CAPISocket::MP_AddByte(byBuff, iOffset, pInfoExt->iDexterity);				// agile b
			CAPISocket::MP_AddByte(byBuff, iOffset, pInfoExt->iIntelligence);			// intelligence b
			CAPISocket::MP_AddByte(byBuff, iOffset, pInfoExt->iMagicAttak);				// horsepower b

			s_pSocket->Send(byBuff, iOffset);								// send
			
			s_pUIMgr->EnableOperationSet(false); // Disable the UI until packets arrive...
			
			return true;
		}
	}

	ReportErrorCharacterCreate(eErrCode); // report error...

	return false;
}

void CGameProcCharacterCreate::ReportErrorCharacterCreate(e_ErrorCharacterCreate eErrCode)
{
	std::string szErr;

	if(ERROR_CHARACTER_CREATE_NO_MORE_CHARACTER == eErrCode)
		::_LoadStringFromResource(IDS_ERR_NO_MORE_CHARACTER, szErr);
	else if(ERROR_CHARACTER_CREATE_INVALID_NATION_AND_INVALID_RACE == eErrCode)
		::_LoadStringFromResource(IDS_ERR_INVALID_NATION_RACE, szErr);
	else if(ERROR_CHARACTER_CREATE_OVERLAPPED_ID == eErrCode)
		::_LoadStringFromResource(IDS_ERR_OVERLAPPED_ID, szErr);
	else if(ERROR_CHARACTER_CREATE_DB_CREATE == eErrCode)
		::_LoadStringFromResource(IDS_ERR_DB_CREATE, szErr);
	else if(ERROR_CHARACTER_CREATE_INVALID_NAME == eErrCode)
		::_LoadStringFromResource(IDS_ERR_INVALID_NAME, szErr);
	else if(ERROR_CHARACTER_CREATE_INVALID_NAME_HAS_SPECIAL_LETTER == eErrCode)
		::_LoadStringFromResource(IDS_ERR_INVALID_NAME_HAS_SPECIAL_LETTER, szErr);
	else if(ERROR_CHARACTER_CREATE_INVALID_RACE == eErrCode)
		::_LoadStringFromResource(IDS_ERR_INVALID_RACE, szErr);
	else if(ERROR_CHARACTER_CREATE_NOT_SUPPORTED_RACE == eErrCode)
		::_LoadStringFromResource(IDS_ERR_NOT_SUPPORTED_RACE, szErr);
	else if(ERROR_CHARACTER_CREATE_INVALID_CLASS == eErrCode)
		::_LoadStringFromResource(IDS_ERR_INVALID_CLASS, szErr);
	else if(ERROR_CHARACTER_CREATE_REMAIN_BONUS_POINT == eErrCode)
		::_LoadStringFromResource(IDS_ERR_REMAIN_BONUS_POINT, szErr);
	else if(ERROR_CHARACTER_CREATE_INVALID_STAT_POINT == eErrCode)
		::_LoadStringFromResource(IDS_ERR_UNKNOWN, szErr);
	else
		::_LoadStringFromResource(IDS_ERR_UNKNOWN, szErr);

	// 
	// When you receive a message that you have failed because you entered the information incorrectly...
	std::string szTitle; ::_LoadStringFromResource(IDS_ERR_CHARACTER_CREATE, szTitle);
	MessageBoxPost(szErr, szTitle, MB_OK);
}

bool CGameProcCharacterCreate::ProcessPacket(DataPack* pDataPack, int& iOffset)
{
	const int iOffsetPrev = iOffset;
	if(false == CGameProcedure::ProcessPacket(pDataPack, iOffset)) iOffset = iOffsetPrev;
	else return true;

	const int iCmd = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// Command parsing...
	switch ( iCmd )										// Branch according to the command..
	{
		case N3_NEW_CHARACTER:				// Character selection message...
		{
			BYTE bySuccess = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// Command parsing...
			if(0 == bySuccess) 
			{
				ProcActiveSet((CGameProcedure*)s_pProcCharacterSelect); // Go to the character select screen.
			}
			else // If it fails.. the reason comes with a non-zero value..
			{
				this->ReportErrorCharacterCreate((e_ErrorCharacterCreate)bySuccess); // Error message pops up...
				s_pUIMgr->EnableOperationSet(false); // Enables UI manipulation... You have to create a character again..
			}
			s_pUIMgr->EnableOperationSet(false); // Disable the UI until packets arrive...
		}
		return true;
	}
	
	return false;
}