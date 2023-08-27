// NpcInfoTable.h: interface for the CNpcInfoTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPCINFOTABLE_H__5F61A01D_DBD7_40CE_8728_03E662E44182__INCLUDED_)
#define AFX_NPCINFOTABLE_H__5F61A01D_DBD7_40CE_8728_03E662E44182__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNpcInfoTable  
{
public:
	short	m_sSid;				// MONSTER(NPC) Serial ID
	TCHAR	m_strName[20];		// MONSTER(NPC) Name

	short	m_sMaxHP;
	short	m_sMaxMP;
	
	BYTE	m_byClass;

	int		m_iExp;

	short	m_sDefense;
	BYTE	m_byAttackRange;

	int		m_sAI;
	int		m_sAttackDelay;

	BYTE	m_bySearchRange;
	int		m_sSpeed;

	int		m_sStandTime;
	BYTE	m_tNpcType;

	int		m_sFamilyType;
	BYTE	m_tItemPer;
	BYTE	m_tDnPer;

public:
	void Initialize();
	CNpcInfoTable();
	virtual ~CNpcInfoTable();

};

#endif // !defined(AFX_NPCINFOTABLE_H__5F61A01D_DBD7_40CE_8728_03E662E44182__INCLUDED_)
