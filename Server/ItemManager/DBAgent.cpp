// DBAgent.cpp: implementation of the CDBAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemManager.h"
#include "DBAgent.h"
#include "ItemManagerDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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
	m_pMain = (CItemManagerDlg*)AfxGetMainWnd();

	CString strConnect;
	strConnect.Format (_T("ODBC;DSN=%s;UID=%s;PWD=%s"), m_pMain->m_strGameDSN, m_pMain->m_strGameUID, m_pMain->m_strGamePWD );

	m_GameDB.SetLoginTimeout (10);
	if( !m_GameDB.Open(NULL,FALSE,FALSE,strConnect) )
	{
		AfxMessageBox("GameDB SQL Connection Fail...");
		return FALSE;
	}

	return TRUE;
}

void CDBAgent::ReConnectODBC(CDatabase *m_db, char *strdb, char *strname, char *strpwd)
{
	char strlog[256];	memset( strlog, 0x00, 256);
	CTime t = CTime::GetCurrentTime();
	sprintf(strlog, "Try ReConnectODBC... %d