// NpcTable.cpp: implementation of the CNpcTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Server.h"
#include "NpcTable.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNpcTable::CNpcTable()
{

}

CNpcTable::~CNpcTable()
{

}

void CNpcTable::Initialize()
{
	m_sSid = -1;				// MONSTER(NPC) Serial ID
	memset(m_strName, 0x00, MAX_ID_SIZE+1);		// MONSTER(NPC) Name
	m_sPid = 0;
	m_sSize = 0;
	m_iWeapon_1 = 0;
	m_iWeapon_2 = 0;
	m_byGroup = 0;
	m_byActType = 0;
	m_byRank = 0;
	m_byTitle = 0;
	m_iSellingGroup = 0;
	m_sLevel = 0;
	m_iExp = 0;
	m_iLoyalty = 0;
	m_iMaxHP = 0;
	m_sMaxMP = 0;
	m_sAttack = 0;
	m_sDefense = 0;
	m_sHitRate = 0;
	m_sEvadeRate = 0;
	m_sDamage = 0;
	m_sAttackDelay = 0;
	m_sSpeed = 0;
	m_sStandTime = 0;
	m_iMagic1 = 0;
	m_iMagic2 = 0;
	m_iMagic3 = 0;
	m_byFireR = 0;
	m_byColdR = 0;
	m_byLightR = 0;
	m_byMagicR = 0;
	m_byDiseaseR = 0;
	m_byPoisonR = 0;
	m_sBulk  = 0;
	m_bySearchRange = 0;
	m_byAttackRange = 0;
	m_byTracingRange = 0;
	m_sAI = 0;
	m_tNpcType = 0;
	m_byFamilyType = 0;
	m_tItemPer = 0;
	m_tDnPer = 0;
	m_iMoney = 0;
	m_iItem = 0;
	m_byDirectAttack = 0;
	m_byMagicAttack = 0;
}
