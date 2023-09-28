#if !defined(AFX_PROGRESSBAR_H__302B9656_4C10_4648_8A00_D23F8F9AD5DB__INCLUDED_)
#define AFX_PROGRESSBAR_H__302B9656_4C10_4648_8A00_D23F8F9AD5DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressBar window

class CProgressBar : public CProgressCtrl
{
	// Construction
public:
	CProgressBar();

	// Attributes
public:
protected:
	CString m_strMessage;
	int m_nSize;
	CRect m_Rect;

	// Operations
public:
	BOOL Create(LPCTSTR strMessage, int nSize, int MaxValue);
	// nSize: horizontal pixel size of the progress bar
	// MaxValue : maximum value of step of progress bar

	CStatusBar *GetStatusBar(); // Get StatusBar pointer from MainFrame
	void Resize();				// adjust position and size
	void Clear();				// reset
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressBar)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CProgressBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProgressBar)
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSBAR_H__302B9656_4C10_4648_8A00_D23F8F9AD5DB__INCLUDED_)
