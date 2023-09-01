// KnightsUserSet.cpp : implementation file
//

#include "stdafx.h"
#include "ebenezer.h"
#include "KnightsUserSet.h"
#include <EbenezerDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKnightsUserSet

IMPLEMENT_DYNAMIC(CKnightsUserSet, CRecordset)

CKnightsUserSet::CKnightsUserSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CKnightsUserSet)
	m_sIDNum = 0;
	m_strUserID = _T("");
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CKnightsUserSet::GetDefaultConnect()
{
	auto mainDlg = static_cast<CEbenezerDlg*>(AfxGetApp()->GetMainWnd());

	char strconnection[256];
	memset(strconnection, NULL, 256);
	sprintf(strconnection, "Driver={SQL Server Native Client 11.0};Server=%s;Database=%s;Uid=%s;Pwd=%s;", mainDlg->m_ODBCServer, mainDlg->m_ODBCDatabase, mainDlg->m_ODBCLogin, mainDlg->m_ODBCPwd);
	return strconnection;
}

CString CKnightsUserSet::GetDefaultSQL()
{
	return _T("[dbo].[KNIGHTS_USER]");
}

void CKnightsUserSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CKnightsUserSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sIDNum]"), m_sIDNum);
	RFX_Text(pFX, _T("[strUserID]"), m_strUserID);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CKnightsUserSet diagnostics

#ifdef _DEBUG
void CKnightsUserSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CKnightsUserSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
