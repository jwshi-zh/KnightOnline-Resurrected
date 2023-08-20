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
	m_iWeapon_1 = 0;			// 臾닿린怨