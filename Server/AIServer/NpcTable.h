// NpcTable.h: interface for the CNpcTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPCTABLE_H__07FF861B_C3CB_4EE8_B64F_E9796B110101__INCLUDED_)
#define AFX_NPCTABLE_H__07FF861B_C3CB_4EE8_B64F_E9796B110101__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNpcTable  
{
public:
	short	m_sSid;
	TCHAR	m_strName[MAX_ID_SIZE+1];
	short	m_sPid;
	short   m_sSize;
	int 	m_iWeapon_1;
	int 	m_iWeapon_2;
	BYTE	m_byGroup;
	BYTE	m_byActType;
	BYTE	m_tNpcType;
	BYTE	m_byFamilyType;
	BYTE	m_byRank;
	BYTE	m_byTitle;
	int 	m_iSellingGroup;
	short	m_sLevel;
	int		m_iExp;
	int		m_iLoyalty;

	int 	m_iMaxHP;
	short	m_sMaxMP;

	short	m_sAttack;
	short	m_sDefense;
	short	m_sHitRate;
	short   m_sEvadeRate;
	short	m_sDamage;
	short	m_sAttackDelay;
	short	m_sSpeed;
	BYTE    m_bySpeed_1;
	BYTE    m_bySpeed_2;
	short	m_sStandTime;
	int		m_iMagic1;
	int		m_iMagic2;
	int		m_iMagic3;

	BYTE	m_byFireR;
	BYTE	m_byColdR;
	BYTE	m_byLightningR;
	BYTE	m_byMagicR;
	BYTE	m_byDiseaseR;
	BYTE	m_byPoisonR;
	BYTE	m_byLightR;
	short	m_sBulk;

	BYTE	m_bySearchRange;
	BYTE	m_byAttackRange;
	BYTE	m_byTracingRange;

	short	m_sAI;
	
	
	BYTE	m_tItemPer;
	BYTE	m_tDnPer;

	int		m_iMoney;
	int		m_iItem;
	BYTE	m_byDirectAttack;
	BYTE	m_byMagicAttack;
		
public:
	CNpcTable();
	virtual ~CNpcTable();

	void Initialize();

};

#endif // !defined(AFX_NPCTABLE_H__07FF861B_C3CB_4EE8_B64F_E9796B110101__INCLUDED_)
