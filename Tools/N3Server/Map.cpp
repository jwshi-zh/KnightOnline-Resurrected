// Map.cpp: implementation of the CMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "3DServer.h"
#include "Map.h"
#include "Region.h"
#include "Define.h"
#include ".\N3Base\N3VMesh.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DMap::C3DMap()
{
	m_pVMesh = NULL;
	m_nMapSize = 0;
	m_fUnitDist = 0.0f;
	m_nVMeshCount = 0;
	m_vNormal = NULL;
	m_fHeight = NULL;

	m_nXRegion = 0;
	m_nZRegion = 0;
}

C3DMap::~C3DMap()
{
	if( m_vNormal ) {
		delete[] m_vNormal;
		m_vNormal = NULL;
	}
	if( m_fHeight ) {
		delete[] m_fHeight;
		m_fHeight = NULL;
	}
	if( m_pVMesh ) {
		delete[] m_pVMesh;
		m_pVMesh = NULL;
	}

	if( m_ppRegion ) {
		for(int i=0; i< m_nXRegion; i++) {
			delete[] m_ppRegion[i];
			m_ppRegion[i] = NULL;
		}
		delete[] m_ppRegion;
		m_ppRegion = NULL;
	}
}

BOOL C3DMap::LoadMap(HANDLE hFile)
{
	DWORD dwNum;
	BOOL ret = FALSE;

	LoadTerrain(hFile);	// 吏