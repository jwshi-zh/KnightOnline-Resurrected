#include "pch.h"
#include "LocalInput.h"
#include "mmsystem.h"

static LPDIRECTINPUTDEVICE lpDID = nullptr;

CLocalInput::CLocalInput(void)
{
	m_lpDI = nullptr;
	m_lpDIDKeyboard = nullptr;
	// m_lpDIDMouse = NULL;

	m_hWnd = nullptr;

	// m_bMouse = FALSE;
	// m_bKeyboard = FALSE;

	m_nMouseFlag = 0;
	m_nMouseFlagOld = 0; // Mouse button down flag
	m_dwTickLBDown = 0; // For detecting left mouse button double click
	m_dwTickRBDown = 0; // For right mouse button double click detection

	m_ptCurMouse.x = m_ptCurMouse.y = 0; // current mouse pointer
	m_ptOldMouse.x = m_ptOldMouse.y = 0; // previous mouse pointer

	SetRect(&m_rcLBDrag, 0, 0, 0, 0); // drag area
	SetRect(&m_rcMBDrag, 0, 0, 0, 0); // drag area
	SetRect(&m_rcRBDrag, 0, 0, 0, 0); // drag area

	SetRect(&m_rcMLimit, 0, 0, 0, 0); // Mouse movement restriction area

	memset(m_byCurKeys, 0, sizeof(m_byCurKeys)); // current key state
	memset(m_byOldKeys, 0, sizeof(m_byOldKeys)); // previous key state
	memset(m_bKeyPresses, 0, sizeof(m_bKeyPresses)); // Is it the moment you press the key?
	memset(m_bKeyPresseds, 0, sizeof(m_bKeyPresseds)); // Is it the moment you press and release the key?
	m_bNoKeyDown = FALSE; // no keystrokes
	
	memset(m_dwTickKeyPress, 0, sizeof(m_dwTickKeyPress));
}	


// 
// Default destructor. Releases all devices that accquired
// 
CLocalInput::~CLocalInput(void)
{
	// shutdown keyboard
	if (m_lpDIDKeyboard)
	{
		UnacquireKeyboard();
		m_lpDIDKeyboard->Release();
		m_lpDIDKeyboard = nullptr;
	}
	
	// shutdown mouse
	// if (m_lpDIDMouse)
	// {
		// UnacquireMouse();
		// m_lpDIDMouse->Release();
		// m_lpDIDMouse = NULL;
	// }
	
	// kill directinput
	if(m_lpDI)
	{
		m_lpDI->Release();
		m_lpDI = nullptr;
	}
}

// 
// Try to accquire all devices. Use SetActiveDevices() if you do not want
// some devices.
// 
BOOL CLocalInput::Init(HINSTANCE hInst, HWND hWnd, BOOL bActivateKeyboard, BOOL bActivateMouse, BOOL ExclusiveMouseAccess)
{
	HRESULT rval;

	m_hWnd = hWnd; // Remember the window handle.

	rval = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)(&m_lpDI), nullptr);
	if(rval != DI_OK) return FALSE;

	// Create the keyboard device
	rval = m_lpDI->CreateDevice(GUID_SysKeyboard, &m_lpDIDKeyboard, nullptr);
	if(rval == DI_OK)
	{
		m_lpDIDKeyboard->SetDataFormat(&c_dfDIKeyboard);
		m_lpDIDKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY);

		AcquireKeyboard();
	}

	// Create the mouse device
	/*	rval = m_lpDI->CreateDevice(GUID_SysMouse, &m_lpDIDMouse, NULL);
	if ( rval == THE_OK )
	{
		m_lpDIDMouse->SetDataFormat(&c_dfDIMouse);
		if( ExclusiveMouseAccess == FALSE )
			m_lpDIDMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		else
			m_lpDIDMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

		AcquireMouse();
	}*/

	return TRUE;
}

// 
// Controls which devices you have accquired.
// 
void CLocalInput::SetActiveDevices(BOOL bKeyboard, BOOL bMouse)
{
	if (bKeyboard)
		AcquireKeyboard();
	else 
		UnacquireKeyboard();
}

// 
// Runs the DirectInput control panel associated with the mouse. If the device does not
// have a control panel associated with it, the default device control panel is launched.
// 
void CLocalInput::RunMouseControlPanel(HWND hWnd)
{
// m_lpDIDMouse->RunControlPanel(hWnd, 0);
}

// 
// Wipes out the internal key data.
// 
void CLocalInput::KeyboardFlushData()
{
	memset(m_byOldKeys, 0, NUMDIKEYS);
	memset(m_byCurKeys, 0, NUMDIKEYS);
}

// 
// This restricts the mouse to a defined area.
// 
void CLocalInput::MouseSetLimits(int x1, int y1, int x2, int y2)
{
	m_rcMLimit.left = x1;
	m_rcMLimit.top = y1;
	m_rcMLimit.right = x2;
	m_rcMLimit.bottom = y2;
}

// 
// Sets the mouse position. This restricts the position to the physical display.
// 
void CLocalInput::MouseSetPos(int x, int y)
{
	// clamp non-free mouse values to limits
	if ((m_ptCurMouse.x = x) >= m_rcMLimit.right)
		m_ptCurMouse.x = m_rcMLimit.right-1;

	if ((m_ptCurMouse.y = y) >= m_rcMLimit.bottom)
		m_ptCurMouse.y = m_rcMLimit.bottom-1;

	if ((m_ptCurMouse.x = x) <= m_rcMLimit.left)
		m_ptCurMouse.x = m_rcMLimit.left+1;

	if ((m_ptCurMouse.y = y) <= m_rcMLimit.top)
		m_ptCurMouse.y = m_rcMLimit.top+1;
}

// 
// Retrieves the keystate of a given key.
// 
BOOL CLocalInput::KeyboardGetKeyState(int nDIKey) const
{
	if(nDIKey < 0 || nDIKey >= NUMDIKEYS) return FALSE;
	return(m_byCurKeys[nDIKey]);
}

// 
// Obtains access to the keyboard.
// 
void CLocalInput::AcquireKeyboard()
{
	if (m_lpDIDKeyboard)
	{
		const HRESULT rval = m_lpDIDKeyboard->Acquire();
		// if(rval != DI_OK) MessageBox(::GetActiveWindow(), "Acquire Keyboard Failed.", "DirectInput", MB_OK);
		if (rval == DI_OK || rval == S_FALSE)
		{
			// m_bKeyboard = TRUE;
			KeyboardFlushData();
			return;
		}
	}	
	// m_bKeyboard = FALSE;
}

// 
// Releases access to the keyboard.
// 
void CLocalInput::UnacquireKeyboard()
{
	KeyboardFlushData();
	// m_bKeyboard = FALSE;
	if (m_lpDIDKeyboard)
	{
		HRESULT rval = m_lpDIDKeyboard->Unacquire();
		// if(rval != DI_OK) MessageBox(::GetActiveWindow(), "UnAcquire Keyboard Failed.", "DirectInput", MB_OK);
	}
}

// 
// Obtains access to the mouse.
// 
void CLocalInput::AcquireMouse()
{
	// if (m_lpDIDMouse)
	// {
		// HRESULT rval = m_lpDIDMouse->Acquire();
		// if ( rval == THE_OK || rval == S_FALSE)
		// {
			// return;
		// }
	// }
}

// 
// Releases access to the mouse.
// 
void CLocalInput::UnacquireMouse()
{
	// m_bMouse = FALSE;
	// if (m_lpDIDMouse)
	// {
		// HRESULT rval = m_lpDIDMouse->Unacquire();
	// }
}

// 
// Updates all devices. Call this before you check for input.
// 
// If possible, it is good to run the whole procedure only once. Doing it several times can lead to confusion.
void CLocalInput::Tick(void)
{
	HRESULT err;
	// int oldx, done;
	// BOOL isPressed;
	// WORD i;
	// DWORD key;

	const HWND hWndActive = ::GetActiveWindow(); // Only when focused...
	if(hWndActive != m_hWnd) return;

	// 
	// KEYBOARD
	// 
	// if(m_bKeyboard)
	// {
		memcpy(m_byOldKeys, m_byCurKeys, NUMDIKEYS); // record of previous key state
		err = m_lpDIDKeyboard->GetDeviceState(NUMDIKEYS, m_byCurKeys); // Record current key state
		if(err != DI_OK) AcquireKeyboard();
		else
		{
			m_bNoKeyDown = TRUE; // I didn't press anything for the first time
			for(int i = 0; i < NUMDIKEYS; i++)
			{
				if(!m_byOldKeys[i] && m_byCurKeys[i]) m_bKeyPresses[i] = TRUE; // the moment it is pressed
				else m_bKeyPresses[i] = FALSE;
				
				if(m_byOldKeys[i] && !m_byCurKeys[i]) m_bKeyPresseds[i] = TRUE; // The moment you press and release...
				else m_bKeyPresseds[i] = FALSE;
				
				if(m_byCurKeys[i]) m_bNoKeyDown = FALSE;
			}
		}
	// }

	// 
	// MOUSE
	// 
	// if(m_bMouse)
	// {
		// DIMOUSESTATE DIMState;
		// err = m_lpDIDMouse->GetDeviceState(sizeof(DIMState), &DIMState); // Get the mouse status - It's strange.. I keep getting an error..
		// if(err != DI_OK)
		// {
		// AcquireMouse();
		// return;
		// }

		// Get new mouse position
		// m_ptCurMouse.x += DIMState.lX;
		// m_ptCurMouse.y += DIMState.lY;

		// if((DIMState.rgbButtons[0] & 0x80)) m_bLBDown = TRUE;
		// else m_bLBDown = FALSE;
		// if((DIMState.rgbButtons[1] & 0x80)) m_bRBDown = TRUE;
		// else m_bRBDown = FALSE;

		// clamp non-free mouse values to limits
		// if (m_ptCurMouse.x >= m_rcMLimit.right)
		// m_ptCurMouse.x = m_rcMLimit.right-1;
		//
		// if (m_ptCurMouse.y >= m_rcMLimit.bottom)
		// m_ptCurMouse.y = m_rcMLimit.bottom-1;
		//
		// if (m_ptCurMouse.x < m_rcMLimit.left)
		// m_ptCurMouse.x = m_rcMLimit.left;
		//
		// if (m_ptCurMouse.y < m_rcMLimit.top)
		// m_ptCurMouse.y = m_rcMLimit.top;

		m_ptOldMouse = m_ptCurMouse; // Copy the previous one...

		RECT rcClient;
		::GetClientRect(m_hWnd, &rcClient);
		::GetCursorPos(&m_ptCurMouse); // It's kind of weird... just get the system mouse cursor position
		::ScreenToClient(m_hWnd, &m_ptCurMouse); // Convert to client area

		if(PtInRect(&rcClient, m_ptCurMouse) == FALSE) // || GetFocus() != m_hWnd) // If out of screen area or not in focus...
		{
		}
		else
		{
			// Preserve mouse button state.
			m_nMouseFlagOld = m_nMouseFlag;
			m_nMouseFlag = 0;
			
			// Get mouse state
			if(_IsKeyDown(VK_LBUTTON)) m_nMouseFlag |= MOUSE_LBDOWN;
			if(_IsKeyDown(VK_MBUTTON)) m_nMouseFlag |= MOUSE_MBDOWN;
			if(_IsKeyDown(VK_RBUTTON)) m_nMouseFlag |= MOUSE_RBDOWN;

			// Right after clicking the button...
			if((m_nMouseFlagOld & MOUSE_LBDOWN) == FALSE && (m_nMouseFlag & MOUSE_LBDOWN)) m_nMouseFlag |= MOUSE_LBCLICK;
			if((m_nMouseFlagOld & MOUSE_MBDOWN) == FALSE && (m_nMouseFlag & MOUSE_MBDOWN)) m_nMouseFlag |= MOUSE_MBCLICK;
			if((m_nMouseFlagOld & MOUSE_RBDOWN) == FALSE && (m_nMouseFlag & MOUSE_RBDOWN)) m_nMouseFlag |= MOUSE_RBCLICK;
			
			// when you release the button
			if((m_nMouseFlagOld & MOUSE_LBDOWN) && (m_nMouseFlag & MOUSE_LBDOWN) == FALSE) m_nMouseFlag |= MOUSE_LBCLICKED;
			if((m_nMouseFlagOld & MOUSE_MBDOWN) && (m_nMouseFlag & MOUSE_MBDOWN) == FALSE) m_nMouseFlag |= MOUSE_MBCLICKED;
			if((m_nMouseFlagOld & MOUSE_RBDOWN) && (m_nMouseFlag & MOUSE_RBDOWN) == FALSE) m_nMouseFlag |= MOUSE_RBCLICKED;
			
			static DWORD dwDblClk = GetDoubleClickTime(); // Get the window's double-click time.
			if(m_nMouseFlag & MOUSE_LBCLICKED) // Detect left double click
			{
				static DWORD dwCLicked = 0;
				if(timeGetTime() < dwCLicked + dwDblClk)
					m_nMouseFlag |= MOUSE_LBDBLCLK;
				dwCLicked = timeGetTime();
			}
			if(m_nMouseFlag & MOUSE_MBCLICKED) // Detect left double click
			{
				static DWORD dwCLicked = 0;
				if(timeGetTime() < dwCLicked + dwDblClk)
					m_nMouseFlag |= MOUSE_MBDBLCLK;
				dwCLicked = timeGetTime();
			}
			if(m_nMouseFlag & MOUSE_RBCLICKED) // Detect left double click
			{
				static DWORD dwCLicked = 0;
				if(timeGetTime() < dwCLicked + dwDblClk)
					m_nMouseFlag |= MOUSE_RBDBLCLK;
				dwCLicked = timeGetTime();
			}

			// Drag region handling
			if(m_nMouseFlag & MOUSE_LBDOWN) { m_rcLBDrag.right = m_ptCurMouse.x; m_rcLBDrag.bottom = m_ptCurMouse.y; }
			if(m_nMouseFlag & MOUSE_MBDOWN) { m_rcMBDrag.right = m_ptCurMouse.x; m_rcMBDrag.bottom = m_ptCurMouse.y; }
			if(m_nMouseFlag & MOUSE_RBDOWN) { m_rcRBDrag.right = m_ptCurMouse.x; m_rcRBDrag.bottom = m_ptCurMouse.y; }
			if(m_nMouseFlag & MOUSE_LBCLICK) { m_rcLBDrag.left = m_ptCurMouse.x; m_rcLBDrag.top = m_ptCurMouse.y; }
			if(m_nMouseFlag & MOUSE_MBCLICK) { m_rcMBDrag.left = m_ptCurMouse.x; m_rcMBDrag.top = m_ptCurMouse.y; }
			if(m_nMouseFlag & MOUSE_RBCLICK) { m_rcRBDrag.left = m_ptCurMouse.x; m_rcRBDrag.top = m_ptCurMouse.y; }
		}
	// } // end of mouse
}
