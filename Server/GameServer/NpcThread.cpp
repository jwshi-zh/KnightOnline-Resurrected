// NpcThread.cpp: implementation of the CNpcThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "server.h"
#include "NpcThread.h"
#include "Npc.h"
#include "Extern.h"
#include "Mmsystem.h"
#include "ServerDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define DELAY				250

DWORD	g_dwLastTimeCount	= GetTickCount();
DWORD	g_dwCurrTimeCount	= GetTickCount();

//////////////////////////////////////////////////////////////////////
// NPC Thread Callback Function
//
UINT NpcThreadProc(LPVOID pParam /* NPC_THREAD_INFO ptr */)
{
	NPC_THREAD_INFO*	pInfo	= (NPC_THREAD_INFO*)pParam;
	CNpc*				pNpc	= NULL;
	CIOCPort*			pIOCP	= NULL;
	CPoint				pt;

	int					i			= 0;
	DWORD				dwDiffTime	= 0;
	DWORD				dwSleep		= 250;
	DWORD				dwTickTime  = 0;
	srand( (unsigned)time( NULL ) );
	myrand( 1, 10000 ); myrand( 1, 10000 );

	float  fTime2 = 0.0f;
	float  fTime3 = 0.0f;
	int    duration_damage=0;

	if(!pInfo) return 0;

	while(!g_bNpcExit)
	{
		fTime2 = TimeGet();

		for(i = 0; i < NPC_NUM; i++)
		{
			pNpc = pInfo->pNpc[i];
			pIOCP = pInfo->pIOCP;
			if( !pNpc ) continue;
			//if((pNpc->m_tNpcType == NPCTYPE_DOOR || pNpc->m_tNpcType == NPCTYPE_ARTIFACT || pNpc->m_tNpcType == NPCTYPE_PHOENIX_GATE || pNpc->m_tNpcType == NPCTYPE_GATE_LEVER) && !pNpc->m_bFirstLive) continue;
			//if( pNpc->m_bFirstLive ) continue;
			if( pNpc->m_sNid < 0 ) continue;		// 