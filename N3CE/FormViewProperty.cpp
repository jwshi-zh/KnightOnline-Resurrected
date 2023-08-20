// FormViewProperty.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "../N3Base/N3Chr.h"

#include "MainFrm.h"
#include "N3CEDoc.h"
#include "N3CEView.h"
#include "FormViewAnimation.h"

#include "FormViewProperty.h"
#include "../N3Base/N3FXPlug.h"
#include "../N3Base/N3FXBundle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CFormViewProperty, CFormView)

CFormViewProperty::CFormViewProperty()
	: CFormView(CFormViewProperty::IDD)
{
	//{{AFX_DATA_INIT(CFormViewProperty)
	//}}AFX_DATA_INIT
}

CFormViewProperty::~CFormViewProperty()
{
}

/////////////////////////////////////////////////////////////////////////////
// CFormViewProperty diagnostics

#ifdef _DEBUG
void CFormViewProperty::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormViewProperty::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CN3CEDoc* CFormViewProperty::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CN3CEDoc)));
	return (CN3CEDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFormViewProperty message handlers

void CFormViewProperty::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	static bool bInitCtrl = false;
	if(false == bInitCtrl)
	{
		CRect rc;
		this->GetClientRect(&rc);
		/////////////////////////////////////
		// Material 