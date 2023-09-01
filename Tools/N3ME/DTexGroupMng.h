// DTexGroupMng.h: interface for the CDTexGroupMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DTEXGROUPMNG_H__C89F0301_7CC1_4AB8_BC19_6CC82919DBD3__INCLUDED_)
#define AFX_DTEXGROUPMNG_H__C89F0301_7CC1_4AB8_BC19_6CC82919DBD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../N3Base/N3Base.h"
#include "LyTerrainDef.h"
#include <list>

class CDTexGroup;
class CDlgDTexGroupView;
class CMainFrame;

typedef std::list<CDTexGroup*>::iterator it_DTexGroup;

class CDTexGroupMng : public CN3Base
{
public:
	std::list<CDTexGroup*>	m_Groups;	//List with group data.
	CDlgDTexGroupView*		m_pGroupView;	//Window showing groups when actually working.
	
	DTEXATTR				m_SelectedDTex;	//Groups and properties selected in the group window when working.
	DTEXTILEATTR			m_SelectedDTexTile;	//The actual texture tile selected at work.

	CMainFrame*				m_pMainFrm;

private:
	static int CompareIdx( const void *arg1, const void *arg2 );

public:
	void DelTileByDTexID(int DTexID);	
	int GetBrushSize();
	DTEXTILEATTR	GetTileAttr(int groupID, int attr);
	DTEXTILEATTR	GetTileAttrManuel(int groupID, int attr, int AttrIdx);
	int GetIndex2ID(int idx);
	int	GetID2Index(int id);
	char* GetGroupName(int id);
	bool SaveToFile(CString RealFileName);
	bool LoadFromFile(CString RealFileName);
	void Init(CWnd* pWndParent);
	void Release();
	void SetGroup(const char* pName);
	void SetGroupID(const char* pName, int id);
	void DelGroup(int ID);
	void SetTile(int ID, int attr, DTEXTILEATTR tile);
	void DelTile(int ID, int attr, DTEXTILEATTR tile);
	CDTexGroupMng();
	virtual ~CDTexGroupMng();

};

#endif // !defined(AFX_DTEXGROUPMNG_H__C89F0301_7CC1_4AB8_BC19_6CC82919DBD3__INCLUDED_)
