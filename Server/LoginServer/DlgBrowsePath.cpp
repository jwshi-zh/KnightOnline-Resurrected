// DlgBrowsePath.cpp : implementation file
//

#include "stdafx.h"
#include "DlgBrowsePath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
//
// This source is part of CShellTree - Selom Ofori
// 
// Version: 1.02 (any previously unversioned copies are older/inferior
//
// This code is free for all to use. Mutatilate it as much as you want
// See MFCENUM sample from microsoft

/////////////////////////////////////////////////////////////////////////////
// CDlgBrowsePath dialog
//
// This source is part of CShellTree - Selom Ofori
// 
// Version: 1.02 (any previously unversioned copies are older/inferior
//
// This code is free for all to use. Mutatilate it as much as you want
// See MFCENUM sample from microsoft

CDlgBrowsePath::CDlgBrowsePath(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBrowsePath::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBrowsePath)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDlgBrowsePath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBrowsePath)
	DDX_Control(pDX, IDC_FILELIST, m_CBPath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgBrowsePath, CDialog)
	//{{AFX_MSG_MAP(CDlgBrowsePath)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_BROWSE_TREE, OnItemexpandingBrowseTree)
	ON_NOTIFY(NM_RCLICK, IDC_BROWSE_TREE, OnRclickBrowseTree)
	ON_NOTIFY(TVN_DELETEITEM, IDC_BROWSE_TREE, OnDeleteitemBrowseTree)
	ON_BN_CLICKED(IDC_SYNCH_FOLDERS, OnSynchFolders)
	ON_NOTIFY(TVN_SELCHANGED, IDC_BROWSE_TREE, OnSelchangedBrowseTree)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBrowsePath message handlers

BOOL CDlgBrowsePath::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 理