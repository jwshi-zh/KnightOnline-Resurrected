// Npc.h: interface for the CNpc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPC_H__6077F7AF_6166_463A_AA80_FBF218781BC6__INCLUDED_)
#define AFX_NPC_H__6077F7AF_6166_463A_AA80_FBF218781BC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "iocport.h"
#include <My_3DStruct.h>
#include "Map.h"
#include "PathFind.h"
#include "user.h"
#include "NpcMagicProcess.h"

#define MAX_MAP_SIZE		10000
#define MAX_PATH_SIZE		100

#define NPC_MAX_USER_LIST	5

#define NPC_ATTACK_SHOUT	0
#define NPC_SUBTYPE_LONG_MON 1

#define NPC_TRACING_STEP	100

#define NPC_HAVE_USER_LIST	5
#define NPC_HAVE_ITEM_LIST	6
#define NPC_PATTEN_LIST		5
#define NPC_PATH_LIST		50
#define NPC_MAX_PATH_LIST	100
#define NPC_EXP_RANGE		50
#define NPC_EXP_PERSENT		50

#define NPC_SECFORMETER_MOVE	4
#define NPC_SECFORMETER_RUN		4
#define NPC_VIEW_RANGE			100

#define MAX_MAGIC_TYPE3			20
#define MAX_MAGIC_TYPE4			9



struct  _NpcSkillList
{
	short	sSid;
	BYTE	tLevel;
	BYTE	tOnOff;
};

struct  _NpcGiveItem
{
	int 	sSid;			// item serial number
	short	count;
};

struct	_ExpUserList
{
	TCHAR	strUserID[MAX_ID_SIZE+1];
	int		iUid;
	int		nDamage;
	BOOL	bIs;							// Determine whether it exists in the field of view (true: exists)
	//BOOL	bSameParty;						// TRUE if there is a member of the same party, FALSE otherwise
};

struct _Target
{
	int	id;							// Target user uid
	float x;						// User x pos
	float y;						// User y pos
	float z;						// User z pos
	int failCount;
};

struct _PattenPos
{
	short x;
	short z;
};

struct _Patten
{
	int	patten_index;
	_PattenPos pPattenPos[NPC_MAX_PATH_LIST];
};

struct _PathList
{
	_PattenPos pPattenPos[NPC_MAX_PATH_LIST];
};

struct _MagicType3
{
	short		sHPAttackUserID;	// Save the ID of the user who used the persistent magic
	short		sHPAmount;			// Damage over time ( Amount over time / (Duration / 2) )
	BYTE		byHPDuration;		// duration
	BYTE		byHPInterval;		// duration interval
	float		fStartTime;			// The time at which the persistence begins...
};

struct _MagicType4
{
	BYTE	byAmount;
	short	sDurationTime;
	float   fStartTime;			// The time at which the persistence begins...
};

struct _TargetHealer
{
	short	sNID;				// npc nid
	short	sValue;				// score
};

class CServerDlg;
/*
      ** Notes when working with Repent AI Server **
1. Modify all variables on the MONSTER DB side..
*/

class CNpc  
{
public:

	CServerDlg* m_pMain;
	CNpcMagicProcess m_MagicProcess;

	_Target	m_Target;				// Save the user to be attacked,
	short		m_ItemUserLevel;		// The user's level that must be referenced to drop magic or higher items when dying

	int		m_TotalDamage;	// Total accumulated damage amount
	_ExpUserList m_DamagedUserList[NPC_HAVE_USER_LIST]; // Manage the user information that gave me the hit value as a list. (Experience point distribution)
	short   m_sMaxDamageUserid;		// Save the ID of the user who gave me the most damage..

	_PathList m_PathList;			// NPC's pass list
	_PattenPos m_pPattenPos;		// pattern of NPCs,

	//int m_iPattenNumber;		// current pattern number
	short m_iPattenFrame;			// The current position of the pattern..

	BYTE  m_byMoveType;			// Behavior type of NPC (movement related)
	BYTE  m_byInitMoveType;		// NPC's initial behavior type (movement related)
	short m_sPathCount;			// NPC PathList Count
	short m_sMaxPathCount;		// NPC PathList Max Count

	BOOL	m_bFirstLive;		// Determines whether the NPC spawns for the first time or is dead and alive.
	BYTE	m_NpcState;			// NPC's status - alive, dead, standing, etc...
	short	m_ZoneIndex;		// The index of the zone where the NPC exists

	short	m_sNid;				// NPC (on server) serial number

	CMapInfo	**m_pOrgMap;		// A pointer to the original MapInfo

	float		m_nInitX;			// first created location X
	float		m_nInitY;			// first created location Y
	float		m_nInitZ;			// first created location Z

	short		m_sCurZone;			// Current Zone;
	float		m_fCurX;			// Current X Pos;
	float		m_fCurY;			// Current Y Pos;
	float		m_fCurZ;			// Current Z Pos;

	float		m_fPrevX;			// Prev X Pos;
	float		m_fPrevY;			// Prev Y Pos;
	float		m_fPrevZ;			// Prev Z Pos;

	//
	//	PathFind Info
	//
	short		m_min_x;
	short		m_min_y;
	short		m_max_x;
	short		m_max_y;

	int		m_pMap[MAX_MAP_SIZE]; // 2D -> 1D array x * sizey + y

	SIZE	m_vMapSize;

	float m_fStartPoint_X, m_fStartPoint_Y;
	float m_fEndPoint_X, m_fEndPoint_Y;

	short m_sStepCount;

	CPathFind m_vPathFind;
	_PathNode	*m_pPath;

	int		m_nInitMinX;					// initial position
	int		m_nInitMinY;
	int		m_nInitMaxX;
	int		m_nInitMaxY;

	// NPC Class, Skill
	//_NpcSkillList	m_NpcSkill[SKILL_NUM];	// Skills NPCs have
	//BYTE			m_tWeaponClass;			// NPC's weapon class

	// Persistent magic...
	float		m_fHPChangeTime;			// HP recovery rate
	float       m_fFaintingTime;			// Time to faint...
	_MagicType3  m_MagicType3[MAX_MAGIC_TYPE3];			// Magic related to HP...
	_MagicType4  m_MagicType4[MAX_MAGIC_TYPE4];			// Abilities related magic...

	//----------------------------------------------------------------
	//	Variables on the MONSTER DB side
	//----------------------------------------------------------------
	short	m_sSid;				// MONSTER(NPC) Serial ID
	TCHAR	m_strName[MAX_ID_SIZE+1];		// MONSTER(NPC) Name
	short	m_sPid;							// MONSTER(NPC) Picture ID
	short   m_sSize;						// Character proportions (by 100 percent)
	int     m_iWeapon_1;
	int     m_iWeapon_2;
	BYTE	m_byGroup;
	BYTE	m_byActType;
	BYTE	m_byRank;
	BYTE	m_byTitle;
	int 	m_iSellingGroup;
	short	m_sLevel;
	int		m_iExp;
	int		m_iLoyalty;
	int		m_iMaxHP;
	short	m_sMaxMP;
	short	m_sAttack;
	short	m_sDefense;
	short	m_sHitRate;
	short   m_sEvadeRate;
	short	m_sDamage;
	short	m_sAttackDelay;
	short	m_sSpeed;
	float   m_fSpeed_1;			// Basic movement type (distance that can be traveled in 1 second)
	float   m_fSpeed_2;			// Jumping movement type.. (distance that can be traveled in 1 second)
	short	m_sStandTime;
	int		m_iMagic1;
	int		m_iMagic2;
	int		m_iMagic3;
	BYTE	m_byFireR;
	BYTE	m_byColdR;
	BYTE	m_byLightningR;
	BYTE	m_byMagicR;
	BYTE	m_byDiseaseR;
	BYTE	m_byPoisonR;
	BYTE	m_byLightR;
	float	m_fBulk;			// Size of monsters (actual proportions)
	BYTE	m_bySearchRange;
	BYTE	m_byAttackRange;
	BYTE	m_byTracingRange;

	short	m_sAI;				// artificial intelligence index
	
	BYTE	m_tNpcType;			// NPC Type
								// 0 : Normal Monster
								// 1 : NPC

	short	m_byFamilyType;		// Determines family relationships among mobs.
	BYTE	m_tItemPer;			// Chance of dropping an item
	BYTE	m_tDnPer;			// probability of running out of money
	BYTE    m_byMoneyType;		// Giving a lot of money to Event monsters (0: Rooting, 1: Sharing immediately without rooting)

	int		m_iMoney;			// falling money
	int		m_iItem;			// falling items
	//BYTE	m_byMemory;			// attack type

	int			m_iHP;
	short		m_sMP;
	
	float   m_fSecForMetor;		// Distance traveled per second...

	//----------------------------------------------------------------
	//	Variables related to MONSTER AI
	//----------------------------------------------------------------
	BYTE	m_tNpcLongType;		// Attack distance: long range (1), short range (0), direct/indirect (2)
	BYTE	m_tNpcAttType;		// Attack Tendency: First Attack (1), Second Attack (0)
	BYTE	m_tNpcOldAttType;	// Attack tendency: first attack (1), second attack (0) (active area control)
	BYTE	m_tNpcGroupType;	// Do they form clusters (1) or not? (0)
	BYTE	m_byNpcEndAttType;	// If you fight to the end (1), otherwise (0)
//	BYTE	m_tNpcTraceType;	// Follow to the end (1), stop when out of sight (0)
	BYTE    m_byAttackPos;		// Determine which part of the user to attack (8 directions)
	BYTE    m_byBattlePos;		// Decide which formation to choose..
	BYTE	m_byWhatAttackType; // Attack type: Normal (0), short-range magic (1), poison (2), healing (3), area magic only (4), 1+4 magic (5)
	BYTE	m_byGateOpen;		// In case of a gate... Use... Gate Npc Status -> 1 : open 0 : close
	BYTE    m_byMaxDamagedNation;	// Save the country of the user who killed me.. (1: Carus, 2: Elmorad)
	BYTE    m_byObjectType;         // Usually 0, object type (gate, lever) is 1
	BYTE	m_byDungeonFamily;		// Groups of the same family in a dungeon (same room)
	BYTE	m_bySpecialType;		// Determining whether the shape of the monster changes (0: unchanged, 1: changing monster,
									// 2: The monster that controls when it dies (the castle gate opens when the captain monster dies),
									// 3:Monsters related to the death of the captain monster (so that when the captain monster dies, the related monsters die together)
									// 4:A monster that adjusts when it dies while changing (m_sControlSid)
									// 5:Monsters that appear after being dead at first,
									// 6:Monsters that act after a certain period of time,
									// 100:Please record the user who inflicted the most damage upon death.
	BYTE	m_byTrapNumber;			// The number of traps in the dungeon,
	BYTE	m_byChangeType;			// 0: normal state, 1: preparation for change, 2: change to another monster, 3: appearance of a monster, 100: death of a monster
	BYTE	m_byRegenType;			// 0: Regen normally.. , 1: Special body that cannot regen once you die, 2: Regen does not work
	BYTE    m_byDeadType;			// 0: if alive, 100: if dead during a war event
	short   m_sChangeSid;			// The Sid number of the monster that changes..
	short   m_sControlSid;			// The Sid number of the monster being controlled.


	//----------------------------------------------------------------
	//	Variables on the MONSTER_POS DB side
	//----------------------------------------------------------------
	int		m_Delay;			// Time to transition to the next state
	float   m_fDelayTime;		// Npc Thread Check Time...

	BYTE	m_byType;
	int		m_sRegenTime;		//NPC play time 

	int		m_nLimitMinX;		// activity area
	int		m_nLimitMinZ;
	int		m_nLimitMaxX;
	int		m_nLimitMaxZ;

	long	m_lEventNpc;

	float m_fAdd_x;
	float m_fAdd_z;

	float m_fBattlePos_x;	
	float m_fBattlePos_z;

	float m_fSecForRealMoveMetor;		// The distance you can go per second.. (distance sent to the actual client)
	float m_fDir;						// NPC's direction

	BOOL m_bPathFlag;					// Pathfinder execution check variable..

	//----------------------------------------------------------------
	//	NPC movement related
	//----------------------------------------------------------------
	_NpcPosition	m_pPoint[MAX_PATH_LINE];			// Reference coordinates when moving

	short m_iAniFrameIndex;
	short m_iAniFrameCount;
	BYTE m_byPathCount;					// So that monsters moving along the path do not overlap,
	BYTE m_byResetFlag;					// In the case of a tracking attack, it is determined whether or not to repeat path finding.
	BYTE m_byActionFlag;				// Behavior change flag ( 0: no behavior change, 1: pursuit from attack)

	short m_iRegion_X;					// current region - x pos
	short m_iRegion_Z;					// current region - z pos
	short m_iFind_X[4];					// Region inspection area to find in find enemy
	short m_iFind_Y[4];

	float   m_fOldSpeed_1;			// Basic movement type (distance that can be traveled in 1 second)
	float   m_fOldSpeed_2;			// Jumping movement type.. (distance that can be traveled in 1 second)

	// test
	short m_sThreadNumber;				// Thread number to which it belongs


public:
	CNpc();
	virtual ~CNpc();

	void Init();
	void InitTarget(void);
	void InitUserList();
	void InitPos();
	void InitMagicValuable();

protected:
	void ClearPathFindData(void);

public:

	void FillNpcInfo(char *temp_send, int &index, BYTE flag);
	void NpcStrategy(BYTE type);
	void NpcTypeParser();
	int  FindFriend(int type=0);
	void  FindFriendRegion(int x, int z, MAP* pMap, _TargetHealer* pHealer, int type=0);
	//void  FindFriendRegion(int x, int z, MAP* pMap, int type=0);
	BOOL IsCloseTarget(CUser *pUser, int nRange);
	void ToTargetMove(CIOCPort* pIOCP, CUser* pUser);
	int SendDead(CIOCPort* pIOCP, int type = 1);			// Npc Dead
	void SendExpToUserList();								// Distribution of user experience points..
	BOOL SetDamage(int nAttackType, int nDamage, TCHAR *id, int uid, CIOCPort* pIOCP);	// NPC's damage calculation..
	BOOL SetHMagicDamage(int nDamage, CIOCPort* pIOCP);	// NPC's damage calculation..
	int GetDefense();										// NPC defense value.
	void ChangeTarget(int nAttackType, CUser *pUser, CIOCPort* pIOCP);
	void ChangeNTarget(CNpc *pNpc, CIOCPort* pIOCP);
	int GetFinalDamage(CUser *pUser, int type = 1);
	int GetNFinalDamage(CNpc *pNpc);
	BYTE GetHitRate(float rate);
	BOOL ResetPath();
	BOOL GetTargetPos(float& x, float& z);
	BOOL IsChangePath(int nStep = 1);
	int Attack(CIOCPort* pIOCP);
	int LongAndMagicAttack(CIOCPort* pIOCP);
	int TracingAttack(CIOCPort* pIOCP);
	int GetTargetPath(int option = 0);
	int	GetPartyDamage(int iNumber);
	int IsCloseTarget(int nRange, int Flag=0);
	BOOL StepMove(int nStep, CIOCPort* pIOCP);
	BOOL StepNoPathMove(int nStep);
	BOOL IsMovingEnd();
	BOOL IsMovable(float x, float z);
	int  IsSurround(CUser* pUser);
	BOOL IsDamagedUserList(CUser *pUser);
	void IsUserInSight();
	BOOL IsLevelCheck(int iLevel);
	BOOL IsHPCheck(int iHP);
	BOOL IsCompStatus(CUser* pUser);
	BOOL IsPathFindCheck(float fDistance);						// A routine that checks whether path find is to be performed.
	void IsNoPathFind(float fDistance);						// A routine to go to the target without pass find..
	BOOL IsInExpRange(CUser* pUser);
	void GiveNpcHaveItem(CIOCPort* pIOCP);		// Drops items from NPCs

	void NpcLive(CIOCPort* pIOCP);
	void NpcFighting(CIOCPort* pIOCP);
	void NpcTracing(CIOCPort* pIOCP);
	void NpcAttacking(CIOCPort* pIOCP);
	void NpcMoving(CIOCPort* pIOCP);
	void NpcSleeping(CIOCPort* pIOCP);
	void NpcFainting(CIOCPort* pIOCP, float currenttime);
	void NpcHealing(CIOCPort* pIOCP);
	void NpcStanding();
	void NpcBack(CIOCPort* pIOCP);
	BOOL SetLive(CIOCPort* pIOCP);

	BOOL IsInRange(int nX, int nZ);
	BOOL RandomMove();
	BOOL RandomBackMove();
	BOOL IsInPathRange();
	int GetNearPathPoint();

	void SendAll(CIOCPort* pIOCP, TCHAR *pBuf, int nLength);
	void SendAttackSuccess(CIOCPort* pIOCP, BYTE byResult, int tuid, short sDamage, int nHP=0, BYTE byFlag = 0, short sAttack_type=1);
	void SendNpcInfoAll(char *temp_send, int &index, int count);	// Send all npc information to the game server...

	// Inline Function
	BOOL SetUid(float x, float z, int id);

	void Dead(CIOCPort* pIOCP, int iDeadType = 0);
	BOOL FindEnemy();
	BOOL CheckFindEnermy();
	int FindEnemyRegion();
	float FindEnemyExpand(int nRX, int nRZ, float fCompDis, int nType);
	int GetMyField();

	void NpcTrace(TCHAR *pMsg);

	int GetDir(float x1, float z1, float x2, float z2);
	void NpcMoveEnd(CIOCPort* pIOCP);

	inline float RandomGenf(float max, float min);
	__Vector3 MyFunc(__Vector3 vCur, __Vector3 vDir, float fYDegree, float fDistance);
	__Vector3 GetDirection(__Vector3 vStart, __Vector3 vEnd);
	// GetVectorPosition : Returns coordinates separated by fDis distance from vOrig in the direction of vOrig->vDest
	__Vector3 GetVectorPosition(__Vector3 vOrig, __Vector3 vDest, float fDis);
	// CalcAdaptivePosition : Returns coordinates separated by fDis distance from vPosDest in the direction of vPosDest->vPosOrig
	__Vector3 CalcAdaptivePosition(__Vector3 vPosOrig, __Vector3 vPosDest, float fAttackDistance);
	__Vector3 ComputeDestPos( __Vector3 vCur, float fDegree, float fDegreeOffset, float fDistance);
	void Yaw2D(float fDirX, float fDirZ, float& fYawResult);
	float GetDistance(__Vector3 vOrig, __Vector3 vDest);
	int  PathFind(CPoint start, CPoint end, float fDistance);
	BOOL GetUserInView();	// Determines if the user is within visual range of the NPC
	BOOL GetUserInViewRange(int x, int z);
	void MoveAttack(CIOCPort* pIOCP);
	void HpChange(CIOCPort* pIOCP);
	void MSpChange(int type, int amount);
	void ItemWoreOut( int type, int damage );
	int	 ItemProdution(int item_number);
	int  GetItemGrade(int item_grade);
	int  GetItemCodeNumber(int level, int item_type);
	int  GetWeaponItemCodeNumber(int item_type);
	void DurationMagic_4(CIOCPort* pIOCP, float currenttime);
	void DurationMagic_3(CIOCPort* pIOCP, float currenttime);
	void ChangeMonsterInfomation(int iChangeType);
	int  GetPartyExp( int party_level, int man, int nNpcExp );
	void ChangeAbility(int iChangeType);
	BOOL Teleport(CIOCPort* pIOCP);
};

#endif // !defined(AFX_NPC_H__6077F7AF_6166_463A_AA80_FBF218781BC6__INCLUDED_)
