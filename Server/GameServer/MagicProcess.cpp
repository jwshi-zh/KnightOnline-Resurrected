// MagicProcess.cpp: implementation of the CMagicProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "server.h"
#include "MagicProcess.h"
#include "ServerDlg.h"
#include "User.h"
#include "Npc.h"
#include "Region.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CRITICAL_SECTION g_region_critical;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMagicProcess::CMagicProcess()
{
	m_pMain = NULL;
	m_pSrcUser = NULL;
	m_bMagicState = NONE;
}

CMagicProcess::~CMagicProcess()
{

}

void CMagicProcess::MagicPacket(char *pBuf)
{
	int index = 0, send_index = 0, magicid = 0, sid = -1, tid = -1, TotalDex=0, righthand_damage = 0;
	int data1 = 0, data2 = 0, data3 = 0, data4 = 0, data5 = 0, data6 = 0, result = 1;
	char send_buff[128]; memset( send_buff, NULL, 128 );
	_MAGIC_TABLE* pTable = NULL;

	sid = m_pSrcUser->m_iUserId;

	BYTE command = GetByte( pBuf, index );		// Get the magic status.  
	tid = GetShort( pBuf, index );            // Get ID of target.
	magicid = GetDWORD( pBuf, index );        // Get ID of magic.
	data1 = GetShort( pBuf, index );
	data2 = GetShort( pBuf, index );
	data3 = GetShort( pBuf, index );
	data4 = GetShort( pBuf, index );
	data5 = GetShort( pBuf, index );
	data6 = GetShort( pBuf, index );
	TotalDex = GetShort( pBuf, index );
	righthand_damage = GetShort( pBuf, index );

	//TRACE("MagicPacket - command=%d, tid=%d, magicid=%d\n", command, tid, magicid);

	pTable = IsAvailable( magicid, tid, command );     // If magic was successful.......
	if( !pTable ) return;

	if( command == MAGIC_EFFECTING )     // Is target another player? 
	{
		switch( pTable->bType1 ) {
		case 1:
			result = ExecuteType1( pTable->iNum, tid, data1, data2, data3, 1 );
			break;
		case 2:
			result = ExecuteType2( pTable->iNum, tid, data1, data2, data3 );	
			break;
		case 3:
			ExecuteType3( pTable->iNum, tid, data1, data2, data3, pTable->bMoral, TotalDex, righthand_damage );
			break;
		case 4:
			ExecuteType4( pTable->iNum, sid, tid, data1, data2, data3, pTable->bMoral );
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
			ExecuteType8( pTable->iNum );
			break;
		case 9:
			ExecuteType9( pTable->iNum );
			break;
		case 10:
			ExecuteType10( pTable->iNum );
			break;
		}

		if(result != 0)	{
			switch( pTable->bType2 ) {
			case 1:
				ExecuteType1( pTable->iNum, tid, data4, data5, data6, 2 );
				break;
			case 2:
				ExecuteType2( pTable->iNum, tid, data1, data2, data3 );	
				break;
			case 3:
				ExecuteType3( pTable->iNum, tid, data1, data2, data3, pTable->bMoral, TotalDex, righthand_damage );
				break;
			case 4:
				ExecuteType4( pTable->iNum, sid, tid, data1, data2, data3, pTable->bMoral );
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
				ExecuteType8( pTable->iNum );
				break;
			case 9:
				ExecuteType9( pTable->iNum );
				break;
			case 10:
				ExecuteType10( pTable->iNum );
				break;	
			}
		}
	}
}

_MAGIC_TABLE* CMagicProcess::IsAvailable(int magicid, int tid, BYTE type )
{
	_MAGIC_TABLE* pTable = NULL;

	int modulator = 0, Class = 0, send_index = 0, moral = 0;

	char send_buff[128];
	memset( send_buff, NULL, 128); 
	if( !m_pSrcUser ) return FALSE;

	pTable = m_pMain->m_MagictableArray.GetData( magicid );     // Get main magic table.
	if( !pTable ) goto fail_return;            

	return pTable;      // Magic was successful! 

fail_return:    // In case the magic failed. 
	memset( send_buff, NULL, 128 ); send_index = 0;
	//SetByte( send_buff, WIZ_MAGIC_PROCESS, send_index );
	//SetByte( send_buff, MAGIC_FAIL, send_index );
	//SetShort( send_buff, m_pSrcUser->GetSocketID(), send_index );

	m_bMagicState = NONE;

	return NULL;     // Magic was a failure!
}

BYTE CMagicProcess::ExecuteType1(int magicid, int tid, int data1, int data2, int data3, BYTE sequence)   // Applied to an attack skill using a weapon.
{	
	int damage = 0, send_index = 0, result = 1;     // Variable initialization. result == 1 : success, 0 : fail
	char send_buff[128];	memset( send_buff, NULL, 128); 
	_MAGIC_TABLE* pMagic = NULL;
	pMagic = m_pMain->m_MagictableArray.GetData( magicid );   // Get main magic table.
	if( !pMagic ) return 0; 

	damage = m_pSrcUser->GetDamage(tid, magicid);  // Get damage points of enemy.	
// 	if(damage <= 0)	damage = 1;
	//TRACE("magictype1 ,, magicid=%d, damage=%d\n", magicid, damage);

//	if (damage > 0) {
		CNpc* pNpc = NULL ;      // Pointer initialization!
		pNpc = m_pMain->m_arNpc.GetData(tid-NPC_BAND);
		if(pNpc == NULL || pNpc->m_NpcState == NPC_DEAD || pNpc->m_iHP == 0)	{
			result = 0;
			goto packet_send;
		}

		if(pNpc->SetDamage(magicid, damage, m_pSrcUser->m_strUserID, m_pSrcUser->m_iUserId + USER_BAND, m_pSrcUser->m_pIocport) == FALSE)	{
			// Npc媛