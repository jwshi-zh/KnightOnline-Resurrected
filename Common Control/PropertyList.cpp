// PropertyList.cpp : implementation file
//
//#include "StdAfx.h"
#include "PropertyList.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyList
D3DVECTOR CPropertyItem::VectorGet()
{
	float xyz[3] = {0,0,0};
	int j = 0, k = 0;
	for(int i = 0; i < 3; i++)
	{
		CString bufr;
		if(i < 2) 
		{
			j = m_curValue.Find(',', j);
			bufr = m_curValue.Mid(k, j-k);
		}
		else 
			bufr = m_curValue.Mid(k);

		xyz[i] = (float)atof(bufr);

		j++;
		k = j;
	}

	D3DVECTOR v = { xyz[0], xyz[1], xyz[2] };
	return v;
}

void CPropertyItem::VectorSet(D3DVECTOR& v)
{
	m_curValue.Format("%f, %f, %f", v.x, v.y, v.z);
}

CPropertyList::CPropertyList()
{
}

CPropertyList::~CPropertyList()
{
}


BEGIN_MESSAGE_MAP(CPropertyList, CListBox)
	//{{AFX_MSG_MAP(CPropertyList)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_CBN_KILLFOCUS(IDC_PROPCMBBOX, OnKillfocusCmbBox)
	ON_CBN_SELCHANGE(IDC_PROPCMBBOX, OnSelchangeCmbBox)
	ON_EN_KILLFOCUS(IDC_PROPEDITBOX, OnKillfocusEditBox)
	ON_EN_CHANGE(IDC_PROPEDITBOX, OnChangeEditBox)
	ON_BN_CLICKED(IDC_PROP_BUTTON, OnButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyList message handlers

BOOL CPropertyList::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CListBox::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~(LBS_OWNERDRAWVARIABLE | LBS_SORT);
	cs.style |= LBS_OWNERDRAWFIXED;

	m_bTracking = FALSE;
	m_nDivider = 0;
	m_bDivIsSet = FALSE;

	return TRUE;
}

void CPropertyList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = 20; //pixels
}


void CPropertyList::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	CDC dc;
	dc.Attach(lpDIS->hDC);
	CRect rectFull = lpDIS->rcItem;
	CRect rect = rectFull;
	if (m_nDivider==0)
		m_nDivider = rect.Width() / 2;
	rect.left = m_nDivider;
	CRect rect2 = rectFull;
	rect2.right = rect.left - 1;
	UINT nIndex = lpDIS->itemID;

	if (nIndex != (UINT) -1)
	{
		//draw two rectangles, one for each row column
		dc.FillSolidRect(rect2,RGB(192,192,192));
		dc.DrawEdge(rect2,EDGE_SUNKEN,BF_BOTTOMRIGHT);
		dc.DrawEdge(rect,EDGE_SUNKEN,BF_BOTTOM);

		//get the CPropertyItem for the current row
		CPropertyItem* pItem = (CPropertyItem*) GetItemDataPtr(nIndex);

		//write the property name in the first rectangle
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(pItem->m_propName,CRect(rect2.left+3,rect2.top+3,
											rect2.right-3,rect2.bottom+3),
					DT_LEFT | DT_SINGLELINE);

		// 留