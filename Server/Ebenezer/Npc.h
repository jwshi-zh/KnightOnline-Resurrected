// Npc.h: interface for the CNpc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPC_H__1DE71CDD_4040_4479_828D_E8EE07BD7A67__INCLUDED_)
#define AFX_NPC_H__1DE71CDD_4040_4479_828D_E8EE07BD7A67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "define.h"

class CEbenezerDlg;

class CNpc  
{
public:
	CEbenezerDlg* m_pMain;

	short	m_sNid;				// NPC (on server) serial number
	short	m_sSid;				// NPC table reference number
	short	m_sCurZone;			// Current Zone;
	short	m_sZoneIndex;		// The index of the zone where the NPC exists
	float	m_fCurX;			// Current X Pos;
	float	m_fCurY;			// Current Y Pos;
	float	m_fCurZ;			// Current Z Pos;
	float	m_fDir;				// The direction the NPC is looking
	short	m_sPid;				// MONSTER(NPC) Picture ID
	short	m_sSize;			// MONSTER(NPC) Size
	int		m_iWeapon_1;
	int		m_iWeapon_2;
	TCHAR	m_strName[MAX_ID_SIZE];		// MONSTER(NPC) Name
	int		m_iMaxHP;
	int		m_iHP;
	BYTE	m_byState;
	BYTE	m_byGroup;
	BYTE	m_byLevel;
	BYTE	m_tNpcType;			// NPC Type
								// 0 : Normal Monster
								// 1 : NPC
								// 2 : Each entrance and exit NPC
								// 3 : guard
	int   m_iSellingGroup;		// ItemGroup
//	DWORD	m_dwStepDelay;		

	short m_sRegion_X;			// region x position
	short m_sRegion_Z;			// region z position
	BYTE	m_NpcState;			// NPC's status - alive, dead, standing, etc...
	BYTE	m_byGateOpen;		// Gate Npc Status -> 1 : open 0 : close
	short   m_sHitRate;			// attack success rate
	BYTE    m_byObjectType;     // Usually 0, object type (gate, lever) is 1

	short   m_byEvent;		    // This is for the quest. 

public:
	CNpc();
	virtual ~CNpc();

	void Initialize();
	void MoveResult(float xpos, float ypos, float zpos, float speed);
	void NpcInOut(BYTE Type, float fx, float fz, float fy);
	void RegisterRegion();
	void RemoveRegion(int del_x, int del_z);
	void InsertRegion(int del_x, int del_z);
	int GetRegionNpcList(int region_x, int region_z, char *buff, int &t_count);
	BYTE GetState()		{ return m_byState; };

};

#endif // !defined(AFX_NPC_H__1DE71CDD_4040_4479_828D_E8EE07BD7A67__INCLUDED_)
