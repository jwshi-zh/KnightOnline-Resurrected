// MagicTable.h: interface for the CMagicTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAGICTABLE_H__3BEFBF64_0058_48B1_AB56_D98E86016DFE__INCLUDED_)
#define AFX_MAGICTABLE_H__3BEFBF64_0058_48B1_AB56_D98E86016DFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMagicTable  
{
public:
	CMagicTable();
	virtual ~CMagicTable();

public:
	short	m_sNum;
	char	m_strName[30];
	BYTE	m_bMoral;
	BYTE	m_bClass;
	BYTE	m_bLevel;
	BYTE	m_bEffect;			// (0:fire, 1:ice, 2:lightning, 3:magic, 4:disease, 5:poison, 6:light, 7:heal, 8:dagger, 9:sword, 10:mace, 11:spear, 12:polearm)
	short	m_sMp;
	short	m_sHp;
	short	m_sSt;
	int		m_iNItem;			// Required item (when used)
	short	m_sDistant;
	short	m_sContime;			// duration (seconds)
	short	m_sRepeat;			// Reusable time (seconds)
	BYTE	m_bExpect;			// success rate
	BYTE	m_bTarget;			// Target (0: target, 1: own radius, 2: target and radius, 3: point and radius, 4: object)
	BYTE	m_bRange;
	BYTE	m_bType;			// Type (0: Attack, 1: Stat Change, 2: Abnormal Control, 3: Graphic Transformation, 4: Character Control, 5: Teleport/Summon, 6: Shield, 7: Cancel Magic, 8: Combat Style)
	BYTE	m_bRate1;			// Application rate 1 (one decimal place, that is, it must be calculated by dividing by 10 when calculating)
	BYTE	m_bVar1;			// Applied variable 1
	BYTE	m_bLimit1;			// skill limit 1
	BYTE	m_bRate2;			// Application rate 2 (one decimal place, that is, it must be calculated by dividing by 10 when calculating)
	BYTE	m_bVar2;			// Applied variable 2
	BYTE	m_bLimit2;			// skill limit 2
	BYTE	m_bRate3;			// Application rate 3 (one decimal place, that is, it must be calculated by dividing by 10 when calculating)
	BYTE	m_bVar3;			// Applied variable 3
	BYTE	m_bLimit3;			// skill limit 3

};

#endif // !defined(AFX_MAGICTABLE_H__3BEFBF64_0058_48B1_AB56_D98E86016DFE__INCLUDED_)
