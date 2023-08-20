#pragma once

#include "N3UIStatic.h"

class CN3UIEdit : public CN3UIStatic  
{
public:
	CN3UIEdit();
	virtual ~CN3UIEdit();
// class
protected:
	class CN3Caret
	{
	public:
		CN3Caret();
		virtual ~CN3Caret();
		void	SetPos(int x, int y);
		void	MoveOffset(int iOffsetX, int iOffsetY);
		void	SetSize(int iSize);
		void	SetColor(D3DCOLOR color);
		void	Render(LPDIRECT3DDEVICE9 lpD3DDev);
		void	InitFlckering();		// Blink reset..
		BOOL	m_bVisible;				// I can see
	protected:
		int		m_iSize;				// caret pixel size
		float	m_fFlickerTimePrev;	// Time to blink...
		bool	m_bFlickerStatus;
		// POINT m_ptPos; // pixel coordinates of caret
		__VertexTransformedColor	m_pVB[2];	// vertex buffer
	};

// Attributes
public:
/*
	static bool		AddEdit(CN3UIEdit* pEdit);
	static bool		DeleteEdit(CN3UIEdit* pEdit);
	static CN3UIEdit* GetEdit(HWND hWnd);
	static HWND		s_hWndParent;

	LRESULT APIENTRY EditWndProcFuncPointer(HWND hWnd, WORD Message, WPARAM wParam, LPARAM lParam);
	static LRESULT	APIENTRY EditWndProc(HWND hWnd, WORD Message, WPARAM wParam, LPARAM lParam);
	static			void SetParentWindowHwnd(HWND hWnd) { s_hWndParent = hWnd; }
*/
	static HWND		s_hWndEdit, s_hWndParent;
	static WNDPROC	s_lpfnEditProc;
	static char		s_szBuffTmp[512];

	static void				SetImeStatus(POINT ptPos, bool bOpen);
	static BOOL				CreateEditWindow(HWND hParent, RECT rect);
	static LRESULT APIENTRY EditWndProc(HWND hWnd, WORD Message, WPARAM wParam, LPARAM lParam);
	static void				UpdateTextFromEditCtrl();
	static void				UpdateCaretPosFromEditCtrl();

protected:
	static CN3Caret		s_Caret;
	UINT				m_nCaretPos;		// Character unit position (byte unit)
	int					m_iCompLength;		// If the number of bytes of characters currently being combined is 0, it is not being combined.
	int					m_iMaxStrLen;		// Maximum number of characters that can be written
	std::string			m_szPassword;		// password buffer

	CN3SndObj*			m_pSnd_Typing;		// sound when typing
// Operations
public:
	virtual const std::string&	GetString();
	virtual void				SetString(const std::string& szString);

	virtual bool		Load(HANDLE hFile);
	virtual void		Render();
	virtual void		Release();
	virtual void		SetVisible(bool bVisible);
	virtual DWORD		MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual BOOL		MoveOffset(int iOffsetX, int iOffsetY);		// Designation of position (the position of chilren is also changed. The position of caret is also changed.)
	void				KillFocus() const;			// remove focus
	bool				SetFocus();				// give focus
	bool				HaveFocus() const {return (this == s_pFocusedEdit);}
	void				SetCaretPos(UINT nPos);	// Set which byte is in.
	void				SetMaxString(int iMax);		// Specifies the maximum number of characters.
protected:
	BOOL				IsHangulMiddleByte( const char* lpszStr, int iPos );	// Is it the second byte character of Hangul?

#ifdef _N3TOOL
public:	
	virtual void		operator = (const CN3UIEdit& other);
	virtual bool		Save(HANDLE hFile);
	void				SetSndTyping(const std::string& strFileName);
	std::string			GetSndFName_Typing() const;
#endif
};
