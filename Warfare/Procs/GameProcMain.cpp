#include "pch.h"

#include <io.h>

#include <lz4.h>
#include <crc32c/crc32c.h>

#include "GameEng.h"
#include "GameProcMain.h"
#include "LocalInput.h"

#include "APISocket.h"
#include "PacketDef.h"

#include "PlayerMySelf.h"
#include "PlayerOtherMgr.h"
#include "ServerMesh.h"
#include "N3FXMgr.h"

#include "UIManager.h"
#include "UILoading.h"
#include "UIChat.h"
#include "UIInventory.h"
#include "UICmd.h"
#include "UIVarious.h"
#include "UIStateBar.h"
#include "UITargetBar.h"
#include "UITransactionDlg.h"
#include "UIHelp.h"
#include "UIMessageWnd.h"
#include "UINotice.h"
#include "UIDroppedItemDlg.h"
#include "UIPerTradeDlg.h"
#include "UIPartyOrForce.h"
#include "UISkillTreeDlg.h"
#include "UIHotKeyDlg.h"
#include "UIClassChange.h"
#include "UINpcEvent.h"
#include "UIItemExchange.h"
#include "UIRepairTooltipDlg.h"
#include "UINpcTalk.h"
#include "UIKnightsOperation.h"
#include "UIPartyBBS.h"
#include "UIWareHouseDlg.h"
#include "UINPCChangeEvent.h"
#include "UIWarp.h"
#include "UIInn.h"
#include "UICreateClanName.h"
#include "UIPartyBBS.h"
#include "UITradeSellBBS.h"
#include "UITradeBBSSelector.h"
#include "UITradeBBSEditDlg.h"
#include "UIQuestMenu.h"
#include "UIQuestTalk.h"
#include "UIDead.h"

#include "SubProcPerTrade.h"
#include "CountableItemEditDlg.h"
#include "MagicSkillMng.h"
#include "WarMessage.h"
#include "GameCursor.h"

#include "N3WorldManager.h"

#include "LightMgr.h"

#include "N3SkyMng.h"
#include "N3ShapeExtra.h"
#include "N3Camera.h"
#include "N3SndObj.h"
#include "N3SndObjStream.h"
#include "N3SndMgr.h"
#include "N3TableBase.h"
#include <LogWriter.h>

enum e_ChatCmd { 	CMD_WHISPER, CMD_TOWN, CMD_TRADE, CMD_EXIT, CMD_PARTY,
					CMD_LEAVEPARTY, CMD_RECRUITPARTY, CMD_JOINCLAN, CMD_WITHDRAWCLAN, CMD_FIRECLAN, 
					CMD_APPOINTVICECHIEF, CMD_GREETING, CMD_EXCITE, CMD_VISIBLE, CMD_INVISIBLE, 
					CMD_CLEAN, CMD_RAINING, CMD_SNOWING, CMD_TIME, CMD_CU_COUNT,
					CMD_NOTICE, CMD_ARREST, CMD_FORBIDCONNECT, CMD_FORBIDCHAT, CMD_PERMITCHAT,
					CMD_GAME_SAVE, 
					CMD_COUNT,
					CMD_UNKNOWN = 0xffffffff };
static std::string s_szCmdMsg[CMD_COUNT]; // in-game commands

CGameProcMain::CGameProcMain()				// r Default constructor.. Refer to the header for the role of each variable..
{	
	m_fLBClickTime = 0.0f;
	m_bLoadComplete	= FALSE;
	m_fRequestGameSave = 300.0f;

	// sound obj...
	m_pSnd_Town = nullptr;
	m_pSnd_Battle = nullptr;

	m_iJoinReqClan = 0;
	m_iJoinReqClanRequierID = 0;

	// UI
	m_pUIMsgDlg = new CUIMessageWnd;
	m_pUIChatDlg = new CUIChat();
	m_pUIStateBarAndMiniMap = new CUIStateBar();
	m_pUIVar = new CUIVarious();
	m_pUICmd = new CUICmd();
	m_pUITargetBar = new CUITargetBar();
	m_pUIHelp = new CUIHelp();
	m_pUINotice = new CUINotice();
	m_pUIClassChange = new CUIClassChange();
	m_pUINpcEvent = new CUINPCEvent();
	m_pUIItemREDlg = new CUIItemExchange();
	m_pUIRepairTooltip = new CUIRepairTooltipDlg();
	m_pUIDroppedItemDlg = new CUIDroppedItemDlg();
	m_pUITransactionDlg = new CUITransactionDlg();
	m_pUIInventory = new CUIInventory();
	m_pUIPartyOrForce = new CUIPartyOrForce();
	m_pUISkillTreeDlg = new CUISkillTreeDlg();
	m_pUIHotKeyDlg = new CUIHotKeyDlg();
	m_pUINpcTalk = new CUINpcTalk();
	m_pUIKnightsOp = new CUIKnightsOperation();			// View the list of guilds, join, etc...
	m_pUIPartyBBS = new CUIPartyBBS(); // Party support system bulletin board??..
	m_pUIWareHouseDlg = new CUIWareHouseDlg();
	m_pUINpcChange = new CUINPCChangeEvent();	
	m_pUIWarp = new CUIWarp();
	m_pUIInn = new CUIInn();
	m_pUICreateClanName = new CUICreateClanName();
	m_pUITradeBBS = new CUITradeSellBBS();
	m_pUITradeBBSSelector = new CUITradeBBSSelector();	
	m_pUITradeBBSEdit = new CUITradeBBSEditDlg();
	m_pUIQuestMenu = new CUIQuestMenu();
	m_pUIQuestTalk = new CUIQuestTalk();
	m_pUIDead = new CUIDead();

	m_pSubProcPerTrade = new CSubProcPerTrade();
	m_pMagicSkillMng = new CMagicSkillMng(this);
	m_pTargetSymbol = new CN3Shape(); // The player draws on the location of the target character.
	m_pWarMessage = new CWarMessage;

	m_pLightMgr = new CLightMgr;
}

CGameProcMain::~CGameProcMain()
{
	CGameProcMain::Release();

	// UI
	delete m_pUIMsgDlg;
	delete m_pUIChatDlg;
	delete m_pUIStateBarAndMiniMap;
	delete m_pUIVar;
	delete m_pUICmd;
	delete m_pUITargetBar;
	delete m_pUIHelp;
	delete m_pUINotice;
	delete m_pUIClassChange;
	delete m_pUINpcEvent;
	delete m_pUIItemREDlg;
	delete m_pUIRepairTooltip;
	delete m_pUIDroppedItemDlg;
	delete m_pUITransactionDlg;
	delete m_pUIInventory;
	delete m_pUIPartyOrForce;
	delete m_pUISkillTreeDlg;
	delete m_pUIHotKeyDlg;
	delete m_pUINpcTalk;
	delete m_pUIKnightsOp;
	delete m_pUIPartyBBS;
	delete m_pUIWareHouseDlg;
	delete m_pUINpcChange;
	delete m_pUIWarp;
	delete m_pUIInn;
	delete m_pUICreateClanName;
	delete m_pUITradeBBS;
	delete m_pUITradeBBSSelector;
	delete m_pUITradeBBSEdit;
	delete m_pUIQuestMenu;
	delete m_pUIQuestTalk;
	delete m_pUIDead;

	delete m_pSubProcPerTrade;
	delete m_pMagicSkillMng;
	delete m_pWarMessage;
	delete m_pTargetSymbol; // The player draws on the location of the target character.

	delete m_pLightMgr;
}

void CGameProcMain::Release()
{
	this->ReleaseSound();
	this->ReleaseUIs();

	m_pSubProcPerTrade->Release();
	m_pMagicSkillMng->Release();
	m_pWarMessage->Release();
	m_pTargetSymbol->Release();
	m_pLightMgr->Release();

	CGameProcedure::Release();
}

void CGameProcMain::ReleaseUIs() const
{
	m_pUIChatDlg->Release();
	m_pUIMsgDlg->Release();
	m_pUICmd->Release();
	m_pUIVar->Release();
	m_pUIStateBarAndMiniMap->Release();
	m_pUITargetBar->Release();
	m_pUIHelp->Release();
	m_pUINotice->Release();
	m_pUIClassChange->Release();
	m_pUINpcEvent->Release();
	m_pUIItemREDlg->Release();
	m_pUIRepairTooltip->Release();
	m_pUIPartyOrForce->Release();
	m_pUISkillTreeDlg->Release();
	m_pUIHotKeyDlg->Release();
	m_pUINpcTalk->Release();
	// m_pUITradeList->Release();
	m_pUIKnightsOp->Release();			// View the list of guilds, join, etc...
	m_pUIPartyBBS->Release();
	m_pUIWareHouseDlg->Release();
	m_pUINpcChange->Release();
	m_pUIWarp->Release();
	m_pUIInn->Release();
	m_pUICreateClanName->Release();

	CN3UIBase::DestroyTooltip();
}

void CGameProcMain::Init()
{
	CGameProcedure::Init();
	m_pLightMgr->Release();
	s_pEng->SetDefaultLight(m_pLightMgr->Light(0), m_pLightMgr->Light(1), m_pLightMgr->Light(2));

	for(auto i = IDS_CMD_WHISPER ; i <= IDS_CMD_GAME_SAVE ; i++ ) // Command loading...
	{
		::_LoadStringFromResource(i, s_szCmdMsg[i - IDS_CMD_WHISPER]);
	}

	s_SndMgr.ReleaseStreamObj(&(CGameProcedure::s_pSnd_BGM));

	if(m_pWarMessage) m_pWarMessage->InitFont();
	this->InitUI(); // Different UI loading for different countries...
	this->InitZone(s_pPlayer->m_InfoExt.iZoneCur, s_pPlayer->Position()); // John Loading...

	// sound obj...
	if(m_pSnd_Battle== nullptr)
	{
		int iIDSndBattle = ((NATION_KARUS == s_pPlayer->m_InfoBase.eNation) ? ID_SOUND_BGM_KA_BATTLE : ID_SOUND_BGM_EL_BATTLE);
		m_pSnd_Battle = s_pEng->s_SndMgr.CreateStreamObj(iIDSndBattle);	// Battle Music ID
		if(m_pSnd_Battle) 
		{
			m_pSnd_Battle->Looping(true);
			m_pSnd_Battle->Stop();
		}
	}
	if(m_pSnd_Town== nullptr)
	{
		m_pSnd_Town = s_pEng->s_SndMgr.CreateStreamObj(ID_SOUND_BGM_TOWN);	// Village Music ID
		if(m_pSnd_Town)
		{
			m_pSnd_Town->Looping(true);
			m_pSnd_Town->Play(nullptr, 3.0f);
		}
	}

	if(s_pUILoading) s_pUILoading->Render("Loading Character Data...", 0);

	// remember the path...
	char szPathOld[_MAX_PATH], szPathFind[_MAX_PATH];
	::GetCurrentDirectory(_MAX_PATH, szPathOld);

	_finddata_t fi;
	long hFind = -1;

	// Read all resources...
	// Read all the anime...
	lstrcpy(szPathFind, szPathOld);
	lstrcat(szPathFind, "\\Chr");
	::SetCurrentDirectory(szPathFind);
	hFind = _findfirst("*.N3Anim", &fi);
	if(hFind)
	{
		std::string szFN = "Chr\\";
		szFN += fi.name;
		CN3AnimControl* pObjTmp = s_MngAniCtrl.Get(szFN.c_str());
		while(_findnext(hFind, &fi) != -1)
		{
			szFN = "Chr\\";
			szFN += fi.name;
			pObjTmp = s_MngAniCtrl.Get(szFN.c_str());
		}
	}
	_findclose(hFind);

	if(s_pUILoading) s_pUILoading->Render("Loading Character Data... 10 %", 10);

	// Read all resources...
	// Read all textures.
	lstrcpy(szPathFind, szPathOld);
	lstrcat(szPathFind, "\\Item");
	::SetCurrentDirectory(szPathFind);
	hFind = _findfirst("*.dxt", &fi);
	if(hFind)
	{
		std::string szFN = "Item\\";
		szFN += fi.name;
		CN3Texture* pObjTmp = s_MngTex.Get(szFN.c_str());
		while(_findnext(hFind, &fi) != -1)
		{
			szFN = "Item\\";
			szFN += fi.name;
			pObjTmp = s_MngTex.Get(szFN.c_str());
		}
	}
	_findclose(hFind);

	if(s_pUILoading) s_pUILoading->Render("Loading Character Data... 25 %", 25);
	
	// Read all resources...
	// Read all the joints...
	lstrcpy(szPathFind, szPathOld);
	lstrcat(szPathFind, "\\Chr");
	::SetCurrentDirectory(szPathFind);
	hFind = _findfirst("*.N3Joint", &fi);
	if(hFind)
	{
		std::string szFN = "Chr\\";
		szFN += fi.name;
		CN3Joint* pObjTmp = s_MngJoint.Get(szFN.c_str());
		while(_findnext(hFind, &fi) != -1)
		{
			szFN = "Chr\\";
			szFN += fi.name;
			pObjTmp = s_MngJoint.Get(szFN.c_str());
		}
	}
	_findclose(hFind);

	if(s_pUILoading) s_pUILoading->Render("Loading Character Data... 50 %", 50);

	// Read all resources...
	// Read the skin...
	lstrcpy(szPathFind, szPathOld);
	lstrcat(szPathFind, "\\Item");
	::SetCurrentDirectory(szPathFind);
	hFind = _findfirst("*.N3CSkins", &fi);
	if(hFind)
	{
		std::string szFN = "Item\\";
		szFN += fi.name;
		CN3CPartSkins* pObjTmp = s_MngSkins.Get(szFN.c_str());
		while(_findnext(hFind, &fi) != -1)
		{
			szFN = "Item\\";
			szFN += fi.name;
			pObjTmp = s_MngSkins.Get(szFN.c_str());
		}
	}
	_findclose(hFind);

	if(s_pUILoading) s_pUILoading->Render("Loading Character Data... 75 %", 75);
	
	// Read all resources...
	// Reading PMesh...
	lstrcpy(szPathFind, szPathOld);
	lstrcat(szPathFind, "\\Item");
	::SetCurrentDirectory(szPathFind);
	hFind = _findfirst("*.N3PMesh", &fi);
	if(hFind)
	{
		std::string szFN = "Item\\";
		szFN += fi.name;
		CN3PMesh* pObjTmp = s_MngPMesh.Get(szFN.c_str());
		while(_findnext(hFind, &fi) != -1)
		{
			szFN = "Item\\";
			szFN += fi.name;
			pObjTmp = s_MngPMesh.Get(szFN.c_str());
		}
	}
	_findclose(hFind);

	if(s_pUILoading) s_pUILoading->Render("Loading Character Data... 100 %", 100);

	this->MsgSend_GameStart();
	// turn the path...
	::SetCurrentDirectory(szPathOld);

}

void CGameProcMain::InitPlayerPosition(const __Vector3& vPos) // Initialize the player position.. Raise him up and make him take the basic action.
{
	__Vector3 vPosFinal = vPos;
	const float fYTerrain = ACT_WORLD->GetHeightWithTerrain(vPos.x, vPos.z);	// Get the height of the terrain...
	const float fYObject = ACT_WORLD->GetHeightNearstPosWithShape(vPos, 1.0f); // Get the height closest to the object.
	if (!s_pWorldMgr->IsIndoor())
	{
		if (T_Abs(vPos.y - fYObject) < T_Abs(vPos.y - fYTerrain)) vPosFinal.y = fYObject; // put it closer...
		else vPosFinal.y = fYTerrain;
	}
	else
	{
		if (fYObject > fYTerrain)
			vPosFinal.y = fYObject;
		else
			vPosFinal.y = fYTerrain;
	}

	s_pPlayer->PositionSet(vPosFinal, true);	// Set character position...
	s_pPlayer->m_vPosFromServer = vPos;
	m_vPlayerPosSended = vPos;					// Recently sent location settings..
	m_fMsgSendTimeMove = 0;						// record the time...

	this->CommandSitDown(false, false, true); // Stand up.. If you warp while sitting... there is a bug..
	this->TargetSelect(-1, false); // untarget..
	this->UpdateCameraAndLight(); // Camera and light recalculation..

	s_pPlayer->Action(PSA_BASIC, true, nullptr, true); // Forced default posture..
}

void CGameProcMain::Tick()
{
	CGameProcedure::Tick();	// keys, mouse input, etc.

	if ( FALSE == m_bLoadComplete ) return;				// If it doesn&#39;t load... go back.
	if(!s_pSocket->IsConnected()) return;

#ifdef _DEBUG
	if(s_pLocalInput->IsKeyPressed(DIK_F8))
	{
		BYTE byBuff[32];
		int iOffset = 0;
		CAPISocket::MP_AddByte(byBuff, iOffset, N3_TEMP_TEST);
		s_pSocket->Send(byBuff, iOffset);


		const int iNPC = s_pOPMgr->m_NPCs.size();
		char szBuff[1024];
		sprintf(szBuff, "NPC Region Test : %d", iNPC);
		CLogWriter::Write(szBuff);
		auto it = s_pOPMgr->m_NPCs.begin(), itEnd = s_pOPMgr->m_NPCs.end();
		for(; it != itEnd; it++)
		{
			CPlayerNPC* pNPC = it->second;

			sprintf(szBuff, "    ID(%d) Name(%s) Pos(%.1f, %.1f)",
				pNPC->IDNumber(),
				pNPC->IDString().c_str(), 
				pNPC->m_vPosFromServer.x,
				pNPC->m_vPosFromServer.z );
			CLogWriter::Write(szBuff);
		}

	}
#endif

	const DWORD dwMouseFlags = s_pLocalInput->MouseGetFlag();	// Mouse button flags - see LocalInput.h
	this->ProcessLocalInput(dwMouseFlags);					// Keyboard or mouse input is processed after the UI...

	MsgSend_Continous();									// It checks the movement, rotation value, attack, etc. at regular intervals and creates and sends packets.

	s_pPlayer->Tick();									// Player Tick (Update)
	s_pWorldMgr->Tick();
	s_pOPMgr->Tick(s_pPlayer->Position());				// Tick another user manager (update)
	// s_pFX-&gt;Tick(); //If the camera value is used internally, an error occurs in the position, so move it into the Render() function...

	const __Vector3 ListenerPos = s_pPlayer->Position();
	const __Vector3 ListenerDir = s_pPlayer->Direction();
	const __Vector3 ListenerUp(0,1,0);

	// Sound Tick...
	CN3SndObj::SetListenerPos(&ListenerPos);
	CN3SndObj::SetListenerOrientation(&ListenerDir, &ListenerUp);

	this->UpdateUI_MiniMap(); // Minimap update...
	this->UpdateUI_TargetBar(); // Target bar processing...
	this->UpdateBGM(); // Background music is processed according to the situation.
	this->UpdateCameraAndLight(); // Camera and light processing.
	
	// ProcessPlayerInclination(); // Slope handling.. (Slipping if the slope is steep even when standing still..).
#ifdef _N3_64GRID_
	m_SMesh.Tick(s_pPlayer, &m_Terrain);				// Server mesh ticks. (Updates)
#endif

	m_pUIStateBarAndMiniMap->UpdatePosition(s_pPlayer->Position(), s_pPlayer->Yaw()); // location update.

	if(m_pMagicSkillMng) m_pMagicSkillMng->Tick();
	if(m_pWarMessage) m_pWarMessage->Tick();
	if(m_pLightMgr) m_pLightMgr->Tick();
	
	// 
	// If no packets have been sent, N3_TIME_NOTIFY is sent once every 2 seconds.
	const float fTime = CN3Base::TimeGet();
	static float fTimePrev = fTime;
	
	static float fTimeInterval1 = 0;
	if(0 == s_pSocket->m_iSendByteCount)
	{
		fTimeInterval1 += fTime - fTimePrev;
		if(fTimeInterval1 >= 2.0f)
		{
			BYTE byCmd = N3_TIME_NOTIFY;
			s_pSocket->Send(&byCmd, 1);
			s_pSocket->m_iSendByteCount = 0;
			fTimeInterval1 = 0;
		}
	}
	else
	{
		s_pSocket->m_iSendByteCount = 0;
		fTimeInterval1 = 0;
	}
	// If no packets have been sent, N3_TIME_NOTIFY is sent once every 2 seconds.
	// 

	// 
	// Timer-like routine..
	static float fInterval2 = 0, fInterval3 = 0, fInterval4 = 0, fInterval5 = 0;
	fInterval2 += fTime - fTimePrev;
	fInterval3 += fTime - fTimePrev;
	fInterval4 += fTime - fTimePrev;
	fInterval5 += fTime - fTimePrev;
	m_fRequestGameSave += fTime - fTimePrev;
	if(fInterval2 > 1200.0f) // save request...
	{
		BYTE byBuff[4];												// buffer..
		int iOffset=0;												// Offset...
		s_pSocket->MP_AddByte(byBuff, iOffset, N3_REQUEST_GAME_SAVE);	// Save request command..
		s_pSocket->Send(byBuff, iOffset);				// sent...

		fInterval2 = 0.0f;
	}
	if(fInterval3 > 10.0f) // Speed hack check..
	{
		MsgSend_SpeedCheck();										// Check speed hack

		fInterval3 = 0.0f;
	}
	if(s_pPlayer->m_InfoBase.iLevel < 12 && fInterval4 > 20.0f) // Show tips one by one as time passes.
	{
		std::string szMsg;
		::_LoadStringFromResource(IDS_HELP_TIP_ALL, szMsg);
		this->m_pUIMsgDlg->AddMsg(szMsg, 0xffffff00); // Show Help...
		::_LoadStringFromResource(IDS_HELP_TIP1 + rand()%30, szMsg);
		this->m_pUIMsgDlg->AddMsg(szMsg, 0xffffff00); // Show Help...
		fInterval4 = 0;
	}
	if(fInterval5 > 5.0f) // Show tips one by one as time passes.
	{
	// m_pUIChatDlg-&gt;ChangeChattingMode(N3_CHAT_CONTINUE); // Forcibly change chat mode...
		m_pUIChatDlg->ShowContinueMsg();
		fInterval5 = 0;
	}
	fTimePrev = fTime;
	// Timer-like routine..
	// 
}

void CGameProcMain::Render()
{
	if ( FALSE == m_bLoadComplete )	return; 		// Is loading finished??

	const D3DCOLOR crSky = ACT_WORLD->GetSkyColorWithSky();
	s_pEng->Clear(crSky); // Clear with fog color.. -&gt; Clear with sky color to see the sky properly..
	s_pEng->s_lpD3DDev->BeginScene();			// Scene renders...
	
	ACT_WORLD->RenderSky();								// Sky render...
	const float fSunAngle = ACT_WORLD->GetSunAngleByRadinWithSky(); // Get the angle of the sun...

	const DWORD dwFilter = D3DTEXF_LINEAR;
	CN3Base::s_lpD3DDev->SetSamplerState( 0, D3DSAMP_MINFILTER, dwFilter );		// Prevents pixels from looking broken when the texture is reduced
	CN3Base::s_lpD3DDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, dwFilter );		// Prevent pixels from appearing broken when the texture is stretched
	CN3Base::s_lpD3DDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, dwFilter );		// Prevents pixels from looking broken when the texture is reduced
	CN3Base::s_lpD3DDev->SetSamplerState( 1, D3DSAMP_MINFILTER, dwFilter );		// Prevents pixels from looking broken when the texture is reduced
	CN3Base::s_lpD3DDev->SetSamplerState( 1, D3DSAMP_MAGFILTER, dwFilter );		// Prevent pixels from appearing broken when the texture is stretched
	CN3Base::s_lpD3DDev->SetSamplerState( 1, D3DSAMP_MIPFILTER, dwFilter );		// Prevents pixels from looking broken when the texture is reduced

	ACT_WORLD->RenderTerrain();						// Terrain render...
	ACT_WORLD->RenderShape();						// object render..
	s_pOPMgr->Render(fSunAngle);				// Another player render...
	s_pPlayer->Render(fSunAngle);			// Player render...

#ifdef _DEBUG
	auto vColPos = s_pPlayer->Position();
	ACT_WORLD->RenderCollisionWithShape(vColPos);				// Collision mesh render...
#endif

#ifdef _N3_64GRID_
	m_SMesh.Render();							// Server mesh render...
#endif

	this->RenderTarget();						// Render the target character or object.

	ACT_WORLD->RenderGrass();						// full render (asm)
	s_pFX->Tick();
	s_pFX->Render();
	ACT_WORLD->RenderBirdMgr();

	CN3Base::s_AlphaMgr.Render(); // Render alpha-sorted polygons.
	
	ACT_WORLD->RenderSkyWeather();							// Sky render...
	
	CGameProcedure::Render(); // Render UI and other basic stuff..
	if(m_pWarMessage) m_pWarMessage->RenderMessage();
	if(s_pGameCursor) s_pGameCursor->Render();

	s_pEng->s_lpD3DDev->EndScene();
	s_pEng->Present(CN3Base::s_hWndBase);
}

void CGameProcMain::RenderTarget()
{
	if(nullptr == m_pTargetSymbol) return;

	// The player draws on the location of the target character.
	CPlayerBase* pTarget = s_pOPMgr->CharacterGetByID(s_pPlayer->m_iIDTarget, false);// Until it is judged as a corpse, the character gives focus.
	if(nullptr == pTarget && nullptr == s_pPlayer->m_pObjectTarget) return;

	float fScale = 1;
	float fYScale = 1;
	__Vector3 vPos(0,0,0);

	if(pTarget)
	{
		fYScale = pTarget->Height() * 1.3f;
		fScale = pTarget->Radius() * 2.0f;
		vPos = pTarget->Position();
	}
	else 
	{
		__Vector3 vTmp = s_pPlayer->m_pObjectTarget->Max() - s_pPlayer->m_pObjectTarget->Min();
		fYScale = vTmp.y;
		vTmp.y = 0;
		fScale = vTmp.Magnitude();
		vPos = s_pPlayer->m_pObjectTarget->Pos();
	}

	m_pTargetSymbol->ScaleSet(fScale, fYScale, fScale);
	m_pTargetSymbol->PosSet(vPos);
	m_pTargetSymbol->Tick();
	if(m_pTargetSymbol->Part(1)) // Set the bottom cymbals above the ground.
	{
		const CN3PMesh* pPMesh = m_pTargetSymbol->Part(1)->Mesh();
		if(pPMesh && pPMesh->GetMaxNumVertices() == 4)
		{
			__VertexT1* pVs = m_pTargetSymbol->Part(1)->Mesh()->GetVertices();
			for(int i = 0; i < 4; i++)
			{
				pVs[i].y =	ACT_WORLD->GetHeightWithTerrain(vPos.x + (pVs[i].x * fYScale), vPos.z + (pVs[i].z * fYScale));
				pVs[i].y -= vPos.y;
				pVs[i].y /= fYScale;
				pVs[i].y += 0.1f / fYScale;
			}
		}
	}
	m_pTargetSymbol->Render();


#ifdef _DEBUG
	if(pTarget) pTarget->RenderCollisionMesh();
	if(s_pPlayer->m_pObjectTarget) s_pPlayer->m_pObjectTarget->RenderCollisionMesh();
#endif
}

bool CGameProcMain::ProcessPacket(DataPack* pDataPack, int& iOffset)
{
	const int iOffsetPrev = iOffset;
	if(false == CGameProcedure::ProcessPacket(pDataPack, iOffset)) iOffset = iOffsetPrev;
	else return true;

	const int iCmd = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// Command parsing...

	switch ( iCmd )										// Branch according to the command..
	{



#ifdef _DEBUG
	case N3_TEMP_TEST:
		{
			const int iNPC = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			char szBuff[32];
			sprintf(szBuff, "NPC Region Test : %d -> ", iNPC);
			std::string szLog = szBuff;
			for(int i = 0; i < iNPC; i++)
			{
				const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
				sprintf(szBuff, "%d, ", iID);
				szLog += szBuff;
			}
			CLogWriter::Write(szLog.c_str());
		}
		return true;
#endif


		case N3_MYINFO:									// My info message...
			this->MsgRecv_MyInfo_All(pDataPack, iOffset);
			return true;
		case N3_HP_CHANGE:
			this->MsgRecv_MyInfo_HP(pDataPack, iOffset);
			return true;
		case N3_MSP_CHANGE:
			this->MsgRecv_MyInfo_MSP(pDataPack, iOffset);
			return true;
		case N3_EXP_CHANGE:
			this->MsgRecv_MyInfo_EXP(pDataPack, iOffset);
			return true;
		case N3_REALM_POINT_CHANGE: // National Contribution..
			this->MsgRecv_MyInfo_RealmPoint(pDataPack, iOffset);
			return true;
		case N3_LEVEL_CHANGE:
			this->MsgRecv_MyInfo_LevelChange(pDataPack, iOffset);
			return true;
		case N3_POINT_CHANGE:
			this->MsgRecv_MyInfo_PointChange(pDataPack, iOffset);
			return true;
		case  N3_CHAT:														// chat message...
			this->MsgRecv_Chat(pDataPack, iOffset);
			return true;
		case N3_WARP:
			{
				const float fX = (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
				const float fZ = (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;

				float fY = ACT_WORLD->GetHeightWithTerrain(fX, fZ, true);
				const float fYObject = ACT_WORLD->GetHeightWithShape(fX, fZ);
				if(fYObject > fY) fY = fYObject;

				this->InitPlayerPosition(__Vector3(fX, fY, fZ)); // Initialize the player position.. Raise him up and make him take the basic action.
			}
			return true;
		case N3_MOVE:
			this->MsgRecv_UserMove(pDataPack, iOffset);
			return true;
		case N3_ROTATE:												// Rotate command...
			this->MsgRecv_Rotation(pDataPack, iOffset);
			return true;
		case N3_REGENE:
			{
				// if(m_pUIDead) m_pUIDead->MsgRecv_Revival(pDataPack, iOffset);
				this->MsgRecv_Regen(pDataPack, iOffset);
				std::string szMsg; ::_LoadStringFromResource(IDS_REGENERATION, szMsg);
				MessageBoxClose(szMsg);
				m_pUITargetBar->SetVisible(false);
			}
			return true;
		case N3_DEAD:
			this->MsgRecv_Dead(pDataPack, iOffset);
			return true;
		case N3_TIME:
			this->MsgRecv_Time(pDataPack, iOffset);
			return true;
		case N3_WEATHER:
			this->MsgRecv_Weather(pDataPack, iOffset);
			return true;
		case N3_USER_INOUT:												// Other users in/out.
			this->MsgRecv_UserInOut(pDataPack, iOffset);
			return true;
		case N3_UPDATE_REGION_UPC:										// The first time you log on, the characters in the surrounding area are updated...
			this->MsgRecv_UserInAndRequest(pDataPack, iOffset);
			return true;
		case N3_REQUEST_USER_IN:										// Get detailed information about the UserIn requested from the server.
			this->MsgRecv_UserInRequested(pDataPack, iOffset);						// 
			return true;
		case N3_UPDATE_REGION_NPC:										// The first time you log on, the characters in the surrounding area are updated...
			this->MsgRecv_NPCInAndRequest(pDataPack, iOffset);
			return true;
		case N3_REQUEST_NPC_IN:											// Get detailed information about the UserIn requested from the server.
			this->MsgRecv_NPCInRequested(pDataPack, iOffset);						// 
			return true;
		case N3_NPC_INOUT:												// NPC in/out...
			this->MsgRecv_NPCInOut(pDataPack, iOffset);
			return true;
		case N3_ATTACK:
			this->MsgRecv_Attack(pDataPack, iOffset);
			return true;
		case N3_NPC_MOVE:												// NPC movement packets..
			this->MsgRecv_NPCMove(pDataPack, iOffset);
			return true;
		case N3_TARGET_HP:
			this->MsgRecv_TargetHP(pDataPack, iOffset);
			return true;
		case N3_ITEM_MOVE:
			this->MsgRecv_ItemMove(pDataPack, iOffset);				// Response to Item Move..
			return true;
		case N3_ITEM_BUNDLE_DROP:
			this->MsgRecv_ItemBundleDrop(pDataPack, iOffset);
			return true;
		case N3_ITEM_BUNDLE_OPEN_REQUEST:
			this->MsgRecv_ItemBundleOpen(pDataPack, iOffset);
			return true;
		case N3_ITEM_TRADE_START:
			this->MsgRecv_ItemTradeStart(pDataPack, iOffset);
			return true;
		case N3_ITEM_TRADE:
			this->MsgRecv_ItemTradeResult(pDataPack, iOffset);
			return true;
		case N3_ITEM_DROPPED_GET:
			this->MsgRecv_ItemDroppedGetResult(pDataPack, iOffset);					// The result of eating an item that fell on the ground..
			return true;
		case N3_ITEM_TRADE_REPAIR:
			this->MsgRecv_NpcEvent(pDataPack, iOffset);
			return true;
		case N3_ITEM_REPAIR_REQUEST:
			this->MsgRecv_ItemRepair(pDataPack, iOffset);
			return true;
		case N3_ITEM_COUNT_CHANGE:
			this->MsgRecv_ItemCountChange(pDataPack, iOffset);
			return true;
		case N3_ITEM_DESTROY:
			this->MsgRecv_ItemDestroy(pDataPack, iOffset);
			return true;
		case N3_ITEM_WEIGHT_CHANGE:
			this->MsgRecv_ItemWeightChange(pDataPack, iOffset);
			return true;
		case N3_USER_LOOK_CHANGE:
			this->MsgRecv_UserLookChange(pDataPack, iOffset);
			return true;
		case N3_ZONE_CHANGE:
			this->MsgRecv_ZoneChange(pDataPack, iOffset);
			this->MsgSend_ZoneChangeComplete(); // Zone Loading completion packet sent..
			return true;
		case N3_STATE_CHANGE:
			this->MsgRecv_UserState(pDataPack, iOffset);
			return true;
		case N3_NOTICE:
			this->MsgRecv_Notice(pDataPack, iOffset);
			return true;
		case N3_PARTY_OR_FORCE:
			this->MsgRecv_PartyOrForce(pDataPack, iOffset);
			return true;
		case N3_PER_TRADE:
			this->MsgRecv_PerTrade(pDataPack, iOffset);
			return true;
		case N3_SKILL_CHANGE:
			this->MsgRecv_SkillChange(pDataPack, iOffset);
			return true;
		case N3_MAGIC:
			this->MsgRecv_MagicProcess(pDataPack, iOffset);
			return true;
		case N3_CLASS_CHANGE:
			this->MsgRecv_NpcChangeOpen(pDataPack, iOffset);
			return true;
		case N3_OBJECT_EVENT:
			this->MsgRecv_ObjectEvent(pDataPack, iOffset);
			return true;
		case N3_CHAT_SELECT_TARGET:
			{
				std::string szID, szMsg;
				const int iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// ID string length..
				CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szID, iLen);	// ID string..

				e_ChatMode eCM = N3_CHAT_UNKNOWN;
				if(szID.empty())
				{
					::_LoadStringFromResource(IDS_CHAT_SELECT_TARGET_FAIL, szMsg);
					eCM = N3_CHAT_NORMAL;
				}
				else
				{
					::_LoadStringFromResource(IDS_CHAT_SELECT_TARGET_SUCCESS, szMsg);
					eCM = N3_CHAT_PRIVATE;
				}
				
				this->MsgOutput(szID + szMsg, 0xffffff00);
				m_pUIChatDlg->ChangeChattingMode(eCM); // Automatically switch to whisper mode.
			}
			return true;
		case N3_CONCURRENT_USER_COUNT: // Number of concurrent users...
			{
				const int iUserCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// ID string length..

				std::string szFmt; ::_LoadStringFromResource(IDS_FMT_CONCURRENT_USER_COUNT, szFmt);
				char szBuff[128] = "";
				sprintf(szBuff, szFmt.c_str(), iUserCount);
				this->MsgOutput(szBuff, D3DCOLOR_ARGB(255,255,255,0));
			}
			return true;
		case N3_DURABILITY_CHANGE:
			this->MsgRecv_DurabilityChange(pDataPack, iOffset);
			return true;
		case N3_KNIGHTS:
			this->MsgRecv_Knights(pDataPack, iOffset);
			return true;
		case N3_KNIGHTS_LIST_BASIC:
			this->MsgRecv_KnightsListBasic(pDataPack, iOffset);
			return true;
		case N3_COMPRESSED_PACKET: // This is compressed data... needs to be parsed one more time!!!
			this->MsgRecv_CompressedPacket(pDataPack, iOffset);
			return true;
		case N3_CONTINOUS_PACKET: // This is compressed data... needs to be parsed one more time!!!
			this->MsgRecv_ContinousPacket(pDataPack, iOffset);
			return true;
		case N3_WAREHOUSE:	// locker..
			this->MsgRecv_WareHouse(pDataPack, iOffset);			// Archive related packets..
			return true;
		case N3_FRIEND_INFO:
			if(m_pUIVar->m_pPageFriends) m_pUIVar->m_pPageFriends->MsgRecv_MemberInfo(pDataPack, iOffset);
			return true;
		case N3_NOAH_CHANGE:
			this->MsgRecv_NoahChange(pDataPack, iOffset);
			return true;
		case N3_WARP_LIST:
			this->MsgRecv_WarpList(pDataPack, iOffset);
			return true;
		// case N3_SERVER_CHECK:
		// this->MsgRecv_ServerCheckAndRequestConcurrentUserCount(pDataPack, iOffset);
		// return true;
		// case N3_SERVER_CONCURRENT_CONNECT:
		// this->MsgRecv_ConcurrentUserCountAndSendServerCheck(pDataPack, iOffset);
		// return true;
		case N3_CORPSE_CHAR: // Regen to notify nearby users that it is a corpse.
			this->MsgRecv_Corpse(pDataPack, iOffset);
			return true;
		case N3_PARTY_BBS:
			if(m_pUIPartyBBS) m_pUIPartyBBS->MsgRecv_RefreshData(pDataPack, iOffset);
			return true;
		case N3_TRADE_BBS:
			if(m_pUITradeBBS) m_pUITradeBBS->MsgRecv_TradeBBS(pDataPack, iOffset);
			return true;
		case N3_QUEST_SELECT:
			if(m_pUIQuestMenu) m_pUIQuestMenu->Open(pDataPack, iOffset);
			return true;
		case N3_QUEST_TALK:
			if(m_pUIQuestTalk) m_pUIQuestTalk->Open(pDataPack, iOffset);
			return true;
		// case N3_CLAN:
		// this->MsgRecv_Clan(pDataPack, iOffset);
			return true;
	}
	
	return false;
}

// Handles keyboard and mouse clicks.
void CGameProcMain::ProcessLocalInput(DWORD dwMouseFlags)
{
	// After loading...
	if ( FALSE == m_bLoadComplete ) return; 

	if (m_pSubProcPerTrade->m_ePerTradeState != PER_TRADE_STATE_NONE) 	/* If a person-to-person transaction is in progress.. */
		return;

	// 
	//
	// mouse handling.
	//
	const POINT ptPrev = s_pLocalInput->MouseGetPosOld();
	const POINT ptCur = s_pLocalInput->MouseGetPos();

	OnMouseMove(ptCur, ptPrev);

	if( dwMouseFlags & MOUSE_RBCLICK )
	{
		OnMouseRBtnPress(ptCur, ptPrev);
	}
	if( dwMouseFlags & MOUSE_RBDOWN )
	{
		OnMouseRbtnDown(ptCur, ptPrev);
	}
	if( dwMouseFlags & MOUSE_RBCLICK )
	{
		OnMouseRBtnPressd(ptCur, ptPrev);
	}
	if( dwMouseFlags & MOUSE_RBDBLCLK )
	{ 
		OnMouseRDBtnPress(ptCur, ptPrev);
	}
	if( dwMouseFlags & MOUSE_LBCLICK )
	{
		OnMouseLBtnPress(ptCur, ptPrev);
	}
	if( dwMouseFlags & MOUSE_LBDOWN )
	{
		OnMouseLbtnDown(ptCur, ptPrev);
	}
	if( dwMouseFlags & MOUSE_LBCLICKED )
	{
		OnMouseLBtnPressd(ptCur, ptPrev);
	}
	if( dwMouseFlags & MOUSE_LBDBLCLK )
	{
		OnMouseLDBtnPress(ptCur, ptPrev);
	}

	// Camera rotation based on mouse...
	float fRotY = 0, fRotX = 0;
	if(0 == ptCur.x) fRotY = -2.0f;
	else if((CN3Base::s_CameraData.vp.Width - 1) == ptCur.x) fRotY = 2.0f;
	if(0 == ptCur.y) fRotX = -1.0f;
	else if((CN3Base::s_CameraData.vp.Height - 1) == ptCur.y) fRotX = 1.0f;
	if(fRotY)
	{
		if(VP_THIRD_PERSON == s_pEng->ViewPoint()) s_pEng->CameraYawAdd(fRotY);
		else s_pPlayer->RotAdd(fRotY);
	}
	if(fRotX && VP_THIRD_PERSON != s_pEng->ViewPoint()) s_pEng->CameraPitchAdd(fRotX);

	//
	// mouse handling.
	//
	// 

	// 
	// hotkey
	int iHotKey = -1;
	if( s_pLocalInput->IsKeyPress(KM_HOTKEY1) ) iHotKey = 0;
	else if( s_pLocalInput->IsKeyPress(KM_HOTKEY2) ) iHotKey = 1;
	else if( s_pLocalInput->IsKeyPress(KM_HOTKEY3) ) iHotKey = 2;
	else if( s_pLocalInput->IsKeyPress(KM_HOTKEY4) ) iHotKey = 3;
	else if( s_pLocalInput->IsKeyPress(KM_HOTKEY5) ) iHotKey = 4;
	else if( s_pLocalInput->IsKeyPress(KM_HOTKEY6) ) iHotKey = 5;
	else if( s_pLocalInput->IsKeyPress(KM_HOTKEY7) ) iHotKey = 6;
	else if( s_pLocalInput->IsKeyPress(KM_HOTKEY8) ) iHotKey = 7;
		
	if ((iHotKey >= 0 && iHotKey < 8) &&
		CN3UIBase::GetFocusedEdit() == nullptr && 
		m_pSubProcPerTrade->m_ePerTradeState == PER_TRADE_STATE_NONE )
	{
		m_pUIHotKeyDlg->EffectTriggerByHotKey(iHotKey);
	}
	// hotkey
	// 

	if(s_pLocalInput->IsKeyPress(KM_CAMERA_CHANGE))												// point of view change..
	{
		this->CommandCameraChange(); // Change the camera perspective...
	}

	// When in third person, raise and lower the camera with the home and end keys.
	if(s_pEng->ViewPoint() == VP_THIRD_PERSON)
	{
		float fPitch = 0;
		if(s_pLocalInput->IsKeyDown(DIK_HOME)) fPitch = D3DXToRadian(45.0f);		// When the home key is pressed...
		else if(s_pLocalInput->IsKeyDown(DIK_END)) fPitch = D3DXToRadian(-45.0f);	// When the End key is pressed...
		if(fPitch) s_pEng->CameraPitchAdd(fPitch);
	}

	if (!IsUIKeyOperated() && nullptr == CN3UIBase::GetFocusedEdit() )			// When not in chat mode
	{
#ifdef _DEBUG
		if ( s_pLocalInput->IsKeyDown(DIK_Q) ) s_pPlayer->m_bTempMoveTurbo = true; // Makes it move super fast. // Temporary function.. Let&#39;s get rid of it later..
		else s_pPlayer->m_bTempMoveTurbo = false; // Makes it move super fast. // Temporary function.. Let&#39;s get rid of it later..
#endif
		if(s_pPlayer->m_InfoBase.iAuthority == AUTHORITY_MANAGER) // Game operators can use this function.
		{
			if ( s_pLocalInput->IsKeyDown(DIK_Q) ) s_pPlayer->m_bTempMoveTurbo = true; // Makes it move super fast. // Temporary function.. Let&#39;s get rid of it later..
			else s_pPlayer->m_bTempMoveTurbo = false; // Makes it move super fast. // Temporary function.. Let&#39;s get rid of it later..
		}

		if(s_pLocalInput->IsKeyPress(KM_TOGGLE_ATTACK))
			this->CommandToggleAttackContinous();		// auto attack..}
		if(s_pLocalInput->IsKeyPress(KM_TOGGLE_RUN))
			this->CommandToggleWalkRun();				// Toggle walk/run
		if(s_pLocalInput->IsKeyPress(KM_TARGET_NEARST_ENEMY))
			this->CommandTargetSelect_NearstEnemy();	// Grab the closest enemy target.
		if(s_pLocalInput->IsKeyPress(KM_TARGET_NEARST_PARTY))
			this->CommandTargetSelect_NearstOurForce(); // Find the nearest party target.

		const float fRotKeyDelta = D3DXToRadian(60); // Rotates 60 degrees per second.
		if(s_pLocalInput->IsKeyDown(KM_ROTATE_LEFT) || s_pLocalInput->IsKeyDown(DIK_LEFT))	
		{
			if(s_pPlayer->IsAlive()) s_pPlayer->RotAdd(-fRotKeyDelta); // Turn left 180 degrees per second.
			if(m_pUIDroppedItemDlg->IsVisible()) m_pUIDroppedItemDlg->LeaveDroppedState();	
		}
		if(s_pLocalInput->IsKeyDown(KM_ROTATE_RIGHT) || s_pLocalInput->IsKeyDown(DIK_RIGHT))	
		{
			if(s_pPlayer->IsAlive()) s_pPlayer->RotAdd(fRotKeyDelta); // Turn right 180 degrees per second.
			if(m_pUIDroppedItemDlg->IsVisible()) m_pUIDroppedItemDlg->LeaveDroppedState();	
		}
		
		if(s_pLocalInput->IsKeyDown(KM_MOVE_FOWARD) || s_pLocalInput->IsKeyDown(DIK_UP))
		{
			bool bStart = false;
			if(s_pLocalInput->IsKeyPress(KM_MOVE_FOWARD) || s_pLocalInput->IsKeyPress(DIK_UP))
			{
				if(VP_THIRD_PERSON == s_pEng->ViewPoint())
				{
					if(!s_pPlayer->m_bTargetOrPosMove) bStart = true;
					s_pPlayer->m_bTargetOrPosMove = false;
					if(s_pPlayer->m_bAttackContinous)
						CommandToggleAttackContinous();
				}
				else
					bStart = true;// at the moment of pressing
			}
			this->CommandMove(MD_FOWARD, bStart); // move forward...
		}
		else if(s_pLocalInput->IsKeyDown(KM_MOVE_BACKWARD) || s_pLocalInput->IsKeyDown(DIK_DOWN))
		{
			bool bStart = false;
			if(s_pLocalInput->IsKeyPress(KM_MOVE_BACKWARD) || s_pLocalInput->IsKeyPress(DIK_DOWN)) 
			{
				if(VP_THIRD_PERSON == s_pEng->ViewPoint())
				{
					if(!s_pPlayer->m_bTargetOrPosMove) bStart = true;
					s_pPlayer->m_bTargetOrPosMove = false;
					if(s_pPlayer->m_bAttackContinous)
						CommandToggleAttackContinous();
				}
				else
					bStart = true;// at the moment of pressing
			}
			this->CommandMove(MD_BACKWARD, bStart); // go back...
		}
		else if(s_pLocalInput->IsKeyPress(KM_TOGGLE_MOVE_CONTINOUS))
		{
			this->CommandToggleMoveContinous();
		}

		if(	s_pLocalInput->IsKeyPressed(KM_MOVE_FOWARD) || s_pLocalInput->IsKeyPressed(DIK_UP) || 
			s_pLocalInput->IsKeyPressed(KM_MOVE_BACKWARD) || s_pLocalInput->IsKeyPressed(DIK_DOWN) ) // The moment you release the forward/reverse key.
		{
			this->CommandMove(MD_STOP, true);
		}

		if(s_pLocalInput->IsKeyPress(KM_TOGGLE_INVENTORY)) this->CommandToggleUIInventory();
		if(s_pLocalInput->IsKeyPress(KM_TOGGLE_STATE)) this->CommandToggleUIState();
		if(s_pLocalInput->IsKeyPress(KM_TOGGLE_SKILL)) this->CommandToggleUISkillTree();
		if(s_pLocalInput->IsKeyPress(KM_TOGGLE_SITDOWN)) this->CommandSitDown(true, !s_pPlayer->m_bSitDown);

		if(s_pLocalInput->IsKeyPress(KM_TOGGLE_HELP)) 
		{
			if(m_pUIHelp->IsVisible()) m_pUIHelp->SetVisible(false);
			else m_pUIHelp->SetVisible(true);
		}

		if(s_pLocalInput->IsKeyPress(KM_TOGGLE_MINIMAP))
		{
			this->CommandToggleUIMiniMap();
		}

		if (s_pLocalInput->IsKeyPress(DIK_PRIOR))
			if (m_pUIHotKeyDlg)	m_pUIHotKeyDlg->PageUp();
		
		if (s_pLocalInput->IsKeyPress(DIK_NEXT))
			if (m_pUIHotKeyDlg)	m_pUIHotKeyDlg->PageDown();

	} // end of if ( !m_UIChatDlg.IsChatMode() )

	// ..... handle the rest of the keyboard..
#if _DEBUG
	if(s_pLocalInput->IsKeyPress(DIK_F12)) // Debugging test...
		s_pEng->Lightning(); // lightning strike...
#endif
}

void CGameProcMain::ProcessPlayerInclination()											// When standing on a slope...
{
	/*	float fFrm = GetIndepTimeFrame();

	__Vector3 vNorm, vNormXZ; 
	s_pTerrain->GetNormal( s_pEng->m_matPlayer.Pos().x, s_pEng->m_matPlayer.Pos().z, vNorm );

	vNorm.Normalize();
	vNormXZ = vNorm;
	vNormXZ.y = 0.0f;

	if ( vNormXZ.Magnitude() > 0.8f )
	{
		vNormXZ.x *= 0.01f*fFrm;
		vNormXZ.z *= 0.01f*fFrm;
		s_pPlayer->m_vPlayerIncline = vNormXZ;
	}*/
}

// 
// 
// 

void CGameProcMain::MsgSend_Continous()						// Sends messages to the server periodically under certain conditions (?).
{
	const float fTime = s_pEng->TimeGet();

	if ( fTime >= m_fMsgSendTimeMove + PACKET_INTERVAL_MOVE )					// If 1 second has passed...
	{
		const __Vector3 vPos = s_pPlayer->Position();
		if(m_vPlayerPosSended != vPos) this->MsgSend_Move(true, true); // If there is any movement, a movement packet is sent.
	}

	if( false == s_pPlayer->m_bMoveContinous && fTime >= m_fMsgSendTimeRot + PACKET_INTERVAL_ROTATE ) // If the player is still... and 2 seconds have passed...
	{
		const float fYaw = s_pPlayer->Yaw();
		if( fYaw != m_fPlayerYawSended ) MsgSend_Rotation(); // Send rotation packet if rotated even slightly
	}
}



// 
// 
// 

void CGameProcMain::MsgSend_Attack(int iTargetID, float fInterval, float fDistance) // Sending attack packets - Prevents hacking by giving the table&#39;s attack cycle the same.
{
	if(s_pPlayer->m_fTimeAfterDeath > 0 || s_pPlayer->IsDead())	return; // It&#39;s dead..

	BYTE byBuff[32];												// buffer..
	int iOffset=0;													// Offset...

	const BYTE bySuccess = true;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ATTACK);						// attack command.
	CAPISocket::MP_AddByte(byBuff, iOffset, 0x01);							// ??? damage??
	CAPISocket::MP_AddByte(byBuff, iOffset, bySuccess);						// Success or not.. - First of all, send it to success.

	fInterval += 0.1f;

	CAPISocket::MP_AddShort(byBuff, iOffset, iTargetID );					// Opponent ID.
	CAPISocket::MP_AddShort(byBuff, iOffset, (int)(fInterval * 100));	// time of attack
	CAPISocket::MP_AddShort(byBuff, iOffset, (int)(fDistance * 10));	// distance attacked

	s_pSocket->Send(byBuff, iOffset);										// sent...
}

void CGameProcMain::MsgSend_Move(bool bMove, bool bContinous)
{
	__Vector3 vPos(0,0,0);
	float fSpeed = s_pPlayer->MoveSpeed();
	
	BYTE byMoveFlag = 0;

	if(true == bMove) // When you start moving or when you move
	{
		if(s_pPlayer->m_fTimeAfterDeath > 0 || s_pPlayer->IsDead())	return; // It&#39;s dead..

		vPos = s_pPlayer->NextPos(PACKET_INTERVAL_MOVE); // By calculating the position one second later.
		byMoveFlag |= 0x01;
	}
	else // at stop
	{
		s_pPlayer->m_bTargetOrPosMove	= false;
		s_pPlayer->m_iMoveTarget		= -1;
		vPos = s_pPlayer->Position();
		fSpeed = 0;
	}

	if(true == bContinous) // If you are moving regularly...
	{
		byMoveFlag |= 0x02;
	}

	BYTE byBuff[64];											// Buffer settings...
	int iOffset=0;											// Offset...

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_MOVE);			// command...
	CAPISocket::MP_AddWord(byBuff, iOffset, vPos.x*10);			// next location
	CAPISocket::MP_AddWord(byBuff, iOffset, vPos.z*10);
	CAPISocket::MP_AddShort(byBuff, iOffset, vPos.y*10);
	CAPISocket::MP_AddWord(byBuff, iOffset, fSpeed*10);			// speed
	CAPISocket::MP_AddByte(byBuff, iOffset, byMoveFlag );		// movement flag.
	s_pSocket->Send(byBuff, iOffset);							// Send packets..

	m_vPlayerPosSended = s_pPlayer->Position(); // Recently sent location settings..
	
	if(true == bMove) m_fMsgSendTimeMove = CN3Base::TimeGet(); // record the time...
}

void CGameProcMain::MsgSend_Rotation()
{
	if(s_pPlayer->IsDead()) return; // How dare you die!

	BYTE byBuff[8];
	int iOffset=0;

	const float fYaw = s_pPlayer->Yaw(); // direction..
	
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ROTATE);
	CAPISocket::MP_AddShort(byBuff, iOffset, fYaw*100);

	s_pSocket->Send(byBuff, iOffset);

	m_fPlayerYawSended = fYaw; // record the rotation
	m_fMsgSendTimeRot = CN3Base::TimeGet(); // record the time...
}


void CGameProcMain::MsgSend_Chat(e_ChatMode eMode, const std::string& szChat)
{
	if(szChat.empty() || szChat.size() >= 128) return;
	if(eMode==N3_CHAT_CLAN && s_pPlayer->m_InfoExt.iKnightsID <=0) return;

	BYTE byBuff[512];
	int iOffset=0;
	
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_CHAT);
	CAPISocket::MP_AddByte(byBuff, iOffset, eMode);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)szChat.size());
	CAPISocket::MP_AddString(byBuff, iOffset, szChat);

	__ASSERT(iOffset<512, "Send Buffer OverFlow");
	s_pSocket->Send(byBuff, iOffset); // send..
}

void CGameProcMain::MsgSend_ChatSelectTarget(const std::string& szTargetID)
{
	if(szTargetID.empty() || szTargetID.size() > 20) return;

	int iOffset = 0;
	BYTE byBuff[32];

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_CHAT_SELECT_TARGET);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)szTargetID.size() );
	CAPISocket::MP_AddString(byBuff, iOffset, szTargetID );

	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgSend_Regen()
{
	if(s_pPlayer->m_iSendRegeneration >= 2) return; // Once sent, a flag that will not be seen until it dies again

	BYTE byBuff[4];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_REGENE);
	CAPISocket::MP_AddByte(byBuff, iOffset, 1); // 1: Survive as a village...
	
	CLogWriter::Write("Send Regeneration");

	s_pSocket->Send(byBuff, iOffset); // send..

	s_pPlayer->m_iSendRegeneration = 2; // Once sent, a flag that will not be seen until it dies again
	TRACE("보냄 - 다시 살아나기\n");
}

bool CGameProcMain::MsgSend_RequestItemBundleOpen(CPlayerNPC* pCorpse)
{
	if(nullptr == pCorpse || pCorpse->m_iDroppedItemID <= 0) return false;

	const float fDistTmp = (pCorpse->Position() - s_pPlayer->Position()).Magnitude();
	if(	fDistTmp >= (pCorpse->Radius() * 2.0f + 6.0f)) return false;

	const int iItemBundleID = pCorpse->m_iDroppedItemID;
	m_pUIDroppedItemDlg->m_iItemBundleID = pCorpse->m_iDroppedItemID;
		
	s_pOPMgr->CorpseRemove(pCorpse, false); // It becomes more and more transparent...
	
	BYTE byBuff[8];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ITEM_BUNDLE_OPEN_REQUEST);
	CAPISocket::MP_AddDword(byBuff, iOffset, iItemBundleID);

	s_pSocket->Send(byBuff, iOffset); // send..

	return true;
}

void CGameProcMain::MsgSend_PartyOrForcePermit(int iPartyOrForce, bool bYesNo)
{
	BYTE byBuff[4];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_PARTY_OR_FORCE);
// CAPISocket::MP_AddByte(byBuff, iOffset, iPartyOrForce);
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PARTY_OR_FORCE_PERMIT);
	CAPISocket::MP_AddByte(byBuff, iOffset, bYesNo);

	s_pSocket->Send(byBuff, iOffset); // send..
}

bool CGameProcMain::MsgSend_PartyOrForceCreate(int iPartyOrForce, const std::string& szID)
{
	if(szID.empty() || szID.size() > 20) return false;

	bool bIAmLeader, bIAmMember;
	int iMemberIndex = -1;
	CPlayerBase* pTarget = nullptr;
	this->PartyOrForceConditionGet(bIAmLeader, bIAmMember, iMemberIndex, pTarget);
	if(true == bIAmMember && false == bIAmLeader) return false; // If I&#39;m in the party and I&#39;m not the leader, I&#39;ll fail...

	BYTE byBuff[32];
	int iOffset=0;

	e_SubPacket_Party eCmdParty = N3_SP_PARTY_OR_FORCE_CREATE;
	if(m_pUIPartyOrForce->MemberCount() >= 2)
	{
		eCmdParty = N3_SP_PARTY_OR_FORCE_INSERT;
	}

	m_pUIPartyOrForce->m_iPartyOrForce = iPartyOrForce;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_PARTY_OR_FORCE);
	// CAPISocket::MP_AddByte(byBuff, iOffset, iPartyOrForce);
	CAPISocket::MP_AddByte(byBuff, iOffset, eCmdParty);
	CAPISocket::MP_AddShort(byBuff, iOffset, szID.size());
	CAPISocket::MP_AddString(byBuff, iOffset, szID);

	s_pSocket->Send(byBuff, iOffset); // send..
	
	if(m_pUIPartyOrForce->MemberCount() <= 0) // If this is your first time creating...
	{
		m_pUIPartyOrForce->MemberAdd(
			s_pPlayer->IDNumber(),
			s_pPlayer->IDString(),
			s_pPlayer->m_InfoBase.iLevel, 
			s_pPlayer->m_InfoBase.eClass, 
			s_pPlayer->m_InfoBase.iHP, 
			s_pPlayer->m_InfoBase.iHPMax);  // I put mine in advance.
	}

	TRACE ("Party or Force 생성 신청 - Target ID(%s)\n", szID.c_str());

	return true;
}

void CGameProcMain::MsgSend_PartyOrForceLeave(int iPartyOrForce)
{
	if(m_pUIPartyOrForce->MemberCount() <= 0) return; // No party members...

	CPlayerBase* pTarget = nullptr;
	bool bIAmLeader, bIAmMember;
	int iMemberIndex = -1;
	this->PartyOrForceConditionGet(bIAmLeader, bIAmMember, iMemberIndex, pTarget);

	BYTE byBuff[8];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_PARTY_OR_FORCE);
	// CAPISocket::MP_AddByte(byBuff, iOffset, iPartyOrForce);
	if(bIAmLeader) // If I&#39;m a leader...
	{
		if(iMemberIndex > 0 && pTarget) // It&#39;s a party member.. Let&#39;s kick it out..
		{
			CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PARTY_OR_FORCE_REMOVE);
			CAPISocket::MP_AddShort(byBuff, iOffset, pTarget->IDNumber());
		}
		else // If the target is not a party member, break the party..
		{
			CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PARTY_OR_FORCE_DESTROY);
		}
	}
	else if(bIAmMember) // If you are not a leader, send a withdrawal message.
	{
		CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PARTY_OR_FORCE_REMOVE);
		CAPISocket::MP_AddShort(byBuff, iOffset, s_pPlayer->IDNumber());
	}
	s_pSocket->Send(byBuff, iOffset); // send..
}

void CGameProcMain::MsgSend_ObjectEvent(int iEventID, int iNPCID)
{
	BYTE byBuff[8];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_OBJECT_EVENT);
	CAPISocket::MP_AddShort(byBuff, iOffset, iEventID);	// Index
	CAPISocket::MP_AddShort(byBuff, iOffset, iNPCID);	// Parameter

	s_pSocket->Send(byBuff, iOffset); // send..
}

void CGameProcMain::MsgSend_Weather(int iWeather, int iPercent)
{
	if(iWeather < 1 || iWeather > 3) return;
	if(iPercent < 0 || iPercent > 100) return;

	BYTE byBuff[8];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_WEATHER); // -&gt; byte - Climate.... 0x01 - Sunny.. 0x02 - Rain 0x03
	CAPISocket::MP_AddByte(byBuff, iOffset, iWeather); // -&gt; byte - Climate.... 0x01 - Sunny.. 0x02 - Rain 0x03
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)iPercent); // short -&gt; sunny day amount of fog, rain and snow as a percentage

	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgSend_Time(int iHour, int iMin)
{
	BYTE byBuff[12];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_TIME); 
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);		// year
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);		// month
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);		// day
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)iHour);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)iMin);

	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgSend_Administrator(e_SubPacket_Administrator eSP, const std::string& szID)
{
	if(szID.empty() || szID.size() >= 20) return;

	BYTE byBuff[64];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ADMINISTRATOR); // Administrator-only packets..
	CAPISocket::MP_AddByte(byBuff, iOffset, eSP);
	CAPISocket::MP_AddShort(byBuff, iOffset, szID.size());
	CAPISocket::MP_AddString(byBuff, iOffset, szID);	

	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgSend_KnightsJoinReq(bool bJoin)
{
	BYTE byBuff[8];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_KNIGHTS); // Administrator-only packets..
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_KNIGHTS_JOIN_REQ);
	CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)bJoin);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_iJoinReqClanRequierID);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_iJoinReqClan);
	
	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgSend_KnightsJoin(int iTargetID)
{
	BYTE byBuff[4];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_KNIGHTS); // Administrator-only packets..
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_KNIGHTS_JOIN);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)iTargetID);
	
	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgSend_KnightsLeave(std::string& szName)
{
	BYTE byBuff[64];
	int iOffset=0;

	const int iLen = szName.size();

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_KNIGHTS); // Administrator-only packets..
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_KNIGHTS_MEMBER_REMOVE);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)iLen);
	CAPISocket::MP_AddString(byBuff, iOffset, szName);	// Put the ID string in the packet..
	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgSend_KnightsWithdraw()
{
	BYTE byBuff[2];
	int iOffset=0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_KNIGHTS); 
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_KNIGHTS_WITHDRAW);
	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgSend_KnightsAppointViceChief(std::string& szName)
{
	BYTE byBuff[64];
	int iOffset=0;

	const int iLen = szName.size();

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_KNIGHTS); // Administrator-only packets..
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_KNIGHTS_APPOINT_VICECHIEF);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)iLen);
	CAPISocket::MP_AddString(byBuff, iOffset, szName);	// Put the ID string in the packet..
	s_pSocket->Send(byBuff, iOffset);
}

bool CGameProcMain::MsgRecv_MyInfo_All(DataPack* pDataPack, int& iOffset)
{
	const int iZone = s_pPlayer->m_InfoExt.iZoneCur;
	s_pPlayer->Release(); // Once you&#39;ve unlocked it all...
	s_pPlayer->m_InfoExt.iZoneCur = iZone;

	const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const int iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	std::string szID; CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szID, iLen);
	s_pPlayer->IDSet(iID, szID, D3DCOLOR_XRGB(100, 210, 255)); // Between light blue and light blue...

	const float fX	= (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	const float fZ	= (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	const float fY	= (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;
	
	s_pPlayer->m_InfoBase.eNation	= (e_Nation)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoBase.eRace		= (e_Race)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoBase.eClass	= (e_Class)CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iFace		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // face shape..
	s_pPlayer->m_InfoExt.iHair		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // hair

	__TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks->Find(s_pPlayer->m_InfoBase.eRace);	// User Player Character Skin structure pointer..
	if(nullptr == pLooks) CLogWriter::Write("CGameProcMain::MsgRecv_MyInfo_All : failed find character resource data (Race : %d)", s_pPlayer->m_InfoBase.eRace);
	__ASSERT(pLooks, "failed find character resource data");
	s_pPlayer->InitChr(pLooks); // Joint setting...

	s_pPlayer->m_InfoExt.iRank		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iTitle		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoBase.iLevel	= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iLevelPrev = s_pPlayer->m_InfoBase.iLevel;
	s_pPlayer->m_InfoExt.iBonusPointRemain = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Remaining bonus points...

	s_pPlayer->m_InfoExt.iExpNext	= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset); 
	s_pPlayer->m_InfoExt.iExp		= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset); 
	s_pPlayer->m_InfoExt.iRealmPoint = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iCity		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	
	std::string szKnightsName;
	const int iKnightsID				= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Affiliated Knight ID
	const auto eKnightsDuty	= (e_KnightsDuty)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Authority in the Knights...
	const int iKnightNameLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // The length of the name of the Knights Templar.
	CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szKnightsName, iKnightNameLen);
	const int iKnightsGrade			= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Affiliation Knight Rank
	const int	iKnightsRank			= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Affiliated Knight Rank
	
	// Knights related settings...
	s_pPlayer->m_InfoExt.eKnightsDuty = eKnightsDuty; // Authority in the Knights...
	s_pPlayer->KnightsInfoSet(iKnightsID, szKnightsName, iKnightsGrade, iKnightsRank);
	m_pUIVar->UpdateKnightsInfo();
	
	s_pPlayer->m_InfoBase.iHPMax	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	
	s_pPlayer->m_InfoBase.iHP		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); 
	s_pPlayer->m_InfoExt.iMSPMax	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iMSP		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iWeightMax	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); 
	s_pPlayer->m_InfoExt.iWeight	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); 

	s_pPlayer->m_InfoExt.iStrength			= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iStrength_Delta		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iStamina			= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iStamina_Delta		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iDexterity			= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iDexterity_Delta	= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iIntelligence		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iIntelligence_Delta	= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iMagicAttak		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iMagicAttak_Delta	= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	
	s_pPlayer->m_InfoExt.iAttack			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	// s_pPlayer->m_InfoExt.iAttack_Delta		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iGuard				= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	// s_pPlayer->m_InfoExt.iGuard_Delta		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iRegistFire		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iRegistCold		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iRegistLight		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iRegistMagic		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iRegistCurse		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iRegistPoison		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	s_pPlayer->m_InfoExt.iGold				= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoBase.iAuthority		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // authority..

	// Skill UI update..
	for(auto i = 0; i < 9; i++ )
	{
		m_pUISkillTreeDlg->m_iSkillInfo[i] = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	}
	m_pUISkillTreeDlg->InitIconUpdate();
	m_pUIHotKeyDlg->ReleaseItem();
	m_pUIHotKeyDlg->InitIconUpdate();			// Checks if the hotkey is valid, and if so, reads it from the registry.

	// What you&#39;re wearing...
	int iItemIDInSlots[ITEM_SLOT_COUNT]; memset(iItemIDInSlots, -1, sizeof(iItemIDInSlots));
	int iItemDurabilityInSlots[ITEM_SLOT_COUNT]; memset(iItemDurabilityInSlots, -1, sizeof(iItemDurabilityInSlots));
	int iItemCountInSlots[ITEM_SLOT_COUNT]; memset(iItemCountInSlots, -1, sizeof(iItemCountInSlots));

	for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )				// As for the number of slots.
	{
		iItemIDInSlots[i]			= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
		iItemDurabilityInSlots[i]	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		iItemCountInSlots[i]		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	}

	m_fMsgSendTimeMove		= 0;						// Network ReQuest timer reset..
	m_fMsgSendTimeRot		= 0;
	m_fPlayerYawSended		= 0;						// Rotation on the player&#39;s y-axis since the last message was sent.
	m_vPlayerPosSended		= s_pPlayer->Position();	// The player&#39;s position at the time the last message was sent.

	// Apply all status window values
	if(m_pUIVar->m_pPageState) m_pUIVar->m_pPageState->UpdateID(szID); // Apply name.
	m_pUIVar->UpdateAllStates(&(s_pPlayer->m_InfoBase), &(s_pPlayer->m_InfoExt));

	// __KnightsInfoBase* pKIB = m_pUIKnightsOp->KnightsInfoFind(s_pPlayer->m_InfoExt.iKnightsID);
	// if(pKIB) m_pUIVar->m_pPageKnights->UpdateKnightsName(pKIB->szName);
	
	// status bar update
	m_pUIStateBarAndMiniMap->UpdateExp(s_pPlayer->m_InfoExt.iExp, s_pPlayer->m_InfoExt.iExpNext, true);
	m_pUIStateBarAndMiniMap->UpdateHP(s_pPlayer->m_InfoBase.iHP, s_pPlayer->m_InfoBase.iHPMax, true);
	m_pUIStateBarAndMiniMap->UpdateMSP(s_pPlayer->m_InfoExt.iMSP, s_pPlayer->m_InfoExt.iMSPMax, true);

	m_pUIPartyOrForce->MemberInfoReInit(); // party window.. update..
	
	__TABLE_ITEM_BASIC* pItem = nullptr;								// Item table structure pointer..
	__TABLE_ITEM_EXT* pItemExt = nullptr;								// Item table structure pointer..

	int iItemIDInInventorys[MAX_ITEM_INVENTORY]; memset(iItemIDInInventorys, -1, sizeof(iItemIDInInventorys));
	int iItemCountInInventorys[MAX_ITEM_INVENTORY]; memset(iItemCountInInventorys, -1, sizeof(iItemCountInInventorys));
	int iItemDurabilityInInventorys[MAX_ITEM_INVENTORY]; memset(iItemDurabilityInInventorys, -1, sizeof(iItemDurabilityInInventorys));

	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )				// As for the number of slots.
	{
		iItemIDInInventorys[i]			= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
		iItemDurabilityInInventorys[i]	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		iItemCountInInventorys[i]		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	}

	m_pUIInventory->ReleaseItem();

	std::string szResrcFN, szIconFN;
	for(auto i = 0; i < ITEM_SLOT_COUNT; i++ )				// As for the number of slots.
	{
		if(0 == iItemIDInSlots[i]) continue;

		pItem = s_pTbl_Items_Basic->Find(iItemIDInSlots[i]/1000*1000);	// Get column data...
		if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
			pItemExt = s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iItemIDInSlots[i]%1000);	// Get column data...
		else
			pItemExt = nullptr;

		if (nullptr == pItem || nullptr == pItemExt )
		{
			__ASSERT(0, "NULL Item!!!");
			CLogWriter::Write("MyInfo - slot - Unknown Item %d, IDNumber", iItemIDInSlots[i]);
			continue; // If there are no items...
		}

		e_PartPosition ePart;
		e_PlugPosition ePlug;
		const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, &szResrcFN, &szIconFN, ePart, ePlug); // Create a file name according to the item
		if(ITEM_TYPE_UNKNOWN == eType) CLogWriter::Write("MyInfo - slot - Unknown Item");
		__ASSERT(ITEM_TYPE_UNKNOWN != eType, "Unknown Item Type");
		const auto eSlot = (e_ItemSlot)i;

		switch(eSlot)
		{
		case ITEM_SLOT_EAR_RIGHT:
		case ITEM_SLOT_EAR_LEFT:
		case ITEM_SLOT_NECK:
		case ITEM_SLOT_RING_RIGHT:
		case ITEM_SLOT_RING_LEFT:
			{
				if(ITEM_TYPE_ICONONLY != eType) CLogWriter::Write("MyInfo - slot - Invalid Item");
				__ASSERT(ITEM_TYPE_ICONONLY == eType, "Invalid Item");
			}
			break;
		case ITEM_SLOT_UPPER: // Parts - Upper Body
		case ITEM_SLOT_LOWER: // Parts - lower body
		case ITEM_SLOT_GLOVES: // Gloves
		case ITEM_SLOT_SHOES: // shoes
		case ITEM_SLOT_HEAD: // hair ?? -&gt; Helmet ??
			{
				if(ITEM_TYPE_PART != eType)  CLogWriter::Write("MyInfo - slot - Invalid Item");
				__ASSERT(ITEM_TYPE_PART == eType, "Invalid Item");
				s_pPlayer->PartSet(ePart, szResrcFN, pItem, pItemExt);	// Set the part...
			}
			break;
		case ITEM_SLOT_HAND_RIGHT: // right hand
		case ITEM_SLOT_HAND_LEFT: // left hand
			{
				if(ITEM_TYPE_PLUG != eType) CLogWriter::Write("MyInfo - slot - Invalid Item");
				__ASSERT(ITEM_TYPE_PLUG == eType, "Invalid Item");
				
				e_PlugPosition ePlugPos;
				if(ITEM_SLOT_HAND_RIGHT == eSlot) ePlugPos = PLUG_POS_RIGHTHAND;
				else ePlugPos = PLUG_POS_LEFTHAND;
				s_pPlayer->PlugSet(ePlugPos, szResrcFN, pItem, pItemExt);	// Set the part...
			}
			break;
		case ITEM_SLOT_SHOULDER: // Cloak
			{
			}
			break;
		case ITEM_SLOT_BELT:
			{
			}
			break;
		}

		s_pPlayer->DurabilitySet(eSlot, iItemDurabilityInSlots[eSlot]);	// Dealing with old weapons...

		auto* spItem = new __IconItemSkill;
		spItem->pItemBasic	= pItem;
		spItem->pItemExt	= pItemExt;
		spItem->szIconFN	= szIconFN; // Copy icon filename..
		spItem->iCount		= iItemCountInSlots[i];
		spItem->iDurability = iItemDurabilityInSlots[i];

		// Put it in your inventory slot.
		m_pUIInventory->m_pMySlot[i] = spItem;
		TRACE("Init Inv Msg Slot %d \n", iItemIDInSlots[i]);
	}

	// Inventory..
	int iItemCount = 0;
	for(auto i = 0; i < MAX_ITEM_INVENTORY; i++ )				// As for the number of inventories.
	{
		if(!iItemIDInInventorys[i]) continue;

		pItem = s_pTbl_Items_Basic->Find(iItemIDInInventorys[i]/1000*1000);	// Get column data...
		pItemExt = nullptr;
		if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
			pItemExt = s_pTbl_Items_Exts[pItem->byExtIndex]->Find(iItemIDInInventorys[i]%1000);	// Get column data...
		if (nullptr == pItem || nullptr == pItemExt )
		{
			__ASSERT(0, "NULL Item");
			CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", iItemIDInInventorys[i]);
			continue; // If there are no items...
		}

		e_PartPosition ePart;
		e_PlugPosition ePlug;
		const e_ItemType eType = CGameProcedure::MakeResrcFileNameForUPC(pItem, nullptr, &szIconFN, ePart, ePlug); // Create a file name according to the item
		if(ITEM_TYPE_UNKNOWN == eType) CLogWriter::Write("MyInfo - slot - Unknown Item");
		__ASSERT(ITEM_TYPE_UNKNOWN != eType, "Unknown Item");

		auto* spItem = new __IconItemSkill;
		spItem->pItemBasic	= pItem;
		spItem->pItemExt	= pItemExt;
		spItem->szIconFN = szIconFN; // Copy icon filename..
		spItem->iCount	= iItemCountInInventorys[i];
		spItem->iDurability = iItemDurabilityInInventorys[i];

		m_pUIInventory->m_pMyInvWnd[i] = spItem; 
		TRACE("Init Inv Msg Inve %d, iOrder %d \n", iItemIDInInventorys[i], i);
	}

	// Inventory & Slot Icon Operation.. ^^
	m_pUIInventory->InitIconUpdate();

	if (m_pUISkillTreeDlg) m_pUISkillTreeDlg->UpdateDisableCheck();
	if (m_pUIHotKeyDlg) m_pUIHotKeyDlg->UpdateDisableCheck();

	if(s_pPlayer->Part(PART_POS_UPPER)->FileName().empty()) // If nothing is set, there is no file name.
		s_pPlayer->PartSet(PART_POS_UPPER, pLooks->szPartFNs[PART_POS_UPPER], nullptr, nullptr);	// upper body..
	if(s_pPlayer->Part(PART_POS_LOWER)->FileName().empty()) // If nothing is set, there is no file name.
		s_pPlayer->PartSet(PART_POS_LOWER, pLooks->szPartFNs[PART_POS_LOWER], nullptr, nullptr);	// lower body..
	if(s_pPlayer->Part(PART_POS_HANDS)->FileName().empty()) // If nothing is set, there is no file name.
		s_pPlayer->PartSet(PART_POS_HANDS,  pLooks->szPartFNs[PART_POS_HANDS], nullptr, nullptr);	// eight..
	if(s_pPlayer->Part(PART_POS_FEET)->FileName().empty()) // If nothing is set, there is no file name.
		s_pPlayer->PartSet(PART_POS_FEET,  pLooks->szPartFNs[PART_POS_FEET], nullptr, nullptr);	// leg..
	if(s_pPlayer->Part(PART_POS_FACE)->FileName().empty()) // If nothing is set, there is no file name.
		s_pPlayer->InitFace();
	if(s_pPlayer->Part(PART_POS_HAIR_HELMET)->FileName().empty()) // If nothing is set, there is no file name.
		s_pPlayer->InitHair();

	s_pPlayer->InventoryChrAnimationInitialize();
	s_pPlayer->SetSoundAndInitFont();

	// s_pPlayer->SettingCollisionCheckPoint();

	// 
	// Read default...
	int iRun = 1;
 	// if(false == CGameProcedure::RegGetSetting(&quot;UserRun&quot;, &amp;iRun, 4)) iRun = 1; // Read the walking and running state from the registry. The default is the running state.
	// if(1 == iRun)
	this->CommandToggleWalkRun(); // makes me run...

	e_ViewPoint eVP = VP_THIRD_PERSON;
	if(false == CGameProcedure::RegGetSetting("CameraMode", &eVP, 4)) eVP = VP_THIRD_PERSON; // record camera status
	s_pEng->ViewPointChange(eVP);
	// Read default...
	// 

	this->InitPlayerPosition(__Vector3(fX, fY, fZ)); // Initialize the player position.. Raise him up and make him take the basic action.

	// berserk temp
	// s_pPlayer-&gt;PlugSet(PLUG_POS_BACK, &quot;item/babacloak.n3cplug_cloak&quot;, NULL); // set the part..
	// end berserk temp

	// berserk
	// s_pPlayer->AttachCloak();

	// ..
	s_pOPMgr->Release();							// Initialize other user management classes..
	
	m_bLoadComplete = TRUE;						// End of loading...

	return true;
}

bool CGameProcMain::MsgRecv_Chat(DataPack* pDataPack, int& iOffset)
{
	std::string szChat;				// buffer..
	const auto eCM	=	(e_ChatMode)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// chat type
	const auto eNation =	(e_Nation)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// sender country
	const int iID =			CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);			// Sender
	const int iChatLen =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);			// Chat string length..
	CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szChat, iChatLen );
	
	if(eCM == N3_CHAT_CONTINUE_DELETE)
	{// Delete persistent notice...
		m_pUIChatDlg->DeleteContinueMsg();
		return true;
	}
	else if(eCM == N3_CHAT_TITLE_DELETE)
	{// Remove title notice...
		m_pUIChatDlg->SetNoticeTitle("", 0xffffffff);
		return true;
	}

	D3DCOLOR crChat = 0xffffffff;
	switch(eCM)
	{
	case N3_CHAT_NORMAL:	crChat = D3DCOLOR_ARGB(255,255,255,255);	break;
	case N3_CHAT_PRIVATE:	crChat = D3DCOLOR_ARGB(255,192,192,0);		break;
	case N3_CHAT_PARTY:		crChat = D3DCOLOR_ARGB(255,0,192,192);		break;
	case N3_CHAT_FORCE:		crChat = D3DCOLOR_ARGB(255,0,192,192);		break;
	// case N3_CHAT_SHOUT:	crChat = D3DCOLOR_ARGB(255,255,0,0);		break;
	case N3_CHAT_SHOUT:		crChat = D3DCOLOR_ARGB(255,0xf8,0x66,0x05);	break;
	case N3_CHAT_CLAN:		crChat = D3DCOLOR_ARGB(255,0,255,0);		break;
	case N3_CHAT_PUBLIC:	crChat = D3DCOLOR_ARGB(255,255,255,0);		break;
	case N3_CHAT_WAR:		crChat = D3DCOLOR_ARGB(255,255,255,0);		break;
	case N3_CHAT_TITLE:		crChat = D3DCOLOR_ARGB(255,255,255,0);		break;
	case N3_CHAT_CONTINUE:	crChat = D3DCOLOR_ARGB(255,255,255,0);		break;
	}

	if(eCM == N3_CHAT_TITLE)
	{
		m_pUIChatDlg->SetNoticeTitle(szChat, crChat);
		return true;
	}
	else if(N3_CHAT_WAR == eCM)
	{
		if(m_pWarMessage) m_pWarMessage->SetMessage(szChat, D3DFONT_BOLD, crChat);
		return true;
	}
	else if(N3_CHAT_CONTINUE == eCM)
	{
		m_pUIChatDlg->AddContinueMsg(eCM, szChat, crChat);
		return true;
	}


	// Interpretation service... ㅡ,.ㅡ a
	if( N3_CHAT_NORMAL == eCM || 
		N3_CHAT_PRIVATE == eCM || 
		N3_CHAT_SHOUT == eCM )
	{
		if(eNation != s_pPlayer->m_InfoBase.eNation)
		{
			const CPlayerBase* pTalker = s_pOPMgr->UPCGetByID(iID, false);
			const bool bIamManager = (0 == s_pPlayer->m_InfoBase.iAuthority) ? true : false;
			const bool bTalkerIsManager = (pTalker && 0 == pTalker->m_InfoBase.iAuthority)  ? true : false;
				
			if(!(bIamManager || bTalkerIsManager)) // If I am not an operator and the other party is not an operator
			{
				int i = szChat.find(':');
				if(i >= 0)
				{
					for(; i < iChatLen; i++)
					{
						szChat[i] = '!' + rand()%10; // Change it to a strange word.
					}
				}
			}
		}
	}
	
	// Inject balloons...
	CPlayerBase* pBPC = nullptr;
	if(iID == s_pPlayer->IDNumber()) pBPC = s_pPlayer;
	else pBPC = s_pOPMgr->CharacterGetByID(iID, false);
	if(pBPC && s_pPlayer->Distance(pBPC->Position()) < 4.0f &&
		(N3_CHAT_NORMAL == eCM || N3_CHAT_SHOUT == eCM) ) // Usually only when chatting or shouting..
		pBPC->BalloonStringSet(szChat, crChat);

	// Put it in the chat window.
	m_pUIChatDlg->AddChatMsg(eCM, szChat, crChat);

	return true;
}

bool CGameProcMain::MsgRecv_UserMove(DataPack* pDataPack, int& iOffset)
{
	const int iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const float fX		= (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f; // Next location when departing or moving. When stopped, it receives the current position.
	const float fZ		= (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	const float fY		= (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;
	const float fSpeed	= (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f; // Movement speed when starting or moving. When stopped, 0 comes.
	const BYTE byMoveFlag = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Moving flag.. 0 stop 1 start, 2 keep moving

	// The part where the function should come.. ^^
	// ID, player state 1, player state 2, current xzy position, current xzy direction, x, z, y in 1 second dnlcl..

	if(0xff == byMoveFlag) // This is a forced location update packet.
	{
		if(iID == s_pPlayer->IDNumber())	// If my mash...
		{
			this->InitPlayerPosition(__Vector3(fX, fY, fZ)); // Initialize the player position.. Raise him up and make him take the basic action.
		}
		else
		{
			CPlayerNPC* pBPC = s_pOPMgr->CharacterGetByID(iID, true);	// Get another player...
			if (nullptr == pBPC)
			{
				this->MsgSend_UserInRequest(iID); // Request if there is no user information.
				return false;
			}
			else if(pBPC->IsDead())
				return false;
			
			pBPC->MoveTo(fX, fY, fZ, 0, 0);	// Current location..
			pBPC->PositionSet(__Vector3(fX, fY, fZ), true);
		}
		return false;
	}

	if(iID == s_pPlayer->IDNumber())	// If my mash...
	{
		s_pPlayer->m_vPosFromServer.Set(fX, fY, fZ);
		return false;
	}

	CPlayerOther* pUPC = s_pOPMgr->UPCGetByID(iID, true);	// Get another player...
	if (nullptr == pUPC) return false;

	// if(0.0f != fSpeed) TRACE("Move - %4.1f ( %d : Mode %d ) %.2f초\n", fSpeed, iID, byMoveFlag, CN3Base::TimeGet());
	// else TRACE(&quot;Stop - %4.1f ( %d : Mode %d ) %.2f초\n&quot;, fSpeed, iID, byMoveFlag, CN3Base::TimeGet());

	// Update with the information received from the server.
	pUPC->MoveTo(fX, fY, fZ, fSpeed, byMoveFlag);	// Current location..

	return true;
}

bool CGameProcMain::MsgRecv_Rotation(DataPack* pDataPack, int& iOffset)
{
	const int iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const float fYaw		= (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/100.0f;

	if(s_pPlayer->IDNumber() == iID) return false;

	CPlayerNPC* pBPC = nullptr;
	pBPC = s_pOPMgr->CharacterGetByID(iID, true);	// Get another user...
	if (nullptr == pBPC ) return false;

	pBPC->RotateTo(fYaw, false);	// With the x, y, z values received from the server

	return true;
}

/*
bool CGameProcMain::MsgRecv_Dead(DataPack* pDataPack, int& iOffset)
{
	int iID	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	// TRACE("Dead message Receive.. !!!!!!!! %d \n", iID);

	if ( iID == s_pPlayer->IDNumber() )
	{
		s_pPlayer-&gt;Action(PSA_PRE_DYING, true, NULL, true); // Kill the player.
	}
	else
	{
		CPlayerOther* pReadyToDead = s_pOPMgr-&gt;PlayerGetByID(iID, true); // Search among the living..
		if(pReadyToDead) // If there is, make it about to die.
		{
			pReadyToDead->Acttion(PSA_PRE_DYING, true, NULL, true);
		}
	}

	return true;
}
*/


bool CGameProcMain::MsgRecv_Regen(DataPack* pDataPack, int& iOffset)
{
	__Vector3 vPosPlayer;
	vPosPlayer.x = (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	vPosPlayer.z = (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	vPosPlayer.y = (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;
	
	this->InitPlayerPosition(vPosPlayer); // Initialize the player position.. Raise him up and make him take the basic action.
	s_pPlayer->RegenerateCollisionMesh(); // Recreate the collision mesh.

	s_pPlayer->m_iSendRegeneration = 0; // Once sent, a flag that will not be seen until it dies again
	s_pPlayer->m_fTimeAfterDeath = 0; // Once sent, a flag that will not be seen until it dies again
	TRACE("받음 - 다시 살아나기(%.1f, %.1f)\n", vPosPlayer.x, vPosPlayer.z);

	//
	// Magic &amp; Effect Reset..
	if(m_pUIStateBarAndMiniMap) m_pUIStateBarAndMiniMap->ClearMagic();
	if(m_pMagicSkillMng) m_pMagicSkillMng->ClearDurationalMagic();
	if(CGameProcedure::s_pFX) s_pFX->StopMine();

	CLogWriter::Write("Receive Regeneration");

	if(s_pPlayer->Nation()==NATION_KARUS) CGameProcedure::s_pFX->TriggerBundle(s_pPlayer->IDNumber(), -1, FXID_REGEN_KARUS, s_pPlayer->IDNumber(), -1);
	else if(s_pPlayer->Nation()==NATION_ELMORAD) CGameProcedure::s_pFX->TriggerBundle(s_pPlayer->IDNumber(), -1, FXID_REGEN_ELMORAD, s_pPlayer->IDNumber(), -1);

	return true;
}

bool CGameProcMain::MsgRecv_Time(DataPack* pDataPack, int& iOffset)
{
	const short year	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const short month = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const short day	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const short hour	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const short min	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	ACT_WORLD->SetGameTimeWithSky(year, month, day, hour, min);

	return true;
}

bool CGameProcMain::MsgRecv_Weather(DataPack* pDataPack, int& iOffset)
{
	int iWeather = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // -&gt; byte - Climate.... 0x01 - Sunny.. 0x02 - Rain 0x03
	int iPercent = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // short -&gt; sunny day amount of fog, rain and snow as a percentage

	
	// 
	// Time for a speed hack check....
	/* float fTimeFromServer = CAPISocket::Parse_GetFloat(pDataPack-&gt;m_pData, iOffset); // float -&gt; periodic time...
	float fTimeLocal = CN3Base::TimeGet();
	
	static float fTimeFromServerPrev = fTimeFromServer;
	static float fTimeLocalPrev = fTimeLocal;
	static int iTSH = 0; // Trying SpeedHacking...
	if(fTimeFromServer != 0.0f)
	{
		float fTDS = fTimeFromServer - fTimeFromServerPrev;
		if (fTDS &gt; 355.0f &amp;&amp; fTDS &lt; 365.0f) // It is supposed to be given once every 6 minutes..
		{
			float fTDL = fTimeLocal - fTimeLocalPrev;
			if(fTDL &lt; (fTDS * 0.8f) || fTDL &gt; (fTDS * 1.2f)) // If the local time is more than 20% greater than the server time...
			{
				iTSH++; // Try Speed Hacking ... ++
			}
			else 
			{
				iTSH = 0;
			}

			if(iTSH &gt;= 3) CGameProcedure::s_bUseSpeedHack = true; // speed hack...
		}

		fTimeFromServerPrev = fTimeFromServer;
		fTimeLocalPrev = fTimeLocal;
	}
	// Time for speed hack check...
	// 
*/
	
	if(iPercent < 20) iPercent = 20;
	if(iPercent > 100) iPercent = 100;
	const float fPercent = iPercent / 100.0f;

	// enum { GEW_CLEAR=0, // clear
	// GEW_DRIZZLE_RAIN=1, // drizzle
	// GEW_RAINY=2, // Moderate amount of rain
	// GEW_HEAVY_RAIN=3, // heavy rain
	// GEW_SNOW1=11, // Cozy Snow
	// GEW_SNOW2=12, // suitable snow
	// GEW_HEAVY_SNOW=13 // heavy snow
	TRACE("받음 - 날씨(%d - %d)\n", iWeather, iPercent);

	ACT_WORLD->SetWeatherWithSky((CN3SkyMng::eSKY_WEATHER)iWeather,iPercent);	// changing sky
	float fDelta = 1.0f;
	if(0x1 == iWeather) fDelta = 0.5f + (1.0f-fPercent)*0.5f; // sunshine. Percent is fog...
	else if(0x02 == iWeather) fDelta = 0.25f + (1.0f - fPercent)*0.75f; // rain .
	else if(0x03 == iWeather) fDelta = 0.25f + (1.0f - fPercent)*0.75f; // It&#39;s snowing..

	s_pEng->FarPlaneDeltaSet(fDelta, false);

	return true;
}

bool CGameProcMain::MsgRecv_UserInOut(DataPack* pDataPack, int& iOffset)
{
	const int iType = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	if ( 0x01 == iType ) // When a user enters. (Updated while updating the user&#39;s area in the original game..)
		this->MsgRecv_UserIn(pDataPack, iOffset, false);
	else if ( 0x02 == iType ) // When User exits.
		this->MsgRecv_UserOut(pDataPack, iOffset);
	else if ( 0x03 == iType ) // When a user enters (dead and resurrected, or a user who starts a game.)
		this->MsgRecv_UserIn(pDataPack, iOffset, true);
	else if ( 0x04 == iType ) // When a user enters (warp)
		this->MsgRecv_UserIn(pDataPack, iOffset, false);
	
	return true;
}

bool CGameProcMain::MsgRecv_UserIn(DataPack* pDataPack, int& iOffset, bool bWithFX)
{
	const int iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	std::string szName;
	const int iNameLen	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szName, iNameLen );

	const auto eNation =				(e_Nation)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Country of Affiliation. If 0, none. One

	// related to knights
	const int iKnightsID =					CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Templar ID
	auto eKnightsDuty =	(e_KnightsDuty)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Country of Affiliation. If 0, none. One
	const int iKnightNameLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // The length of the name of the Knights Templar.
	std::string szKnightsName;
	CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szKnightsName, iKnightNameLen);
	const int iKnightsGrade = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);			// Rating
	const int iKnightsRank = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);			// ranking

	const int iLevel =					CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // level...
	const auto eRace =					(e_Race)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	const auto eClass =				(e_Class)CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const float fXPos	=					(CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	const float fZPos	=					(CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	float fYPos	=					(CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;

	const float fYTerrain = ACT_WORLD->GetHeightWithTerrain(fXPos, fZPos);	// Get the height of the terrain...
	const float fYObject = ACT_WORLD->GetHeightNearstPosWithShape(__Vector3(fXPos, fYPos, fZPos), 1.0f); // Get the height closest to the object.
	if (fYObject > fYTerrain) fYPos = fYObject;
	else fYPos = fYTerrain;

	const int iFace = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // hair..
	const int iHair = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // face shape

	const int iStatus = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // 1 - Standing.. 2 - Sitting.. 3 ... Dying..
	const int iStatusSize = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // 0 - normal size, 1 - large. 2 - It got smaller..

	const int iRecruitParty = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // 1 - Moderate. 2 - Looking for a party...
	const BYTE byAuthority = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // authority...

	DWORD	dwItemIDs[MAX_ITEM_SLOT_OPC];  // Equipped items - Other players (including NPCs) 0 ~ 4 upper body, lower body, helmet, arm, foot 5 cloak 6 right hand 7 left hand
	int		iItemDurabilities[MAX_ITEM_SLOT_OPC]; // Durability of worn items..
	for(int i = 0; i < MAX_ITEM_SLOT_OPC; i++)
	{
		dwItemIDs[i] = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);			// The ID of the item being worn
		iItemDurabilities[i] = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	// The current durability of the items you are wearing
	}

	if ( iID == s_pPlayer->IDNumber()) 
		return false; 									// If it&#39;s my packet .. // Ignore it..

	CPlayerOther* pUPC = s_pOPMgr->UPCGetByID(iID, false);
	if(pUPC) // If you already have a character with the same ID...
	{
		CLogWriter::Write("User In - Duplicated ID (%d, %s) Pos(%.2f,%.2f,%.2f)", iID, szName.c_str(), fXPos, fYPos, fZPos);
		TRACE("User In - Duplicated ID (%d, %s) Pos(%.2f,%.2f,%.2f)\n", iID, szName.c_str(), fXPos, fYPos, fZPos);

		pUPC->Action(PSA_BASIC, true, nullptr, true); // forced to live...
		pUPC->m_fTimeAfterDeath = 0;
		pUPC->PositionSet(__Vector3(fXPos, fYPos, fZPos), true);
		return false;
	}

	const D3DCOLOR crID = (eNation != s_pPlayer->m_InfoBase.eNation) ? D3DCOLOR_XRGB(255,96,96) : D3DCOLOR_XRGB(128,128,255); // Apply different colors depending on the country

	pUPC = new CPlayerOther();
	pUPC->IDSet(iID, szName, crID);
	pUPC->m_InfoBase.eNation = eNation;
	pUPC->m_InfoBase.eClass = eClass;
	pUPC->m_InfoBase.iLevel = iLevel;
	pUPC->m_InfoBase.iAuthority = byAuthority;
	pUPC->Init(eRace, iFace, iHair, dwItemIDs, iItemDurabilities);
	pUPC->RotateTo(D3DXToRadian(rand()%360), true);
	pUPC->KnightsInfoSet(iKnightsID, szKnightsName, iKnightsGrade, iKnightsRank);

	// __KnightsInfoBase* pKIB = m_pUIKnightsOp->KnightsInfoFind(iKightsID);
	// if(pKIB) pUPC->KnightsNameSet(pKIB->szName, 0xffff0000);

	pUPC->PositionSet(__Vector3(fXPos, fYPos, fZPos), true);			// Set another player&#39;s current location.
	pUPC->MoveTo(fXPos, fYPos, fZPos, 0, 0);					// Current location..
	pUPC->RotateTo(D3DXToRadian(rand()%360), true);
	s_pOPMgr->UPCAdd(pUPC);										// Add character...

	// if(bWithFX)
	if(bWithFX && pUPC->m_InfoBase.iAuthority != AUTHORITY_MANAGER)
	{
		if(eNation==NATION_KARUS) CGameProcedure::s_pFX->TriggerBundle(iID, -1, FXID_REGEN_KARUS, iID, -1);
		else if(eNation==NATION_ELMORAD) CGameProcedure::s_pFX->TriggerBundle(iID, -1, FXID_REGEN_ELMORAD, iID, -1);
	}

	// Sitting.. standing..
	if(1 == iStatus) // to stand...
	{
		pUPC->Action(PSA_BASIC, true, nullptr, true);
		pUPC->ActionMove(PSM_STOP);
	}
	else if(2 == iStatus) // to sit...
	{
		pUPC->Action(PSA_SITDOWN, true, nullptr, true);
		pUPC->ActionMove(PSM_STOP);
	}
	else if(3 == iStatus) // is dead...
	{
		s_pOPMgr->MoveToCorpsesForcely(pUPC, false); // transfer to the body.
	}

	if(0x01 == iStatusSize)
	{
		pUPC->ScaleSetGradually(1.0f); // normal size..
		pUPC->FlickerFactorSet(1.0f); // Resurrected, translucent... flickering...
	}
	else if(0x02 == iStatusSize) pUPC->ScaleSetGradually(2.0f); // got bigger...
	else if(0x03 == iStatusSize) pUPC->ScaleSetGradually(0.5f);// got smaller
	else if(0x04 == iStatusSize) pUPC->FlickerFactorSet(0.7f); // Resurrected, translucent... flickering...

	if(1 == iRecruitParty)
	{
		pUPC->InfoStringSet("", 0);
	}
	else if(2 == iRecruitParty) // Looking for a party...
	{
		const int iLevel = pUPC->m_InfoBase.iLevel;
		int iLMin = iLevel - 8;
		if(iLMin < 0) iLMin = 0;
		int iLMax = iLevel + 8;
		if(iLMax > 60) iLMax = 60;

		char szBuff[128];
		std::string szMsg;
		::_LoadStringFromResource(IDS_WANT_PARTY_MEMBER, szMsg);
		sprintf(szBuff, szMsg.c_str(), iLMin, iLMax);
		pUPC->InfoStringSet(szBuff, 0xff00ff00);
	}

	return true;
}

bool CGameProcMain::MsgRecv_UserOut(DataPack* pDataPack, int& iOffset)
{
	const int iID	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	// Turns a dead character into a corpse.
	CPlayerNPC* pUPC = s_pOPMgr->UPCGetByID(iID, false);
	if( pUPC == nullptr) return false;
	if( pUPC->IsDead() )
	{// If you perform a UserOut while the character is dead, it will be replaced with a corpse.
		s_pOPMgr->CorpseAdd(iID);
		return true;
	}

	return s_pOPMgr->UPCDelete(iID);					// Remove character...
}

// Receives all IDs in the surrounding area as much as the count... Well.. Requests only those that need to be updated to the server..
bool CGameProcMain::MsgRecv_UserInAndRequest(DataPack* pDataPack, int& iOffset)
{
	const int iUPCCountReceived = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	// TRACE("UPC region update : %d\n", iUPCCountReceived);
	
	if(0 == iUPCCountReceived) return false;
	if(iUPCCountReceived < 0 || iUPCCountReceived >= 1000)
	{
		char szErr[256];
		sprintf(szErr, "영역 요청 오류 - 플레이어 갯수 %d", iUPCCountReceived);
		CGameProcedure::ReportDebugStringAndSendToServer(szErr);
		__ASSERT(0, szErr);
		return false;
	}

	int iID = 0;
	m_SetUPCID.clear(); // Wipe it off...
	auto itIDEnd = m_SetUPCID.end();

	for(auto i = 0; i < iUPCCountReceived; i++ )
	{
		iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		// TRACE("               ID : %d\n", iID);
		if(iID == s_pPlayer->IDNumber()) continue; // It&#39;s my ID.. so don&#39;t put it there.

		m_SetUPCID.insert(iID);
	}

	if(m_SetUPCID.empty()) // If you don&#39;t get anything new, you lose them all.
	{
		s_pOPMgr->ReleaseUPCs();
		return false;
	}

	// Search for the newly received ID and the UPC ID in the list.
	CPlayerOther* pUPC = nullptr;
	auto itUPC = s_pOPMgr->m_UPCs.begin(), itUPCEnd = s_pOPMgr->m_UPCs.end();
	for(; itUPC != itUPCEnd; )
	{
		pUPC = itUPC->second;
		iID = pUPC->IDNumber();
		
		auto itID = m_SetUPCID.find(iID);
		if(itID != itIDEnd) // If you are on the new list...
		{
			m_SetUPCID.erase(itID); // Take it off the request list.
			itUPC++;
		}
		else  // If it doesn&#39;t exist in the new location, delete it.
		{
			// TRACE("           delete : %d\n", iID);

			if(pUPC && pUPC->IsDead()) 
			{
				itUPC++;
			}
			else
			{
				delete pUPC; pUPC = nullptr;
				itUPC = s_pOPMgr->m_UPCs.erase(itUPC);
			}
		}
	}

	// 
	// Create and send request packets.
	const int iNewUPCCount = m_SetUPCID.size();
	if(iNewUPCCount > 0)
	{
		int iOffset=0;														// Offset of buffer..
		std::vector<BYTE> byBuff(iNewUPCCount * 2 + 10, 0);					// Packet buffer...
		CAPISocket::MP_AddByte(&(byBuff[0]), iOffset, N3_REQUEST_USER_IN);	// command.
		CAPISocket::MP_AddShort(&(byBuff[0]), iOffset, iNewUPCCount);		// number of ids.
		
		auto itID = m_SetUPCID.begin(); itIDEnd = m_SetUPCID.end();
		for(auto i = 0; itID != itIDEnd; itID++, i++)
		{
			iID = *itID;
			CAPISocket::MP_AddShort(&(byBuff[0]), iOffset, iID);			// IDs for which detailed information is required.
		}
		s_pSocket->Send(&(byBuff[0]), iOffset); // send
	}
	// Create and send request packets.
	// 

	return true;
}

bool CGameProcMain::MsgRecv_UserInRequested(DataPack* pDataPack, int& iOffset)
{
	const int iPlayerCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	if(0 == iPlayerCount) return false;
	if(iPlayerCount < 0 || iPlayerCount >= 1000)
	{
		char szErr[256];
		sprintf(szErr, "영역정보 받기 오류 - 플레이어 갯수 %d", iPlayerCount);
		CGameProcedure::ReportDebugStringAndSendToServer(szErr);
		__ASSERT(0, szErr);
		return false;
	}

#ifdef _DEBUG
	const float fTime = CN3Base::TimeGet();
#endif

	// int iOffset2 = iOffset;
	for(auto i = 0; i < iPlayerCount; i++ )
	{
		this->MsgRecv_UserIn(pDataPack, iOffset); // The number of users equal to the number of players...
	}

#ifdef _DEBUG
	TRACE("  User In Requested (Count:%d) : Avg(%.4f Sec)\n", iPlayerCount, (CN3Base::TimeGet() - fTime)/iPlayerCount);
#endif

	return true;
}


bool CGameProcMain::MsgRecv_NPCInOut(DataPack* pDataPack, int& iOffset)
{
	const BYTE byType		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	if ( byType == 0x01 ) // When an NPC enters
	{
		return this->MsgRecv_NPCIn(pDataPack, iOffset); // NPC In Handling
	}
	else // when the NPC leaves.
	{
		return this->MsgRecv_NPCOut(pDataPack, iOffset);
	}

	return true;
}

bool CGameProcMain::MsgRecv_NPCIn(DataPack* pDataPack, int& iOffset)
{
	const int		iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Unique ID managed by Server
	const int		iIDResrc	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // resource ID
	int		iType		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// NPC Type - 0x05 : Merchant
	int		iItemTrdeID	= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);	// Group ID to trade items ID to request to the server
	const int		iScale		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // scale 100 is 1.0
	const int		iItemID0	= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset); // resource ID
	const int		iItemID1	= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset); // resource ID
	const int 	iNameLen	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	std::string szName;									// NPC ID.
	if(iNameLen > 0) CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szName, iNameLen );
	else szName = "";

	// #ifdef _DEBUG
	// CLogWriter::Write("NPC In - ID(%d) Name(%s) Time(%.1f)", iID, szName.c_str(), CN3Base::TimeGet()); // 캐릭 세팅..
	// #endif

	const auto eNation = (e_Nation)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Country of Affiliation. If 0, none. One
	const int iLevel		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	const float fXPos	= (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	const float fZPos	= (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	float fYPos	= (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;

	const float fYTerrain = ACT_WORLD->GetHeightWithTerrain(fXPos, fZPos);	// Get the height of the terrain...
	const float fYObject = ACT_WORLD->GetHeightNearstPosWithShape(__Vector3(fXPos, fYPos, fZPos), 1.0f); // Get the height closest to the object.
	if (fYObject > fYTerrain) fYPos = fYObject;
	else fYPos = fYTerrain;

	const DWORD dwStatus =	CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset); // State... Write in various ways or calculations. 0 door open, 1 closed. 2, 4, 8, 16 ....
	const DWORD dwType =		CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Type... If 0, character type NPC, if 1, object type NPC

	CPlayerNPC* pNPC = s_pOPMgr->NPCGetByID(iID, false);
	if(pNPC) // If you already have a character with the same ID...
	{
		CLogWriter::Write("NPC In - Duplicated ID (%d, %s) Pos(%.2f,%.2f,%.2f)", iID, szName.c_str(), fXPos, fYPos, fZPos);
		TRACE("NPC In - Duplicated ID (%d, %s) Pos(%.2f,%.2f,%.2f)\n", iID, szName.c_str(), fXPos, fYPos, fZPos);
		pNPC->Action(PSA_BASIC, true, nullptr, true); // forced to live...
		pNPC->m_fTimeAfterDeath = 0;
		pNPC->PositionSet(__Vector3(fXPos, fYPos, fZPos), true);
		return false;
	}

	const D3DCOLOR crID = (eNation != s_pPlayer->m_InfoBase.eNation) ? D3DCOLOR_XRGB(255,128,128) : D3DCOLOR_XRGB(192,192,255);

	pNPC = new CPlayerNPC();
	pNPC->IDSet(iID, szName, crID);				// Initialization.. and ID setting.
	pNPC->m_InfoBase.eNation = eNation;
	pNPC->m_InfoBase.eRace = RACE_NPC;			// Announce that you are an NPC.
	pNPC->m_InfoBase.iLevel = iLevel;
	pNPC->m_InfoBase.iAuthority = AUTHORITY_NPC;// Privilege NPCs do not have privileges..
	
	s_pOPMgr->NPCAdd(pNPC);						// Add character...

	// Now, according to the packet, the character is decorated..(?).. Item mounting, weapon mounting, etc...
	CN3Shape* pShape = nullptr;
	__TABLE_PLAYER_LOOKS* pLooks = nullptr;
	if(0 == dwType)
	{
		pLooks = s_pTbl_NPC_Looks->Find(iIDResrc);	// default skin...
		if(nullptr == pLooks) // If there is no character basic appearance table...
		{
			pLooks = s_pTbl_NPC_Looks->GetIndexedData(0);
			char szBuff[256];
			sprintf(szBuff, "Normal NPC In : [Name(%s), ResourceID(%d)]", szName.c_str(), iIDResrc);
			this->MsgOutput(szBuff, 0xffff00ff);
			CLogWriter::Write(szBuff);
		}
	}
	else
	{
		pShape = ACT_WORLD->ShapeGetByIDWithShape(iIDResrc); // Look for Object NPCs...
		if(nullptr == pShape)
		{
			char szBuff[256];
			sprintf(szBuff, "Object NPC In : [Name(%s), ResourceID(%d)]", szName.c_str(), iIDResrc);
			this->MsgOutput(szBuff, 0xffff00ff);
			CLogWriter::Write(szBuff);
			pLooks = s_pTbl_NPC_Looks->GetIndexedData(0);
		}
	}
		
	if(pLooks)
	{
		pNPC->InitChr(pLooks);	// Joint setting...
		pNPC->ScaleSet(iScale / 100.0f);

		std::string szPartFN;
		const e_PartPosition eParts[6] = { PART_POS_UPPER, PART_POS_LOWER, PART_POS_FACE, PART_POS_HANDS, PART_POS_FEET, PART_POS_HAIR_HELMET };
		for(int i = 0; i < 6; i++)
		{
			pNPC->PartSet(eParts[i], pLooks->szPartFNs[eParts[i]], nullptr, nullptr);
		}

		if(iItemID0)
		{
			__TABLE_ITEM_BASIC* pItem0 = s_pTbl_Items_Basic->Find(iItemID0/1000*1000);
			__TABLE_ITEM_EXT* pItemExt0 = nullptr;
			if(pItem0 && pItem0->byExtIndex >= 0 && pItem0->byExtIndex < MAX_ITEM_EXTENSION)
				pItemExt0 = s_pTbl_Items_Exts[pItem0->byExtIndex]->Find(iItemID0%1000);	// Get column data...
			if(pItem0 && pItemExt0)
			{
				e_PartPosition ePart;
				e_PlugPosition ePlug;
				std::string szItemFN;
				CGameProcedure::MakeResrcFileNameForUPC(pItem0, &szItemFN, nullptr, ePart, ePlug);
				pNPC->PlugSet(PLUG_POS_RIGHTHAND, szItemFN, pItem0, pItemExt0);
			}
			else
			{
				__ASSERT(0, "Invalid Item ID And Extension");
			}
		}

		if(iItemID1)
		{
			__TABLE_ITEM_BASIC* pItem1 = s_pTbl_Items_Basic->Find(iItemID1/1000*1000);
			__TABLE_ITEM_EXT* pItemExt1 = nullptr;
			if(pItem1 && pItem1->byExtIndex >= 0 && pItem1->byExtIndex < MAX_ITEM_EXTENSION)
				pItemExt1 = s_pTbl_Items_Exts[pItem1->byExtIndex]->Find(iItemID1%1000);	// Get column data...
			if(pItem1 && pItemExt1)
			{
				e_PartPosition ePart;
				e_PlugPosition ePlug;
				std::string szItemFN;
				CGameProcedure::MakeResrcFileNameForUPC(pItem1, &szItemFN, nullptr, ePart, ePlug);
				pNPC->PlugSet(PLUG_POS_LEFTHAND, szItemFN, pItem1, pItemExt1);
			}
			else
			{
				__ASSERT(0, "Invalid Item ID And Extension");
			}
		}
	}
	else if(pShape && (pShape->Type() & OBJ_SHAPE_EXTRA))
	{
		auto* pSE = (CN3ShapeExtra*)pShape;
		pNPC->m_pShapeExtraRef = pSE; // Setting the reference object...
		pSE->m_bVisible = true; // Disable the flag that forces it not to render..
		
		bool bShouldBeRotate = true;
		__Vector3 vAxis(0,1,0);
		float fRadian = 0, fRadian2 = 0;
		bool bVisible = false;

		if(OBJECT_TYPE_DOOR_LEFTRIGHT == pSE->m_iEventType) // left and right open gates
		{
			vAxis.Set(0,1,0);
			fRadian = D3DXToRadian(80);
			fRadian2 = 0;
		} 
		else if(OBJECT_TYPE_DOOR_TOPDOWN == pSE->m_iEventType)
		{
			vAxis.Set(0,0,1);
			fRadian = D3DXToRadian(90);
			fRadian2 = 0;
		} // upper and lower opening gates
		else if(OBJECT_TYPE_LEVER_TOPDOWN == pSE->m_iEventType)
		{
			vAxis.Set(1,0,0);
			fRadian = D3DXToRadian(45);
			fRadian2 = D3DXToRadian(-45);
		} // up and down lever
		else if(OBJECT_TYPE_FLAG == pSE->m_iEventType)
		{
			bShouldBeRotate = false;
		}

		if(0x01 == dwStatus) // If you make it visible or open left and right...
		{
			if(bShouldBeRotate) // Heat
			{
				pSE->RotateTo(0, vAxis, fRadian, 1, true); // open right away.
				pSE->RotateTo(1, vAxis, -fRadian, 1, true); // open right away.
			}
			else // show
			{
				pSE->m_bVisible = true;
			}
		}
		else if(0x00 == dwStatus) // made invisible or closed.
		{
			if(bShouldBeRotate) // close
			{
				pSE->RotateTo(0, vAxis, fRadian2, 1, true); // open right away.
				pSE->RotateTo(1, vAxis, -fRadian2, 1, true); // open right away.
			}
			else // not to be seen
			{
				pSE->m_bVisible = false;
			}
		}
	}
	else
	{
		char szBuff[256];
		sprintf(szBuff, "No Resource NPC In : [Name(%s), ResourceID(%d)]", szName.c_str(), iIDResrc);
		CLogWriter::Write(szBuff);
	}

	pNPC->PositionSet(__Vector3(fXPos, fYPos, fZPos), true);	// Set your current location...
	pNPC->MoveTo(fXPos, fYPos, fZPos, 0, 0);					// Current location..
	pNPC->RotateTo(D3DXToRadian(rand()%360), false);
	pNPC->Action(PSA_BASIC, true, nullptr, true);
	pNPC->ActionMove(PSM_STOP);

	return true;
}

bool CGameProcMain::MsgRecv_NPCOut(DataPack* pDataPack, int& iOffset)
{
	const int  iID		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Unique ID managed by Server
	return s_pOPMgr->NPCDelete(iID);										// Remove character...
}


// Receives all IDs in the surrounding area as much as the count... Well.. Requests only those that need to be updated to the server..
bool CGameProcMain::MsgRecv_NPCInAndRequest(DataPack* pDataPack, int& iOffset)
{
	const int iNPCCountReceived = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	TRACE("NPC region update : %d\n", iNPCCountReceived);

	if(0 == iNPCCountReceived) return false;
	if(iNPCCountReceived < 0 || iNPCCountReceived >= 1000)
	{
		char szErr[256];
		sprintf(szErr, "영역정보 요청 오류 - NPC 갯수 %d", iNPCCountReceived);
		CGameProcedure::ReportDebugStringAndSendToServer(szErr);
		__ASSERT(0, szErr);
		return false;
	}

	int iID = 0;
	m_SetNPCID.clear(); // Wipe it off...
	auto itIDEnd = m_SetNPCID.end();

	for(auto i = 0; i < iNPCCountReceived; i++ )
	{
		iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		m_SetNPCID.insert(iID);
		TRACE("               ID : %d\n", iID);
	}

	if(m_SetNPCID.empty()) // If you don&#39;t get anything new, you lose them all.
	{
		s_pOPMgr->ReleaseNPCs();
		return false;
	}

	// Search for the new ID and the NPC ID in the list.
	CPlayerNPC* pNPC = nullptr;
	auto itNPC = s_pOPMgr->m_NPCs.begin(), itNPCEnd = s_pOPMgr->m_NPCs.end();
	for(; itNPC != itNPCEnd; )
	{
		pNPC = itNPC->second;
		iID = pNPC->IDNumber();
		
		auto itID = m_SetNPCID.find(iID);
		if(itID != itIDEnd) // If you are on the new list...
		{
			m_SetNPCID.erase(itID); // Take it off the request list.
			itNPC++;
		}
		else  // If it doesn&#39;t exist in the new location, delete it.
		{
			TRACE("           delete : %d\n", iID);

			if(pNPC && pNPC->IsDead()) 
			{
				itNPC++;
			}
			else
			{
				delete pNPC; pNPC = nullptr;
				itNPC = s_pOPMgr->m_NPCs.erase(itNPC);
			}
		}
	}

	// 
	// Create and send request packets.
	const int iNewNPCCount = m_SetNPCID.size();
	if(iNewNPCCount > 0)
	{
		int iOffset=0;														// Offset of buffer..
		std::vector<BYTE> byBuff(iNewNPCCount * 2 + 10, 0);					// Packet buffer...
		CAPISocket::MP_AddByte(&(byBuff[0]), iOffset, N3_REQUEST_NPC_IN);	// command.
		CAPISocket::MP_AddShort(&(byBuff[0]), iOffset, iNewNPCCount);		// number of ids.
		
		auto itID = m_SetNPCID.begin(); itIDEnd = m_SetNPCID.end();
		for(auto i = 0; itID != itIDEnd; itID++, i++)
		{
			iID = *itID;
			CAPISocket::MP_AddShort(&(byBuff[0]), iOffset, iID);			// IDs for which detailed information is required.
		}
		s_pSocket->Send(&(byBuff[0]), iOffset); // send
	}
	// Create and send request packets.
	// 

	return true;
}

bool CGameProcMain::MsgRecv_NPCInRequested(DataPack* pDataPack, int& iOffset)
{
	const int iNPCCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	if(0 == iNPCCount) return false;
	if(iNPCCount < 0 || iNPCCount >= 1000)
	{
		char szErr[256];
		sprintf(szErr, "영역정보 받기오류 - NPC 갯수 %d", iNPCCount);
		CGameProcedure::ReportDebugStringAndSendToServer(szErr);
		__ASSERT(0, szErr);
		return false;
	}

#ifdef _DEBUG
	const float fTime = CN3Base::TimeGet();
#endif

	for(auto i = 0; i < iNPCCount; i++ )
	{
		this->MsgRecv_NPCIn(pDataPack, iOffset); // The number of users equal to the number of players...
	}
	
#ifdef _DEBUG
	TRACE("  NPC In Requested (Count:%d) : Avg(%.4f Sec)\n", iNPCCount, (CN3Base::TimeGet() - fTime)/iNPCCount);
#endif

	return true;
}

bool CGameProcMain::MsgRecv_NPCMove(DataPack* pDataPack, int& iOffset)
{
	const int iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const float fXPos		= (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	const float fZPos		= (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	float fYPos		= (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;
	const float fSpeed	= (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;

	// The part where the function should come.. ^^
	CPlayerNPC* pNPC = nullptr;
	pNPC = s_pOPMgr->NPCGetByID(iID, true);				// Get an NPC as an ID...
	if (nullptr == pNPC )
	{
		this->MsgSend_NPCInRequest(iID);
		return false;		// If there is a living NPC...
	}

	const float fY = ACT_WORLD->GetHeightWithTerrain(fXPos, fZPos);		// Terrain height value..
	const int iMoveMode = (fSpeed > 0) ? 2 : 0;
	pNPC->MoveTo(fXPos, fY, fZPos, fSpeed, iMoveMode);	// NPC location updates..

	__ASSERT(!(iMoveMode && fSpeed == 0), "Invalid NPC Move Packet");

	return true;
}

bool CGameProcMain::MsgRecv_Attack(DataPack* pDataPack, int& iOffset)
{
	const int iType		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // 0x01 - physical attack, 0x02 magic attack 0x03, continuous magic attack
	const int iResult		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // 0x00 failure, 0x01 success
	const int iIDAttacker = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // over attacked
	const int iIDTarget	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Being targeted and being attacked.

	if ( iIDAttacker == iIDTarget ) return false;		// What if the attacker and the victim are the same????

	const bool bIAmTarget = (iIDTarget == s_pPlayer->IDNumber()) ? true : false;
	const bool bIAmAttacker = (iIDAttacker == s_pPlayer->IDNumber()) ? true : false;
	
	CPlayerBase* pAttacker = nullptr;
	if ( bIAmAttacker ) pAttacker = s_pPlayer;	// If the attacker is myself
	else pAttacker = s_pOPMgr->CharacterGetByID(iIDAttacker, true);
	if(nullptr == pAttacker) // Hey, there&#39;s no one attacking you??
	{
		if(iIDAttacker > 10000) // NPC is over 1000.
			this->MsgSend_NPCInRequest(iIDAttacker); // If there is no NPC information, request it.
		else if(iIDAttacker < 3000)
			this->MsgSend_UserInRequest(iIDAttacker); // If there is no NPC information, request it.
		return false; // If there are no attackers, return.
	}

	CPlayerBase* pTarget = nullptr;
	if(bIAmTarget) // If the target is me...
	{
		this->CommandSitDown(false, false); // stand up
		if(m_pMagicSkillMng->IsCasting())
		{
			const __TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(s_pPlayer->m_dwMagicID);
			if(pSkill)
			{
				const int SuccessValue = rand()%100;
				if(SuccessValue >= pSkill->iPercentSuccess) // Failure according to the probability shown in the skill table.
					s_pPlayer->Action(PSA_BASIC, false, nullptr, true); // Casting cancellation, forced setting as default action..
			}
		}
		pTarget = s_pPlayer;
	}
	else // If the target is different...
	{
		pTarget = s_pOPMgr->CharacterGetByID(iIDTarget, true); // First of all, take it from among the living ones..
		if(nullptr == pTarget) pTarget = s_pOPMgr->CharacterGetByID(iIDTarget, false); // If you don&#39;t have it, grab it as a target regardless of dying
	}

	if(nullptr == pTarget) return false; // No target!!!!

	// pTarget-&gt;m_iIDTargetMe = iIDAttacker; // who&#39;s attacking??

	if(pAttacker != s_pPlayer && pAttacker && pAttacker->IsAlive()) // When the attacking nim is not me but another living nim...
	{
		((CPlayerNPC*)pAttacker)->RotateTo(pTarget); // Calculate direction towards target
		pAttacker->m_iIDTarget = iIDTarget; // Set Target ID...
		if(0x01 == iType) pAttacker->Action(PSA_ATTACK, false, pTarget); // direct physical attack.
		else if(0x02 == iType) pAttacker->Action(PSA_SPELLMAGIC, false, pTarget); // magic attack...
		// else if(0x03 == iType) pAttacker-&gt;Action(PSA_SPELLMAGIC, false, pTarget); // Continuous magic attack...
	}

	pTarget->m_bGuardSuccess = false; // A flag for whether the defense was successful.
	if(0x0 == iResult) // attack failed
	{
		if(pTarget->IsAlive() && 0 == pTarget->m_iSkillStep) // If you are not dead and you are not using a skill, it will act as a block.
		{
			pTarget->m_bGuardSuccess = true; // A flag for whether the defense was successful.
			pTarget->Action(PSA_GUARD, false);
		}

		if(pAttacker == s_pPlayer) // If the attacker is the player himself...
		{
			char szBuf[128] = "";
			std::string szFmt; ::_LoadStringFromResource(IDS_MSG_FMT_TARGET_ATTACK_FAILED, szFmt);
			sprintf(szBuf, szFmt.c_str(), pTarget->IDString().c_str());
			MsgOutput(szBuf, 0xffffffff);
		}
	}
	else if(0x2 == iResult) // Attack And Dead - Die from this attack!!!
	{
		if(pTarget == s_pPlayer)
		{
			// if(m_pUIDead) m_pUIDead->SetVisible(true);
			std::string szMsg; ::_LoadStringFromResource(IDS_REGENERATION, szMsg);
			CGameProcedure::MessageBoxPost(szMsg, "", MB_OK, BEHAVIOR_REGENERATION); // Send recreate message..
		}
		
		pTarget->m_fTimeAfterDeath = 0.1f; // Before dying...
		if(pTarget->m_pShapeExtraRef) s_pOPMgr->CharacterDelete(pTarget->IDNumber()); // When it is an object, it is destroyed..!!
	}

	if(pTarget == s_pPlayer) 
	{
		TRACE("player is under of attack (%d)\n", iIDAttacker);
	}

	return true;
}

bool CGameProcMain::MsgRecv_Dead(DataPack* pDataPack, int& iOffset)
{
	const int iIDTarget	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Being targeted and being attacked.

	
	CPlayerBase* pTarget = nullptr;
	if(iIDTarget == s_pPlayer->IDNumber())
	{
		pTarget = s_pPlayer;

		// if(m_pUIDead) m_pUIDead->SetVisible(true);
		std::string szMsg; ::_LoadStringFromResource(IDS_REGENERATION, szMsg);
		CGameProcedure::MessageBoxPost(szMsg, "", MB_OK, BEHAVIOR_REGENERATION); // Send recreate message..
		CLogWriter::Write("Dead!!!");
	}
	else
	{
		pTarget = this->CharacterGetByID(iIDTarget, false);
	}

	if(pTarget)
	{
		if(pTarget->m_pShapeExtraRef) s_pOPMgr->CharacterDelete(pTarget->IDNumber());
		else
		{
			pTarget->m_fTimeAfterDeath = 0.1f; // Before dying...
			pTarget->ActionDying(PSD_KEEP_POSITION, __Vector3(0,0,1));
		}

		return true;
	}

	return false;
}

bool CGameProcMain::MsgRecv_ItemMove(DataPack* pDataPack, int& iOffset)
{
	__InfoPlayerMySelf* pInfoExt = &(s_pPlayer->m_InfoExt);
	__InfoPlayerBase* pInfoBase = &(s_pPlayer->m_InfoBase);

	const BYTE bResult =		CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// 0x01 : true, 0x00 : false..
	if (bResult)
	{
		pInfoExt->iAttack = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		pInfoExt->iGuard =	CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		pInfoExt->iWeightMax = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		
		pInfoBase->iHPMax = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		pInfoExt->iMSPMax = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		
		// It is the value added or subtracted by the item.
		pInfoExt->iStrength_Delta =		CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		pInfoExt->iStamina_Delta	=	CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		pInfoExt->iDexterity_Delta =	CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		pInfoExt->iIntelligence_Delta =	CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		pInfoExt->iMagicAttak_Delta =	CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

		pInfoExt->iRegistFire =		CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		pInfoExt->iRegistCold =		CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		pInfoExt->iRegistLight =	CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		pInfoExt->iRegistMagic =	CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		pInfoExt->iRegistCurse =	CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		pInfoExt->iRegistPoison	=	CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

		if(pInfoBase->iHP > pInfoBase->iHPMax) pInfoBase->iHP = pInfoBase->iHPMax; // range check...
		if(pInfoExt->iMSP > pInfoExt->iMSPMax) pInfoExt->iMSP = pInfoExt->iMSPMax; // range check...

		m_pUIVar->m_pPageState->UpdateHP(pInfoBase->iHP, pInfoBase->iHPMax);
		m_pUIVar->m_pPageState->UpdateMSP(pInfoExt->iMSP, pInfoExt->iMSPMax);

		m_pUIStateBarAndMiniMap->UpdateHP(pInfoBase->iHP, pInfoBase->iHPMax, false);
		m_pUIStateBarAndMiniMap->UpdateMSP(pInfoExt->iMSP, pInfoExt->iMSPMax, false);
		
		m_pUIVar->m_pPageState->UpdateAttackPoint(pInfoExt->iAttack, pInfoExt->iAttack_Delta);
		m_pUIVar->m_pPageState->UpdateGuardPoint(pInfoExt->iGuard, pInfoExt->iGuard_Delta);
		m_pUIVar->m_pPageState->UpdateWeight(pInfoExt->iWeight, pInfoExt->iWeightMax);
		
		m_pUIVar->m_pPageState->UpdateStrength(pInfoExt->iStrength, pInfoExt->iStrength_Delta);
		m_pUIVar->m_pPageState->UpdateStamina(pInfoExt->iStamina, pInfoExt->iStamina_Delta);
		m_pUIVar->m_pPageState->UpdateDexterity(pInfoExt->iDexterity, pInfoExt->iDexterity_Delta);
		m_pUIVar->m_pPageState->UpdateIntelligence(pInfoExt->iIntelligence, pInfoExt->iIntelligence_Delta);
		m_pUIVar->m_pPageState->UpdateMagicAttak(pInfoExt->iMagicAttak, pInfoExt->iMagicAttak_Delta);

		m_pUIVar->m_pPageState->UpdateRegistFire(pInfoExt->iRegistFire, pInfoExt->iRegistFire_Delta);
		m_pUIVar->m_pPageState->UpdateRegistCold(pInfoExt->iRegistCold, pInfoExt->iRegistCold_Delta);
		m_pUIVar->m_pPageState->UpdateRegistLight(pInfoExt->iRegistLight, pInfoExt->iRegistLight_Delta);
		m_pUIVar->m_pPageState->UpdateRegistMagic(pInfoExt->iRegistMagic, pInfoExt->iRegistMagic_Delta);
		m_pUIVar->m_pPageState->UpdateRegistCurse(pInfoExt->iRegistCurse, pInfoExt->iRegistCurse_Delta);
		m_pUIVar->m_pPageState->UpdateRegistPoison(pInfoExt->iRegistPoison, pInfoExt->iRegistPoison_Delta);
	}

	m_pUIInventory->ReceiveResultFromServer(bResult);
	TRACE("Init Inv Move Res %d \n", (int)bResult);

	return true;
}

bool CGameProcMain::MsgRecv_ItemWeightChange(DataPack* pDataPack, int& iOffset)		// Item weight changes.
{
	__InfoPlayerMySelf* pInfoExt = &(s_pPlayer->m_InfoExt);
	pInfoExt->iWeight = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	m_pUIVar->m_pPageState->UpdateWeight(pInfoExt->iWeight, pInfoExt->iWeightMax);

	return true;
}

bool CGameProcMain::MsgRecv_UserLookChange(DataPack* pDataPack, int& iOffset)
{
	const int iID				= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const auto eSlot	= (e_ItemSlot)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	const DWORD dwItemID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	const int iDurability = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	if(iID == s_pPlayer->IDNumber()) return false;
	
	CPlayerOther* pUPC = s_pOPMgr->UPCGetByID(iID, false);
	if(nullptr == pUPC) return false;

	__TABLE_ITEM_BASIC* pItem = s_pTbl_Items_Basic->Find(dwItemID/1000*1000);

	__TABLE_ITEM_EXT* pItemExt = nullptr;
	if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
		pItemExt = s_pTbl_Items_Exts[pItem->byExtIndex]->Find(dwItemID%1000);	// Get column data...
	if(dwItemID && (nullptr == pItem || nullptr == pItemExt))
	{
		__ASSERT(0, "NULL Item!!!");
		return false;
	}

	e_PartPosition ePartPos = PART_POS_UNKNOWN, ePartPos2 = PART_POS_UNKNOWN;
	e_PlugPosition ePlugPos = PLUG_POS_UNKNOWN, ePlugPos2 = PLUG_POS_UNKNOWN;

	if(ITEM_SLOT_HEAD == eSlot) ePartPos = PART_POS_HAIR_HELMET;
	else if(ITEM_SLOT_UPPER == eSlot) ePartPos = PART_POS_UPPER;
	else if(ITEM_SLOT_LOWER == eSlot) ePartPos = PART_POS_LOWER;
	else if(ITEM_SLOT_GLOVES == eSlot) ePartPos = PART_POS_HANDS;
	else if(ITEM_SLOT_SHOES == eSlot) ePartPos = PART_POS_FEET;
	else if(ITEM_SLOT_HAND_RIGHT == eSlot) ePlugPos = PLUG_POS_RIGHTHAND;
	else if(ITEM_SLOT_HAND_LEFT == eSlot) ePlugPos = PLUG_POS_LEFTHAND;

	if(ePartPos != PART_POS_UNKNOWN) 
	{
		if(dwItemID) // if there is an item
		{
			std::string szItemFN;
			CGameProcedure::MakeResrcFileNameForUPC(pItem, &szItemFN, nullptr, ePartPos2, ePlugPos2);
			pUPC->PartSet(ePartPos, szItemFN, pItem, pItemExt); // Paste the item...
			pUPC->DurabilitySet(eSlot, iDurability);
		}
		else
		{
			const __TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks->Find(pUPC->m_InfoBase.eRace);	// User Player Character Skin structure pointer..
			if(nullptr == pLooks)
			{
				CLogWriter::Write("CGameProcMain::MsgRecv_UserLookChange() - failed find table : Race (%d)", pUPC->m_InfoBase.eRace);
				__ASSERT(pLooks, "failed find table");
			}
			else
			{
				if(PART_POS_HAIR_HELMET == ePartPos) pUPC->InitHair(); // If you don&#39;t have a head...
				else pUPC->PartSet(ePartPos, pLooks->szPartFNs[ePartPos], nullptr, nullptr);
			}
		}

		return true;
	}
	else if(ePlugPos != PLUG_POS_UNKNOWN)
	{
		if(dwItemID)
		{
			std::string szItemFN;
			CGameProcedure::MakeResrcFileNameForUPC(pItem, &szItemFN, nullptr, ePartPos2, ePlugPos2);
			pUPC->PlugSet(ePlugPos, szItemFN, pItem, pItemExt);
			pUPC->DurabilitySet(eSlot, iDurability);
		}
		else
		{
			pUPC->PlugSet(ePlugPos, "", nullptr, nullptr);
			pUPC->DurabilitySet(eSlot, 0);
		}

		// If it is within the sound range, the sound changes.
		const __Vector3 vPosPlayer = s_pPlayer->Position();
		const float fDist = pUPC->Distance(vPosPlayer);
		if(fDist < SOUND_RANGE_TO_SET) pUPC->SetSoundPlug(pItem); // If inside SOUND_RANGE.

		return true;
	}
	else
	{
		CLogWriter::Write("CGameProcMain::MsgRecv_UserLookChange() - Unknown Slot(%d) Item(%d)", eSlot, dwItemID);
		return false;
	}

	return false;
}

bool CGameProcMain::MsgRecv_ItemBundleDrop(DataPack* pDataPack, int& iOffset)		// Response to Item appearing in field
{
	const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const int iItemID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);

	CPlayerBase* pCorpse = s_pOPMgr->NPCGetByID(iID, false);
	if(nullptr == pCorpse) pCorpse = s_pOPMgr->CorpseGetByID(iID);
	
	if(pCorpse)
		pCorpse->m_iDroppedItemID = iItemID; // Dropped Item ID Bundle

	return true;
}

bool CGameProcMain::MsgRecv_ItemBundleOpen(DataPack* pDataPack, int& iOffset)		// Open item boxes or search corpses.
{
	DWORD dwItemID = 0;
	int iItemCount = 0;

	const POINT ptCur = s_pLocalInput->MouseGetPos();
	m_pUIDroppedItemDlg->EnterDroppedState(ptCur.x, ptCur.y);
	TRACE ("아이템 리스트 도착.. \n");

	for(int i = 0; i < MAX_ITEM_BUNDLE_DROP_PIECE; i++)
	{
		dwItemID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
		iItemCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

		// Open the monster item window in this part and prepare it.
		if ( dwItemID )
			m_pUIDroppedItemDlg->AddToItemTable(dwItemID, iItemCount, i);
	}

	m_pUIDroppedItemDlg->InitIconUpdate();

	return true;
}

void CGameProcMain::MsgRecv_ItemRepair(DataPack* pDataPack, int& iOffset)			// Item Repair Result..
{
	const int iResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// Trade id
	const int iGold   = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);		// Trade id
	m_pUIInventory->ReceiveResultFromServer( iResult, iGold );
}

void CGameProcMain::MsgRecv_ItemCountChange(DataPack* pDataPack, int& iOffset)		// Item Count Change..
{
	const int iTotalCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// Trade id

	for(auto i = 0; i < iTotalCount; i++ )
	{
		const int iDistrict	= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// Trade id
		const int iIndex		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// Trade id
		const int iID			  = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);		// Trade id
		const int	iCount		= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);		// Trade id
		m_pUIInventory->ItemCountChange(iDistrict, iIndex, iCount, iID);
	}	

	if (m_pUISkillTreeDlg) m_pUISkillTreeDlg->UpdateDisableCheck();
	if (m_pUIHotKeyDlg) m_pUIHotKeyDlg->UpdateDisableCheck();
}

void CGameProcMain::MsgRecv_ItemDestroy(DataPack* pDataPack, int& iOffset)			// Item Count Change..
{
	const int	iResult		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// Trade id
	m_pUIInventory->ReceiveResultItemRemoveFromServer(iResult);
};

void CGameProcMain::MsgRecv_MyInfo_HP(DataPack* pDataPack, int& iOffset)
{
	const int iHPMax =	CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const int iHP =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	const int iHPChange = iHP - s_pPlayer->m_InfoBase.iHP;
	char szBuf[256] = "";
	if(iHPChange < 0)
	{
		std::string szFmt; ::_LoadStringFromResource(IDS_MSG_FMT_HP_LOST, szFmt);
		sprintf(szBuf, szFmt.c_str(), -iHPChange);
		MsgOutput(szBuf, 0xffff3b3b);
	}
	else if(iHPChange > 0)
	{
		std::string szFmt; ::_LoadStringFromResource(IDS_MSG_FMT_HP_RECOVER, szFmt);
		sprintf(szBuf, szFmt.c_str(), iHPChange);
		MsgOutput(szBuf, 0xff6565ff);
	}

	s_pPlayer->m_InfoBase.iHPMax = iHPMax;
	s_pPlayer->m_InfoBase.iHP = iHP;

	m_pUIVar->m_pPageState->UpdateHP(iHP, iHPMax);
	m_pUIStateBarAndMiniMap->UpdateHP(iHP, iHPMax, false);
}

void CGameProcMain::MsgRecv_MyInfo_MSP(DataPack* pDataPack, int& iOffset)
{
	const int iMSPMax = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const int iMSP = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	const int iMSPChange = iMSP - s_pPlayer->m_InfoExt.iMSP;

	const e_Class_Represent eCR = CGameProcedure::GetRepresentClass(s_pPlayer->m_InfoBase.eClass);
	const bool bUseMP = (CLASS_REPRESENT_WARRIOR != eCR && CLASS_REPRESENT_ROGUE != eCR) ? true : false;
	
	char szBuf[256] = "";
	if(iMSPChange < 0)
	{
		std::string szFmt;
		if(bUseMP) ::_LoadStringFromResource(IDS_MSG_FMT_MP_USE, szFmt);
		else ::_LoadStringFromResource(IDS_MSG_FMT_SP_USE, szFmt);
		sprintf(szBuf, szFmt.c_str(), -iMSPChange);
		MsgOutput(szBuf, 0xffff3b3b);
	}
	else if(iMSPChange > 0)
	{
		std::string szFmt;
		if(bUseMP) ::_LoadStringFromResource(IDS_MSG_FMT_MP_RECOVER, szFmt);
		else ::_LoadStringFromResource(IDS_MSG_FMT_SP_RECOVER, szFmt);
		sprintf(szBuf, szFmt.c_str(), iMSPChange);
		MsgOutput(szBuf, 0xff6565ff);
	}

	s_pPlayer->m_InfoExt.iMSPMax = iMSPMax;
	s_pPlayer->m_InfoExt.iMSP = iMSP;

	m_pUIVar->m_pPageState->UpdateMSP(iMSP, iMSPMax);
	m_pUIStateBarAndMiniMap->UpdateMSP(iMSP, iMSPMax, false);

	if (m_pUISkillTreeDlg) m_pUISkillTreeDlg->UpdateDisableCheck();
	if (m_pUIHotKeyDlg) m_pUIHotKeyDlg->UpdateDisableCheck();
}

void CGameProcMain::MsgRecv_MyInfo_EXP(DataPack* pDataPack, int& iOffset)
{
	const int iExp = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	const int iExpChange = iExp - s_pPlayer->m_InfoExt.iExp;
	s_pPlayer->m_InfoExt.iExp = iExp;
	m_pUIVar->m_pPageState->UpdateExp(iExp, s_pPlayer->m_InfoExt.iExpNext);
	m_pUIStateBarAndMiniMap->UpdateExp(iExp, s_pPlayer->m_InfoExt.iExpNext, false);

	if(s_pPlayer->m_InfoExt.iLevelPrev == s_pPlayer->m_InfoBase.iLevel && iExpChange != 0)
	{
		char szBuf[256] = "";
		if(iExpChange > 0)
		{
			std::string szFmt; ::_LoadStringFromResource(IDS_MSG_FMT_EXP_GET, szFmt);
			sprintf(szBuf, szFmt.c_str(), iExpChange);
		}
		else if(iExpChange < 0)
		{
			std::string szFmt; ::_LoadStringFromResource(IDS_MSG_FMT_EXP_LOST, szFmt);
			sprintf(szBuf, szFmt.c_str(), -iExpChange);
		}
		MsgOutput(szBuf, 0xffffff00);
	}

	s_pPlayer->m_InfoExt.iLevelPrev = s_pPlayer->m_InfoBase.iLevel;
}

bool CGameProcMain::MsgRecv_MyInfo_LevelChange(DataPack* pDataPack, int& iOffset)
{
	const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const int iLevel = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	if(iID == s_pPlayer->IDNumber())
	{
		__InfoPlayerBase*	pInfoBase = &(s_pPlayer->m_InfoBase);
		__InfoPlayerMySelf*	pInfoExt = &(s_pPlayer->m_InfoExt);

		const int iLevelPrev = pInfoBase->iLevel;
		pInfoBase->iLevel = iLevel;
		pInfoExt->iBonusPointRemain = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // Remaining bonus points...

		const BYTE	bExtraSkillPoint		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// total points
		TRACE("Skill change Extra value %d\n", bExtraSkillPoint);

		const int iExpNext	= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
		const int iExp		= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset); 

		// Calculate the new experience gained.
		int iExpChange = 0;
		if(iLevel > iLevelPrev)
			iExpChange = (pInfoExt->iExpNext - pInfoExt->iExp) + iExp; // When you level up...
		else
			iExpChange = -(pInfoExt->iExp + iExpNext - iExp); // If you level down...
		char szBuf[256] = "";
		if(iExpChange > 0)
		{
			std::string szFmt; ::_LoadStringFromResource(IDS_MSG_FMT_EXP_GET, szFmt);
			sprintf(szBuf, szFmt.c_str(), iExpChange);
		}
		else if(iExpChange < 0)
		{
			std::string szFmt; ::_LoadStringFromResource(IDS_MSG_FMT_EXP_LOST, szFmt);
			sprintf(szBuf, szFmt.c_str(), -iExpChange);
		}
		MsgOutput(szBuf, 0xffffff00);

			
		pInfoExt->iExpNext	= iExpNext; 
		pInfoExt->iExp		= iExp; 

		pInfoBase->iHPMax =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	
		pInfoBase->iHP =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

		pInfoExt->iMSPMax =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		pInfoExt->iMSP =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

		pInfoExt->iWeightMax =	CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		pInfoExt->iWeight	=	CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

		m_pUIVar->UpdateAllStates(&(s_pPlayer->m_InfoBase), &(s_pPlayer->m_InfoExt)); // All info updated..

		m_pUIStateBarAndMiniMap->UpdateExp(pInfoExt->iExp, pInfoExt->iExpNext, true);
		m_pUIStateBarAndMiniMap->UpdateHP(pInfoBase->iHP, pInfoBase->iHPMax, false);
		m_pUIStateBarAndMiniMap->UpdateMSP(pInfoExt->iMSP, pInfoExt->iMSPMax, false);

		m_pUISkillTreeDlg->m_iSkillInfo[0] = bExtraSkillPoint;
		m_pUISkillTreeDlg->InitIconUpdate();		// Since the level has changed .. Skills can also be added..

		if(iLevel > iLevelPrev) // Add the Level Up effect here.
		{
			if(s_pPlayer->Nation()==NATION_KARUS) CGameProcedure::s_pFX->TriggerBundle(iID, -1, FXID_LEVELUP_KARUS, iID, -1);
			else if(s_pPlayer->Nation()==NATION_ELMORAD) CGameProcedure::s_pFX->TriggerBundle(iID, -1, FXID_LEVELUP_ELMORAD, iID, -1);
		}
	}
	else // it&#39;s different..
	{
		CPlayerOther* pUPC = s_pOPMgr->UPCGetByID(iID, false);
		if(pUPC)
		{
			if(iLevel > pUPC->m_InfoBase.iLevel)
			{
				if(pUPC->Nation()==NATION_KARUS) CGameProcedure::s_pFX->TriggerBundle(iID, -1, FXID_LEVELUP_KARUS, iID, -1);
				else if(pUPC->Nation()==NATION_ELMORAD) CGameProcedure::s_pFX->TriggerBundle(iID, -1, FXID_LEVELUP_ELMORAD, iID, -1);
			}
			pUPC->m_InfoBase.iLevel = iLevel;
		}
	}

	return TRUE;
}

void CGameProcMain::MsgRecv_MyInfo_RealmPoint(DataPack* pDataPack, int& iOffset)
{
	s_pPlayer->m_InfoExt.iRealmPoint	= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset); 
	if(m_pUIVar->m_pPageState)
		m_pUIVar->m_pPageState->UpdateRealmPoint(s_pPlayer->m_InfoExt.iRealmPoint); // State contribution is divided by 10
}

void CGameProcMain::MsgRecv_MyInfo_PointChange(DataPack* pDataPack, int& iOffset)
{
	const int iType = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// 0x01 : true, 0x00 : false..
	const int iVal =	CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	// 0x01 : true, 0x00 : false..

	s_pPlayer->m_InfoBase.iHPMax =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iMSPMax =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iAttack =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	s_pPlayer->m_InfoExt.iWeightMax =	CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	m_pUIVar->m_pPageState->UpdateHP(s_pPlayer->m_InfoBase.iHP, s_pPlayer->m_InfoBase.iHPMax);
	m_pUIStateBarAndMiniMap->UpdateHP(s_pPlayer->m_InfoBase.iHP, s_pPlayer->m_InfoBase.iHPMax, false);
	
	m_pUIVar->m_pPageState->UpdateMSP(s_pPlayer->m_InfoExt.iMSP, s_pPlayer->m_InfoExt.iMSPMax);
	m_pUIStateBarAndMiniMap->UpdateMSP(s_pPlayer->m_InfoExt.iMSP, s_pPlayer->m_InfoExt.iMSPMax, false);

	m_pUIVar->m_pPageState->UpdateAttackPoint(s_pPlayer->m_InfoExt.iAttack, s_pPlayer->m_InfoExt.iAttack_Delta);
	m_pUIVar->m_pPageState->UpdateWeight(s_pPlayer->m_InfoExt.iWeight, s_pPlayer->m_InfoExt.iWeightMax);


	// Point change - send b1 (1 strength, 2 health, 3 agility, 4 intelligence, 5 horse power) s (-1 +1)
	// When receiving b1 (1 strength, 2 health, 3 agility, 4 intelligence, 5 horse power) s (absolute value)

	if(0x01 == iType) // Strength
	{
		s_pPlayer->m_InfoExt.iStrength = iVal;
		m_pUIVar->m_pPageState->UpdateStrength(s_pPlayer->m_InfoExt.iStrength, s_pPlayer->m_InfoExt.iStrength_Delta);
	}
	else if(0x02 == iType) // Stamina
	{
		s_pPlayer->m_InfoExt.iStamina = iVal;
		m_pUIVar->m_pPageState->UpdateStamina(s_pPlayer->m_InfoExt.iStamina, s_pPlayer->m_InfoExt.iStamina_Delta);
	}
	else if(0x03 == iType) // Dex
	{
		s_pPlayer->m_InfoExt.iDexterity = iVal;
		m_pUIVar->m_pPageState->UpdateDexterity(s_pPlayer->m_InfoExt.iDexterity, s_pPlayer->m_InfoExt.iDexterity_Delta);
	}
	else if(0x04 == iType) // Intelligence
	{
		s_pPlayer->m_InfoExt.iIntelligence = iVal;
		m_pUIVar->m_pPageState->UpdateIntelligence(s_pPlayer->m_InfoExt.iIntelligence, s_pPlayer->m_InfoExt.iIntelligence_Delta);
	}
	else if(0x05 == iType) // Magic Attack
	{
		s_pPlayer->m_InfoExt.iMagicAttak = iVal;
		m_pUIVar->m_pPageState->UpdateMagicAttak(s_pPlayer->m_InfoExt.iMagicAttak, s_pPlayer->m_InfoExt.iMagicAttak_Delta);
	}

	if(iType >= 1 && iType <= 5)
	{
		s_pPlayer->m_InfoExt.iBonusPointRemain--;
		m_pUIVar->m_pPageState->UpdateBonusPointAndButtons(s_pPlayer->m_InfoExt.iBonusPointRemain); // Are bonus points applicable?
	}
}

void CGameProcMain::InitUI()
{
	const int iW = CN3Base::s_CameraData.vp.Width;
	const int iH = CN3Base::s_CameraData.vp.Height;

	RECT rc;
	int iX = 0, iY = 0;

	e_Nation eNation = s_pPlayer->m_InfoBase.eNation; // nation....

	__TABLE_UI_RESRC* pTbl = s_pTbl_UI->Find(eNation);
	if(nullptr == pTbl) return;
	
	m_pUICmd->Init(s_pUIMgr);
	m_pUICmd->LoadFromFile(pTbl->szCmd);
	rc = m_pUICmd->GetRegion();
	m_pUICmd->SetPos((iW - (rc.right - rc.left))/2, iH - (rc.bottom - rc.top));
	m_pUICmd->SetStyle(UISTYLE_FOCUS_UNABLE | UISTYLE_HIDE_UNABLE);

	m_pUIChatDlg->Init(s_pUIMgr);					// Add to the list as a Manager child
	m_pUIChatDlg->LoadFromFile(pTbl->szChat);
	rc = m_pUIChatDlg->GetRegion();
	RECT rcCmd = m_pUICmd->GetRegion(); rcCmd.top += 5; // ..hard-coded..
	iX = 0;
	iY = iH - ((rc.bottom - rc.top) + (rcCmd.bottom - rcCmd.top));
	CGameProcedure::UIPostData_Read(UI_POST_WND_CHAT, m_pUIChatDlg, iX, iY);
	m_pUIChatDlg->SetStyle(UISTYLE_FOCUS_UNABLE | UISTYLE_HIDE_UNABLE);
	m_pUIChatDlg->SetVisibleWithNoSound(true);

	m_pUIMsgDlg->Init(s_pUIMgr);
	m_pUIMsgDlg->LoadFromFile(pTbl->szMsgOutput);
	m_pUIMsgDlg->SetStyle(UISTYLE_FOCUS_UNABLE | UISTYLE_HIDE_UNABLE);

	// Align the position of the chat window and the message window.
	m_pUIChatDlg->MoveOffset(0, -1);

	m_pUIStateBarAndMiniMap->Init(s_pUIMgr);
	m_pUIStateBarAndMiniMap->LoadFromFile(pTbl->szStateBar);
	m_pUIStateBarAndMiniMap->SetStyle(UISTYLE_FOCUS_UNABLE | UISTYLE_HIDE_UNABLE);
#ifdef _DEBUG
	m_pUIStateBarAndMiniMap->SetPos(0, 70); // Down a bit for displaying debug info...
#else
	m_pUIStateBarAndMiniMap->SetPos(0, 0);
#endif

	// Multipurpose UI - status, guild management, quests, friend management, etc...
	m_pUIVar->Init(s_pUIMgr);
	m_pUIVar->LoadFromFile(pTbl->szVarious);
	m_pUIVar->SetVisibleWithNoSound(false);
	m_pUIVar->m_pPageState->LoadFromFile(pTbl->szState);
	m_pUIVar->m_pPageState->SetVisibleWithNoSound(true);
	m_pUIVar->m_pPageKnights->LoadFromFile(pTbl->szKnights);
	m_pUIVar->m_pPageKnights->SetVisibleWithNoSound(false);
	m_pUIVar->m_pPageFriends->LoadFromFile(pTbl->szFriends);
	m_pUIVar->m_pPageFriends->SetVisibleWithNoSound(false);
	m_pUIVar->SetStyle(m_pUIVar->GetStyle() | UISTYLE_POS_LEFT);
	// m_pUIVar->m_pPageQuest->LoadFromFile(pTbl->szQuest);
	// m_pUIVar->m_pPageQuest->SetVisibleWithNoSound(false);


	
	m_pUITargetBar->Init(s_pUIMgr);
	m_pUITargetBar->LoadFromFile(pTbl->szTargetBar);
	m_pUITargetBar->SetStyle(UISTYLE_FOCUS_UNABLE | UISTYLE_HIDE_UNABLE);
	rc = m_pUITargetBar->GetRegion();
	m_pUITargetBar->SetPos((iW - (rc.right - rc.left))/2, 0);
	m_pUITargetBar->SetVisibleWithNoSound(false);

	m_pUIHelp->Init(s_pUIMgr);
	m_pUIHelp->LoadFromFile(pTbl->szHelp);
	CGameProcedure::UIPostData_Read(UI_POST_WND_HELP, m_pUIHelp, 0, 0);
	rc = m_pUIHelp->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUIHelp->SetPos(iX, iY);
	// m_pUIHelp->SetStyle(UISTYLE_SHOW_ME_ALONE);
	
	// announcement..
	m_pUINotice->Init(s_pUIMgr);
	m_pUINotice->LoadFromFile(pTbl->szNotice);
	m_pUINotice->SetVisibleWithNoSound(false);

	m_pUIClassChange->Init(s_pUIMgr);
	m_pUIClassChange->LoadFromFile(pTbl->szClassChange);
	m_pUIClassChange->SetVisibleWithNoSound(false);
	rc = m_pUIClassChange->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUIClassChange->SetPos(iX, iY);
	m_pUIClassChange->SetStyle(UISTYLE_USER_MOVE_HIDE);

	m_pUINpcChange->Init(s_pUIMgr);
	m_pUINpcChange->LoadFromFile(pTbl->szChangeClassInit);
	m_pUINpcChange->SetVisibleWithNoSound(false);
	rc = m_pUINpcChange->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUINpcChange->SetPos(iX, iY);
	m_pUINpcChange->SetStyle(UISTYLE_SHOW_ME_ALONE | UISTYLE_USER_MOVE_HIDE);

	m_pUINpcEvent->Init(s_pUIMgr);
	m_pUINpcEvent->LoadFromFile(pTbl->szNpcEvent);
	m_pUINpcEvent->SetVisibleWithNoSound(false);
	m_pUINpcEvent->SetStyle(UISTYLE_SHOW_ME_ALONE | UISTYLE_USER_MOVE_HIDE);
	rc = m_pUINpcEvent->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUINpcEvent->SetPos(iX, iY);

	m_pUINpcTalk->Init(s_pUIMgr);
	m_pUINpcTalk->LoadFromFile(pTbl->szNpcTalk);
	m_pUINpcTalk->SetVisibleWithNoSound(false);
	rc = m_pUINpcTalk->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUINpcTalk->SetPos(iX, iY);

	m_pUIWarp->Init(s_pUIMgr);
	m_pUIWarp->LoadFromFile(pTbl->szZoneChangeOrWarp);
	m_pUIWarp->SetVisibleWithNoSound(false);
	rc = m_pUIWarp->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUIWarp->SetPos(iX, iY);
	m_pUIWarp->SetStyle(UISTYLE_USER_MOVE_HIDE | UISTYLE_SHOW_ME_ALONE);

	// m_pUITradeList->Init(s_pUIMgr);
	// m_pUITradeList->LoadFromFile(pTbl->szNpcExchangeList);
	// m_pUITradeList->SetVisibleWithNoSound(false);
	// rc = m_pUITradeList->GetRegion();
	// iX = (iW - (rc.right - rc.left))/2;
	// iY = (iH - (rc.bottom - rc.top))/2;
	// m_pUITradeList->SetPos(iX, iY);

	m_pUIItemREDlg->Init(s_pUIMgr);
	m_pUIItemREDlg->LoadFromFile(pTbl->szExchangeRepair);
	m_pUIItemREDlg->SetVisibleWithNoSound(false);
	rc = m_pUIItemREDlg->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUIItemREDlg->SetPos(iX, iY);
	m_pUIItemREDlg->InitIconWnd(UIWND_EXCHANGE_REPAIR);
	m_pUIItemREDlg->SetUIType(UI_TYPE_ICON_MANAGER);
	m_pUIItemREDlg->SetState(UI_STATE_COMMON_NONE);

	m_pUIRepairTooltip->Init(s_pUIMgr);
	m_pUIRepairTooltip->LoadFromFile(pTbl->szRepairTooltip);
	m_pUIRepairTooltip->SetVisibleWithNoSound(false);
	m_pUIRepairTooltip->InitPos();
	m_pUIRepairTooltip->SetStyle(UISTYLE_ALWAYSTOP|UISTYLE_FOCUS_UNABLE);

	m_pUIPartyOrForce->Init(s_pUIMgr);
	m_pUIPartyOrForce->LoadFromFile(pTbl->szPartyOrForce);
	m_pUIPartyOrForce->SetStyle(UISTYLE_HIDE_UNABLE);
	rc = m_pUIPartyOrForce->GetRegion();
	iX = iW - (rc.right - rc.left);
	m_pUIPartyOrForce->SetPos(iX, 0);
	m_pUIPartyOrForce->SetVisible(false); // Force not to see
// CGameProcedure::UIPostData_Read(UI_POST_WND_PARTY, m_pUIPartyOrForce, iX, 0);

	// Dropped Item Dlg..
	m_pUIDroppedItemDlg->Init(s_pUIMgr);
	m_pUIDroppedItemDlg->LoadFromFile(pTbl->szDroppedItem);
	m_pUIDroppedItemDlg->SetVisibleWithNoSound(false);
	m_pUIDroppedItemDlg->InitIconWnd(UIWND_DROPITEM);
	m_pUIDroppedItemDlg->SetUIType(UI_TYPE_ICON_MANAGER);
	m_pUIDroppedItemDlg->SetState(UI_STATE_COMMON_NONE);
	m_pUIDroppedItemDlg->SetStyle(m_pUIDroppedItemDlg->GetStyle() | UISTYLE_USER_MOVE_HIDE);

	m_pUITransactionDlg->Init(s_pUIMgr);
	m_pUITransactionDlg->LoadFromFile(pTbl->szTransaction);
	rc = m_pUITransactionDlg->GetRegion();
	m_pUITransactionDlg->SetPos(iW - (rc.right-rc.left), 10);
	m_pUITransactionDlg->SetVisibleWithNoSound(false);
	m_pUITransactionDlg->InitIconWnd(UIWND_TRANSACTION);
	m_pUITransactionDlg->SetUIType(UI_TYPE_ICON_MANAGER);
	m_pUITransactionDlg->SetState(UI_STATE_COMMON_NONE);
	m_pUITransactionDlg->SetStyle(m_pUITransactionDlg->GetStyle() | UISTYLE_POS_RIGHT | UISTYLE_USER_MOVE_HIDE);

	m_pSubProcPerTrade->InitPerTradeDlg(s_pUIMgr);

	m_pUIInventory->Init(s_pUIMgr);
	m_pUIInventory->LoadFromFile(pTbl->szInventory);
	m_pUIInventory->SetVisibleWithNoSound(false);
	m_pUIInventory->SetPos(465,10);
	m_pUIInventory->InitIconWnd(UIWND_INVENTORY);
	m_pUIInventory->SetUIType(UI_TYPE_ICON_MANAGER);
	m_pUIInventory->SetState(UI_STATE_COMMON_NONE);
	m_pUIInventory->SetStyle(m_pUIInventory->GetStyle() | UISTYLE_POS_RIGHT);

	// Countable Item Edit Dlg..
	CN3UIWndBase::m_pCountableItemEdit = new CCountableItemEditDlg;
	CN3UIWndBase::m_pCountableItemEdit->Init(s_pUIMgr);
	CN3UIWndBase::m_pCountableItemEdit->LoadFromFile(pTbl->szPersonalTradeEdit);
	CN3UIWndBase::m_pCountableItemEdit->SetStyle(UISTYLE_ALWAYSTOP);
	// position calculation ..
	rc = CN3UIWndBase::m_pCountableItemEdit->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	CN3UIWndBase::m_pCountableItemEdit->SetPos(iX, iY);	
	CN3UIWndBase::m_pCountableItemEdit->SetVisibleWithNoSound(false);
	CN3UIWndBase::m_pCountableItemEdit->SetUIType(UI_TYPE_BASE);
	CN3UIWndBase::m_pCountableItemEdit->SetState(UI_STATE_COMMON_NONE);

	m_pUISkillTreeDlg->Init(s_pUIMgr);
	m_pUISkillTreeDlg->LoadFromFile(pTbl->szSkillTree);
	m_pUISkillTreeDlg->SetVisibleWithNoSound(false);
	rc = m_pUISkillTreeDlg->GetRegion();
	m_pUISkillTreeDlg->SetPos(iW - (rc.right-rc.left), 10);
	m_pUISkillTreeDlg->InitIconWnd(UIWND_SKILL_TREE);
	m_pUISkillTreeDlg->SetUIType(UI_TYPE_ICON_MANAGER);
	m_pUISkillTreeDlg->SetState(UI_STATE_COMMON_NONE);
	m_pUISkillTreeDlg->SetStyle(m_pUISkillTreeDlg->GetStyle() | UISTYLE_POS_RIGHT);

	
	// default ui pos .. When the resolution is changed.. Relative position must be obtained.. by ecli666
	rc = m_pUIStateBarAndMiniMap->GetRegion();
	m_pUIHotKeyDlg->Init(s_pUIMgr);
	m_pUIHotKeyDlg->LoadFromFile(pTbl->szHotKey);
	m_pUIHotKeyDlg->SetStyle(UISTYLE_HIDE_UNABLE);
	CGameProcedure::UIPostData_Read(UI_POST_WND_HOTKEY, m_pUIHotKeyDlg, rc.left, rc.bottom);
	m_pUIHotKeyDlg->SetVisibleWithNoSound(true); // Absolutely visible!!!
	m_pUIHotKeyDlg->InitIconWnd(UIWND_HOTKEY);
	m_pUIHotKeyDlg->SetUIType(UI_TYPE_ICON_MANAGER);
	m_pUIHotKeyDlg->SetState(UI_STATE_COMMON_NONE);

	m_pUIKnightsOp->Init(s_pUIMgr);	// View the list of guilds, join, etc...
	m_pUIKnightsOp->LoadFromFile(pTbl->szKnightsOperation);
	m_pUIKnightsOp->SetVisibleWithNoSound(false);

	// Party support bulletin board..
	m_pUIPartyBBS->Init(s_pUIMgr);
	m_pUIPartyBBS->LoadFromFile(pTbl->szPartyBBS);
	m_pUIPartyBBS->SetVisibleWithNoSound(false);
	rc = m_pUIPartyBBS->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUIPartyBBS->SetPos(iX, iY);


	m_pUIWareHouseDlg->Init(s_pUIMgr);
	m_pUIWareHouseDlg->LoadFromFile(pTbl->szWareHouse);
	rc = m_pUIWareHouseDlg->GetRegion();
	m_pUIWareHouseDlg->SetPos(iW - (rc.right-rc.left), 10);
	m_pUIWareHouseDlg->SetVisibleWithNoSound(false);
	m_pUIWareHouseDlg->InitIconWnd(UIWND_WARE_HOUSE);
	m_pUIWareHouseDlg->SetUIType(UI_TYPE_ICON_MANAGER);
	m_pUIWareHouseDlg->SetState(UI_STATE_COMMON_NONE);
	m_pUIWareHouseDlg->SetStyle(UISTYLE_USER_MOVE_HIDE | UISTYLE_POS_RIGHT);

	m_pTargetSymbol->LoadFromFile(pTbl->szTargetSymbolShape); // The player draws on the location of the target character.

	m_pUIInn->Init(s_pUIMgr);
	m_pUIInn->LoadFromFile(pTbl->szInn);
	m_pUIInn->SetVisibleWithNoSound(false);
	m_pUIInn->SetStyle(UISTYLE_SHOW_ME_ALONE | UISTYLE_USER_MOVE_HIDE);
	rc = m_pUIInn->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUIInn->SetPos(iX, iY);
	
	m_pUICreateClanName->Init(s_pUIMgr);
	m_pUICreateClanName->LoadFromFile(pTbl->szInputClanName);
	m_pUICreateClanName->SetVisibleWithNoSound(false);
	rc = m_pUICreateClanName->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUICreateClanName->SetPos(iX, iY);
	m_pUICreateClanName->SetStyle(UISTYLE_USER_MOVE_HIDE);

	// Quest Menu
	m_pUIQuestMenu->Init(s_pUIMgr);
	m_pUIQuestMenu->LoadFromFile(pTbl->szQuestMenu);
	m_pUIQuestMenu->SetVisibleWithNoSound(false);
	m_pUIQuestMenu->SetStyle(UISTYLE_USER_MOVE_HIDE);

	// Quest Talk
	m_pUIQuestTalk->Init(s_pUIMgr);
	m_pUIQuestTalk->LoadFromFile(pTbl->szQuestTalk);
	m_pUIQuestTalk->SetVisibleWithNoSound(false);
	m_pUIQuestTalk->SetStyle(UISTYLE_USER_MOVE_HIDE);
	rc = m_pUIQuestTalk->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUIQuestTalk->SetPos(iX, iY);

	// dead ui
	m_pUIDead->Init(s_pUIMgr);
	m_pUIDead->LoadFromFile(pTbl->szDead);
	m_pUIDead->SetVisibleWithNoSound(false);
	rc = m_pUIDead->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUIDead->SetPos(iX, iY);


	// commerce bulletin board
	m_pUITradeBBS->Init(s_pUIMgr);
	m_pUITradeBBS->LoadFromFile(pTbl->szTradeBBS);
	m_pUITradeBBS->SetVisibleWithNoSound(false);
	m_pUITradeBBS->SetStyle(UISTYLE_USER_MOVE_HIDE);
	rc = m_pUITradeBBS->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUITradeBBS->SetPos(iX, iY);

	// Select commerce type
	m_pUITradeBBSSelector->Init(s_pUIMgr);
	m_pUITradeBBSSelector->LoadFromFile(pTbl->szTradeBBSSelector);
	m_pUITradeBBSSelector->SetVisibleWithNoSound(false);
	rc = m_pUITradeBBSSelector->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUITradeBBSSelector->SetPos(iX, iY);
	m_pUITradeBBSSelector->SetStyle(UISTYLE_USER_MOVE_HIDE);

	// Commerce Item Content
	m_pUITradeBBSEdit->LoadFromFile(pTbl->szTradeExplanation);
	m_pUITradeBBSEdit->SetVisibleWithNoSound(false);
	rc = m_pUITradeBBSEdit->GetRegion();
	iX = (iW - (rc.right - rc.left))/2;
	iY = (iH - (rc.bottom - rc.top))/2;
	m_pUITradeBBSEdit->SetPos(iX, iY);
}

void CGameProcMain::MsgSend_RequestTargetHP(short siIDTarget, BYTE byUpdateImmediately)
{
	BYTE byBuff[4];
	int iOffset = 0;
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_TARGET_HP);
	CAPISocket::MP_AddShort(byBuff, iOffset, siIDTarget);
	CAPISocket::MP_AddByte(byBuff, iOffset, byUpdateImmediately); // 0x00 - incrementally.. 0x01 - immediate update..

	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgRecv_TargetHP(DataPack* pDataPack, int& iOffset)
{
	const int iID				= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	// SID
	const BYTE byUpdateImmediately = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// 0x00 - incrementally.. 0x01 - immediate update..
	const int iTargetHPMax	= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);  // HP
	const int iTargetHPCur	= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);  // HP
	const int iTargetHPChange	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);  // HP

	if(iTargetHPMax <= 0)
	{
		CPlayerNPC* pTarget = this->s_pOPMgr->CharacterGetByID(iID, true);
		std::string szBuff("Invalid HP Change - 0 ID(");
		if(pTarget) szBuff += pTarget->IDString();
		szBuff += ')';
		CLogWriter::Write(szBuff.c_str());
	}
	__ASSERT(iTargetHPMax > 0, "최대 체력 수치는 0이상이어야 합니다.");

	if(iID == s_pPlayer->m_iIDTarget) // If I&#39;m attacking...
	{
		bool bUI = false;
		if(byUpdateImmediately) bUI = true;
		m_pUITargetBar->UpdateHP(iTargetHPCur, iTargetHPMax, bUI);
	// TRACE("Target HP Update %d %d\n", iID, iTargetHPPercent);
	}

	CPlayerNPC* pTarget = s_pOPMgr->CharacterGetByID(iID, true);
	if(pTarget)
	{
		pTarget->m_InfoBase.iHP = iTargetHPCur;
		pTarget->m_InfoBase.iHPMax = iTargetHPMax;

		char szBuf[256] = "";
		if(iTargetHPChange < 0)
		{
			std::string szFmt; ::_LoadStringFromResource(IDS_MSG_FMT_TARGET_HP_LOST, szFmt);
			sprintf(szBuf, szFmt.c_str(), pTarget->IDString().c_str(), -iTargetHPChange);
			MsgOutput(szBuf, 0xffffffff);
		}
		else if(iTargetHPChange > 0)
		{
			std::string szFmt; ::_LoadStringFromResource(IDS_MSG_FMT_TARGET_HP_RECOVER, szFmt);
			sprintf(szBuf, szFmt.c_str(), pTarget->IDString().c_str(), iTargetHPChange);
			MsgOutput(szBuf, 0xff6565ff);
		}
	}
}

// Commerce............
bool CGameProcMain::MsgSend_NPCEvent(short siIDTarget)
{
	BYTE byBuff[4];
	int iOffset = 0;
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_NPC_EVENT);
	CAPISocket::MP_AddShort(byBuff, iOffset, siIDTarget);

	s_pSocket->Send(byBuff, iOffset);

	return true;
}

void CGameProcMain::MsgSend_NPCInRequest(int iID) // If there is no NPC information, request it.
{
	TRACE("found NPC ghost (ID:%d)\n", iID);

	int iOffset=0;													// Offset of buffer..
	BYTE byBuff[32];;												// Packet buffer...
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_REQUEST_NPC_IN);		// command.
	CAPISocket::MP_AddShort(byBuff, iOffset, 1);					// number of ids.
	CAPISocket::MP_AddShort(byBuff, iOffset, iID);					// IDs for which detailed information is required.

	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgSend_UserInRequest(int iID) // Request if there is no user information.
{
	TRACE("found User ghost (ID:%d)\n", iID);

	int iOffset=0;													// Offset of buffer..
	BYTE byBuff[32];;												// Packet buffer...
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_REQUEST_USER_IN);	// command.
	CAPISocket::MP_AddShort(byBuff, iOffset, 1);					// number of ids.
	CAPISocket::MP_AddShort(byBuff, iOffset, iID);					// IDs for which detailed information is required.

	s_pSocket->Send(byBuff, iOffset);
}


void CGameProcMain::MsgSend_Warp() // Warp - can be zone movement..
{
	__WarpInfo WI;
	const int iSel = m_pUIWarp->InfoGetCur(WI);
	if(iSel < 0 || WI.szName.empty()) return;

	BYTE byBuff[8];
	int iOffset = 0;

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_WARP_LIST);
	CAPISocket::MP_AddByte(byBuff, iOffset, WI.iID); // Send Warp ID...
	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::MsgSend_ZoneChangeComplete() // Zone change complete.. (map loading finished..)
{
	BYTE byBuff[4];
	int iOffset = 0;
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_ZONE_CHANGE);

	s_pSocket->Send(byBuff, iOffset);
}

void CGameProcMain::DoCommercialTransaction(int iTradeID)
{
	if (m_pUITransactionDlg->IsVisible())
		return;

	m_pUITransactionDlg->m_iTradeID = iTradeID;

	if ( !m_pUITransactionDlg->IsVisible() )
		m_pUITransactionDlg->SetVisible(true);

	if (m_pUIInventory->IsVisible()) // If your inventory isn&#39;t open...
		this->CommandToggleUIInventory();

	if (m_pUISkillTreeDlg->IsVisible())
		m_pUISkillTreeDlg->Close();

	m_pUITransactionDlg->EnterTransactionState();
}

bool CGameProcMain::MsgRecv_ItemTradeStart(DataPack* pDataPack, int& iOffset)			// Item trade...
{
	const int iTradeID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);		// Trade id
	CPlayerNPC* pNPC = s_pOPMgr->NPCGetByID(s_pPlayer->m_iIDTarget, true);
	if (!pNPC)	return true;
	m_pUINpcEvent->Open(NPC_EVENT_ITEM_TRADE, iTradeID, pNPC->GetNPCOriginID());

	return true;
}

bool CGameProcMain::MsgRecv_ItemTradeResult(DataPack* pDataPack, int& iOffset)			// Item transaction result..
{
	byte bfType = 0x00;	int	iMoney = 0;
	const byte bResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// Trade id

	switch ( bResult )
	{
		case 0x00:
			bfType = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// Trade id
			m_pUITransactionDlg->ReceiveResultTradeFromServer( bResult, bfType, iMoney );
			break;

		case 0x01:
			iMoney = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);		// Trade id
			m_pUITransactionDlg->ReceiveResultTradeFromServer( bResult, bfType, iMoney );
			break;

		case 0x03:
			m_pUITransactionDlg->ReceiveResultTradeMoveSuccess();
			break;

		case 0x04:
			m_pUITransactionDlg->ReceiveResultTradeMoveFail();
			break;
	}
	
	return true;
}

void CGameProcMain::InitZone(int iZone, const __Vector3& vPosPlayer)
{
	if(m_pSnd_Battle) m_pSnd_Battle->Stop(0.0f); // stop the music...
	if(m_pSnd_Town) m_pSnd_Town->Stop(0.0f);

	MsgSend_SpeedCheck(true);		// Setting the speed hack check standard time
	static int iZonePrev = -1;
	if(iZonePrev != iZone) // Loading only in case of other zones..
	{
		m_pLightMgr->Release();
		s_pEng->SetDefaultLight(m_pLightMgr->Light(0), m_pLightMgr->Light(1), m_pLightMgr->Light(2));

		if(m_pUIChatDlg) m_pUIChatDlg->ChangeChattingMode(N3_CHAT_NORMAL); // If you move the zone, it changes to the normal chatting state.
		if(m_pUIPartyOrForce) m_pUIPartyOrForce->MemberDestroy();	// Breaking up the party when moving zones...
		this->UpdateUI_PartyOrForceButtons(); // Updates the party button on the command line as needed.

		s_pPlayer->m_bMoveContinous = true; // stop...
		this->CommandToggleMoveContinous();

		CLogWriter::Write("CGameProcMain::InitZone -> Zone Change (%d -> %d) Position(%.1f, %.1f, %.1f)", iZonePrev, iZone, vPosPlayer.x, vPosPlayer.y, vPosPlayer.z);

		m_bLoadComplete = false; // Loading finished...
		CLogWriter::Write("%d->ClearDurationalMagic()",m_pMagicSkillMng); // TmpLog1122
		m_pMagicSkillMng->ClearDurationalMagic();
		CLogWriter::Write("%d->ClearAll()", s_pFX); // TmpLog1122
		s_pFX->ClearAll();

		if(s_pUILoading)
		{
			CLogWriter::Write("s_pUILoading->Render()"); // TmpLog1122
			s_pUILoading->Render("", 0);
		}
		
		s_pPlayer->m_InfoExt.iZoneCur = iZone;
		iZonePrev = iZone; // Remember the last read zone number.

		CLogWriter::Write("%d->Find(s_pPlayer->m_InfoExt.iZoneCur)",s_pTbl_Zones); // TmpLog1122
		__TABLE_ZONE* pZoneData = s_pTbl_Zones->Find(s_pPlayer->m_InfoExt.iZoneCur);
		if(nullptr == pZoneData) { CLogWriter::Write("can't find zone data. (zone : %d)", s_pPlayer->m_InfoExt.iZoneCur); __ASSERT(0, "Zone Data Not Found!"); return; }

		CLogWriter::Write("%d->Release()",s_pOPMgr); // TmpLog1122

		s_pOPMgr->Release(); // Everyone else flies.
		CLogWriter::Write("%d->InitWorld()",s_pWorldMgr); // TmpLog1122
		s_pWorldMgr->InitWorld(iZone, vPosPlayer);

		// Loading the minimap...
		CLogWriter::Write("%d->GetWidthByMeterWithTerrain()",ACT_WORLD); // TmpLog1122
		const float fWidth = ACT_WORLD->GetWidthByMeterWithTerrain();
		CLogWriter::Write("%d->LoadMap()",m_pUIStateBarAndMiniMap); // TmpLog1122
		CLogWriter::Write("%d->szMiniMapFNszMiniMapFN",pZoneData); // TmpLog1122
		m_pUIStateBarAndMiniMap->LoadMap(pZoneData->szMiniMapFN, fWidth, fWidth);

		CLogWriter::Write("GetRepresentClass()"); // TmpLog1122
		// Set the zoom ratio...
		float fZoom = 6.0f;
		const e_Class_Represent eCR = CGameProcedure::GetRepresentClass(s_pPlayer->m_InfoBase.eClass);
		if(CLASS_REPRESENT_ROGUE == eCR) fZoom = 3.0f; // The log series shows more extensive and detailed maps.
		CLogWriter::Write("%d->ZoomSet()",m_pUIStateBarAndMiniMap); // TmpLog1122
		m_pUIStateBarAndMiniMap->ZoomSet(fZoom);

		CLogWriter::Write("%d->szTerrainFN.c_str()",pZoneData); // TmpLog1122
		// char szBuf[256];
		char szFName[_MAX_PATH];
		_splitpath(pZoneData->szTerrainFN.c_str(), nullptr, nullptr, szFName, nullptr);
		char szFName2[_MAX_PATH];
		char szFullPathName[_MAX_PATH];
		sprintf(szFName2,"%s_Bird",szFName);
		_makepath(szFullPathName, nullptr, "misc\\bird", szFName2, "lst");

		m_pLightMgr->LoadZoneLight(pZoneData->szLightObjFN.c_str());

		m_bLoadComplete = true; // Loading finished...
	}
		
	// camera settings...
	CN3Camera* pCamera		= s_pEng->CameraGetActive();		// Get the camera activated...
	if(pCamera)
	{
		__Vector3 vPosPlayer = s_pPlayer->Position();
		pCamera->m_Data.fFOV	= D3DXToRadian(70);				// Field of View ..
		pCamera->m_Data.fFP		= 512.0f;						// Far Plane..
		pCamera->m_Data.fNP		= 0.5f;							// Near Plane..
		CLogWriter::Write("pCamera->LookAt()"); // TmpLog1122
		auto vUp = __Vector3(0, 1, 0);
		pCamera->LookAt(vPosPlayer + __Vector3(0,0,-1), vPosPlayer, vUp);
		CLogWriter::Write("pCamera->Tick()"); // TmpLog1122
		pCamera->Tick();
		CLogWriter::Write("pCamera->Apply()"); // TmpLog1122
		pCamera->Apply();
	}
	// Basic character positioning and camera positioning..
	// 

	CLogWriter::Write("InitPlayerPosition() Position(%.1f, %.1f, %.1f)",vPosPlayer.x, vPosPlayer.y, vPosPlayer.z); // TmpLog1122
	this->InitPlayerPosition(vPosPlayer); // Initialize the player position.. Raise him up and make him take the basic action.
	CLogWriter::Write("%d->Release()2",s_pOPMgr); // TmpLog1122
	s_pOPMgr->Release(); // Delete other players...
}


void CGameProcMain::MsgSend_GameStart()
{
	BYTE byBuff[32];															// Packet buffer...
	int iOffset=0;															// Packet Offset...

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_GAMESTART);						// Game Start Packet Commands..
	CAPISocket::MP_AddByte(byBuff, iOffset, s_pPlayer->IDString().size());		// ID Length Packet..
	CAPISocket::MP_AddString(byBuff, iOffset, s_pPlayer->IDString());			// Put the ID string in the packet..

	s_pSocket->Send(byBuff, iOffset);	
}

bool CGameProcMain::CommandToggleWalkRun()
{
	const bool bRun = s_pPlayer->ToggleRunMode();
	const bool bMove = s_pPlayer->m_bMoveContinous;

	// Command button update..
	if(m_pUICmd->m_pBtn_Act_Walk)
	{
		m_pUICmd->m_pBtn_Act_Walk->SetVisible(bRun);
		if(bMove) m_pUICmd->m_pBtn_Act_Walk->SetState(UI_STATE_BUTTON_DOWN);
		else m_pUICmd->m_pBtn_Act_Walk->SetState(UI_STATE_BUTTON_NORMAL);
	}
	if(m_pUICmd->m_pBtn_Act_Run)
	{
		m_pUICmd->m_pBtn_Act_Run->SetVisible(!bRun);
		if(bMove) m_pUICmd->m_pBtn_Act_Run->SetState(UI_STATE_BUTTON_DOWN);
		else m_pUICmd->m_pBtn_Act_Run->SetState(UI_STATE_BUTTON_NORMAL);
	}

	return bRun;
}

bool CGameProcMain::CommandToggleAttackContinous()
{
	CPlayerBase* pTarget = s_pPlayer->TargetPointerCheck(false);
	this->CommandEnableAttackContinous(!s_pPlayer->m_bAttackContinous, pTarget);

	return s_pPlayer->m_bAttackContinous;
}

bool CGameProcMain::CommandToggleMoveContinous()
{
	if(s_pPlayer->IsDead()) return false;

	s_pPlayer->ToggleMoveMode();				// Auto advance toggle..
	if(s_pPlayer->m_bMoveContinous)
	{
		this->CommandMove(MD_FOWARD, true);
		if(m_pUICmd->m_pBtn_Act_Run) m_pUICmd->m_pBtn_Act_Run->SetState(UI_STATE_BUTTON_DOWN);
		if(m_pUICmd->m_pBtn_Act_Walk) m_pUICmd->m_pBtn_Act_Walk->SetState(UI_STATE_BUTTON_DOWN);
	}
	else
	{
		this->CommandMove(MD_STOP, true); // Send movement packets..
		if(m_pUICmd->m_pBtn_Act_Run) m_pUICmd->m_pBtn_Act_Run->SetState(UI_STATE_BUTTON_NORMAL);
		if(m_pUICmd->m_pBtn_Act_Walk) m_pUICmd->m_pBtn_Act_Walk->SetState(UI_STATE_BUTTON_NORMAL);
	}

	return s_pPlayer->m_bMoveContinous;
}

void CGameProcMain::CommandMove(e_MoveDirection eMD, bool bStartOrEnd)
{
	if(bStartOrEnd) this->CloseUIs(); // Various commerce, warp, etc... Closing the UI...

	if(s_pPlayer->IsDead()) return; // How dare you die!

	if(MD_FOWARD == eMD || MD_BACKWARD == eMD)
	{
		s_pUIMgr->UserMoveHideUIs();
		this->CommandSitDown(false, false, true); // stand up and...
		if(s_pPlayer->m_bStun) return; // Passed out, can&#39;t move..
		if(MD_FOWARD == eMD)
		{
			if(s_pPlayer->IsRunning()) s_pPlayer->ActionMove(PSM_RUN); // run away...
			else s_pPlayer->ActionMove(PSM_WALK); // to walk...
		}
		else
		{
			s_pPlayer->ActionMove(PSM_WALK_BACKWARD); // junior..
		}

		if( bStartOrEnd ) // When moving, first check for collision.
		{
			const float fSpeed = s_pPlayer->MoveSpeedCalculationAndCheckCollision(); // Get the speed and do a collision check with that speed. If the return value is 0, it is a collision.
			if(0 == fSpeed) // If you can&#39;t move...
			{
				s_pPlayer->ActionMove(PSM_STOP); // stoppage..
			}
			else
			{
				this->MsgSend_Move(true, false); // At the moment of movement... Send movement start packet.
			}

			if(m_pUICmd->m_pBtn_Act_Run) m_pUICmd->m_pBtn_Act_Run->SetState(UI_STATE_BUTTON_DOWN);
			if(m_pUICmd->m_pBtn_Act_Walk) m_pUICmd->m_pBtn_Act_Walk->SetState(UI_STATE_BUTTON_DOWN);
		}
	}
	else if(MD_STOP == eMD)
	{
		s_pPlayer->m_bMoveContinous = false; // Stop walking...
		s_pPlayer->ActionMove(PSM_STOP);
		
		if(m_pUICmd->m_pBtn_Act_Run) m_pUICmd->m_pBtn_Act_Run->SetState(UI_STATE_BUTTON_NORMAL);
		if(m_pUICmd->m_pBtn_Act_Walk) m_pUICmd->m_pBtn_Act_Walk->SetState(UI_STATE_BUTTON_NORMAL);

		this->MsgSend_Move(false, false); // Send motion stop packets.
	}

	if(s_pPlayer->m_bAttackContinous) // If you are attacking...
	{
		CPlayerBase* pTarget = s_pOPMgr->CharacterGetByID(s_pPlayer->m_iIDTarget, false);
		if(s_pPlayer->IsAttackableTarget(pTarget)) // Attack if possible.
			s_pPlayer->Action(PSA_ATTACK, false, pTarget); // attack
	}
}

void CGameProcMain::CommandEnableAttackContinous(bool bEnable, CPlayerBase* pTarget)
{
	if(bEnable == s_pPlayer->m_bAttackContinous) return;
	if(bEnable)
	{
		this->CloseUIs(); // Various commerce, warp, etc... Closing the UI...
		s_pUIMgr->UserMoveHideUIs();

		if(s_pPlayer->m_bStun) return; // I&#39;m stunned, I can&#39;t attack.
		if(nullptr == pTarget) return;
		s_pPlayer->RotateTo(pTarget); // turn
		if(pTarget->m_InfoBase.eNation == s_pPlayer->m_InfoBase.eNation) return; // If the country is the same, skip it.
	}
	s_pPlayer->m_bAttackContinous = bEnable; // Record the status...

	if(bEnable)
		SetGameCursor(s_hCursorAttack);
	else
	{
		const e_Nation eNation = s_pPlayer->m_InfoBase.eNation;
		SetGameCursor(((NATION_ELMORAD == eNation) ? s_hCursorNormal1 : s_hCursorNormal));
	}

	if(m_pUICmd->m_pBtn_Act_Attack)
	{
		if(bEnable) m_pUICmd->m_pBtn_Act_Attack->SetState(UI_STATE_BUTTON_DOWN);
		else m_pUICmd->m_pBtn_Act_Attack->SetState(UI_STATE_BUTTON_NORMAL);
	}

	std::string szMsg;
	if(	bEnable ) // Auto attack!
	{
		::_LoadStringFromResource(IDS_MSG_ATTACK_START, szMsg);
		szMsg = pTarget->IDString() + szMsg;
		this->PlayBGM_Battle();
		
		if(s_pPlayer->IsAttackableTarget(pTarget))
			s_pPlayer->Action(PSA_BASIC, true, pTarget);
	}
	else // No auto attack.
	{
		::_LoadStringFromResource(IDS_MSG_ATTACK_STOP, szMsg);
		s_pPlayer->Action(PSA_BASIC, true, pTarget);
	}

	this->MsgOutput(szMsg, 0xff00ffff);


	if(	bEnable && false == s_pPlayer->IsAttackableTarget(pTarget)) // Check the country, distance and angle and return if it is impossible to attack.
	{
		std::string szMsg; ::_LoadStringFromResource(IDS_MSG_ATTACK_DISABLE, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		return;
	}
}

bool CGameProcMain::CommandToggleUIState()
{
	const bool bNeedOpen = !(m_pUIVar->IsVisible());
	if(bNeedOpen)
	{
		s_pUIMgr->SetFocusedUI(m_pUIVar);
		m_pUIVar->Open();
	}
	else m_pUIVar->Close();

	// Command button update..
	// if(m_pUICmd->m_pBtn_Character)
	// {
		// if(bNeedOpen) m_pUICmd-&gt;m_pBtn_Character-&gt;SetState(UI_STATE_BUTTON_DOWN); // Release button press..
		// else m_pUICmd-&gt;m_pBtn_Character-&gt;SetState(UI_STATE_BUTTON_NORMAL); // 버튼 누름
	// }

	return bNeedOpen;
}

bool CGameProcMain::CommandToggleUIInventory()
{
	bool bNeedOpen = false;

	// If you are dealing privately...
	if (m_pSubProcPerTrade->m_ePerTradeState != PER_TRADE_STATE_NONE)
		return bNeedOpen;

	if (m_pUINpcEvent->IsVisible())
		return bNeedOpen;

	if( m_pUIInn->IsVisible()) return bNeedOpen;
	if( m_pUICreateClanName->IsVisible()) return bNeedOpen;

	if (m_pUIWareHouseDlg->IsVisible())
	{
		if (m_pUIInventory->IsVisible())
			m_pUIInventory->Close(true);
		return bNeedOpen;
	}

	if (m_pUITransactionDlg->IsVisible())
	{
		if (m_pUIInventory->IsVisible())
			m_pUIInventory->Close(true);
		return bNeedOpen;
	}
	
	if (m_pUIInventory->IsVisible())
	{
		m_pUIInventory->Close(true);
		return bNeedOpen;
	}
	else
	{
		bNeedOpen = true;
		if(m_pUISkillTreeDlg->IsVisible()) m_pUISkillTreeDlg->Close();
		
		s_pUIMgr->SetFocusedUI(m_pUIInventory);
		m_pUIInventory->Open();
	}

	return bNeedOpen;
}

bool CGameProcMain::CommandToggleUISkillTree()
{
	const bool bNeedOpen = !(m_pUISkillTreeDlg->IsVisible());

	// If you are dealing privately...
	if (m_pSubProcPerTrade->m_ePerTradeState != PER_TRADE_STATE_NONE)
		return bNeedOpen;

	if (bNeedOpen)
	{
		if (m_pUIInventory->IsVisible())
			m_pUIInventory->Close();
		if (m_pUITransactionDlg->IsVisible())
			m_pUITransactionDlg->LeaveTransactionState();
		if (m_pUIWareHouseDlg->IsVisible())
			m_pUIWareHouseDlg->LeaveWareHouseState();

		s_pUIMgr->SetFocusedUI(m_pUISkillTreeDlg);
		m_pUISkillTreeDlg->Open();
	}
	else
	{
		m_pUISkillTreeDlg->Close();
	}

	return bNeedOpen;
}

bool CGameProcMain::CommandToggleUIMiniMap()
{
	return m_pUIStateBarAndMiniMap->ToggleMiniMap();
}

void CGameProcMain::CommandCameraChange() // Change the camera perspective...
{
	if(VP_THIRD_PERSON == s_pEng->ViewPoint() && s_pPlayer->m_bTargetOrPosMove)
	{
		s_pPlayer->m_bMoveContinous = false;
		s_pPlayer->m_bTargetOrPosMove = false;
		CommandToggleMoveContinous();
	}

	s_pEng->ViewPointChange(VP_UNKNOWN); // Change the point of view in order.
}

void CGameProcMain::MsgOutput(const std::string& szMsg, D3DCOLOR crMsg)
{
	m_pUIMsgDlg->AddMsg(szMsg, crMsg);
}

bool CGameProcMain::MsgRecv_ItemDroppedGetResult(DataPack* pDataPack, int& iOffset)	// The result of eating an item that fell on the ground...
{
	BYTE	bResult; 
	BYTE	bPos;
	int		iItemID = 0;
	int		iGoldID = 0;
	int		iStrLen = 0;
	short	sItemCount = 0;
	std::string szString = "";

	bResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	if ( (bResult == 0x01) || (bResult == 0x02) || (bResult == 0x05) )
	{
		bPos	= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		iItemID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
			if ( (bResult == 0x01) || (bResult == 0x05) )
			{
				sItemCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			}
		iGoldID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	}


	if (bResult == 0x03)
	{
		iItemID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
		iStrLen = (int)CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szString, iStrLen);
	}

	TRACE("받음 - Item Get %d %d\n", bResult, iGoldID);

	if ( m_pUIDroppedItemDlg )
		m_pUIDroppedItemDlg->GetItemByIDToInventory(bResult, iItemID, iGoldID, bPos, sItemCount, iStrLen, szString);
	return true;
}

void CGameProcMain::MsgRecv_ZoneChange(DataPack* pDataPack, int& iOffset)
{
	const int iZone	= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	const float fX	= (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	const float fZ	= (CAPISocket::Parse_GetWord(pDataPack->m_pData, iOffset))/10.0f;
	const float fY	= (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;
	const int iVictoryNation = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	CGameProcedure::LoadingUIChange(iVictoryNation);


	__Vector3 vPosPlayer;
	vPosPlayer.x = fX;
	vPosPlayer.y = fY;
	vPosPlayer.z = fZ;
	this->InitPlayerPosition(vPosPlayer); // Initialize the player position.. Raise him up and make him take the basic action.
	s_pPlayer->RegenerateCollisionMesh(); // Recreate the collision mesh.
	s_pPlayer->m_iSendRegeneration = 0; // Once sent, a flag that will not be seen until it dies again
	s_pPlayer->m_fTimeAfterDeath = 0; // Once sent, a flag that will not be seen until it dies again

	if(s_pPlayer->IsDead())
	{
		TRACE("ZoneChange - 다시 살아나기(%.1f, %.1f)\n", fX, fZ);

		// Magic &amp; Effect Reset..
		if(m_pUIStateBarAndMiniMap) m_pUIStateBarAndMiniMap->ClearMagic();
		if(m_pMagicSkillMng) m_pMagicSkillMng->ClearDurationalMagic();
		if(CGameProcedure::s_pFX) s_pFX->StopMine();

		if(s_pPlayer->Nation()==NATION_KARUS) CGameProcedure::s_pFX->TriggerBundle(s_pPlayer->IDNumber(), -1, FXID_REGEN_KARUS, s_pPlayer->IDNumber(), -1);
		else if(s_pPlayer->Nation()==NATION_ELMORAD) CGameProcedure::s_pFX->TriggerBundle(s_pPlayer->IDNumber(), -1, FXID_REGEN_ELMORAD, s_pPlayer->IDNumber(), -1);
	}

	this->InitZone(iZone, __Vector3(fX, fY, fZ)); // Zone Update
}

void CGameProcMain::MsgRecv_UserState(DataPack* pDataPack, int& iOffset)
{
	const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const auto eSP = (e_SubPacket_State)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset); // 0x01
	const int iState = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	CPlayerBase* pBPC = nullptr;
	if ( s_pPlayer->IDNumber() == iID ) pBPC = s_pPlayer;
	else pBPC = s_pOPMgr->UPCGetByID(iID, false); 
	
	if(nullptr == pBPC) return;

	if(N3_SP_STATE_CHANGE_SITDOWN == eSP) // Sitting, standing, dying...
	{
		if(pBPC != s_pPlayer) // When not a player...
		{
			if(0x01 == iState) 
				pBPC->Action(PSA_BASIC, true);
			else if(0x02 == iState) 
				pBPC->Action(PSA_SITDOWN, true);
			else if(0x03 == iState) 
				pBPC->Action(PSA_DYING, false, nullptr, true);
		}
	}
	else if(N3_SP_STATE_CHANGE_RECRUIT_PARTY == eSP) // Looking for a party...
	{
		if(0x01 == iState) 
		{
			pBPC->InfoStringSet("", 0);
		}
		else if(0x02 == iState) 
		{
			const int iLevel = pBPC->m_InfoBase.iLevel;
			int iLMin = iLevel - 8;
			if(iLMin < 0) iLMin = 0;
			int iLMax = iLevel + 8;
			if(iLMax > 60) iLMax = 60;

			char szBuff[128];
			std::string szMsg;
			::_LoadStringFromResource(IDS_WANT_PARTY_MEMBER, szMsg);
			sprintf(szBuff, szMsg.c_str(), iLMin, iLMax);
			pBPC->InfoStringSet(szBuff, 0xff00ff00);
		}
	}
	else if(N3_SP_STATE_CHANGE_SIZE == eSP) // change in size
	{
		if(0x01 == iState)
		{
			pBPC->ScaleSetGradually(1.0f); // normal size..
			pBPC->FlickerFactorSet(1.0f); // Resurrected, translucent... flickering...
		}
		else if(0x02 == iState) pBPC->ScaleSetGradually(2.0f); // got bigger...
		else if(0x03 == iState) pBPC->ScaleSetGradually(0.5f);// got smaller
		else if(0x04 == iState) pBPC->FlickerFactorSet(0.7f); // Resurrected, translucent... flickering...
	}
	else if(N3_SP_STATE_CHANGE_ACTION == eSP) // change in size
	{
		if(1 == iState) pBPC->AnimationAdd(ANI_GREETING0, true); // greeting
		else if(11 == iState) pBPC->AnimationAdd(ANI_WAR_CRY1, true); // provocation
	}
}

void CGameProcMain::MsgRecv_Notice(DataPack* pDataPack, int& iOffset)
{
	if(m_pUINotice) m_pUINotice->RemoveNotice();

	const int iNoticeCount = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	for(int i = 0; i < iNoticeCount; i++)
	{
		const int iStrLen = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		if(iStrLen <= 0) continue;

		std::string szNotice;
		CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szNotice, iStrLen);
		if(m_pUINotice) m_pUINotice->m_Texts.push_back(szNotice);
	}

	if(m_pUINotice && iNoticeCount > 0)
	{
		m_pUINotice->GenerateText();

		const RECT rc = m_pUINotice->GetRegion();
		const int x = (CN3Base::s_CameraData.vp.Width/2) - (rc.right - rc.left)/2;
		const int y = (CN3Base::s_CameraData.vp.Height/2) - (rc.bottom - rc.top)/2;
		m_pUINotice->SetPos(x, y);
		m_pUINotice->SetVisible(true);
	}
}

void CGameProcMain::MsgRecv_PartyOrForce(DataPack* pDataPack, int& iOffset)
{
// int iPartyOrForce = CAPISocket::Parse_GetByte(pDataPack-&gt;m_pData, iOffset);
	int iSubCmd	= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	
	switch(iSubCmd)
	{
		case N3_SP_PARTY_OR_FORCE_PERMIT:			// 0x02 // Send - b1(YesNo) | Recv - s1(ID) The ID of the person making the request
		{
			int iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			int iStrLen		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			std::string szID;
			CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szID, iStrLen);
			
			if(iID >= 0)
			{
				std::string szMsg; ::_LoadStringFromResource(IDS_PARTY_PERMIT, szMsg);
				CGameProcedure::MessageBoxPost(szID + szMsg, "", MB_YESNO, BEHAVIOR_PARTY_PERMIT);
			}
		}
		break;

		case N3_SP_PARTY_OR_FORCE_INSERT:			// 0x02 // Send - s1(ID) | Recv - s3(ID, HPMax, HP) b2(Level, Class) - Find out the string by ID.
		{
			int iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			if(iID >= 0)
			{
				int iIDLength	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
				std::string szID; CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szID, iIDLength);
				int iHPMax		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
				int iHP			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
				int iLevel		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
				auto eClass	= (e_Class)(CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset));

				m_pUIPartyOrForce->MemberAdd(iID, szID, iLevel, eClass, iHP, iHPMax); // Add another party...
				if(iID != s_pPlayer->IDNumber()) // Output message if not self.
				{
					std::string szMsg; ::_LoadStringFromResource(IDS_PARTY_INSERT, szMsg);
					this->MsgOutput(szID + szMsg, D3DCOLOR_ARGB(255,255,255,255));
				}
			}
			else // I can&#39;t come to the party...
			{
				std::string szMsg;

				if(-1 == iID) ::_LoadStringFromResource(IDS_PARTY_INSERT_ERR_REJECTED, szMsg); // The other party refused to join the party.
				else if(-2 == iID) ::_LoadStringFromResource(IDS_PARTY_INSERT_ERR_LEVEL_DIFFERENCE, szMsg); // The level difference is too big...
				else if(-3 == iID) ::_LoadStringFromResource(IDS_PARTY_INSERT_ERR_INVALID_NATION, szMsg); // It&#39;s a country where you can&#39;t party.
				else ::_LoadStringFromResource(IDS_PARTY_INSERT_ERR, szMsg); // The other party refused to join the party.

				this->MsgOutput(szMsg, D3DCOLOR_ARGB(255,255,255,255));
				if(m_pUIPartyOrForce->MemberCount() == 1) m_pUIPartyOrForce->MemberDestroy(); // If there is only one member, it is the case that I created the party.
			}
			
			this->UpdateUI_PartyOrForceButtons(); // Updates the party button on the command line as needed.
		}
		break;
	
		case N3_SP_PARTY_OR_FORCE_REMOVE:			// 0x03	// Send - s1(ID) | Recv - s1(ID) -
		{
			int iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

			if(iID == s_pPlayer->IDNumber())
			{
				std::string szMsg; ::_LoadStringFromResource(IDS_PARTY_DESTROY, szMsg); // left the party...
				this->MsgOutput(szMsg, D3DCOLOR_ARGB(255,255,255,255));  // party release message
				m_pUIPartyOrForce->MemberDestroy(); // If you&#39;re yourself... you&#39;ll have a party.
			}
			else
			{
				int iMemberIndex = -1;
				const __InfoPartyOrForce* pInfo = m_pUIPartyOrForce->MemberInfoGetByID(iID, iMemberIndex);
				if(pInfo)
				{
					std::string szMsg; ::_LoadStringFromResource(IDS_PARTY_LEAVE, szMsg); // left the party...
					this->MsgOutput(pInfo->szID + szMsg, D3DCOLOR_ARGB(255,255,255,255)); // A message saying someone left the party...
					m_pUIPartyOrForce->MemberRemove(iID); // If others...
				}
			}

			this->UpdateUI_PartyOrForceButtons(); // Updates the party button on the command line as needed.
		}
		break;
		
		case N3_SP_PARTY_OR_FORCE_DESTROY:			// 0x04	// Send
		{
			m_pUIPartyOrForce->MemberDestroy(); // I&#39;m having a party..
			std::string szMsg; ::_LoadStringFromResource(IDS_PARTY_DESTROY, szMsg);
			this->MsgOutput(szMsg, D3DCOLOR_ARGB(255,255,255,255));

			this->UpdateUI_PartyOrForceButtons(); // Updates the party button on the command line as needed.
		}
		break;

		case N3_SP_PARTY_OR_FORCE_HP_CHANGE:		// 0x05 // Recv - s3(ID, HPMax, HP) - If you are yourself, you must break the party.
		{
			int iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			int iHPMax		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			int iHP			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

			m_pUIPartyOrForce->MemberHPChange(iID, iHP, iHPMax);
		}
		break;
		
		case N3_SP_PARTY_OR_FORCE_LEVEL_CHANGE:		// 0x06	// Recv - s1(ID), b1(Level)
		{
			int iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			int iLevel		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

			m_pUIPartyOrForce->MemberLevelChange(iID, iLevel);
		}
		break;
		
		case N3_SP_PARTY_OR_FORCE_CLASS_CHANGE:		// 0x07 // Recv - s1(ID), b1(Class) Rarely, but when changing jobs...
		{
			int iID			= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			auto eClass	= (e_Class)(CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset));

			m_pUIPartyOrForce->MemberClassChange(iID, eClass);
		}
		break;
		
		case N3_SP_PARTY_OR_FORCE_STATUS_CHANGE:	// 0x08 // Recv - s1(ID), b1(Status)...poison, curse, persistent magic, blessing
		{
			int iID	=			CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			auto ePS =	(e_PartyStatus)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			int iSuffer = 		CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			bool bSuffer = (iSuffer) ? true : false; 

			m_pUIPartyOrForce->MemberStatusChange(iID, ePS, bSuffer);
		}
		break;
	}
}

void CGameProcMain::CommandSitDown(bool bLimitInterval, bool bSitDown, bool bImmediately)
{
	if(bSitDown == s_pPlayer->m_bSitDown) return; // If the status is the same, go back.

	// Seating and standing restrictions.
	if(bLimitInterval)
	{
		const float fTime = CN3Base::TimeGet();
		static float fTimePrev = fTime - 4.0f;
		if(fTime - fTimePrev < 3.0f) return;
		fTimePrev = fTime;
	}

	e_StateAction eSA = s_pPlayer->State();
	const e_StateMove eSM = s_pPlayer->StateMove();

	if(bSitDown)
	{
		if(eSM != PSM_STOP) // If you are running, stop.
			return;

		if(this->m_pMagicSkillMng->IsCasting()) // You can&#39;t sit down while casting a skill.
			return;

		if(s_pPlayer->m_bAttackContinous) // If you keep attacking...
			this->CommandEnableAttackContinous(false, nullptr); // Continue to cancel the attack...
	}

	bool bVBs[2] = { false, false };
	int iState = 0;
	if(bSitDown)
	{
		eSA = PSA_SITDOWN;
		bVBs[0] = false; bVBs[1] = true;
		iState = 0x02;
	} // If you sit down
	else
	{
		eSA = PSA_BASIC;
		bVBs[0] = true; bVBs[1] = false;
		iState = 0x01;
	} // if you stand up
	
	s_pPlayer->m_bSitDown = bSitDown;
	s_pPlayer->Action(eSA, true, nullptr, bImmediately);

	if(m_pUICmd->m_pBtn_Act_SitDown)
	{
		m_pUICmd->m_pBtn_Act_SitDown->SetVisible(bVBs[0]);
		m_pUICmd->m_pBtn_Act_SitDown->SetState(UI_STATE_BUTTON_DOWN); // button press
	}
	if(m_pUICmd->m_pBtn_Act_StandUp)
	{
		m_pUICmd->m_pBtn_Act_StandUp->SetVisible(bVBs[1]);
		m_pUICmd->m_pBtn_Act_StandUp->SetState(UI_STATE_BUTTON_DOWN); // button press
	}

	this->MsgSend_StateChange(N3_SP_STATE_CHANGE_SITDOWN, iState); // sat down.. packets..
}

void CGameProcMain::CommandTargetSelect_NearstEnemy() // Grab the closest enemy target.
{
	CPlayerNPC* pTarget = s_pOPMgr->CharacterGetByNearstEnemy(s_pPlayer->m_InfoBase.eNation, s_pPlayer->Position());
	this->TargetSelect(pTarget);
	s_pPlayer->RotateTo(pTarget);
}

void CGameProcMain::CommandTargetSelect_NearstOurForce() // Find the nearest party target.
{
	CPlayerOther* pTarget = m_pUIPartyOrForce->MemberGetByNearst(s_pPlayer->Position());
	this->TargetSelect(pTarget);
	s_pPlayer->RotateTo(pTarget);
}

void CGameProcMain::CloseUIs()
{
}

void CGameProcMain::MsgSend_StateChange(e_SubPacket_State eSP, int iState)
{
	BYTE byBuff[4];											// Packet buffer...
	int iOffset=0;											// Packet Offset...

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_STATE_CHANGE);	// state change...
	CAPISocket::MP_AddByte(byBuff, iOffset, eSP);
	CAPISocket::MP_AddByte(byBuff, iOffset, iState);

	s_pSocket->Send(byBuff, iOffset);	
}

void CGameProcMain::MsgSend_PerTradeReq(int iDestID, bool bNear)
{
	BYTE byBuff[4];											// Packet buffer...
	int iOffset=0;											// Packet Offset...

	CAPISocket::MP_AddByte(byBuff, iOffset, N3_PER_TRADE);			
	CAPISocket::MP_AddByte(byBuff, iOffset, N3_SP_PER_TRADE_REQ);		
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)iDestID );			// Opponent ID.
	if(bNear)
		CAPISocket::MP_AddByte(byBuff, iOffset, 1);
	else
		CAPISocket::MP_AddByte(byBuff, iOffset, 2);
	// 1: Normal transaction
	// 2: Commerce board transaction

	s_pSocket->Send(byBuff, iOffset);									// sent...

	TRACE("아이디: %d, 아이템 거래 신청 패킷 보냄.. \n", iDestID);
}

void CGameProcMain::MsgRecv_PerTrade(DataPack* pDataPack, int& iOffset)
{
	const BYTE	bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	short	sOtherID, sItemCount, sCount, sDurability;
	BYTE	bResult, bItemPos;
	int		iItemID, iCount, iTotalGold;

	switch ( bSubCom )
	{
		case N3_SP_PER_TRADE_REQ:
			TRACE("아이템 거래 신청 패킷 받음.. \n");
			sOtherID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	

			if (m_pUITransactionDlg->IsVisible()) 	/* If you are trading with a merchant... */
			{
				// refuse..
				m_pUITransactionDlg->LeaveTransactionState();
				m_pSubProcPerTrade->LeavePerTradeState(PER_TRADE_RESULT_MY_DISAGREE);
				break;
			}

			if (m_pUIWareHouseDlg->IsVisible()) 	/* If it is stored in the storage box.. */
			{
				// refuse..
				m_pUIWareHouseDlg->LeaveWareHouseState();
				m_pSubProcPerTrade->LeavePerTradeState(PER_TRADE_RESULT_MY_DISAGREE);
				break;
			}

			if ( m_pUIInventory->IsVisible() )
				m_pUIInventory->Close();

			if (m_pSubProcPerTrade->m_ePerTradeState != PER_TRADE_STATE_NONE)
			{
				std::string stdMsg;
				::_LoadStringFromResource(IDS_PER_TRADEING_OTHER, stdMsg);
				CGameProcedure::s_pProcMain->MsgOutput(stdMsg, 0xff9b9bff);
				break;
			}

			if (m_pUIQuestMenu->IsVisible())
				m_pUIQuestMenu->SetVisible(false);

			if (m_pUIQuestTalk->IsVisible())
				m_pUIQuestTalk->SetVisible(false);

			if (m_pUINpcEvent->IsVisible())
				m_pUINpcEvent->Close();

			if (m_pUITradeBBSSelector->IsVisible())
				m_pUITradeBBSSelector->SetVisible(false);

			if(m_pUITradeBBS->IsVisible())
				m_pUITradeBBS->SetVisible(false);

			m_pSubProcPerTrade->ReceiveMsgPerTradeReq(sOtherID);
			break;


		case N3_SP_PER_TRADE_AGREE:
			bResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			m_pSubProcPerTrade->ReceiveMsgPerTradeAgree(bResult);
			break;

		case N3_SP_PER_TRADE_ADD:
			bResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			m_pSubProcPerTrade->ReceiveMsgPerTradeAdd(bResult);
			break;

		case N3_SP_PER_TRADE_OTHER_ADD:
			iItemID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
			iCount  = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
			sDurability = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			m_pSubProcPerTrade->ReceiveMsgPerTradeOtherAdd(iItemID, iCount, (int)sDurability);			
			break;

		case N3_SP_PER_TRADE_OTHER_DECIDE:
			m_pSubProcPerTrade->ReceiveMsgPerTradeOtherDecide();
			break;

		case N3_SP_PER_TRADE_DONE:
			bResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			if ( bResult == 0x01 )		// If successful...
			{	
				iTotalGold = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
				m_pSubProcPerTrade->ReceiveMsgPerTradeDoneSuccessBegin(iTotalGold);
				sItemCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
				for(auto i = 0; i < sItemCount; i++ )
				{
					bItemPos = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	
					iItemID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
					sCount  = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	
					sDurability = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
					m_pSubProcPerTrade->ReceiveMsgPerTradeDoneItemMove(bItemPos, iItemID, sCount, sDurability);					
				}
				m_pSubProcPerTrade->ReceiveMsgPerTradeDoneSuccessEnd();
			}
			else						// If it fails...
				m_pSubProcPerTrade->ReceiveMsgPerTradeDoneFail();
			break;

		case N3_SP_PER_TRADE_CANCEL:
				m_pSubProcPerTrade->ReceiveMsgPerTradeCancel();
			break;
	}
}

void CGameProcMain::TargetSelect(int iID, bool bMustAlive)
{
	CPlayerNPC* pTarget = s_pOPMgr->CharacterGetByID(iID, bMustAlive);
	this->TargetSelect(pTarget);
}

void CGameProcMain::TargetSelect(CPlayerNPC* pTarget)
{
	if(PSA_SPELLMAGIC == s_pPlayer->State()) // You cannot change the target while casting magic.
		return;

	const int iIDTargetPrev = s_pPlayer->m_iIDTarget; // Remember the previous target...
	s_pPlayer->m_iIDTarget = -1;
	s_pPlayer->m_pObjectTarget = nullptr;

	if(pTarget && pTarget->m_InfoBase.iAuthority != AUTHORITY_MANAGER)
	{ // In order for the operator not to give focus...
		s_pPlayer->m_iIDTarget = pTarget->IDNumber();
		if(iIDTargetPrev != s_pPlayer->m_iIDTarget) // Different from the previous target
		{
			this->CommandEnableAttackContinous(false, nullptr);	// Cancel auto attack...

			D3DCOLOR crID = 0xffffffff;
			if(	pTarget->PlayerType() == PLAYER_OTHER) // User..
			{
				if(pTarget->m_InfoBase.eNation != s_pPlayer->m_InfoBase.eNation) crID = 0xffff4040; // in other countries
				else crID = 0xff6b9fff;
			}
			else // NPC
			{
				if(pTarget->m_InfoBase.eNation != s_pPlayer->m_InfoBase.eNation) crID = 0xffff6060; // in other countries
				else crID = 0xff1064ff;
			}
			
			if( pTarget->IsAlive() )// Only the living state shows the target bar.
			{
				m_pUITargetBar->SetVisible(true);
				m_pUITargetBar->SetIDString(pTarget->IDString(), crID); // put your name in...
				this->MsgSend_RequestTargetHP(s_pPlayer->m_iIDTarget, 0x01);	// Request HP information from the server.
			}
			else
			{
				m_pUITargetBar->SetVisible(false); // Remove the target bar...
			}
		}
	}
	else
	{
		s_pPlayer->m_iIDTarget = -1; // Get the target ID.
		m_pUITargetBar->SetVisible(false); // Remove the target bar...

		this->CommandEnableAttackContinous(false, nullptr); // Cancel auto attack...
	}

	if(	PSA_SITDOWN != s_pPlayer->State() && 
		PSM_STOP == s_pPlayer->StateMove() &&
		PSA_BASIC == s_pPlayer->State()) s_pPlayer->Action(PSA_BASIC, true); // Change your basic posture...

	this->UpdateUI_PartyOrForceButtons(); // Updates the party button on the command line as needed.
}

void CGameProcMain::MsgRecv_SkillChange(DataPack* pDataPack, int& iOffset)			// Skill change...
{
	const int iType	= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	const int iValue	= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	m_pUISkillTreeDlg->m_iSkillInfo[iType] = iValue;
	m_pUISkillTreeDlg->m_iSkillInfo[0]++;
	m_pUISkillTreeDlg->InitIconUpdate();		// Since the skill points have changed.. the skills can also change..
}

void CGameProcMain::MsgRecv_MagicProcess(DataPack* pDataPack, int& iOffset)
{
	const auto eSP = (e_SubPacket_Magic)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	
	switch ( eSP )
	{
	case N3_SP_MAGIC_CASTING:
		m_pMagicSkillMng->MsgRecv_Casting(pDataPack, iOffset);
		break;
	case N3_SP_MAGIC_FLYING:
		m_pMagicSkillMng->MsgRecv_Flying(pDataPack, iOffset);
		break;
	case N3_SP_MAGIC_EFFECTING:
		m_pMagicSkillMng->MsgRecv_Effecting(pDataPack, iOffset);
		break;
	case N3_SP_MAGIC_FAIL:
		m_pMagicSkillMng->MsgRecv_Fail(pDataPack, iOffset);
		break;
	case N3_SP_MAGIC_TYPE4BUFFTYPE:
		m_pMagicSkillMng->MsgRecv_BuffType(pDataPack, iOffset);
		break;
	}	
}

void CGameProcMain::MsgRecv_ClassChange(DataPack* pDataPack, int& iOffset)			// job change...
{
	const auto eSP = (e_SubPacket_ClassChange)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	switch ( eSP )
	{
		case N3_SP_CLASS_CHANGE_SUCCESS:		// Success..
			m_pUIClassChange->Open(N3_SP_CLASS_CHANGE_SUCCESS);
			break;

		case N3_SP_CLASS_CHANGE_NOT_YET:		// Not Yet..
			m_pUIClassChange->Open(N3_SP_CLASS_CHANGE_NOT_YET);
			break;

		case N3_SP_CLASS_CHANGE_ALREADY:		// Already..
			m_pUIClassChange->Open(N3_SP_CLASS_CHANGE_ALREADY);
			break;

		case N3_SP_CLASS_CHANGE_FAILURE:		// Failure..
			m_pUIClassChange->RestorePrevClass();
			break;
	}
}

void CGameProcMain::MsgRecv_ObjectEvent(DataPack* pDataPack, int& iOffset)
{
	const int iType = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// Event Type
	const int iResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// result..

	if(OBJECT_TYPE_BINDPOINT == iType) // bind point
	{
		std::string szMsg;
		if(0x01 == iResult) ::_LoadStringFromResource(IDS_BIND_POINT_SUCCESS, szMsg);
		else ::_LoadStringFromResource(IDS_BIND_POINT_FAILED, szMsg);
		this->MsgOutput(szMsg, 0xff00ff00);
	}
	else if(OBJECT_TYPE_DOOR_LEFTRIGHT == iType ||
			OBJECT_TYPE_DOOR_TOPDOWN == iType ||
			OBJECT_TYPE_LEVER_TOPDOWN == iType ||
			OBJECT_TYPE_FLAG == iType) // Handling objects...
	{
		const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	// Gate ID to open and close
		const int iActivate = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// open and close...

		const CPlayerNPC* pNPC = s_pOPMgr->NPCGetByID(iID, true);
		__ASSERT(pNPC, "Invalid NPC ID");
		if(pNPC)
		{
			__ASSERT(pNPC->m_pShapeExtraRef && (pNPC->m_pShapeExtraRef->Type() & OBJ_SHAPE_EXTRA), "Invalid object!!");
			std::string szMsg = "Object Event - invalid object!!";
			if(pNPC->m_pShapeExtraRef)
			{
				__Vector3 vAxis(0,1,0);
				float fRadian = D3DXToRadian(90);
				bool bShouldBeRotate = true; // do i have to turn it??
				if(OBJECT_TYPE_DOOR_LEFTRIGHT == iType) // left and right open gates
				{
					if(0x01 == iActivate) { fRadian = D3DXToRadian(80); ::_LoadStringFromResource(IDS_DOOR_OPENED, szMsg); }
					else { fRadian = D3DXToRadian(0); ::_LoadStringFromResource(IDS_DOOR_CLOSED, szMsg); }
					vAxis.Set(0,1,0);
				} 
				else if(OBJECT_TYPE_DOOR_TOPDOWN == iType) // upper and lower opening gates
				{
					if(0x01 == iActivate) { fRadian = D3DXToRadian(90); ::_LoadStringFromResource(IDS_DOOR_OPENED, szMsg); }
					else  { D3DXToRadian(0); ::_LoadStringFromResource(IDS_DOOR_CLOSED, szMsg); }
					vAxis.Set(0,0,1);
				}
				else if(OBJECT_TYPE_LEVER_TOPDOWN == iType) // up and down lever
				{
					if(0x01 == iActivate) { fRadian = D3DXToRadian(-45); ::_LoadStringFromResource(IDS_LEVER_ACTIVATE, szMsg); }
					else  { fRadian = D3DXToRadian(45); ::_LoadStringFromResource(IDS_LEVER_DEACTIVATE, szMsg); }
					vAxis.Set(1,0,0);
				}
				else if(OBJECT_TYPE_FLAG == iType)
				{
					bShouldBeRotate = false; // do i have to turn it??
				}

				if(0x01 == iActivate)
				{
				}
				else // if(0x00 == iActivate);
				{
					::_LoadStringFromResource(IDS_DOOR_CLOSED, szMsg);
				}

				auto* pSE = (CN3ShapeExtra*)pNPC->m_pShapeExtraRef;
				if(bShouldBeRotate)
				{
					pSE->RotateTo(0, vAxis, fRadian, 1);
					pSE->RotateTo(1, vAxis, -fRadian, 1);
				}
				else 
				{
					if(iActivate) pSE->m_bVisible = true;
					else pSE->m_bVisible = false;
				}
			}
			this->MsgOutput(szMsg, 0xff00ff00);
		}
	}
	else
	{
		__ASSERT(0, "Unknown Object Event");
	}
}

void CGameProcMain::ParseChattingCommand(const std::string& szCmd)
{
	static char szCmds[4][1024] = { "", "", "", "" };
	static BYTE byBuff[1024] = "";
	sscanf(szCmd.c_str(), "/%s %s %s %s", szCmds[0], szCmds[1], szCmds[2], szCmds[3]);

	if(0 == lstrcmp(szCmds[0], "goto"))
	{
		auto fX = (float)atof(szCmds[1]);
		auto fZ = (float)atof(szCmds[2]);
		
		int iOffset = 0;
		CAPISocket::MP_AddByte(byBuff, iOffset, N3_WARP);
		CAPISocket::MP_AddWord(byBuff, iOffset, (fX * 10));
		CAPISocket::MP_AddWord(byBuff, iOffset, (fZ * 10));

		s_pSocket->Send(byBuff, iOffset);
	}
	
	e_ChatCmd eCmd = CMD_UNKNOWN;
	for(int i = 0; i < CMD_COUNT; i++)
	{
		if(0 == lstrcmpi(szCmds[0], s_szCmdMsg[i].c_str()))
		{
			eCmd = (e_ChatCmd)i;
			break;
		}
	}

	switch(eCmd)
	{
		case CMD_WHISPER:
		{
			this->MsgSend_ChatSelectTarget(szCmds[1]); // Set up a one-on-one chat partner.
		}
		break;

		case CMD_TOWN:
		{
			if(s_pPlayer->m_bStun) return; // I can&#39;t pass out..
			if(s_pPlayer->m_InfoBase.iHP * 2 >= s_pPlayer->m_InfoBase.iHPMax) // Must have more than half HP.
			{
				int iOffset = 0;
				CAPISocket::MP_AddWord(byBuff, iOffset, N3_HOME);		// go to town...
				s_pSocket->Send(byBuff, iOffset);
			}
			else // Must have more than half HP.
			{
				std::string szMsg; ::_LoadStringFromResource(IDS_ERR_GOTO_TOWN_OUT_OF_HP, szMsg);
				this->MsgOutput(szMsg, 0xffff00ff);
			}
		}
		break;

		case CMD_TRADE:
		{
			CPlayerOther* pOPC = s_pOPMgr->UPCGetByID(s_pPlayer->m_iIDTarget, true);
			if(	pOPC &&
				(pOPC->Position() - s_pPlayer->Position()).Magnitude() < (pOPC->Height() + 5.0f) && 
				!m_pUITransactionDlg->IsVisible() ) // Another player is caught as a target.. If you are close.. // Item trade between individuals.. // Unless you are in a commercial transaction..
			{
				std::string szMsg; ::_LoadStringFromResource(IDS_PERSONAL_TRADE_REQUEST, szMsg);
				MsgOutput(pOPC->IDString() + szMsg, 0xffffff00);

				MsgSend_PerTradeReq(pOPC->IDNumber());
				
				if (m_pUINpcEvent->IsVisible())
					m_pUINpcEvent->Close();

				if (m_pUIQuestTalk->IsVisible())
					m_pUIQuestTalk->SetVisible(false);

				if (m_pUIQuestMenu->IsVisible())
					m_pUIQuestMenu->SetVisible(false);

				m_pSubProcPerTrade->EnterWaitMsgFromServerStatePerTradeReq();
			}
		}
		break;

		case CMD_EXIT:
		{
			PostQuitMessage(0);
		}
		break;

		case CMD_PARTY:
		{
			CPlayerBase* pTarget = s_pOPMgr->UPCGetByID(s_pPlayer->m_iIDTarget, true);
			if(pTarget)
			{
				std::string szMsg;
				if(this->MsgSend_PartyOrForceCreate(0, pTarget->IDString())) ::_LoadStringFromResource(IDS_PARTY_INVITE, szMsg); // party request...
				else  ::_LoadStringFromResource(IDS_PARTY_INVITE_FAILED, szMsg); // Party invite failed
				this->MsgOutput(pTarget->IDString() + szMsg, 0xffffff00);
			}
		}
		break;

		case CMD_LEAVEPARTY:
		{
			this->MsgSend_PartyOrForceLeave(0); // party request...
		}
		break;

		case CMD_RECRUITPARTY:
		{
			if(m_pUIPartyBBS)
			{
				if(s_pPlayer->m_bRecruitParty)
				{
					m_pUIPartyBBS->MsgSend_RegisterCancel();
				}
				else
				{
					m_pUIPartyBBS->MsgSend_Register();
				}
			}

			// if(m_pUIPartyBBS && !m_pUIPartyBBS->IsVisible())
			// m_pUIPartyBBS->MsgSend_RefreshData(0);
		}
		break;

		case CMD_JOINCLAN:
		{
			if(s_pPlayer->m_InfoExt.eKnightsDuty == KNIGHTS_DUTY_CHIEF ||
				s_pPlayer->m_InfoExt.eKnightsDuty == KNIGHTS_DUTY_VICECHIEF)
			{			
				this->MsgSend_KnightsJoin(s_pPlayer->m_iIDTarget);
			}
		}
		break;

		case CMD_WITHDRAWCLAN:
		{
			this->MsgSend_KnightsWithdraw();
		}
		break;

		case CMD_FIRECLAN:
		{
			if(s_pPlayer->m_InfoExt.eKnightsDuty == KNIGHTS_DUTY_CHIEF)
			{
				std::string szName = szCmds[1];
				MsgSend_KnightsLeave(szName);			
			}
		}
		break;

		case CMD_APPOINTVICECHIEF:
		{
			if(s_pPlayer->m_InfoExt.eKnightsDuty == KNIGHTS_DUTY_CHIEF)
			{
				std::string szName = szCmds[1];
				MsgSend_KnightsAppointViceChief(szName);
			}
		}
		break;

		case CMD_GREETING:
		{
			if(	s_pPlayer->State() == PSA_BASIC && 
				s_pPlayer->StateMove() == PSM_STOP )
			{
				this->MsgSend_StateChange(N3_SP_STATE_CHANGE_ACTION, 1);
			}
		}
		break;

		case CMD_EXCITE:
		{
			if(	s_pPlayer->State() == PSA_BASIC && 
				s_pPlayer->StateMove() == PSM_STOP )
			{
				this->MsgSend_StateChange(N3_SP_STATE_CHANGE_ACTION, 11);
			}
		}
		break;

		case CMD_VISIBLE:
		{
			this->MsgSend_StateChange(N3_SP_STATE_CHANGE_VISIBLE, 0);
		}
		break;

		case CMD_INVISIBLE:
		{
			this->MsgSend_StateChange(N3_SP_STATE_CHANGE_VISIBLE, 255);
		}
		break;

		case CMD_CLEAN:
		{
			int iPercent = atoi(szCmds[1]);
			this->MsgSend_Weather(1, iPercent);
		}
		break;

		case CMD_RAINING:
		{
			int iPercent = atoi(szCmds[1]);
			this->MsgSend_Weather(2, iPercent);
		}
		break;

		case CMD_SNOWING:
		{
			int iPercent = atoi(szCmds[1]);
			this->MsgSend_Weather(3, iPercent);
		}
		break;

		case CMD_TIME:
		{
			int iHour = atoi(szCmds[1]);
			int iMin = atoi(szCmds[2]);
			this->MsgSend_Time(iHour, iMin);
		}
		break;

		case CMD_CU_COUNT:
		{
			int iOffset=0;
			CAPISocket::MP_AddByte(byBuff, iOffset, N3_CONCURRENT_USER_COUNT); 
			s_pSocket->Send(byBuff, iOffset);
		}
		break;

		case CMD_NOTICE:
		{
			if(szCmd.size() >= 7)
			{
				std::string szChat = szCmd.substr(6); // Any string except &quot;/notice&quot;
				this->MsgSend_Chat(N3_CHAT_PUBLIC, szChat);
			}
		}
		break;

		case CMD_ARREST:
		{
			this->MsgSend_Administrator(N3_SP_ADMINISTRATOR_ARREST, szCmds[1]); // Tracking
		}
		break;

		case CMD_FORBIDCONNECT:
		{
			this->MsgSend_Administrator(N3_SP_ADMINISTRATOR_FORBID_CONNECT, szCmds[1]); // access prohibited
		}
		break;
		
		case CMD_FORBIDCHAT:
		{
			this->MsgSend_Administrator(N3_SP_ADMINISTRATOR_CHAT_FORBID, szCmds[1]); // no chat
		}
		break;
		
		case CMD_PERMITCHAT:
		{
			this->MsgSend_Administrator(N3_SP_ADMINISTRATOR_CHAT_PERMIT, szCmds[1]); // chat permission
		}
		break;
		
		case CMD_GAME_SAVE:
		{
			if(m_fRequestGameSave > 300.0f)
			{
				BYTE byBuff[4];												// buffer..
				int iOffset=0;												// Offset...
				s_pSocket->MP_AddByte(byBuff, iOffset, N3_REQUEST_GAME_SAVE);	// Save request command..
				s_pSocket->Send(byBuff, iOffset);				// sent...
				m_fRequestGameSave = 0.0f;

				std::string szMsg;
				::_LoadStringFromResource(IDS_REQUEST_GAME_SAVE, szMsg);
				this->MsgOutput(szMsg, 0xffffff00);
			}
			else
			{
				char szBuf[256];
				std::string szMsg;
				::_LoadStringFromResource(IDS_DELAY_GAME_SAVE, szMsg);
				sprintf(szBuf, szMsg.c_str(), 5);
				this->MsgOutput(szBuf, 0xffffff00);
			}
		}
		break;

		default:
			break;
	} // end of switch(eCmd)
}

void CGameProcMain::UpdateUI_PartyOrForceButtons()
{
	// Change party button state...
	CPlayerBase* pTarget = nullptr;
	bool bIAmLeader = false, bIAmMemberOfParty = false;
	int iMemberIndex = -1;
	this->PartyOrForceConditionGet(bIAmLeader, bIAmMemberOfParty, iMemberIndex, pTarget);
	m_pUIPartyOrForce->MemberSelect(iMemberIndex);

	m_pUICmd->UpdatePartyButtons(bIAmLeader, bIAmMemberOfParty, iMemberIndex, pTarget);
}

const __InfoPartyOrForce* CGameProcMain::PartyOrForceConditionGet(bool& bIAmLeader, bool& bIAmMember, int& iMemberIndex, class CPlayerBase*& pTarget)
{
	// Change party button state...
	bIAmLeader = false;
	iMemberIndex = -1;
	bIAmMember = false;
	pTarget = s_pOPMgr->CharacterGetByID(s_pPlayer->m_iIDTarget, true);

	if(m_pUIPartyOrForce->MemberCount() >= 2)
	{
		bIAmMember = true;
		if(m_pUIPartyOrForce->MemberInfoGetByIndex(0)->iID == s_pPlayer->IDNumber()) bIAmLeader = true;
		return m_pUIPartyOrForce->MemberInfoGetByID(s_pPlayer->m_iIDTarget, iMemberIndex);
	}

	return nullptr;
}

void CGameProcMain::UpdateUI_MiniMap()
{
	if(nullptr == m_pUIStateBarAndMiniMap || !m_pUIStateBarAndMiniMap->IsVisible()) return;


	m_pUIStateBarAndMiniMap->PositionInfoClear();

	D3DCOLOR crType = 0xffffffff;
	const e_Nation eNation = s_pPlayer->m_InfoBase.eNation;

	auto it = s_pOPMgr->m_NPCs.begin(), itEnd = s_pOPMgr->m_NPCs.end();
	CPlayerNPC* pNPC = nullptr;
	for(; it != itEnd; it++) // NPC
	{
		pNPC = it->second;

		if(eNation != pNPC->m_InfoBase.eNation) crType = 0xff800000; // Other country NPCs or monsters Orange
		else crType = 0xff00a0ff; // same country NPC light blue

		m_pUIStateBarAndMiniMap->PositionInfoAdd(pNPC->IDNumber(), pNPC->Position(), crType, false);
	}

	auto it2 = s_pOPMgr->m_UPCs.begin(), itEnd2 = s_pOPMgr->m_UPCs.end();
	CPlayerOther* pUPC = nullptr;
	const __TABLE_ZONE* pZoneInfo = s_pTbl_Zones->Find(s_pPlayer->m_InfoExt.iZoneCur);
	for(; it2 != itEnd2; it2++) // User
	{
		pUPC = it2->second;

		bool bDrawTop = false;
		if(eNation != pUPC->m_InfoBase.eNation) // In case of enemy country
		{
			if(pUPC->State() == PSA_SITDOWN)
			{
				pUPC->m_InfoBase.bRenderID = false; // ID not displayed.
				continue; // If you sit down.. it will pass..
			}
			else
			{
				pUPC->m_InfoBase.bRenderID = true; // Show ID.
			}

			if(pZoneInfo && FALSE == pZoneInfo->bIndicateEnemyPlayer) continue; // If an enemy country should not be displayed.. it passes..
			
			crType = 0xffff4040; // Other countries - bright red
		}
		else // Same country...
		{
			int iMemberIndex = -1;
			const __InfoPartyOrForce* pPI = m_pUIPartyOrForce->MemberInfoGetByID(pUPC->IDNumber(), iMemberIndex);
			if(pPI)
			{
				bDrawTop = true; // draw on top
				crType = 0xffffff00; // Party member // Yellow
			}
			else
			{
				crType = 0xff0040ff; // Light blue unless you are a party member
			}
		}

		if(pUPC && pUPC->m_InfoBase.iAuthority != AUTHORITY_MANAGER)	// Points are marked on the mini-map only if you are not an operator.
			m_pUIStateBarAndMiniMap->PositionInfoAdd(pUPC->IDNumber(), pUPC->Position(), crType, bDrawTop);
	}
}

void CGameProcMain::UpdateUI_TargetBar()
{
	if(nullptr == m_pUITargetBar || !m_pUITargetBar->IsVisible()) return;

	// When a dead character is selected, the target bar is not drawn.
	CPlayerNPC* t_pTarget = s_pOPMgr->CharacterGetByID(s_pPlayer->m_iIDTarget, false);
	if( t_pTarget && t_pTarget->State() == PSA_DEATH )
	{
		m_pUITargetBar->SetVisible(false);
		return;
	}

	const CPlayerNPC* pTarget = s_pOPMgr->CharacterGetByID(s_pPlayer->m_iIDTarget, true);
	if(pTarget) // If the target is valid...
	{
		const float fTimeTmp = CN3Base::TimeGet();
		if(fTimeTmp > m_pUITargetBar->m_fTimeSendPacketLast + PACKET_INTERVAL_REQUEST_TARGET_HP) // Request target information after 1 second
		{
			this->MsgSend_RequestTargetHP(s_pPlayer->m_iIDTarget, false);
			m_pUITargetBar->m_fTimeSendPacketLast = fTimeTmp;
		}
	}
	else // If there is no target...
	{
		s_pPlayer->m_iIDTarget = -1;
		m_pUITargetBar->SetVisible(false);
	}
}

void CGameProcMain::UpdateBGM()
{
	if(nullptr == m_pSnd_Battle || !m_pSnd_Battle->IsPlaying() ) return;

	// if(s_pPlayer->pTarget && s_pPlayer->pTarget->IsAlive())
	const __Vector3 vPosPlayer = s_pPlayer->Position();
	const e_Nation eNationPlayer = s_pPlayer->m_InfoBase.eNation;

	bool bStopBattleBgm = true;
	CPlayerBase* pBPC;
	auto it = s_pOPMgr->m_NPCs.begin(), itEnd = s_pOPMgr->m_NPCs.end();
	for(; it != itEnd && bStopBattleBgm; it++)
	{
		pBPC = it->second;
		if(eNationPlayer == pBPC->m_InfoBase.eNation) continue;

		if((vPosPlayer - pBPC->Position()).Magnitude() < 12.0f)
			bStopBattleBgm = false;
	}

	CPlayerOther* pUPC;
	auto it2 = s_pOPMgr->m_UPCs.begin(), itEnd2 = s_pOPMgr->m_UPCs.end();
	for(; it2 != itEnd2 && bStopBattleBgm; it2++)
	{
		pUPC = it2->second;
		if(eNationPlayer == pUPC->m_InfoBase.eNation) continue;

		if((vPosPlayer - pUPC->Position()).Magnitude() < 12.0f)
			bStopBattleBgm = false;
	}

	if(bStopBattleBgm) this->PlayBGM_Town();
}

void CGameProcMain::UpdateCameraAndLight()
{
	__Vector3 vPosPlayer = s_pPlayer->Position();
	if(s_pPlayer->State() == PSA_SITDOWN)
	{
		const float fRootY = s_pPlayer->RootPosition().y;
		const float fH = s_pPlayer->Height();
		vPosPlayer.y += fRootY - (fH / 2.0f);
	}

	D3DCOLOR crDiffuses[MAX_GAME_LIGHT];
	D3DCOLOR crAmbients[MAX_GAME_LIGHT];
	for(int i = 0; i < MAX_GAME_LIGHT; i++)
	{
		crDiffuses[i] = ACT_WORLD->GetLightDiffuseColorWithSky(i);
		crAmbients[i] = ACT_WORLD->GetLightAmbientColorWithSky(i);
	}
	
	s_pEng->Tick(	crDiffuses, crAmbients, ACT_WORLD->GetFogColorWithSky(), 
					vPosPlayer, s_pPlayer->Rotation(),
					s_pPlayer->Height(), ACT_WORLD->GetSunAngleByRadinWithSky() );		// Enter the character position and the angle of the sun.
	s_pEng->ApplyCameraAndLight();	// Apply the values set for the camera and light to the D3D Device.
}

void CGameProcMain::MsgRecv_DurabilityChange(DataPack* pDataPack, int& iOffset)		// Durability changes...
{
	const auto eSlot = (e_ItemSlot)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	const int iCurValue  = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	

	s_pPlayer->DurabilitySet(eSlot, iCurValue);
	m_pUIInventory->DurabilityChange(eSlot, iCurValue);
}

void CGameProcMain::MsgRecv_NpcEvent(DataPack* pDataPack, int& iOffset)				// Npc Event(Exchange, Repair both)..
{
	const int iTradeID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);		// Trade id
	CPlayerNPC* pNPC = s_pOPMgr->NPCGetByID(s_pPlayer->m_iIDTarget, true);
	if (!pNPC)	return;

	m_pUINpcEvent->Open(NPC_EVENT_TRADE_REPAIR, iTradeID, pNPC->GetNPCOriginID());
}


void CGameProcMain::MsgRecv_Knights(DataPack* pDataPack, int& iOffset)
{
	const auto eSP = (e_SubPacket_Knights)(CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset));	// Sub Packet

	switch(eSP)
	{
	case N3_SP_KNIGHTS_CREATE: // produce..
		this->MsgRecv_Knights_Create(pDataPack, iOffset);
		break;
	case N3_SP_KNIGHTS_WITHDRAW: // secession
		this->MsgRecv_Knights_Withdraw(pDataPack, iOffset);
		break;
	case N3_SP_KNIGHTS_JOIN: // join
		this->MsgRecv_Knights_Join(pDataPack, iOffset);
		break;
	case N3_SP_KNIGHTS_MEMBER_REMOVE: // Delete member -
		this->MsgRecv_Knights_Leave(pDataPack, iOffset);
		break;
	case N3_SP_KNIGHTS_APPOINT_VICECHIEF: // Appointment of vice-captain - same as permission to join
		this->MsgRecv_Knights_AppointViceChief(pDataPack, iOffset);
 	break;
	case N3_SP_KNIGHTS_MEMBER_INFO_ALL:
		this->MsgRecv_Knights_MemberInfoAll(pDataPack, iOffset);
		break;
	case N3_SP_KNIGHTS_GRADE_CHANGE_ALL:
		this->MsgRecv_Knights_GradeChangeAll(pDataPack, iOffset);
		break;
	case N3_SP_KNIGHTS_DESTROY: // Crossover Send - | Recv - b1 (1: success 0: failure)
		{
			BYTE	bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			
			std::string szMsg;
			switch ( (e_SubPacket_KNights_Common)bSubCom )
			{
			case N3_SP_KNIGHTS_COMMON_SUCCESS: // Clan destruction success
				::_LoadStringFromResource(IDS_CLAN_WITHDRAW_SUCCESS, szMsg);
				m_pUIKnightsOp->KnightsInfoDelete(s_pPlayer->m_InfoExt.iKnightsID);
				this->MsgOutput(szMsg, 0xffffff00);
				break;
			case N3_SP_KNIGHTS_COMMON_DBFAIL: // DB search failed..
			case N3_SP_KNIGHTS_COMMON_FAIL_NONE_USER:	// no user...
				break;
			}
			s_pPlayer->m_InfoExt.eKnightsDuty = KNIGHTS_DUTY_UNKNOWN;
			s_pPlayer->KnightsInfoSet(0, "", 0, 0);
			m_pUIVar->UpdateKnightsInfo();
		}
		break;
	case N3_SP_KNIGHTS_DUTY_CHANGE: // Change of position...
		MsgRecv_Knights_Duty_Change(pDataPack, iOffset);
		break;
	case N3_SP_KNIGHTS_JOIN_REQ:
		MsgRecv_Knigts_Join_Req(pDataPack, iOffset);
		break;

	/* case N3_SP_KNIGHTS_APPOINT_CHIEF: // Appointment of leader - same as permission to join
		{
			::_LoadStringFromResource(IDS_KNIGHTS_APPOINT_CHIEF_SUCCESS, szMsg); // success
		}
		break;
	case N3_SP_KNIGHTS_DESTROY: // Crossover Send - | Recv - b1 (1: success 0: failure)
		{
			::_LoadStringFromResource(IDS_KNIGHTS_DESTROY_SUCCESS, szMsg); // success

			s_pPlayer->m_InfoExt.iKnightsID = 0;
			s_pPlayer->m_InfoExt.eKnightsDuty = KNIGHTS_DUTY_UNKNOWN;

			// subtract from the knights..
			m_pUIVar->m_pPageKnights->UpdateKnightsName("");
			m_pUIVar->m_pPageKnights->UpdateKnightsChiefName("");
			m_pUIVar->m_pPageKnights->UpdateKnightsDuty(KNIGHTS_DUTY_UNKNOWN);

			m_pUIKnightsOp-&gt;MsgSend_KnightsList(0); // Request the list of knights again.
		}
		break;

		case N3_SP_KNIGHTS_MEMBER_JOIN_ADMIT: // Permission to join members Send - s1(Knights ID) | Recv - b1 (1: success 0: failure)
		{
			::_LoadStringFromResource(IDS_KNIGHTS_ADMIT_SUCCESS, szMsg); // success
		}
		break;
		case N3_SP_KNIGHTS_MEMBER_JOIN_REJECT: // Reject member registration - same as registration permission
		{
			::_LoadStringFromResource(IDS_KNIGHTS_REJECT_SUCCESS, szMsg); // success
		}
		break;
		case N3_SP_KNIGHTS_MEMBER_PUNISH: // Discipline for members - same as permission to join
		{
			::_LoadStringFromResource(IDS_KNIGHTS_PUNISH_SUCCESS, szMsg); // success
		}
		break;
		case N3_SP_KNIGHTS_APPOINT_OFFICER: // Appointment of officers - same as permission to join
		{
			::_LoadStringFromResource(IDS_KNIGHTS_APPOINT_OFFICER_SUCCESS, szMsg); // success
		}
		break;
		case N3_SP_KNIGHTS_LIST: // request all lists Send - | s1(Knights Count) Loop { s1(Knights ID) s1(Name Length) str1 (Name) }
		{
			if(false == m_pUIKnightsOp->IsVisible())
			{
				sm_pUIKnightsOp-&gt;Open(s_pPlayer-&gt;m_InfoExt.eKnightsDuty); // Open the UI and initialize the list, etc..
			}
			else 
			{
				m_pUIKnightsOp-&gt;ChangeUIByDuty(s_pPlayer-&gt;m_InfoExt.eKnightsDuty); // Open the UI and initialize the list, etc..
			}
			m_pUIKnightsOp->MsgRecv_KnightsList(pDataPack, iOffset);
		}
		break;
	case N3_SP_KNIGHTS_MEMBER_INFO_ALL: // 전체 멤버 Send - s1(page) | s1(Member Count) Loop { s1(Name Length) str1 (Name) }
		{
			m_pUIVar->m_pPageKnights->MsgRecv_MemberInfo(pDataPack, iOffset);
		}
		break;
	case N3_SP_KNIGHTS_MEMBER_INFO_ONLINE: // Current access list Send - s1(page) | s1(Member Count) Loop { s1(Name Length) str1 (Name) }
		{
			m_pUIVar->m_pPageKnights->MsgRecv_MemberInfo(pDataPack, iOffset);
		}
		break;
	case N3_SP_KNIGHTS_STASH: // Knights Warehouse
		{
		}
		break;
	case N3_SP_KNIGHTS_DUTY_CHANGE: // Position change..
		{
			int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			e_KnightsDuty eDuty = (e_KnightsDuty)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

			if(iID == s_pPlayer-&gt;IDNumber()) // Change my position..
			{
				m_pUIVar->m_pPageKnights->UpdateKnightsDuty(eDuty);
			}
			else
			{
				CPlayerOther* pUPC = s_pOPMgr->UPCGetByID(iID, false);
				if(pUPC) // Change the position to another..
			}
		}
		break;
	default:
		__ASSERT(0, "Invalid Knights SubPacket");
		break;
*/	}
}

void CGameProcMain::MsgRecv_KnightsListBasic(DataPack* pDataPack, int& iOffset) // Get basic information about the Knights...
{
	const auto eSP = (e_SubPacket_KnightsList)(CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset));	// Sub Packet
	switch(eSP)
	{
	case N3_SP_KNIGHTS_LIST_BASIC_ALL: // Receive - s1(knights Count) { s21(id, namelength), str1(name) }
		{
			const int iCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	// number of knights
			for(int i = 0; i < iCount; i++)
			{
				std::string szID;
				const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// Templar ID
				const int iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// ID string length..
				CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szID, iLen);	// ID string..

				m_pUIKnightsOp->KnightsInfoInsert(iID, szID); // Receive all knights information..
			}
		}
		break;
	case N3_SP_KNIGHTS_LIST_BASIC_INSERT: // Receive - s2(id, name length), str1(name)
		{
			std::string szID;
			const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// Templar ID
			const int iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// ID string length..
			CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szID, iLen);	// ID string..

			m_pUIKnightsOp->KnightsInfoInsert(iID, szID); // Add guild information..
		}
		break;
	case N3_SP_KNIGHTS_LIST_BASIC_REMOVE: // Receive - s1(id)
		{
			const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// Templar ID
			m_pUIKnightsOp->KnightsInfoDelete(iID); // Clear guild information..
		}
		break;
	}
}

void CGameProcMain::MsgRecv_CompressedPacket(DataPack* pDataPack, int& iOffset) // This is compressed data... needs to be parsed one more time!!!
{
	short sCompLen, sOrgLen;
	DWORD dwCrcValue;
	sCompLen =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);	// Get Compressed Data Length...
	sOrgLen =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// Get original data length...
	dwCrcValue =	CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);	// Get CRC Value...

	const auto pOutBuf = new char[sOrgLen];
	LZ4_decompress_safe((char*)(pDataPack->m_pData + iOffset), pOutBuf, sCompLen, sOrgLen);
	const uint32_t dwCalculatedCrc = crc32c::Crc32c((const unsigned char*)pOutBuf, sOrgLen);

	if (dwCrcValue != dwCalculatedCrc)
	{
		return;
	}

	DataPack DataPackTemp;
	DataPackTemp.m_Size = sOrgLen;
	DataPackTemp.m_pData = (BYTE*)pOutBuf;
	int iOffset2 = 0;
	this->ProcessPacket(&DataPackTemp, iOffset2); // Parsing right away...
	DataPackTemp.m_Size = 0;
	DataPackTemp.m_pData = nullptr;
}

void CGameProcMain::MsgRecv_ContinousPacket(DataPack* pDataPack, int& iOffset) // This is compressed data... needs to be parsed one more time!!!
{
	const int iWholeSize = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// Get original data length...
	int iOffset2 = iOffset, iOffsetPrev = 0, iSizeThisPacket = 0;
	int i = 0;
	while(iOffset2 < iWholeSize)
	{
		iOffsetPrev = iOffset2;
		iSizeThisPacket = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset2);		// Get original data length...

		if(iSizeThisPacket <= 0 || iSizeThisPacket >= iWholeSize)
		{
			char szErr[256];
			sprintf(szErr, "연속 패킷 받기 오류 - 순서(%d) 크기(%d/%d)", i, iSizeThisPacket, iWholeSize);
			CGameProcedure::ReportDebugStringAndSendToServer(szErr);
			__ASSERT(0, szErr);
			break; // stop!!
		}
		i++;

		this->ProcessPacket(pDataPack, iOffset2);
		iOffset2 = iOffsetPrev + iSizeThisPacket + 2;
	}

	iOffset += iWholeSize;
}

void CGameProcMain::MsgRecv_WareHouse(DataPack* pDataPack, int& iOffset)			// Archive related packets..
{
	BYTE	bResult, bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	switch ( (e_SubPacket_WareHouse)bSubCom )
	{
		case N3_SP_WARE_INN:
			m_pUIInn->SetVisible(true);
			break;

		case N3_SP_WARE_OPEN:
			MsgRecv_WareHouseOpen(pDataPack, iOffset);
			break;

		case N3_SP_WARE_GET_IN:
			bResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// 0x01 : true, 0x00 : false..
			if (m_pUIWareHouseDlg) m_pUIWareHouseDlg->ReceiveResultToWareMsg(bResult);
			break;

		case N3_SP_WARE_GET_OUT:
			bResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// 0x01 : true, 0x00 : false..
			if (m_pUIWareHouseDlg) m_pUIWareHouseDlg->ReceiveResultFromWareMsg(bResult);
			break;

		case N3_SP_WARE_WARE_MOVE:
			bResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// 0x01 : true, 0x00 : false..
			if (m_pUIWareHouseDlg) m_pUIWareHouseDlg->ReceiveResultWareToWareMsg(bResult);
			break;

		case N3_SP_WARE_INV_MOVE:
			bResult = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// 0x01 : true, 0x00 : false..
			if (m_pUIWareHouseDlg) m_pUIWareHouseDlg->ReceiveResultInvToInvMsg(bResult);
			break;
	}
}

void CGameProcMain::MsgRecv_WareHouseOpen(DataPack* pDataPack, int& iOffset)		// Open the locker...
{
	if (m_pUIWareHouseDlg->IsVisible())
		return;

	int iWareGold, iItemID, iItemDurability, iItemCount;
	iWareGold		= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	m_pUIWareHouseDlg->EnterWareHouseStateStart(iWareGold);

	for(auto i = 0; i < MAX_ITEM_WARE_PAGE*MAX_ITEM_TRADE; i++ )				// As for the number of slots.
	{
		iItemID			= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
		iItemDurability	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		iItemCount		= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		m_pUIWareHouseDlg->AddItemInWare( iItemID, iItemDurability, iItemCount, i );
	}

	if (!m_pUIWareHouseDlg->IsVisible())
		m_pUIWareHouseDlg->SetVisible(true);

	if (m_pUIInventory->IsVisible())		// If your inventory isn&#39;t open...
		this->CommandToggleUIInventory();

	if (m_pUISkillTreeDlg->IsVisible())
		m_pUISkillTreeDlg->Close();

	m_pUIWareHouseDlg->EnterWareHouseStateEnd();
}

void CGameProcMain::PlayBGM_Town()
{
	if(m_pSnd_Battle) m_pSnd_Battle->Stop(3.0f);
	if(nullptr == m_pSnd_Town || m_pSnd_Town->IsPlaying()) return;
	m_pSnd_Town->SetMaxVolume(60);
	m_pSnd_Town->Play(nullptr, 3.0f); // Set battle music.. Cancel when there are no monsters around..
}

void CGameProcMain::PlayBGM_Battle()
{
	if(m_pSnd_Town) m_pSnd_Town->Stop(3.0f);
	if(nullptr == m_pSnd_Battle || m_pSnd_Battle->IsPlaying()) return;
	m_pSnd_Battle->SetMaxVolume(80);
	m_pSnd_Battle->Play(nullptr, 3.0f); // Set battle music.. Cancel when there are no monsters around..
}

void CGameProcMain::ReleaseSound()
{
	if(ACT_WORLD && ACT_WORLD->GetSkyRef()) ACT_WORLD->GetSkyRef()->ReleaseSound();
	CN3Base::s_SndMgr.ReleaseStreamObj(&m_pSnd_Town);
	CN3Base::s_SndMgr.ReleaseStreamObj(&m_pSnd_Battle);
}

void CGameProcMain::MsgRecv_NpcChangeOpen(DataPack* pDataPack, int& iOffset)		// Class Change and Initialization..
{
	const BYTE	bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	switch (bSubCom)
	{
		case N3_SP_CLASS_CHANGE_PURE:
			m_pUINpcChange->Open();
			break;

		case N3_SP_CLASS_CHANGE_REQ:
			this->MsgRecv_ClassChange(pDataPack, iOffset);
			break;

		case N3_SP_CLASS_ALL_POINT:
			this->MsgRecv_AllPointInit(pDataPack, iOffset);
			break;

		case N3_SP_CLASS_SKILL_POINT:
			this->MsgRecv_SkillPointInit(pDataPack, iOffset);
			break;

		case N3_SP_CLASS_POINT_CHANGE_PRICE_QUERY:
			this->MsgRecv_PointChangePriceQueryRequest(pDataPack, iOffset);
			break;
	}
}

void CGameProcMain::MsgRecv_AllPointInit(DataPack* pDataPack, int& iOffset)			// All Point Initialization..
{
	const BYTE	bType		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	const DWORD	dwGold		= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);	

	char szBuf[256] = "";
	std::string szMsg; 

	switch (bType)
	{
		case 0x00:	// lack of money...
			::_LoadStringFromResource(IDS_POINTINIT_NOT_ENOUGH_NOAH, szMsg);
			sprintf(szBuf, szMsg.c_str(), dwGold);
			CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffff3b3b);
			break;

		case 0x01:	// success..
			s_pPlayer->m_InfoExt.iStrength = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			m_pUIVar->m_pPageState->UpdateStrength(s_pPlayer->m_InfoExt.iStrength, s_pPlayer->m_InfoExt.iStrength_Delta);

			s_pPlayer->m_InfoExt.iStamina = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			m_pUIVar->m_pPageState->UpdateStamina(s_pPlayer->m_InfoExt.iStamina, s_pPlayer->m_InfoExt.iStamina_Delta);

			s_pPlayer->m_InfoExt.iDexterity = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			m_pUIVar->m_pPageState->UpdateDexterity(s_pPlayer->m_InfoExt.iDexterity, s_pPlayer->m_InfoExt.iDexterity_Delta);

			s_pPlayer->m_InfoExt.iIntelligence = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			m_pUIVar->m_pPageState->UpdateIntelligence(s_pPlayer->m_InfoExt.iIntelligence, s_pPlayer->m_InfoExt.iIntelligence_Delta);

			s_pPlayer->m_InfoExt.iMagicAttak = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			m_pUIVar->m_pPageState->UpdateMagicAttak(s_pPlayer->m_InfoExt.iMagicAttak, s_pPlayer->m_InfoExt.iMagicAttak_Delta);

			s_pPlayer->m_InfoBase.iHPMax =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			s_pPlayer->m_InfoExt.iMSPMax =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			s_pPlayer->m_InfoExt.iAttack =		CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			s_pPlayer->m_InfoExt.iWeightMax =	CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

			m_pUIVar->m_pPageState->UpdateHP(s_pPlayer->m_InfoBase.iHP, s_pPlayer->m_InfoBase.iHPMax);
			m_pUIStateBarAndMiniMap->UpdateHP(s_pPlayer->m_InfoBase.iHP, s_pPlayer->m_InfoBase.iHPMax, false);
			
			m_pUIVar->m_pPageState->UpdateMSP(s_pPlayer->m_InfoExt.iMSP, s_pPlayer->m_InfoExt.iMSPMax);
			m_pUIStateBarAndMiniMap->UpdateMSP(s_pPlayer->m_InfoExt.iMSP, s_pPlayer->m_InfoExt.iMSPMax, false);

			m_pUIVar->m_pPageState->UpdateAttackPoint(s_pPlayer->m_InfoExt.iAttack, s_pPlayer->m_InfoExt.iAttack_Delta);
			m_pUIVar->m_pPageState->UpdateWeight(s_pPlayer->m_InfoExt.iWeight, s_pPlayer->m_InfoExt.iWeightMax);

			s_pPlayer->m_InfoExt.iBonusPointRemain = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Remaining bonus points...
			m_pUIVar->m_pPageState->UpdateBonusPointAndButtons(s_pPlayer->m_InfoExt.iBonusPointRemain); // Are bonus points applicable?

			// Money changes.. Inventory changes, commerce.. Exclusive to personal transactions..
			s_pPlayer->m_InfoExt.iGold = dwGold;
			if (m_pUIInventory->IsVisible())
				m_pUIInventory->GoldUpdate();
			if (m_pUITransactionDlg->IsVisible())
				m_pUITransactionDlg->GoldUpdate();
			if (m_pSubProcPerTrade && m_pSubProcPerTrade->m_pUIPerTradeDlg->IsVisible())
				m_pSubProcPerTrade->m_pUIPerTradeDlg->GoldUpdate();

			break;

		case 0x02:	// Already..
			::_LoadStringFromResource(IDS_POINTINIT_ALREADY, szMsg);
			sprintf(szBuf, szMsg.c_str());
			CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffff3b3b);
			break;
	}
}

void CGameProcMain::MsgRecv_SkillPointInit(DataPack* pDataPack, int& iOffset)		// Reset Skill Points..
{
	const BYTE	bType		= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	const DWORD	dwGold		= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);	
	int i;
	char szBuf[256] = "";
	std::string szMsg; 

	switch (bType)
	{
		case 0x00:	// lack of money...
			::_LoadStringFromResource(IDS_POINTINIT_NOT_ENOUGH_NOAH, szMsg);
			sprintf(szBuf, szMsg.c_str(), dwGold);
			CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffff3b3b);
			break;

		case 0x01:	// success..
			m_pUISkillTreeDlg->m_iSkillInfo[0] = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			for(auto i = 1; i < 9; i++ )
				m_pUISkillTreeDlg->m_iSkillInfo[i] = 0;
			m_pUISkillTreeDlg->InitIconUpdate();

			// Money changes.. Inventory changes, commerce.. Exclusive to personal transactions..
			s_pPlayer->m_InfoExt.iGold = dwGold;
			if (m_pUIInventory->IsVisible())
				m_pUIInventory->GoldUpdate();
			if (m_pUITransactionDlg->IsVisible())
				m_pUITransactionDlg->GoldUpdate();
			if (m_pSubProcPerTrade && m_pSubProcPerTrade->m_pUIPerTradeDlg->IsVisible())
				m_pSubProcPerTrade->m_pUIPerTradeDlg->GoldUpdate();

			if (m_pUIHotKeyDlg)
				m_pUIHotKeyDlg->AllFactorClear();
			break;

		case 0x02:	// Already..
			::_LoadStringFromResource(IDS_POINTINIT_ALREADY, szMsg);
			sprintf(szBuf, szMsg.c_str());
			CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffff3b3b);
			break;
	}
}

void CGameProcMain::MsgRecv_PointChangePriceQueryRequest(DataPack* pDataPack, int& iOffset)		// Response packet for price..
{
	const DWORD	dwGold		= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);	
	m_pUINpcChange->ReceivePriceFromServer(dwGold);
}

void CGameProcMain::MsgRecv_NoahChange(DataPack* pDataPack, int& iOffset)		// Noah change..
{
	const BYTE	bType				= CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	const DWORD	dwGoldOffset	= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	const DWORD	dwGold			= CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);	

	char szBuf[256] = "";
	std::string szMsg; 

	switch (bType)
	{
		case N3_SP_NOAH_GET:
			::_LoadStringFromResource(IDS_NOAH_CHANGE_GET, szMsg);
			sprintf(szBuf, szMsg.c_str(), dwGoldOffset);
			CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xff6565ff);
			break;

		case N3_SP_NOAH_LOST:
			::_LoadStringFromResource(IDS_NOAH_CHANGE_LOST, szMsg);
			sprintf(szBuf, szMsg.c_str(), dwGoldOffset);
			CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffff3b3b);
			break;

		case N3_SP_NOAH_SPEND:
			::_LoadStringFromResource(IDS_NOAH_CHANGE_LOST, szMsg);
			sprintf(szBuf, szMsg.c_str(), dwGoldOffset);
			CGameProcedure::s_pProcMain->MsgOutput(szBuf, 0xffff3b3b);
			break;
	}

	// Money changes.. Inventory changes, commerce.. Exclusive to personal transactions..
	s_pPlayer->m_InfoExt.iGold = dwGold;
	if (m_pUIInventory->IsVisible())
		m_pUIInventory->GoldUpdate();
	if (m_pUITransactionDlg->IsVisible())
		m_pUITransactionDlg->GoldUpdate();
	if (m_pSubProcPerTrade && m_pSubProcPerTrade->m_pUIPerTradeDlg->IsVisible())
		m_pSubProcPerTrade->m_pUIPerTradeDlg->GoldUpdate();
}

void CGameProcMain::MsgRecv_WarpList(DataPack* pDataPack, int& iOffset)		// Warp list - could be a zone change..
{
	m_pUIWarp->Reset();

	int iStrLen = 0;

	const int iListCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	for(int i = 0; i < iListCount; i++)
	{
		__WarpInfo WI;
		
		WI.iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Warp ID
		iStrLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // name length
		CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, WI.szName, iStrLen); // name
		iStrLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // consent length
		CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, WI.szAgreement, iStrLen); // agreement
		WI.iZone = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);				// zone number
		WI.iMaxUser = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);			// Max user count.
		WI.iGold = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);				// money
		WI.vPos.x = (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;	// coordinate
		WI.vPos.z = (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;	//
		WI.vPos.y = (CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset))/10.0f;	// 

		m_pUIWarp->InfoAdd(WI);
	}

	m_pUIWarp->UpdateList();
	m_pUIWarp->SetVisible(true);
}

/*
void CGameProcMain::MsgRecv_ServerCheckAndRequestConcurrentUserCount(DataPack* pDataPack, int&amp; iOffset) // Receive the server IP and port and check concurrent users.
{
	std::string szIP;
	int iStrLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // IP..
	CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szIP, iStrLen);
	DWORD dwPort = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // Port

	__WarpInfo WI;
	if(m_pUIWarp->InfoGetCur(WI) < 0) return;

bool bNeedConnectSubSocket = (szIP != s_pSocket-&gt;GetCurrentIP() || dwPort != s_pSocket-&gt;GetCurrentPort()); // Check if the IP and port to connect are the same

if(bNeedConnectSubSocket) // If you need to connect to a subsocket...
	{
int iErr = s_pSocketSub-&gt;Connect(s_hWndSubSocket, szIP.c_str(), dwPort); // Connect through the subsocket...
		if(iErr)
		{
			this->ReportServerConnectionFailed(WI.szName, iErr, false);
			return;
		}
	}

// check neighbors..
	int iOffsetSend = 0;
	BYTE byBuff[8];
	
	CAPISocket::MP_AddByte(byBuff, iOffsetSend, N3_SERVER_CONCURRENT_CONNECT);
	CAPISocket::MP_AddShort(byBuff, iOffsetSend, WI.iZone);
CAPISocket::MP_AddByte(byBuff, iOffsetSend, s_pPlayer-&gt;m_InfoBase.eNation); // Concurrent reception by country..

if(bNeedConnectSubSocket) s_pSocketSub-&gt;Send(byBuff, iOffsetSend); // Send to subsocket.
else s_pSocket-&gt;Send(byBuff, iOffsetSend); // Send to this socket..
}


void CGameProcMain::MsgRecv_ConcurrentUserCountAndSendServerCheck(DataPack* pDataPack, int&amp; iOffset) // Sends a packet to receive concurrent users and connect to the server.
{
	int iConcurrentUser = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // IP..
	if(s_pSocketSub->IsConnected()) s_pSocketSub->Disconnect();

	__WarpInfo WI;
	if(m_pUIWarp->InfoGetCur(WI) < 0) return;

if (iConcurrentUser &lt; WI.iMaxUser) // If less than the concurrency limit...
	{
		int iOffsetSend = 0;
		BYTE byBuff[8];
		
		CAPISocket::MP_AddByte(byBuff, iOffsetSend, N3_SERVER_CHECK);
		CAPISocket::MP_AddShort(byBuff, iOffsetSend, WI.iID);

		s_pSocket->Send(byBuff, iOffsetSend);
	}
	else
	{
std::string szMsg; ::_LoadStringFromResource(IDS_MSG_CONCURRENT_USER_OVERFLOW, szMsg); // Concurrent connection limit exceeded..
		this->MsgOutput(szMsg, 0xffff0000);
	}
}
*/

void CGameProcMain::MsgRecv_Knights_Create(DataPack* pDataPack, int& iOffset)
{
	BYTE	bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	switch ( (e_SubPacket_KNights_Create)bSubCom )
	{
		case N3_SP_KNIGHTS_CREATE_SUCCESS:
			{
				const int sid = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
				std::string szID;
				const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// Templar ID
				const int iLen = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);		// ID string length..
				CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szID, iLen);	// ID string..
				const int iGrade = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// Rating
				const int iRank = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// ranking
				const DWORD dwGold = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);

				if(s_pPlayer->IDNumber()==sid)
				{
					m_pUIInn->Message(IDS_CLAN_MAKE_SUCCESS);

					// Money changes.. Inventory changes, commerce.. Exclusive to personal transactions..
					s_pPlayer->m_InfoExt.iGold = dwGold;
					if (m_pUIInventory->IsVisible()) m_pUIInventory->GoldUpdate();
					if (m_pUITransactionDlg->IsVisible()) m_pUITransactionDlg->GoldUpdate();
					if (m_pSubProcPerTrade && m_pSubProcPerTrade->m_pUIPerTradeDlg->IsVisible())
						m_pSubProcPerTrade->m_pUIPerTradeDlg->GoldUpdate();

					// Knights (clan) UI update...do it...
					s_pPlayer->m_InfoExt.eKnightsDuty = KNIGHTS_DUTY_CHIEF;
					s_pPlayer->KnightsInfoSet(iID, szID, iGrade, iRank);
					m_pUIVar->UpdateKnightsInfo();

					if(m_pUIVar->m_pPageKnights->IsVisible())
					{
						m_pUIVar->m_pPageKnights->MsgSend_MemberInfoAll();
						m_pUIVar->m_pPageKnights->ChangeUIByDuty(s_pPlayer->m_InfoExt.eKnightsDuty);
					}

					// m_pUIKnightsOp-&gt;KnightsInfoInsert(iID, szID); // Add guild information..
				}
				else
				{
					CPlayerOther* pUPC = s_pOPMgr->UPCGetByID(sid, true);
					if(pUPC)
						pUPC->KnightsInfoSet(iID, szID, iGrade, iRank);
				}
			}
			break;
		case N3_SP_KNIGHTS_CREATE_FAIL_LOWLEVEL:
			m_pUIInn->Message(IDS_CLAN_DENY_LOWLEVEL);
			break;
		case N3_SP_KNIGHTS_CREATE_FAIL_DUPLICATEDNAME:
			m_pUICreateClanName->Open(IDS_CLAN_REINPUT_NAME);
			break;

		case N3_SP_KNIGHTS_CREATE_FAIL_LOWMONEY:
			m_pUIInn->Message(IDS_CLAN_DENY_LOWGOLD);
			break;
		case N3_SP_KNIGHTS_CREATE_FAIL_ALREADYJOINED:
			m_pUIInn->Message(IDS_CLAN_DENY_ALREADYJOINED);
			break;
		case N3_SP_KNIGHTS_CREATE_FAIL_INVALIDDAY:
			m_pUIInn->Message(IDS_CLAN_DENY_INVALIDDAY);
			break;
		case N3_SP_KNIGHTS_CREATE_FAIL_DBFAIL:
		case N3_SP_KNIGHTS_CREATE_FAIL_UNKNOWN:
			m_pUIInn->Message(IDS_CLAN_DENY_UNKNOWN);
			break;
		case N3_SP_KNIGHTS_CREATE_FAIL_INVALIDSERVER:
			m_pUIInn->Message(IDS_CLAN_DENY_INVALID_SERVER);
			break;
	}
}

void CGameProcMain::MsgRecv_Knights_Withdraw(DataPack* pDataPack, int& iOffset)
{
	BYTE	bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	std::string szMsg;
	switch ( (e_SubPacket_KNights_Common)bSubCom )
	{
	case N3_SP_KNIGHTS_COMMON_DBFAIL:
		break;
	case N3_SP_KNIGHTS_COMMON_SUCCESS:
		{
			const int sid = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			if(s_pPlayer->IDNumber()==sid)
			{
				s_pPlayer->m_InfoExt.iKnightsID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
				s_pPlayer->m_InfoExt.eKnightsDuty = (e_KnightsDuty)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
				m_pUIVar->UpdateKnightsInfo();

				s_pPlayer->KnightsInfoSet(s_pPlayer->m_InfoExt.iKnightsID, "", 0, 0);
				::_LoadStringFromResource(IDS_CLAN_WITHDRAW_SUCCESS, szMsg);
				this->MsgOutput(szMsg, 0xffffff00);

				if(m_pUIVar->m_pPageKnights->IsVisible())
				{
					m_pUIVar->m_pPageKnights->MsgSend_MemberInfoAll();
					m_pUIVar->m_pPageKnights->ChangeUIByDuty(s_pPlayer->m_InfoExt.eKnightsDuty);
				}
			}
			else
			{
				const int iKnightsID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
				auto eKnightsDuty = (e_KnightsDuty)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

				CPlayerOther* pUPC = s_pOPMgr->UPCGetByID(sid, true);
				if(pUPC)
				{
					// __KnightsInfoBase* pKIB = m_pUIKnightsOp->KnightsInfoFind(iKnightsID);
					// if(pKIB) pUPC->KnightsNameSet(pKIB->szName, 0xffff0000);
					// else pUPC->KnightsNameSet("", 0xffff0000);
					pUPC->KnightsInfoSet(iKnightsID, "", 0, 0);
				}
			}
		}
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_NONE_USER:
	case N3_SP_KNIGHTS_COMMON_FAIL_DEAD_USER:
	case N3_SP_KNIGHTS_COMMON_FAIL_ENEMY_USER:
	case N3_SP_KNIGHTS_COMMON_FAIL_OTHER_CLAN_USER:
	case N3_SP_KNIGHTS_COMMON_FAIL_INVALIDRIGHT:
	case N3_SP_KNIGHTS_COMMON_FAIL_NONE_CLAN:
	case N3_SP_KNIGHTS_COMMON_FAIL_FULL:
	case N3_SP_KNIGHTS_COMMON_FAIL_ME:
	case N3_SP_KNIGHTS_COMMON_FAIL_NOT_JOINED:
		::_LoadStringFromResource(IDS_CLAN_WITHDRAW_FAIL, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_BATTLEZONE:
		::_LoadStringFromResource(IDS_CLAN_COMMON_FAIL_BATTLEZONE, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	}
}

void CGameProcMain::MsgRecv_Knights_Join(DataPack* pDataPack, int& iOffset)
{
	BYTE	bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	std::string szMsg;
	switch ( (e_SubPacket_KNights_Common)bSubCom )
	{
	case N3_SP_KNIGHTS_COMMON_DBFAIL: // DB search failed..
		break;
	case N3_SP_KNIGHTS_COMMON_SUCCESS: // Clan registration successful
		{
			const int sid = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			const auto eDuty = (e_KnightsDuty)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			const int iL = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // The length of the name of the Knights Templar.
			std::string szKnightsName;
			CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szKnightsName, iL);
			const int iGrade = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// Rating
			const int iRank = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// ranking

			if(s_pPlayer->IDNumber()==sid)
			{
				s_pPlayer->m_InfoExt.eKnightsDuty = eDuty;
				s_pPlayer->KnightsInfoSet(iID, szKnightsName, iGrade, iRank);
				m_pUIVar->UpdateKnightsInfo();

				::_LoadStringFromResource(IDS_CLAN_JOIN_SUCCESS, szMsg);
				this->MsgOutput(szMsg, 0xffffff00);

				if(m_pUIVar->m_pPageKnights->IsVisible())
				{
					m_pUIVar->m_pPageKnights->MsgSend_MemberInfoAll();
					m_pUIVar->m_pPageKnights->ChangeUIByDuty(s_pPlayer->m_InfoExt.eKnightsDuty);
				}
			}
			else
			{
				CPlayerOther* pUPC = s_pOPMgr->UPCGetByID(sid, true);								
				if(pUPC)
					pUPC->KnightsInfoSet(iID, szKnightsName, iGrade, iRank);
			}
		}
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_NONE_USER:	// no user...
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_NONE_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_DEAD_USER:	// Opponent is dead...
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_DEAD_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_ENEMY_USER: // The country of the other user is different..
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_ENEMY_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_OTHER_CLAN_USER: // The opposing user is already a member of another clan or guild.
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_OTHER_CLAN_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_INVALIDRIGHT: // No permission..
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_INVALIDRIGHT, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_NONE_CLAN:	// Knights that don&#39;t exist...
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_NONE_CLAN, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_FULL:	// Full staff..
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_CLAN_FULL, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_ME:
		::_LoadStringFromResource(IDS_CLAN_COMMON_FAIL_ME, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_NOT_JOINED:
		::_LoadStringFromResource(IDS_CLAN_COMMON_FAIL_NOTJOINED, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_REJECT:
		::_LoadStringFromResource(IDS_CLAN_JOIN_REJECT, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_BATTLEZONE:
		::_LoadStringFromResource(IDS_CLAN_COMMON_FAIL_BATTLEZONE, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	}	
}

void CGameProcMain::MsgRecv_Knights_Leave(DataPack* pDataPack, int& iOffset)
{
	BYTE	bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	std::string szMsg;
	switch ( (e_SubPacket_KNights_Common)bSubCom )
	{
	case N3_SP_KNIGHTS_COMMON_DBFAIL: // DB search failed..
		break;
	case N3_SP_KNIGHTS_COMMON_SUCCESS: // Clan withdrawal success
		{
			const int sid = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			const auto eDuty = (e_KnightsDuty)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
			const int iL = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // The length of the name of the Knights Templar.
			std::string szKnightsName;
			CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szKnightsName, iL);
			const int iGrade = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);	// Rating
			const int iRank = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);		// ranking

			if(s_pPlayer->IDNumber()==sid)
			{
				s_pPlayer->m_InfoExt.eKnightsDuty = eDuty;
				s_pPlayer->KnightsInfoSet(iID, szKnightsName, iGrade, iRank);
				m_pUIVar->UpdateKnightsInfo();
				
				::_LoadStringFromResource(IDS_CLAN_JOIN_SUCCESS, szMsg);
				this->MsgOutput(szMsg, 0xffffff00);

				if(m_pUIVar->m_pPageKnights->IsVisible())
				{
					m_pUIVar->m_pPageKnights->MsgSend_MemberInfoAll();
					m_pUIVar->m_pPageKnights->ChangeUIByDuty(s_pPlayer->m_InfoExt.eKnightsDuty);
				}
			}
			else
			{
				CPlayerOther* pUPC = s_pOPMgr->UPCGetByID(sid, true);								
				if(pUPC)
					pUPC->KnightsInfoSet(iID, "", 0, 0);
			}
		}
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_NONE_USER:	// no user...
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_NONE_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_DEAD_USER:	// Opponent is dead...
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_DEAD_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_ENEMY_USER: // The country of the other user is different..
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_ENEMY_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_OTHER_CLAN_USER: // The opposing user is already a member of another clan or guild.
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_OTHER_CLAN_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_INVALIDRIGHT: // no permission..
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_INVALIDRIGHT, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_NONE_CLAN:	// Knights that don&#39;t exist...
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_NONE_CLAN, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_FULL:	// Full staff..
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_CLAN_FULL, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_ME:
		::_LoadStringFromResource(IDS_CLAN_COMMON_FAIL_ME, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_NOT_JOINED:
		::_LoadStringFromResource(IDS_CLAN_COMMON_FAIL_NOTJOINED, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_BATTLEZONE:
		::_LoadStringFromResource(IDS_CLAN_COMMON_FAIL_BATTLEZONE, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	}	
}

void CGameProcMain::MsgRecv_Knights_AppointViceChief(DataPack* pDataPack, int& iOffset)
{
	BYTE	bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	std::string szMsg;
	switch ( (e_SubPacket_KNights_Common)bSubCom )
	{
	case N3_SP_KNIGHTS_COMMON_DBFAIL: // DB search failed..
		break;
	case N3_SP_KNIGHTS_COMMON_SUCCESS: // Clan registration successful
		{
			const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			const auto eDuty = (e_KnightsDuty)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

			s_pPlayer->m_InfoExt.iKnightsID = iID;
			s_pPlayer->m_InfoExt.eKnightsDuty = eDuty;
			m_pUIVar->UpdateKnightsInfo();

			::_LoadStringFromResource(IDS_CLAN_JOIN_SUCCESS, szMsg);
			this->MsgOutput(szMsg, 0xffffff00);

			if(m_pUIVar->m_pPageKnights->IsVisible())
			{
				m_pUIVar->m_pPageKnights->MsgSend_MemberInfoAll();
				m_pUIVar->m_pPageKnights->ChangeUIByDuty(s_pPlayer->m_InfoExt.eKnightsDuty);
			}
		}
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_NONE_USER:	// no user...
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_NONE_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_DEAD_USER:	// Opponent is dead...
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_DEAD_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_ENEMY_USER: // The country of the other user is different..
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_ENEMY_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_OTHER_CLAN_USER: // The opposing user is already a member of another clan or guild.
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_OTHER_CLAN_USER, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_INVALIDRIGHT: // No permission..
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_INVALIDRIGHT, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_NONE_CLAN:	// Knights that don&#39;t exist...
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_NONE_CLAN, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_FULL:	// Full staff..
		::_LoadStringFromResource(IDS_CLAN_JOIN_FAIL_CLAN_FULL, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_ME:
		::_LoadStringFromResource(IDS_CLAN_COMMON_FAIL_ME, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_NOT_JOINED:
		::_LoadStringFromResource(IDS_CLAN_COMMON_FAIL_NOTJOINED, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	case N3_SP_KNIGHTS_COMMON_FAIL_BATTLEZONE:
		::_LoadStringFromResource(IDS_CLAN_COMMON_FAIL_BATTLEZONE, szMsg);
		this->MsgOutput(szMsg, 0xffffff00);
		break;
	}	
}

void CGameProcMain::MsgRecv_Knights_MemberInfoAll(DataPack* pDataPack, int& iOffset)
{
	BYTE	bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	std::string szMsg;
	switch ( (e_SubPacket_KNights_Common)bSubCom )
	{
	case N3_SP_KNIGHTS_COMMON_SUCCESS:
		m_pUIVar->m_pPageKnights->MsgRecv_MemberInfo(pDataPack, iOffset);
		break;
	case N3_SP_KNIGHTS_COMMON_DBFAIL: // DB search failed..
	default:
		break;		
	}
}

void CGameProcMain::MsgRecv_Knights_GradeChangeAll(DataPack* pDataPack, int& iOffset)
{
	const int iCount = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	if(iCount <= 0) return;
	
	std::vector<int> iIDs(iCount, 0);
	std::vector<int> iGrades(iCount, 0);
	std::vector<int> iRanks(iCount, 0);

	for(int i = 0; i < iCount; i++)
	{
		iIDs[i] = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
		iGrades[i] = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
		iRanks[i] = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	}

	auto it = s_pOPMgr->m_UPCs.begin(), itEnd = s_pOPMgr->m_UPCs.end();
	for(; it != itEnd; it++)
	{
		CPlayerOther* pUPC = it->second;
		if(nullptr == pUPC) continue;

		const int iIDTmp = pUPC->m_InfoExt.iKnightsID;
		if(iIDTmp <= 0) continue;

		for(auto i = 0; i < iCount; i++)
		{
			if(iIDs[i] == iIDTmp)
			{
				pUPC->KnightsInfoSet(iIDTmp, pUPC->m_InfoExt.szKnights, iGrades[i], iRanks[i]);
				break;
			}
		}
	}

}

void CGameProcMain::MsgRecv_Knights_Duty_Change(DataPack* pDataPack, int& iOffset)
{
	BYTE bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	switch ( (e_SubPacket_KNights_Common)bSubCom )
	{
	case N3_SP_KNIGHTS_COMMON_SUCCESS:
		{
			const int sid = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			const int iID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			const auto eDuty = (e_KnightsDuty)CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

			if(s_pPlayer->IDNumber()==sid)
			{
				s_pPlayer->m_InfoExt.iKnightsID = iID;
				s_pPlayer->m_InfoExt.eKnightsDuty = eDuty;
				m_pUIVar->UpdateKnightsInfo();
				if(s_pPlayer->m_InfoExt.iKnightsID == 0)
					s_pPlayer->KnightsInfoSet(0, "", 0, 0);
				// std::string szName;
				// __KnightsInfoBase* pKIB = m_pUIKnightsOp->KnightsInfoFind(s_pPlayer->m_InfoExt.iKnightsID);
				// if(pKIB) m_pUIVar->m_pPageKnights->UpdateKnightsName(pKIB->szName);
				// else m_pUIVar->m_pPageKnights->UpdateKnightsName("");
			}
			else
			{
				CPlayerOther* pUPC = s_pOPMgr->UPCGetByID(sid, true);								
				if(pUPC)
					if(iID == 0) pUPC->KnightsInfoSet(0, "", 0, 0);
			}
		}
		break;
	}
}

void CGameProcMain::MsgRecv_Knigts_Join_Req(DataPack* pDataPack, int& iOffset)
{
	BYTE bSubCom = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);

	switch ( (e_SubPacket_KNights_Common)bSubCom )
	{
	case N3_SP_KNIGHTS_COMMON_SUCCESS:
		{
			m_iJoinReqClanRequierID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
			m_iJoinReqClan = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

			const int iL = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset); // The length of the name of the Knights Templar.
			std::string szKnightsName;
			CAPISocket::Parse_GetString(pDataPack->m_pData, iOffset, szKnightsName, iL);

			// std::string szName;
			// __KnightsInfoBase* pKIB = m_pUIKnightsOp->KnightsInfoFind(m_iJoinReqClan);
			// if(!pKIB)
			// {
				// MsgSend_KnightsJoinReq(false);
				// break;
			// }

			std::string szMsg;
			::_LoadStringFromResource(IDS_CLAN_JOIN_REQ, szMsg);

			char szTmp[256];
			sprintf(szTmp, szMsg.c_str(), szKnightsName.c_str());
			
			CGameProcedure::s_pProcMain->MessageBoxPost(szTmp, "", MB_YESNO, BEHAVIOR_CLAN_JOIN);			
		}
		break;
	}
}

int CGameProcMain::MsgRecv_VersionCheck(DataPack* pDataPack, int& iOffset) // virtual
{
	const int iVersion = CGameProcedure::MsgRecv_VersionCheck(pDataPack, iOffset);
	this->MsgSend_CharacterSelect(); // virtual

	return iVersion;
}

bool CGameProcMain::MsgRecv_CharacterSelect(DataPack* pDataPack, int& iOffset) // virtual
{
	const bool bSuccess = CGameProcedure::MsgRecv_CharacterSelect(pDataPack, iOffset);

	// If you die in the war zone and change servers, the value is set again.
	if(s_pPlayer->IsDead())
	{
		this->InitPlayerPosition(s_pPlayer->Position()); // Initialize the player position.. Raise him up and make him take the basic action.
		s_pPlayer->RegenerateCollisionMesh(); // Recreate the collision mesh.
		s_pPlayer->m_iSendRegeneration = 0; // Once sent, a flag that will not be seen until it dies again
		s_pPlayer->m_fTimeAfterDeath = 0; // Once sent, a flag that will not be seen until it dies again

		//
		// Magic &amp; Effect Reset..
		if(m_pUIStateBarAndMiniMap) m_pUIStateBarAndMiniMap->ClearMagic();
		if(m_pMagicSkillMng) m_pMagicSkillMng->ClearDurationalMagic();
		if(CGameProcedure::s_pFX) s_pFX->StopMine();

		CLogWriter::Write("Receive Regeneration");

		if(s_pPlayer->Nation()==NATION_KARUS) CGameProcedure::s_pFX->TriggerBundle(s_pPlayer->IDNumber(), -1, FXID_REGEN_KARUS, s_pPlayer->IDNumber(), -1);
		else if(s_pPlayer->Nation()==NATION_ELMORAD) CGameProcedure::s_pFX->TriggerBundle(s_pPlayer->IDNumber(), -1, FXID_REGEN_ELMORAD, s_pPlayer->IDNumber(), -1);
	}
			

	this->InitZone(s_pPlayer->m_InfoExt.iZoneCur, s_pPlayer->Position()); // change john.
	s_pProcMain->MsgSend_GameStart(); // After loading the zone, the game starts.

	return bSuccess;
}

void CGameProcMain::MsgRecv_Corpse(DataPack *pDataPack, int &iOffset)
{
	const int iID	= CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);// ID of the user to regenerate

	if( s_pPlayer->IDNumber() != iID )
	{
		s_pOPMgr->CorpseAdd( iID );

		if( s_pPlayer->m_iIDTarget == iID )
			s_pPlayer->m_iIDTarget = -1; // If I have focus on the character I regen, it initializes it.
	}
}

void CGameProcMain::MsgSend_PerTradeBBSReq(std::string szName, int iDestID)
{
	if(	!m_pUITransactionDlg->IsVisible() ) // Item transaction between individuals.. // If not in commerce..
	{
		std::string szMsg; ::_LoadStringFromResource(IDS_PERSONAL_TRADE_REQUEST, szMsg);
		MsgOutput(szName + szMsg, 0xffffff00);

		MsgSend_PerTradeReq(iDestID, false);

		if (m_pUINpcEvent->IsVisible())
			m_pUINpcEvent->Close();
		m_pSubProcPerTrade->EnterWaitMsgFromServerStatePerTradeReq(szName);
	}
}

void CGameProcMain::MsgSend_CharacterSelect() // virtual
{
	CGameProcedure::MsgSend_CharacterSelect();
	
	m_pMagicSkillMng->ClearDurationalMagic();
	s_pFX->ClearAll();
}

void CGameProcMain::ControlViewVDegree(short sValue)
{
	if(s_pEng->ViewPoint() == VP_THIRD_PERSON)
	{
		float fPitch = 0;
		if(sValue > 0)	fPitch = D3DXToRadian(45.0f)*3;
		else			fPitch = D3DXToRadian(-45.0f)*3;
		if(fPitch) s_pEng->CameraPitchAdd(fPitch);
	}
}
/*
bool CGameProcMain::OnMouseMove(POINT ptCur, POINT ptPrev)
{
	if(s_pUIMgr->m_bDoneSomething) return false;

	if(ptCur.x != ptPrev.x || ptCur.y != ptPrev.y)
	{
		if(m_pMagicSkillMng->m_dwRegionMagicState==1)
		{
			__Vector3 vPick;
			CPlayerNPC*	 pTarget = NULL;
		
			int iID = -1;
pTarget = s_pOPMgr-&gt;Pick(ptCur.x, ptCur.y, iID, &amp;vPick); // Picking people everywhere..
this-&gt;TargetSelect(iID, false); // catch the target...
if (NULL == pTarget) // If there is no target...
			{
s_pPlayer-&gt;m_pObjectTarget = ACT_WORLD-&gt;PickWithShape(ptCur.x, ptCur.y, true, &amp;vPick); // Save the location where it was taken.
if (NULL == s_pPlayer-&gt;m_pObjectTarget) // If there is no target...
				{
ACT_WORLD-&gt;PickWideWithTerrain(ptCur.x, ptCur.y, vPick); // Take a picture of the terrain.
				}
			}
			s_pFX->SetBundlePos(m_pMagicSkillMng->m_iMyRegionTargetFXID, m_pMagicSkillMng->m_iMyRegionTargetFXID, vPick);
		}
	}

	return true;
}
*/
bool CGameProcMain::OnMouseMove(POINT ptCur, POINT ptPrev)
{
	if(s_pUIMgr->m_bDoneSomething) return false;

	if(ptCur.x != ptPrev.x || ptCur.y != ptPrev.y)
	{
		if(m_pMagicSkillMng->m_dwRegionMagicState==1)
		{
			__Vector3 vNormal, vMyPos, vGap, vDir;

			vMyPos = s_pPlayer->Position();
			vMyPos.y += s_pPlayer->Height() / 2;

			ACT_WORLD->PickWideWithTerrain(ptCur.x, ptCur.y, m_vMouseSkillPos); // Take a picture of the terrain.

			vDir = m_vMouseSkillPos - vMyPos;
			vGap = vDir;
			vDir.Normalize();

			const bool bColShape = ACT_WORLD->CheckCollisionWithShape(vMyPos, vDir, vGap.Magnitude(), &m_vMouseSkillPos, &vNormal);

			if(!bColShape) // If there is no target...
			{
				int iID = -1;
				s_pOPMgr->Pick(ptCur.x, ptCur.y, iID, &m_vMouseSkillPos); // Picking people everywhere..
				this->TargetSelect(iID, false); // catch the target...
			}
			else
			{
				m_vMouseSkillPos.y = ACT_WORLD->GetHeightWithTerrain(m_vMouseSkillPos.x, m_vMouseSkillPos.z);
			}
			s_pFX->SetBundlePos(m_pMagicSkillMng->m_iMyRegionTargetFXID, m_pMagicSkillMng->m_iMyRegionTargetFXID, m_vMouseSkillPos);
		}
	}

	return true;
}

// left double click
bool CGameProcMain::OnMouseLDBtnPress(POINT ptCur, POINT ptPrev)
{
	if(s_pUIMgr->m_bDoneSomething) return false;

	CPlayerNPC* pTarget = s_pOPMgr->CharacterGetByID(s_pPlayer->m_iIDTarget, true);

	if(pTarget && pTarget->m_InfoBase.iAuthority == AUTHORITY_MANAGER)
	{
		s_pPlayer->m_iIDTarget = -1;
		pTarget = nullptr;
	}

	if(VP_THIRD_PERSON == s_pEng->ViewPoint())
	{
		if(s_pPlayer->IsAttackableTarget(pTarget, false))
		{
			this->CommandMove(MD_STOP, true);
			this->CommandEnableAttackContinous(true, pTarget); // auto attack
		}
		else if(pTarget && VP_THIRD_PERSON == s_pEng->ViewPoint())
		{
			this->CommandMove(MD_FOWARD, true);
			s_pPlayer->SetMoveTargetID(s_pPlayer->m_iIDTarget);
		}
	}
	else
	{
		s_pPlayer->m_bAttackContinous = false;
		CommandToggleAttackContinous();
	}
	return true;
}

// left click
bool CGameProcMain::OnMouseLBtnPress(POINT ptCur, POINT ptPrev)
{
	if(s_pUIMgr->m_bDoneSomething) return false;

	const POINT ptPlayer = ::_Convert3D_To_2DCoordinate(	s_pPlayer->Position(), 
		                                                    CN3Base::s_CameraData.mtxView,
		                                                    CN3Base::s_CameraData.mtxProjection,
		                                                    CN3Base::s_CameraData.vp.Width,
		                                                    CN3Base::s_CameraData.vp.Height);
	
	__Vector3 vDir((float)(ptCur.x - ptPlayer.x), 0, (float)(ptPlayer.y - ptCur.y));
	__Matrix44 mtxTmp; mtxTmp.RotationY(s_pEng->CameraYaw());
	vDir *= mtxTmp;
	vDir.Normalize();
	const float fYaw = ::_Yaw2D(vDir.x, vDir.z);

	CPlayerNPC*	 pTarget			= nullptr;
	BOOL		 bFindCorpse		= false;

	int iID = -1;
	pTarget = s_pOPMgr->PickPrecisely(ptCur.x, ptCur.y, iID, &m_vMouseLBClickedPos); // Picking people everywhere..
	this->TargetSelect(iID, false); // catch the target...
	if(nullptr == pTarget) // If there is no target...
	{
		if(s_pPlayer->m_bAttackContinous) // If you keep attacking...
			this->CommandEnableAttackContinous(false, nullptr); // Continue to cancel the attack...

		s_pPlayer->m_pObjectTarget = ACT_WORLD->PickWithShape(ptCur.x, ptCur.y, true, &m_vMouseLBClickedPos); // Save the captured location.
		if(nullptr == s_pPlayer->m_pObjectTarget) // If there is no target...
		{
			// Dig through the corpse and open the item box.
			CPlayerNPC* pCorpse = s_pOPMgr->PickCorpse(ptCur.x, ptCur.y, iID); // Picking..
			if(false == this->MsgSend_RequestItemBundleOpen(pCorpse)) // A request to search the body...
			{
				ACT_WORLD->PickWideWithTerrain(ptCur.x, ptCur.y, m_vMouseLBClickedPos); // Take a picture of the terrain.
			}
			else
				bFindCorpse = true;
		}
	}

	if(m_pMagicSkillMng->m_dwRegionMagicState==1)
	{
		// s_pFX->SetBundlePos(m_pMagicSkillMng->m_iMyRegionTargetFXID, m_pMagicSkillMng->m_iMyRegionTargetFXID, m_vMouseLBClickedPos);
		s_pFX->SetBundlePos(m_pMagicSkillMng->m_iMyRegionTargetFXID, m_pMagicSkillMng->m_iMyRegionTargetFXID, m_vMouseSkillPos);
		m_pMagicSkillMng->m_dwRegionMagicState = 2;
	}

	if(!s_pPlayer->IsDead() && VP_THIRD_PERSON == s_pEng->ViewPoint())
	{
		m_fLBClickTime = CN3Base::TimeGet();
		this->CommandSitDown(false, false); // once you get it up...

		if(pTarget)
		{
			if(s_pLocalInput->IsKeyDown(DIK_LCONTROL) || s_pLocalInput->IsKeyDown(DIK_RCONTROL))
			{
				if(s_pPlayer->IsAttackableTarget(pTarget, false))
				{
					if(s_pPlayer->m_bTargetOrPosMove)
						this->CommandMove(MD_STOP, true);

					this->CommandEnableAttackContinous(true, pTarget); // auto attack
				}
				else
				{
					this->CommandMove(MD_FOWARD, true);
					s_pPlayer->SetMoveTargetID(s_pPlayer->m_iIDTarget);
				}
			}
			else
			{
				if(s_pPlayer->m_bTargetOrPosMove && s_pPlayer->m_iMoveTarget != pTarget->IDNumber())
					this->CommandMove(MD_STOP, true);
			}
		}
		else if(!bFindCorpse)
		{
			s_pPlayer->RotateTo(fYaw, true);
			__Vector3 vMovePoint;
			ACT_WORLD->PickWideWithTerrain(ptCur.x, ptCur.y, vMovePoint); // Take a picture of the terrain.

			const float fDist = (vMovePoint - s_pPlayer->Position()).Magnitude();

			if(!s_pPlayer->m_bTargetOrPosMove && fDist > 1.5f)
				this->CommandMove(MD_FOWARD, true);

			if(fDist > 1.5f)
			{
				s_pPlayer->SetMoveTargetPos(vMovePoint);
			}
			else if(s_pPlayer->m_bTargetOrPosMove)
			{
				this->CommandMove(MD_STOP, true);
			}
		}
	}
	else if(!s_pPlayer->IsDead())
	{
		if(pTarget && (s_pLocalInput->IsKeyDown(DIK_LCONTROL) || s_pLocalInput->IsKeyDown(DIK_RCONTROL)))
		{
			if(s_pPlayer->IsAttackableTarget(pTarget, false))
			{
				if(s_pPlayer->m_bTargetOrPosMove)
					this->CommandMove(MD_STOP, true);

				this->CommandEnableAttackContinous(true, pTarget); // auto attack
			}
			else
			{
				s_pPlayer->m_bAttackContinous = false;
				CommandToggleAttackContinous();
			}
		}
	}

	return true;
}

// left click up
bool CGameProcMain::OnMouseLBtnPressd(POINT ptCur, POINT ptPrev)
{
	if(!s_pLocalInput->IsKeyDown(KM_MOVE_FOWARD))
	{
		// if(!s_pPlayer->m_bTargetOrPosMove)
		// this->CommandMove(MD_STOP, true);
	}
	return true;
}

// when left pressed
bool CGameProcMain::OnMouseLbtnDown(POINT ptCur, POINT ptPrev)
{
	if(s_pUIMgr->m_bDoneSomething) return false;

	const POINT ptPlayer = ::_Convert3D_To_2DCoordinate(	s_pPlayer->Position(), 
		                                                    CN3Base::s_CameraData.mtxView,
		                                                    CN3Base::s_CameraData.mtxProjection,
		                                                    CN3Base::s_CameraData.vp.Width,
		                                                    CN3Base::s_CameraData.vp.Height);
	
	__Vector3 vDir((float)(ptCur.x - ptPlayer.x), 0, (float)(ptPlayer.y - ptCur.y));
	__Matrix44 mtxTmp; mtxTmp.RotationY(s_pEng->CameraYaw());
	vDir *= mtxTmp;
	vDir.Normalize();
	const float fYaw = ::_Yaw2D(vDir.x, vDir.z);

	if(!s_pPlayer->IsDead() && VP_THIRD_PERSON == s_pEng->ViewPoint() && !s_pLocalInput->IsKeyDown(KM_MOVE_FOWARD)) // If it&#39;s a third person point of view... if you don&#39;t touch the UI...
	{
		__Vector3 vMovePoint;
		float fDist;
		const float fNowTime = CN3Base::TimeGet();

		if( fNowTime - m_fLBClickTime > 0.1f && s_pPlayer->m_bTargetOrPosMove)
		{
			if(s_pPlayer->m_bAttackContinous)
				CommandToggleAttackContinous();


			this->CommandSitDown(false, false); // once you get it up...
			s_pPlayer->RotateTo(fYaw, true);
			this->CommandMove(MD_FOWARD, false);

			ACT_WORLD->PickWideWithTerrain(ptCur.x, ptCur.y, vMovePoint); // Take a picture of the terrain.
			fDist = (vMovePoint - s_pPlayer->Position()).Magnitude();

			s_pPlayer->SetMoveTargetPos(vMovePoint);
		}
		else if( fNowTime - m_fLBClickTime > 0.1f && !s_pPlayer->m_bTargetOrPosMove)
		{
			if(s_pPlayer->m_bAttackContinous)
				CommandToggleAttackContinous();

			s_pPlayer->RotateTo(fYaw, true);
			ACT_WORLD->PickWideWithTerrain(ptCur.x, ptCur.y, vMovePoint); // Take a picture of the terrain.

			fDist = (vMovePoint - s_pPlayer->Position()).Magnitude();

			if(fDist > 1.5f)
			{
				this->CommandMove(MD_FOWARD, true);
				s_pPlayer->SetMoveTargetPos(vMovePoint);
			}
		}
	}
	return true;
}

// right click
bool CGameProcMain::OnMouseRBtnPress(POINT ptCur, POINT ptPrev)
{
	if(s_pUIMgr->m_bDoneSomething) return false;

	if(s_pLocalInput->IsKeyDown(DIK_LCONTROL) || s_pLocalInput->IsKeyDown(DIK_RCONTROL))
	{
		m_pUIHotKeyDlg->EffectTriggerByMouse();
		return true;
	}

	int iID = -1;
	CPlayerNPC* pNPC = s_pOPMgr->PickNPC(ptCur.x, ptCur.y, iID); // Picking..

	if(nullptr == pNPC)
	{
		CPlayerNPC* pCorpse = s_pOPMgr->PickCorpse(ptCur.x, ptCur.y, iID); // Picking..
		if(false == this->MsgSend_RequestItemBundleOpen(pCorpse)) // Dig through the corpse and open the item box.
		{
			CN3Shape* pShape = ACT_WORLD->PickWithShape(ptCur.x, ptCur.y, true);
			if(	pShape && pShape == s_pPlayer->m_pObjectTarget && pShape->m_iEventID) // If there is an event...
			{
				const float fD = (s_pPlayer->Position() - pShape->Pos()).Magnitude();
				const float fDLimit = (s_pPlayer->Radius() + pShape->Radius()) * 2.0f;
				if(fD > fDLimit) // if the distance is far
				{
					std::string szMsg; 
					if(OBJECT_TYPE_BINDPOINT == pShape->m_iEventType) ::_LoadStringFromResource(IDS_BIND_POINT_REQUEST_FAIL, szMsg);
					else ::_LoadStringFromResource(IDS_ERR_REQUEST_OBJECT_EVENT_SO_FAR, szMsg);
					this->MsgOutput(szMsg, 0xffff8080);
				}
				else
				{
					if(OBJECT_TYPE_BINDPOINT == pShape->m_iEventType)
					{
						std::string szMsg; ::_LoadStringFromResource(IDS_REQUEST_BINDPOINT, szMsg);
						this->MessageBoxPost(szMsg, "", MB_YESNO, BEHAVIOR_REQUEST_BINDPOINT); // Binding point setting message box
					}
					else if(OBJECT_TYPE_WARP_POINT == pShape->m_iEventType)
					{
						this->MsgSend_ObjectEvent(pShape->m_iEventID, pShape->m_iNPC_ID); // Send object event...
					}
				}
			}
		}
	}
	else if(pNPC->IDNumber() == s_pPlayer->m_iIDTarget) // The target character and the picked character are the same...)
	{
		if(	pNPC->m_pShapeExtraRef ) // If it is an object-type NPC... If there is an ID of an NPC to control...
		{
			if(pNPC->m_pShapeExtraRef->m_iNPC_ID > 0)
			{
				const float fD = (s_pPlayer->Position() - pNPC->m_pShapeExtraRef->Pos()).Magnitude();
				const float fDLimit = (s_pPlayer->Radius() + pNPC->m_pShapeExtraRef->Radius()) * 2.0f;
				if(fD > fDLimit) // if the distance is far
				{
					std::string szMsg; ::_LoadStringFromResource(IDS_ERR_REQUEST_OBJECT_EVENT_SO_FAR, szMsg);
					this->MsgOutput(szMsg, 0xffff8080);
				}
				else
				{
					this->MsgSend_ObjectEvent(pNPC->m_pShapeExtraRef->m_iEventID, pNPC->IDNumber());
				}
			}
		}
		else // Usually NPCs...
		{
			if(pNPC->m_InfoBase.eNation == s_pPlayer->m_InfoBase.eNation) // Only in the same country...
			{
				const float fD = (s_pPlayer->Position() - pNPC->Position()).Magnitude();
				const float fDLimit = (s_pPlayer->Radius() + pNPC->Radius()) * 3.0f;
				if(fD > fDLimit) // if the distance is far
				{
					std::string szMsg; ::_LoadStringFromResource(IDS_ERR_REQUEST_NPC_EVENT_SO_FAR, szMsg);
					this->MsgOutput(szMsg, 0xffff8080);
				}
				else
				{
					s_pPlayer->ActionMove(PSM_STOP); // once you stop...
					pNPC->RotateTo(s_pPlayer); // Turn the direction towards the player.
					this->MsgSend_NPCEvent(iID); // event request.
					if (m_pUITransactionDlg) m_pUITransactionDlg->m_iNpcID = pNPC->IDNumber();
				}
			}
		}
	}
	return true;
}

// right click up
bool CGameProcMain::OnMouseRBtnPressd(POINT ptCur, POINT ptPrev)
{
	return true;
}

// when right pressed
bool CGameProcMain::OnMouseRbtnDown(POINT ptCur, POINT ptPrev)
{
	const float fMouseSensivity = 0.02f;

	const float fRotY = D3DXToRadian(180.0f) * ((ptCur.x - ptPrev.x) * fMouseSensivity); // Calculate the amount to rotate...
	const float fRotX = D3DXToRadian(180.0f) * ((ptCur.y - ptPrev.y) * fMouseSensivity);
	if(fRotY && s_pPlayer->IsAlive())
	{
		if(VP_THIRD_PERSON == s_pEng->ViewPoint()) s_pEng->CameraYawAdd(fRotY);
		else if(false == s_pPlayer->m_bStun) s_pPlayer->RotAdd(fRotY); // Only when I&#39;m not fainting...
	}
	if(fRotX)
	{
		if(VP_THIRD_PERSON == s_pEng->ViewPoint()) s_pEng->CameraZoom(-fRotX); // camera zoom
		else s_pEng->CameraPitchAdd(fRotX); // camera angle
	}

	if(fRotY || fRotX)
	{
		SetGameCursor(nullptr);
		::SetCursorPos(ptPrev.x, ptPrev.y);
		s_pLocalInput->MouseSetPos(ptPrev.x, ptPrev.y);
	}
	return true;
}

// right double click
bool CGameProcMain::OnMouseRDBtnPress(POINT ptCur, POINT ptPrev)
{
	if(s_pUIMgr->m_bDoneSomething) return false;

	// If skill magic was used...
	m_pUIHotKeyDlg->EffectTriggerByMouse();

	return true;
}

void CGameProcMain::ProcessUIKeyInput(bool bEnable)
{
	if(m_pUIChatDlg && !m_pUIChatDlg->IsChatMode())
	{
		CGameProcedure::ProcessUIKeyInput();
		if(s_pLocalInput->IsKeyPress(DIK_RETURN) && !s_bKeyPress)
		{
			m_pUIChatDlg->SetFocus();
		}
	}
	else if(m_pUIChatDlg && m_pUIChatDlg->IsChatMode())
	{
		s_bKeyPress = false;
		if(m_pUIChatDlg->GetEnableKillFocus())
		{
			m_pUIChatDlg->SetEnableKillFocus(false);
			m_pUIChatDlg->KillFocus();
		}
	}
}

void CGameProcMain::MsgSend_SpeedCheck(bool bInit)
{
	BYTE	byBuff[10];											// buffer..
	int		iOffset=0;											// Offset...
	const float	fTime = CN3Base::TimeGet();							// client time

	s_pSocket->MP_AddByte(byBuff, iOffset, N3_CHECK_SPEEDHACK);	// Speed hack check packet..
	s_pSocket->MP_AddByte(byBuff, iOffset, bInit);				// Type to be used by the server as standard time. If true, standard time. If false, check type.
	s_pSocket->MP_AddFloat(byBuff, iOffset, fTime);				// client time
	s_pSocket->Send(byBuff, iOffset);							// sent...
}
