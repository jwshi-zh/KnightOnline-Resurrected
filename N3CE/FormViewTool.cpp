// FormViewTool.cpp : implementation file
//

#include "stdafx.h"
#include "n3ce.h"
#include "FormViewTool.h"

#include "N3CEDoc.h"
#include "N3CEView.h"
#include "FormViewAnimation.h"
#include "MainFrm.h"
#include "../N3Base/N3Chr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormViewTool

IMPLEMENT_DYNCREATE(CFormViewTool, CFormView)

CFormViewTool::CFormViewTool()
	: CFormView(CFormViewTool::IDD)
{
	//{{AFX_DATA_INIT(CFormViewTool)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CFormViewTool::~CFormViewTool()
{
}

void CFormViewTool::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormViewTool)
	DDX_Control(pDX, IDC_LIST_SOUND1, m_ListSound1);
	DDX_Control(pDX, IDC_LIST_SOUND0, m_ListSound0);
	DDX_Control(pDX, IDC_CB_LOD, m_CBLOD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormViewTool, CFormView)
	//{{AFX_MSG_MAP(CFormViewTool)
	ON_BN_CLICKED(IDC_B_MAKE_SMOOTH, OnBMakeSmooth)
	ON_CBN_SELCHANGE(IDC_CB_LOD, OnSelchangeCbLod)
	ON_BN_CLICKED(IDC_B_ADD_TEST_SOUND0, OnBAddTestSound0)
	ON_BN_CLICKED(IDC_B_DELETE_TEST_SOUND0, OnBDeleteTestSound0)
	ON_BN_CLICKED(IDC_B_ADD_TEST_SOUND1, OnBAddTestSound1)
	ON_BN_CLICKED(IDC_B_DELETE_TEST_SOUND1, OnBDeleteTestSound1)
	ON_LBN_SELCHANGE(IDC_LIST_SOUND0, OnSelchangeListSound0)
	ON_LBN_SELCHANGE(IDC_LIST_SOUND1, OnSelchangeListSound1)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_B_APPLY_OFFSET_FRAME, OnBApplyOffsetFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormViewTool diagnostics

#ifdef _DEBUG
void CFormViewTool::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormViewTool::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFormViewTool message handlers

void CFormViewTool::UpdateAllInfo()
{

}

void CFormViewTool::OnBMakeSmooth() 
{
	CN3Chr* pChr = ((CN3CEDoc*)GetDocument())->m_Scene.ChrGet(0);
	if(NULL == pChr) return;

	int nPC = pChr->PartCount();
	for(int i = 0; i < nPC; i++)
	{
		CN3CPart* pPart = pChr->Part(i);
		if(NULL == pPart) continue;

		for(int j = 0; j < MAX_CHR_LOD; j++)
		{
			CN3Skin* pSkin = pPart->Skin(j);
			if(NULL == pSkin) continue;

			pSkin->ReGenerateSmoothNormal(); // 踰