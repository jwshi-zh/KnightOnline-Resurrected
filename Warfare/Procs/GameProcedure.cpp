#include "pch.h"

#include "GameDef.h"
#include "GameEng.h"
#include "packetdef.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "UIMessageBox.h"
#include "UIMessageBoxManager.h"
#include "UIManager.h"
#include "..\resource.h"

#include "N3FXMgr.h"
#include "PlayerMyself.h"
#include "GameProcedure.h"
#include "GameProcLogIn.h"
#include "GameProcNationSelect.h"
#include "GameProcCharacterCreate.h"
#include "GameProcCharacterSelect.h"
#include "GameProcMain.h"
#include "GameProcOption.h"

#include "UILoading.h"
#include "UINotice.h"
#include "UIHelp.h"
#include "UIHotKeyDlg.h"
#include "UIChat.h"
#include "UIVarious.h"
#include "UIPartyOrForce.h"
#include "UIMessageWnd.h"
#include "UIEndingDisplay.h"

#include "N3UIEdit.h"
#include "N3SndObjStream.h"
#include "N3TableBase.h"
#include "N3FXBundle.h"
#include "BitmapFile.h"
#include "MagicSkillMng.h"
#include "GameCursor.h"

CN3SndObjStream*	CGameProcedure::s_pSnd_BGM = nullptr;			// Main background music pointer..
CLocalInput*		CGameProcedure::s_pLocalInput = nullptr;		// Mouse and keyboard input object .. I wrote Direct Input.
CAPISocket*			CGameProcedure::s_pSocket = nullptr;			// main socket object
CAPISocket*			CGameProcedure::s_pSocketSub = nullptr;		// subsocket object
CGameEng*			CGameProcedure::s_pEng = nullptr;				// 3D Wrapper Engine
CN3FXMgr*			CGameProcedure::s_pFX = nullptr;

CUIManager*			CGameProcedure::s_pUIMgr = nullptr;			// UI Manager
CUILoading*			CGameProcedure::s_pUILoading = nullptr;		// loading bar...
CUIMessageBoxManager*	CGameProcedure::s_pMsgBoxMgr = nullptr;		// MessageBox Manager
// bool				CGameProcedure::s_bUseSpeedHack = false;

CGameProcedure*				CGameProcedure::s_pProcPrev = nullptr;
CGameProcedure*				CGameProcedure::s_pProcActive = nullptr;

CGameProcLogIn*				CGameProcedure::s_pProcLogIn = nullptr;
CGameProcNationSelect*		CGameProcedure::s_pProcNationSelect = nullptr;
CGameProcCharacterCreate*	CGameProcedure::s_pProcCharacterCreate = nullptr;
CGameProcCharacterSelect*	CGameProcedure::s_pProcCharacterSelect = nullptr;
CGameProcMain*				CGameProcedure::s_pProcMain = nullptr;
CGameProcOption*			CGameProcedure::s_pProcOption = nullptr;
CGameCursor*				CGameProcedure::s_pGameCursor = nullptr;

HCURSOR	CGameProcedure::s_hCursorNormal = nullptr;
HCURSOR	CGameProcedure::s_hCursorNormal1 = nullptr;
HCURSOR	CGameProcedure::s_hCursorClick = nullptr;
HCURSOR	CGameProcedure::s_hCursorClick1 = nullptr;
HCURSOR	CGameProcedure::s_hCursorAttack = nullptr;
HCURSOR	CGameProcedure::s_hCursorPreRepair = nullptr;
HCURSOR	CGameProcedure::s_hCursorNowRepair = nullptr;

e_LogInClassification CGameProcedure::s_eLogInClassification; // Accessed services.. MGame, Daum, KnightOnLine ....
std::string	CGameProcedure::s_szAccount = ""; // account string..
std::string	CGameProcedure::s_szPassWord = ""; // account password..
std::string	CGameProcedure::s_szServer = ""; // server string...
bool CGameProcedure::m_bCursorLocked = false;
HCURSOR	CGameProcedure::m_hPrevGameCursor = nullptr;
HWND CGameProcedure::s_hWndSubSocket = nullptr; // Window handle for subsockets..
int	CGameProcedure::s_iChrSelectIndex = 0;
bool CGameProcedure::s_bNeedReportConnectionClosed = false; // Should I report that the server is disconnected?
bool CGameProcedure::s_bWindowed = false; // Run windowed mode??
bool CGameProcedure::s_bKeyPress = false;	// If a key has been manipulated in the UI when it is pressed
bool CGameProcedure::s_bKeyPressed = false;	// If the key has been manipulated in the ui when the key is raised

CGameProcedure::CGameProcedure()
{
	m_bCursorLocked = false;
}

CGameProcedure::~CGameProcedure()
{
	m_bCursorLocked = false;
}

void CGameProcedure::Release()
{
	s_pUIMgr->SetFocusedUI(nullptr);
}

void CGameProcedure::Init()
{
	s_pUIMgr->SetFocusedUI(nullptr);
}

void CGameProcedure::StaticMemberInit(HINSTANCE hInstance, HWND hWndMain, HWND hWndSub)
{
#if _DEBUG 
	s_bWindowed = true;
#endif // #if _DEBUG

#ifndef _DEBUG
	// In the case of the Taiwanese version, it uses a fake full mode due to IME problems.
	int iLangID = ::GetUserDefaultLangID();
	if(0x0404 == iLangID) 
	{
		s_bWindowed = true;
		
		DEVMODE dm;
		dm.dmSize = sizeof(DEVMODE);
		dm.dmPelsWidth = CN3Base::s_Options.iViewWidth;
		dm.dmPelsHeight = CN3Base::s_Options.iViewHeight;
		dm.dmBitsPerPel = CN3Base::s_Options.iViewColorDepth;
		dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		::ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	}
#endif // #ifndef _DEBUG


	RECT rc;
	::GetClientRect(hWndMain, &rc);
	s_pEng = new CGameEng();
	if(false == s_pEng->Init(s_bWindowed, hWndMain, CN3Base::s_Options.iViewWidth, CN3Base::s_Options.iViewHeight, CN3Base::s_Options.iViewColorDepth, TRUE)) exit(-1);
	// Creating a basic 3D engine for the game..
	::SetFocus(hWndMain); // Set focus this window..
	
	RECT rcTmp = rc; rcTmp.left = (rc.right - rc.left) / 2; rcTmp.bottom = rcTmp.top + 30;
	CN3UIEdit::CreateEditWindow(hWndMain, rcTmp);
	// 

	s_hWndSubSocket = hWndSub; // Window handle for subsockets..

	CGameBase::StaticMemberInit(); // Table and terrain, object, character initialization...

	// 
	// Game Procedure socket and local input, 3D engine, resource table loading and initialization...
	s_pSocket = new CAPISocket();
	s_pSocketSub = new CAPISocket();
	
    // Create a cursor...
	s_hCursorNormal =		LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_NORMAL));
	s_hCursorNormal1 =		LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_NORMAL1));
	s_hCursorClick =		LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_CLICK));
	s_hCursorClick1 =		LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_CLICK1));
	s_hCursorAttack =		LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_ATTACK));
	s_hCursorPreRepair =	LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_PRE_REPAIR));
	s_hCursorNowRepair =	LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_NOW_REPAIR));

	if(!CN3Base::s_Options.bWindowCursor)
	{
		s_pGameCursor = new CGameCursor();
		s_pGameCursor->LoadFromFile("ui\\cursor.uif");
	}
	SetGameCursor(s_hCursorNormal);

	s_pLocalInput = new CLocalInput();
	s_pLocalInput->Init(hInstance, hWndMain, FALSE); // Initialize input only.

	// 
	// Sound initialization..
	if(CN3Base::s_Options.bSndEnable)
	{
		CN3Base::s_SndMgr.Init(hWndMain);
		CN3Base::s_SndMgr.SetDuplicated(CN3Base::s_Options.bSndDuplicated);
	}
	CN3FXBundle::SetEffectSndDistance(float(CN3Base::s_Options.iEffectSndDist));

	s_pFX = new CN3FXMgr();

	const __TABLE_UI_RESRC* pTblUI = s_pTbl_UI->Find(NATION_ELMORAD); // The default is Elmorad UI.
	__ASSERT(pTblUI, "기본 UI 가 없습니다.");

	s_pUIMgr = new CUIManager(); // Default UIManager
	s_pMsgBoxMgr = new CUIMessageBoxManager(); // MessageBox Manager

	// tooltip...
	CN3UIBase::EnableTooltip(pTblUI->szToolTip);

	// 
	// Create each procedure
	s_pProcLogIn			= new CGameProcLogIn();				// login procedure
	s_pProcNationSelect		= new CGameProcNationSelect();		// country selection
	s_pProcCharacterSelect	= new CGameProcCharacterSelect();	// Character selection
	s_pProcCharacterCreate	= new CGameProcCharacterCreate();	// character creation
	s_pProcMain				= new CGameProcMain();				// main game procedure
	s_pProcOption			= new CGameProcOption();			// game options procedure
}

void CGameProcedure::StaticMemberRelease()
{
#ifndef _DEBUG
	// In the case of the Taiwanese version, it uses a fake full mode due to IME problems.
	int iLangID = ::GetUserDefaultLangID();
	if(0x0404 == iLangID) 
	{
		DEVMODE dm;
		::EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &dm);

		// If the display mode of the registry and the display mode of the current game are different, it is restored.
		if(	dm.dmPelsWidth != CN3Base::s_Options.iViewWidth || 
			dm.dmPelsHeight != CN3Base::s_Options.iViewHeight || 
			dm.dmBitsPerPel != CN3Base::s_Options.iViewColorDepth)
		{
			dm.dmSize = sizeof(DEVMODE);
			dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			::ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
		}
	}
#endif // #ifndef _DEBUG


	delete s_pSocket; s_pSocket = nullptr; // Losing communication...
	delete s_pSocketSub; s_pSocketSub = nullptr; // Remove sub sockets..
	delete s_pFX; s_pFX = nullptr;

	// 
	// Write default...
	if(s_pPlayer)
	{
		int iRun = s_pPlayer->IsRunning(); // If the movement mode was running
		CGameProcedure::RegPutSetting("UserRun", &iRun, 4); // Record walking and running status..
	}
	
	if(s_pEng)
	{
		e_ViewPoint eVP = s_pEng->ViewPoint();
		CGameProcedure::RegPutSetting("CameraMode", &eVP, 4); // record camera status
	}
	// Write default...
	// 

	// 
	// Show ending screen.
	if(s_pPlayer)
	{
		const e_Nation eNation = s_pPlayer->m_InfoBase.eNation;
		const __TABLE_UI_RESRC* pTbl = s_pTbl_UI->Find(eNation);
		if(pTbl)
		{
			CUIEndingDisplay Credit; // Show ending...
			Credit.LoadFromFile(pTbl->szEndingDisplay);
			Credit.Render();
		}
	}
	// Show ending screen.
	// 

	// if ( (s_pProcMain) && (s_pProcMain->m_pUIHotKeyDlg) )
	// s_pProcMain->m_pUIHotKeyDlg->CloseIconRegistry();

	// Stores information such as UI location and visibility.
	if(s_pProcMain)
	{
		UIPostData_Write(UI_POST_WND_CHAT, s_pProcMain->m_pUIChatDlg);
		UIPostData_Write(UI_POST_WND_HOTKEY, s_pProcMain->m_pUIHotKeyDlg);
		UIPostData_Write(UI_POST_WND_HELP, s_pProcMain->m_pUIHelp);
		// UIPostData_Write(UI_POST_WND_PARTY, s_pProcMain->m_pUIPartyOrForce);
	}

	// each procedure
	delete s_pProcLogIn; s_pProcLogIn = nullptr; 						// login procedure
	delete s_pProcNationSelect; s_pProcNationSelect = nullptr; 		// country selection
	delete s_pProcCharacterSelect; s_pProcCharacterSelect = nullptr; 	// Character selection
	delete s_pProcCharacterCreate; s_pProcCharacterCreate = nullptr; 	// character creation
	delete s_pProcMain; s_pProcMain = nullptr; 						// main game procedure
	delete s_pProcOption; s_pProcOption = nullptr; 					// game options procedure

	// Blowing the UI...
	if(s_pUILoading) delete s_pUILoading; s_pUILoading = nullptr;		// Loading Bar
	delete s_pMsgBoxMgr;
	delete s_pUIMgr; s_pUIMgr = nullptr;				// UI Manager

	// delete s_pIME; s_pIME = NULL;
	delete s_pLocalInput; s_pLocalInput = nullptr;
	delete s_pEng; s_pEng = nullptr; // The engine blows at the end of the gel.!!!!!

	if(s_pGameCursor) delete s_pGameCursor; s_pGameCursor = nullptr;

	CGameBase::StaticMemberRelease();
}

void CGameProcedure::Tick()
{
	s_pLocalInput->Tick(); // It receives input from the keyboard and mouse.
	if(s_pGameCursor) s_pGameCursor->Tick();

	ProcessUIKeyInput();

	DWORD dwMouseFlags = s_pLocalInput->MouseGetFlag();
	const POINT ptPrev = s_pLocalInput->MouseGetPosOld();
	const POINT ptCur = s_pLocalInput->MouseGetPos();

	const e_Nation eNation = s_pPlayer->m_InfoBase.eNation;
	if(dwMouseFlags & MOUSE_LBCLICK) SetGameCursor(((NATION_ELMORAD == eNation) ? s_hCursorClick1 : s_hCursorClick));
	else if(dwMouseFlags & MOUSE_LBCLICKED) SetGameCursor(((NATION_ELMORAD == eNation) ? s_hCursorNormal1 : s_hCursorNormal));
	if(dwMouseFlags & MOUSE_RBCLICKED)
	{
		if(s_pPlayer->m_bAttackContinous && s_pProcActive == s_pProcMain) // If the main process is...
			SetGameCursor(s_hCursorAttack);
		else
			SetGameCursor(((NATION_ELMORAD == eNation) ? s_hCursorNormal1 : s_hCursorNormal));
	}

	DWORD dwRet = s_pMsgBoxMgr->MouseProcAndTick(dwMouseFlags, s_pLocalInput->MouseGetPos(),
	                                             s_pLocalInput->MouseGetPosOld());

	if(0 == dwRet)
	{
		dwRet = s_pUIMgr->MouseProc(dwMouseFlags, ptCur, ptPrev);
		s_pUIMgr->Tick();
	}

	// If Monga...
	// if((dwRet & UI_MOUSEPROC_CHILDDONESOMETHING) || (dwRet & UI_MOUSEPROC_DONESOMETHING))
	// s_pLocalInput->MouseRemoveFlag(0xffMOUSE_LBCLICK | MOUSE_LBCLICKED | MOUSE_LBDBLCLK);
	s_pUIMgr->m_bDoneSomething = false;		// I did the manipulation in the UI...
	if(dwRet != UI_MOUSEPROC_NONE)
		s_pUIMgr->m_bDoneSomething = true;		// I did the manipulation in the UI...

	CN3Base::s_SndMgr.Tick(); // Sound Engine...

	// Screen capture key..
	if(s_pLocalInput->IsKeyPress(DIK_NUMPADMINUS)) // Press the minus key on the keypad...
	{
		SYSTEMTIME st;
		::GetLocalTime(&st);
		char szFN[128] = "";
		// sprintf(szFN, "%d_%d_%d_%d.%d.%d.jpg", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		sprintf(szFN, "%d_%d_%d_%d.%d.%d.ksc", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		this->CaptureScreenAndSaveToFile(szFN);
	}

	// 
	// Handling Network Msg
	DataPack* pDataPack = nullptr;
	while ( s_pSocket->m_qRecvPkt.size() > 0 )			// Is there a packet in the packet list????
	{
		int iOffset = 0;
		pDataPack = s_pSocket->m_qRecvPkt.front();				// Copy the first one in the queue.
		if (false == ProcessPacket(pDataPack, iOffset))
		{
			// It is not a situation to process packets.
			int iTempOffst = 0;
			const int iCmd = CAPISocket::Parse_GetByte(pDataPack->m_pData, iTempOffst);
			CLogWriter::Write("Invalid Packet... (%d)", iCmd);
		}
		delete pDataPack;
		s_pSocket->m_qRecvPkt.pop();					// Remove packet from queue.
	}

	while ( s_pSocketSub->m_qRecvPkt.size() > 0 )		// Is there a packet in the packet list????
	{
		int iOffset = 0;
		pDataPack = s_pSocketSub->m_qRecvPkt.front();			// Copy the first one in the queue.
		if (false == ProcessPacket(pDataPack, iOffset)) break;	// It is not a situation to process packets.
		delete pDataPack;
		s_pSocketSub->m_qRecvPkt.pop();					// Remove packet from queue.
	}
	// Handling Network Msg
	// 
}

void CGameProcedure::Render()
{
	if(s_pUIMgr) s_pUIMgr->Render(); // Rendering UIs...

	s_pMsgBoxMgr->Render();
	if(s_pGameCursor) s_pGameCursor->Render();
}

void CGameProcedure::TickActive()
{
	if(s_pProcActive != s_pProcPrev) // If the process changes...
	{
		if(s_pProcPrev) s_pProcPrev->Release();
		if(s_pProcActive) s_pProcActive->Init();

		s_pProcPrev = s_pProcActive;
	}

	if(s_pProcActive)	s_pProcActive->Tick();		// Current Procedure Tick ................................
}

void CGameProcedure::RenderActive()
{
	// if(s_pProcActive != s_pProcPrev) // If the procedure changes...
	// {
		// if(s_pProcPrev) s_pProcPrev->Release();
		// if(s_pProcActive) s_pProcActive->Init();
		//
		// s_pProcPrev = s_pProcActive;
	// }
	// else
	// {
		if(s_pProcActive == s_pProcPrev) s_pProcActive->Render();
	// }
}

bool CGameProcedure::CaptureScreenAndSaveToFile(const std::string& szFN)
{
	return true;
	/*
	if(szFN.empty()) return false;
	CJpegFile file;

	RECT wndRect;
	GetWindowRect(CN3Base::s_hWndBase, &wndRect);
	
	HANDLE hDIB = file.CopyScreenToDIB(&wndRect);
	if(hDIB)
	{
		int nQuality = 90;
		char szBuf[256] = "";

		// Enables operators to take good quality screen captures...
		if(s_pPlayer->m_InfoBase.iAuthority == AUTHORITY_MANAGER)
			nQuality = 100;

		if(file.EncryptJPEG(hDIB, nQuality, szFN, szBuf) == TRUE)
		{
			TRACE("Screen Captue %s\n", szFN.c_str());
		}
		GlobalFree(hDIB);
	}
	return true;
	*/
/*
	int iW = CN3Base::s_CameraData.vp.Width;
	int iH = CN3Base::s_CameraData.vp.Height;

	bool bResult = false;
	LPDIRECT3DSURFACE8 lpDDSTmp = NULL;
	LPDIRECT3DSURFACE8 lpDDSBack = NULL;
	CN3Base::s_lpD3DDev-&gt;GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &amp;lpDDSBack);
	if(lpDDSBack)
	{
		CN3Base::s_lpD3DDev->CreateImageSurface(iW, iH, D3DFMT_X8R8G8B8, &lpDDSTmp);
		if(lpDDSTmp)
		{
			HRESULT rval = D3DXLoadSurfaceFromSurface(lpDDSTmp, NULL, NULL, lpDDSBack, NULL, NULL, D3DX_FILTER_NONE, 0);
			// HRESULT rval = s_lpD3DDev->CopyRects(lpDDSBack, NULL, 0, lpDDSTmp, NULL);
			// char szErr[256];
			// ::D3DXGetErrorString(rval, szErr, 256);

			if(D3D_OK == rval)
			{
				D3DLOCKED_RECT LR;
				if(D3D_OK == lpDDSTmp->LockRect(&LR, NULL, 0))
				{
					// std::vector<BYTE> buff(iW * iH * 3, 0);
					CBitMapFile bmf;
					bmf.Create(iW, iH);

					for(int y = 0; y < iH; y++)
					{
						BYTE* pPS = ((BYTE*)LR.pBits) + LR.Pitch * y;
						// BYTE* pPD = (BYTE*)(&(buff[y * (iW * 3)]));
						BYTE* pPD = (BYTE*)(bmf.Pixels(0, y));

						for(int x = 0; x < iW; x++, pPS += 4, pPD +=3 )
						{
							pPD[0] = pPS[0];
							pPD[1] = pPS[1];
							pPD[2] = pPS[2];
						}
					}
					lpDDSTmp->UnlockRect();

					// CJpeg jpg;
					// jpg.SaveJPG(szFN.c_str(), iW, iH, &(buff[0]));
					bmf.SaveToFile(szFN.c_str());
				}
			}
			
			lpDDSTmp->Release();
			lpDDSTmp = NULL;
		}


		lpDDSBack->Release();
		lpDDSBack = NULL;
	}

	return bResult;
*/
}

void CGameProcedure::ProcActiveSet(CGameProcedure *pProc)
{
	if(nullptr == pProc || s_pProcActive == pProc) return;

	if(s_pUIMgr) s_pUIMgr->EnableOperationSet(true); // Allows you to manipulate the UI.
	CGameProcedure::MessageBoxClose(-1); // If the MessageBox is floating, hide it.

	s_pProcPrev = s_pProcActive; // Remember the previous pointer...
	s_pProcActive = pProc;
}

void CGameProcedure::ReConnect()
{
	s_bNeedReportConnectionClosed = false; // Should I report that the server is disconnected?
	CGameProcedure::s_pSocket->ReConnect();
	s_bNeedReportConnectionClosed = true; // Should I report that the server is disconnected?
}

std::string CGameProcedure::MessageBoxPost(const std::string& szMsg, const std::string& szTitle, int iStyle, e_Behavior eBehavior)
{
	return s_pMsgBoxMgr->MessageBoxPost(szMsg, szTitle, iStyle, eBehavior);
}

void CGameProcedure::MessageBoxClose(const std::string& szMsg)
{
	s_pMsgBoxMgr->MessageBoxClose(szMsg);
}

void CGameProcedure::MessageBoxClose(int iMsgBoxIndex)
{
	if( iMsgBoxIndex == -1 )
		s_pMsgBoxMgr->MessageBoxCloseAll();
}


bool CGameProcedure::RegPutSetting( const char *ValueName, void *pValueData, long length )
{
	HKEY	hKey;

	if ( RegOpenKey( HKEY_CURRENT_USER, GetStrRegKeySetting().c_str(), &hKey ) != ERROR_SUCCESS ) 
	{
		if ( RegCreateKey( HKEY_CURRENT_USER, GetStrRegKeySetting().c_str(), &hKey ) != ERROR_SUCCESS )
		{
			__ASSERT(0, "Registry Create Failed!!!");
			return false;
		}
		if(RegOpenKey( HKEY_CURRENT_USER, GetStrRegKeySetting().c_str(), &hKey ) != ERROR_SUCCESS ) 
		{
			__ASSERT(0, "Registry Open Failed!!!");
			return false;
		}
	}

	// set the value
	if ( RegSetValueEx( hKey, ValueName, 0, REG_BINARY, (const BYTE *)pValueData, length ) != ERROR_SUCCESS )
	{
		__ASSERT(0, "Registry Write Failed!!!");
		RegCloseKey( hKey );
		return false;
	}

	if ( RegCloseKey( hKey ) != ERROR_SUCCESS ) 
	{
		__ASSERT(0, "Registry Close Failed!!!");
		return false;
	}

	return true;
}

bool CGameProcedure::RegGetSetting( const char *ValueName, void *pValueData, long length)
{
	HKEY	hKey;
	DWORD	Type;
	DWORD	len;

	len = length;

	if ( RegOpenKey( HKEY_CURRENT_USER, GetStrRegKeySetting().c_str(), &hKey ) != ERROR_SUCCESS ) 
	{
		// __ASSERT(0, "Registry Open Failed!!!");
		return false;
	}

	// get the value
	if ( RegQueryValueEx( hKey, ValueName, nullptr, &Type, (BYTE *)pValueData, &len ) != ERROR_SUCCESS )
	{
		// __ASSERT(0, "Registry Query Failed!!!");
		RegCloseKey( hKey );
		return false;
	}

	if ( RegCloseKey( hKey ) != ERROR_SUCCESS ) 
	{
		// __ASSERT(0, "Registry Close Failed!!!");
		return false;
	}

	return true;
}

void CGameProcedure::UIPostData_Write(const std::string& szKey, CN3UIBase* pUI)
{
	if(szKey.empty() || nullptr == pUI) return;

	__WndInfo WI;
	lstrcpyn(WI.szName, szKey.c_str(), 16);
	WI.bVisible = pUI->IsVisible();
	WI.ptPosition = pUI->GetPos();

	RegPutSetting(WI.szName, &WI, sizeof(__WndInfo) );
}

void CGameProcedure::UIPostData_Read(const std::string& szKey, CN3UIBase* pUI, int iDefaultX, int	 iDefaultY)
{
	if(szKey.empty() || nullptr == pUI) return;

	// 1. Create default data.
	// 2. Read data..
	// 3. Determine whether the realm is valid.

	__WndInfo WI;
	WI.ptPosition.x = iDefaultX;
	WI.ptPosition.y = iDefaultY;
	if(false == RegGetSetting(szKey.c_str(), &WI, sizeof(__WndInfo))) WI.bVisible = true; // If there is no basic data, it will be shown unconditionally.

	const RECT rc = pUI->GetRegion();

	if (WI.ptPosition.x < 0) WI.ptPosition.x = 0;
	if (WI.ptPosition.x + (rc.right - rc.left) > CN3Base::s_CameraData.vp.Width)
		WI.ptPosition.x = CN3Base::s_CameraData.vp.Width - (rc.right - rc.left);
	if (WI.ptPosition.y < 0) WI.ptPosition.y = 0;
	if (WI.ptPosition.y + (rc.bottom - rc.top) > CN3Base::s_CameraData.vp.Height)
		WI.ptPosition.y = CN3Base::s_CameraData.vp.Height - (rc.bottom - rc.top);

	pUI->SetVisible(WI.bVisible);
	if(0 == WI.ptPosition.x && 0 == WI.ptPosition.y)
		pUI->SetPos(iDefaultX, iDefaultY);
	else pUI->SetPos(WI.ptPosition.x, WI.ptPosition.y);
}

void CGameProcedure::SetGameCursor(HCURSOR hCursor, bool bLocked)
{
	if(s_pGameCursor)
	{
		e_Cursor eCursor = CURSOR_KA_NORMAL;

		if(hCursor == s_hCursorNormal)
			eCursor = CURSOR_KA_NORMAL;
		else if(hCursor == s_hCursorNormal1)
			eCursor = CURSOR_EL_NORMAL;
		else if(hCursor == s_hCursorClick)
			eCursor = CURSOR_KA_CLICK;
		else if(hCursor == s_hCursorClick1)
			eCursor = CURSOR_EL_CLICK;
		else if(hCursor == s_hCursorAttack)
			eCursor = CURSOR_ATTACK;
		else if(hCursor == s_hCursorPreRepair)
			eCursor = CURSOR_PRE_REPAIR;
		else if(hCursor == s_hCursorNowRepair)
			eCursor = CURSOR_NOW_REPAIR;
		else if(hCursor == nullptr)
			eCursor = CURSOR_UNKNOWN;

		SetGameCursor(eCursor, bLocked);

		if ((!m_bCursorLocked) && bLocked)
		{
			m_bCursorLocked = true;
		}

	}
	else
	{
		if ((m_bCursorLocked) && (!bLocked) ) return;
		else if ( ((m_bCursorLocked) && bLocked) || ((!m_bCursorLocked) && !bLocked) )
		{
			::SetCursor(hCursor);
			return;
		}
		else if ((!m_bCursorLocked) && bLocked)
		{
			m_hPrevGameCursor = ::GetCursor();
			m_bCursorLocked = true;
			::SetCursor(hCursor);
		}
	}
}

void CGameProcedure::SetGameCursor(e_Cursor eCursor, bool bLocked)
{
	if(s_pGameCursor == nullptr) return;
	s_pGameCursor->SetGameCursor(eCursor, bLocked);
}

void CGameProcedure::RestoreGameCursor()
{
	if(s_pGameCursor)
	{
		if (m_bCursorLocked) 
			m_bCursorLocked = false;

		if(s_pGameCursor) s_pGameCursor->RestoreGameCursor();
	}
	else
	{
		if (m_bCursorLocked) 
			m_bCursorLocked = false;

		::SetCursor(m_hPrevGameCursor);	
	}
}

std::string CGameProcedure::GetStrRegKeySetting()
{
	char szBuff[256];
	sprintf(szBuff, "Software\\KnightOnline\\%s_%s_%d", s_szAccount.c_str(), s_szServer.c_str(), s_iChrSelectIndex);
	return szBuff;
}

bool CGameProcedure::ProcessPacket(DataPack* pDataPack, int& iOffset)
{
	const int iCmd = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// Command parsing...
	switch ( iCmd )										// Branch according to the command..
	{
		case N3_VERSION_CHECK: // Encryption is also received.
			this->MsgRecv_VersionCheck(pDataPack, iOffset); // virtual
			return true;

		case N3_GAME_SERVER_LOGIN:
			this->MsgRecv_GameServerLogIn(pDataPack, iOffset);
			return true;

		case N3_SERVER_CHANGE:				// server change message.
		{
			// Reconnect to another zone server.
			std::string szName, szIP;
			// iLen = CAPISocket::Parse_GetShort(pDataPack-&gt;m_pData, iOffset); // server name
			// CAPISocket::Parse_GetString(pDataPack-&gt;m_pData, iOffset, szName, iLen);
			const int iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // server IP
			CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szIP, iLen);
			const DWORD dwPort = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			s_pPlayer->m_InfoExt.iZoneInit = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			s_pPlayer->m_InfoExt.iZoneCur = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			const int iVictoryNation = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			CGameProcedure::LoadingUIChange(iVictoryNation);

			s_bNeedReportConnectionClosed = false; // Should I report that the connection to the server has been lost?
			s_pSocket->Disconnect(); // hang up...
			Sleep(2000); // 2 second delay.. to give the server time to process.
			const int iErr = s_pSocket->Connect(s_hWndBase, szIP.c_str(), dwPort);
			s_bNeedReportConnectionClosed = true; // Should I report that the server is disconnected?

			if(iErr) this->ReportServerConnectionFailed("Current Zone", iErr, true); // Server connection error.. Exit.
			else
			{
				// If you send a version check... the version and encryption key will come as a response.
				// Send Character_Select in case of main procedure and send GameServer_LogIn in case of login.
				this->MsgSend_VersionCheck(); 
			}
		}
		return true;

		case N3_CHARACTER_SELECT:
		{
			this->MsgRecv_CharacterSelect(pDataPack, iOffset); // virtual
		}
		return true;

		case N3_ALIVE_CHECK:
		{
			this->MsgSend_AliveCheck();
		}
		return true;
	}

	return false;
}

void CGameProcedure::ReportServerConnectionFailed(const std::string& szServerName, int iErrCode, bool bNeedQuitGame)
{
	char szErr[256];
	std::string szFmt; ::_LoadStringFromResource(IDS_FMT_CONNECT_ERROR, szFmt);
	sprintf(szErr, szFmt.c_str(), szServerName.c_str(), iErrCode);

	const e_Behavior eBehavior = ((bNeedQuitGame) ? BEHAVIOR_EXIT : BEHAVIOR_NOTHING);
	CGameProcedure::MessageBoxPost(szErr, "", MB_OK, eBehavior);
	return;
}

void CGameProcedure::ReportServerConnectionClosed(bool bNeedQuitGame)
{
	if(!s_bNeedReportConnectionClosed) return;

	std::string szMsg; ::_LoadStringFromResource(IDS_CONNECTION_CLOSED, szMsg);
	const e_Behavior eBehavior = ((bNeedQuitGame) ? BEHAVIOR_EXIT : BEHAVIOR_NOTHING);
	CGameProcedure::MessageBoxPost(szMsg, "", MB_OK, eBehavior);

	if(s_pPlayer)
	{
		const __Vector3 vPos = s_pPlayer->Position();
		CLogWriter::Write("Socket Closed... Zone(%d) Pos(%.1f, %.1f, %.1f) Exp(%d)",
			s_pPlayer->m_InfoExt.iZoneCur, vPos.x, vPos.y, vPos.z, s_pPlayer->m_InfoExt.iExp);
	}
	else
	{
		CLogWriter::Write("Socket Closed...");
	}

	if(s_pSocket) s_pSocket->Release();
}

void CGameProcedure::ReportDebugStringAndSendToServer(const std::string& szDebug)
{
	if(szDebug.empty()) return;

	CLogWriter::Write(szDebug.c_str());

	if(s_pSocket && s_pSocket->IsConnected())
	{
		const int iLen = szDebug.size();
		std::vector<BYTE> buffer;	// buffer..
		buffer.reserve(iLen + 4);
		int iOffset=0;												// Offset...
		s_pSocket->MP_AddByte(&(buffer[0]), iOffset, N3_REPORT_DEBUG_STRING);
		s_pSocket->MP_AddShort(&(buffer[0]), iOffset, iLen);
		s_pSocket->MP_AddString(&(buffer[0]), iOffset, szDebug);
		s_pSocket->Send(&(buffer[0]), iOffset);				// sent...
	}
}

void CGameProcedure::MsgSend_GameServerLogIn()
{
	BYTE byBuff[128];										// Packet buffer...
	int iOffset = 0;										// Offset of buffer..

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_GAME_SERVER_LOGIN);	// command.
	CAPISocket::MP_AddShort(byBuff, iOffset, s_szAccount.size());	// id length.
	CAPISocket::MP_AddString(byBuff, iOffset, s_szAccount);			// real id.
	CAPISocket::MP_AddShort(byBuff, iOffset, s_szPassWord.size());	// password length
	CAPISocket::MP_AddString(byBuff, iOffset, s_szPassWord);		// real password
		
	s_pSocket->Send(byBuff, iOffset);								// send
}

void CGameProcedure::MsgSend_VersionCheck() // virtual
{
	// Version Check
	int iOffset = 0;
	BYTE byBuffs[4];
	CAPISocket::MP_AddByte(byBuffs, iOffset, N3_VERSION_CHECK);				// command.
	s_pSocket->Send(byBuffs, iOffset);	// send

#ifdef _CRYPTION
	s_pSocket->m_bEnableSend = FALSE; // Can you send..?
#endif // #ifdef _CRYPTION
}

void CGameProcedure::MsgSend_CharacterSelect() // virtual
{
	BYTE byBuff[64];
	int iOffset = 0;
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_CHARACTER_SELECT);				// command.
	CAPISocket::MP_AddShort(byBuff, iOffset, s_szAccount.size());				// account length.
	CAPISocket::MP_AddString(byBuff, iOffset, s_szAccount);						// account string..
	CAPISocket::MP_AddShort(byBuff, iOffset, s_pPlayer->IDString().size());		// Character ID length.
	CAPISocket::MP_AddString(byBuff, iOffset, s_pPlayer->IDString());			// Character ID string..
	CAPISocket::MP_AddByte(byBuff, iOffset, s_pPlayer->m_InfoExt.iZoneInit);	// Whether it is the first connection or not 0x01: First connection
	CAPISocket::MP_AddByte(byBuff, iOffset, s_pPlayer->m_InfoExt.iZoneCur);		// Character zone number in the character selection screen
	s_pSocket->Send(byBuff, iOffset);	// send

	CLogWriter::Write("MsgSend_CharacterSelect - name(%s) zone(%d)",
		s_pPlayer->IDString().c_str(), s_pPlayer->m_InfoExt.iZoneCur); // Debugging log...
}

void CGameProcedure::MsgSend_AliveCheck()
{
	BYTE byBuff[4];
	int iOffset = 0;
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ALIVE_CHECK);				// command.
	s_pSocket->Send(byBuff, iOffset);	// send
}

int CGameProcedure::MsgRecv_VersionCheck(DataPack* pDataPack, int& iOffset) // virtual
{
	const int iVersion = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	// version
#ifdef _CRYPTION
	__int64 iPublicKey = CAPISocket::Parse_GetInt64(pDataPack->m_pData, iOffset); // encryption public key
	DataPack::InitCrypt(iPublicKey);
	s_pSocket->m_bEnableSend = TRUE; // Can you send..?
#endif // #ifdef _CRYPTION

	if(iVersion != CURRENT_VERSION)
	{
		char szErr[256];

		const int iLangID = ::GetUserDefaultLangID();
		if(0x0404 == iLangID)// Taiwan Language
		{
			std::string szFmt; ::_LoadStringFromResource(IDS_VERSION_CONFIRM_TW, szFmt);
			CGameProcedure::MessageBoxPost(szFmt, "", MB_OK, BEHAVIOR_EXIT);
		}
		else
		{
			std::string szFmt; ::_LoadStringFromResource(IDS_VERSION_CONFIRM, szFmt);
			sprintf(szErr, szFmt.c_str(), CURRENT_VERSION / 1000.0f, iVersion / 1000.0f);
			CGameProcedure::MessageBoxPost(szErr, "", MB_OK, BEHAVIOR_EXIT);
		}

	}

	return iVersion;
}

int CGameProcedure::MsgRecv_GameServerLogIn(DataPack* pDataPack, int& iOffset) // virtual
{
	const int iNation = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Country - 0 None 0xff - Failed..
	return iNation;
}

bool CGameProcedure::MsgRecv_CharacterSelect(DataPack* pDataPack, int& iOffset) // virtual
{
	const int iResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // 0x00 fail
	if(1 == iResult) // success..
	{
		const int iZoneCur = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		const float fX = (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
		const float fZ = (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
		const float fY = (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;

		const int iVictoryNation = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		CGameProcedure::LoadingUIChange(iVictoryNation);

		const int iZonePrev = 
		s_pPlayer->m_InfoExt.iZoneCur = iZoneCur;
		s_pPlayer->PositionSet(__Vector3(fX, fY, fZ), true);

		CLogWriter::Write("MsgRecv_CharacterSelect - name(%s) zone(%d -> %d)", s_pPlayer->m_InfoBase.szID.c_str(), iZonePrev, iZoneCur);
		return true;
	}
	else // failure
	{
		CLogWriter::Write("MsgRecv_CharacterSelect - failed(%d)", iResult);
		return false;
	}

	if(iResult) return true;
	else return false;
}

void CGameProcedure::ProcessUIKeyInput(bool bEnable)
{
	s_bKeyPressed	= false;	// If the key has been manipulated in the ui when the key is raised

	if(!bEnable)
	{
		if(s_bKeyPress)
		{
			for(int i = 0; i < NUMDIKEYS; i++)
			{
				if(s_pLocalInput->IsKeyPressed(i))
				{
					if(!s_bKeyPressed) s_bKeyPress = false;
					break;
				}
			}
		}
		return;
	}


	CN3UIBase* pMsgBox = s_pMsgBoxMgr->GetFocusMsgBox();
	CN3UIBase* pUIFocus = s_pUIMgr->GetFocusedUI();

	if(pMsgBox && pMsgBox->IsVisible())// this_ui
	{
		for(int i = 0; i < NUMDIKEYS; i++)
		{
			if(s_pLocalInput->IsKeyPress(i))
				s_bKeyPress |= pMsgBox->OnKeyPress(i);
			if(s_pLocalInput->IsKeyPressed(i))
				s_bKeyPressed |= pMsgBox->OnKeyPressed(i);
		}
	}
	else if(pUIFocus && pUIFocus->IsVisible()) // If you have a focused UI...
	{
		for(int i = 0; i < NUMDIKEYS; i++)
		{
			if(s_pLocalInput->IsKeyPress(i))
			{
				if(pUIFocus->m_pChildUI && pUIFocus->m_pChildUI->IsVisible())
					s_bKeyPress |= pUIFocus->m_pChildUI->OnKeyPress(i);
				else
					s_bKeyPress |= pUIFocus->OnKeyPress(i);
			}
			if(s_pLocalInput->IsKeyPressed(i))
			{
				if(pUIFocus->m_pChildUI && pUIFocus->m_pChildUI->IsVisible())
					s_bKeyPressed |= pUIFocus->m_pChildUI->OnKeyPressed(i);
				else
					s_bKeyPressed |= pUIFocus->OnKeyPressed(i);
			}
		}
	}

	if(s_bKeyPress)
	{
		for(int i = 0; i < NUMDIKEYS; i++)
		{
			if(s_pLocalInput->IsKeyPressed(i))
			{
				if(!s_bKeyPressed) s_bKeyPress = false;
				break;
			}
		}
	}
}

bool CGameProcedure::IsUIKeyOperated()
{
	if (!s_bKeyPress && !s_bKeyPressed)
		return false;

	return true;
}

void CGameProcedure::LoadingUIChange(int iVictoryNation)
{
	if(s_pPlayer->m_InfoExt.iVictoryNation == iVictoryNation)
		return;

	s_pPlayer->m_InfoExt.iVictoryNation = iVictoryNation;

	std::string szLoading;
	if(s_pUILoading) delete s_pUILoading; s_pUILoading = nullptr;		// Loading Bar

	s_pUILoading = new CUILoading();
	__ASSERT(s_pUILoading, "로딩화면 생성 실패");
	if(s_pUILoading == nullptr) return;

	const __TABLE_UI_RESRC* pTblUI = s_pTbl_UI->Find(NATION_ELMORAD); // The default is Elmorad UI.
	__ASSERT(pTblUI, "기본 UI 가 없습니다.");
	if(pTblUI == nullptr) return;

	switch(iVictoryNation)
	{
	case VICTORY_ABSENCE:
		szLoading = pTblUI->szLoading;
		break;
	case VICTORY_ELMORAD:
		szLoading = pTblUI->szElLoading;
		break;
	case VICTORY_KARUS:
		szLoading = pTblUI->szKaLoading;
		break;
	default:
		szLoading = pTblUI->szLoading;
		break;
	}

	TRACE("Loading UIF : %s\n", szLoading.c_str());
	s_pUILoading->LoadFromFile(szLoading); // Creating a basic loading bar...
}
