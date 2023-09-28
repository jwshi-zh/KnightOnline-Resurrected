#include "pch.h"

#include "PacketDef.h"
#include "GameEng.h"
#include "APISocket.h"

#include "GameProcNationSelect.h"
#include "PlayerMySelf.h"
#include "UINationSelectDlg.h"
#include "UIManager.h"

CGameProcNationSelect::CGameProcNationSelect()
{
	m_pUINationSelectDlg = nullptr;
	s_pPlayer->m_InfoBase.eNation = NATION_NOTSELECTED; // You haven't selected a country yet.
}

CGameProcNationSelect::~CGameProcNationSelect()
{
	delete m_pUINationSelectDlg; m_pUINationSelectDlg = nullptr;
}

void CGameProcNationSelect::Release()
{
	CGameProcedure::Release();

	delete m_pUINationSelectDlg; m_pUINationSelectDlg = nullptr;
}

void CGameProcNationSelect::Init()
{
	CGameProcedure::Init();

	std::string szTemp = "UI\\Co_NationSelect.uif";
	const __TABLE_UI_RESRC* pTbl = s_pTbl_UI->Find(NATION_ELMORAD);
	if(pTbl)
	{
		szTemp = pTbl->szNationSelect;
	}

	m_pUINationSelectDlg = new CUINationSelectDlg();
	m_pUINationSelectDlg->Init(s_pUIMgr);
	m_pUINationSelectDlg->LoadFromFile(szTemp);
	m_pUINationSelectDlg->m_pProcNationSelectRef = this; // Put a reference pointer...

	s_pPlayer->m_InfoBase.eNation = NATION_NOTSELECTED; // You haven't selected a country yet.
}

void CGameProcNationSelect::Tick()
{
	CGameProcedure::Tick();	// keys, mouse input, etc.

	if(NATION_KARUS == s_pPlayer->m_InfoBase.eNation || NATION_ELMORAD == s_pPlayer->m_InfoBase.eNation)
		CGameProcedure::ProcActiveSet((CGameProcedure*)s_pProcCharacterSelect); // Once you've chosen your country, go straight to character selection.
}

void CGameProcNationSelect::Render()
{
	const DWORD color = 0x00000000; // in black...
	s_pEng->Clear(color); // clear..
	s_pEng->s_lpD3DDev->BeginScene();			// Scene renders...

	CGameProcedure::Render(); // Rendering UI and other basic stuff..

	s_pEng->s_lpD3DDev->EndScene();			// Start scene render...
	s_pEng->Present(CN3Base::s_hWndBase);
}


void CGameProcNationSelect::MsgSendNationSelect(e_Nation eNation)
{
	BYTE byBuff[4];										// Packet buffer...
	int iOffset=0;										// Offset of buffer..

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_NATION_SELECT);	// command.
	CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)eNation);		// id length.
		
	s_pSocket->Send(byBuff, iOffset);								// send

	s_pUIMgr->EnableOperationSet(false); // Do nothing until a response packet is received.
}

bool CGameProcNationSelect::ProcessPacket(DataPack* pDataPack, int& iOffset)
{
	const int iOffsetPrev = iOffset;
	if(false == CGameProcedure::ProcessPacket(pDataPack, iOffset)) iOffset = iOffsetPrev;
	else return true;

	const int iCmd = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// Command parsing...
	switch ( iCmd )										// Branch according to the command..
	{
		case N3_NATION_SELECT:							// Character selection message...
		{
			const int iNation = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Country - 0 Failure.. 1 - Carus 2 - Elmorad..

			if( 0 == iNation ) 	s_pPlayer->m_InfoBase.eNation = NATION_NOTSELECTED; // You haven't selected a country yet.
			else if( 1 == iNation ) s_pPlayer->m_InfoBase.eNation = NATION_KARUS;
			else if( 2 == iNation ) s_pPlayer->m_InfoBase.eNation = NATION_ELMORAD;
		}
		return true;
	}
	
	return false;
}