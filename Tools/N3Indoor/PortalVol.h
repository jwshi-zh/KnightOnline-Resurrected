// PortalVol.h: interface for the CPortalVol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORTALVOL_H__4C0506E3_C898_4272_85AD_CA4ED3126858__INCLUDED_)
#define AFX_PORTALVOL_H__4C0506E3_C898_4272_85AD_CA4ED3126858__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PvsBase.h"
#include "PortalWall.h"
#include "..\N3Base\My_3DStruct.h"

#include <vector>

class CPvsObjFactory;
class CPvsMgr;
class CN3Shape;

typedef struct tagVisPartIndex {
	int m_iPartIndex;
	std::vector<int> m_ivVector;
} __VPI;

class CPortalVol : public CPvsBase  
{
	friend class CPvsObjFactory;
	friend class CPvsMgr;

	typedef std::list<CPvsBase*>::iterator iter;
	typedef std::list<__VPI>::iterator	viter;
	typedef std::vector<int>::iterator Iiter;

	DECLARE_DYNAMIC( CPortalVol )

	__VertexColor				m_pvVertex[8];
	unsigned short				m_pIndex[36];

// Intermediate data required for loading..
	std::list<int>				m_piIDList;
//..

	std::list<CPvsBase*>		m_pPvsList;

//.. What you need in compile mode...
	std::list<int>				m_pVisibleiIDList;

//.. What you need from pvsMgr's Tick...
	e_ExtBool					m_eRenderType;

//.. Portal's priority in compile mode.. Clear first with -1.. 0 priority is yourself..
	int							m_iPriority;

//.. Shape file name..
	std::string						m_strShapeFile;
	CN3Shape*					m_pShape;
	__Matrix44						m_MtxShapeMove;	
	__Matrix44						m_MtxShapeScale;
	__Vector3						m_vShapePos;
	__Vector3						m_vShapeScale;
	std::list<__VPI>				m_viIndex;					// Visible Index List..

//..
	std::vector<int>				m_ivColVector;			// Collision Index List..

	void SetShape(std::string szStr, CN3Shape* pShape);

	void AddVisibleIDList(CPortalVol* pVol);
	void AddWallLinkByPointer(CPvsBase* pBase);
	void AddVolLinkByPointer(CPortalVol* pVol);
	void DeleteLinkByiOrder(int iOrder);
	void WallAdjust(CPvsBase* pBase);

	CPortalVol();
	virtual ~CPortalVol();

	bool CreatePvsObject();
	bool DeletePvsObject();

	void SetState(e_PvsState ePS);

	void Load(FILE* stream);
	void Save(FILE* stream);

	void Translate();

	void TickEdit();
	void TickCompile();
	void TickExecute();

	void RenderEdit();
	void RenderCompile();
	void RenderExecute();

	void RenderShape();
	void RenderCollision();

	//..
	void SplitAndMakeShape(CN3Shape* pShape);

	//.. 
	bool IsInVolumn(__Vector3 vec);
	bool IsInVolumnEx(__Vector3 vec1, __Vector3 vec2, __Vector3 Vec3);
	bool IsInVolumnExEx(__Vector3 vec1, __Vector3 vec2);
	bool IsInVolumnExExEx(__Vector3 vOrig, __Vector3 vDir, e_WallType eWT, __Vector3 &vPick);

	bool	IntersectTriangle(const __Vector3& vOrig, const __Vector3& vDir,
							  const __Vector3& v0, const __Vector3& v1, const __Vector3& v2,
							  float& fT, float& fU, float& fV, __Vector3* pVCol);

	void SplitAndMakeCollision(CN3Shape* pShape);
};

#endif // !defined(AFX_PORTALVOL_H__4C0506E3_C898_4272_85AD_CA4ED3126858__INCLUDED_)
