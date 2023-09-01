// LevelUpTableSet.cpp : implementation file
//

#include "stdafx.h"
#include "Ebenezer.h"
#include "LevelUpTableSet.h"
#include <EbenezerDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLevelUpTableSet

IMPLEMENT_DYNAMIC(CLevelUpTableSet, CRecordset)

CLevelUpTableSet::CLevelUpTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CLevelUpTableSet)
	m_level = 0;
	m_Exp = 0;
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CLevelUpTableSet::GetDefaultConnect()
{
	auto mainDlg = static_cast<CEbenezerDlg*>(AfxGetApp()->GetMainWnd());

	char strconnection[256];
	memset(strconnection, NULL, 256);
	sprintf(strconnection, "Driver={SQL Server Native Client 11.0};Server=%s;Database=%s;Uid=%s;Pwd=%s;", mainDlg->m_ODBCServer, mainDlg->m_ODBCDatabase, mainDlg->m_ODBCLogin, mainDlg->m_ODBCPwd);
	return strconnection;
}

CString CLevelUpTableSet::GetDefaultSQL()
{
	return _T("[dbo].[LEVEL_UP]");
}

void CLevelUpTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CLevelUpTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Byte(pFX, _T("[level]"), m_level);
	RFX_Long(pFX, _T("[Exp]"), m_Exp);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CLevelUpTableSet diagnostics

#ifdef _DEBUG
void CLevelUpTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CLevelUpTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
