// Npc.cpp: implementation of the CNpc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ebenezer.h"
#include "Npc.h"
#include "EbenezerDlg.h"
#include "Map.h"
#include "PacketDefine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CRITICAL_SECTION g_region_critical;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNpc::CNpc()
{

}

CNpc::~CNpc()
{

}

void CNpc::Initialize()
{
	m_pMain = (CEbenezerDlg*)AfxGetMainWnd();

	m_sNid = -1;				// NPC (