// Party.cpp: implementation of the CParty class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "server.h"
#include "ServerDlg.h"
#include "Party.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CRITICAL_SECTION g_region_critical;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParty::CParty()
{
	m_pMain = NULL;
}

CParty::~CParty()
{
	
}

void CParty::Initialize()
{
	m_pMain = (CServerDlg*)AfxGetMainWnd();
}

void CParty::PartyProcess(char *pBuf)
{
	int index = 0;
	BYTE subcommand;

	subcommand = GetByte( pBuf, index );
	switch( subcommand ) {
	case PARTY_CREATE:
		PartyCreate( pBuf+index );
		break;
	case PARTY_INSERT:
		PartyInsert( pBuf+index );
		break;
	case PARTY_REMOVE:
		PartyRemove( pBuf+index );
		break;
	case PARTY_DELETE:
		PartyDelete( pBuf+index );
		break;
	}
}

void CParty::PartyCreate(char *pBuf)
{
	int index = 0;
	short sPartyIndex = 0;
	short sUid = 0, sHP = 0, sClass = 0;
	BYTE  byLevel= 0;
	_PARTY_GROUP* pParty = NULL;
	CUser* pUser = NULL;

	sPartyIndex = GetShort( pBuf, index );
	sUid = GetShort(pBuf, index);
	//sHP = GetShort(pBuf, index);
	//byLevel = GetByte(pBuf, index);
	//sClass = GetShort(pBuf, index);

	pUser = m_pMain->GetUserPtr(sUid);
	if(pUser)	{
		pUser->m_byNowParty = 1;
		pUser->m_sPartyNumber = sPartyIndex;
	}

	EnterCriticalSection( &g_region_critical );

	pParty = new _PARTY_GROUP;
	pParty->wIndex = sPartyIndex;
	pParty->uid[0] = sUid;

	if( m_pMain->m_arParty.PutData( pParty->wIndex, pParty ) ) {
		TRACE("Party - Create() : Party 