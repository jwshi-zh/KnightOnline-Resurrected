// WallMgr.h: interface for the CWallMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WALLMGR_H__5633920C_20A6_4FD3_9D71_B522269163F8__INCLUDED_)
#define AFX_WALLMGR_H__5633920C_20A6_4FD3_9D71_B522269163F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\N3BASE\N3BaseFileAccess.h"
#include "../N3Base/N3ShapeMgr.h"
#include <list>

class CWall;
class CMapMng;
class CDlgMakeWall;

class CWallMgr : public CN3BaseFileAccess  
{
protected:
	__Vector3			m_BaseCube[8];
	__VertexXyzColor	m_CubeVB[36];
	__VertexXyzColor	m_LineVB[2];
	__VertexXyzColor	m_BoardVB[4];

public:
	CMapMng*				m_pRefMapMng;				// Terrain reference pointer..
	std::list<CWall*>		m_pWalls;					// walls...
	CDlgMakeWall*			m_pDlg;						// path make dialog..
	bool					m_bActive;					// Whether this function is activated... 1: Enabled, 0: Disabled...
	CWall*					m_pCurrWall;				// The wall you are currently making..or the buffer you are preparing to create..

protected:
	void MakeLine(__Vector3 sv, __Vector3 ev, D3DCOLOR color);
	void MakeCube(__Vector3 cv, D3DCOLOR color);
	void MakeBoard(__Vector3 sv, __Vector3 ev, D3DCOLOR color);

public:
	void AddWall2Coll(CN3ShapeMgr* pShapeMgr);
	void SetFocus(CWall* pWall);
	void SetCurrWall(CWall* pWall);
	void DelWall(CWall* pWall);
	int	 GetSize() { return m_pWalls.size(); }
	CWall*	GetpWall(int idx);
		
	void Render();
	void UpdateWall();
	void SetActive(bool active);
	BOOL MouseMsgFilter(LPMSG pMsg);

	virtual bool	Load(HANDLE hFile);
	virtual bool	Save(HANDLE hFile);

	CWallMgr();
	virtual ~CWallMgr();

};

#endif // !defined(AFX_WALLMGR_H__5633920C_20A6_4FD3_9D71_B522269163F8__INCLUDED_)
