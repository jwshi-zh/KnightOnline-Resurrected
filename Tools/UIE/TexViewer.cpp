// TexViewer.cpp : implementation file
//

#include "stdafx.h"
#include "UIE.h"
#include "TexViewer.h"
#include "MainFrm.h"
#include "..\N3Base\N3UIDef.h"
#include "..\N3Base\N3Texture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTexViewer

CTexViewer::CTexViewer()
:m_WhiteDashPen(PS_DOT, 1, RGB(255,255,255))
{
	m_pTex = NULL;
	Release();

	m_hCursorSelect = LoadCursor(NULL, IDC_CROSS);
	m_hCursorZoomIn = (HCURSOR)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_CURSOR_ZOOMIN), IMAGE_CURSOR, 0, 0,LR_DEFAULTSIZE);
	m_hCursorZoomOut = (HCURSOR)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_CURSOR_ZOOMOUT), IMAGE_CURSOR, 0, 0,LR_DEFAULTSIZE);
	m_hCursorHand = (HCURSOR)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND), IMAGE_CURSOR, 0, 0,LR_DEFAULTSIZE);

	m_hCursorSizeAll = LoadCursor(NULL, IDC_SIZEALL);
	m_hCursorSizeWE = LoadCursor(NULL, IDC_SIZEWE);
	m_hCursorSizeNS = LoadCursor(NULL, IDC_SIZENS);
	m_hCursorSizeNWSE = LoadCursor(NULL, IDC_SIZENWSE);
	m_hCursorSizeNESW = LoadCursor(NULL, IDC_SIZENESW);
}

CTexViewer::~CTexViewer()
{
	Release();
}


BEGIN_MESSAGE_MAP(CTexViewer, CWnd)
	//{{AFX_MSG_MAP(CTexViewer)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTexViewer message handlers

BOOL CTexViewer::OnEraseBkgnd(CDC* pDC) 
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	CN3EngTool* pEng = &(pFrm->m_Eng);

	CRect rc;
	GetClientRect(&rc);
	pEng->SetViewPort(rc);
	pEng->Clear(0xff606060, rc);

	pEng->s_lpD3DDev->BeginScene();
	Render();
	pEng->s_lpD3DDev->EndScene();
	pEng->Present(m_hWnd);

	return CWnd::OnEraseBkgnd(pDC);
}

void CTexViewer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	dc.SetViewportOrg(-m_ptLeftTopInImage.x * m_fScale, -m_ptLeftTopInImage.y * m_fScale);
	// Draw the currently selected area
	if (m_rcSelectedRect.top != -1)
	{
		// After rearranging the left, right, top and bottom of the selected area according to the size, convert to screen coordinates
		CRect rcSelected;
		if (m_rcSelectedRect.left<m_rcSelectedRect.right)
		{
			rcSelected.left = m_rcSelectedRect.left;
			rcSelected.right = m_rcSelectedRect.right;
		}
		else
		{
			rcSelected.left = m_rcSelectedRect.right;
			rcSelected.right = m_rcSelectedRect.left;
		}
		if (m_rcSelectedRect.top<m_rcSelectedRect.bottom)
		{
			rcSelected.top = m_rcSelectedRect.top;
			rcSelected.bottom = m_rcSelectedRect.bottom;
		}
		else
		{
			rcSelected.top = m_rcSelectedRect.bottom;
			rcSelected.bottom = m_rcSelectedRect.top;
		}
		rcSelected.left *= m_fScale;
		rcSelected.right *= m_fScale;
		rcSelected.top *= m_fScale;
		rcSelected.bottom *= m_fScale;
		
		dc.SetBkColor(RGB(0,0,0));
		dc.SelectStockObject(NULL_BRUSH);
		CPen* pOldPen = dc.SelectObject(&m_WhiteDashPen);
		dc.Rectangle(&rcSelected);	// paint with a white dotted pen
		dc.SelectObject(pOldPen);
	}
	// Draw if there is an area by ImageType
	int i;
	int iOldMode = dc.SetROP2(R2_NOTXORPEN);
	dc.SelectStockObject(NULL_BRUSH);
	for (i=0; i<m_iImageTypeCount; ++i)
	{
		if (m_iCurSelectedImage == i) continue;	// Skip currently selected
		CRect rcTmp = m_ImageRects[i];
		rcTmp.left *= m_fScale;		rcTmp.right *= m_fScale;
		rcTmp.top *= m_fScale;		rcTmp.bottom *= m_fScale;
		dc.Rectangle(&rcTmp);
	}
	dc.SetROP2(iOldMode);

	dc.SetViewportOrg(0,0);
	// Do not call CWnd::OnPaint() for painting messages
}

void CTexViewer::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bDrag = TRUE;
	SetCapture();

	if (EDITMODE_SELECT == m_eEditMode)
	{
		// If there is a specified rectangle, check whether the rectangle is transformed.
		if (-1 != m_rcSelectedRect.left)
		{
			CRect rcReal = m_rcSelectedRect;
			ImageToScreen(&rcReal);
			m_eDragType = CheckDragType(rcReal, point);
		}
		else m_eDragType = DRAGTYPE_NONE;

		if (DRAGTYPE_NONE == m_eDragType)
		{
			CPoint pt = point;
			ScreenToImage(&pt);				// Convert to image coordinates
			m_rcSelectedRect.SetRect(pt, pt);
			m_eDragType = DRAGTYPE_SELECT;
			m_bDeselect = TRUE;
			Invalidate();
		}
		else if (DRAGTYPE_MOVE == m_eDragType)
		{
			m_ptClickOffset = point;
			ScreenToImage(&m_ptClickOffset);
			m_ptClickOffset = m_ptClickOffset-m_rcSelectedRect.TopLeft();
		}
	}
	GetParent()->PostMessage(UM_UPDATE_INFO, TRUE, MAKELPARAM((short)point.y,(short)point.x));
	
	m_ptMouseOld = point;
	CWnd::OnLButtonDown(nFlags, point);
}

void CTexViewer::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bDrag)
	{
		m_bDrag = FALSE;
		if (EDITMODE_SELECT == m_eEditMode)
		{
			if (DRAGTYPE_SELECT == m_eDragType)
			{
				if (m_bDeselect) m_rcSelectedRect.SetRect(-1,-1,-1,-1);	// deselect
				else
				{
					CPoint pt = point;
					ScreenToImage(&pt);				// Convert to image coordinates
					
					// Organize the left and top of the rectangle with small coordinates, right and bottom with large coordinates
					if (m_rcSelectedRect.left > pt.x)
					{
						m_rcSelectedRect.right = m_rcSelectedRect.left;
						m_rcSelectedRect.left = pt.x;
					}
					else m_rcSelectedRect.right = pt.x;
					if (m_rcSelectedRect.top > pt.y)
					{
						m_rcSelectedRect.bottom = m_rcSelectedRect.top;
						m_rcSelectedRect.top = pt.y;
					}
					else m_rcSelectedRect.bottom = pt.y;
				}			
			}
			else
			{
				ProcessDrag(point);
			}
			m_eDragType = DRAGTYPE_NONE;
			if (m_iCurSelectedImage>=0) m_ImageRects[m_iCurSelectedImage] = m_rcSelectedRect;	// If saving by ImageType, copy the selected rectangle
			Invalidate();
		}
		else if (EDITMODE_ZOOM == m_eEditMode)
		{
			if (m_ptMouseOld == point)
			{
				CRect rc;
				GetClientRect(&rc);
				CPoint ptPrev = point;
				ScreenToImage(&ptPrev);	// Save image coordinates before zooming
				Zoom((GetAsyncKeyState(VK_MENU) & 0xff00) ? FALSE : TRUE);
				CPoint ptNext = ptPrev;
				ImageToScreen(&ptNext);	// Convert image coordinates to screen coordinates after zooming
				ptNext.x = int((ptNext.x-rc.CenterPoint().x)/m_fScale);		// set it to the center of the screen
				ptNext.y = int((ptNext.y-rc.CenterPoint().y)/m_fScale);
				SetLeftTopInImage( m_ptLeftTopInImage + ptNext);	// shift by difference
			}
			else
			{
			}
		}
		else if (EDITMODE_HAND == m_eEditMode)
		{
			CPoint ptDiff = m_ptMouseOld - point;
			ptDiff.x = (int)(ptDiff.x / m_fScale);
			ptDiff.y = (int)(ptDiff.y / m_fScale);
			SetLeftTopInImage(m_ptLeftTopInImage+ptDiff);
		}
	}
	m_ptMouseOld = point;
	ReleaseCapture();
	GetParent()->PostMessage(UM_UPDATE_INFO, TRUE, MAKELPARAM((short)point.y,(short)point.x));
	CWnd::OnLButtonUp(nFlags, point);
}

void CTexViewer::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bDrag && (MK_LBUTTON & nFlags))
	{
		if (EDITMODE_SELECT == m_eEditMode)
		{
			// screen scroll check
			CRect rcClient;
			GetClientRect(&rcClient);
			CPoint ptOffset(0,0);
			int iBorder = 20;	// Increase by 1 if within 20 of the edge of the screen
			if (point.x < rcClient.left+iBorder) ptOffset.x = -1;
			else if (point.x > rcClient.right-iBorder) ptOffset.x = 1;
			if (point.y < rcClient.top+iBorder) ptOffset.y = -1;
			else if (point.y > rcClient.bottom-iBorder) ptOffset.y = 1;
			iBorder = 10;		// If within 10, increase by 5
			if (point.x < rcClient.left+iBorder) ptOffset.x = -5;
			else if (point.x > rcClient.right-iBorder) ptOffset.x = 5;
			if (point.y < rcClient.top+iBorder) ptOffset.y = -5;
			else if (point.y > rcClient.bottom-iBorder) ptOffset.y = 5;
			iBorder = 5;		// If within 5, increase by 20
			if (point.x < rcClient.left+iBorder) ptOffset.x = -20;
			else if (point.x > rcClient.right-iBorder) ptOffset.x = 20;
			if (point.y < rcClient.top+iBorder) ptOffset.y = -20;
			else if (point.y > rcClient.bottom-iBorder) ptOffset.y = 20;
			if (CPoint(0,0) != ptOffset) SetLeftTopInImage(m_ptLeftTopInImage+ptOffset);

			if (DRAGTYPE_SELECT == m_eDragType)
			{
				// Update selection
				CPoint pt = point;
				ScreenToImage(&pt);				// Convert to image coordinates
				m_rcSelectedRect.right = pt.x;
				m_rcSelectedRect.bottom = pt.y;
				m_bDeselect = FALSE;		// deselect off
			}
			else
			{	// In the case of area deformation, it is processed as follows
				ProcessDrag(point);
			}
			Invalidate();
		}
		else if (EDITMODE_HAND == m_eEditMode)
		{
			CPoint ptDiff = m_ptMouseOld - point;
			ptDiff.x = (int)(ptDiff.x / m_fScale);
			ptDiff.y = (int)(ptDiff.y / m_fScale);
			SetLeftTopInImage(m_ptLeftTopInImage+ptDiff);
		}
	}
	m_ptMouseOld = point;
	GetParent()->PostMessage(UM_UPDATE_INFO, TRUE, MAKELPARAM((short)point.y,(short)point.x));
	CWnd::OnMouseMove(nFlags, point);
}

void CTexViewer::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	SetLeftTopInImage(m_ptLeftTopInImage);	// Since the size has changed, set it again so that the top left corner is recalculated
	
}

BOOL CTexViewer::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	switch(m_eEditMode)
	{
	case EDITMODE_SELECT:
		{
			HCURSOR hCur = m_hCursorSelect;
			eDRAGTYPE		eDT = m_eDragType;
			if (DRAGTYPE_NONE == m_eDragType)	// If the drag type is NONE, get the position of the cursor and test it
			{
				CPoint pt;
				if (GetCursorPos(&pt))
				{
					ScreenToClient(&pt);
					CRect rcReal = m_rcSelectedRect;
					ImageToScreen(&rcReal);
					eDT = CheckDragType(rcReal, pt);
				}
			}

			switch(eDT)
			{
			case DRAGTYPE_NONE:			break;
			case DRAGTYPE_MOVE:
				hCur=m_hCursorSizeAll;		break;
			case DRAGTYPE_LEFT:
			case DRAGTYPE_RIGHT:
				hCur=m_hCursorSizeWE;			break;
			case DRAGTYPE_TOP:
			case DRAGTYPE_BOTTOM:
				hCur=m_hCursorSizeNS;			break;
			case DRAGTYPE_LEFTTOP:
			case DRAGTYPE_RIGHTBOTTOM:
				hCur=m_hCursorSizeNWSE;			break;
			case DRAGTYPE_RIGHTTOP:
			case DRAGTYPE_LEFTBOTTOM:
				hCur=m_hCursorSizeNESW;			break;
			}
			SetCursor(hCur);
			return TRUE;
		}
		break;
	case EDITMODE_ZOOM:
		if (GetAsyncKeyState(VK_MENU) & 0xff00)	SetCursor(m_hCursorZoomOut);
		else SetCursor(m_hCursorZoomIn);
		return TRUE;
		break;
	case EDITMODE_HAND:
		SetCursor(m_hCursorHand);
		return TRUE;
		break;
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CTexViewer::Release()
{
	CN3Base::s_MngTex.Delete(&m_pTex);
	m_TexSize = CSize(0,0);
	m_fScale = 1.0f;
	m_ptLeftTopInImage = CPoint(0,0);
	m_eEditMode = EDITMODE_SELECT;
	m_rcSelectedRect.SetRect(-1,-1,-1,-1);
	m_bDrag = FALSE;
	m_ptMouseOld = CPoint(-1,-1);
	m_bDeselect = FALSE;
	m_eDragType = DRAGTYPE_NONE;
	m_ptClickOffset = CPoint(-1,-1);

	int i;
	for (i=0; i<MAX_IMAGETYPE; ++i)
	{
		m_ImageRects[i].SetRect(-1,-1,-1,-1);
	}
	m_iImageTypeCount = 0;
	m_iCurSelectedImage = -1;
}

void CTexViewer::Render()
{
	if (NULL == m_pTex) return;
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	CN3EngTool* pEng = &(pFrm->m_Eng);
	LPDIRECT3DDEVICE8 lpD3DDev = pEng->s_lpD3DDev;

	// backup render state
	DWORD dwZEnable, dwAlphaBlend, dwSrcBlend, dwDestBlend, dwFog;
	lpD3DDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlend);
	lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
	lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
	lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
	DWORD dwMagFilter, dwMinFilter, dwMipFilter;
	lpD3DDev->GetTextureStageState(0, D3DTSS_MAGFILTER,   &dwMagFilter);
	lpD3DDev->GetTextureStageState(0, D3DTSS_MINFILTER,   &dwMinFilter);
	lpD3DDev->GetTextureStageState(0, D3DTSS_MIPFILTER,   &dwMipFilter);

	// set render state
	if (D3DZB_FALSE != dwZEnable) lpD3DDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	if (FALSE != dwAlphaBlend) lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	if (FALSE != dwFog) lpD3DDev->SetRenderState(D3DRS_FOGENABLE   , FALSE);
	if (D3DTEXF_POINT != dwMagFilter ) lpD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER,   D3DTEXF_POINT);
	if (D3DTEXF_POINT != dwMinFilter ) lpD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER,   D3DTEXF_POINT);
	if (D3DTEXF_NONE != dwMipFilter ) lpD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER,   D3DTEXF_NONE);
	lpD3DDev->SetTexture(0, m_pTex->Get());
	lpD3DDev->SetTexture(1, NULL);
	lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_SELECTARG1);
	lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1,  D3DTA_TEXTURE);
	lpD3DDev->SetTextureStageState(1, D3DTSS_COLOROP,  D3DTOP_DISABLE);

	//  Calculated considering the picture position and magnification
	CRect rcRender(0,0, m_TexSize.cx*m_fScale, m_TexSize.cy*m_fScale);
	rcRender.OffsetRect(-m_ptLeftTopInImage.x*m_fScale, -m_ptLeftTopInImage.y*m_fScale);
	static __VertexTransformed		Vertices[4];
	const float z = 0.9f; const float rhw = 1.0f; const D3DCOLOR color = 0xffffffff;
	Vertices[0].Set((float)rcRender.left, (float)rcRender.top, z, rhw, color, 0.0f, 0.0f);
	Vertices[1].Set((float)rcRender.right, (float)rcRender.top, z, rhw, color, 1.0f, 0.0f);
	Vertices[2].Set((float)rcRender.right, (float)rcRender.bottom, z, rhw, color, 1.0f, 1.0f);
	Vertices[3].Set((float)rcRender.left, (float)rcRender.bottom, z, rhw, color, 0.0f, 1.0f);

	lpD3DDev->SetVertexShader(FVF_TRANSFORMED);
	HRESULT hr = lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Vertices, sizeof(Vertices[0]));

	// restore
	if (D3DZB_FALSE != dwZEnable) lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	if (FALSE != dwAlphaBlend) lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlend);
	if (FALSE != dwFog) lpD3DDev->SetRenderState(D3DRS_FOGENABLE   , dwFog);
	if (D3DTEXF_POINT != dwMagFilter ) lpD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER,   dwMagFilter);
	if (D3DTEXF_POINT != dwMinFilter ) lpD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER,   dwMinFilter);
	if (D3DTEXF_NONE != dwMipFilter ) lpD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER,   dwMipFilter);
}

void CTexViewer::SetTexture(LPCTSTR pszFName)
{
	CN3Base::s_MngTex.Delete(&m_pTex);
	m_pTex = CN3Base::s_MngTex.Get(pszFName);

	if (m_pTex)	{m_TexSize.cx = m_pTex->Width();	m_TexSize.cy = m_pTex->Height();}
	else m_TexSize.cx = m_TexSize.cy = 0;
}

BOOL CTexViewer::Zoom(BOOL bZoomIn)
{
	if (bZoomIn) m_fScale *= 2.0f;
	else m_fScale *= 0.5f;
	if (m_fScale>16.0f) {m_fScale = 16.0f; return FALSE;}
	if (m_fScale<0.125f) {m_fScale = 0.125f; return FALSE;}
	Invalidate();
	return TRUE;
}

BOOL CTexViewer::Zoom(float fScale)
{
	if (fScale>16.0f) fScale = 16.0f;
	if (fScale<0.125f) fScale = 0.125f;
	m_fScale = fScale;
	Invalidate();
	return TRUE;
}

BOOL CTexViewer::ScreenToImage(POINT	*pPoint)
{
	if (pPoint == NULL || 0.0f == m_fScale || m_ptLeftTopInImage == CPoint(-1,-1)) return FALSE;
	pPoint->x = (int)(m_ptLeftTopInImage.x + pPoint->x/m_fScale);
	pPoint->y = (int)(m_ptLeftTopInImage.y + pPoint->y/m_fScale);
	return TRUE;
}

BOOL CTexViewer::ScreenToImage(RECT* pRect)
{
	if (pRect == NULL || 0.0f == m_fScale || m_ptLeftTopInImage == CPoint(-1,-1)) return FALSE;
	pRect->left = (int)(m_ptLeftTopInImage.x + pRect->left/m_fScale);
	pRect->right = (int)(m_ptLeftTopInImage.x + pRect->right/m_fScale);
	pRect->top = (int)(m_ptLeftTopInImage.y + pRect->top/m_fScale);
	pRect->bottom = (int)(m_ptLeftTopInImage.y + pRect->bottom/m_fScale);
	return TRUE;
}

BOOL CTexViewer::ImageToScreen(POINT	*pPoint)
{
	if (pPoint == NULL || 0.0f == m_fScale || m_ptLeftTopInImage == CPoint(-1,-1)) return FALSE;
	pPoint->x = (int)((pPoint->x - m_ptLeftTopInImage.x)*m_fScale);
	pPoint->y = (int)((pPoint->y - m_ptLeftTopInImage.y)*m_fScale);
	return TRUE;
}

BOOL CTexViewer::ImageToScreen(RECT* pRect)
{
	if (pRect == NULL || 0.0f == m_fScale || m_ptLeftTopInImage == CPoint(-1,-1)) return FALSE;
	pRect->left = (int)((pRect->left - m_ptLeftTopInImage.x)*m_fScale);
	pRect->right = (int)((pRect->right - m_ptLeftTopInImage.x)*m_fScale);
	pRect->top = (int)((pRect->top - m_ptLeftTopInImage.y)*m_fScale);
	pRect->bottom = (int)((pRect->bottom - m_ptLeftTopInImage.y)*m_fScale);
	return TRUE;
}

CTexViewer::eEDITMODE CTexViewer::SetEditMode(eEDITMODE eMode)
{
	if (m_eEditMode == eMode) return m_eEditMode;
	if (m_bDrag) return m_eEditMode;		// If you are dragging, do not change it. There is a high possibility of abnormal operation.
	m_eEditMode = eMode;
	return m_eEditMode;
}

void CTexViewer::SetLeftTopInImage(CPoint ptLeftTop)
{
	// Calculate the limiting area
	//  Calculated considering the picture position and magnification
	CRect rcClient;				// Client area
	GetClientRect(&rcClient);
	CPoint ptLimit;
	ptLimit.x = int(m_TexSize.cx - rcClient.Width()/m_fScale);
	ptLimit.y = int(m_TexSize.cy - rcClient.Height()/m_fScale);
	if (ptLimit.x < 0) ptLimit.x = 0;	if (ptLimit.y < 0) ptLimit.y = 0;

	// Calculation by limiting area
	if (ptLeftTop.x < 0) ptLeftTop.x = 0;	if (ptLeftTop.y < 0) ptLeftTop.y = 0;
	if (ptLeftTop.x > ptLimit.x) ptLeftTop.x = ptLimit.x;
	if (ptLeftTop.y > ptLimit.y) ptLeftTop.y = ptLimit.y;

	m_ptLeftTopInImage = ptLeftTop;
	Invalidate();
}

BOOL CTexViewer::GetSelectedUVRect(__FLOAT_RECT* pFRect) const
{
	if (NULL == pFRect) return FALSE;
	if (-1 == m_rcSelectedRect.left) return FALSE;
	ASSERT(m_TexSize.cx > 2 && m_TexSize.cy > 2);

	CRect rc = m_rcSelectedRect;
	if (rc.right < rc.left)
	{
		rc.left = m_rcSelectedRect.right;
		rc.right = m_rcSelectedRect.left;
	}
	if (rc.bottom < rc.top)
	{
		rc.top = m_rcSelectedRect.bottom;
		rc.bottom = m_rcSelectedRect.top;
	}
	pFRect->left = 	(float)rc.left/m_TexSize.cx;	pFRect->right = (float)rc.right/m_TexSize.cx;
	pFRect->top = 	(float)rc.top/m_TexSize.cy;	pFRect->bottom = (float)rc.bottom/m_TexSize.cy;

	return TRUE;
}
void CTexViewer::SetSelectedUVRect(const __FLOAT_RECT* pFRect)
{
	if (NULL == pFRect) return;
	ASSERT(m_TexSize.cx > 2 && m_TexSize.cy > 2);

	m_rcSelectedRect.left = int(pFRect->left*m_TexSize.cx + 0.5f);		// The reason for adding 0.5f is rounding..
	m_rcSelectedRect.right = int(pFRect->right*m_TexSize.cx + 0.5f);
	m_rcSelectedRect.top = int(pFRect->top*m_TexSize.cy + 0.5f);
	m_rcSelectedRect.bottom = int(pFRect->bottom*m_TexSize.cy + 0.5f);
}

CTexViewer::eDRAGTYPE CTexViewer::CheckDragType(CRect rcSel, CPoint point)
{
	CPoint pt;
	CRect rcCheck;
	const int W = 4;
	pt = rcSel.TopLeft();
	rcCheck.SetRect(pt.x-W, pt.y-W, pt.x+W, pt.y+W);
	if (rcCheck.PtInRect(point)) return DRAGTYPE_LEFTTOP;
	pt = rcSel.BottomRight();
	rcCheck.SetRect(pt.x-W, pt.y-W, pt.x+W, pt.y+W);
	if (rcCheck.PtInRect(point)) return DRAGTYPE_RIGHTBOTTOM;
	pt = CPoint(rcSel.right, rcSel.top);
	rcCheck.SetRect(pt.x-W, pt.y-W, pt.x+W, pt.y+W);
	if (rcCheck.PtInRect(point)) return DRAGTYPE_RIGHTTOP;
	pt = CPoint(rcSel.left, rcSel.bottom);
	rcCheck.SetRect(pt.x-W, pt.y-W, pt.x+W, pt.y+W);
	if (rcCheck.PtInRect(point)) return DRAGTYPE_LEFTBOTTOM;

	rcCheck.SetRect(rcSel.left-W, rcSel.top, rcSel.left+W, rcSel.bottom);
	if (rcCheck.PtInRect(point)) return DRAGTYPE_LEFT;
	rcCheck.SetRect(rcSel.right-W, rcSel.top, rcSel.right+W, rcSel.bottom);
	if (rcCheck.PtInRect(point)) return DRAGTYPE_RIGHT;
	rcCheck.SetRect(rcSel.left, rcSel.top-W, rcSel.right, rcSel.top+W);
	if (rcCheck.PtInRect(point)) return DRAGTYPE_TOP;
	rcCheck.SetRect(rcSel.left, rcSel.bottom-W, rcSel.right, rcSel.bottom+W);
	if (rcCheck.PtInRect(point)) return DRAGTYPE_BOTTOM;

	if (rcSel.PtInRect(point)) return DRAGTYPE_MOVE;
	return DRAGTYPE_NONE;
}

// Routine to process in case of region transformation
void CTexViewer::ProcessDrag(CPoint point)
{
	ScreenToImage(&point);

	switch(m_eDragType)
	{
	case DRAGTYPE_NONE:			break;
	case DRAGTYPE_MOVE:
		{
			CPoint ptDiff = (point - m_ptClickOffset) - m_rcSelectedRect.TopLeft();
			if (m_rcSelectedRect.left+ptDiff.x < 0) ptDiff.x = -m_rcSelectedRect.left;
			else if (m_rcSelectedRect.right+ptDiff.x >= m_TexSize.cx) ptDiff.x = m_TexSize.cx-m_rcSelectedRect.right;
			if (m_rcSelectedRect.top+ptDiff.y < 0) ptDiff.y = -m_rcSelectedRect.top;
			else if (m_rcSelectedRect.bottom+ptDiff.y >= m_TexSize.cy) ptDiff.y = m_TexSize.cy-m_rcSelectedRect.bottom;
			m_rcSelectedRect.OffsetRect(ptDiff);
		}
		break;
	case DRAGTYPE_LEFT:			
		if (point.x<0) m_rcSelectedRect.left = 0;
		else if (point.x>=m_rcSelectedRect.right) m_rcSelectedRect.left = m_rcSelectedRect.right;
		else m_rcSelectedRect.left = point.x;
		break;
	case DRAGTYPE_RIGHT:
		if (point.x<=m_rcSelectedRect.left) m_rcSelectedRect.right = m_rcSelectedRect.left;
		else if (point.x>m_TexSize.cx) m_rcSelectedRect.right = m_TexSize.cx;
		else m_rcSelectedRect.right = point.x;
		break;
	case DRAGTYPE_TOP:
		if (point.y<0) m_rcSelectedRect.top = 0;
		else if (point.y>=m_rcSelectedRect.bottom) m_rcSelectedRect.top = m_rcSelectedRect.bottom;
		else m_rcSelectedRect.top = point.y;
		break;
	case DRAGTYPE_BOTTOM:
		if (point.y<=m_rcSelectedRect.top) m_rcSelectedRect.bottom = m_rcSelectedRect.top;
		else if (point.y>m_TexSize.cy) m_rcSelectedRect.bottom = m_TexSize.cy;
		else m_rcSelectedRect.bottom = point.y;
		break;
	case DRAGTYPE_LEFTTOP:
		if (point.x<0) m_rcSelectedRect.left = 0;
		else if (point.x>=m_rcSelectedRect.right) m_rcSelectedRect.left = m_rcSelectedRect.right;
		else m_rcSelectedRect.left = point.x;
		if (point.y<0) m_rcSelectedRect.top = 0;
		else if (point.y>=m_rcSelectedRect.bottom) m_rcSelectedRect.top = m_rcSelectedRect.bottom;
		else m_rcSelectedRect.top = point.y;
		break;
	case DRAGTYPE_RIGHTTOP:
		if (point.x<=m_rcSelectedRect.left) m_rcSelectedRect.right = m_rcSelectedRect.left;
		else if (point.x>m_TexSize.cx) m_rcSelectedRect.right = m_TexSize.cx;
		else m_rcSelectedRect.right = point.x;
		if (point.y<0) m_rcSelectedRect.top = 0;
		else if (point.y>=m_rcSelectedRect.bottom) m_rcSelectedRect.top = m_rcSelectedRect.bottom;
		else m_rcSelectedRect.top = point.y;
		break;
	case DRAGTYPE_LEFTBOTTOM:
		if (point.x<0) m_rcSelectedRect.left = 0;
		else if (point.x>=m_rcSelectedRect.right) m_rcSelectedRect.left = m_rcSelectedRect.right;
		else m_rcSelectedRect.left = point.x;
		if (point.y<=m_rcSelectedRect.top) m_rcSelectedRect.bottom = m_rcSelectedRect.top;
		else if (point.y>m_TexSize.cy) m_rcSelectedRect.bottom = m_TexSize.cy;
		else m_rcSelectedRect.bottom = point.y;
		break;
	case DRAGTYPE_RIGHTBOTTOM:
		if (point.x<=m_rcSelectedRect.left) m_rcSelectedRect.right = m_rcSelectedRect.left;
		else if (point.x>m_TexSize.cx) m_rcSelectedRect.right = m_TexSize.cx;
		else m_rcSelectedRect.right = point.x;
		if (point.y<=m_rcSelectedRect.top) m_rcSelectedRect.bottom = m_rcSelectedRect.top;
		else if (point.y>m_TexSize.cy) m_rcSelectedRect.bottom = m_TexSize.cy;
		else m_rcSelectedRect.bottom = point.y;
		break;
	}
}

BOOL CTexViewer::SetImageTypeIndex(int iIndex)
{
	if (iIndex<0 || iIndex>=m_iImageTypeCount) return FALSE;
	m_iCurSelectedImage = iIndex;
	m_rcSelectedRect = m_ImageRects[iIndex];
	Invalidate();
	return TRUE;
}

CRect CTexViewer::GetImageRect(int iIndex)
{
	if (iIndex<0 || iIndex>=m_iImageTypeCount){ASSERT(0);return CRect(0,0,0,0);}
	return m_ImageRects[iIndex];
}

BOOL CTexViewer::AutoMultiRectSelect(BOOL bHorizon, CString& strErrMsg)	// Image Rects connected horizontally or vertically are automatically selected in the same size.
{
	if (-1 == m_rcSelectedRect.left)
	{
		strErrMsg = "No area currently selected.";
		return FALSE;
	}
	else if (0 == m_rcSelectedRect.Width() || 0 == m_rcSelectedRect.Height())
	{
		strErrMsg = "The horizontal or vertical size of the selected area is 0니다.";
		return FALSE;
	}

	Invalidate();
	int i;
	int iRow = 0;
	int iCol = 0;
	CPoint ptLeftTop = m_rcSelectedRect.TopLeft();
	if (bHorizon)
	{	// Divide by moving horizontally
		for(i=0; i<m_iImageTypeCount; ++i)
		{
			if ( (ptLeftTop.x + m_rcSelectedRect.Width()*(iRow+1)) > m_TexSize.cx) {++iCol; iRow = 0;}
			if ( (ptLeftTop.y + m_rcSelectedRect.Height()*(iCol+1)) > m_TexSize.cy)
			{
				strErrMsg.Format("Not all rectangular areas can be selected. (%d sets)", i+1);
				return FALSE;
			}
			m_ImageRects[i].SetRect(ptLeftTop.x+m_rcSelectedRect.Width()*iRow,
									ptLeftTop.y+m_rcSelectedRect.Height()*iCol,
									ptLeftTop.x+m_rcSelectedRect.Width()*(iRow+1),
									ptLeftTop.y+m_rcSelectedRect.Height()*(iCol+1));
			++iRow;
		}
	}
	else
	{	// Divide by moving vertically
		for(i=0; i<m_iImageTypeCount; ++i)
		{
			if ( (ptLeftTop.y + m_rcSelectedRect.Height()*(iCol+1)) > m_TexSize.cy) {++iRow; iCol = 0;}
			if ( (ptLeftTop.x + m_rcSelectedRect.Width()*(iRow+1)) > m_TexSize.cx)
			{
				strErrMsg.Format("Not all rectangular areas can be selected. (%d sets)", i+1);
				return FALSE;
			}
			m_ImageRects[i].SetRect(ptLeftTop.x+m_rcSelectedRect.Width()*iRow,
									ptLeftTop.y+m_rcSelectedRect.Height()*iCol,
									ptLeftTop.x+m_rcSelectedRect.Width()*(iRow+1),
									ptLeftTop.y+m_rcSelectedRect.Height()*(iCol+1));
			++iCol;
		}
	}
	return TRUE;
}
