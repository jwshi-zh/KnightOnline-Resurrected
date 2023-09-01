// MapMng.h: interface for the CMapMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPMNG_H__995D141A_97C5_4CE7_B100_D729B35E9959__INCLUDED_)
#define AFX_MAPMNG_H__995D141A_97C5_4CE7_B100_D729B35E9959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Cursor Mode
enum {CM_SELECT = 1, CM_OBJECT_BRUSH = 2, CM_POS = 3, CM_ROTATE = 4, CM_SCALE = 5,
 CM_EDIT_TERRAIN = 6, CM_SMOOTH_TERRAIN = 7, CM_SET_DTEX=8, CM_EDIT_RIVER=9,
 CM_EDIT_LIGHTMAP=10, CM_MAKE_NPCPATH=11, CM_MAKE_WALL=12, CM_EDIT_EVENT=13,
 CM_EDIT_POND=14, CM_FLAT_TERRAIN=15, CM_EDIT_REGENUSER=16, CM_EDIT_SOUND=17,
 CM_EDIT_SEED = 18, CM_EDIT_LIGHT = 19 };

const int NUM_UNIT_LIGHT = 2;

#include <afxtempl.h>
#include "../N3Base/N3Base.h"
#include "PosDummy.h"
#include "RotDummy.h"
#include "ScaleDummy.h"
#include "RiverMng.h"
#include "PondMng.h"
#include "LyTerrain.h"
#include "RegenUser.h"
#include "SowSeedMng.h"

class CMainFrame;
class CN3Scene;
class CN3Camera;
class CN3Light;
//class CDlgSceneGraph;
class CDlgShapeList;
class CBrushDlg;
class CN3Transform;
class CN3TransformCollision;
class CDlgBase;
class CN3Base;
class CNPCPathMgr;
class CWallMgr;
class CN3ShapeMgr;
class CEventMgr;
class CRegenUser;
class CWarpMgr;
class CSoundMgr;
class CLightObjMgr;

class CMapMng : public CN3Base
{
public:
	CMapMng(CMainFrame* pMainFrm);
	virtual ~CMapMng();

// Attributes
public:

	CN3Transform*	GetSelOutputObj(int iIndex) const 
	{
		if (iIndex < 0 || iIndex >= m_SelOutputObjArray.GetSize() )
			return NULL;
		return m_SelOutputObjArray.GetAt(iIndex);
	}
	int				GetSelOutputObjCount() const {return m_SelOutputObjArray.GetSize();};
	CLyTerrain*		GetTerrain() const { return m_pTerrain;}
	CWnd*			GetDlgSourceList() const {return (CWnd*)m_pDlgSourceList;}
	CWnd*			GetDlgOutputList() const {return (CWnd*)m_pDlgOutputList;}
	CWnd*			GetDlgBase() const {return (CWnd*)m_pDlgBase;}
	CN3Scene*       GetSceneOutput() { return m_pSceneOutput;}
	D3DSHADEMODE	m_ShadeMode;
	D3DFILLMODE		m_FillMode;
	bool			m_bViewWireFrame; 				// View the selected object as a wire frame.
	bool			m_bRenderAxisAndGrid;			// Determine whether to draw axes and grids or not..

	int				m_iZoneID; // zone id...
	bool			m_bHideObj;
	bool			m_bLoadingComplete; // Render only if this is true.

	CMainFrame*			m_pMainFrm;						// Main window
	CSowSeedMng			m_SowSeedMng;
	CDlgShapeList*		m_pDlgSourceList;			// Dialog to show object list
	CN3Transform*		GetSelSourceObj(){ return m_pSelSourceObj;}

protected:
	enum	ENUM_EDIT_STATE {eEDIT_NOT=0,eEDIT_COPY,eEDIT_PASTE,eEDIT_NUM};

	CLightObjMgr*		m_pLightObjMgr;	
	CSoundMgr*			m_pSoundMgr;
	CWarpMgr*			m_pWarpMgr;
	CRegenUser*			m_pRegenUser;
	CEventMgr*			m_pEventMgr;					// Manage zone events...
	CWallMgr*			m_pWall;						// Wall management that can't go outside the zone..
	CNPCPathMgr*		m_pNPCPath;						// Class to make NPC paths..
	CLyTerrain*			m_pTerrain;						// Terrain
	CN3Scene*			m_pSceneSource;					// Scene containing what to show in the source object list
	CN3Scene*			m_pSceneOutput;					// Scene including objects placed on the map
	
	//CDlgSceneGraph*		m_pDlgSourceList;			// Dialog to show object list
	//CDlgSceneGraph*		m_pDlgOutputList;			// Dialog to show objects placed on the map
	CDlgShapeList*		m_pDlgOutputList;				// Dialog to show objects placed on the map
	CDlgBase*			m_pDlgBase;						// Edit object properties dialog box..
	CBrushDlg*			m_pBrushDlg;					// Dialog box for specifying terrain editing brush options
	

	CN3Transform*		m_pSelSourceObj;				// Selected from source object list
//	CTypedPtrArray<CPtrArray, CN3TransformCollision*> m_SelOutputObjArray;	// Selected objects among the objects placed on the map
	CTypedPtrArray<CPtrArray, CN3Transform*>	m_SelOutputObjArray;	// Selected objects among the objects placed on the map

	ENUM_EDIT_STATE		m_eSelObjBackState;		//	Display the status of temporary copy (ctrl+c)
	CTypedPtrArray<CPtrArray, CN3Transform*>	m_SelOutputObjBack;	// Contains the selected objects among the objects placed on the map

	int					m_CursorMode;					// Differentiate function mode of mouse
	BOOL				m_bRenderSelObj;				// Determine whether to draw the selected item in the source object list or n

	CTransDummy*		m_pDummy;						// Class responsible for moving, rotating and scaling the object
	CPosDummy			m_PosDummy;						// movement
	CRotDummy			m_RotDummy;						// rotation
	CScaleDummy			m_ScaleDummy;					// Zoom in zoom out
	CRiverMng			m_RiverMng;						// River water information management
	CPondMng			m_PondMng;						// Pond information management

	RECT				m_rcSelDrag;					// Drag area

	struct __Sort										// Structure to sort when picking objects
	{
		CN3TransformCollision*	pObj;
		float		fCamDist;
		__Vector3	vMin, vMax;
	};
// Operations
public:
	void	SetLight(float fLgt);
	void	ActWarpMgr();
	bool	IsWarpMgrActive();
	void	DeleteSelectedSourceObjects();
	void	DeleteOverlappedObjects();
	void	DeleteUnusedFiles();
	void	SetZoneID(int id) { m_iZoneID = id; if(m_pTerrain) m_pTerrain->m_iZoneID = id; }	// Set terrain zone ID.
	void	ImportShape(const char* szFullPath);
	void	MakeTerrainMovableAttr(CN3ShapeMgr* pShapeMgr);		// Organize the tiles that can be reached and the tiles that cannot be reached on the terrain
	void	ImportPostDataFromScene(const char* szFileName); // Load the objects placed in the scene..
	void	UpDateFP(); 
	void	Tick();
	void	Render();
	void	Release();
	void	SavePartition(float x, float z, float width);
	void	SaveToFile(LPCTSTR lpszPathName);			// save map file
	void	LoadFromFile(LPCTSTR lpszPathName);			// load map file
	BOOL	MouseMsgFilter(LPMSG pMsg);			// function of mouse
	void	ImportTerrain(const char* szMeshFN);			// Read terrain data from VMesh file
	void	ImportTerrainHeight(const char* szMeshFN);		// Read only the height of the terrain from the VMesh file.
	void	MakeGameFiles(LPCTSTR lpszPathName, float fSize = 128.0f);	// Convert to game data
	void	MakeServerDataFiles(LPCTSTR lpszPathName);
	void	SelectObject(CN3Base* pObj, BOOL IsSourceObj, BOOL bAdd = FALSE);	// select the object
	void	RenderObjectToWindow(CN3TransformCollision* pObj, HWND hWnd);	// Draw an object in a specific window.
	void	SetCursorMode(int iMode);						// Change the way the mouse cursor is used.
	int		GetCursorMode() const {return m_CursorMode;}	// Get how to use the mouse cursor.
	void	Invalidate();									// Update View screen
	void	FocusSelObj();									// Move the focus to the selected object.
	void	FocusAll();										// Make the entire map visible.
	void	FocusAt(__Vector3 v);							// Move the focus to the specified poin
	void	DropSelObjToTerrain();							// Attach the selected object to the terrain (adjust only the Y value)
	void	DeleteSelObjectFromOutputScene();			// Delete the selected object in OutputScene.
	void	RenderDragRect(RECT* rc);					// Draw the drag area.
	void	UpdateAll();								// Update the source and output dialogs

	void	LoadObjectPostData(LPCTSTR lpszFileName);		// Read the shape arrangement information from the text file.
	void	SaveObjectPostData(LPCTSTR lpszFileName);		// Save the shape arrangement information as a text file.
	void	SaveObjectPostDataPartition(LPCTSTR lpszFileName, float psx, float psz, float width);
	void	RenderGrid(float fGridSize, float fMaxDistance);	// Draw a grid on the map at regular intervals
	void	OnSelChanged();								// What to do when the selected object changes
	void	SelectObjectByDragRect(RECT* pRect, BOOL bAdd);	// select object by dragging
	void	LoadSourceObjects();						// Read Source Object from Folder
	CN3Transform* AddChr(CN3Scene* pDestScene, const std::string& szFN, BOOL bGenerateChainNumber);		// Copy and add a character object to a specific Scene
	CN3Transform* AddShape(CN3Scene* pDestScene, const std::string& szFN, BOOL bGenerateChainNumber);	// Copy and add a Shape object to a specific Scene
	CN3Transform* AddObjectToOutputScene(CN3Transform* pObj);	// If you put the object selected in the source list, it is copied and put into the OutputScene.
	CN3Camera* CameraGet();

	CN3Base*	Pick(POINT point, int* pnPart);												// object picking function
	static int SortByCameraDistance(const void *pArg1, const void *pArg2);	// Used for sorting function qsort according to camera distance

//	BOOL	CameraMove(LPMSG pMsg);		// Handle messages about camera movement
	BOOL	GetObjectMinMax(CN3Transform* pObj, __Vector3& vMin, __Vector3& vMax);	// Find the maximum and minimum points of the object

	CStatusBar* GetStatusBar();

	void	SetEditState(ENUM_EDIT_STATE eEditStat);	//	edit the selected object by drawing
};

#endif // !defined(AFX_MAPMNG_H__995D141A_97C5_4CE7_B100_D729B35E9959__INCLUDED_)

