// N3ShapeMgr.cpp: implementation of the CN3ShapeMgr class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _N3GAME
#include "../WarFare/StdAfx.h"
#include "../WarFare/GameProcedure.h"
#include "../WarFare/UILoading.h"
#else
#include "StdAfxBase.h"
#endif // end of #ifndef _N3GAME

#include "N3ShapeMgr.h"

#ifndef _3DSERVER
#include "N3ShapeExtra.h"
#endif // end of #ifndef _3DSERVER

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CN3ShapeMgr::CN3ShapeMgr()
{
	m_fMapWidth = 0.0f;
	m_fMapLength = 0.0f;
	m_nCollisionFaceCount = 0;
	m_pvCollisions = NULL;
	memset(m_pCells, 0, sizeof(m_pCells));
}

CN3ShapeMgr::~CN3ShapeMgr()
{
#ifndef _3DSERVER
	int iSC = m_Shapes.size();
	for(int i = 0; i < iSC; i++) delete m_Shapes[i];
	m_Shapes.clear();
#endif // end of #ifndef _3DSERVER

	delete [] m_pvCollisions; m_pvCollisions = NULL;
	for(int z = 0; z < MAX_CELL_MAIN; z++)
	{
		for(int x = 0; x < MAX_CELL_MAIN; x++)
		{
			delete m_pCells[x][z];
		}
	}
}

void CN3ShapeMgr::Release()
{
#ifndef _3DSERVER
	this->ReleaseShapes();
#endif // end of #ifndef _3DSERVER

	m_fMapWidth = 0.0f;
	m_fMapLength = 0.0f;
	m_nCollisionFaceCount = 0;
	delete [] m_pvCollisions; m_pvCollisions = NULL;
	for(int z = 0; z < MAX_CELL_MAIN; z++)
	{
		for(int x = 0; x < MAX_CELL_MAIN; x++)
		{
			delete m_pCells[x][z];
			m_pCells[x][z] = NULL;
		}
	}
	memset(m_pCells, 0, sizeof(MAX_CELL_MAIN));

#ifndef _3DSERVER
	m_ShapesToRender.clear();
#endif // end of #ifndef _3DSERVER

#ifdef _N3TOOL
	m_CollisionExtras.clear(); // 異