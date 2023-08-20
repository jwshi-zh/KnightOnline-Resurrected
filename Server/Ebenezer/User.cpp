// User.cpp: implementation of the CUser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ebenezer.h"
#include "EbenezerDlg.h"
#include "User.h"
#include "AiPacket.h"
#include "Map.h"
#include "PacketDefine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern CRITICAL_SECTION g_region_critical;
extern CRITICAL_SECTION g_LogFile_critical;
extern BYTE g_serverdown_flag;

// Cryption
extern T_KEY g_private_key;
///~

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUser::CUser()
{
}

CUser::~CUser()
{
}

void CUser::Initialize()
{
	m_pMain = (CEbenezerDlg*)AfxGetMainWnd();

	// Cryption
	Make_public_key();
	jct.SetPublicKey(m_Public_key);
	jct.SetPrivateKey(g_private_key);

	m_CryptionFlag = 0;
	m_Sen_val = 0;
	m_Rec_val = 0;
	///~
	
	m_MagicProcess.m_pMain = m_pMain;
	m_MagicProcess.m_pSrcUser = this;

	m_RegionX = -1;
	m_RegionZ = -1;

	m_sBodyAc = 0;
	m_sTotalHit = 0;
	m_sTotalAc = 0;
	m_sTotalHitrate = 0;
	m_sTotalEvasionrate = 0;
	
	m_sItemMaxHp = 0;
	m_sItemMaxMp = 0;
	m_sItemWeight = 0;
	m_sItemHit = 0;
	m_sItemAc = 0;
	m_sItemStr = 0;
	m_sItemSta = 0;
	m_sItemDex = 0;
	m_sItemIntel = 0;
	m_sItemCham = 0;
	m_sItemHitrate = 100;
	m_sItemEvasionrate = 100;

	m_sSpeed = 0;

	m_iMaxHp = 0;
	m_iMaxMp = 1;
	m_iMaxExp = 0;
	m_sMaxWeight = 0;

	m_bFireR = 0;
	m_bColdR = 0;
	m_bLightningR = 0;
	m_bMagicR = 0;
	m_bDiseaseR = 0;
	m_bPoisonR = 0;

	m_sDaggerR = 0;			
	m_sSwordR = 0;			
	m_sAxeR = 0;						
	m_sMaceR = 0;						
	m_sSpearR = 0;					
	m_sBowR = 0;		
	
	m_bMagicTypeLeftHand = 0;		// For weapons and shields with special power.		
	m_bMagicTypeRightHand = 0;		
	m_sMagicAmountLeftHand = 0;        
	m_sMagicAmountRightHand = 0;       

	m_iZoneIndex = 0;	
	m_bResHpType = USER_STANDING;
	m_bWarp = 0x00;

	m_sPartyIndex = -1;		
	m_sExchangeUser = -1;
	m_bExchangeOK = 0x00;
	m_sPrivateChatUser = -1;
	m_bNeedParty = 0x01;

	m_fHPLastTimeNormal = 0.0f;		// For Automatic HP recovery. 
	m_fHPStartTimeNormal = 0.0f;
	m_bHPAmountNormal = 0;
	m_bHPDurationNormal = 0;
	m_bHPIntervalNormal = 5;

	m_fAreaLastTime = 0.0f;		// For Area Damage spells Type 3.
	m_fAreaStartTime = 0.0f;
	m_bAreaInterval = 5;
	m_iAreaMagicID = 0;

	m_sFriendUser = -1; 

	InitType3();	 // Initialize durational type 3 stuff :)
	InitType4();	 // Initialize durational type 4 stuff :)

	m_fSpeedHackClientTime = 0.0f;
	m_fSpeedHackServerTime = 0.0f;
	m_bSpeedHackCheck = 0;

	m_fBlinkStartTime = 0.0f;

	m_sAliveCount = 0;

	m_bAbnormalType = 1;	// User starts out in normal size.

	m_sWhoKilledMe = -1;
	m_iLostExp = 0;

	m_fLastTrapAreaTime = 0.0f;

	memset( m_strAccountID, NULL, MAX_ID_SIZE+1 );
/*
	m_iSelMsgEvent[0] = -1;		// 