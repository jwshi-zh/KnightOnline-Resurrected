// Map.cpp: implementation of the CMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ebenezer.h"
#include "Map.h"
#include "Region.h"
#include "Define.h"
#include "User.h"
#include "EventSet.h"
#include "EbenezerDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CRITICAL_SECTION g_region_critical;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DMap::C3DMap()
{
	m_nMapSize = 0;
	m_fUnitDist = 0.0f;
	m_fHeight = NULL;

	m_nXRegion = 0;
	m_nZRegion = 0;

	m_ppRegion = NULL;
	m_nZoneNumber = 0;
	m_bType = 0;
	m_wBundle = 1;
	m_sMaxUser = 150;	// Max user in Battlezone!!!
	memset( m_MapName, NULL, 256 );
	m_pMain = NULL;
}

C3DMap::~C3DMap()
{
	if( m_ppRegion ) {
		for(int i=0; i< m_nXRegion; i++) {
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

	if( m_ppnEvent ) {
		for( int i=0; i<m_nMapSize; i++ ) {
			delete[] m_ppnEvent[i];
			m_ppnEvent[i] = NULL;
		}
		delete[] m_ppnEvent;
		m_ppnEvent = NULL;
	}

	if( !m_EventArray.IsEmpty() )
		m_EventArray.DeleteAllData();
	if( !m_ObjectEventArray.IsEmpty() )
		m_ObjectEventArray.DeleteAllData();
	if( !m_ObjectRegeneArray.IsEmpty() )
		m_ObjectRegeneArray.DeleteAllData();
	if( !m_WarpArray.IsEmpty() )
		m_WarpArray.DeleteAllData();

	DeleteCriticalSection( &g_region_critical );
}

BOOL C3DMap::LoadMap(HANDLE hFile)
{
	m_pMain = (CEbenezerDlg*)AfxGetMainWnd();

	LogFileWrite("load teerr\r\n");

	LoadTerrain( hFile );
	m_N3ShapeMgr.Create((m_nMapSize - 1)*m_fUnitDist, (m_nMapSize-1)*m_fUnitDist);
	if( !m_N3ShapeMgr.LoadCollisionData(hFile) )
		return FALSE;

	if(	(m_nMapSize - 1) * m_fUnitDist != m_N3ShapeMgr.Width() || 
		(m_nMapSize - 1) * m_fUnitDist != m_N3ShapeMgr.Height() )
	{
		return FALSE;
	}

	LogFileWrite("mapfile adfasfdasdd\r\n");
	int mapwidth = (int)m_N3ShapeMgr.Width();

	m_nXRegion = (int)(mapwidth/VIEW_DISTANCE) + 1;
	m_nZRegion = (int)(mapwidth/VIEW_DISTANCE) + 1;

	m_ppRegion = new CRegion*[m_nXRegion];
	for(int i=0; i<m_nXRegion; i++) {
		m_ppRegion[i] = new CRegion[m_nZRegion];
	}

	LoadObjectEvent(hFile);
	LogFileWrite("amp tile\r\n");
	LoadMapTile(hFile);
	LogFileWrite("regene\r\n");
	LoadRegeneEvent(hFile);		// 