// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "N3Indoor.h"

#include "N3IndoorDoc.h"
#include "N3IndoorView.h"
#include "OrganizeView.h"

#include "DlgBrowsePath.h"
#include "DlgBase.h"
#include "DlgShapeList.h"
#include "DlgUnusedFiles.h"
#include "TotalToolSheet.h"

#include "TransDummy.h"
#include "../N3Base/N3Scene.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_WORKSHOP_OPEN, OnFileWorkshopOpen)
	ON_COMMAND(ID_FILE_OPEN_GAMEDATA, OnFileOpenGamedata)
	ON_COMMAND(ID_FILE_SAVE_WORKSHOP, OnFileSaveWorkshop)
	ON_COMMAND(ID_FILE_SAVE_GAMEDATA, OnFileSaveGamedata)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_SOURCEOBJECT, OnViewSourceobject)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SOURCEOBJECT, OnUpdateViewSourceobject)
	ON_COMMAND(ID_CURSOR_SELECT, OnCursorSelect)
	ON_COMMAND(ID_CURSOR_POS, OnCursorPos)
	ON_COMMAND(ID_CURSOR_ROTATE, OnCursorRotate)
	ON_COMMAND(ID_CURSOR_SCALE, OnCursorScale)
	ON_COMMAND(ID_TIP_FOCUS_SELOBJ, OnTipFocusSelobj)
	ON_COMMAND(ID_TIP_FOCUS_ALL, OnTipFocusAll)
	ON_COMMAND(ID_TIP_DELETE_OBJ, OnTipDeleteObj)
	ON_UPDATE_COMMAND_UI(ID_CURSOR_SELECT, OnUpdateCursorSelect)
	ON_UPDATE_COMMAND_UI(ID_CURSOR_POS, OnUpdateCursorPos)
	ON_UPDATE_COMMAND_UI(ID_CURSOR_ROTATE, OnUpdateCursorRotate)
	ON_UPDATE_COMMAND_UI(ID_CURSOR_SCALE, OnUpdateCursorScale)
	ON_COMMAND(ID_TIP_NEXT_SHAPE, OnTipNextShape)
	ON_COMMAND(ID_FILE_MRU_FILE1, OnFileMruFile1)
	ON_COMMAND(ID_FILE_MRU_FILE2, OnFileMruFile2)
	ON_COMMAND(ID_FILE_MRU_FILE3, OnFileMruFile3)
	ON_COMMAND(ID_FILE_MRU_FILE4, OnFileMruFile4)
	ON_COMMAND(ID_VIEW_OUTPUTOBJECT, OnViewOutputobject)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUTOBJECT, OnUpdateViewOutputobject)
	ON_COMMAND(ID_EDIT_PROPERTY, OnEditProperty)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTY, OnUpdateEditProperty)
	ON_COMMAND(ID_TIP_DELETE_UNUSED_FILES, OnTipDeleteUnusedFiles)
	ON_COMMAND(ID_TOTAL_PROPERTY_TOOL, OnTotalPropertyTool)
	ON_UPDATE_COMMAND_UI(ID_TOTAL_PROPERTY_TOOL, OnUpdateTotalPropertyTool)
	ON_COMMAND(ID_FILE_SERVER_DATA, OnFileServerData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_eState = STATE_EDIT;										// state variables..
	m_eSelectState = SELECT_STATE_PVOLUMN;
	m_dwRenderingOption = dw_Render_None;	

	m_pDlgSourceList = NULL;							// Dialog to show object list
	m_pDlgOutputList = NULL;							// Dialog to show objects placed on the map
	m_pSceneSource = NULL;					// A Scene containing the things to show in the source object list

	m_pDummy = NULL;

	m_LastSelectedElement.pSelectPointer = NULL;
	m_bViewSelectedTotalShape = false;
	m_bViewSelectedFloor = false;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if(m_Eng.Init(TRUE, m_hWnd, 64, 64, 0, TRUE) == false) return 0;
	m_Eng.GridCreate(300, 300); // Create grid..

	m_Camera.EyePosSet(cvInitEyeOffset);

	D3DCOLORVALUE crLgt;
	crLgt.a = 0.0f, crLgt.r = crLgt.g = crLgt.b = 0.8f;
	m_Light.m_Data.InitDirection(0, __Vector3(0,-1,0), crLgt);	

	m_pDlgSourceList = new CDlgShapeList(this);
	m_pDlgSourceList->Create(IDD_SHAPE_LIST, this);
	m_pDlgSourceList->SetWindowText("Object List");

	m_pDlgOutputList = new CDlgShapeList(this);
	m_pDlgOutputList->Create(IDD_SHAPE_LIST, this);
	m_pDlgOutputList->SetWindowText("Output List");
	m_pDlgOutputList->m_IsSourceObj = FALSE;
	(m_pDlgOutputList->GetDlgItem(IDC_OK))->ShowWindow(false);

	m_pDlgBase = new CDlgBase(this);
	m_pDlgBase->Create(IDD_BASE, this);
	m_pDlgBase->OneTimeInit();

	m_pTotalSheet = new CTotalToolSheet(IDS_TOTAL_SHEET);
	m_pTotalSheet->Create(this, WS_OVERLAPPED|WS_CAPTION);
	m_pTotalSheet->ShowWindow(false);

	SetWindowPos(this, 0, 0, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOOWNERZORDER);         
	m_pDlgSourceList->ShowWindow(true);
	m_pDlgOutputList->ShowWindow(true);

	RECT rect;
	GetWindowRect(&rect);
	
	if (m_pDlgSourceList && m_pDlgOutputList)
	{
		RECT rectMy;	int iWidth, iHeight;
		m_pDlgSourceList->GetWindowRect(&rectMy);	iWidth = rectMy.right - rectMy.left;	iHeight = rectMy.bottom - rectMy.top;
		m_pDlgSourceList->SetWindowPos(this, rect.right, rect.top, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
		m_pDlgOutputList->SetWindowPos(this, rect.right, rect.top+iHeight, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
	}
	
	GetOrganizeView()->InitEnvironSetting();

	m_eState = STATE_EDIT;
	m_eSelectState = SELECT_STATE_PVOLUMN;
	m_dwRenderingOption = dw_Render_None;
	m_eEditState = EDIT_SELECT;

	// set route...
	std::string str;
	char szPathCur[256] = "";
	GetCurrentDirectory(256, szPathCur);
	GetOrganizeView()->SetDlgItemText(IDC_EDIT_RESOURCE_PATH, szPathCur);	
	str =  szPathCur; str += "\\N3Indoor";
	CN3Base::PathSet(str);
	m_strResourcePath = str;

	m_dwRenderingOption |= dw_Render_Add_Shape;

	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.x = 0;
	cs.y = 0;
	cs.cy = 1020;
	cs.cx = 1200;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRect rc; GetClientRect(rc);
	int iW = (int)(rc.Width()*0.9f) / 4;
	int iH = rc.Height();

	if(m_wndSplitter.CreateStatic(this, 1, 2) == FALSE) return FALSE;
	if(m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(COrganizeView), CSize(iW, iH), pContext) == FALSE) return FALSE;
	if(m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CN3IndoorView), CSize(iW*3, iH), pContext) == FALSE) return FALSE;

	m_wndSplitter.SetColumnInfo(0, iW, 0);
	m_wndSplitter.SetColumnInfo(1, iW*3, 0);

	return TRUE;
}

void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	FloorInfo FInfo;
	vfiter vfit = m_FloorList.begin();
	while (vfit != m_FloorList.end())
	{
		FInfo = *vfit++;
		FInfo.m_vVolume.clear();
	}
	m_FloorList.clear();

	if (m_pSceneSource) {delete m_pSceneSource; m_pSceneSource = NULL;}
	if (m_pDlgSourceList) {m_pDlgSourceList->DestroyWindow(); delete m_pDlgSourceList; m_pDlgSourceList = NULL;}
	if (m_pDlgOutputList) {m_pDlgOutputList->DestroyWindow(); delete m_pDlgOutputList; m_pDlgOutputList = NULL;}	
	if (m_pDlgBase) {m_pDlgBase->DestroyWindow(); delete m_pDlgBase; m_pDlgBase = NULL;}
	if (m_pTotalSheet) {m_pTotalSheet->DestroyWindow(); delete m_pTotalSheet; m_pTotalSheet = NULL;}
}

COrganizeView* CMainFrame::GetOrganizeView()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
	COrganizeView* pView = DYNAMIC_DOWNCAST(COrganizeView, pWnd);
	return pView;
}

CN3IndoorView* CMainFrame::GetIndoorView()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CN3IndoorView* pView = DYNAMIC_DOWNCAST(CN3IndoorView, pWnd);
	return pView;
}

void CMainFrame::SetFPSString(LPCTSTR pStr)
{
	GetOrganizeView()->SetDlgItemText(IDC_FPS, pStr);
}

void CMainFrame::OnFileNew() 
{
	// TODO: Add your command handler code here
	GetOrganizeView()->OnFileNew();
}

void CMainFrame::OnFileWorkshopOpen() 
{
	// TODO: Add your command handler code here
	GetOrganizeView()->OnFileWorkshopOpen();
}

void CMainFrame::OnFileOpenGamedata() 
{
	// TODO: Add your command handler code here
	GetOrganizeView()->OnFileOpenGamedata();
}

void CMainFrame::OnFileSaveWorkshop() 
{
	// TODO: Add your command handler code here
	GetOrganizeView()->OnFileSaveWorkshop();
}

void CMainFrame::OnFileSaveGamedata() 
{
	// TODO: Add your command handler code here
	GetOrganizeView()->OnFileSaveGamedata();
}

void CMainFrame::OnFileServerData() 
{
	// TODO: Add your command handler code here
	GetOrganizeView()->OnFileServerData();	
}

void CMainFrame::OnKillFocus(CWnd* pNewWnd) 
{
	CFrameWnd::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	CN3IndoorApp* pApp = (CN3IndoorApp* )AfxGetApp();
	if (pApp)
		pApp->m_bActive = false;	
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CFrameWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	CN3IndoorApp* pApp = (CN3IndoorApp* )AfxGetApp();
	if (pApp)
		pApp->m_bActive = true;		
}

void CMainFrame::OnViewSourceobject() 
{
	// TODO: Add your command handler code here
	if (m_pDlgSourceList) m_pDlgSourceList->ShowWindow(!m_pDlgSourceList->IsWindowVisible());	
}

void CMainFrame::OnUpdateViewSourceobject(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (m_pDlgSourceList) pCmdUI->SetCheck(m_pDlgSourceList->IsWindowVisible());		
}

void CMainFrame::OnViewOutputobject() 
{
	// TODO: Add your command handler code here
	if (m_pDlgOutputList) m_pDlgOutputList->ShowWindow(!m_pDlgOutputList->IsWindowVisible());		
}

void CMainFrame::OnUpdateViewOutputobject(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (m_pDlgOutputList) pCmdUI->SetCheck(m_pDlgOutputList->IsWindowVisible());			
}

CN3Transform* CMainFrame::AddChr(CN3Scene* pDestScene, const std::string& szFN, BOOL bGenerateChainNumber)
{
	CN3Chr* pChr = new CN3Chr;
	if(false == pChr->LoadFromFile(szFN)) // If the call fails...
	{
		delete pChr;
		return NULL;
	}

	if(bGenerateChainNumber)
	{
		int nCC = pDestScene->ChrCount();
		int nChainNumber = 0;
		char szCompare[_MAX_PATH];
		for(int i = 0; i < nCC; i++)
		{
			lstrcpy(szCompare, pDestScene->ChrGet(i)->m_szName.c_str());
			int nL = lstrlen(szCompare);
			if(nL < 5) continue;

			szCompare[nL-5] = NULL; // Remove the trailing underscore and four-digit number.
			if(pChr->m_szName == szCompare)
			{
				nChainNumber = atoi(&(szCompare[nL-4])) + 1;
			}
		}

		char szName[_MAX_PATH];
		wsprintf(szName, "%s_%.4d", pChr->m_szName.c_str(), nChainNumber);
		pChr->m_szName = szName;
	}

	pDestScene->ChrAdd(pChr);
	return pChr;
}

CN3Transform* CMainFrame::AddShape(CN3Scene* pDestScene, const std::string& szFN, BOOL bGenerateChainNumber)	
{
	CN3Shape* pShape = new CN3Shape;
	if(false == pShape->LoadFromFile(szFN)) //	If the call fails...
	{
		delete pShape;
		return NULL;
	}

	pDestScene->ShapeAdd(pShape); // add
	return pShape;
}

void CMainFrame::LoadSourceObjects()					
{
	ASSERT(m_pSceneSource && m_pDlgSourceList);

	m_pSceneSource->Release();

	WIN32_FIND_DATA FindFileData;

	// Add all characters from source\Chr folder
	CString szChrPath;
	szChrPath.Format("%sChr\\", CN3Base::s_szPath.c_str());
	SetCurrentDirectory(szChrPath); // Change the path to the szFolder\Chr folder.
	HANDLE hFind = FindFirstFile("*.N3Chr", &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		AddChr(m_pSceneSource, std::string(szChrPath + FindFileData.cFileName), FALSE);
		while(FindNextFile(hFind, &FindFileData))
		{
			AddChr(m_pSceneSource, std::string(szChrPath + FindFileData.cFileName), FALSE);
		}
		FindClose(hFind);
	}

	CString szShapePath;
	szShapePath.Format("%sObject\\", CN3Base::s_szPath.c_str());
	SetCurrentDirectory(szShapePath);
	hFind = FindFirstFile("*.N3Shape", &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		AddShape(m_pSceneSource, std::string(szShapePath + FindFileData.cFileName), FALSE);
		while(FindNextFile(hFind, &FindFileData))
		{
			AddShape(m_pSceneSource, std::string(szShapePath + FindFileData.cFileName), FALSE);
		}
		FindClose(hFind);
	}	

	m_pSceneSource->Tick();
	m_pDlgSourceList->UpdateTree(m_pSceneSource); // update list
}

void CMainFrame::RefreshSourceObjects()
{
	LoadSourceObjects();

	COrganizeView* pView = GetOrganizeView();
	ShapeInfo*	pSI = NULL;	
	siiter siit = pView->m_PVSMgr.m_plShapeInfoList.begin();
	while( siit != pView->m_PVSMgr.m_plShapeInfoList.end() )
	{
		pSI = *siit++;			
		pSI->m_pShape = m_pSceneSource->ShapeGetByFileName(pSI->m_strShapeFile);
	}
	
	CPortalVolume* pVol = NULL;
	iter it = pView->m_PVSMgr.m_pPvsList.begin();
	while(it != pView->m_PVSMgr.m_pPvsList.end())
	{
		pVol = *it++;

		siiter siit = pVol->m_plShapeInfoList.begin();
		while( siit != pVol->m_plShapeInfoList.end() )
		{
			pSI = *siit++;			
			pSI->m_pShape = m_pSceneSource->ShapeGetByFileName(pSI->m_strShapeFile);
		}
	}
}

void CMainFrame::OutputDlgRefresh()	// If you put the object selected in the source list, it is copied and put into the OutputScene.
{
	m_pDlgOutputList->UpdateTree();
}

void CMainFrame::RenderObjectToWindow(CN3TransformCollision* pObj, HWND hWnd)	// Draws an object in a specific window.
{
	if (pObj == NULL || hWnd == NULL) return;
	DWORD dwType = pObj->Type();
	if (!(dwType & OBJ_CHARACTER || dwType & OBJ_SHAPE)) return;

	LPDIRECT3DDEVICE8 pD3DDev = m_Eng.s_lpD3DDev;

	// Save CameraData
	__CameraData CameraDataBackUp;
	CopyMemory(&CameraDataBackUp, &CN3Base::s_CameraData, sizeof(CameraDataBackUp));

	// clear & Set Viewport - tigger
	CRect rc;
	::GetClientRect(hWnd, &rc);
	m_Eng.Clear(0xff606060, rc);
	m_Eng.SetViewPort(rc);

	// begin
	pD3DDev->BeginScene();

	// Identifying the location and size of an object
	__Vector3 vDir(-1,-1,3);	vDir.Normalize();
	__Vector3 vMin = pObj->Min();
	__Vector3 vMax = pObj->Max();

	__Vector3 vEye, vAt, vUp(0,1,0);
	vAt = vMin + (vMax-vMin)/2.0f;
	vAt.y -= (vMax-vMin).Magnitude() / 8.0f;
	vEye = vAt - vDir*((vMax-vMin).Magnitude() * 0.7f);

	// back up
	__Matrix44 mtxOldView, mtxOldProj;
	pD3DDev->GetTransform(D3DTS_VIEW, &mtxOldView);
	pD3DDev->GetTransform(D3DTS_PROJECTION, &mtxOldProj);
	DWORD dwLighting;
	pD3DDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);

	// Camera frustum setting.. (Because the transform is changed inside the Apply function, it must be put in this location.)
	CN3Camera TempCamera;
	TempCamera.EyePosSet(vEye);
	TempCamera.AtPosSet(vAt);
	TempCamera.UpVectorSet(vUp);
	TempCamera.Tick();
	TempCamera.Apply();	// Put the data into the temporary camera and calculate the frustum information.

	// Set Render State
	pD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	// drawing
	if (dwType & OBJ_CHARACTER)
	{
		CN3Chr* pChr = (CN3Chr*)pObj;
		pChr->Tick(-1000);
		pChr->Render();
	}
	else if (dwType & OBJ_SHAPE)
	{
		CN3Shape* pShape = (CN3Shape*)pObj;
		pShape->Tick(-1000);
		pShape->Render();
	}

	pD3DDev->EndScene(); // end
	m_Eng.Present(hWnd); // present

	// restore (If you don't return to the previous state, picking from the terrain won't work properly)
	pD3DDev->SetTransform(D3DTS_VIEW, &mtxOldView);
	pD3DDev->SetTransform(D3DTS_PROJECTION, &mtxOldProj);
	CopyMemory(&CN3Base::s_CameraData, &CameraDataBackUp, sizeof(CameraDataBackUp));
	pD3DDev->SetViewport(&CN3Base::s_CameraData.vp);
	pD3DDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
}

void CMainFrame::OnCursorSelect() 
{
	// TODO: Add your command handler code here
	COrganizeView* pView = GetOrganizeView();
	if (pView) pView->OnCurserSelect(EDIT_SELECT);
}

void CMainFrame::OnUpdateCursorSelect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(m_eEditState == EDIT_SELECT);
}

void CMainFrame::OnCursorPos() 
{
	// TODO: Add your command handler code here
	COrganizeView* pView = GetOrganizeView();
	if (pView) pView->OnCurserSelect(EDIT_MOVE);
}

void CMainFrame::OnUpdateCursorPos(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(m_eEditState == EDIT_MOVE);	

	if (m_eState == STATE_EDIT)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnCursorRotate() 
{
	// TODO: Add your command handler code here
	COrganizeView* pView = GetOrganizeView();
	if (pView) pView->OnCurserSelect(EDIT_ROTATE);
}

void CMainFrame::OnUpdateCursorRotate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(m_eEditState == EDIT_ROTATE);		
	if ( (m_eState == STATE_EDIT) && (m_eSelectState == SELECT_STATE_SHAPE) )
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnCursorScale() 
{
	// TODO: Add your command handler code here
	COrganizeView* pView = GetOrganizeView();
	if (pView) pView->OnCurserSelect(EDIT_SCALE);
}

void CMainFrame::OnUpdateCursorScale(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(m_eEditState == EDIT_SCALE);	
	if (m_eState == STATE_EDIT)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}

ShapeInfo* CMainFrame::GetShapeForDisplay()
{
	if (m_eEditState != EDIT_SELECT)
	{
		if (m_pDummy && m_pDummy->m_SelObjArray.GetSize() > 0)
		{
			int iSize = m_pDummy->m_SelObjArray.GetSize();
			for (int i=0; i<iSize; ++i)
			{
				SelectElement se = m_pDummy->m_SelObjArray.GetAt(i);
				if (se.eST == TYPE_SHAPE_ONLY)
					return (ShapeInfo* )se.pSelectPointer;
			}
		}
	}

	if (m_LastSelectedElement.pSelectPointer)
	{
		if (m_LastSelectedElement.eST == TYPE_SHAPE_ONLY)
			return (ShapeInfo* )m_LastSelectedElement.pSelectPointer;
	}

	return NULL;
}

void CMainFrame::OnTipFocusSelobj() 
{
	// TODO: Add your command handler code here
	// Find the object that will have focus.
	bool bFound = false;
	CPortalVolume* pVol = NULL;
	COrganizeView* pView = GetOrganizeView();

	__Vector3 vMin, vMax;
	vMin.Set(FLT_MAX,FLT_MAX,FLT_MAX);	vMax.Set(-FLT_MAX,-FLT_MAX,-FLT_MAX);

	if (m_eEditState != EDIT_SELECT)
	{
		if (m_pDummy && m_pDummy->m_SelObjArray.GetSize() > 0)
		{
			int iSize = m_pDummy->m_SelObjArray.GetSize();
			for (int i=0; i<iSize; ++i)
			{
				SelectElement se = m_pDummy->m_SelObjArray.GetAt(i);
				if (se.eST != TYPE_SHAPE_ONLY)
				{
					CPortalVolume* pVol2 = (CPortalVolume* )se.pSelectPointer;
					__Vector3 vTmpMin = pVol2->Min();
					__Vector3 vTmpMax = pVol2->Max();

					if (vTmpMin.x < vMin.x) vMin.x = vTmpMin.x;
					if (vTmpMin.y < vMin.y) vMin.y = vTmpMin.y;
					if (vTmpMin.z < vMin.z) vMin.z = vTmpMin.z;
					if (vTmpMax.x > vMax.x) vMax.x = vTmpMax.x;
					if (vTmpMax.y > vMax.y) vMax.y = vTmpMax.y;
					if (vTmpMax.z > vMax.z) vMax.z = vTmpMax.z;
				}
				else
				{
					ShapeInfo* pSelObj = (ShapeInfo* )se.pSelectPointer;
					__Vector3 vTmpMin = pSelObj->m_pShape->RawMin()*pSelObj->m_Matrix;
					__Vector3 vTmpMax = pSelObj->m_pShape->RawMax()*pSelObj->m_Matrix;

					if (vTmpMin.x < vMin.x) vMin.x = vTmpMin.x;
					if (vTmpMin.y < vMin.y) vMin.y = vTmpMin.y;
					if (vTmpMin.z < vMin.z) vMin.z = vTmpMin.z;
					if (vTmpMax.x > vMax.x) vMax.x = vTmpMax.x;
					if (vTmpMax.y > vMax.y) vMax.y = vTmpMax.y;
					if (vTmpMax.z > vMax.z) vMax.z = vTmpMax.z;
				}
			}
		
			if (vMin.x != FLT_MAX && vMax.x != -FLT_MAX)
			{	// Adjust camera distance according to object size
				__Vector3 vDir = m_Camera.Dir();
				__Vector3 vAt = vMin + ((vMax-vMin)/2);
				m_Camera.AtPosSet(vAt);
				m_Camera.EyePosSet(vAt - vDir*(vMax-vMin).Magnitude());
			}			
			return;
		}
	}
	else
	{
		if (m_SelVolArray.GetSize() > 0)
		{
			int iSize = m_SelVolArray.GetSize();
			for (int i=0; i<iSize; ++i)
			{
				CPortalVolume* pVol2 = m_SelVolArray.GetAt(i);
				__Vector3 vTmpMin = pVol2->Min();
				__Vector3 vTmpMax = pVol2->Max();

				if (vTmpMin.x < vMin.x) vMin.x = vTmpMin.x;
				if (vTmpMin.y < vMin.y) vMin.y = vTmpMin.y;
				if (vTmpMin.z < vMin.z) vMin.z = vTmpMin.z;
				if (vTmpMax.x > vMax.x) vMax.x = vTmpMax.x;
				if (vTmpMax.y > vMax.y) vMax.y = vTmpMax.y;
				if (vTmpMax.z > vMax.z) vMax.z = vTmpMax.z;
			}
		
			if (vMin.x != FLT_MAX && vMax.x != -FLT_MAX)
			{	// Adjust camera distance according to object size
				__Vector3 vDir = m_Camera.Dir();
				__Vector3 vAt = vMin + ((vMax-vMin)/2);
				m_Camera.AtPosSet(vAt);
				m_Camera.EyePosSet(vAt - vDir*(vMax-vMin).Magnitude());
			}			
			return;
		}
	}

	if (m_LastSelectedElement.pSelectPointer)
	{
		if (m_LastSelectedElement.eST != TYPE_SHAPE_ONLY)
		{
			CPortalVolume* pVol2 = (CPortalVolume* )m_LastSelectedElement.pSelectPointer;
			__Vector3 vTmpMin = pVol2->Min();
			__Vector3 vTmpMax = pVol2->Max();

			if (vTmpMin.x < vMin.x) vMin.x = vTmpMin.x;
			if (vTmpMin.y < vMin.y) vMin.y = vTmpMin.y;
			if (vTmpMin.z < vMin.z) vMin.z = vTmpMin.z;
			if (vTmpMax.x > vMax.x) vMax.x = vTmpMax.x;
			if (vTmpMax.y > vMax.y) vMax.y = vTmpMax.y;
			if (vTmpMax.z > vMax.z) vMax.z = vTmpMax.z;
		}
		else
		{
			ShapeInfo* pSelObj = (ShapeInfo* )m_LastSelectedElement.pSelectPointer;
			__Vector3 vTmpMin = pSelObj->m_pShape->RawMin()*pSelObj->m_Matrix;
			__Vector3 vTmpMax = pSelObj->m_pShape->RawMax()*pSelObj->m_Matrix;

			if (vTmpMin.x < vMin.x) vMin.x = vTmpMin.x;
			if (vTmpMin.y < vMin.y) vMin.y = vTmpMin.y;
			if (vTmpMin.z < vMin.z) vMin.z = vTmpMin.z;
			if (vTmpMax.x > vMax.x) vMax.x = vTmpMax.x;
			if (vTmpMax.y > vMax.y) vMax.y = vTmpMax.y;
			if (vTmpMax.z > vMax.z) vMax.z = vTmpMax.z;
		}

		if (vMin.x != FLT_MAX && vMax.x != -FLT_MAX)
		{	// Adjust camera distance according to object size
			__Vector3 vDir = m_Camera.Dir();
			__Vector3 vAt = vMin + ((vMax-vMin)/2);
			m_Camera.AtPosSet(vAt);
			m_Camera.EyePosSet(vAt - vDir*(vMax-vMin).Magnitude());
		}			
	}
}

void CMainFrame::OnTipFocusAll() 
{
	// TODO: Add your command handler code here
	__Vector3 vMin, vMax;
	FindMinMaxTotalShape(vMin, vMax);

	if (vMin.x != FLT_MAX && vMax.x != -FLT_MAX)
	{	// Adjust camera distance according to object size
		__Vector3 vDir = m_Camera.Dir();
		__Vector3 vAt = vMin + ((vMax-vMin)/2);
		m_Camera.AtPosSet(vAt);
		m_Camera.EyePosSet(vAt - vDir*(vMax-vMin).Magnitude());
	}
}

void CMainFrame::FindMinMaxTotalShape(__Vector3 &vecMin, __Vector3 &vecMax)
{
	COrganizeView* pView = GetOrganizeView();

	__Vector3 vMin, vMax;
	vMin.Set(FLT_MAX,FLT_MAX,FLT_MAX);	vMax.Set(-FLT_MAX,-FLT_MAX,-FLT_MAX);

	ShapeInfo* pSI;
	siiter siit = pView->m_PVSMgr.m_plShapeInfoList.begin();
	while(siit != pView->m_PVSMgr.m_plShapeInfoList.end())
	{
		pSI = *siit++;

		CN3TransformCollision* pSelObj = (CN3TransformCollision* )pSI->m_pShape;
		__Vector3 vTmpMin = pSelObj->RawMin()*pSI->m_Matrix;
		__Vector3 vTmpMax = pSelObj->RawMax()*pSI->m_Matrix;

		if (vTmpMin.x < vMin.x) vMin.x = vTmpMin.x;
		if (vTmpMin.y < vMin.y) vMin.y = vTmpMin.y;
		if (vTmpMin.z < vMin.z) vMin.z = vTmpMin.z;
		if (vTmpMax.x > vMax.x) vMax.x = vTmpMax.x;
		if (vTmpMax.y > vMax.y) vMax.y = vTmpMax.y;
		if (vTmpMax.z > vMax.z) vMax.z = vTmpMax.z;
	}	

	vecMin = vMin;
	vecMax = vMax;
}

void CMainFrame::OnTipNextShape() 
{
	// TODO: Add your command handler code here
	COrganizeView* pView = GetOrganizeView();
	pView->SelectNextShape();	
}

void CMainFrame::OnTipDeleteObj() 
{
	bool bDelete = false;

	// TODO: Add your command handler code here
	COrganizeView* pView = GetOrganizeView();
	CPortalVolume* pVol = NULL;

	if (m_eEditState == EDIT_SELECT)
	{
		int iCount = m_SelVolArray.GetSize();
		for (int i = 0; i < iCount; i++ )
		{
			bDelete = true;
			pVol = m_SelVolArray.GetAt(i);
			pView->m_PVSMgr.DeletePortalVolLinkedByiID(pVol->m_iID);
			pView->m_PVSMgr.DeletePortalVolByPointer(pVol);
		}
	}
	else if (m_pDummy && m_pDummy->m_SelObjArray.GetSize() > 0)
	{
		int iSize = m_pDummy->m_SelObjArray.GetSize();
		for (int i=0; i<iSize; ++i)
		{
			SelectElement se = m_pDummy->m_SelObjArray.GetAt(i);
			if (se.eST == TYPE_VOLUME_TOTAL)
			{
				bDelete = true;
				pVol = (CPortalVolume* )se.pSelectPointer;
				pView->m_PVSMgr.DeletePortalVolLinkedByiID(pVol->m_iID);
				pView->m_PVSMgr.DeletePortalVolByPointer(pVol);
			}
		}
	}

	if (bDelete)
	{
		TotalValidateCheckAfterDelete();
		pView->m_SelectedListCtrl.SetHotItem(-1);
		pView->m_iSelectionIndex = -1;
		pView->RefreshSelectedList();
		pView->RefreshLinkToList();
		pView->RefreshLinkedList();	
	}
}

void CMainFrame::TotalValidateCheckAfterDelete()
{
	int i, iSize;
	bool bFound;
	CPortalVolume* pVol;
	COrganizeView* pView = GetOrganizeView();

	// 1. m_pDummy Check..
	if (m_pDummy && m_pDummy->m_SelObjArray.GetSize() > 0)
	{
LOOP1:
		iSize = m_pDummy->m_SelObjArray.GetSize();
		for (i=0; i<iSize; ++i)
		{
			bFound = false;

			SelectElement se = m_pDummy->m_SelObjArray.GetAt(i);					
			if (se.eST != TYPE_SHAPE_ONLY)
			{
				if (pView->m_PVSMgr.IsExistPortalVolumeByPointer((CPortalVolume *)se.pSelectPointer))
					bFound = true;
			}
			else
			{
				if (pView->m_PVSMgr.IsExistTotalShapeByPointer((ShapeInfo *)se.pSelectPointer))
					bFound = true;
				
				if (pView->m_PVSMgr.IsExistLinkedShapeByPointer((ShapeInfo *)se.pSelectPointer))
					bFound = true;
			}

			// If you can't find it...
			if (!bFound)
			{
				m_pDummy->m_SelObjArray.RemoveAt(i);
				goto LOOP1;
			}
		}	
	}

	// 2. m_SelVolArray Check..
	if (m_SelVolArray.GetSize() > 0)
	{
LOOP2:
		iSize = m_SelVolArray.GetSize();
		for (i=0; i<iSize; ++i)
		{
			bFound = false;

			// looking for...
			pVol = m_SelVolArray.GetAt(i);
			if (pView->m_PVSMgr.IsExistPortalVolumeByPointer((CPortalVolume *)pVol))
				bFound = true;

			// If you can't find it...
			if (!bFound)
			{
				m_SelVolArray.RemoveAt(i);
				goto LOOP2;
			}
		}
	}

	// 3. m_LastSelectedElement Check..
	if (m_LastSelectedElement.eST != TYPE_SHAPE_ONLY)
	{
		bFound = false;

		// looking for...
		if (pView->m_PVSMgr.IsExistPortalVolumeByPointer((CPortalVolume *)m_LastSelectedElement.pSelectPointer))
			bFound = true;

		// If you can't find it...
		if (!bFound)
			m_LastSelectedElement.pSelectPointer = NULL;
	}
	else
	{
		bFound = false;

		// looking for...
		if (pView->m_PVSMgr.IsExistTotalShapeByPointer((ShapeInfo *)m_LastSelectedElement.pSelectPointer))
			bFound = true;
		
		if (pView->m_PVSMgr.IsExistLinkedShapeByPointer((ShapeInfo *)m_LastSelectedElement.pSelectPointer))
			bFound = true;

		// If you can't find it...
		if (!bFound)
			m_LastSelectedElement.pSelectPointer = NULL;
	}
}

void CMainFrame::OnFileMruFile1() 
{
	// TODO: Add your command handler code here
	CN3IndoorApp* pApp = (CN3IndoorApp* )AfxGetApp();
	std::string str = pApp->GetMRU1();	
	std::string strT;

	char szExt[_MAX_EXT];
	_splitpath(str.c_str(), NULL, NULL, NULL, szExt);
	strT = szExt;

	COrganizeView* pView = GetOrganizeView();

	if (strT == ".wshop") pView->OpenWorkShopFile(str);
	if (strT == ".n3indoor")  pView->OpenGameDataFile(str);
}

void CMainFrame::OnFileMruFile2() 
{
	// TODO: Add your command handler code here
	CN3IndoorApp* pApp = (CN3IndoorApp* )AfxGetApp();
	std::string str = pApp->GetMRU2();		
	std::string strT;

	char szExt[_MAX_EXT];
	_splitpath(str.c_str(), NULL, NULL, NULL, szExt);
	strT = szExt;

	COrganizeView* pView = GetOrganizeView();

	if (strT == ".wshop") pView->OpenWorkShopFile(str);
	if (strT == ".n3indoor")  pView->OpenGameDataFile(str);
}

void CMainFrame::OnFileMruFile3() 
{
	// TODO: Add your command handler code here
	CN3IndoorApp* pApp = (CN3IndoorApp* )AfxGetApp();
	std::string str = pApp->GetMRU3();		
	std::string strT;

	char szExt[_MAX_EXT];
	_splitpath(str.c_str(), NULL, NULL, NULL, szExt);
	strT = szExt;

	COrganizeView* pView = GetOrganizeView();
	strT = szExt;

	if (strT == ".wshop") pView->OpenWorkShopFile(str);
	if (strT == ".n3indoor")  pView->OpenGameDataFile(str);
}

void CMainFrame::OnFileMruFile4() 
{
	// TODO: Add your command handler code here
	CN3IndoorApp* pApp = (CN3IndoorApp* )AfxGetApp();
	std::string str = pApp->GetMRU4();		
	std::string strT;

	char szExt[_MAX_EXT];
	_splitpath(str.c_str(), NULL, NULL, NULL, szExt);
	strT = szExt;

	COrganizeView* pView = GetOrganizeView();

	if (strT == ".wshop") pView->OpenWorkShopFile(str);
	if (strT == ".n3indoor")  pView->OpenGameDataFile(str);
}

void CMainFrame::OnEditProperty() 
{
	// TODO: Add your command handler code here
	ASSERT(m_pDlgBase);	
	if (m_pDlgBase) m_pDlgBase->ShowWindow(!m_pDlgBase->IsWindowVisible());	
	SetFocus();
}

void CMainFrame::OnUpdateEditProperty(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	ASSERT(m_pDlgBase);	
	if (m_pDlgBase) pCmdUI->SetCheck(m_pDlgBase->IsWindowVisible());	
}

void CMainFrame::UpdateShapeInfoDisplay()
{
	ASSERT(m_pDlgBase);	
	if (m_pDlgBase)	m_pDlgBase->UpdateInfo();
}

void CMainFrame::OnTipDeleteUnusedFiles() 
{
	if (!m_pDlgOutputList)	return;

	// TODO: Add your command handler code here
	typedef std::map<std::string, CN3BaseFileAccess*> mapBase;
	typedef mapBase::value_type valBase;
	typedef mapBase::iterator it_Base;

	std::vector<std::string> invalidFNs;
	std::vector<std::string> unusedFNs;
	std::string szFN;
	
	//  Once everything is put on the map.
	mapBase mBases;
	int iSC = m_pDlgOutputList->GetTotalShapeInfoCount();
	
	CN3Shape* pShape = NULL;
	CN3VMesh* pVMesh = NULL;
	CN3SPart* pPart = NULL;
	CN3PMesh* pPMesh = NULL;
	CN3Texture* pTex = NULL;
	for(int i = 0; i < iSC; i++)
	{
		pShape = m_pDlgOutputList->GetShapeByiOrder(i);
		if(NULL == pShape) continue;

		szFN = CN3Base::PathGet() + pShape->FileName();
		CharLower(&(szFN[0]));
		mBases.insert(valBase(szFN, pShape));

		pVMesh = pShape->CollisionMesh();
		if(pVMesh)
		{
			szFN = CN3Base::PathGet() + pVMesh->FileName();
			CharLower(&(szFN[0]));
			mBases.insert(valBase(szFN, pVMesh));
		}
		else
		{
			invalidFNs.push_back("NULL VMesh : " + pShape->FileName());
		}

		int iSPC = pShape->PartCount();
		for(int j = 0; j < iSPC; j++)
		{
			pPart = pShape->Part(j);
			if(NULL == pPart)
			{
				CString szErr;
				szErr.Format("NULL Part : %s - %d Part", pShape->FileName().c_str(), j);
				invalidFNs.push_back(szErr.operator LPCTSTR());
				continue;
			}

			pPMesh = pPart->Mesh();
			if(pPMesh)
			{
				szFN = CN3Base::PathGet() + pPMesh->FileName();
				CharLower(&(szFN[0]));
				mBases.insert(valBase(szFN, pPMesh));
			}
			else
			{
				CString szErr;
				szErr.Format("NULL PMesh : %s - %d Part", pShape->FileName().c_str(), j);
				invalidFNs.push_back(szErr.operator LPCTSTR());
			}

			int iSTC = pPart->TexCount();
			for(int k = 0; k < iSTC; k++)
			{
				pTex = pPart->Tex(k);
				if(pTex)
				{
					szFN = CN3Base::PathGet() + pTex->FileName();
					CharLower(&(szFN[0]));
					mBases.insert(valBase(szFN, pTex));
				}
				else
				{
					CString szErr;
					szErr.Format("NULL Texture : %s - %d Part, %d Texture", pShape->FileName().c_str(), j, k);
					invalidFNs.push_back(szErr.operator LPCTSTR());
					continue;
				}
			}
		}
	}

	// looking for a file...
	std::string szPath = CN3Base::PathGet() + "object\\";
	::SetCurrentDirectory(szPath.c_str());
	CFileFind ff;

	BOOL bFind;
	CString szFNTmp;
	CString szFNTmp2;

	for(ff.FindFile(); bFind = ff.FindNextFile(); )
	{
		szFNTmp = ff.GetFilePath();
		szFNTmp2 = ff.GetFileName();

		if(szFNTmp2 == "." || szFNTmp2 == "..") continue;

		szFNTmp.MakeLower();
		
		szFN = szFNTmp;
		it_Base it = mBases.find(szFN);
		if(it != mBases.end()) continue; // If you find it, write it.

		unusedFNs.push_back(szFN);
	}

	if (!bFind)
	{
		szFNTmp = ff.GetFilePath();
		szFNTmp2 = ff.GetFileName();

		szFNTmp.MakeLower();
		
		szFN = szFNTmp;
		it_Base it = mBases.find(szFN);
		if(it == mBases.end()) 
			unusedFNs.push_back(szFN);
	}
	
	// Opens the Delete File dialog box.
	CDlgUnusedFiles dlg;
	int iUFC = unusedFNs.size();
	for(i = 0; i < iUFC; i++)
	{
		dlg.m_FileNames.Add(unusedFNs[i].c_str());
	}

	int iIFC = invalidFNs.size();
	for(i = 0; i < iIFC; i++)
	{
		dlg.m_InvalidFileNames.Add(invalidFNs[i].c_str());
	}

	dlg.DoModal();
	
	// update all..
	if(m_pDummy)
		m_pDummy->ClearObjs();
	m_SelVolArray.RemoveAll();
	m_LastSelectedElement.pSelectPointer = NULL;

	this->RefreshSourceObjects(); // Update the Source Object...
	if (m_pDlgSourceList) m_pDlgSourceList->UpdateTree(m_pSceneSource);
	OutputDlgRefresh();
}

void CMainFrame::OnTotalPropertyTool() 
{
	// TODO: Add your command handler code here
	ASSERT(m_pTotalSheet);	
	if (m_pTotalSheet) m_pTotalSheet->ShowWindow(!m_pTotalSheet->IsWindowVisible());	
	SetFocus();
}

void CMainFrame::OnUpdateTotalPropertyTool(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	ASSERT(m_pTotalSheet);	
	if (m_pTotalSheet) pCmdUI->SetCheck(m_pTotalSheet->IsWindowVisible());		
}

