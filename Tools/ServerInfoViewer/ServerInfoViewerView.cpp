// ServerInfoViewerView.cpp : implementation of the CServerInfoViewerView class
//

#include "stdafx.h"
#include "ServerInfoViewer.h"

#include "MainFrm.h"
#include "ServerInfoViewerDoc.h"
#include "ServerInfoViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerInfoViewerView

IMPLEMENT_DYNCREATE(CServerInfoViewerView, CView)

BEGIN_MESSAGE_MAP(CServerInfoViewerView, CView)
	//{{AFX_MSG_MAP(CServerInfoViewerView)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerInfoViewerView construction/destruction

CServerInfoViewerView::CServerInfoViewerView()
{
	// TODO: add construction code here

}

CServerInfoViewerView::~CServerInfoViewerView()
{
}

BOOL CServerInfoViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CServerInfoViewerView drawing

void CServerInfoViewerView::OnDraw(CDC* pDC)
{
	CServerInfoViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	D3DCOLOR crBkg = 0x00000000;
	
	if(NULL == pFrm || NULL == pFrm->m_Eng.s_lpD3DDev) return;

	pFrm->m_Eng.Clear(crBkg);

	pFrm->m_Camera.Tick();
	pFrm->m_Light.Tick();

	pFrm->m_Camera.Apply();
	pFrm->m_Light.Apply();

	pFrm->m_Eng.s_lpD3DDev->BeginScene();

	__Matrix44 mtx;
	mtx.Identity();
	mtx.Scale(64, 64, 64);
	mtx.PosSet(2048.0f, 0, 2048.0f);
	pFrm->m_Eng.RenderGrid(mtx); // 4 誘명