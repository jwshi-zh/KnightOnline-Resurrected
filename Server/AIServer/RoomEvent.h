// RoomEvent.h: interface for the CRoomEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROOMEVENT_H__70001CA8_64B8_422F_B0F4_ED0F2FA95E0E__INCLUDED_)
#define AFX_ROOMEVENT_H__70001CA8_64B8_422F_B0F4_ED0F2FA95E0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "STLMap.h"

#define MAX_CHECK_EVENT	5

typedef CSTLMap <int>			mapNpcArray;

struct _RoomEvent
{
	short	sNumber;			// command, condition number
	short	sOption_1;			// option 1 (usually has the number of the monster)
	short	sOption_2;			// option 2 (number of monsters)
};

class CNpc;
class CServerDlg;

class CRoomEvent  
{
public:
	int     m_iZoneNumber;		// zone number
	short	m_sRoomNumber;		// room number (0:empty room)
	BYTE	m_byStatus;			// room status (1:init, 2:progress, 3:clear)
	BYTE	m_byCheck;			// number of conditionals
	BYTE	m_byRoomType;		// Room type (0: normal, 1: trap room, 2:,,,,)

	int		m_iInitMinX;		// room region x
	int		m_iInitMinZ;
	int		m_iInitMaxX;
	int		m_iInitMaxZ;

	int		m_iEndMinX;			// room end region x arrival point,,
	int		m_iEndMinZ;
	int		m_iEndMaxX;
	int		m_iEndMaxZ;

	_RoomEvent  m_Logic[MAX_CHECK_EVENT];		// conditions
	_RoomEvent  m_Exec[MAX_CHECK_EVENT];		// execution statement

	float   m_fDelayTime;						// time

	mapNpcArray	m_mapRoomNpcArray;				// room npc uid array
	CServerDlg* m_pMain;

private:
	BYTE    m_byLogicNumber;	// Current condition statement check number (Condition number starts from 1) (clear state if m_byCheck and m_byLogicNumber are the same)


public:
	CRoomEvent();
	virtual ~CRoomEvent();

	void MainRoom( float fcurtime );
	void InitializeRoom();

private:
	void Initialize();
	BOOL  CheckEvent( int event_num, float fcurtime );
	BOOL  RunEvent( int event_num );
	BOOL  CheckMonsterCount( int sid, int count, int type );
	CNpc* GetNpcPtr( int sid );
	void  EndEventSay( int option1, int option2 );

};

#endif // !defined(AFX_ROOMEVENT_H__70001CA8_64B8_422F_B0F4_ED0F2FA95E0E__INCLUDED_)
