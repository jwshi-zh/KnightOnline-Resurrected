// LauncherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Launcher.h"
#include "LauncherDlg.h"
#include <wininet.h>

#include "PacketDef.h"
#include "APISocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLauncherDlg dialog

CLauncherDlg::CLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLauncherDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLauncherDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	
	m_pSocket = NULL;

	memset( m_strServiceName, NULL, 256 );
	m_nCurVersion = 0;
	m_nServerVersion = 0;
	m_nGetFileNum = 0;
}



void CLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLauncherDlg)
	DDX_Control(pDX, IDC_STATUS, m_Status);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLauncherDlg, CDialog)
	//{{AFX_MSG_MAP(CLauncherDlg)
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CLauncherDlg message handlers

BOOL CLauncherDlg::OnInitDialog()
{
	CDialog::OnInitDialog() ;

	CString szInfo; szInfo.LoadString(IDS_INFO_VERSION_CHECK);
	m_Status.SetWindowText(szInfo); // 