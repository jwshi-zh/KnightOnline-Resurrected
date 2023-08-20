// PosDummy.cpp: implementation of the CPosDummy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "n3me.h"
#include "PosDummy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPosDummy::CPosDummy()
{
}

CPosDummy::~CPosDummy()
{

}

void CPosDummy::SetSelObj(CN3Transform* pObj)
{
	m_SelObjArray.RemoveAll();
	if (pObj)
	{
		m_SelObjArray.Add(pObj);
		m_vPos = pObj->Pos();
//		m_qRot.x = m_qRot.y = m_qRot.z = m_qRot.w = 0;
	}
}

void CPosDummy::SetVertex(__Vector3* pVertex)
{
	if(pVertex) m_vPos = (*pVertex);
}

BOOL CPosDummy::MouseMsgFilter(LPMSG pMsg)
{
	if (m_SelObjArray.GetSize() == 0) return FALSE;

	switch(pMsg->message)
	{
	case WM_MOUSEMOVE:
		{
			POINT point = {short(LOWORD(pMsg->lParam)), short(HIWORD(pMsg->lParam))};
			DWORD nFlags = pMsg->wParam;
			if (m_pSelectedCube && (nFlags & MK_LBUTTON))
			{
				__Vector3 vRayDir, vRayOrig;	// 