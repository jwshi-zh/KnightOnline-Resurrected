// DBProcess.cpp: implementation of the CDBProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "versionmanager.h"
#include "define.h"
#include "DBProcess.h"
#include "VersionManagerDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDBProcess::CDBProcess()
{

}

CDBProcess::~CDBProcess()
{

}

BOOL CDBProcess::InitDatabase(char *strconnection)
{
	m_VersionDB.SetLoginTimeout(100);

	m_pMain = (CVersionManagerDlg*)AfxGetMainWnd();

	if( !m_VersionDB.Open( NULL,FALSE,FALSE, strconnection ) )
		return FALSE;

	return TRUE;
}

void CDBProcess::ReConnectODBC(CDatabase *m_db, const char *strdb, const char *strname, const char *strpwd)
{
	char strlog[256];	memset( strlog, 0x00, 256);
	CTime t = CTime::GetCurrentTime();
	sprintf(strlog, "Try ReConnectODBC... %d