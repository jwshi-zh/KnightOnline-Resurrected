// Map.cpp: implementation of the CMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Map.h"
#include "Serverdlg.h"
#include "Region.h"
#include "Npc.h"
#include "User.h"
#include "RoomEvent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern CRITICAL_SECTION g_region_critical;

CMapInfo::CMapInfo()
{
	m_sEvent	= 0;
}

CMapInfo::~CMapInfo()
{
}

MAP::MAP()
{
	m_pMain = (CServerDlg*)AfxGetMainWnd();
	m_nMapSize = 0;
	m_fUnitDist = 0.0f;
	m_fHeight = NULL;

	m_sizeRegion.cx = 0;
	m_sizeRegion.cy = 0;
	m_sizeMap.cx = 0;
	m_sizeMap.cy = 0;

	m_ppRegion = NULL;
	//m_pRoomEvent = NULL;
	m_nZoneNumber = 0;
	m_byRoomType = 0;
	m_byRoomEvent = 0;
	m_byRoomStatus = 1;
	m_byInitRoomCount = 0;
	memset( m_MapName, NULL, 256 );
	m_sKarusRoom = 0;
	m_sElmoradRoom = 0;
//	for(int i=0; i<MAX_DUNGEON_BOSS_MONSTER; i++)
//		m_arDungeonBossMonster[i] = 1;
}

MAP::~MAP()
{
	RemoveMapData();
}

void MAP::RemoveMapData()
{
//	int i, j, k;

	if( m_ppRegion ) {
		for(int i=0; i< m_sizeRegion.cx; i++) {
			delete[] m_ppRegion[i];
			m_ppRegion[i] = NULL;
		}
		delete[] m_ppRegion;
		m_ppRegion = NULL;
	}

	if( m_fHeight ){
		for(int i=0; i< m_nMapSize; i++) {
			delete[] m_fHeight[i];
			m_fHeight[i] = NULL;
		}
		delete[] m_fHeight;
	}
	
	if( m_pMap ) {
		for(int i=0; i<m_sizeMap.cx; i++) {
			delete[] m_pMap[i];
			m_pMap[i] = NULL;
		}
		delete[] m_pMap;
		m_pMap = NULL;
	}

	if( !m_ObjectEventArray.IsEmpty() )
		m_ObjectEventArray.DeleteAllData();
	if( !m_arRoomEventArray.IsEmpty() )
		m_arRoomEventArray.DeleteAllData();

	DeleteCriticalSection( &g_region_critical );
}

BOOL MAP::IsMovable(int dest_x, int dest_y)
{
	if(dest_x < 0 || dest_y < 0 ) return FALSE;

	if(!m_pMap) return FALSE;
	if(dest_x >= m_sizeMap.cx || dest_y >= m_sizeMap.cy) return FALSE;

	BOOL bRet = FALSE;
	if(m_pMap[dest_x][dest_y].m_sEvent == 0)
		bRet = TRUE;
	else
		bRet = FALSE;
	return bRet;
	//return (BOOL)m_pMap[dest_x][dest_y].m_bMove;
}

///////////////////////////////////////////////////////////////////////
//	媛