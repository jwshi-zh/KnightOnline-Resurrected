// AujardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Aujard.h"
#include "AujardDlg.h"
#include "ItemTableset.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PROCESS_CHECK		100
#define CONCURRENT_CHECK	200
#define SERIAL_TIME			300
#define PACKET_CHECK		400

WORD	g_increase_serial = 50001;

DWORD WINAPI	ReadQueueThread(LPVOID lp);
CRITICAL_SECTION g_LogFileWrite;

DWORD WINAPI ReadQueueThread(LPVOID lp)
{
	CAujardDlg* pMain = (CAujardDlg*)lp;
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
			case WIZ_LOGIN:
				pMain->AccountLogIn( recv_buff+index );
				break;
			case WIZ_NEW_CHAR:
				pMain->CreateNewChar( recv_buff+index );
				break;
			case WIZ_DEL_CHAR:
				pMain->DeleteChar( recv_buff+index );
				break;
			case WIZ_SEL_CHAR:
				pMain->SelectCharacter( recv_buff+index );
				break;
			case WIZ_SEL_NATION:
				pMain->SelectNation( recv_buff+index );
				break;
			case WIZ_ALLCHAR_INFO_REQ:
				pMain->AllCharInfoReq( recv_buff+index );
				break;
			case WIZ_LOGOUT:
				pMain->UserLogOut( recv_buff+index );				
				break;
			case WIZ_DATASAVE:
				pMain->UserDataSave( recv_buff+index );
				break;
			case WIZ_KNIGHTS_PROCESS:
				pMain->KnightsPacket( recv_buff+index );
				break;
			case WIZ_CLAN_PROCESS:
				pMain->KnightsPacket( recv_buff+index );
				break;
			case WIZ_LOGIN_INFO:
				pMain->SetLogInInfo( recv_buff+index );
				break;
			case WIZ_KICKOUT:
				pMain->UserKickOut( recv_buff+index );
				break;
			case WIZ_BATTLE_EVENT:
				pMain->BattleEventResult( recv_buff+index );
				break;
			case DB_COUPON_EVENT:
				pMain->CouponEvent( recv_buff+index );
				break;
			}

			recvlen = 0;
			memset( recv_buff, NULL, 1024 );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAujardDlg dialog

CAujardDlg::CAujardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAujardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAujardDlg)
	m_DBProcessNum = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	memset( m_strGameDSN, 0x00, 24 );
	memset( m_strGameUID, 0x00, 24 );
	memset( m_strGamePWD, 0x00, 24 );
	memset( m_strAccountDSN, 0x00, 24 );
	memset( m_strAccountUID, 0x00, 24 );
	memset( m_strAccountPWD, 0x00, 24 );
	memset( m_strLogDSN, 0x00, 24 );
	memset( m_strLogUID, 0x00, 24 );
	memset( m_strLogPWD, 0x00, 24 );

	m_iSendPacketCount = 0;
	m_iPacketCount = 0;
	m_iRecvPacketCount = 0;
}

void CAujardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAujardDlg)
	DDX_Control(pDX, IDC_OUT_LIST, m_OutputList);
	DDX_Text(pDX, IDC_DB_PROCESS, m_DBProcessNum);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAujardDlg, CDialog)
	//{{AFX_MSG_MAP(CAujardDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAujardDlg message handlers

BOOL CAujardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//----------------------------------------------------------------------
	//	Logfile initialize
	//----------------------------------------------------------------------
	CTime time=CTime::GetCurrentTime();
	char strLogFile[50];		memset(strLogFile, 0x00, 50);
	wsprintf(strLogFile, "AujardLog-%d-%d-%d.txt", time.GetYear(), time.GetMonth(), time.GetDay());
	m_LogFile.Open( strLogFile, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone );
	m_LogFile.SeekToEnd();

	m_iLogFileDay = time.GetDay();

	InitializeCriticalSection( &g_LogFileWrite );
	
	m_LoggerRecvQueue.InitailizeMMF( MAX_PKTSIZE, MAX_COUNT, SMQ_LOGGERSEND, FALSE );	// Dispatcher 