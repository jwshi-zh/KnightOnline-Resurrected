#include "pch.h"
#include "Warfaremain.h"

#include "PacketDef.h"
#include "APISocket.h"
#include "PlayerMySelf.h"
#include "GameProcMain.h"
#include "GameEng.h"
#include "UIChat.h"

#include "N3WorldManager.h"

#include "N3SndMgr.h"
#include "N3UIEdit.h"

#include "../resource.h"

BOOL g_bActive = TRUE;
HHOOK ghookdata = nullptr;

/*
LRESULT CALLBACK OYBLowLevelKeyboardProc (INT nCode, WPARAM wParam, LPARAM lParam)
{    
	BOOL	bControlKeyDown = 0;    
	BOOL	bFlags;
	DWORD	vkCode;
	KBDLLHOOKSTRUCT *pkbhs;

	switch ( CGameProcedure::s_eVersion )
	{
		case W95:
		case W98:
		case WME:

			bFlags = lParam >> 29;
			bFlags	&= 0x00000001;
			vkCode = lParam >> 16;
			vkCode	&= 0x00000fff;
			switch (nCode)    
			{        
				case HC_ACTION:        
				{   
					// Check to see if the CTRL key is pressed
					bControlKeyDown = GetAsyncKeyState (VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1);                        
					// Disable CTRL+ESC
					if ((vkCode == 0x01) && bControlKeyDown)                
						return 1;            
					// Disable ALT+TAB
					if ((vkCode == 0x0F) && bFlags)                
						return 1;            
					// Disable ALT+ESC
					if ((vkCode == 0x01) && bFlags)
						return 1;			
					// Disable WINDOWS keys
					if ((vkCode == VK_LWIN) || (vkCode == VK_RWIN))
						return 1;					
						break;        
				}        
				default:            
					break;    
			}    
			break;

		case WNT4:
		case W2K:

			// By returning a non-zero value from the hook procedure, the
			// message does not get passed to the target window
			pkbhs = (KBDLLHOOKSTRUCT *) lParam;    
			switch (nCode)    
			{        
				case HC_ACTION:        
				{            
					// Check to see if the CTRL key is pressed
					bControlKeyDown = GetAsyncKeyState (VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1);                        
					// Disable CTRL+ESC
					if (pkbhs->vkCode == VK_ESCAPE && bControlKeyDown)                
						return 1;            
					// Disable ALT+TAB
					if (pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN)                
						return 1;            
					// Disable ALT+ESC
					if (pkbhs->vkCode == VK_ESCAPE && pkbhs->flags & LLKHF_ALTDOWN)			    
						return 1;			
					// Disable WINDOWS keys
					if (pkbhs-&gt;vkCode == VK_LWIN || pkbhs-&gt;vkCode == VK_RWIN)
						return 1;					
						break;        
				}        
				default:            
					break;    
			}    
			break;

	}
	return CallNextHookEx (ghookdata, nCode, wParam, lParam);
} 
*/

LRESULT CALLBACK WndProcMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	switch(message)
    {
	case WM_COMMAND:
		{
			const WORD wNotifyCode = HIWORD(wParam); // notification code
			const CN3UIEdit* pEdit = CN3UIEdit::GetFocusedEdit();
			if(EN_CHANGE == wNotifyCode && pEdit)
			{
				WORD wID = LOWORD(wParam);         // item, control, or accelerator identifier
				const HWND hwndCtl = (HWND)lParam;
				if(hwndCtl == CN3UIEdit::s_hWndEdit)
				{
					pEdit->UpdateTextFromEditCtrl();
					pEdit->UpdateCaretPosFromEditCtrl();
					CGameProcedure::SetGameCursor(CGameProcedure::s_hCursorNormal);
				}
			}
		}
		break;

	case WM_NOTIFY:
		{
			int idCtrl = (int) wParam;
			auto* pnmh = (NMHDR*) lParam; 
		}
		break;

	case WM_KEYDOWN:
		{
			const int iLangID = ::GetUserDefaultLangID();
			if(0x0404 == iLangID) // Taiwan Language
			{
				CUIChat* pUIChat = CGameProcedure::s_pProcMain->m_pUIChatDlg;
				const int iVK = (int)wParam;
				if(	pUIChat &&
					VK_ESCAPE != iVK &&
					VK_RETURN != iVK &&
					CGameProcedure::s_pProcMain &&
					CGameProcedure::s_pProcActive == CGameProcedure::s_pProcMain &&
					!pUIChat->IsChatMode())
				{
					if(!(GetKeyState(VK_CONTROL)&0x8000))
					{
						pUIChat->SetFocus();
						PostMessage(CN3UIEdit::s_hWndEdit, WM_KEYDOWN, wParam, lParam);
						return 0;
					}
				}
			}

// #ifndef _DEBUG //this_reui
			// int iVK = (int)wParam;
			// if(	VK_RETURN == iVK &&
			// CGameProcedure::s_pProcMain &&
			// CGameProcedure::s_pProcActive == CGameProcedure::s_pProcMain )
			// {
				// CUIChat* pUIChat = CGameProcedure::s_pProcMain->m_pUIChatDlg;
				// if(pUIChat) pUIChat->SetFocus();
			// }
// #endif
		}
		break;

		case WM_SOCKETMSG:
		{
			switch(WSAGETSELECTEVENT(lParam))
			{
			case FD_CONNECT:
				TRACE("Socket connected..\n");
				break;
			// case FD_ACCEPT:
				// break;
			case FD_CLOSE:
				{
					// MessageBox(::GetActiveWindow(), "socket closed", "error", MB_OK);
					if(CGameProcedure::s_bNeedReportConnectionClosed) 
						CGameProcedure::ReportServerConnectionClosed(true);
					TRACE("Socket closed..\n");
				} 
				break;
			case FD_READ:
				{
					CGameProcedure::s_pSocket->Receive();
				}
				break;
			default:
				__ASSERT(0, "WM_SOCKETMSG ");
				break;
			}
		}
		break;

		case WM_ACTIVATE:
			{
				const int iActive = LOWORD(wParam);           // activation flag
				int iMinimized = (BOOL) HIWORD(wParam); // minimized flag
				HWND hwndPrevious = (HWND) lParam;       // window handle

				switch(iActive)
				{
				case WA_CLICKACTIVE:
				case WA_ACTIVE:
#ifdef _DEBUG
					g_bActive = TRUE;
#endif
					break;
				case WA_INACTIVE:
#ifdef _DEBUG
					g_bActive = FALSE;
#endif
					if(false == CGameProcedure::s_bWindowed)
					{
						CLogWriter::Write("WA_INACTIVE.");
						PostQuitMessage(0); // Windowed mode or tingin??
					}
					break;
				}
			}
			break;

		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT:
			
			// Close the socket first.
			CGameProcedure::s_pSocket->Disconnect();
			CGameProcedure::s_pSocketSub->Disconnect();

			PostQuitMessage(0);
			break;
		case WM_MOUSEWHEEL:
			{
				if(CGameProcedure::s_pProcActive == CGameProcedure::s_pProcMain)
				{
					const float fDelta = ((short)HIWORD(wParam)) * 0.05f;
					CGameProcedure::s_pEng->CameraZoom(fDelta);
					// CGameProcedure::s_pProcMain->ControlViewVDegree((short)HIWORD(wParam));
				}
			}
			break;
	}

	// if ( CGameProcedure::s_pIME && CGameProcedure::s_pIME->IMEMessageProc(message, wParam, lParam))
	// {
		// return 0;
	// }
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK WndProcSub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	if(message == WM_SOCKETMSG)
	{
		switch(WSAGETSELECTEVENT(lParam))
		{
		case FD_CONNECT:
			break;
		case FD_ACCEPT:
			break;
		case FD_CLOSE:
			break;
		case FD_READ:
			CGameProcedure::s_pSocketSub->Receive();
			break;
		default:
			__ASSERT(0, "WM_SOCKETMSG ");
			break;
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// 
//
// 
HWND CreateMainWindow(HINSTANCE hInstance)
{
	WNDCLASS    wc;

	// only register the window class once - use hInstance as a flag.
	wc.style         = 0;
	wc.lpfnWndProc   = (WNDPROC)WndProcMain;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WARFARE));
	wc.hCursor       = nullptr;
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = nullptr;
	wc.lpszClassName = "Knight OnLine Client";

	if (0 == ::RegisterClass(&wc))
	{
		CLogWriter::Write("Cannot register window class.");
		exit(-1);
	}

	const DWORD	style = WS_POPUP | WS_CLIPCHILDREN;
	return ::CreateWindow("Knight OnLine Client", "Knight OnLine Client", style, 0, 0, CN3Base::s_Options.iViewWidth, CN3Base::s_Options.iViewHeight, NULL, NULL, hInstance, NULL);
}

HWND CreateSubWindow(HINSTANCE hInstance)
{
	WNDCLASS    wc;

	// only register the window class once - use hInstance as a flag.
	wc.style         = 0;
	wc.lpfnWndProc   = (WNDPROC)WndProcSub;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = nullptr;
	wc.hCursor       = nullptr;
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = nullptr;
	wc.lpszClassName = "Knight OnLine Sub";

	if (0 == ::RegisterClass(&wc))
	{
		CLogWriter::Write("Cannot register window class.");
		exit(-1);
	}

	const DWORD	style = WS_POPUP;
	return ::CreateWindow("Knight OnLine Sub", "Knight OnLine Sub", style, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
}

/*
DWORD WINAPI CheckSpeedHackProc( LPVOID lpParameter )
{
	float fTime, fTime2;
	while(1)
	{
		fTime = CN3Base::TimeGet();
		Sleep(1000);
		fTime2 = CN3Base::TimeGet();
		if(fTime2 - fTime > 1.1f)
		{
			int iii = 0;
			CLogWriter::Write("Speedhack detected");
			exit(-1);
			// CGameProcedure::s_bUseSpeedHack = true;
		}
	}
}
*/

const int MAX_GAME_PROCEDURE = 8;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{


	// 
	// For speed hack checking...
	// DWORD dwCSHID;
	// HANDLE hThreadCheckSpeedHack = ::CreateThread(NULL, 0, CheckSpeedHackProc, NULL, CREATE_SUSPENDED, &dwCSHID);
	// ::SetThreadPriority(hThreadCheckSpeedHack, THREAD_PRIORITY_NORMAL);
	// For speed hack checking...
	// 



	
	char szPath[_MAX_PATH] = "";
	GetCurrentDirectory(_MAX_PATH, szPath);
	CN3Base::PathSet(szPath);

	// Read settings...
	char szIniPath[_MAX_PATH] = "";
	lstrcpy(szIniPath, CN3Base::PathGet().c_str());
	lstrcat(szIniPath, "Option.Ini");

	CN3Base::s_Options.iTexLOD_Chr = GetPrivateProfileInt("Texture", "LOD_Chr", 0, szIniPath);
	if(CN3Base::s_Options.iTexLOD_Chr < 0) CN3Base::s_Options.iTexLOD_Chr = 0;
	if(CN3Base::s_Options.iTexLOD_Chr >= 2) CN3Base::s_Options.iTexLOD_Chr = 1;
	
	CN3Base::s_Options.iTexLOD_Shape = GetPrivateProfileInt("Texture", "LOD_Shape", 0, szIniPath);
	if(CN3Base::s_Options.iTexLOD_Shape < 0) CN3Base::s_Options.iTexLOD_Shape = 0;
	if(CN3Base::s_Options.iTexLOD_Shape >= 2) CN3Base::s_Options.iTexLOD_Shape = 1;

	CN3Base::s_Options.iTexLOD_Terrain = GetPrivateProfileInt("Texture", "LOD_Terrain", 0, szIniPath);
	if(CN3Base::s_Options.iTexLOD_Terrain < 0) CN3Base::s_Options.iTexLOD_Terrain = 0;
	if(CN3Base::s_Options.iTexLOD_Terrain >= 2) CN3Base::s_Options.iTexLOD_Terrain = 1;

	CN3Base::s_Options.iUseShadow = GetPrivateProfileInt("Shadow", "Use", 1, szIniPath);
	
	CN3Base::s_Options.iViewWidth = GetPrivateProfileInt("ViewPort", "Width", 1024, szIniPath);
	CN3Base::s_Options.iViewHeight = GetPrivateProfileInt("ViewPort", "Height", 768, szIniPath);
	if(1024 == CN3Base::s_Options.iViewWidth) CN3Base::s_Options.iViewHeight = 768;
	else if(1280 == CN3Base::s_Options.iViewWidth) CN3Base::s_Options.iViewHeight = 1024;
	else if(1600 == CN3Base::s_Options.iViewWidth) CN3Base::s_Options.iViewHeight = 1200;
	else 
	{
		CN3Base::s_Options.iViewWidth = 1024;
		CN3Base::s_Options.iViewHeight = 768;
	}

	CN3Base::s_Options.iViewColorDepth = GetPrivateProfileInt("ViewPort", "ColorDepth", 16, szIniPath);
	if(CN3Base::s_Options.iViewColorDepth != 16 && CN3Base::s_Options.iViewColorDepth != 32)
		CN3Base::s_Options.iViewColorDepth = 16;
	CN3Base::s_Options.iViewDist = GetPrivateProfileInt("ViewPort", "Distance", 512, szIniPath);
	if(CN3Base::s_Options.iViewDist < 256) CN3Base::s_Options.iViewDist = 256;
	if(CN3Base::s_Options.iViewDist > 512) CN3Base::s_Options.iViewDist = 512;

	CN3Base::s_Options.iEffectSndDist = GetPrivateProfileInt("Sound", "Distance", 48, szIniPath);
	if(CN3Base::s_Options.iEffectSndDist < 20) CN3Base::s_Options.iEffectSndDist = 20;
	if(CN3Base::s_Options.iEffectSndDist > 48) CN3Base::s_Options.iEffectSndDist = 48;

	const int iSndEnable = GetPrivateProfileInt("Sound", "Enable", 1, szIniPath);
	CN3Base::s_Options.bSndEnable = (iSndEnable) ? true : false; // sound...

	const int iSndDuplicate = GetPrivateProfileInt("Sound", "Duplicate", 0, szIniPath);
	CN3Base::s_Options.bSndDuplicated = (iSndDuplicate) ? true : false; // sound...

	const int iWindowCursor = GetPrivateProfileInt("Cursor", "WindowCursor", 1, szIniPath);
	CN3Base::s_Options.bWindowCursor = (iWindowCursor) ? true : false; // cursor...

	// Creating a subwindow to be used as the second socket..
	const HWND hWndSub = CreateSubWindow(hInstance);
	::ShowWindow(hWndSub, SW_HIDE); // hide...
	
	// Create the main window...
	const HWND hWndMain = CreateMainWindow(hInstance);
	if(nullptr == hWndMain || nullptr == hWndSub)
	{
		CLogWriter::Write("Cannot create window.");
		exit(-1);
	}
    ::ShowWindow(hWndMain, nCmdShow); // show...
	::SetActiveWindow(hWndMain);

	// Launcher upgrade...
	FILE* pFile = fopen("Launcher2.exe", "r"); // I have something to upgrade..
	if(pFile)
	{
		fclose(pFile);
		if(::DeleteFile("Launcher.exe")) // Erase the original...
		{
			::rename("Launcher2.exe", "Launcher.exe"); // change name..
		}
	}

	// Handling program arguments..
	if(lpCmdLine && lstrlen(lpCmdLine) > 0 && lstrlen(lpCmdLine) < 64) // When something comes in...
	{
		char szService[64], szAccountTmp[64], szPWTmp[64];
		sscanf(lpCmdLine, "%s %s %s", szService, szAccountTmp, szPWTmp);

		if(0 == lstrcmpi(szService, "MGame")) // Log in to your Mgame account...
			CGameProcedure::s_eLogInClassification = LIC_MGAME;
		else if(0 == lstrcmpi(szService, "Daum")) // Log in to the following account...
			CGameProcedure::s_eLogInClassification = LIC_DAUM;
		else 
			CGameProcedure::s_eLogInClassification = LIC_KNIGHTONLINE;
		CGameProcedure::s_szAccount = szAccountTmp; // account
		CGameProcedure::s_szPassWord = szPWTmp; // off duty.

		if(0 == lstrcmpi(szService, "$#$%&^@!#$%#@^%&#%$&^운영팀전용게임")) // A game dedicated to the management team...
			CGameProcedure::s_bWindowed = true;
		else
			CGameProcedure::s_bWindowed = false;
	}

	// 
	// Create Static Member...
	CGameProcedure::StaticMemberInit(hInstance, hWndMain, hWndSub);		// Destruction is done in WM_DESTROY.
	CGameProcedure::ProcActiveSet((CGameProcedure*)CGameProcedure::s_pProcLogIn);	// Start with the login procedure..

	// and its installation, called in InitInstance()
	/*	switch ( CGameProcedure::s_eVersion )
	{
		case W95:
		case W98:
		case WME:
			// ghookdata = SetWindowsHookEx(WH_KEYBOARD, OYBLowLevelKeyboardProc, AfxGetInstanceHandle(), 0);
			break;
		case WNT4:
		case W2K:
			// ghookdata = SetWindowsHookEx(WH_KEYBOARD_LL, OYBLowLevelKeyboardProc, AfxGetInstanceHandle(), 0);
			break;
	}
*/
    BOOL bGotMsg = FALSE;

#if _DEBUG
	const HDC hDC = GetDC(hWndMain);
#endif // #if _DEBUG

    MSG msg; memset(&msg, 0, sizeof(MSG));
	while( WM_QUIT != msg.message  )
    {
        // Use PeekMessage() if the app is active, so we can use idle time to
        // render the scene. Else, use GetMessage() to avoid eating CPU time.
		if( g_bActive )
			bGotMsg = PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE );
		else
			bGotMsg = GetMessage( &msg, nullptr, 0U, 0U );

		if( bGotMsg )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Render a frame during idle time (no messages are waiting)
			if( g_bActive)
			{
				CGameProcedure::TickActive();
				CGameProcedure::RenderActive();
#if _DEBUG
				static float fTimePrev = CN3Base::TimeGet();
				static char szDebugs[4][256] = { "", "", "", "" };
				const float fTime = CN3Base::TimeGet();
				if(fTime > fTimePrev + 0.5f)
				{
					fTimePrev = fTime;

					sprintf(szDebugs[0], "지형 : 보통(%d) 타일(%d) || Object : 갯수(%d) 부분수(%d) 폴리곤(%d)",
						CN3Base::s_RenderInfo.nTerrain_Polygon,
						CN3Base::s_RenderInfo.nTerrain_Tile_Polygon,
						CN3Base::s_RenderInfo.nShape,
						CN3Base::s_RenderInfo.nShape_Part,
						CN3Base::s_RenderInfo.nShape_Polygon);
					
					sprintf(szDebugs[1], "캐릭터 : 갯수(%d), 파트수(%d), 폴리곤(%d), 무기(%d), 무기폴리곤(%d)", 
						CN3Base::s_RenderInfo.nChr,
						CN3Base::s_RenderInfo.nChr_Part,
						CN3Base::s_RenderInfo.nChr_Polygon,
						CN3Base::s_RenderInfo.nChr_Plug,
						CN3Base::s_RenderInfo.nChr_Plug_Polygon);

					sprintf(szDebugs[2], "Camera : Lens(%.1f) NearPlane(%.1f) FarPlane(%.1f)",
						D3DXToDegree(CN3Base::s_CameraData.fFOV),
						CN3Base::s_CameraData.fNP,
						CN3Base::s_CameraData.fFP );

					if(CGameProcedure::s_pProcMain && CGameBase::ACT_WORLD && CGameBase::ACT_WORLD->GetSkyRef())
					{
						int iYear = 0, iMonth = 0, iDay = 0, iH = 0, iM = 0;
						CGameBase::ACT_WORLD->GetSkyRef()->GetGameTime(&iYear, &iMonth, &iDay, &iH, &iM);
						sprintf(szDebugs[3], "%.2f Frm/Sec, %d년%d월%d일 %d시%d분", CN3Base::s_fFrmPerSec, iYear, iMonth, iDay, iH, iM);
					}
					else szDebugs[3][0] = NULL;
				}

				for(int i = 0; i < 4; i++)
					if(szDebugs[i])
						TextOut(hDC, 0, i*18, szDebugs[i], lstrlen(szDebugs[i])); // Display rendering information on screen..
#endif // #if _DEBUG


				// #ifndef _DEBUG
				// static HDC hDC = GetDC(hWndMain);
				// static char szDebug[256] = "";
				// static float fTimePrev = CN3Base::TimeGet();
				// float fTime = CN3Base::TimeGet();
				// if(fTime > fTimePrev + 0.5f)
				// {
					// sprintf(szDebug, "%f", CN3Base::s_fFrmPerSec);
					// fTimePrev = fTime;
				// }
				// TextOut(hDC, 0, 0, szDebug, lstrlen(szDebug)); // Display rendering information on the screen..
				// #endif
            }
		}
    }

#if _DEBUG
	ReleaseDC(hWndMain, hDC);
#endif // #if _DEBUG



	// 
	// For speed hack checking...
	// ::TerminateThread(hThreadCheckSpeedHack, 0);
	// hThreadCheckSpeedHack = 0;
	// dwCSHID = 0;
	// For speed hack checking...
	// 




	CGameProcedure::StaticMemberRelease(); // unlock all......

	return msg.wParam;
}
