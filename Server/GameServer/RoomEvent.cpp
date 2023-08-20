// RoomEvent.cpp: implementation of the CRoomEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "server.h"
#include "RoomEvent.h"
#include "ServerDlg.h"
#include "define.h"
//#include "Npc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern CRITICAL_SECTION g_region_critical;

CRoomEvent::CRoomEvent()
{
	m_iZoneNumber = 0;
	m_sRoomNumber = 0;
	m_byStatus	= 1;
	m_iInitMinX = 0;
	m_iInitMinZ = 0;
	m_iInitMaxX = 0;
	m_iInitMaxZ = 0;
	m_iEndMinX = 0;
	m_iEndMinZ = 0;
	m_iEndMaxX = 0;
	m_iEndMaxZ = 0;
	m_byCheck = 0;
	m_byRoomType = 0;
	m_pMain = (CServerDlg*) AfxGetMainWnd();

	Initialize();
}

CRoomEvent::~CRoomEvent()
{
	if( !m_mapRoomNpcArray.IsEmpty() )	
		m_mapRoomNpcArray.DeleteAllData();
}

void CRoomEvent::Initialize()
{
	m_fDelayTime = 0.0f;
	m_byLogicNumber = 1;

	for(int i=0; i<MAX_CHECK_EVENT; i++)	{
		m_Logic[i].sNumber = 0;
		m_Logic[i].sOption_1 = 0;
		m_Logic[i].sOption_2 = 0;
		m_Exec[i].sNumber = 0;
		m_Exec[i].sOption_1 = 0;
		m_Exec[i].sOption_2 = 0;
	}
}

void CRoomEvent::MainRoom( float fcurtime )
{
	// 議곌굔 寃