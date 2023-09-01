// MAP.h: interface for the MAP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAP_H__BCC14687_C38F_4597_8522_2170ED077037__INCLUDED_)
#define AFX_MAP_H__BCC14687_C38F_4597_8522_2170ED077037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <N3ShapeMgr.h>
#include "STLMap.h"
#include "RoomEvent.h"

typedef CSTLMap <_OBJECT_EVENT>		ObjectEventArray;
typedef	map < int, int* >::iterator		Iterator;
typedef CSTLMap <CRoomEvent>		RoomEventArray;

class CRegion;
class CNpc;
class CUser;
class CServerDlg;
//class CRoomEvent;

class CMapInfo					// Information of each coordinate
{
public:
	short	m_sEvent;			// Event number in current coordinates

	CMapInfo();
	virtual ~CMapInfo();
};

class MAP  
{
public:
	CServerDlg*	m_pMain;
	CN3ShapeMgr m_N3ShapeMgr;
	CMapInfo**		m_pMap;					// Tile information (1 cell: 4 meters)
	CRegion**		m_ppRegion;				// 64 meters of tile information..
	//CRoomEvent*		m_pRoomEvent;
	CSize			m_sizeMap;				// the size of the map
	CSize			m_sizeRegion;			// map's region size
	int m_nZoneNumber;						// zone number
	int	m_nServerNo;
	char m_MapName[256];
	int			m_nMapSize;		// Grid Unit ex) 4m
	float		m_fUnitDist;	// i Grid Distance
	float**		m_fHeight;
//	short		m_arDungeonBossMonster[MAX_DUNGEON_BOSS_MONSTER];
	BYTE		m_byRoomType;		// Room initialization related ( 0: automatic initialization, 1: war event related (initialization when specific conditions are completed)
	BYTE		m_byRoomEvent;		// event room(0:empty, 1:use)
	BYTE		m_byRoomStatus;		// room status (1: In progress, 2: Initializing the room, 3: Initializing the room)
	BYTE		m_byInitRoomCount;	// Control room initialization time (to keep in sync with monsters)
	ObjectEventArray m_ObjectEventArray;
	RoomEventArray	 m_arRoomEventArray;
	short	m_sKarusRoom;			// number of karus surnames
	short	m_sElmoradRoom;			// The number of surnames in elmorad

public:
	MAP();
	virtual ~MAP();

	BOOL LoadMap( HANDLE hFile );
	void LoadTerrain( HANDLE hFile );
	void LoadMapTile( HANDLE hFile );
	void LoadObjectEvent(HANDLE hFile);
	BOOL LoadRoomEvent( int zone_number );
	BOOL ObjectIntersect(float x1, float z1, float y1, float x2, float z2, float y2);
	float GetHeight( float x, float z );

	Iterator RegionNpcRemove( int rx, int rz, int nid );
	void RegionNpcAdd( int rx, int rz, int nid );
	Iterator RegionUserRemove( int rx, int rz, int uid );
	void RegionUserAdd( int rx, int rz, int uid );
	BOOL RegionItemRemove( int rx, int rz, int itemid, int count, int index );
	void RegionItemAdd( int rx, int rz, int itemid, int count, int index );
	int  GetRegionUserSize(int rx, int rz);
	int  GetRegionNpcSize(int rx, int rz);

	int GetXRegionMax() {return m_sizeRegion.cx-1;};
	int GetZRegionMax() {return m_sizeRegion.cy-1;};

	int IsRoomCheck(float fx, float fz);	// Used in the dungeon, determining where the user's current location is in the dungeon
	BOOL IsRoomStatusCheck();

	BOOL IsMovable(int dest_x, int dest_y);
	void InitializeRoom();

	CRoomEvent* SetRoomEvent( int number );

protected:
	void RemoveMapData();

};

#endif // !defined(AFX_MAP_H__BCC14687_C38F_4597_8522_2170ED077037__INCLUDED_)

