// EbenezerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Ebenezer.h"
#include "EbenezerDlg.h"
#include "User.h"
#include "AiPacket.h"
#include "PacketDefine.h"

#include "ItemTableSet.h"
#include "MagicTableSet.h"
#include "MagicType1Set.h"
#include "MagicType2Set.h"
#include "MagicType3Set.h"
#include "MagicType4Set.h"
#include "MagicType5Set.h"
#include "MagicType8Set.h"
#include "ZoneInfoSet.h"
#include "CoefficientSet.h"
#include "LevelUpTableSet.h"
#include "KnightsSet.h"
#include "KnightsUserSet.h"
#include "KnightsRankSet.h"
#include "HomeSet.h"
#include "BattleSet.h"

#define GAME_TIME       	100
#define SEND_TIME			200
#define PACKET_CHECK		300
#define ALIVE_TIME			400
#define MARKET_BBS_TIME		1000

#define NUM_FLAG_VICTORY    4
#define AWARD_GOLD          5000

// Cryption
T_KEY		g_private_key = 0x1234567890123456;
///~

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRITICAL_SECTION g_serial_critical;
CRITICAL_SECTION g_region_critical;
CRITICAL_SECTION g_LogFile_critical;
CIOCPort	CEbenezerDlg::m_Iocport;

WORD	g_increase_serial = 1;
BYTE	g_serverdown_flag = FALSE;

DWORD WINAPI	ReadQueueThread(LPVOID lp);

DWORD WINAPI ReadQueueThread(LPVOID lp)
{
	CEbenezerDlg* pMain = (CEbenezerDlg*)lp;
	int recvlen = 0, index = 0, uid = -1, send_index = 0, buff_length = 0;
	BYTE command, result;
	char pBuf[1024], send_buff[1024];
	memset( pBuf, NULL, 1024 );
	memset( send_buff, NULL, 1024 );
	CUser* pUser = NULL;
	int currenttime = 0;

	while(TRUE) {
		if( pMain->m_LoggerRecvQueue.GetFrontMode() != R ) {
			index = 0;
			recvlen = pMain->m_LoggerRecvQueue.GetData( pBuf );

			if( recvlen > MAX_PKTSIZE || recvlen == 0 ) {
				Sleep(1);
				continue;
			}

			command = GetByte( pBuf, index );
			uid = GetShort( pBuf, index );
			if( command == KNIGHTS_ALLLIST_REQ+0x10 && uid == -1 )	{
				pMain->m_KnightsManager.RecvKnightsAllList( pBuf+index );
				continue;
			}
			if( uid < 0 || uid >= MAX_USER )
				goto loop_pass;
			if( !pMain->m_Iocport.m_SockArray[uid] )
				goto loop_pass;

			pUser = (CUser*)pMain->m_Iocport.m_SockArray[uid];

			switch( command ) {
				case WIZ_LOGIN:
					result = GetByte( pBuf, index );
					if( result == 0xFF )
						memset( pUser->m_strAccountID, NULL, MAX_ID_SIZE+1 );
					SetByte( send_buff, WIZ_LOGIN, send_index );
					SetByte( send_buff, result, send_index );					// 