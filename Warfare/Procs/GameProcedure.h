#pragma once

#include "GameDef.h"
#include "GameBase.h"

#define		UI_POST_WND_CHAT		"Wnd_Chat"
#define		UI_POST_WND_HOTKEY		"Wnd_Hotkey"
#define		UI_POST_WND_PARTY		"Wnd_Party"
#define		UI_POST_WND_HELP		"Wnd_Help"

const int MAX_MSG_BOX = 4;

struct __WndInfo
{
	char			szName[16];
	bool			bVisible;
	POINT			ptPosition;

	__WndInfo()	{ memset(this, 0, sizeof(__WndInfo));	}
};

class CHotkeyData 
{
public:
	int	row;
	int column;
	int iID;

	CHotkeyData() {	memset(this, 0, sizeof(CHotkeyData)); }
	CHotkeyData(int rw, int cl, int id) 
	{
		CHotkeyData();
		row = rw;	column = cl;	iID = id;
	}
};

class DataPack;

class CGameProcedure : public CGameBase
{
public:		
	static class CN3SndObjStream*	s_pSnd_BGM;			// Main background music pointer..
	static class CLocalInput*		s_pLocalInput;		// Mouse and keyboard input object .. I wrote Direct Input.
	static class CAPISocket*		s_pSocket;			// main socket object
	static class CAPISocket*		s_pSocketSub;		// subsocket object..
	static class CGameEng*			s_pEng;				// 3D Wrapper Engine
	static class CN3FXMgr*			s_pFX;
	
	static class CUIManager*		s_pUIMgr;					// UI Manager
	static class CUILoading*		s_pUILoading;				// loading bar...
	static class CUIMessageBoxManager*	s_pMsgBoxMgr;			// MessageBox Manager

	static class CGameProcLogIn*			s_pProcLogIn;
	static class CGameProcNationSelect*		s_pProcNationSelect;
	static class CGameProcCharacterCreate*	s_pProcCharacterCreate;
	static class CGameProcCharacterSelect*	s_pProcCharacterSelect;
	static class CGameProcMain*				s_pProcMain;
	static class CGameProcOption*			s_pProcOption;

	static class CGameProcedure*			s_pProcPrev;
	static class CGameProcedure*			s_pProcActive;

	static class CGameCursor*				s_pGameCursor;

	static HCURSOR	s_hCursorNormal;
	static HCURSOR	s_hCursorNormal1;
	static HCURSOR	s_hCursorClick;
	static HCURSOR	s_hCursorClick1;
	static HCURSOR	s_hCursorAttack;
	static HCURSOR	s_hCursorPreRepair;
	static HCURSOR	s_hCursorNowRepair;

	static std::string				s_szAccount; // account string..
	static std::string				s_szPassWord; // account password..
	static std::string				s_szServer; // servername string.
	static int						s_iChrSelectIndex; // What character is this account...??

	static bool		m_bCursorLocked;
	static HCURSOR	m_hPrevGameCursor;
	static HWND		s_hWndSubSocket; // Window handle for subsockets..

	static bool		s_bNeedReportConnectionClosed; // Should I report that the server is disconnected?
	static bool		s_bWindowed; // Run windowed mode??
	static bool		s_bKeyPress;	// If a key has been manipulated in the UI when it is pressed
	static bool		s_bKeyPressed;	// If the key has been manipulated in the ui when the key is raised

public:
	static std::string MessageBoxPost(const std::string& szMsg, const std::string& szTitle, int iStyle, e_Behavior eBehavior = BEHAVIOR_NOTHING);
	static void MessageBoxClose(const std::string& szMsg);
	static void MessageBoxClose(int iMsgBoxIndex);

	static void ProcActiveSet(CGameProcedure* pProc);
	static void ReConnect();
	
	static void ReportServerConnectionFailed(const std::string& szServerName, int iErrCode, bool bNeedQuitGame);
	static void ReportServerConnectionClosed(bool bNeedQuitGame);
	static void ReportDebugStringAndSendToServer(const std::string& szDebug);

	virtual int		MsgRecv_VersionCheck(DataPack* pDataPack, int& iOffset); // You will also receive an encryption key.
	virtual int		MsgRecv_GameServerLogIn(DataPack* pDataPack, int& iOffset); // virtual - Returns the country code.
	virtual bool	MsgRecv_CharacterSelect(DataPack* pDataPack, int& iOffset);

	static void		MsgSend_GameServerLogIn();
	void			MsgSend_VersionCheck();
	virtual void	MsgSend_CharacterSelect();
	void			MsgSend_AliveCheck();


	
	virtual void Release(); // Freeing up resources...
	virtual void Init(); // Initialize and load necessary elements
	virtual void Tick();  // Returns the procedure index. If 0, proceed as is.
	virtual void Render();
	virtual void ProcessUIKeyInput(bool bEnable = true);

	static bool	IsUIKeyOperated();
	static void	StaticMemberInit(HINSTANCE hInstance, HWND hWndMain, HWND hWndSub);
	static void StaticMemberRelease();

	static void TickActive();
	static void RenderActive();

	static bool RegPutSetting( const char *ValueName, void *pValueData, long length );
	static bool RegGetSetting( const char *ValueName, void *pValueData, long length );

	static void	UIPostData_Read(const std::string& szKey, class CN3UIBase* pUI, int iDefaultX, int iDefaultY);
	static void	UIPostData_Write(const std::string& szKey, CN3UIBase* pUI);

	static bool CaptureScreenAndSaveToFile(const std::string& szFN);

	static void	SetGameCursor(HCURSOR hCursor, bool bLocked = false);
	static void	SetGameCursor(e_Cursor eCursor, bool bLocked = false);
	static void RestoreGameCursor();

protected:
	virtual bool ProcessPacket(DataPack* pDataPack, int& iOffset);

private:
	static std::string GetStrRegKeySetting();

public:
	void LoadingUIChange(int iVictoryNation);
	CGameProcedure();
	virtual ~CGameProcedure();
};
