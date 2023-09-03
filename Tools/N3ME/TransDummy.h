// TransDummy.h: interface for the CTransDummy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSDUMMY_H__EBD00CDE_FCE8_411E_A7A3_020D137904E2__INCLUDED_)
#define AFX_TRANSDUMMY_H__EBD00CDE_FCE8_411E_A7A3_020D137904E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../N3Base/N3Transform.h"
#include <afxtempl.h>

enum {DUMMY_CENTER=0, DUMMY_X=1, DUMMY_Y=2, DUMMY_Z=3, NUM_DUMMY=4,DUMMY_NO=5};
const int NUM_CUBEVERTEX = 36;

struct __DUMMYCUBE
{
	int iType;		// Center , X, Y, Z type classification.
	__VertexXyzNormalColor	Vertices[NUM_CUBEVERTEX];	// construction points of the cube
	__Vector3				vCenterPos;	// the center point of the cube
	float					fDistance;	// distance from camera
};

class CLyTerrain;

class CTransDummy : public CN3Transform
{
public:
	CLyTerrain* m_pTerrainRef;

	CTransDummy();
	virtual ~CTransDummy();

// Attributes
public:
protected:
	__DUMMYCUBE			m_DummyCubes[NUM_DUMMY];	// 4 cubes (center, x, y, z);
	__VertexXyzColor	m_LineVertices[6];			// The points that make up the line connecting the four cubes
	CTypedPtrArray<CPtrArray, CN3Transform*> m_SelObjArray;	// Objects selected in MapMng
	__DUMMYCUBE*		m_pSortedCubes[NUM_DUMMY];	// Pointers sorted by Cube distance
	__DUMMYCUBE*		m_pSelectedCube;			// selected cube
	__Vector3			m_vPrevPos;	// Position rotation zoom value before this mouse drag
	__Quaternion		m_qPrevRot;	// Position rotation zoom value before this mouse drag
	__Vector3*			m_vPrevScaleArray;	// Zoom values before this mouse drag (array because it must have values of all selected objects)

// Operations
public:
	virtual void Tick();
	virtual void Render();
	virtual void Release();
	virtual void SetSelObj(CN3Transform* pObj);		// Replace selected object
	virtual void AddSelObj(CN3Transform* pObj);	// Add selected object
	virtual BOOL MouseMsgFilter(LPMSG pMsg);	// mouse message handling
	void GetPickRay(POINT point, __Vector3& vDir, __Vector3& vOrig);	// A function that gets a line extending towards the direction the mouse pointer is pointing at.
protected:
	__DUMMYCUBE* Pick(int x, int y);	// cube picking
	void InitDummyCube(int iType, __DUMMYCUBE* pDummyCube, __Vector3& vOffset, D3DCOLOR color);	// cube initialization
	static int SortCube(const void* pArg1, const void* pArg2);	// Sort function for cube camera distance
	virtual void TransDiff(__Vector3* pvDiffPos, __Quaternion* pvDiffRot, __Vector3* pvDiffScale);		// Transforms the selected objects by the difference.
};

#endif // !defined(AFX_TRANSDUMMY_H__EBD00CDE_FCE8_411E_A7A3_020D137904E2__INCLUDED_)
