// TransDummy.cpp: implementation of the CTransDummy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "n3me.h"
#include "TransDummy.h"

#include "../N3Base/Pick.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransDummy::CTransDummy()
{
	ZeroMemory(m_pSortedCubes, sizeof(m_pSortedCubes));

	const float fCubeOffset = 10.0f;
	InitDummyCube(DUMMY_CENTER, &(m_DummyCubes[DUMMY_CENTER]), __Vector3(0,0,0), D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0x00));
	InitDummyCube(DUMMY_X, &(m_DummyCubes[DUMMY_X]), __Vector3(fCubeOffset,0,0), D3DCOLOR_ARGB(0xff, 0xff, 0x00, 0x00));
	InitDummyCube(DUMMY_Y, &(m_DummyCubes[DUMMY_Y]), __Vector3(0,fCubeOffset,0), D3DCOLOR_ARGB(0xff, 0x00, 0xff, 0x00));
	InitDummyCube(DUMMY_Z, &(m_DummyCubes[DUMMY_Z]), __Vector3(0,0,fCubeOffset), D3DCOLOR_ARGB(0xff, 0x00, 0x00, 0xff));
	D3DCOLOR LineColor = D3DCOLOR_ARGB(0xff,0xaa,0xaa,0xaa);
	m_LineVertices[0].Set(0,0,0, LineColor);	m_LineVertices[1].Set(fCubeOffset,0,0, LineColor);
	m_LineVertices[2].Set(0,0,0, LineColor);	m_LineVertices[3].Set(0,fCubeOffset,0, LineColor);
	m_LineVertices[4].Set(0,0,0, LineColor);	m_LineVertices[5].Set(0,0,fCubeOffset, LineColor);
	m_vPrevScaleArray = NULL;

	Release();
}

CTransDummy::~CTransDummy()
{
	Release();
}

void CTransDummy::Release()
{
	m_SelObjArray.RemoveAll();
	m_pSelectedCube = NULL;
	m_vPrevPos.Set(0,0,0);
	m_qPrevRot.x = m_qPrevRot.y = m_qPrevRot.z = m_qPrevRot.w = 0;

	if (m_vPrevScaleArray) {delete [] m_vPrevScaleArray; m_vPrevScaleArray = NULL;}
}

void CTransDummy::InitDummyCube(int iType, __DUMMYCUBE* pDummyCube, __Vector3& vOffset, D3DCOLOR color)
{
	ASSERT(pDummyCube);
	const float fCubeSize = 1.0f;
	__Vector3	vCubeV[8], vCubeN[6];
	vCubeV[0].Set( fCubeSize, fCubeSize, fCubeSize);	vCubeV[1].Set( fCubeSize, fCubeSize,-fCubeSize);	vCubeV[2].Set(-fCubeSize, fCubeSize,-fCubeSize);
	vCubeV[3].Set(-fCubeSize, fCubeSize, fCubeSize);	vCubeV[4].Set( fCubeSize,-fCubeSize, fCubeSize);	vCubeV[5].Set( fCubeSize,-fCubeSize,-fCubeSize);
	vCubeV[6].Set(-fCubeSize,-fCubeSize,-fCubeSize);	vCubeV[7].Set(-fCubeSize,-fCubeSize, fCubeSize);
	vCubeN[0].Set( 1, 0, 0);	vCubeN[1].Set( 0, 1, 0);	vCubeN[2].Set( 0, 0, 1);
	vCubeN[3].Set(-1, 0, 0);	vCubeN[4].Set( 0,-1, 0);	vCubeN[5].Set( 0, 0,-1);

	pDummyCube->Vertices[0].Set(vCubeV[0], vCubeN[0], color);	pDummyCube->Vertices[1].Set(vCubeV[4], vCubeN[0], color);	pDummyCube->Vertices[2].Set(vCubeV[5], vCubeN[0], color);
	pDummyCube->Vertices[3].Set(vCubeV[1], vCubeN[0], color);	pDummyCube->Vertices[4].Set(vCubeV[0], vCubeN[0], color);	pDummyCube->Vertices[5].Set(vCubeV[5], vCubeN[0], color);
	pDummyCube->Vertices[6].Set(vCubeV[0], vCubeN[1], color);	pDummyCube->Vertices[7].Set(vCubeV[1], vCubeN[1], color);	pDummyCube->Vertices[8].Set(vCubeV[2], vCubeN[1], color);
	pDummyCube->Vertices[9].Set(vCubeV[0], vCubeN[1], color);	pDummyCube->Vertices[10].Set(vCubeV[2], vCubeN[1], color);	pDummyCube->Vertices[11].Set(vCubeV[3], vCubeN[1], color);
	pDummyCube->Vertices[12].Set(vCubeV[0], vCubeN[2], color);	pDummyCube->Vertices[13].Set(vCubeV[7], vCubeN[2], color);	pDummyCube->Vertices[14].Set(vCubeV[4], vCubeN[2], color);
	pDummyCube->Vertices[15].Set(vCubeV[0], vCubeN[2], color);	pDummyCube->Vertices[16].Set(vCubeV[3], vCubeN[2], color);	pDummyCube->Vertices[17].Set(vCubeV[7], vCubeN[2], color);
	pDummyCube->Vertices[18].Set(vCubeV[6], vCubeN[3], color);	pDummyCube->Vertices[19].Set(vCubeV[3], vCubeN[3], color);	pDummyCube->Vertices[20].Set(vCubeV[2], vCubeN[3], color);
	pDummyCube->Vertices[21].Set(vCubeV[6], vCubeN[3], color);	pDummyCube->Vertices[22].Set(vCubeV[7], vCubeN[3], color);	pDummyCube->Vertices[23].Set(vCubeV[3], vCubeN[3], color);
	pDummyCube->Vertices[24].Set(vCubeV[6], vCubeN[4], color);	pDummyCube->Vertices[25].Set(vCubeV[5], vCubeN[4], color);	pDummyCube->Vertices[26].Set(vCubeV[4], vCubeN[4], color);
	pDummyCube->Vertices[27].Set(vCubeV[6], vCubeN[4], color);	pDummyCube->Vertices[28].Set(vCubeV[4], vCubeN[4], color);	pDummyCube->Vertices[29].Set(vCubeV[7], vCubeN[4], color);
	pDummyCube->Vertices[30].Set(vCubeV[6], vCubeN[5], color);	pDummyCube->Vertices[31].Set(vCubeV[2], vCubeN[5], color);	pDummyCube->Vertices[32].Set(vCubeV[1], vCubeN[5], color);
	pDummyCube->Vertices[33].Set(vCubeV[6], vCubeN[5], color);	pDummyCube->Vertices[34].Set(vCubeV[1], vCubeN[5], color);	pDummyCube->Vertices[35].Set(vCubeV[5], vCubeN[5], color);

	int i;
	for (i=0; i<NUM_CUBEVERTEX; ++i)
	{
		pDummyCube->Vertices[i].x += vOffset.x;
		pDummyCube->Vertices[i].y += vOffset.y;
		pDummyCube->Vertices[i].z += vOffset.z;
	}
	pDummyCube->iType = iType;
	pDummyCube->vCenterPos = vOffset;
}

void CTransDummy::Tick()
{
	if (m_SelObjArray.GetSize()==0) return;
	// Scale 議곗