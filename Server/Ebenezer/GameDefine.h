#ifndef _GAMEDEFINE_H
#define _GAMEDEFINE_H

#define __VERSION			1081

//////////////////// Class Define ////////////////////
#define KARUWARRRIOR		101
#define KARUROGUE			102
#define KARUWIZARD			103
#define KARUPRIEST			104
#define BERSERKER			105
#define GUARDIAN			106
#define HUNTER				107
#define PENETRATOR			108
#define SORSERER			109
#define NECROMANCER			110
#define SHAMAN				111
#define DARKPRIEST			112

#define ELMORWARRRIOR		201
#define ELMOROGUE			202
#define ELMOWIZARD			203
#define ELMOPRIEST			204
#define BLADE				205
#define PROTECTOR			206
#define RANGER				207
#define ASSASSIN			208
#define MAGE				209
#define ENCHANTER			210
#define CLERIC				211
#define DRUID				212
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
// Race Define
/////////////////////////////////////////////////////
#define KARUS_BIG			1
#define KARUS_MIDDLE		2
#define KARUS_SMALL			3
#define KARUS_WOMAN			4
#define BABARIAN			11
#define ELMORAD_MAN			12
#define ELMORAD_WOMAN		13

/////////////////////////////////////////////////////
// ITEM_SLOT DEFINE
const BYTE RIGHTEAR			= 0;
const BYTE HEAD				= 1;
const BYTE LEFTEAR			= 2;
const BYTE NECK				= 3;
const BYTE BREAST			= 4;
const BYTE SHOULDER			= 5;
const BYTE RIGHTHAND		= 6;
const BYTE WAIST			= 7;
const BYTE LEFTHAND			= 8;
const BYTE RIGHTRING		= 9;
const BYTE LEG				= 10;
const BYTE LEFTRING			= 11;
const BYTE GLOVE			= 12;
const BYTE FOOT				= 13;
const BYTE RESERVED			= 14;
/////////////////////////////////////////////////////

// Success Rate by Hit Ratio //
#define GREAT_SUCCESS			0X01
#define SUCCESS					0X02
#define NORMAL					0X03
#define	FAIL					0X04

// Item Move Direction Define 
#define ITEM_INVEN_SLOT			0x01
#define ITEM_SLOT_INVEN			0x02
#define ITEM_INVEN_INVEN		0x03
#define ITEM_SLOT_SLOT			0x04
#define ITEM_INVEN_ZONE			0x05
#define ITEM_ZONE_INVEN			0x06

// Item Weapon Type Define
#define WEAPON_DAGGER			1
#define WEAPON_SWORD			2
#define WEAPON_AXE				3
#define WEAPON_MACE				4
#define WEAPON_SPEAR			5
#define WEAPON_SHIELD			6
#define WEAPON_BOW				7
#define WEAPON_LONGBOW			8
#define WEAPON_LAUNCHER			10
#define WEAPON_STAFF			11
#define WEAPON_ARROW			12
#define WEAPON_JAVELIN			13
#define WEAPON_WORRIOR_AC		21
#define WEAPON_LOG_AC			22
#define WEAPON_WIZARD_AC		23
#define WEAPON_PRIEST_AC		24
////////////////////////////////////////////////////////////
// User Status //
#define USER_STANDING			0X01
#define USER_SITDOWN			0X02
#define USER_DEAD				0x03
//#define USER_BLINKING			0x04
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Magic State
#define NONE				0x01
#define CASTING				0x02
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Durability Type
#define ATTACK				0x01
#define DEFENCE				0x02
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Knights Authority Type
/*
#define CHIEF				0x06
#define VICECHIEF			0x05*/
#define OFFICER				0x04
#define KNIGHT				0x03
//#define TRAINEE			0x02
#define PUNISH				0x01	

#define CHIEF				0x01
#define VICECHIEF			0x02
#define TRAINEE				0x05
#define COMMAND_CAPTAIN		100
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// COMMUNITY TYPE DEFINE
#define CLAN_TYPE			0x01
#define KNIGHTS_TYPE		0x02
////////////////////////////////////////////////////////////

#define MAX_CLAN			24
#define MAX_KNIGHTS_BANK	200
#define MAX_KNIGHTS_MARK	512

#define ITEM_GOLD			900000000
#define ITEM_NO_TRADE		900000001	// Untradable Items... Non-Ramuggle Christmas Event >.<

////////////////////////////////////////////////////////////
// EVENT TYPE DEFINE
#define ZONE_CHANGE			0x01
#define ZONE_TRAP_DEAD		0x02
#define ZONE_TRAP_AREA		0x03

////////////////////////////////////////////////////////////
// EVENT MISCELLANOUS DATA DEFINE
#define ZONE_TRAP_INTERVAL	   1		// Interval is one second right now.
#define ZONE_TRAP_DAMAGE	   10		// HP Damage is 10 for now :)

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// USER POINT DEFINE
#define STR					0x01
#define STA					0x02
#define DEX					0x03
#define INTEL				0x04
#define CHA					0x05

#define ORDER_SKILL			0x01
#define MANNER_SKILL		0X02
#define LANGUAGE_SKILL		0x03
#define BATTLE_SKILL		0x04
#define PRO_SKILL1			0x05
#define PRO_SKILL2			0x06
#define PRO_SKILL3			0x07
#define PRO_SKILL4			0x08

/////////////////////////////////////////////////////////////
// ITEM TYPE DEFINE
#define ITEM_TYPE_FIRE				0x01
#define ITEM_TYPE_COLD				0x02
#define ITEM_TYPE_LIGHTNING			0x03
#define ITEM_TYPE_POISON			0x04
#define ITEM_TYPE_HP_DRAIN			0x05
#define ITEM_TYPE_MP_DAMAGE			0x06
#define ITEM_TYPE_MP_DRAIN			0x07
#define ITEM_TYPE_MIRROR_DAMAGE		0x08

/////////////////////////////////////////////////////////////
// ITEM LOG TYPE 
#define ITEM_MERCHANT_BUY			0x01
#define ITEM_MERCHANT_SELL			0x02
#define ITEM_MONSTER_GET			0x03
#define ITEM_EXCHANGE_PUT			0x04
#define ITEM_EXCHANGE_GET			0x05
#define ITEM_DESTROY				0x06
#define ITEM_WAREHOUSE_PUT			0x07
#define ITEM_WAREHOUSE_GET			0x08
#define ITEM_UPGRADE				0x09

/////////////////////////////////////////////////////////////
// JOB GROUP TYPES
#define GROUP_WARRIOR				1
#define GROUP_ROGUE					2
#define GROUP_MAGE					3
#define GROUP_CLERIC				4
#define GROUP_ATTACK_WARRIOR		5
#define GROUP_DEFENSE_WARRIOR		6
#define GROUP_ARCHERER				7
#define GROUP_ASSASSIN				8
#define GROUP_ATTACK_MAGE			9
#define GROUP_PET_MAGE				10
#define GROUP_HEAL_CLERIC			11
#define GROUP_CURSE_CLERIC			12

//////////////////////////////////////////////////////////////
// USER ABNORMAL STATUS TYPES
#define ABNORMAL_NORMAL			0x01
#define ABNORMAL_GIANT			0x02
#define ABNORMAL_DWARF			0x03
#define ABNORMAL_BLINKING		0x04

//////////////////////////////////////////////////////////////
// Object Type
#define NORMAL_OBJECT		0
#define SPECIAL_OBJECT		1

//////////////////////////////////////////////////////////////
// REGENE TYPES
#define REGENE_NORMAL		0
#define REGENE_MAGIC		1
#define REGENE_ZONECHANGE	2

//////////////////////////////////////////////////////////////
// TYPE 3 ATTRIBUTE TYPES
#define ATTRIBUTE_FIRE				1
#define ATTRIBUTE_ICE				2
#define ATTRIBUTE_LIGHTNING			3

//extern CRITICAL_SECTION g_LogFile_critical;

struct _CLASS_COEFFICIENT
{
	short	sClassNum;
	char	strClassName[30];
	float	ShortSword;
	float	Sword;
	float	Axe;
	float	Club;
	float	Spear;
	float	Pole;
	float	Staff;
	float	Bow;
	float	HP;
	float	MP;
	float	SP;
	float	AC;
	float	Hitrate;
	float	Evasionrate;
};

struct _ZONE_ITEM {		// Bundle unit
	DWORD bundle_index;
	int itemid[6];
	short count[6];
	float x;
	float z;
	float y;
	float time;
};

struct	_ITEM_DATA
{
	int		nNum;
	short	sDuration;
	short	sCount;
	__int64	nSerialNum;
};

struct	_EXCHANGE_ITEM
{
	int itemid;
	int count;
	short duration;
	BYTE pos;
	__int64	nSerialNum;
};

struct _ITEM_TABLE
{
	int   m_iNum;
	char  m_strName[50];
	BYTE  m_bKind;
	BYTE  m_bSlot;
	BYTE  m_bRace;
	BYTE  m_bClass;
	short m_sDamage;
	short m_sDelay;
	short m_sRange;
	short m_sWeight;
	short m_sDuration;
	int	  m_iBuyPrice;
	int	  m_iSellPrice;
	short m_sAc;
	BYTE  m_bCountable;
	int	  m_iEffect1;
	int	  m_iEffect2;
	BYTE  m_bReqLevel;
	BYTE  m_bReqRank;
	BYTE  m_bReqTitle;
	BYTE  m_bReqStr;
	BYTE  m_bReqSta;
	BYTE  m_bReqDex;
	BYTE  m_bReqIntel;
	BYTE  m_bReqCha;
	BYTE  m_bSellingGroup;
	BYTE  m_ItemType;
	short m_sHitrate;
	short m_sEvarate;
	short m_sDaggerAc;
	short m_sSwordAc;
	short m_sMaceAc;
	short m_sAxeAc;
	short m_sSpearAc;
	short m_sBowAc;
	BYTE  m_bFireDamage;
	BYTE  m_bIceDamage;
	BYTE  m_bLightningDamage;
	BYTE  m_bPoisonDamage;
	BYTE  m_bHPDrain;
	BYTE  m_bMPDamage;
	BYTE  m_bMPDrain;
	BYTE  m_bMirrorDamage;
	BYTE  m_bDroprate;
	BYTE  m_bStrB;
	BYTE  m_bStaB;
	BYTE  m_bDexB;
	BYTE  m_bIntelB;
	BYTE  m_bChaB;
	short m_MaxHpB;
	short m_MaxMpB;
	BYTE  m_bFireR;
	BYTE  m_bColdR;
	BYTE  m_bLightningR;
	BYTE  m_bMagicR;
	BYTE  m_bPoisonR;
	BYTE  m_bCurseR;
};

struct _USER_DATA
{
	char	m_id[MAX_ID_SIZE+1];
	char	m_Accountid[MAX_ID_SIZE+1];

	BYTE	m_bZone;
	float	m_curx;
	float	m_curz;
	float	m_cury;

	BYTE	m_bNation;
	BYTE	m_bRace;
	short	m_sClass;
	BYTE	m_bHairColor;
	BYTE	m_bRank;
	BYTE	m_bTitle;
	BYTE	m_bLevel;
	int		m_iExp;
	int		m_iLoyalty;
	BYTE	m_bFace;
	BYTE	m_bCity;
	short	m_bKnights;
	BYTE	m_bFame;
	short	m_sHp;
	short	m_sMp;
	short	m_sSp;
	BYTE	m_bStr;
	BYTE	m_bSta;
	BYTE	m_bDex;
	BYTE	m_bIntel;
	BYTE	m_bCha;
	BYTE	m_bAuthority
	BYTE	m_bPoints;
	int		m_iGold;
	short	m_sBind;
	int		m_iBank;

	BYTE    m_bstrSkill[9];
	_ITEM_DATA m_sItemArray[HAVE_MAX+SLOT_MAX];
	_ITEM_DATA m_sWarehouseArray[WAREHOUSE_MAX];

	BYTE	m_bLogout;
	BYTE	m_bWarehouse;
	DWORD	m_dwTime;

};

struct _LEVELUP
{
	short	m_sLevel;
	int		m_iExp;
};

struct	_PARTY_GROUP
{
	WORD wIndex;
	short uid[8];
	short sMaxHp[8];
	short sHp[8];
	BYTE bLevel[8];
	short sClass[8];
	BYTE bItemRouting;
	_PARTY_GROUP() {
		for(int i=0;i<8;i++) {
			uid[i] = -1; sMaxHp[i] = 0; sHp[i] = 0; bLevel[i] = 0; sClass[i] = 0;
		}
		bItemRouting = 0;
	};
};

struct _OBJECT_EVENT
{
	BYTE byLife;
	int sBelong;
	short sIndex;			// 100th Division - Karus Bind Point | 200th Elmorad Bind Point | 1100's - Carus Gates 1200 - Elmorad's Gates
	short sType;			// 0 - Bind Point, 1 - Gates that open left and right, 2 - Gates that open up and down, 3 - Lever, 4 - Flag Lever, 6:Iron Window, 7-Breakable Resurrection Monument
	short sControlNpcID;	// NPC ID to control (Object Index to control), Type-> 5 : Warp Group ID
	short sStatus;
	float fPosX;
	float fPosY;
	float fPosZ;
};

struct _REGENE_EVENT
{
	int	  sRegenePoint;
	float fRegenePosX;
	float fRegenePosY;
	float fRegenePosZ;
	float fRegeneAreaZ;
	float fRegeneAreaX;
};

struct _KNIGHTS_USER
{
	BYTE    byUsed;								// In use: 1, Not in use: 0
	char	strUserName[MAX_ID_SIZE+1];
};

struct _MAGIC_TABLE
{
	int		iNum;
	short	sFlyingEffect;
	BYTE	bMoral;
	BYTE	bSkillLevel;	
	short	sSkill;
	short	sMsp;
	short   sHP;
	BYTE	bItemGroup;
	int		iUseItem;
	BYTE	bCastTime;
	BYTE	bReCastTime;
	BYTE	bSuccessRate;
	BYTE	bType1;
	BYTE	bType2;
	short   sRange;
	BYTE	bEtc;
};

struct _MAGIC_TYPE8
{
	int     iNum;
	BYTE    bTarget;
	short   sRadius;
	BYTE    bWarpType;
	short   sExpRecover;
};

struct _MAGIC_TYPE5
{
	int		iNum;
	BYTE	bType;
	BYTE	bExpRecover;
	short	sNeedStone;
};

struct _MAGIC_TYPE4
{
	int     iNum;
	short   sMSP;
	BYTE    bBuffType;
	BYTE    bRadius;
	short   sDuration;
	BYTE    bAttackSpeed;
	BYTE    bSpeed;
	short   sAC;
	BYTE    bAttack;
	short   sMaxHP;
	BYTE    bHitRate;
	short   sAvoidRate;
	BYTE    bStr;
	BYTE    bSta;
	BYTE    bDex;
	BYTE    bIntel;
	BYTE    bCha;
	BYTE    bFireR;
	BYTE    bColdR;
	BYTE    bLightningR;
	BYTE    bMagicR;
	BYTE    bDiseaseR;
	BYTE    bPoisonR;
};

struct _MAGIC_TYPE3
{
	int		iNum;
	BYTE	bRadius;
	short	sAngle;
	short	sFirstDamage;
	short	sEndDamage;
	short	sTimeDamage;
	BYTE	bDirectType;
	short	sDuration;
	BYTE	bAttribute;
};

struct _MAGIC_TYPE2
{
	int     iNum;
	BYTE    bHitType;
	short   sHitRate;
	short	sAddDamage;
	short   sAddRange;
	BYTE    bNeedArrow;
};

struct _MAGIC_TYPE1
{
	int		iNum;
	BYTE	bHitType;
	short	sHitRate;
	short	sHit;
	BYTE	bDelay;
	BYTE	bComboType;
	BYTE	bComboCount;
	short	sComboDamage;
	short	sRange;
};

struct _ZONE_SERVERINFO
{
	short		sServerNo;
	short		sPort;
	char		strServerIP[20];
	_ZONE_SERVERINFO() {memset(strServerIP, 0x00, 20);};
};

struct _WARP_INFO
{
	short	sWarpID;
	char	strWarpName[32];
	char	strAnnounce[256];
	DWORD	dwPay;
	short	sZone;
	float	fX;
	float	fY;
	float	fZ;
	float	fR;

	_WARP_INFO() {
		sWarpID = 0; sZone = 0;
		fX = fZ = fY = fR = 0.0f;
		memset( strWarpName, 0x00, 32 ); memset( strAnnounce, 0x00, 256 );
	};
};

struct _HOME_INFO
{
	BYTE	bNation;
	float	ElmoZoneX;
	float	ElmoZoneZ;
	BYTE	ElmoZoneLX;
	BYTE	ElmoZoneLZ;
	float	KarusZoneX;
	float	KarusZoneZ;
	BYTE	KarusZoneLX;
	BYTE	KarusZoneLZ;
	float	FreeZoneX;
	float	FreeZoneZ;
	BYTE	FreeZoneLX;
	BYTE	FreeZoneLZ;
//
	float	BattleZoneX;
	float	BattleZoneZ;
	BYTE	BattleZoneLX;
	BYTE	BattleZoneLZ;
//
};

#endif