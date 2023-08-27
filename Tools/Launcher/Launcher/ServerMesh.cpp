// ServerMesh.cpp: implementation of the CServerMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ServerMesh.h"
#include "N3Terrain.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const int SGRID_SIZE = 64;
static const int SMAP_SIZE  = 4096;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerMesh::CServerMesh()
{

}

CServerMesh::~CServerMesh()
{

}



void CServerMesh::Tick(CN3Terrain* pTerrain, const __Vector3& vPosPlayer)
{
	__Vector3 pos = vPosPlayer;
	int ixposL, ixposR, izposL, izposR, ixpos, izpos;
	float fHeightLB, fHeightRB, fHeightTop, fHeightBottom;	

	ixpos   = ((int)pos.x)/SGRID_SIZE; 
	ixpos *= SGRID_SIZE;
	izpos   = ((int)pos.z)/SGRID_SIZE; 
	izpos *= SGRID_SIZE;

	D3DCOLOR color = D3DCOLOR_RGBA(50,25,25,0);

	// 