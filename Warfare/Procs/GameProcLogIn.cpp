#include "pch.h"
#include "GameEng.h"
#include "GameProcLogIn.h"
#include "UILogIn.h"
#include "PlayerMySelf.h"
#include "UIManager.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "PacketDef.h"

#include "N3Camera.h"
#include "N3Light.h"
#include "N3Chr.h"
#include "N3SndObj.h"
#include "N3SndObjStream.h"
#include "N3SndMgr.h"

CGameProcLogIn::CGameProcLogIn()
{
	m_pUILogIn	= nullptr;
	m_bLogIn = false;
}

CGameProcLogIn::~CGameProcLogIn()
{
	delete m_pUILogIn;
}

void CGameProcLogIn::Release()
{
	CGameProcedure::Release();

	delete m_pUILogIn; m_pUILogIn = nullptr;
}

void CGameProcLogIn::Init()
{
	CGameProcedure::Init();

	s_pEng->s_SndMgr.ReleaseStreamObj(&(CGameProcedure::s_pSnd_BGM));
	CGameProcedure::s_pSnd_BGM = s_pEng->s_SndMgr.CreateStreamObj(35);	// The 26-second sound of a monster howling...

	m_pUILogIn = new CUILogIn();
	m_pUILogIn->Init(s_pUIMgr);

	const __TABLE_UI_RESRC* pTbl = s_pTbl_UI->GetIndexedData(0); // Because there are no national standards...
	if(pTbl) m_pUILogIn->LoadFromFile(pTbl->szLoginIntro);

	RECT rc = m_pUILogIn->GetRegion();
	const int iX = (CN3Base::s_CameraData.vp.Width - (rc.right - rc.left))/2;
	const int iY = CN3Base::s_CameraData.vp.Height - (rc.bottom - rc.top);
	m_pUILogIn->SetPos(iX, iY);
	rc.left = 0; rc.top = 0; rc.right = CN3Base::s_CameraData.vp.Width; rc.bottom = CN3Base::s_CameraData.vp.Height;
	m_pUILogIn->SetRegion(rc); // You need to do this in order for the UI to work properly.
	s_pUIMgr->SetFocusedUI((CN3UIBase*)m_pUILogIn);

	// socket connection..
	char szIniPath[_MAX_PATH] = "";
	lstrcpy(szIniPath, CN3Base::PathGet().c_str());
	lstrcat(szIniPath, "Server.Ini");
	const int iServerCount = GetPrivateProfileInt("Server", "Count", 0, szIniPath);

	char szIPs[256][32]; memset(szIPs, 0, sizeof(szIPs));
	for(auto i = 0; i < iServerCount; i++)
	{
		char szKey[32] = "";
		sprintf(szKey, "IP%d", i);
		GetPrivateProfileString("Server", szKey, "", szIPs[i], 32, szIniPath);
	}
	int iServer = -1;
	if(iServerCount > 0) iServer = rand()%iServerCount;
	
	if(	iServer >= 0 && lstrlen(szIPs[iServer]) )
	{
		s_bNeedReportConnectionClosed = false; // Should I report that the server is disconnected?
		const int iErr = s_pSocket->Connect(s_hWndBase, szIPs[iServer], SOCKET_PORT_LOGIN);
		s_bNeedReportConnectionClosed = true; // Should I report that the server is disconnected?
		if(iErr) this->ReportServerConnectionFailed("LogIn Server", iErr, true);
		else
		{
			m_pUILogIn->FocusToID();
		}
	}
	else
	{
		this->MessageBoxPost("No server list", "LogIn Server fail", MB_OK, BEHAVIOR_EXIT); // finish
	}
}

void CGameProcLogIn::Tick()
{
	CGameProcedure::Tick();

	static float fTmp	= 0;
	if(fTmp == 0)
	{
		if(CGameProcedure::s_pSnd_BGM) CGameProcedure::s_pSnd_BGM->Play();
	}
	fTmp += CN3Base::s_fSecPerFrm;
	if(fTmp > 21.66f)
	{
		fTmp = 0;
		if(CGameProcedure::s_pSnd_BGM) CGameProcedure::s_pSnd_BGM->Stop();		
	}
}

void CGameProcLogIn::Render()
{
	s_pEng->Clear(0x00000000);
	s_pEng->s_lpD3DDev->BeginScene();

	CGameProcedure::Render();

	s_pEng->s_lpD3DDev->EndScene();
	s_pEng->Present(CN3Base::s_hWndBase);
}

bool CGameProcLogIn::MsgSend_AccountLogIn()
{
	m_pUILogIn->AccountIDGet(s_szAccount); // Remember account...
	m_pUILogIn->AccountPWGet(s_szPassWord); // Remember password...

	if(	s_szAccount.empty() || s_szPassWord.empty() || s_szAccount.size() >= 20 || s_szPassWord.size() >= 12) return false;

	m_pUILogIn->SetVisibleLogInUIs(false); // Disable the UI until packets arrive...
	m_pUILogIn->SetRequestedLogIn(true);
	m_bLogIn = true; // Attempting to log in...

	BYTE byBuff[256];										// Packet buffer...
	int iOffset=0;										// Offset of buffer..

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ACCOUNT_LOGIN);				// command.
	CAPISocket::MP_AddShort(byBuff, iOffset, s_szAccount.size());	// id length.
	CAPISocket::MP_AddString(byBuff, iOffset, s_szAccount);		// real id.
	CAPISocket::MP_AddShort(byBuff, iOffset, s_szPassWord.size());	// password length
	CAPISocket::MP_AddString(byBuff, iOffset, s_szPassWord);		// real password
		
	s_pSocket->Send(byBuff, iOffset);								// send

	return true;
}

void CGameProcLogIn::MsgSend_GameServerList()
{
	BYTE byBuff[1];
	int iOffset = 0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_GAMESERVER_GROUP_LIST);

	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcLogIn::MsgRecv_GameServerGroupList(DataPack* pDataPack, int& iOffset) const
{
	const int iServerCount = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// number of servers
	for(int i = 0; i < iServerCount; i++)
	{
		std::string szServerName, szServerIp;
		uint16_t iConcurrentUserCount, iConcurrentCapacity;

		int iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szServerIp, iLen);
		iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szServerName, iLen);
		iConcurrentUserCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Current number of concurrent users..
		iConcurrentCapacity = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Current number of concurrent users..
		
		m_pUILogIn->ServerInfoAdd(i, szServerName, szServerIp, iConcurrentUserCount, iConcurrentCapacity); // ServerList
	}

	m_pUILogIn->OpenServerList();
}

void CGameProcLogIn::MsgRecv_AccountLogIn(int iCmd, DataPack* pDataPack, int& iOffset)
{
	int iResult = CAPISocket::Parse_GetByte( pDataPack->m_pData, iOffset ); // Recv - b1 (0: Failure 1: Success 2: No ID 3: PW Wrong 4: Server Checking)
	if(1 == iResult) // Connection successful..
	{
		// Close all message boxes...
		this->MessageBoxClose(-1);

		m_pUILogIn->HideLoginSubview();

		BYTE byBuff[4];
		int iOffset = 0;

		CAPISocket::MP_AddByte(byBuff, iOffset, N3_NOTICE_LIST);

		s_pSocket->Send(byBuff, iOffset);								// send
	}
	else if(2 == iResult) // If you fail because you don't have an ID...
	{
		std::string szMsg;
		std::string szTmp;
		::_LoadStringFromResource(IDS_NO_MGAME_ACCOUNT, szMsg);
		::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);

		this->MessageBoxPost(szMsg, szTmp, MB_OK);
	}
	else if(3 == iResult) // PassWord Failed
	{
		std::string szMsg;
		std::string szTmp;
		::_LoadStringFromResource(IDS_WRONG_PASSWORD, szMsg);
		::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);
		this->MessageBoxPost(szMsg, szTmp, MB_OK); // It asks if you want to connect with MGame ID.
	}
	else if(4 == iResult) // Server maintenance??
	{
		std::string szMsg;
		std::string szTmp;
		::_LoadStringFromResource(IDS_SERVER_CONNECT_FAIL, szMsg);
		::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);
		this->MessageBoxPost(szMsg, szTmp, MB_OK); // It asks if you want to connect with MGame ID.
	}
	else if(5 == iResult) // What numbers are connected. Let's tell the server to shut it down.
	{
		int iLen = CAPISocket::Parse_GetShort( pDataPack->m_pData, iOffset );
		if(iOffset > 0)
		{
			std::string szIP;
			CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szIP, iLen);
			DWORD dwPort = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

			CAPISocket socketTmp;
			s_bNeedReportConnectionClosed = false; // Should I report that the server is disconnected?
			if(0 == socketTmp.Connect(s_hWndBase, szIP.c_str(), dwPort))
			{
				// Connect to the game server address you received from the login server and tell them to cut it.
				int iOffset2 = 0;
				BYTE Buff[32];
				CAPISocket::MP_AddByte(Buff, iOffset2, N3_KICK_OUT); // Recv s1, str1(IP) s1(port) | Send s1, str1(ID)
				CAPISocket::MP_AddShort(Buff, iOffset2, s_szAccount.size()); 
				CAPISocket::MP_AddString(Buff, iOffset2, s_szAccount); // Recv s1, str1(IP) s1(port) | Send s1, str1(ID)
				
				socketTmp.Send(Buff, iOffset2);
				socketTmp.Disconnect(); // cut it..
			}
			s_bNeedReportConnectionClosed = true; // Should I report that the server is disconnected?

			std::string szMsg;
			std::string szTmp;
			::_LoadStringFromResource(IDS_LOGIN_ERR_ALREADY_CONNECTED_ACCOUNT, szMsg);
			::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);
			this->MessageBoxPost(szMsg, szTmp, MB_OK); // Ask if you want to connect again.
		}
	}
	else
	{
		std::string szMsg;
		std::string szTmp;
		::_LoadStringFromResource(IDS_CURRENT_SERVER_ERROR, szMsg);
		::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);
		this->MessageBoxPost(szMsg, szTmp, MB_OK); // It asks if you want to connect with MGame ID.
	}

	if(1 != iResult) // Login failed..
	{
		m_pUILogIn->SetVisibleLogInUIs(true); // Successful connection..Unable to manipulate UI..
		m_pUILogIn->SetRequestedLogIn(false);
		m_bLogIn = false; // Attempting to log in...
	}
}

int CGameProcLogIn::MsgRecv_VersionCheck(DataPack* pDataPack, int& iOffset) // virtual
{
	const int iVersion = CGameProcedure::MsgRecv_VersionCheck(pDataPack, iOffset);
	if(iVersion == CURRENT_VERSION)
	{
		CGameProcedure::MsgSend_GameServerLogIn(); // Log in to the game server..
		m_pUILogIn->ConnectButtonSetEnable(false);
	}

	return true;


	return iVersion;
}

int CGameProcLogIn::MsgRecv_GameServerLogIn(DataPack* pDataPack, int& iOffset) // virtual - Returns the country code.
{
	const int iNation = CGameProcedure::MsgRecv_GameServerLogIn(pDataPack, iOffset); // Country - 0 None 0xff - Failed..

	if( 0xff == iNation )
	{
		this->MessageBoxPost("Cannot connect to server, please try again later.", "", MB_OK);
		m_pUILogIn->ConnectButtonSetEnable(true); // failure
	}
	else if (0xfe == iNation)
	{
		this->MessageBoxPost("Server is full. Please try again later.", "", MB_OK);
		m_pUILogIn->ConnectButtonSetEnable(true); // failure
	}
	else
	{
		if(0 == iNation) s_pPlayer->m_InfoBase.eNation = NATION_NOTSELECTED;
		else if(1 == iNation) s_pPlayer->m_InfoBase.eNation = NATION_KARUS;
		else if(2 == iNation) s_pPlayer->m_InfoBase.eNation = NATION_ELMORAD;
	}

	if ( NATION_NOTSELECTED == s_pPlayer->m_InfoBase.eNation )
	{
		s_pEng->s_SndMgr.ReleaseStreamObj(&(CGameProcedure::s_pSnd_BGM));
		CGameProcedure::s_pSnd_BGM = s_pEng->s_SndMgr.CreateStreamObj(ID_SOUND_BGM_EL_BATTLE);
		if(CGameProcedure::s_pSnd_BGM)
		{
			CGameProcedure::s_pSnd_BGM->Looping(true);
			CGameProcedure::s_pSnd_BGM->Play();
		}

		CGameProcedure::ProcActiveSet((CGameProcedure*)s_pProcNationSelect);
	}
	else if( NATION_KARUS == s_pPlayer->m_InfoBase.eNation || NATION_ELMORAD == s_pPlayer->m_InfoBase.eNation)
	{
		s_pEng->s_SndMgr.ReleaseStreamObj(&(CGameProcedure::s_pSnd_BGM));
		CGameProcedure::s_pSnd_BGM = s_pEng->s_SndMgr.CreateStreamObj(ID_SOUND_BGM_EL_BATTLE);
		if(CGameProcedure::s_pSnd_BGM)
		{
			CGameProcedure::s_pSnd_BGM->Looping(true);
			CGameProcedure::s_pSnd_BGM->Play();
		}

		CGameProcedure::ProcActiveSet((CGameProcedure*)s_pProcCharacterSelect);
	}

	return iNation;
}

void CGameProcLogIn::MsgRecv_NoticeList(DataPack* pDataPack, int& iOffset)
{
	int noticeCount = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	if (noticeCount > 0) {
		std::vector<std::string> vSzNotice{};
		vSzNotice.reserve(noticeCount);

		for (auto i = 0; i < noticeCount; i++) {
			std::string szNotice;
			int iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szNotice, iLen);
			vSzNotice.push_back(szNotice);
		}

		m_pUILogIn->OpenNotice(noticeCount, vSzNotice);
	}
	else {
		this->MsgSend_GameServerList();
	}
}

bool CGameProcLogIn::ProcessPacket(DataPack* pDataPack, int& iOffset)
{
	const int iOffsetPrev = iOffset;
	if(false == CGameProcedure::ProcessPacket(pDataPack, iOffset)) iOffset = iOffsetPrev;
	else return true;

	s_pPlayer->m_InfoBase.eNation = NATION_UNKNOWN;
	const int iCmd = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// Command parsing...
	s_pPlayer->m_InfoBase.eNation = NATION_UNKNOWN;
	switch ( iCmd )										// Branch according to the command..
	{
		case N3_GAMESERVER_GROUP_LIST: // It will be sent as soon as you log in.
			this->MsgRecv_GameServerGroupList(pDataPack, iOffset);
			return true;

		case N3_ACCOUNT_LOGIN: // Account access successful.
		case N3_ACCOUNT_LOGIN_MGAME: // MGame account access successful..
			this->MsgRecv_AccountLogIn(iCmd, pDataPack, iOffset);
			return true;
		case N3_NOTICE_LIST:
			this->MsgRecv_NoticeList(pDataPack, iOffset);
	}

	return false;
}

void CGameProcLogIn::ConnectToGameServer() // Connect to the game server of your choice
{
	auto rServerInfo = m_pUILogIn->ServerInfoGetSelected();

	s_bNeedReportConnectionClosed = false; // Should I report that the server is disconnected?
	const int iErr = s_pSocket->Connect(s_hWndBase, rServerInfo.m_szIP.c_str(), SOCKET_PORT_GAME); // game server socket connection
	s_bNeedReportConnectionClosed = true; // Should I report that the connection to the server has been lost?
	if(iErr)
	{
		this->ReportServerConnectionFailed(rServerInfo.m_szName.c_str(), iErr, false);
		m_pUILogIn->ConnectButtonSetEnable(true);
	}
	else
	{
		s_szServer = rServerInfo.m_szName;
		this->MsgSend_VersionCheck();
	}
}
