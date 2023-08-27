// BattleSet.cpp : implementation file
//

#include "stdafx.h"
#include "ebenezer.h"
#include "BattleSet.h"
#include <EbenezerDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBattleSet

IMPLEMENT_DYNAMIC(CBattleSet, CRecordset)

CBattleSet::CBattleSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CBattleSet)
	m_sIndex = 0;
	m_byNation = 0;
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CBattleSet::GetDefaultConnect()
{
	auto mainDlg = static_cast<CEbenezerDlg*>(AfxGetApp()->GetMainWnd());

	char strconnection[256];
	memset(strconnection, NULL, 256);
	sprintf(strconnection, "Driver={SQL Server Native Client 11.0};Server=%s;Database=%s;Uid=%s;Pwd=%s;", mainDlg->m_ODBCServer, mainDlg->m_ODBCDatabase, mainDlg->m_ODBCLogin, mainDlg->m_ODBCPwd);
	return strconnection;
}

CString CBattleSet::GetDefaultSQL()
{
	return _T("[dbo].[BATTLE]");
}

void CBattleSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CBattleSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sIndex]"), m_sIndex);
	RFX_Byte(pFX, _T("[byNation]"), m_byNation);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CBattleSet diagnostics

#ifdef _DEBUG
void CBattleSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CBattleSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
