// N3CEView.cpp : implementation of the CN3CEView class
//

#include "stdafx.h"
#include "N3CE.h"
#include "N3CEDoc.h"
#include "N3CEView.h"
#include "MainFrm.h"
#include "FormViewAnimation.h"
#include "FormViewProperty.h"
#include "FormViewTool.h"
#include "mmsystem.h"
#include "PosDummy.h"

#include "../N3Base/N3SndObj.h"
#include "../N3Base/N3FXPlug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CN3CEView

IMPLEMENT_DYNCREATE(CN3CEView, CView)

BEGIN_MESSAGE_MAP(CN3CEView, CView)
	//{{AFX_MSG_MAP(CN3CEView)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_VIEW_XRAY, OnViewXray)
	ON_UPDATE_COMMAND_UI(ID_VIEW_XRAY, OnUpdateViewXray)
	ON_COMMAND(ID_VIEW_JOINT, OnViewJoint)
	ON_UPDATE_COMMAND_UI(ID_VIEW_JOINT, OnUpdateViewJoint)
	ON_WM_DROPFILES()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CN3CEView construction/destruction

CN3CEView::CN3CEView()
{
	// TODO: add construction code here
	m_bRenderOptionXRay = false;
	m_bRenderOptionJoint = false;
	m_bPlayingNow = false;

	m_ptPrev.x = m_ptPrev.y = 0;

	m_pJointSelected = NULL;
	m_fTickPrev = CN3Base::TimeGet();

	m_eCursorMode = eCM_Nothing; // 而ㅼ