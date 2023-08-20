// PathFind.cpp: implementation of the CPathFind class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "server.h"
#include "PathFind.h"
#include "math.h"
#include "Serverdlg.h"

//#include "Extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define LEVEL_ONE_FIND_CROSS			2
#define LEVEL_ONE_FIND_DIAGONAL			3
#define LEVEL_TWO_FIND_CROSS			11
#define LEVEL_TWO_FIND_DIAGONAL			10

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPathFind::CPathFind()
{
	m_pStack = (STACK *)calloc(1, sizeof(STACK));
	m_pOpen = NULL;
	m_pClosed = NULL;
	m_pMap = NULL;
	m_lMapUse = 0;
	m_pMain = (CServerDlg*)AfxGetMainWnd();
}

CPathFind::~CPathFind()
{
	ClearData();
	free(m_pStack);
}

void CPathFind::ClearData()
{
	_PathNode *t_node1, *t_node2;

	if(m_pOpen)
	{
		t_node1 = m_pOpen->NextNode;
		while(t_node1)
		{
			t_node2 = t_node1->NextNode;
			free(t_node1);
			t_node1 = t_node2;
		}
		free(m_pOpen);
		m_pOpen = NULL;
	}

	if(m_pClosed)
	{
		t_node1 = m_pClosed->NextNode;
		while(t_node1)
		{
			t_node2 = t_node1->NextNode;
			free(t_node1);
			t_node1 = t_node2;
		}
		free(m_pClosed);
		m_pClosed = NULL;
	}
}

void CPathFind::SetMap(int x, int y, int *pMap)
{
	m_vMapSize.cx = x;
	m_vMapSize.cy = y;
	m_pMap = pMap;
/*	if(InterlockedCompareExchange(&m_lMapUse, (LONG)1, (LONG)0) == 0)
	{
		m_vMapSize.cx = x;
		m_vMapSize.cy = y;
		m_pMap = map;
		::InterlockedExchange(&m_lMapUse, 0);
	}
	else TRACE("