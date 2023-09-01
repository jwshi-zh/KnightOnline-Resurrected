#if !defined(AFX_TEXVIEWER_H__E7AB26A3_E760_421C_B0CE_E2118D0FB88A__INCLUDED_)
#define AFX_TEXVIEWER_H__E7AB26A3_E760_421C_B0CE_E2118D0FB88A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TexViewer.h : header file
//
#define UM_UPDATE_INFO	((WM_USER)+101)
/////////////////////////////////////////////////////////////////////////////
// CTexViewer window
#define MAX_IMAGETYPE		200
class CN3Texture;
class CTexViewer : public CWnd
{
// Construction
public:
	CTexViewer();

// Attributes
public:
	enum	eEDITMODE {EDITMODE_SELECT=0, EDITMODE_ZOOM, EDITMODE_HAND, NUM_EDITMODE};
	eEDITMODE		GetEditMode() const {return m_eEditMode;}
	CRect			GetSelectedRect() const {return m_rcSelectedRect;}
	CSize			GetTexSize() const {return m_TexSize;}
protected:
	CN3Texture*		m_pTex;				// texture
	CSize			m_TexSize;			// texture size
	float			m_fScale;			// screen magnification
	CPoint			m_ptLeftTopInImage;	// The coordinates of the upper left corner of the texture shown in the upper left corner of this window.
	eEDITMODE		m_eEditMode;		// The current editing situation. (eg, area selection, zoom in/out...)
	CRect			m_rcSelectedRect;	// Selected rectangle (based on image coordinates)
	BOOL			m_bDrag;			// Are you dragging?
	CPen			m_WhiteDashPen;		// white dotted line
	CPoint			m_ptMouseOld;		// Remember the mouse's previous point
	BOOL			m_bDeselect;		// to deselect?
	CPoint			m_ptClickOffset;	// When you click to move the selection area
										// Relative coordinates of the click point of the selection window (selection area lefttop 0,0 standard) (image pixel coordinate system)
										
	enum	eDRAGTYPE {DRAGTYPE_NONE=0, DRAGTYPE_MOVE, DRAGTYPE_LEFT, DRAGTYPE_RIGHT,
					DRAGTYPE_TOP, DRAGTYPE_BOTTOM, DRAGTYPE_LEFTTOP, DRAGTYPE_RIGHTTOP,
					DRAGTYPE_LEFTBOTTOM, DRAGTYPE_RIGHTBOTTOM, DRAGTYPE_SELECT};
	eDRAGTYPE		m_eDragType;			// Drag state

	// Cursor
	HCURSOR			m_hCursorSelect;
	HCURSOR			m_hCursorZoomIn;
	HCURSOR			m_hCursorZoomOut;
	HCURSOR			m_hCursorHand;

	HCURSOR			m_hCursorSizeAll;
	HCURSOR			m_hCursorSizeWE;
	HCURSOR			m_hCursorSizeNS;
	HCURSOR			m_hCursorSizeNWSE;
	HCURSOR			m_hCursorSizeNESW;

	// image type related
	int				m_iImageTypeCount;								// The number of image types to be selected
	CRect			m_ImageRects[MAX_IMAGETYPE];					// ImageRect of m_iImageTypeCount
	int				m_iCurSelectedImage;							// Currently selected ImageType
// Operations
public:
	void			Release();
	BOOL			Zoom(BOOL bZoomIn);		// in: zoom in, out: zoom out
	BOOL			Zoom(float fScale);		// Zoom to fx
	void			Render();				// Rendering the texture
	void			SetTexture(LPCTSTR pszFName);	// texture designation
	eEDITMODE		SetEditMode(eEDITMODE eMode);	// Mode change (zoom, hand, select) returns the previous mode if it fails.
	void			SetLeftTopInImage(CPoint ptLeftTop);	// Replace top left coordinates of image
	BOOL			GetSelectedUVRect(struct __FLOAT_RECT* pFRect) const;	// Get the currently selected UV coordinates
	void			SetSelectedUVRect(const struct __FLOAT_RECT* pFRect);	// Put the currently selected UV coordinates

	// image type related
	void			SetImageTypeCount(int iCount) {m_iImageTypeCount = iCount;}	// Setting the number of image types
	BOOL			SetImageTypeIndex(int iIndex);	// Set zero base selected image type
	CRect			GetImageRect(int iIndex);
	BOOL			AutoMultiRectSelect(BOOL bHorizon, CString& strErrMsg);
protected:
	BOOL			ScreenToImage(POINT	*pPoint);	// screen coordinates to image coordinates
	BOOL			ScreenToImage(RECT* pRect);		// screen coordinates to image coordinates
	BOOL			ImageToScreen(POINT	*pPoint);	// image coordinates to screen coordinates
	BOOL			ImageToScreen(RECT* pRect);		// image coordinates to screen coordinates
	eDRAGTYPE		CheckDragType(CRect rcSel, CPoint point);
	void			ProcessDrag(CPoint point);		// Routine to process in case of region transformation
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTexViewer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTexViewer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTexViewer)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXVIEWER_H__E7AB26A3_E760_421C_B0CE_E2118D0FB88A__INCLUDED_)
