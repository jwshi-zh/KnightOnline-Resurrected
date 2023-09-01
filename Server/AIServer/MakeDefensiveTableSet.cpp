// MakeDefensiveTableSet.cpp : implementation file
//

#include "stdafx.h"
#include "server.h"
#include "MakeDefensiveTableSet.h"
#include <ServerDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMakeDefensiveTableSet

IMPLEMENT_DYNAMIC(CMakeDefensiveTableSet, CRecordset)

CMakeDefensiveTableSet::CMakeDefensiveTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CMakeDefensiveTableSet)
	m_byLevel = 0;
	m_sClass_1 = 0;
	m_sClass_2 = 0;
	m_sClass_3 = 0;
	m_sClass_4 = 0;
	m_sClass_5 = 0;
	m_sClass_6 = 0;
	m_sClass_7 = 0;
	m_nFields = 8;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CMakeDefensiveTableSet::GetDefaultConnect()
{
	auto mainDlg = static_cast<CServerDlg*>(AfxGetApp()->GetMainWnd());

	char strconnection[256];
	memset(strconnection, NULL, 256);
	sprintf(strconnection, "Driver={SQL Server Native Client 11.0};Server=%s;Database=%s;Uid=%s;Pwd=%s;", mainDlg->m_ODBCServer, mainDlg->m_ODBCDatabase, mainDlg->m_ODBCLogin, mainDlg->m_ODBCPwd);
	return strconnection;
}

CString CMakeDefensiveTableSet::GetDefaultSQL()
{
	return _T("[dbo].[MAKE_DEFENSIVE]");
}

void CMakeDefensiveTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CMakeDefensiveTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Byte(pFX, _T("[byLevel]"), m_byLevel);
	RFX_Int(pFX, _T("[sClass_1]"), m_sClass_1);
	RFX_Int(pFX, _T("[sClass_2]"), m_sClass_2);
	RFX_Int(pFX, _T("[sClass_3]"), m_sClass_3);
	RFX_Int(pFX, _T("[sClass_4]"), m_sClass_4);
	RFX_Int(pFX, _T("[sClass_5]"), m_sClass_5);
	RFX_Int(pFX, _T("[sClass_6]"), m_sClass_6);
	RFX_Int(pFX, _T("[sClass_7]"), m_sClass_7);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMakeDefensiveTableSet diagnostics

#ifdef _DEBUG
void CMakeDefensiveTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CMakeDefensiveTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
