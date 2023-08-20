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
	m_sMaxHP = ini.GetProfileInt("MAIN", "理