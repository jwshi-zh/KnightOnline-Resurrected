// User.h: interface for the CUser class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USER_H__5FEC1968_ED75_4AAF_A4DB_CB48F6940B2E__INCLUDED_)
#define AFX_USER_H__5FEC1968_ED75_4AAF_A4DB_CB48F6940B2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include "IOCPSocket2.h"
#include "define.h"
#include "GameDefine.h"
#include "MagicProcess.h"
#include "Npc.h"
#include "EVENT.h"
#include "EVENT_DATA.h"
#include "LOGIC_ELSE.h"
#include "EXEC.h"     

#include <list>
typedef	 std::list<_EXCHANGE_ITEM*> ItemList;
typedef  list<int> UserEventList;

#define BANISH_DELAY_TIME    30

class CEbenezerDlg;
class CUser : public CIOCPSocket2  
{
public:
	_USER_DATA*	m_pUserData{};

	char	m_strAccountID[MAX_ID_SIZE+1];
	
	short	m_RegionX{};
	short	m_RegionZ{};

	int		m_iMaxExp{};
	short	m_sMaxWeight{};
	BYTE    m_sSpeed{};

	short	m_sBodyAc{};

	short	m_sTotalHit{};
	short	m_sTotalAc{};
	float	m_sTotalHitrate{};
	float	m_sTotalEvasionrate{};

	short   m_sItemMaxHp{};
	short   m_sItemMaxMp{};
	short	m_sItemWeight{};
	short	m_sItemHit{};
	short	m_sItemAc{};
	short	m_sItemStr{};
	short	m_sItemSta{};
	short	m_sItemDex{};
	short	m_sItemIntel{};
	short	m_sItemCham{};
	short	m_sItemHitrate{};
	short	m_sItemEvasionrate{};

	BYTE	m_bFireR{};
	BYTE	m_bColdR{};
	BYTE	m_bLightningR{};
	BYTE	m_bMagicR{};
	BYTE	m_bDiseaseR{};
	BYTE	m_bPoisonR{};

	BYTE    m_bMagicTypeLeftHand{};			// The type of magic item in user's left hand  
	BYTE    m_bMagicTypeRightHand{};			// The type of magic item in user's right hand
	short   m_sMagicAmountLeftHand{};         // The amount of magic item in user's left hand
	short	m_sMagicAmountRightHand{};        // The amount of magic item in user's left hand

	short   m_sDaggerR{};						// Resistance to Dagger
	short   m_sSwordR{};						// Resistance to Sword
	short	m_sAxeR{};						// Resistance to Axe
	short	m_sMaceR{};						// Resistance to Mace
	short	m_sSpearR{};						// Resistance to Spear
	short	m_sBowR{};						// Resistance to Bow		

	short	m_iMaxHp{};
	short	m_iMaxMp{};
	
	short	m_iZoneIndex{};

	float	m_fWill_x{};
	float	m_fWill_z{};
	float	m_fWill_y{};

	BYTE	m_bResHpType{};					// HP recovery type
	BYTE	m_bWarp{};						// John on the go...
	BYTE	m_bNeedParty{};					// party... looking for it

	short	m_sPartyIndex{};
	short	m_sExchangeUser{};				// User in exchange
	BYTE	m_bExchangeOK{};

	ItemList	m_ExchangeItemList{};
	_ITEM_DATA	m_MirrorItem[HAVE_MAX];			// Writes a backup item list when exchanging.

	short	m_sPrivateChatUser{};

	float	m_fHPLastTimeNormal{};					// For Automatic HP recovery. 
	float	m_fHPStartTimeNormal{};
	short	m_bHPAmountNormal{};
	BYTE	m_bHPDurationNormal{};
	BYTE	m_bHPIntervalNormal{};

	float	m_fHPLastTime[MAX_TYPE3_REPEAT];		// For Automatic HP recovery and Type 3 durational HP recovery.
	float	m_fHPStartTime[MAX_TYPE3_REPEAT];
	short	m_bHPAmount[MAX_TYPE3_REPEAT];
	BYTE	m_bHPDuration[MAX_TYPE3_REPEAT];
	BYTE	m_bHPInterval[MAX_TYPE3_REPEAT];
	short	m_sSourceID[MAX_TYPE3_REPEAT];
	BOOL	m_bType3Flag{};

	float	m_fAreaLastTime{};			// For Area Damage spells Type 3.
	float   m_fAreaStartTime{};
	BYTE    m_bAreaInterval{};
	int     m_iAreaMagicID{};

	BYTE	m_bAttackSpeedAmount{};		// For Character stats in Type 4 Durational Spells.
	BYTE    m_bSpeedAmount{};
	short   m_sACAmount{};
	BYTE    m_bAttackAmount{};
	short	m_sMaxHPAmount{};
	BYTE	m_bHitRateAmount{};
	short	m_sAvoidRateAmount{};
	BYTE	m_bStrAmount{};
	BYTE	m_bStaAmount{};
	BYTE	m_bDexAmount{};
	BYTE	m_bIntelAmount{};
	BYTE	m_bChaAmount{};
	BYTE	m_bFireRAmount{};
	BYTE	m_bColdRAmount{};
	BYTE	m_bLightningRAmount{};
	BYTE	m_bMagicRAmount{};
	BYTE	m_bDiseaseRAmount{};
	BYTE	m_bPoisonRAmount{};
	
	short   m_sDuration1{};  float   m_fStartTime1{};
	short   m_sDuration2{};  float   m_fStartTime2{};
	short   m_sDuration3{};  float   m_fStartTime3{};
	short   m_sDuration4{};  float   m_fStartTime4{};
	short   m_sDuration5{};  float   m_fStartTime5{};
	short   m_sDuration6{};  float   m_fStartTime6{};
	short   m_sDuration7{};  float   m_fStartTime7{};
	short   m_sDuration8{};  float   m_fStartTime8{};
	short   m_sDuration9{};  float   m_fStartTime9{};

	BYTE	m_bType4Buff[MAX_TYPE4_BUFF];
	BOOL	m_bType4Flag{};
		
	CEbenezerDlg* m_pMain{};
	CMagicProcess m_MagicProcess;

	float	m_fSpeedHackClientTime{}, m_fSpeedHackServerTime{};
	BYTE	m_bSpeedHackCheck{};

	short	m_sFriendUser{};				// who are you trying to make friends with?

	float	m_fBlinkStartTime{};			// When did you start to blink?

	short	m_sAliveCount{};

	BYTE	m_bAbnormalType;			// Is the player normal,a giant, or a dwarf?

	short	m_sWhoKilledMe{};				// Who killed me???
	int		m_iLostExp{};					// Experience point that was lost when you died.

	float	m_fLastTrapAreaTime{};		// The last moment you were in the trap area.

	BOOL	m_bZoneChangeFlag{};			// I hate Sungyong!!

	BYTE	m_bRegeneType{};				// Did you die and go home or did you type '/town'?

	float	m_fLastRegeneTime{};			// The last moment you got resurrected.

	BOOL	m_bZoneChangeSameZone{};		// Did the server change when you warped?

	// Related for event... Mr. Jung-ae, if I see this, I'll shoot cocas ^^;
//	int					m_iSelMsgEvent[5];	// Running select messagebox event
	int					m_iSelMsgEvent[MAX_MESSAGE_EVENT];	// Running select messagebox event
	short				m_sEventNid{};		// Last selected event NPC number
	UserEventList		m_arUserEvent{};		// list of executed events

	char	m_strCouponId[MAX_COUPON_ID_LENGTH];		// What was the number of the coupon?
	int		m_iEditBoxEvent{};

	short	m_sEvent[MAX_CURRENT_EVENT];				// List of already executed events :)
	

public:
	void RecvDeleteChar( char* pBuf );
	BOOL ExistComEvent(int eventid);
	void SaveComEvent(int eventid);
	BOOL CheckItemCount(int itemid, short min, short max);
	void CouponEvent( char* pBuf );
	void LogCoupon(int itemid, int count);
	void RecvEditBox(char *pBuf);
	BOOL CheckCouponUsed();
	BOOL CheckRandom(short percent);
	void OpenEditBox(int message, int event);
	BOOL CheckEditBox();
	void NativeZoneReturn();
	void EventMoneyItemGet( int itemid, int count );
	void KickOutZoneUser(BOOL home = FALSE);
	void TrapProcess();
	BOOL JobGroupCheck(short jobgroupid);
	void SelectMsg(EXEC* pExec);
	void SendNpcSay(EXEC* pExec);
	BOOL CheckClass(short class1, short class2, short class3, short class4, short class5, short class6);
	void Make_public_key();
	void RecvSelectMsg(char *pBuf);
	BOOL GiveItem(int itemid, short count);
	BOOL RobItem(int itemid, short count);
	BOOL CheckExistItem(int itemid, short count);
	BOOL CheckWeight(int itemid, short count);
	BOOL CheckSkillPoint(BYTE skillnum, BYTE min, BYTE max);
	BOOL GoldLose(int gold);
	void GoldGain(int gold);
	void SendItemWeight();
	void ItemLogToAgent(const char *srcid, const char *tarid, int type, __int64 serial, int itemid, int count, int durability);
	void TestPacket( char* pBuf );
	BOOL RunEvent(EVENT_DATA *pEventData);
	BOOL RunNpcEvent(CNpc* pNpc, EXEC* pExec);
	BOOL CheckEventLogic(EVENT_DATA* pEventData);
	void ClientEvent(char* pBuf);
	void KickOut( char* pBuf );
	void SetLogInInfoToDB(BYTE bInit);
	void BlinkTimeCheck(float currenttime);
	void MarketBBSSellPostFilter();
	void MarketBBSBuyPostFilter();
	void MarketBBSMessage(char *pBuf);
	void MarketBBSSellDelete(short index);
	void MarketBBSBuyDelete(short index);
	void MarketBBSUserDelete();
	void MarketBBSTimeCheck();
	void MarketBBSRemotePurchase(char *pBuf);
	void MarketBBSReport(char *pBuf, BYTE type);
	void MarketBBSDelete(char *pBuf);
	void MarketBBSRegister(char *pBuf);
	void MarketBBS(char *pBuf);
	void PartyBBSNeeded(char *pBuf, BYTE type);
	void PartyBBSDelete(char *pBuf);
	void PartyBBSRegister(char *pBuf);
	void PartyBBS(char *pBuf);
	void Corpse();
	void FriendAccept(char *pBuf);
	void FriendRequest(char *pBuf);
	void Friend(char *pBuf);
	BOOL WarpListObjectEvent(short objectindex, short nid);
	BOOL FlagObjectEvent(short objectindex, short nid);
	BOOL GateLeverObjectEvent(short objectindex, short nid);
	BOOL GateObjectEvent(short objectindex, short nid);
	BOOL BindObjectEvent(short objectindex, short nid);
	void InitType3();
	BOOL GetWarpList( int warp_group );
	void ServerChangeOk( char* pBuf );
	void ZoneConCurrentUsers( char* pBuf );
	void SelectWarpList( char* pBuf );
	void GoldChange(short tid, int gold);
	void AllSkillPointChange();
	void AllPointChange();
	void ClassChangeReq();
	void FriendReport( char* pBuf );
	CUser* GetItemRoutingUser(int itemid, short itemcount);
	void Home();
	void ReportBug(char *pBuf);
	int GetEmptySlot( int itemid, int bCountable );
	void InitType4();
	void WarehouseProcess( char* pBuf );
	short GetACDamage(int damage, short tid);
	short GetMagicDamage(int damage, short tid);
	void Type3AreaDuration( float currenttime);
	void ServerStatusCheck();
	void SpeedHackTime( char* pBuf );
	void OperatorCommand( char* pBuf );
	void ItemRemove( char* pBuf );
	void SendAllKnightsID();
	BYTE ItemCountChange(int itemid, int type, int amount);
	void Type4Duration(float currenttime);
	void ItemRepair( char* pBuf );
	int ExchangeDone();
	void HPTimeChange( float currenttime );
	void HPTimeChangeType3( float currenttime );
	void ItemDurationChange( int slot, int maxvalue, int curvalue, int amount );
	void ItemWoreOut( int type, int damage );
	void Dead();
	void LoyaltyDivide( short tid );
	void UserDataSaveToAgent();
	void CountConcurrentUser();
	void SendUserInfo(char *temp_send, int &index);
	void ChatTargetSelect( char* pBuf );
	BOOL ItemEquipAvailable( _ITEM_TABLE* pTable );
	void ClassChange( char* pBuf );
	void MSpChange(int amount);
	void UpdateGameWeather( char* pBuf, BYTE type );
	void ObjectEvent( char* pBuf );
	void SkillPointChange( char* pBuf );
	BOOL ExecuteExchange();
	void InitExchange(BOOL bStart);
	void ExchangeCancel();
	void ExchangeDecide();
	void ExchangeAdd( char* pBuf );
	void ExchangeAgree( char* pBuf );
	void ExchangeReq( char* pBuf );
	void ExchangeProcess( char* pBuf );
	void PartyDelete();
	void PartyRemove( int memberid );
	void PartyInsert();
	void PartyCancel();
	void PartyRequest( int memberid, BOOL bCreate );
	void PartyProcess( char* pBuf );
	void SendNotice();
	void UserLookChange( int pos, int itemid, int durability );
	void SpeedHackUser();
	void VersionCheck();
	void LoyaltyChange( short tid );
	void StateChange( char* pBuf );
	void PointChange( char* pBuf );
	void ZoneChange( int zone, float x, float z );
	void ItemGet( char* pBuf );
	BOOL IsValidName( char* name );
	void AllCharInfoToAgent();
	void SelNationToAgent( char* pBuf );
	void DelCharToAgent( char* pBuf );
	void NewCharToAgent( char* pBuf );
	void BundleOpenReq( char* pBuf );
	void SendTargetHP( BYTE echo, int tid, int damage = 0 );
	void ItemTrade( char* pBuf );
	void NpcEvent( char* pBuf );
	BOOL IsValidSlotPos( _ITEM_TABLE* pTable, int destpos );
	void ItemMove( char* pBuf );
	void Warp( char* pBuf );
	void RequestNpcIn( char* pBuf );
	void SetUserAbility();
	void LevelChange(short level, BYTE type=TRUE);	// type : TRUE => level up, FALSE => level down
	void HpChange(int amount, int type=0, bool attack=false);
	short GetDamage(short tid, int magicid);
	void SetSlotItemValue();
	BYTE GetHitRate(float rate);
	void RequestUserIn( char* pBuf );
	void InsertRegion(int del_x, int del_z);
	void RemoveRegion( int del_x, int del_z );
	void RegisterRegion();
	void SetDetailData();
	void SendTimeStatus();
	void Regene(char* pBuf, int magicid = 0);
	void SetMaxMp();
	void SetMaxHp(int iFlag=0); // Fill 0:default, 1:hp as much as maxhp
	void ExpChange(int iExp);
	void Chat( char* pBuf );
	void LogOut();
	void SelCharToAgent( char* pBuf );
	void SendMyInfo();
	void SelectCharacter( char* pBuf );
	void Send2AI_UserUpdateInfo();
	void Attack( char* pBuf );
	void UserInOut( BYTE Type );
	void Initialize();
	void MoveProcess( char* pBuf );
	void Rotate( char* pBuf );
	void LoginProcess( char* pBuf );
	void Parsing( int len, char* pData );
	void CloseProcess();
	CUser();
	virtual ~CUser();
};

#endif // !defined(AFX_USER_H__5FEC1968_ED75_4AAF_A4DB_CB48F6940B2E__INCLUDED_)
