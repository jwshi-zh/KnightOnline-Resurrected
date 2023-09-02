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
#include <vector>

enum {DUMMY_CENTER=0, DUMMY_X=1, DUMMY_Y=2, DUMMY_Z=3, NUM_DUMMY=4,DUMMY_NO=5};
const int NUM_CUBEVERTEX = 36;

struct __DUMMYCUBE
{
	int iType;																				// Center , X, Y, Z type classification.
	__VertexXyzNormalColor	Vertices[NUM_CUBEVERTEX];	  // construction points of the cube
	__Vector3	vCenterPos;														  // the center point of the cube
	float	fDistance;																	// distance from camera
};

class CPortalVolume;
enum e_SelectType	{	TYPE_VOLUME_TOTAL = 0, TYPE_VOLUME_ONLY, TYPE_SHAPE_ONLY	};
enum e_DummyType { DUMMY_TRANS = 0, DUMMY_POS, DUMMY_ROT, DUMMY_SCALE, DUMMY_SWAP };

typedef struct tagSelectElement{	
	e_SelectType eST;
	CN3Transform*	pSelectPointer;

	tagSelectElement()
	{
		pSelectPointer = NULL;
	}
} SelectElement;


class CTransDummy : public CN3Transform
{
public:
	CTransDummy();
	virtual ~CTransDummy();

// Attributes
public:
protected:
	e_DummyType m_ceDType;
	__DUMMYCUBE			m_DummyCubes[NUM_DUMMY];			// 4 cubes (center, x, y, z);
	__VertexXyzColor	m_LineVertices[6];										// The points that make up the line connecting the four cubes
public:
	e_DummyType m_cePrevType;
	CArray<SelectElement, SelectElement> m_SelObjArray;				// Objects selected in MapMng
protected:
	__DUMMYCUBE*		m_pSortedCubes[NUM_DUMMY];			// Pointers sorted by Cube distance
	__DUMMYCUBE*		m_pSelectedCube;								// selected cube
	__Vector3			m_vPrevPos;													// Position rotation zoom value before this mouse drag
	__Quaternion		m_qPrevRot;													// Position rotation zoom value before this mouse drag

	typedef std::vector<__Vector3> Tv;
	typedef std::vector<Tv>::iterator tvit;
	std::vector<Tv> m_vPrevScaleArray;											// Zoom values before this mouse drag (array because it must have values of all selected objects)

//	__Vector3*			m_vPrevScaleArray;										// Zoom values before this mouse drag (array because it must have values of all selected objects)

// Operations
public:
	e_DummyType GetDummyState()	{	return m_ceDType;	}
	virtual bool IsExistTotalVolByPointer(CPortalVolume* pVol);
	virtual CPortalVolume* GetFirstElementTotalVol();

	virtual void Tick();
	virtual void Render();
	virtual void Release();
	virtual void SetSelObj(SelectElement Obj, bool bOne = true);							// Replace selected object
	void	ClearObjs()	{ 
		m_SelObjArray.RemoveAll();
		tvit ti = m_vPrevScaleArray.begin();
		while(ti != m_vPrevScaleArray.end())
		{
			Tv tv = *ti++;
			tv.clear();
		}
		m_vPrevScaleArray.clear();
	}
	virtual void AddSelObj(SelectElement Obj);							// Add selected object
	virtual BOOL MouseMsgFilter(LPMSG pMsg);						// mouse message handling
	void GetPickRay(POINT point, __Vector3& vDir, __Vector3& vOrig);	// A function that gets a line extending towards the direction the mouse pointer is pointing at.
protected:
	__DUMMYCUBE* Pick(int x, int y);										// cube picking
	void InitDummyCube(int iType, __DUMMYCUBE* pDummyCube, __Vector3& vOffset, D3DCOLOR color);	// cube initialization
	static int SortCube(const void* pArg1, const void* pArg2);	// Sort function for cube camera distance
	virtual void TransDiff(__Vector3* pvDiffPos, __Quaternion* pvDiffRot, __Vector3* pvDiffScale);		// Transforms the selected objects by the difference.
};

#endif // !defined(AFX_TRANSDUMMY_H__EBD00CDE_FCE8_411E_A7A3_020D137904E2__INCLUDED_)
