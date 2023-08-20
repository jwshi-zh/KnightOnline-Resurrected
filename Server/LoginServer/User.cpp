// User.cpp: implementation of the CUser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "versionmanager.h"
#include "versionmanagerdlg.h"
#include "User.h"

#pragma warning(disable : 4786)		// Visual C++ Only
#include <set>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUser::CUser()
{

}

CUser::~CUser()
{

}

void CUser::Initialize()
{
	m_pMain = (CVersionManagerDlg*)AfxGetMainWnd();

	CIOCPSocket2::Initialize();
}

void CUser::CloseProcess()
{

	CIOCPSocket2::CloseProcess();
}

void CUser::Parsing(int len, char *pData)
{
	int index = 0, send_index = 0, i=0, client_version = 0;
	char buff[2048]; memset( buff, 0x00, 2048 );
	BYTE command = GetByte( pData, index );

	switch( command ) {
	case LS_VERSION_REQ:
		SetByte( buff, LS_VERSION_REQ, send_index );
		SetShort( buff, m_pMain->m_nLastVersion, send_index );
		Send( buff, send_index );
		break;
	case LS_SERVERLIST:
		m_pMain->m_DBProcess.LoadUserCountList();		// 湲곕