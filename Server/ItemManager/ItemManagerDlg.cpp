// ItemManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ItemManager.h"
#include "ItemManagerDlg.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD WINAPI	ReadQueueThread(LPVOID lp);

DWORD WINAPI ReadQueueThread(LPVOID lp)
{
	CItemManagerDlg* pMain = (CItemManagerDlg*)lp;
	int recvlen = 0, index = 0;
	BYTE command;
	char recv_buff[1024];
	memset( recv_buff, NULL, 1024 );
	CString string;
	//char logstr[256];

	while(TRUE) {
		if( pMain->m_LoggerRecvQueue.GetFrontMode() != R ) {
			index = 0;
			recvlen = pMain->m_LoggerRecvQueue.GetData( recv_buff );
			if( recvlen > MAX_PKTSIZE ) {
				Sleep(1);
				continue;
			}

			command = GetByte( recv_buff, index );
			switch( command ) {
			case WIZ_ITEM_LOG:
				pMain->ItemLogWrite( recv_buff+index );
				break;
			case WIZ_DATASAVE:
				pMain->ExpLogWrite( recv_buff+index );
				break;
			}

			recvlen = 0;
			memset( recv_buff, NULL, 1024 );
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
// CItemManagerDlg dialog

CItemManagerDlg::CItemManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CItemManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CItemManagerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	memset( m_strGameDSN, 0x00, 24 );
	memset( m_strGameUID, 0x00, 24 );
	memset( m_strGamePWD, 0x00, 24 );
	m_nItemLogFileDay = 0;
	m_nServerNo = 0; m_nZoneNo = 0;
}

void CItemManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemManagerDlg)
	DDX_Control(pDX, IDC_OUT_LIST, m_strOutList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CItemManagerDlg, CDialog)
	//{{AFX_MSG_MAP(CItemManagerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_EXIT_BTN, OnExitBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemManagerDlg message handlers

BOOL CItemManagerDlg::OnInitDialog()
{
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	//----------------------------------------------------------------------
	//	Logfile initialize
	//----------------------------------------------------------------------
	CTime time=CTime::GetCurrentTime();
	char strLogFile[50];		memset(strLogFile, 0x00, 50);
	wsprintf(strLogFile, "ItemLog-%d-%d-%d.txt", time.GetYear(), time.GetMonth(), time.GetDay());
	m_ItemLogFile.Open( strLogFile, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone );
	m_ItemLogFile.SeekToEnd();

	memset(strLogFile, 0x00, 50);
	wsprintf(strLogFile, "ExpLog-%d-%d-%d.txt", time.GetYear(), time.GetMonth(), time.GetDay());
	m_ExpLogFile.Open( strLogFile, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone );
	m_ExpLogFile.SeekToEnd();


	m_nItemLogFileDay = time.GetDay();
	m_nExpLogFileDay = time.GetDay();

	m_LoggerRecvQueue.InitailizeMMF( MAX_PKTSIZE, MAX_COUNT, SMQ_ITEMLOGGER, FALSE );	// Dispatcher 