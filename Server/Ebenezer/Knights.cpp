// Knights.cpp: implementation of the CKnights class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ebenezer.h"
#include "Knights.h"
#include "User.h"
#include "GameDefine.h"
#include "PacketDefine.h"
#include "EbenezerDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKnights::CKnights()
{
	//m_pMain = NULL;
	
/*	CString strConnect;
	strConnect.Format (_T("ODBC;DSN=%s;UID=%s;PWD=%s"), "KN_Online", "knight", "knight");
	m_KnightsDB.SetLoginTimeout (10);
	if( !m_KnightsDB.Open(NULL,FALSE,FALSE,strConnect) )
		AfxMessageBox("KnightsDB Connection Fail...");	*/
}

CKnights::~CKnights()
{

}

void CKnights::InitializeValue()
{
	m_sIndex = 0;
	m_byFlag = 0;			// 1 : Clan, 2 : 湲곗