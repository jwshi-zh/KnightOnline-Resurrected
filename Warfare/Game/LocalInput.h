#pragma once

#include <DInput.h>

const int DK_NONE = 0;
const int DK_RELEASE = 1;
const int DK_PRESS = 2;
const int DK_REPEAT = 4;
const int NUMDIKEYS = 256;

const int MOUSE_LBCLICK		= 0x1;
const int MOUSE_LBCLICKED	= 0x2;
const int MOUSE_LBDOWN		= 0x4;
const int MOUSE_MBCLICK		= 0x8;
const int MOUSE_MBCLICKED	= 0x10;
const int MOUSE_MBDOWN		= 0x20;
const int MOUSE_RBCLICK		= 0x40;
const int MOUSE_RBCLICKED	= 0x80;
const int MOUSE_RBDOWN		= 0x100;
const int MOUSE_LBDBLCLK	= 0x200;
const int MOUSE_MBDBLCLK	= 0x400;
const int MOUSE_RBDBLCLK	= 0x800;

class CLocalInput
{
private:
	void AcquireMouse();
	void UnacquireMouse();
	void AcquireKeyboard();
	void UnacquireKeyboard();

protected:
	LPDIRECTINPUT8			m_lpDI;
	LPDIRECTINPUTDEVICE8	m_lpDIDKeyboard;
	// LPDIRECTINPUTDEVICE8	m_lpDIDMouse;

	HWND m_hWnd;

	// BOOL m_bMouse;
	// BOOL m_bKeyboard;

	int m_nMouseFlag, m_nMouseFlagOld; // Mouse button down flag
	DWORD m_dwTickLBDown; // For detecting left mouse button double click
	DWORD m_dwTickRBDown; // For right mouse button double click detection

	POINT	m_ptCurMouse; // current mouse pointer
	POINT	m_ptOldMouse; // previous mouse pointer

	RECT	m_rcLBDrag; // drag area
	RECT	m_rcMBDrag; // drag area
	RECT	m_rcRBDrag; // drag area

	RECT m_rcMLimit; // Mouse movement restriction area
	BYTE m_byCurKeys[NUMDIKEYS]; // current key state
	BYTE m_byOldKeys[NUMDIKEYS]; // previous key state
	BOOL m_bKeyPresses[NUMDIKEYS]; // Is it the moment you press the key?
	BOOL m_bKeyPresseds[NUMDIKEYS]; // Is it the moment you press and release the key?
	BOOL m_bNoKeyDown; // no keystrokes

	DWORD m_dwTickKeyPress[NUMDIKEYS];
	
public:
	void KeyboardClearInput(int iIndex = -1) // Invalidates keyboard input. The default is nullify all.
	{
		if(-1 == iIndex)
		{
			memset(m_byOldKeys, 0, sizeof(m_byOldKeys));
			memset(m_byCurKeys, 0, sizeof(m_byCurKeys));
			memset(m_bKeyPresses, 0, sizeof(m_bKeyPresses));
			memset(m_bKeyPresseds, 0, sizeof(m_bKeyPresseds));
		}
		else if(iIndex >= 0 && iIndex < NUMDIKEYS) // Invalidating specific keys.
		{
			m_byCurKeys[iIndex] = m_byOldKeys[iIndex] = m_bKeyPresses[iIndex] = m_bKeyPresseds[iIndex] = 0;
		}
	}
	BOOL IsNoKeyDown() const { return m_bNoKeyDown; }
	BOOL IsKeyDown(int iIndex) { if(iIndex < 0 || iIndex >= NUMDIKEYS) return FALSE; return m_byCurKeys[iIndex]; } // Whether the keyboard is pressed... DIK_???? defined in &quot;DInput.h&quot; See scancode..
	BOOL IsKeyPress(int iIndex) { if(iIndex < 0 || iIndex >= NUMDIKEYS) return FALSE; return m_bKeyPresses[iIndex]; } // The moment you press the keyboard... DIK_???? defined in &quot;DInput.h&quot; See scancode..
	BOOL IsKeyPressed(int iIndex) { if(iIndex < 0 || iIndex >= NUMDIKEYS) return FALSE; return m_bKeyPresseds[iIndex]; } // The moment you press and release the keyboard... DIK_???? defined in &quot;DInput.h&quot; See scancode..
	
	BOOL Init(HINSTANCE hInst, HWND hWnd, BOOL bActivateKeyboard= TRUE, BOOL bActivateMouse = TRUE, BOOL ExclusiveMouseAccess = TRUE );

	void Tick(void);
	void KeyboardFlushData();
	void RunMouseControlPanel(HWND hWnd);
	void MouseSetLimits(int x1, int y1, int x2, int y2);
	void SetActiveDevices(BOOL bKeyboard, BOOL bMouse);
	void MouseSetPos(int x, int y);

	BOOL KeyboardGetKeyState(int nDIKey) const; // Check for recently pressed keys.

	const POINT MouseGetPos() { return m_ptCurMouse; }
	const POINT MouseGetPosOld() { return m_ptOldMouse; }
	
	RECT MouseGetLBDragRect() { return m_rcLBDrag; }
	RECT MouseGetMBDragRect() { return m_rcMBDrag; }
	RECT MouseGetRBDragRect() { return m_rcRBDrag; }

	int MouseGetFlag() { return m_nMouseFlag; } // It is combined with the or operation of Mouse Flag.
	int MouseGetFlagOld() { return m_nMouseFlagOld; }
	void MouseRemoveFlag(int nFlag = -1) { if(-1 == nFlag) m_nMouseFlag = m_nMouseFlagOld = 0; else m_nMouseFlag &= (~nFlag); } // Remove specific Mouse Flags

	CLocalInput(void);
	~CLocalInput(void);
};
