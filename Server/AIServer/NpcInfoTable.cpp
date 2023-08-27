// NpcInfoTable.cpp: implementation of the CNpcInfoTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Server.h"
#include "NpcInfoTable.h"
#include "ini.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNpcInfoTable::CNpcInfoTable()
{

}

CNpcInfoTable::~CNpcInfoTable()
{

}

void CNpcInfoTable::Initialize()
{
	CIni ini;
	ini.SetPath("mon1.ini");
	
	m_sSid = ini.GetProfileInt("MAIN", "sSid", 0);
	strcpy(m_strName, ini.GetProfileString("MAIN", "strName", "monster"));
	m_sMaxHP = ini.GetProfileInt("MAIN", "Max HP", 0);
	m_sMaxMP = ini.GetProfileInt("MAIN", "Max MP", 0);
	m_byClass = ini.GetProfileInt("MAIN", "weapon type", 0);
	m_iExp = ini.GetProfileInt("MAIN", "Experience", 0);
	m_sDefense = ini.GetProfileInt("MAIN", "defense value", 0);
	m_byAttackRange = ini.GetProfileInt("MAIN", "range", 0);
	m_sAI = ini.GetProfileInt("MAIN", "artificial intelligence index", 0);
	m_sAttackDelay = ini.GetProfileInt("MAIN", "attack delay", 0);
	m_bySearchRange = ini.GetProfileInt("MAIN", "enemy detection range", 0);
	m_sSpeed = ini.GetProfileInt("MAIN", "speed", 0);
	m_sStandTime = ini.GetProfileInt("MAIN", "StandTime", 0);
	m_tNpcType = ini.GetProfileInt("MAIN", "NpcType", 0);
	m_sFamilyType = ini.GetProfileInt("MAIN", "FamilyType", 0);
	m_tItemPer = ini.GetProfileInt("MAIN", "ItemPer", 0);
	m_tDnPer = ini.GetProfileInt("MAIN", "DnPer", 0);

}
