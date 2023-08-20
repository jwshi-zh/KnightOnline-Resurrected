// NpcMagicProcess.cpp: implementation of the CNpcMagicProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "server.h"
#include "NpcMagicProcess.h"
#include "ServerDlg.h"
#include "User.h"
#include "Npc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNpcMagicProcess::CNpcMagicProcess()
{
	m_pMain = NULL;
	m_pSrcNpc = NULL;
	m_bMagicState = NONE;
}

CNpcMagicProcess::~CNpcMagicProcess()
{

}

void CNpcMagicProcess::MagicPacket(char *pBuf, int len, CIOCPort* pIOCP)
{
	int index = 0, send_index = 0, magicid = 0, sid = -1, tid = -1, data1 = 0, data2 = 0, data3 = 0, data4 = 0, data5 = 0, data6 = 0;
	char send_buff[128]; memset( send_buff, NULL, 128 );
	_MAGIC_TABLE* pTable = NULL;
	
	BYTE command = GetByte( pBuf, index );		// Get the magic status.  
	if( command == MAGIC_FAIL ) {			    // Client indicates that magic failed. Just send back packet.
		SetByte( send_buff, AG_MAGIC_ATTACK_RESULT, send_index );
		SetString( send_buff, pBuf, len-1, send_index );	// len ==> include WIZ_MAGIC_PROCESS command byte. 
		//m_pMain->Send_Region( send_buff, send_index, m_pSrcUser->m_pUserData->m_bZone, m_pSrcUser->m_RegionX, m_pSrcUser->m_RegionZ );
		m_bMagicState = NONE;
		return;
	}

	magicid = GetDWORD( pBuf, index );        // Get ID of magic.
	sid = GetShort( pBuf, index );			  // Get ID of source.
	tid = GetShort( pBuf, index );            // Get ID of target.

	data1 = GetShort( pBuf, index );          // ( Remember, you don't definately need this. )
	data2 = GetShort( pBuf, index );		  // ( Only use it when you really feel it's needed. )
	data3 = GetShort( pBuf, index );
	data4 = GetShort( pBuf, index );
	data5 = GetShort( pBuf, index );
	data6 = GetShort( pBuf, index );          // Get data1 ~ data6 (No, I don't know what the hell 'data' is.) 

	pTable = IsAvailable( magicid, tid, command );     // If magic was successful.......
	if( !pTable ) return;

	if( command == MAGIC_EFFECTING )     // Is target another player? 
	{
		//if (tid < -1 || tid >= MAX_USER) return;	

		switch( pTable->bType1 ) {
		case 1:
			ExecuteType1( pTable->iNum, tid, data1, data2, data3 );
			break;
		case 2:
			ExecuteType2( pTable->iNum, tid, data1, data2, data3 );	
			break;
		case 3:
			ExecuteType3( pTable->iNum, tid, data1, data2, data3, pTable->bMoral );
			break;
		case 4:
			ExecuteType4( pTable->iNum, tid );
			break;
		case 5:
			ExecuteType5( pTable->iNum );
			break;
		case 6:
			ExecuteType6( pTable->iNum );
			break;
		case 7:
			ExecuteType7( pTable->iNum );
			break;
		case 8:
			ExecuteType8( pTable->iNum, tid, sid, data1, data2, data3 );
			break;
		case 9:
			ExecuteType9( pTable->iNum );
			break;
		case 10:
			ExecuteType10( pTable->iNum );
			break;
		}

		switch( pTable->bType2 ) {
		case 1:
			ExecuteType1( pTable->iNum, tid, data4, data5, data6 );
			break;
		case 2:
			ExecuteType2( pTable->iNum, tid, data1, data2, data3 );	
			break;
		case 3:
			ExecuteType3( pTable->iNum, tid, data1, data2, data3, pTable->bMoral );
			break;
		case 4:
			ExecuteType4( pTable->iNum, tid );
			break;
		case 5:
			ExecuteType5( pTable->iNum );
			break;
		case 6:
			ExecuteType6( pTable->iNum );
			break;
		case 7:
			ExecuteType7( pTable->iNum );
			break;
		case 8:
			ExecuteType8( pTable->iNum, tid, sid, data1, data2, data3 );
			break;
		case 9:
			ExecuteType9( pTable->iNum );
			break;
		case 10:
			ExecuteType10( pTable->iNum );
			break;	
		}
	}
	else if( command == MAGIC_CASTING ) {
		SetByte( send_buff, AG_MAGIC_ATTACK_RESULT, send_index );
		SetString( send_buff, pBuf, len-1, send_index );	// len ==> include WIZ_MAGIC_PROCESS command byte. 
		m_pSrcNpc->SendAll(pIOCP, send_buff, send_index);
	}
}

_MAGIC_TABLE* CNpcMagicProcess::IsAvailable(int magicid, int tid, BYTE type )
{
	CUser* pUser = NULL;
	CNpc* pNpc = NULL;
	_MAGIC_TABLE* pTable = NULL;

	int modulator = 0, Class = 0, send_index = 0, moral = 0;

	char send_buff[128];
	memset( send_buff, NULL, 128); 
	if( !m_pSrcNpc ) return FALSE;

	pTable = m_pMain->m_MagictableArray.GetData( magicid );     // Get main magic table.
	if( !pTable ) goto fail_return;            

	if( tid >= 0 && tid < MAX_USER )     // Compare morals between source and target character.       
	{
		pUser = m_pMain->GetUserPtr(tid);
		if( !pUser || pUser->m_bLive == USER_DEAD ) goto fail_return;
		moral = pUser->m_bNation;
	}
	else if( tid >= NPC_BAND )     // Compare morals between source and target NPC.            
	{
		pNpc = m_pMain->m_arNpc.GetData(tid - NPC_BAND);
		if( !pNpc || pNpc->m_NpcState == NPC_DEAD ) goto fail_return;
		moral = pNpc->m_byGroup;
	}
	else if ( tid == -1) {  // Party Moral check routine.
		if (pTable->bMoral == MORAL_AREA_ENEMY) {
			if( m_pSrcNpc->m_byGroup == 0) {	// Switch morals. 