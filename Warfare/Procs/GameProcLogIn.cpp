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
	m_pChr			= nullptr;
	m_pTexBkg		= nullptr;

	m_pCamera = nullptr;
	for(int i = 0; i < 3; i++) m_pLights[i] = nullptr;

	m_bLogIn = false; // Avoid duplicate logins..
}

CGameProcLogIn::~CGameProcLogIn()
{
	delete m_pUILogIn;
	delete m_pChr;
	delete m_pTexBkg;

	delete m_pCamera;
	for(int i = 0; i < 3; i++) delete m_pLights[i];
}

void CGameProcLogIn::Release()
{
	CGameProcedure::Release();

	delete m_pUILogIn; m_pUILogIn = nullptr;
	delete m_pChr; m_pChr = nullptr;
	delete m_pTexBkg; m_pTexBkg = nullptr;

	delete m_pCamera; m_pCamera = nullptr;
	for(int i = 0; i < 3; i++) { delete m_pLights[i]; m_pLights[i] = nullptr; }
}

void CGameProcLogIn::Init()
{
	CGameProcedure::Init();

	m_pTexBkg = new CN3Texture();
	m_pTexBkg->LoadFromFile("Intro\\Moon.dxt");

	m_pChr = new CN3Chr();
	m_pChr->LoadFromFile("Intro\\Intro.N3Chr");
	m_pChr->AniCurSet(0); // Looping animation...
	
	m_pCamera = new CN3Camera();
	m_pCamera->EyePosSet(0.22f, 0.91f, -1.63f);
	m_pCamera->AtPosSet(-0.19f, 1.1f, 0.09f);
	m_pCamera->m_Data.fNP = 0.1f;
	m_pCamera->m_Data.fFP = 32.0f;
	m_pCamera->m_bFogUse = false;

	for(int i = 0; i < 3; i++) m_pLights[i] = new CN3Light();
	m_pLights[0]->LoadFromFile("Intro\\0.N3Light");
	m_pLights[1]->LoadFromFile("Intro\\1.N3Light");
	m_pLights[2]->LoadFromFile("Intro\\2.N3Light");

	s_pEng->s_SndMgr.ReleaseStreamObj(&(CGameProcedure::s_pSnd_BGM));
	CGameProcedure::s_pSnd_BGM = s_pEng->s_SndMgr.CreateStreamObj(35);	// The 26-second sound of a monster howling...

	m_pUILogIn = new CUILogIn();
	m_pUILogIn->Init(s_pUIMgr);

	const __TABLE_UI_RESRC* pTbl = s_pTbl_UI->GetIndexedData(0); // Because there are no national standards...
	if(pTbl) m_pUILogIn->LoadFromFile(pTbl->szLogIn);

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
			m_pUILogIn->FocusToID(); // Put the focus on the ID input window.
			
			// Game server list request..
			int iOffset = 0;
			BYTE byBuffs[4];
			CAPISocket::MP_AddByte(byBuffs, iOffset, N3_GAMESERVER_GROUP_LIST);					// command.
			s_pSocket->Send(byBuffs, iOffset);											// send
		}
	}
	else
	{
		this->MessageBoxPost("No server list", "LogIn Server fail", MB_OK, BEHAVIOR_EXIT); // finish
	}

	// If you log in with your game account...
	if(LIC_KNIGHTONLINE != s_eLogInClassification)
	{
		this->MsgSend_AccountLogIn(s_eLogInClassification); // log in..
	}
}

void CGameProcLogIn::Tick() // Returns the procedure index. If 0, proceed as is.
{
	CGameProcedure::Tick();	// keys, mouse input, etc.

	for(int i = 0; i < 3; i++) 	m_pLights[i]->Tick();
	m_pChr->Tick();

	static float fTmp	= 0;
	if(fTmp == 0)
	{
		if(CGameProcedure::s_pSnd_BGM) CGameProcedure::s_pSnd_BGM->Play(); // the music starts...
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
	const D3DCOLOR crEnv = 0x00000000;
	s_pEng->Clear(crEnv); // the background is black
	s_pEng->s_lpD3DDev->BeginScene();			// Scene renders...

	// __Vector3 vEye(0.22f, 0.91f, -1.63f), vAt(-0.19f, 1.1048f, 0.0975f), vUp(0,1,0);
	// __Matrix44 mtxView, mtxPrj, mtxWorld;
	// mtxWorld.Identity();

	 // Grab the camera...
	m_pCamera->Tick();
	m_pCamera->Apply();
	/*	D3DVIEWPORT9 vp;
	CN3Base::s_lpD3DDev->GetViewport(&vp);
	float fLens = D3DXToRadian(55.0f);
	float fAspect = (float)vp.Width / (float)vp.Height;
	float fNear = 0.1f;
	float fFar = 100.0f;

	::D3DXMatrixLookAtLH(&mtxView, &vEye, &vAt, &vUp);
	::D3DXMatrixPerspectiveFovLH(&mtxPrj, fLens, fAspect, fNear, fFar);
	CN3Base::s_lpD3DDev->SetTransform(D3DTS_VIEW, &mtxView); 
	CN3Base::s_lpD3DDev->SetTransform(D3DTS_PROJECTION, &mtxPrj); 
	CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtxWorld); 
	*/

	for(int i = 0; i < 8; i++) 	CN3Base::s_lpD3DDev->LightEnable(i, FALSE);
	for(auto i = 0; i < 3; i++) 	m_pLights[i]->Apply();

	// catch the light...
	/* D3DLIGHT9 lgt0, lgt1, lgt2;
	
	memset(&lgt0, 0, sizeof(D3DLIGHT9));
	lgt0.Type = D3DLIGHT_POINT;
	lgt0.Attenuation0 = 0.5f;
	lgt0.Range = 100000.0f;
	lgt0.Position = __Vector3(-500, 100, -50);
	lgt0.Diffuse.r = 232/255.0f; lgt0.Diffuse.g = 226/255.0f; lgt0.Diffuse.b = 215/255.0f;

	memset(&lgt1, 0, sizeof(D3DLIGHT9));
	lgt1.Type = D3DLIGHT_POINT;
	lgt1.Attenuation0 = 1.0f;
	lgt1.Range = 100000.0f;
	lgt1.Position = __Vector3(1000, -300, -50);
	// lgt1.Ambient.r = 56/255.0f; lgt1.Ambient.g = 58/255.0f; lgt1.Ambient.b = 129/255.0f;
	lgt1.Diffuse.r = 66/255.0f; lgt1.Diffuse.g = 68/255.0f; lgt1.Diffuse.b = 168/255.0f;

	memset(&lgt2, 0, sizeof(D3DLIGHT9));
	lgt2.Type = D3DLIGHT_POINT;
	lgt2.Attenuation0 = 1.0f;
	lgt2.Range = 100000.0f;
	lgt2.Position = __Vector3(-200, -800, 350);
	// lgt2.Ambient.r = 52/255.0f; lgt2.Ambient.g = 56/255.0f; lgt2.Ambient.b = 107/255.0f;
	lgt2.Diffuse.r = 124/255.0f; lgt2.Diffuse.g = 45/255.0f; lgt2.Diffuse.b = 31/255.0f;

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	for(int i = 0; i < 8; i++)
		CN3Base::s_lpD3DDev->LightEnable(0, FALSE);
	CN3Base::s_lpD3DDev->LightEnable(0, TRUE);
	CN3Base::s_lpD3DDev->LightEnable(1, TRUE);
	CN3Base::s_lpD3DDev->LightEnable(2, TRUE);
	CN3Base::s_lpD3DDev->SetLight(0, &lgt0);
	CN3Base::s_lpD3DDev->SetLight(1, &lgt1);
	CN3Base::s_lpD3DDev->SetLight(2, &lgt2);
	*/

	// 
	// drawing the moon...
	D3DVIEWPORT9 vp;
	CN3Base::s_lpD3DDev->GetViewport(&vp);

	const float fMW = (m_pTexBkg->Width() * vp.Width / 1024.0f)*1.3f;
	const float fMH = (m_pTexBkg->Height() * vp.Height / 768.0f)*1.3f;
	const float fX = 100.0f * vp.Width / 1024.0f;
	const float fY = 50.0f * vp.Height / 768.0f;

	const float fRHW = 1.0f;
	__VertexTransformed vMoon[4];
	vMoon[0].Set(fX,     fY,     0, fRHW, 0xffffffff, 0.0f, 0.0f);
	vMoon[1].Set(fX+fMW, fY,     0, fRHW, 0xffffffff, 1.0f, 0.0f);
	vMoon[2].Set(fX+fMW, fY+fMH, 0, fRHW, 0xffffffff, 1.0f, 1.0f);
	vMoon[3].Set(fX,     fY+fMH, 0, fRHW, 0xffffffff, 0.0f, 1.0f);
	
	DWORD dwZWrite;
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWrite);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	CN3Base::s_lpD3DDev->SetTexture(0, m_pTexBkg->Get());
	CN3Base::s_lpD3DDev->SetFVF(FVF_TRANSFORMED);
	CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vMoon, sizeof(__VertexTransformed));

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWrite);
	// drawing the moon...
	// 

	
	m_pChr->Render(); // Drawing a character...

	CGameProcedure::Render(); // Render UI and other basic stuff..

	s_pEng->s_lpD3DDev->EndScene();			// Start scene render...
	s_pEng->Present(CN3Base::s_hWndBase);
}

bool CGameProcLogIn::MsgSend_AccountLogIn(e_LogInClassification eLIC)
{
	if(LIC_KNIGHTONLINE == eLIC) 
	{
		m_pUILogIn->AccountIDGet(s_szAccount); // Remember account...
		m_pUILogIn->AccountPWGet(s_szPassWord); // Remember password...
	}
	if(	s_szAccount.empty() || s_szPassWord.empty() || s_szAccount.size() >= 20 || s_szPassWord.size() >= 12) return false;

	m_pUILogIn->SetVisibleLogInUIs(false); // Disable the UI until packets arrive...
	m_pUILogIn->SetRequestedLogIn(true);
	m_bLogIn = true; // Attempting to log in...

	BYTE byBuff[256];										// Packet buffer...
	int iOffset=0;										// Offset of buffer..

	BYTE byCmd = N3_ACCOUNT_LOGIN;
	if(LIC_KNIGHTONLINE == eLIC) byCmd = N3_ACCOUNT_LOGIN;
	else if(LIC_MGAME == eLIC) byCmd = N3_ACCOUNT_LOGIN_MGAME;
	// else if(LIC_DAUM == eLIC) byCmd = N3_ACCOUNT_LOGIN_DAUM;

	CAPISocket::MP_AddByte(byBuff, iOffset, byCmd);				// command.
	CAPISocket::MP_AddShort(byBuff, iOffset, s_szAccount.size());	// id length.
	CAPISocket::MP_AddString(byBuff, iOffset, s_szAccount);		// real id.
	CAPISocket::MP_AddShort(byBuff, iOffset, s_szPassWord.size());	// password length
	CAPISocket::MP_AddString(byBuff, iOffset, s_szPassWord);		// real password
		
	s_pSocket->Send(byBuff, iOffset);								// send

	return true;
}

void CGameProcLogIn::MsgRecv_GameServerGroupList(DataPack* pDataPack, int& iOffset) const
{
	const int iServerCount = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// number of servers
	for(int i = 0; i < iServerCount; i++)
	{
		__GameServerInfo GSI;
		int iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, GSI.szIP, iLen);
		iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, GSI.szName, iLen);
		GSI.iConcurrentUserCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Current number of concurrent users..
		
		m_pUILogIn->ServerInfoAdd(GSI); // ServerList
	}

	m_pUILogIn->ServerInfoUpdate();
}

void CGameProcLogIn::MsgRecv_AccountLogIn(int iCmd, DataPack* pDataPack, int& iOffset)
{
	int iResult = CAPISocket::Parse_GetByte( pDataPack->m_pData, iOffset ); // Recv - b1 (0: Failure 1: Success 2: No ID 3: PW Wrong 4: Server Checking)
	if(1 == iResult) // Connection successful..
	{
		// Close all message boxes...
		this->MessageBoxClose(-1);
		m_pUILogIn->OpenServerList(); // Read server list...
	}
	else if(2 == iResult) // If you fail because you don't have an ID...
	{
		if(N3_ACCOUNT_LOGIN == iCmd)
		{
			std::string szMsg;
			std::string szTmp;
			::_LoadStringFromResource(IDS_NOACCOUNT_RETRY_MGAMEID, szMsg);
			::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);

			this->MessageBoxPost(szMsg, szTmp, MB_YESNO, BEHAVIOR_MGAME_LOGIN); // It asks if you want to connect with MGame ID.
		}
		else
		{
			std::string szMsg;
			std::string szTmp;
			::_LoadStringFromResource(IDS_NO_MGAME_ACCOUNT, szMsg);
			::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);

			this->MessageBoxPost(szMsg, szTmp, MB_OK); // It asks if you want to connect with MGame ID.
		}
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
		__GameServerInfo GSI;
		std::string szFmt; ::_LoadStringFromResource(IDS_FMT_GAME_SERVER_LOGIN_ERROR, szFmt);
		m_pUILogIn->ServerInfoGetCur(GSI);
		char szErr[256];
		sprintf(szErr, szFmt.c_str(), GSI.szName.c_str(), iNation);
		this->MessageBoxPost(szErr, "", MB_OK);
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
	}

	return false;
}

void CGameProcLogIn::ConnectToGameServer() // Connect to the game server of your choice
{
	__GameServerInfo GSI;
	if(false == m_pUILogIn->ServerInfoGetCur(GSI)) return; // After choosing a server...

	s_bNeedReportConnectionClosed = false; // Should I report that the server is disconnected?
	const int iErr = s_pSocket->Connect(s_hWndBase, GSI.szIP.c_str(), SOCKET_PORT_GAME); // game server socket connection
	s_bNeedReportConnectionClosed = true; // Should I report that the connection to the server has been lost?
	if(iErr)
	{
		this->ReportServerConnectionFailed(GSI.szName, iErr, false);
		m_pUILogIn->ConnectButtonSetEnable(true);
	}
	else
	{
		s_szServer = GSI.szName;
		this->MsgSend_VersionCheck();
	}
}

/*
void CGameProcLogIn::PacketSend_MGameLogin()
{
	if(m_szID.size() >= 20 || m_szPW.size() >= 12)
	{
	// MessageBox("ID must be 20 characters, PassWord must be less than 12 characters.", "LogIn Error");
		return;
	}

	int send_index = 0;
	BYTE send_buff[128];
	memset( send_buff, NULL, 128 );

	CAPISocket::MP_AddByte( send_buff, send_index, N3_ACCOUNT_LOGIN_MGAME); // Send - s1 (ID length) str1 (ID string: 20 bytes or less) s1 (PW length) str1 (PW string: 12 bytes or less) | Recv - b1 (0: Failure 1: Success 2: No ID 3: PW Wrong 4: Server Checking)
	CAPISocket::MP_AddShort( send_buff, send_index, (short)(m_szID.size()));
	CAPISocket::MP_AddString( send_buff, send_index, m_szID);
	CAPISocket::MP_AddShort( send_buff, send_index, (short)(m_szPW.size()));
	CAPISocket::MP_AddString( send_buff, send_index, m_szPW);

	s_pSocket->Send( send_buff, send_index );
}*/
