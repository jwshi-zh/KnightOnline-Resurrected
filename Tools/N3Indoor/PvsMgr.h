#if !defined(AFX_PVSMGR_H__1391B742_4359_49B6_A268_EC098B8F20FD__INCLUDED_)
#define AFX_PVSMGR_H__1391B742_4359_49B6_A268_EC098B8F20FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include <list>
#include <vector>
#include <queue>
#include "PvsBase.h"

class COrganizeView;
class CPortalWall;
class CPortalVol;
class CN3Base;
class CN3Shape;

#include "PvsObjFactory.h"

typedef struct tagCOLLISION {
	e_WallType	eWT;
	__Vector3	Wvec[5];
	__Vector3	Vvec[24];
} __Collision;

typedef struct tagDEBUGCOLLISION {
	__Vector3	Vvec[8];
} __DCollision;


// Sort by priority...
template<class T> struct Myless : public std::less<T> {
bool operator()(const T& x, const T& y) const
{
	return (x->m_iPriority > y->m_iPriority);
}
};

class CPvsMgr
{
	friend class COrganizeView;

	typedef std::list<CPvsBase*>::iterator iter;
	typedef std::list<int>::iterator iIDiter;
	typedef std::list<CPortalVol* >::iterator itviter;
	typedef std::list<__Collision>::iterator iciter;

	bool					m_bShapePerVolumn;

	std::list<CPvsBase*>	m_pPvsList;
	std::list<__Collision>	m_ColList;		//	Collision check structure list..
	std::list<CPortalVol*>  m_pVoltList;	// Sorted list of collision-checked Volumes.
	__DCollision			m_dcol;

	//.. Main Shape.. ^^
	__Matrix44					m_MtxShapeMove;	
	__Matrix44					m_MtxShapeScale;
	__Matrix44					m_MtxShapeRot;
public:
	__Vector3					m_vShapePos;
	__Vector3					m_vShapeScale;
	__Quaternion			 m_qShapeRot;

private:
	CPvsObjFactory			m_Factory;

	int		m_iTotalCount;
	int		m_iIncreseIndex;

	int		m_iCurIndex;

	void	InsertNewPortalVolumn();
	void	InsertNewPortalWall(e_WallType eWT, CPortalVol * const pVol);
	CPortalWall* InsertNewPortalWallGetPointer(e_WallType eWT);

	bool	DeletePvsObjByiOrder(int iOrder);
	void	DeletePvsLinkedObjByiID(int iID);

	void	DeleteAllPvsObj();

	CPvsBase* GetPvsObjByiOrder(int iOrder);
	CPvsBase* GetPvsWallByiOrder(int iOrder);
	CPvsBase* GetPvsWallByiID(int iID);
	CPvsBase* GetPvsVolByiID(int iID);
	CPortalVol* GetLinkedVolumnByID(int iID);

	void	Load(FILE* stream);
	void	Save(FILE* stream);

	void	TickEdit();
	void	RenderEdit();

	void	TickCompile();
	void	RenderCompile();

	void	TickExecute();
	void	RenderExecute();

	void	CalcCompile();

	void	TotalShapeRender();
	void	TotalCollisionRender();

	void	ComputeVisibilty(CPortalVol * const pVol);
	void	SetPriority(CPortalVol * const pVol);
	void	SetPriorityRecursive(CPortalVol* const pVol, int iRecursive);
	void	PrepareCollisionDetect(CPortalVol* const pVol);
	void	PrepareCollisionDetectOne(CPortalVol* const pVol, CPortalWall* const pWall);
	void	PrepareCollisionDetectTwo(CPortalVol* const pVol, CPortalWall* const pWall, e_WallType eWT, __Collision& col); 
	bool	CollisionDetectMain(CPortalVol* const pVolMy);
	bool	IntersectTriangle(const __Vector3& vOrig, const __Vector3& vDir,
							  const __Vector3& v0, const __Vector3& v1, const __Vector3& v2,
							  float& fT, float& fU, float& fV, __Vector3* pVCol);
	e_ReturnCode	CollisionDetectSub(const __Vector3& vOrig, const __Vector3& vDir, CPortalVol* pVolMy);
	bool			CheckPvsWall(const __Vector3& vOrig, const __Vector3& vDir, CPortalVol* pVolMy, e_WallType eWT);
	bool			CheckPvsVolumnWall(const __Vector3& vOrig, const __Vector3& vDir, CPortalVol* pVolMy, e_WallType eWT);

	// Divide the shape into space.
	void	SplitShapeToVolumn();

	// Rendering for Debug..
//	void	RenderCollision(__Collision& col);

public:
	CPvsMgr();
	virtual ~CPvsMgr();
};


#endif // !defined(AFX_PVSMGR_H__1391B742_4359_49B6_A268_EC098B8F20FD__INCLUDED_)
