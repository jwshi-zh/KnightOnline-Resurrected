#pragma once

#include "GameProcedure.h"
#include <set>

class CGameProcMain : public CGameProcedure  
{
	friend class CGameProcedure;
protected:
	std::set<int> m_SetNPCID;
	std::set<int> m_SetUPCID;

public:
#ifdef _N3_64GRID_
	CServerMesh		*m_pSMesh;									// The mesh class required by the server..
#endif
	class CUIInventory*			m_pUIInventory;			// Inventory
	class CUIVarious*			m_pUIVar;				// Multipurpose UI with character status window, guild management, etc. as pages
	class CUIChat*				m_pUIChatDlg;			// Chat input/output dialog..
	class CUIMessageWnd*		m_pUIMsgDlg;			// Game message output box.
	class CUIStateBar*			m_pUIStateBarAndMiniMap;	// mp,hp,exp, minimap....
	class CUICmd*				m_pUICmd;				// Command button at the bottom left of the window.
	class CUITargetBar*			m_pUITargetBar;			// target status window.
	class CUITransactionDlg*	m_pUITransactionDlg;
	class CUIDroppedItemDlg*	m_pUIDroppedItemDlg;
	class CSubProcPerTrade*		m_pSubProcPerTrade;
	class CUIHelp*				m_pUIHelp;
	class CUIPartyOrForce*		m_pUIPartyOrForce;
	class CUISkillTreeDlg*		m_pUISkillTreeDlg;
	class CUIHotKeyDlg*			m_pUIHotKeyDlg;
	class CUINotice*			m_pUINotice;
	class CUIClassChange*		m_pUIClassChange;
	class CUINPCEvent*			m_pUINpcEvent;
	class CUIItemExchange*		m_pUIItemREDlg;
	class CUIRepairTooltipDlg*	m_pUIRepairTooltip;
	class CUIWareHouseDlg*		m_pUIWareHouseDlg;
	class CUIInn*				m_pUIInn;
	class CUICreateClanName*	m_pUICreateClanName;
	


	class CUIKnightsOperation*	m_pUIKnightsOp;					// View the list of guilds, join, etc...
	class CUIPartyBBS*			m_pUIPartyBBS;					// Party support bulletin board.
	class CUITradeSellBBS*		m_pUITradeBBS;					// commerce bulletin board
	class CUIQuestMenu*			m_pUIQuestMenu;					// Quest Menu
	class CUIQuestTalk*			m_pUIQuestTalk;					// Quest Talk
	class CUIDead*				m_pUIDead;						// Dead UI
	class CUITradeBBSSelector*	m_pUITradeBBSSelector;			// Select commerce bulletin board type
	class CUITradeBBSEditDlg*	m_pUITradeBBSEdit;				// Commerce Post Description

	class CN3Shape*				m_pTargetSymbol;				// The player draws on the location of the target character.

	class CN3SndObjStream*		m_pSnd_Town, *m_pSnd_Battle;	// Village music, battle music pointers..
	class CMagicSkillMng*		m_pMagicSkillMng;

	class CUINpcTalk*			m_pUINpcTalk;	
	class CUINPCChangeEvent*	m_pUINpcChange;
	class CUIWarp*				m_pUIWarp;
	class CWarMessage*			m_pWarMessage;					// war message
	class CLightMgr*			m_pLightMgr;

	   
	// ..
	BOOL		m_bLoadComplete;							// Is loading complete??
	
	float		m_fMsgSendTimeMove;							// The time the last mobile message was sent.
	float		m_fMsgSendTimeRot;							// Time of last sent message..
	float		m_fPlayerYawSended; // Rotation on the player's y-axis since the last message was sent.
	float		m_fRequestGameSave;	// Last request to save game data
	__Vector3	m_vPlayerPosSended; // The player's position at the time the last message was sent.

	__Vector3	m_vMouseLBClickedPos;
	__Vector3	m_vMouseSkillPos;

	float		m_fLBClickTime;

	int			m_iJoinReqClan;
	int			m_iJoinReqClanRequierID;

	int			KM_COUNT;

protected:
	bool	ProcessPacket(DataPack* pDataPack, int& iOffset);

	bool	MsgRecv_CharacterSelect(DataPack* pDataPack, int& iOffset); // virtual
	int		MsgRecv_VersionCheck(DataPack* pDataPack, int& iOffset); // virtual

	bool	MsgRecv_MyInfo_All(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_MyInfo_HP(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_MyInfo_MSP(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_MyInfo_EXP(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_MyInfo_LevelChange(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_MyInfo_RealmPoint(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_MyInfo_PointChange(DataPack* pDataPack, int& iOffset);

	bool	MsgRecv_Chat(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_UserMove(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_Rotation(DataPack* pDataPack, int& iOffset);
// bool	MsgRecv_Dead(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_Regen(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Corpse(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_Time(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_Weather(DataPack* pDataPack, int& iOffset);
	
	bool	MsgRecv_UserInAndRequest(DataPack* pDataPack, int& iOffset);		// Receives all User IDs in the surrounding area as much as count... Well.. Requests only those that need to be updated to the server..
	bool	MsgRecv_UserInRequested(DataPack* pDataPack, int& iOffset);					// Receive the ID of the user requested from the server and process User..
	bool	MsgRecv_UserInOut(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_UserIn(DataPack* pDataPack, int& iOffset, bool bWithFX=false);
	bool	MsgRecv_UserOut(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_UserState(DataPack* pDataPack, int& iOffset);

	bool	MsgRecv_NPCInAndRequest(DataPack* pDataPack, int& iOffset);		// Receives all NPC IDs in the surrounding area by the count... Well.. Requests only those that need to be updated to the server..
	bool	MsgRecv_NPCInRequested(DataPack* pDataPack, int& iOffset);				// Receive NPC IDs requested from the server and process User..
	bool	MsgRecv_NPCInOut(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_NPCIn(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_NPCOut(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_NPCMove(DataPack* pDataPack, int& iOffset);

	bool	MsgRecv_Attack(DataPack* pDataPack, int& iOffset);
	bool	MsgRecv_Dead(DataPack* pDataPack, int& iOffset);

	bool	MsgRecv_ItemMove(DataPack* pDataPack, int& iOffset);					// Response to Item Move..
	bool	MsgRecv_ItemBundleDrop(DataPack* pDataPack, int& iOffset);			// Response to Item appearing in field
	bool	MsgRecv_ItemBundleOpen(DataPack* pDataPack, int& iOffset);			// Open item boxes or search corpses.
	bool	MsgRecv_ItemTradeStart(DataPack* pDataPack, int& iOffset);			// Item trade...
	bool	MsgRecv_ItemTradeResult(DataPack* pDataPack, int& iOffset);			// Item transaction results..
	bool	MsgRecv_ItemDroppedGetResult(DataPack* pDataPack, int& iOffset);	// The result of eating an item that fell on the ground..
	bool	MsgRecv_ItemWeightChange(DataPack* pDataPack, int& iOffset);		// Item weight changes.

	bool	MsgRecv_UserLookChange(DataPack* pDataPack, int& iOffset);			// Appearance changes. Wearing items, etc.

	void	MsgRecv_TargetHP(DataPack* pDataPack, int& iOffset);

	void	MsgRecv_ZoneChange(DataPack* pDataPack, int& iOffset);			// Just a simple zone change.

	void	MsgRecv_Notice(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_PartyOrForce(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_PerTrade(DataPack* pDataPack, int& iOffset);

	void	MsgRecv_SkillChange(DataPack* pDataPack, int& iOffset);			// Skill change...
	void	MsgRecv_MagicProcess(DataPack* pDataPack, int& iOffset);			// Skill change...
	void	MsgRecv_ClassChange(DataPack* pDataPack, int& iOffset);			// job change...
	void	MsgRecv_ObjectEvent(DataPack* pDataPack, int& iOffset);			// Response to object event request

	void	MsgRecv_DurabilityChange(DataPack* pDataPack, int& iOffset);		// Durability change...
	void	MsgRecv_NpcEvent(DataPack* pDataPack, int& iOffset);				// Npc Event(Exchange, Repair both)..

	void	MsgRecv_Knights(DataPack* pDataPack, int& iOffset);				// A packet related to the manipulation of the knights..
	void	MsgRecv_KnightsListBasic(DataPack* pDataPack, int& iOffset);		// Knights basic information..
	
	void	MsgRecv_CompressedPacket(DataPack* pDataPack, int& iOffset);		// It's a compressed packet... It needs to be decompressed and parsed once again in a loop!!!
	void	MsgRecv_ContinousPacket(DataPack* pDataPack, int& iOffset);		// It is a packet that comes attached.. You need to parse it once more while looping!!!

	void	MsgRecv_ItemRepair(DataPack* pDataPack, int& iOffset);			// Item Repair Result..
	void	MsgRecv_ItemCountChange(DataPack* pDataPack, int& iOffset);		// Item Count Change..
	void	MsgRecv_ItemDestroy(DataPack* pDataPack, int& iOffset);			// Item Count Change..

	void	MsgRecv_WareHouse(DataPack* pDataPack, int& iOffset);			// Archive related packets..
	void	MsgRecv_WareHouseOpen(DataPack* pDataPack, int& iOffset);		// Open the locker...

	void	MsgRecv_NpcChangeOpen(DataPack* pDataPack, int& iOffset);		// Class Change and Initialization..
	void	MsgRecv_AllPointInit(DataPack* pDataPack, int& iOffset);			// All Point Initialization..
	void	MsgRecv_SkillPointInit(DataPack* pDataPack, int& iOffset);		// Reset Skill Points..
	void	MsgRecv_PointChangePriceQueryRequest(DataPack* pDataPack, int& iOffset);		// Response packet for price..

	void	MsgRecv_NoahChange(DataPack* pDataPack, int& iOffset);			// Noah change..
	void	MsgRecv_WarpList(DataPack* pDataPack, int& iOffset);				// warp list..
// void MsgRecv_ServerCheckAndRequestConcurrentUserCount(DataPack* pDataPack, int& iOffset); // Receive the server IP and port and check the concurrent users.
// void	MsgRecv_ConcurrentUserCountAndSendServerCheck(DataPack* pDataPack, int& iOffset);
	
	// knights...
	void	MsgRecv_Knights_Create(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Knights_Withdraw(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Knights_Join(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Knights_Leave(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Knights_AppointViceChief(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Knights_MemberInfoAll(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Knights_GradeChangeAll(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Knights_Duty_Change(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Knigts_Join_Req(DataPack* pDataPack, int& iOffset);

public:
	void	ProcessUIKeyInput(bool bEnable = true);
	bool	OnMouseMove(POINT ptCur, POINT ptPrev);
	bool	OnMouseLbtnDown(POINT ptCur, POINT ptPrev);
	bool	OnMouseLBtnPressd(POINT ptCur, POINT ptPrev);
	bool	OnMouseLBtnPress(POINT ptCur, POINT ptPrev);
	bool	OnMouseLDBtnPress(POINT ptCur, POINT ptPrev);
	bool	OnMouseRbtnDown(POINT ptCur, POINT ptPrev);
	bool	OnMouseRBtnPressd(POINT ptCur, POINT ptPrev);
	bool	OnMouseRBtnPress(POINT ptCur, POINT ptPrev);
	bool	OnMouseRDBtnPress(POINT ptCur, POINT ptPrev);
	void	ControlViewVDegree(short sValue);
	void	PlayBGM_Town();
	void	PlayBGM_Battle();

	void	DoCommercialTransaction(int iTradeID);								// commerce..

	const __InfoPartyOrForce*	PartyOrForceConditionGet(bool& bIAmLeader, bool& bIAmMember, int& iMemberIndex, class CPlayerBase*& pTarget);
	void						TargetSelect(int iID, bool bMustAlive);
	void						TargetSelect(class CPlayerNPC* pTarget);

	bool	CommandToggleUIInventory();
	bool	CommandToggleUIState();
	bool	CommandToggleAttackContinous();
	bool	CommandToggleMoveContinous();
	bool	CommandToggleWalkRun();
	bool	CommandToggleUISkillTree();
	bool	CommandToggleUIMiniMap();

	void	CommandMove(e_MoveDirection eMD, bool bStartOrEnd); // Direction of movement (backward, forward, stop), start to move?
	void	CommandEnableAttackContinous(bool bEnable, CPlayerBase* pTarget);
	void	CommandCameraChange(); // Change the camera perspective...
	void	CommandSitDown(bool bLimitInterval, bool bSitDown, bool bImmediately = false);

	void	CommandTargetSelect_NearstEnemy(); // Grab the closest enemy target.
	void	CommandTargetSelect_NearstOurForce(); // Find the nearest party target.

	void	CloseUIs(); // Various commerce, warp, etc... Closing the UI...

	void	MsgOutput(const std::string& szMsg, D3DCOLOR crMsg);

	void	InitZone(int iZone, const __Vector3& vPosPlayer);
	void	InitUI();
	void	InitPlayerPosition(const __Vector3& vPos); // Initialize the player position.. Raise him up and make him take the basic action.

	void	MsgSend_Continous();												// Sends messages to the server periodically under certain conditions (?).
	void	MsgSend_Attack(int iTargetID, float fInterval, float fDistance);	// Sending attack packets - Prevents hacking by giving the table attack cycle at the same time.
	void	MsgSend_Move(bool bMove, bool bContinous);							// Send movement packet to server. // Is it moving? Are they moving periodically?
	void	MsgSend_Rotation();													// Send a rotation packet to the server.
	void	MsgSend_Chat(enum e_ChatMode eMode, const std::string& szChat);		// Send a chat message to the server.
	void	MsgSend_ChatSelectTarget(const std::string& szTargetID); // Set up a one-on-one chat partner.
	void	MsgSend_Regen();
	bool	MsgSend_RequestItemBundleOpen(CPlayerNPC* pCorpse); // Open item boxes or search corpses.
	void	MsgSend_RequestTargetHP(short siIDTarget, BYTE byUpdateImmediately); // 0x00 - incrementally.. 0x01 - immediate update..
	void	MsgSend_GameStart();
	bool	MsgSend_NPCEvent(short siIDTarget);
	void	MsgSend_NPCInRequest(int iID); // If there is no NPC information, request it.
	void	MsgSend_UserInRequest(int iID); // Request if there is no user information.
	void	MsgSend_Warp(); // Warp?? - Could be a Zone Change.
	void	MsgSend_ZoneChangeComplete(); // Zone change complete.. (map loading finished..)
	void	MsgSend_StateChange(enum e_SubPacket_State eSP, int iState);
	void	MsgSend_PerTradeReq(int iDestID, bool bNear = true);
	void	MsgSend_SpeedCheck(bool bInit = false);

	void	MsgSend_PartyOrForcePermit(int iPartyOrForce, bool bYesNo); // iPartyOrForce 1 : Party, 2:Force
	void	MsgSend_PartyOrForceLeave(int iPartyOrForce); // iPartyOrForce 1 : Party, 2:Force
	bool	MsgSend_PartyOrForceCreate(int iPartyOrForce, const std::string& szID); // iPartyOrForce 1 : Party, 2:Force

	void	MsgSend_ObjectEvent(int iEventID, int iNPCID);		// Event request set in object..
	void	MsgSend_Weather(int iWeather, int iPercent);
	void	MsgSend_Time(int iHour, int iMin);
	void	MsgSend_Administrator(enum e_SubPacket_Administrator eSP, const std::string& szID);
	
	void	MsgSend_KnightsJoin(int iTargetID);
	void	MsgSend_KnightsLeave(std::string& szName);
	void	MsgSend_KnightsWithdraw();
	void	MsgSend_KnightsAppointViceChief(std::string& szName);
	void	MsgSend_KnightsJoinReq(bool bJoin);	
	void	MsgSend_PerTradeBBSReq(std::string szName, int iDestID);
	void	MsgSend_CharacterSelect(); // virtual


	void	ProcessPlayerInclination();				// Slope handling.. (Slipping if the slope is steep even when standing still..).
	void	ProcessLocalInput(DWORD dwMouseFlags);	// Handling keyboard presses.
	void	ParseChattingCommand(const std::string& szCmd);
	

	void	UpdateUI_PartyOrForceButtons(); // Updates the party button on the command line as needed.
	void	UpdateUI_MiniMap();
	void	UpdateUI_TargetBar();
	void	UpdateBGM();
	void	UpdateCameraAndLight();

	void	RenderTarget();
	
	void	Init();									// Read UI and UI resources, etc.
	void	Release();								// Release..
	void	ReleaseUIs() const;
	void	ReleaseSound();
	
	void	Tick();								// Miscellaneous calculations...
	void	Render();							// rendering..

	CGameProcMain();									// constructor.
	virtual ~CGameProcMain();							// destructor.
};
