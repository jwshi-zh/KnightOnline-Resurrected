// DBAgent.cpp: implementation of the CDBAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Aujard.h"
#include "DBAgent.h"
#include "AujardDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern CRITICAL_SECTION g_LogFileWrite;

CDBAgent::CDBAgent()
{

}

CDBAgent::~CDBAgent()
{

}

BOOL CDBAgent::DatabaseInit()
{
//	Main DB Connecting..
/////////////////////////////////////////////////////////////////////////////////////
	m_pMain = (CAujardDlg*)AfxGetMainWnd();

	CString strConnect;
	strConnect.Format (_T("ODBC;DSN=%s;UID=%s;PWD=%s"), m_pMain->m_strGameDSN, m_pMain->m_strGameUID, m_pMain->m_strGamePWD );

	m_GameDB.SetLoginTimeout (10);
	if( !m_GameDB.Open(NULL,FALSE,FALSE,strConnect) )
	{
		AfxMessageBox("GameDB SQL Connection Fail...");
		return FALSE;
	}

	strConnect.Format (_T("ODBC;DSN=%s;UID=%s;PWD=%s"), m_pMain->m_strAccountDSN, m_pMain->m_strAccountUID, m_pMain->m_strAccountPWD );

	m_AccountDB.SetLoginTimeout (10);

	if( !m_AccountDB.Open(NULL,FALSE,FALSE,strConnect) )
	{
		AfxMessageBox("AccountDB SQL Connection Fail...");
		return FALSE;
	}

	m_AccountDB1.SetLoginTimeout (10);

	if( !m_AccountDB1.Open(NULL,FALSE,FALSE,strConnect) )
	{
		AfxMessageBox("AccountDB1 SQL Connection Fail...");
		return FALSE;
	}

	return TRUE;
}

void CDBAgent::ReConnectODBC(CDatabase *m_db, char *strdb, char *strname, char *strpwd)
{
	char strlog[256];	memset( strlog, 0x00, 256);
	CTime t = CTime::GetCurrentTime();
	sprintf(strlog, "Try ReConnectODBC... \r\n");
	m_pMain->WriteLogFile( strlog );
	//m_pMain->m_LogFile.Write(strlog, strlen(strlog));

	// DATABASE 