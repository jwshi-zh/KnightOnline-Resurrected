// DlgChrProperty.cpp : implementation file
//

#include "stdafx.h"
#include "DlgChrProperty.h"
#include "MainFrm.h"
#include "N3CEDoc.h"
#include "N3CEView.h"

#include "../N3Base/N3Chr.h"
#include "Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CDlgChrProperty::CDlgChrProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChrProperty::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChrProperty)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgChrProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChrProperty)
	DDX_Control(pDX, IDC_LP_MATERIAL, m_LPMaterial);
	DDX_Control(pDX, IDC_CB_PART_CHARACTER, m_CBChrPart);
	DDX_Control(pDX, IDC_CB_PLUG_CHARACTER, m_CBChrPlug);
	DDX_Control(pDX, IDC_LP_CHR, m_LPChr);
	DDX_Control(pDX, IDC_LP_CHR_PART, m_LPCPart);
	DDX_Control(pDX, IDC_LP_CHR_PLUG, m_LPCPlug);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChrProperty, CDialog)
	//{{AFX_MSG_MAP(CDlgChrProperty)
	ON_CBN_SELCHANGE(IDC_CB_PART_CHARACTER, OnSelchangeCbPartCharacter)
	ON_CBN_SELCHANGE(IDC_CB_PLUG_CHARACTER, OnSelchangeCbPlugCharacter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChrProperty message handlers


BOOL CDlgChrProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int y = 5;
	CRect rc;
	int nH = 0;
	
	int i = 0;
	CString str, strTmp;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 罹