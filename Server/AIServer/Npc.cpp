#include "stdafx.h"
#include "math.h"
#include "Npc.h"
#include "Serverdlg.h"
#include "Gamesocket.h"
#include "Region.h"
#include "Party.h"

int surround_x[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
int surround_z[8] = {0, -1, -1, -1, 0, 1, 1, 1};


int test_id = 1056;
int cur_test = 0;	// 1 = in test, 0 is OK

#include "extern.h"
//BOOL g_bDebug = TRUE;

#define ATROCITY_ATTACK_TYPE 1				// go first
#define TENDER_ATTACK_TYPE	 0				// after work

// Behavior change related
#define NO_ACTION				0
#define ATTACK_TO_TRACE			1				// chase from attack
#define MONSTER_CHANGED			1 
#define LONG_ATTACK_RANGE		30				// long range attack range
#define SHORT_ATTACK_RANGE		3				// direct attack range

// 무기 관련
#define WEAPON_ITEM				1
#define DEFENCR_ITEM			2
#define ACCESSORY_ITEM			3

#define ARROW_MIN				391010000
#define ARROW_MAX				392010000

#define KARUS_MAN				1
#define ELMORAD_MAN				2

#define ATTACK_LIMIT_LEVEL		10
#define FAINTING_TIME			2

extern CRITICAL_SECTION g_region_critical;
extern CRITICAL_SECTION g_LogFileWrite;

/*
      ** Notes when working with Repent AI Server **
1. Modify to NPC_SECFORMETER_MOVE(4) as there are no variables called m_fSpeed_1,m_fSpeed_2,,
Later, this will also be modified in the knights way (table)
2. No m_byGroup variable.. (All of these comments..)
3. No m_byTracingRange variable. Replace with m_bySearchRange*2
4. Modify GetFinalDamage(), GetNFinalDamage(), GetDefense() functions.. Attack value calculation formula..
5. Modify FillNpcInfo()
6. Modify SendNpcInfoAll()
7. Modify the SendAttackSuccess() part.. Modify the calling part...
*/


//////////////////////////////////////////////////////////////////////
//	Inline Function
//
inline BOOL CNpc::SetUid(float x, float z, int id)
{
	if(m_sNid == test_id)
	{
		int x=0;
	}

	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size()) 
	{
		TRACE("#### Npc-SetUid ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return FALSE;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];

	int x1 = (int)x / TILE_SIZE;
	int z1 = (int)z / TILE_SIZE;
	int nRX = (int)x / VIEW_DIST;
	int nRY = (int)z / VIEW_DIST;

	if(x1 < 0 || z1 < 0 || x1 > pMap->m_sizeMap.cx || z1 > pMap->m_sizeMap.cy)
	{
		TRACE("#### SetUid Fail : [nid=%d, sid=%d], x1=%d, z1=%d #####\n", m_sNid+NPC_BAND, m_sSid, x1, z1);
		return FALSE;
	}

	// If map movement is impossible, npc registration fails..
	// Action: Fix this part later...
	// if(pMap->m_pMap[x1][z1].m_sEvent == 0) return FALSE;
	if(nRX > pMap->GetXRegionMax() || nRY > pMap->GetZRegionMax() || nRX < 0 || nRY < 0)
	{
		TRACE("#### SetUid Fail : [nid=%d, sid=%d], nRX=%d, nRZ=%d #####\n", m_sNid+NPC_BAND, m_sSid, nRX, nRY);
		return FALSE;
	}

	if(m_iRegion_X != nRX || m_iRegion_Z != nRY)
	{
		int nOld_RX = m_iRegion_X;
		int nOld_RZ = m_iRegion_Z;
		m_iRegion_X = nRX;		m_iRegion_Z = nRY;

		//TRACE("++ Npc-SetUid RegionAdd : [nid=%d, name=%s], x=%.2f, z=%.2f, nRX=%d, nRZ=%d \n", m_sNid+NPC_BAND, m_strName,x,z, nRX, nRY);
		// Move npc to new region - Add npc's information..
		CNpc* pNpc = m_pMain->m_arNpc.GetData( id-NPC_BAND );
		if(pNpc == NULL)
			return FALSE;
		pMap->RegionNpcAdd(m_iRegion_X, m_iRegion_Z, id);

		// Delete npc information from existing region information..
		if (nOld_RX<0 || nOld_RZ<0 || nOld_RX > pMap->m_sizeRegion.cx || nOld_RZ > pMap->m_sizeRegion.cy) {
		}
		else {
			pMap->RegionNpcRemove(nOld_RX, nOld_RZ, id);
			//TRACE("-- Npc-SetUid RegionRemove : [nid=%d, name=%s], nRX=%d, nRZ=%d \n", m_sNid+NPC_BAND, m_strName, nOld_RX, nOld_RZ);
		}
	}

	return TRUE;
}

CNpc::CNpc()
{
	m_NpcState = NPC_LIVE;
	m_byGateOpen = GATE_CLOSE;
	m_byObjectType = NORMAL_OBJECT;
	m_byPathCount = 0;
	m_byAttackPos = 0;
	m_sThreadNumber = -1;
	InitTarget();

	m_ItemUserLevel = 0;
	m_Delay = 0;
	m_fDelayTime = TimeGet();

	m_tNpcAttType = ATROCITY_ATTACK_TYPE;		// aggression
	m_tNpcOldAttType = ATROCITY_ATTACK_TYPE;		// aggression
	m_tNpcLongType = 0;		// Far (1), Close (0)
	m_tNpcGroupType = 0;	// Help (1) or not (0)
	m_byNpcEndAttType = 1;
	m_byWhatAttackType = 0;
	m_byMoveType = 1;
	m_byInitMoveType  = 1;
	m_byRegenType = 0;
	m_byDungeonFamily = 0;
	m_bySpecialType = 0;
	m_byTrapNumber = 0;
	m_byChangeType = 0;
	m_byDeadType = 0;
	m_sChangeSid = 0;
	m_sControlSid = 0;
	m_sPathCount = 0;
	m_sMaxPathCount = 0;
	m_byMoneyType = 0;

	m_pPath = NULL;
	m_pOrgMap = NULL;


	m_bFirstLive = TRUE;

	m_fHPChangeTime = TimeGet();
	m_fFaintingTime = 0.0f;

	::ZeroMemory(m_pMap, sizeof(m_pMap));// Initialize it as a one-dimensional map.

	m_iRegion_X = 0;
	m_iRegion_Z = 0;
	m_nLimitMinX = m_nLimitMinZ = 0;
	m_nLimitMaxX = m_nLimitMaxZ = 0;
	m_lEventNpc = 0;
	m_fSecForRealMoveMetor = 0.0f;
	InitUserList();
	InitMagicValuable();

	m_pMain = (CServerDlg*)AfxGetMainWnd();
	m_MagicProcess.m_pMain = m_pMain;
	m_MagicProcess.m_pSrcNpc = this;

	for(int i=0; i<NPC_MAX_PATH_LIST; i++)	{
		m_PathList.pPattenPos[i].x = -1;
		m_PathList.pPattenPos[i].z = -1;
	}
	m_pPattenPos.x = m_pPattenPos.z = 0;

}

CNpc::~CNpc()
{
	ClearPathFindData();
	InitUserList();
}

///////////////////////////////////////////////////////////////////////
//	Clear route-finding data.
//
void CNpc::ClearPathFindData()
{
	::ZeroMemory(m_pMap, sizeof(m_pMap));	// for one-dimensional maps

	m_bPathFlag = FALSE;
	m_sStepCount = 0;
	m_iAniFrameCount = 0;
	m_iAniFrameIndex = 0;
	m_fAdd_x = 0.0f;	m_fAdd_z = 0.0f;

	for(int i=0; i<MAX_PATH_LINE; i++)
	{
		m_pPoint[i].byType = 0;
		m_pPoint[i].bySpeed = 0;
		m_pPoint[i].fXPos = -1.0f;
		m_pPoint[i].fZPos = -1.0f;
	}
}

/////////////////////////////////////////////////////////////////////////////
//	Initialize the user list.
//
void CNpc::InitUserList()
{
	m_sMaxDamageUserid = -1;
	m_TotalDamage = 0;
	for(int i = 0; i < NPC_HAVE_USER_LIST; i++)
	{
		m_DamagedUserList[i].bIs = FALSE;
		m_DamagedUserList[i].iUid = -1;
		m_DamagedUserList[i].nDamage = 0;
		::ZeroMemory(m_DamagedUserList[i].strUserID, sizeof(m_DamagedUserList[i].strUserID));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//	Initialize the target.
//
inline void CNpc::InitTarget()
{
	if(m_byAttackPos != 0)
	{
		if(m_Target.id >= 0 && m_Target.id < NPC_BAND)
		{
			CUser* pUser = m_pMain->GetUserPtr(m_Target.id);
			if(pUser != NULL)
			{
				if(m_byAttackPos > 0 && m_byAttackPos < 9)
					pUser->m_sSurroundNpcNumber[m_byAttackPos-1] = -1;
			}
		}
	}
	m_byAttackPos = 0;
	m_Target.id = -1;
	m_Target.x = 0.0;
	m_Target.y = 0.0;
	m_Target.z = 0.0;
	m_Target.failCount = 0;
}


//	NPC basic information reset
void CNpc::Init()
{
	if(m_ZoneIndex == -1)	m_ZoneIndex = m_pMain->GetZoneIndex(m_sCurZone);
	m_Delay = 0;
	m_fDelayTime = TimeGet();

	if(m_ZoneIndex < 0 || m_ZoneIndex >= m_pMain->g_arZone.size()) 
	{
		TRACE("#### Npc-Init ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return;
	}
	m_pOrgMap = m_pMain->g_arZone[m_ZoneIndex]->m_pMap;	// MapInfo information setting
}

//	NPC basic location information initialization (adjusts the formation of NPCs moving along the path..
void CNpc::InitPos()
{
	float fDD = 1.5f;
	if(m_byBattlePos == 0)	{
		m_fBattlePos_x = 0.0f;
		m_fBattlePos_z = 0.0f;
	}
	else if(m_byBattlePos == 1)	{
		float fx[5] = {0.0f, -(fDD*2),  -(fDD*2), -(fDD*4),  -(fDD*4)};
		float fz[5] = {0.0f,  (fDD*1),  -(fDD*1),  (fDD*1),  -(fDD*1)};
		m_fBattlePos_x = fx[m_byPathCount-1];
		m_fBattlePos_z = fz[m_byPathCount-1];
	}
	else if(m_byBattlePos == 2)	{
		float fx[5] = {0.0f,  0.0f, -(fDD*2), -(fDD*2), -(fDD*2)};
		float fz[5] = {0.0f, -(fDD*2), (fDD*1), (fDD*1), (fDD*3)};
		m_fBattlePos_x = fx[m_byPathCount-1];
		m_fBattlePos_z = fz[m_byPathCount-1];
	}
	else if(m_byBattlePos == 3)	{
		float fx[5] = {0.0f, -(fDD*2),  -(fDD*2), -(fDD*2), -(fDD*4)};
		float fz[5] = {0.0f,  (fDD*2),   0.0f,    -(fDD*2),  0.0f};
		m_fBattlePos_x = fx[m_byPathCount-1];
		m_fBattlePos_z = fz[m_byPathCount-1];
	}
}

void CNpc::InitMagicValuable()
{
	for(int i=0; i<MAX_MAGIC_TYPE4; i++)	{
		m_MagicType4[i].byAmount = 100;
		m_MagicType4[i].sDurationTime = 0;
		m_MagicType4[i].fStartTime = 0.0f;
	}

	for(int i=0; i<MAX_MAGIC_TYPE3; i++)	{
		m_MagicType3[i].sHPAttackUserID = -1;
		m_MagicType3[i].sHPAmount = 0;
		m_MagicType3[i].byHPDuration = 0;
		m_MagicType3[i].byHPInterval = 2;
		m_MagicType3[i].fStartTime = 0.0f;
	}
}

// Differentiate by NPC state.
void CNpc::NpcLive(CIOCPort* pIOCP)
{
	// Dungeon Work: In the case of a changing monster, it is treated as a change..
	if( m_byRegenType == 2 || (m_byRegenType==1 && m_byChangeType==100) )	{	// To prevent regen,,,
		m_NpcState = NPC_LIVE;
		m_Delay = m_sRegenTime;
		m_fDelayTime = TimeGet();
		return;
	}
	if( m_byChangeType == 1 )	{			// Changes the monster's information.
		m_byChangeType = 2;
		ChangeMonsterInfomation( 1 );
	}

	if(SetLive(pIOCP))	{
		m_NpcState = NPC_STANDING;
		m_Delay = m_sStandTime;
		m_fDelayTime = TimeGet();
	}
	else	{
		m_NpcState = NPC_LIVE;
		m_Delay = m_sStandTime;
		m_fDelayTime = TimeGet();
	}
}

/////////////////////////////////////////////////////////////////////////////
//	When an NPC attacks.
//
void CNpc::NpcFighting(CIOCPort* pIOCP)
{
	//if(cur_test)		NpcTrace(_T("NpcFighting()"));
	if(m_iHP <= 0)	{
		Dead(pIOCP);
		return;
	}
	
	m_Delay = Attack(pIOCP);
	m_fDelayTime = TimeGet();
}

/////////////////////////////////////////////////////////////////////////////
//	When an NPC is tracking a user.
//
void CNpc::NpcTracing(CIOCPort* pIOCP)
{
	char pBuf[1024];
	::ZeroMemory(pBuf, 1024);	
	int index = 0;
	float fMoveSpeed=0.0f;

	if(m_sStepCount != 0)	{
		if(m_fPrevX < 0 || m_fPrevZ < 0)	{
			TRACE("### Npc-NpcTracing  Fail : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fPrevX, m_fPrevZ);
		}
		else	{
			m_fCurX = m_fPrevX;		m_fCurZ = m_fPrevZ; 
		}
	}

	//if(cur_test)		NpcTrace(_T("NpcTracing()"));

	//  Fixed guards are not to be pursued.
	if(m_tNpcType == NPC_DOOR || m_tNpcType == NPC_ARTIFACT || m_tNpcType == NPC_PHOENIX_GATE || m_tNpcType == NPC_GATE_LEVER || m_tNpcType == NPC_DOMESTIC_ANIMAL || m_tNpcType == NPC_SPECIAL_GATE || m_tNpcType == NPC_DESTORY_ARTIFACT)
	{
		InitTarget();
		m_NpcState = NPC_STANDING;
		m_Delay = m_sStandTime;
		m_fDelayTime = TimeGet();
		return;
	}	

	/* will work
	In the case of dungeon monsters, a routine to check that they do not leave a certain area
	*/
	int nFlag = IsCloseTarget(m_byAttackRange, 1);
	if(nFlag == 1)						// Is it close enough for close combat?
	{
		//TRACE("Npc-NpcTracing : trace->attack으로 바뀜 : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ);
		NpcMoveEnd(pIOCP);	// End of move...
		m_NpcState = NPC_FIGHTING;
		m_Delay = 0;
		m_fDelayTime = TimeGet();
		return;
	}
	else if(nFlag == -1)		// 타겟이 없어짐...
	{
		InitTarget();
		NpcMoveEnd(pIOCP);	// End of move...
		m_NpcState = NPC_STANDING;
		m_Delay = m_sStandTime;
		m_fDelayTime = TimeGet();
		return;
	}
	//else if(nFlag == 2 && m_tNpcType == NPC_BOSS_MONSTER)	{
	else if(nFlag == 2 && m_tNpcLongType == 2)	{
		NpcMoveEnd(pIOCP);	// End of move...
		m_NpcState = NPC_FIGHTING;
		m_Delay = 0;
		m_fDelayTime = TimeGet();
		return;
	}

	if(m_byActionFlag == ATTACK_TO_TRACE)
	{
		m_byActionFlag = NO_ACTION;
		m_byResetFlag = 1;
	}

	if(m_byResetFlag == 1)
	{
		if(ResetPath() == FALSE)// && !m_tNpcTraceType)
		{
			TRACE("##### NpcTracing Fail: Passfind failed, with NPC_STANDING ######\n");
			InitTarget();
			NpcMoveEnd(pIOCP);	// End of move...
			m_NpcState = NPC_STANDING;
			m_Delay = m_sStandTime;
			m_fDelayTime = TimeGet();
			return;
		}
	}

	if(m_bPathFlag == FALSE)
	{
//		TRACE("StepMove : x=%.2f, z=%.2f\n", m_fCurX, m_fCurZ);
		if(StepMove(1, pIOCP) == FALSE)	// One square movement (walking motion, two squares when running)
		{
			m_NpcState = NPC_STANDING;
			m_Delay = m_sStandTime;
			m_fDelayTime = TimeGet();
			TRACE("### NpcTracing Fail : StepMove 실패, %s, %d ### \n", m_strName, m_sNid+NPC_BAND);
			return;
		}	
	}
	else if(m_bPathFlag == TRUE)
	{
//		TRACE("StepNoPathMove : x=%.2f, z=%.2f\n", m_fCurX, m_fCurZ);
		if(StepNoPathMove(1) == FALSE)	// One square movement (walking motion, two squares when running)
		{
			m_NpcState = NPC_STANDING;
			m_Delay = m_sStandTime;
			m_fDelayTime = TimeGet();
			TRACE("### NpcTracing Fail : StepNoPathMove 실패, %s, %d ### \n", m_strName, m_sNid+NPC_BAND);
			return;
		}	
	}

	if(IsMovingEnd())				// When the move is over
	{
		::ZeroMemory(pBuf, 1024);	index = 0;	
		SetByte(pBuf, MOVE_RESULT, index);
		SetByte(pBuf, SUCCESS, index);
		SetShort(pBuf, m_sNid+NPC_BAND, index);
		Setfloat(pBuf, m_fCurX, index);
		Setfloat(pBuf, m_fCurZ, index);
		Setfloat(pBuf, m_fCurY, index);
		Setfloat(pBuf, 0, index);
		//TRACE("Npc TRACE end --> nid = %d, cur=[x=%.2f, y=%.2f, metor=%d], prev=[x=%.2f, z=%.2f], frame=%d, speed = %d \n", m_sNid, m_fCurX, m_fCurZ, 0, m_fPrevX, m_fPrevZ, m_sStepCount, 0);
		SendAll(pIOCP, pBuf, index);   // thread 에서 send
	}	
	else
	{
		SetByte(pBuf, MOVE_RESULT, index);
		SetByte(pBuf, SUCCESS, index);
		SetShort(pBuf, m_sNid+NPC_BAND, index);
		Setfloat(pBuf, m_fPrevX, index);
		Setfloat(pBuf, m_fPrevZ, index);
		Setfloat(pBuf, m_fPrevY, index);
		fMoveSpeed = m_fSecForRealMoveMetor / ((double)m_sSpeed / 1000);
		Setfloat(pBuf, fMoveSpeed, index);
		//Setfloat(pBuf, m_fSecForRealMoveMetor, index);
		//TRACE("Npc Tracing --> nid = %d, cur=[x=%.2f, z=%.2f], prev=[x=%.2f, z=%.2f, metor = %.2f], frame=%d, speed = %d \n", m_sNid, m_fCurX, m_fCurZ, m_fPrevX, m_fPrevZ, m_fSecForRealMoveMetor, m_sStepCount, m_sSpeed);
		SendAll(pIOCP, pBuf, index);   // thread 에서 send
	}

	if(nFlag == 2 && m_tNpcLongType == 0 && m_tNpcType != NPC_HEALER)
	{
		// Trace Attack
		int nRet = TracingAttack(pIOCP);
		//TRACE("--> Npc-NpcTracing : TracingAttack : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ);
		if(nRet == 0)	{
			InitTarget();
			NpcMoveEnd(pIOCP);	// End of move...
			m_NpcState = NPC_STANDING;
			m_Delay = m_sStandTime;
			m_fDelayTime = TimeGet();
			return;
		}
	}	

	m_Delay = m_sSpeed;	
	m_fDelayTime = TimeGet();
}

void CNpc::NpcAttacking(CIOCPort* pIOCP)
{
	//if(cur_test)
		//NpcTrace(_T("NpcAttacking()"));

	if(m_iHP <= 0)	{	
		Dead(pIOCP);		 // Kill the monster right away.. (Experience not distributed)
		return;
	}

//	TRACE("Npc Attack - [nid=%d, sid=%d]\n", m_sNid, m_sSid);
	
	int ret = IsCloseTarget(m_byAttackRange);
	if(ret == 1)	{	// Is it close enough to attack?
		m_NpcState = NPC_FIGHTING;
		m_Delay = 0;
		m_fDelayTime = TimeGet();
		return;
	}

	//if(m_tNpcType == NPCTYPE_DOOR || m_tNpcType == NPCTYPE_ARTIFACT || m_tNpcType == NPCTYPE_PHOENIX_GATE || m_tNpcType == NPCTYPE_GATE_LEVER)		return;		// 성문 NPC는 공격처리 안하게

	// Task: In this part... the guards can also attack...
	if(m_tNpcType == NPC_DOOR || m_tNpcType == NPC_ARTIFACT || m_tNpcType == NPC_PHOENIX_GATE || m_tNpcType == NPC_GATE_LEVER || m_tNpcType == NPC_DOMESTIC_ANIMAL || m_tNpcType == NPC_SPECIAL_GATE || m_tNpcType == NPC_DESTORY_ARTIFACT)
	{
		m_NpcState = NPC_STANDING;
		m_Delay = m_sStandTime/2;
		m_fDelayTime = TimeGet();
		return;
	}

	int nValue = GetTargetPath();
	if(nValue == -1)	{	// As the target disappears or becomes distant...
		if(RandomMove() == FALSE)	{
			InitTarget();
			m_NpcState = NPC_STANDING;
			m_Delay = m_sStandTime;
			m_fDelayTime = TimeGet();
			return;
		}

		InitTarget();
		m_NpcState = NPC_MOVING;
		m_Delay = m_sSpeed;
		m_fDelayTime = TimeGet();
		return;
	}
	else if(nValue == 0)	{
		m_fSecForMetor = m_fSpeed_2;			// When attacking, run at speed...
		IsNoPathFind(m_fSecForMetor);			// Go straight to the target.
	}

	m_NpcState = NPC_TRACING;
	m_Delay = 0;
	m_fDelayTime = TimeGet();
}

/////////////////////////////////////////////////////////////////////////////
//	When an NPC moves.
//
void CNpc::NpcMoving(CIOCPort* pIOCP)
{
	//if(cur_test)	NpcTrace(_T("NpcMoving()"));
	char pBuf[1024];
	::ZeroMemory(pBuf, 1024);	
	int index = 0;
	float fMoveSpeed = 0.0f;

	if(m_iHP <= 0) {
		Dead(pIOCP);
		return;
	}

	if(m_sStepCount != 0)	{
		if(m_fPrevX < 0 || m_fPrevZ < 0)	{
			TRACE("### Npc-Moving Fail : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fPrevX, m_fPrevZ);
		}
		else	{
			m_fCurX = m_fPrevX;		m_fCurZ = m_fPrevZ; 
		}
	}

	if(FindEnemy() == TRUE)	{	// find the enemy 
	/*	if(m_tNpcType == NPCTYPE_GUARD) 
		{ 
			NpcMoveEnd(pIOCP);	// End of move...
			m_NpcState = NPC_FIGHTING; 
			m_Delay = 0; 
			m_fDelayTime = TimeGet();
		}
		else */
		{ 
			NpcMoveEnd(pIOCP);	// End of move...
			m_NpcState = NPC_ATTACKING;
			m_Delay = m_sSpeed;
			m_fDelayTime = TimeGet();
		}
		return;
	}	

	if(IsMovingEnd())	{				// When the move is over
		m_fCurX = m_fPrevX;		m_fCurZ = m_fPrevZ; 
		if(m_fCurX < 0 || m_fCurZ < 0)	{
			TRACE("Npc-NpcMoving-2 : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ);
		}

		int rx=m_fCurX / VIEW_DIST;
		int rz=m_fCurZ / VIEW_DIST;
		//TRACE("** NpcMoving --> IsMovingEnd() 이동이 끝남,, rx=%d, rz=%d, stand로\n", rx, rz);
		m_NpcState = NPC_STANDING;
		m_Delay = m_sStandTime;
		m_fDelayTime = TimeGet();

		if(m_Delay < 0) {
			m_Delay = 0;	
			m_fDelayTime = TimeGet();
		}

		return;
	}

	if(m_bPathFlag == FALSE)	{
		if(StepMove(1, pIOCP) == FALSE)	{	// One square movement (walking motion, two squares when running)
			m_NpcState = NPC_STANDING;
			m_Delay = m_sStandTime;
			m_fDelayTime = TimeGet();
			return;
		}	
	}
	else if(m_bPathFlag == TRUE)	{
		if(StepNoPathMove(1) == FALSE)	{	// One square movement (walking motion, two squares when running)
			m_NpcState = NPC_STANDING;
			m_Delay = m_sStandTime;
			m_fDelayTime = TimeGet();
			return;
		}	
	}

	if(IsMovingEnd())	{				// When the move is over
		::ZeroMemory(pBuf, 1024);	index = 0;	
		SetByte(pBuf, MOVE_RESULT, index);
		SetByte(pBuf, SUCCESS, index);
		SetShort(pBuf, m_sNid+NPC_BAND, index);
		Setfloat(pBuf, m_fPrevX, index);
		Setfloat(pBuf, m_fPrevZ, index);
		Setfloat(pBuf, m_fPrevY, index);
		Setfloat(pBuf, 0, index);
		//TRACE("Npc Move end --> nid = %d, cur=[x=%.2f, y=%.2f, metor=%d], prev=[x=%.2f, z=%.2f], frame=%d, speed = %d \n", m_sNid+NPC_BAND, m_fCurX, m_fCurZ, 0, m_fPrevX, m_fPrevZ, m_sStepCount, 0);
		SendAll(pIOCP, pBuf, index);   // send in thread
	}
	else	{
		SetByte(pBuf, MOVE_RESULT, index);
		SetByte(pBuf, SUCCESS, index);
		SetShort(pBuf, m_sNid+NPC_BAND, index);
		Setfloat(pBuf, m_fPrevX, index);
		Setfloat(pBuf, m_fPrevZ, index);
		Setfloat(pBuf, m_fPrevY, index);
		fMoveSpeed = m_fSecForRealMoveMetor / ((double)m_sSpeed / 1000);
		Setfloat(pBuf, fMoveSpeed, index);
		//Setfloat(pBuf, m_fSecForRealMoveMetor, index);
		//TRACE("Npc Move --> nid = %d, cur=[x=%.2f, z=%.2f], prev=[x=%.2f, z=%.2f, metor = %.2f], frame=%d, speed = %d \n", m_sNid+NPC_BAND, m_fCurX, m_fCurZ, m_fPrevX, m_fPrevZ, m_fSecForRealMoveMetor, m_sStepCount, m_sSpeed);
		SendAll(pIOCP, pBuf, index);   // send in thread
	}

	m_Delay = m_sSpeed;	
	m_fDelayTime = TimeGet();
}

/////////////////////////////////////////////////////////////////////////////
//	When an NPC is standing.
//
void CNpc::NpcStanding()
{
	//if(cur_test)		NpcTrace(_T("NpcStanding()"));

	char send_buff[128];
	int send_index = 0;

/*	if(m_pMain->m_byNight == 2)	{	// at night
		m_NpcState = NPC_SLEEPING;
		m_Delay = 0;
		m_fDelayTime = TimeGet();
		return;
	}	*/

	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(pMap == NULL)	{
		TRACE("### NpcStanding Zone Index Error : nid=%d, name=%s, zoneindex=%d ###\n", m_sNid+NPC_BAND, m_strName, m_ZoneIndex);
		return;
	}

/*	BOOL bCheckRange = FALSE;
	bCheckRange = IsInRange( (int)m_fCurX, (int)m_fCurZ);
	if( bCheckRange )	{	// If you are in the active area
		if( m_tNpcAttType != m_tNpcOldAttType )	{
			m_tNpcAttType = ATROCITY_ATTACK_TYPE;	// with aggression
			//TRACE("Aggressiveness changed to first attack\n");
		}
	}
	else	{
		if( m_tNpcAttType == ATROCITY_ATTACK_TYPE )	{
			m_tNpcAttType = TENDER_ATTACK_TYPE;
			//TRACE("Attack tendency changed to back attack\n");
		}
	}	*/

	// dungeon work
	// Determine whether it is a dungeon zone first
	CRoomEvent* pRoom = NULL;
	pRoom = pMap->m_arRoomEventArray.GetData( m_byDungeonFamily );
	if( pRoom )	{
		if( pRoom->m_byStatus == 1 )	{	// If the room state is not executed, the monster is standing
			m_NpcState = NPC_STANDING;
			m_Delay = m_sStandTime;
			m_fDelayTime = TimeGet();
			return;
		}
	}

	if(RandomMove() == TRUE)	{
		m_iAniFrameCount = 0;
		m_NpcState = NPC_MOVING;
		m_Delay = m_sStandTime;
		m_fDelayTime = TimeGet();
		return;
	}	

	m_NpcState = NPC_STANDING;
	m_Delay = m_sStandTime;
	m_fDelayTime = TimeGet();
	
	if( m_tNpcType == NPC_SPECIAL_GATE && m_pMain->m_byBattleEvent == BATTLEZONE_OPEN )	{
		// So that the door opens and closes automatically (using standing time at 2-minute intervals)
		m_byGateOpen = !m_byGateOpen;		// 
		// Send information to client and gameserver
		memset( send_buff, NULL, 128 );		send_index = 0;
		SetByte( send_buff, AG_NPC_GATE_OPEN, send_index );
		SetShort( send_buff, m_sNid+NPC_BAND, send_index );
		SetByte( send_buff, m_byGateOpen, send_index );
		SendAll( &m_pMain->m_Iocport, send_buff, send_index);
	}
}

/////////////////////////////////////////////////////////////////////////////
//	Maintain the distance to the target within the range of range (cell unit)
//
void CNpc::NpcBack(CIOCPort* pIOCP)
{
	if(m_Target.id >= 0 && m_Target.id < NPC_BAND)	{
		if(m_pMain->GetUserPtr((m_Target.id - USER_BAND)) == NULL)	{	// Check if Target User exists
			m_NpcState = NPC_STANDING;
			m_Delay = m_sSpeed;//STEP_DELAY;
			m_fDelayTime = TimeGet();
			return;
		}
	}
	else if(m_Target.id >= NPC_BAND && m_Target.id < INVALID_BAND)	{
		if(m_pMain->m_arNpc.GetData(m_Target.id-NPC_BAND) == NULL)	{
			m_NpcState = NPC_STANDING;
			m_Delay = m_sSpeed;
			m_fDelayTime = TimeGet();
			return;
		}
	}

	char pBuf[1024];
	::ZeroMemory(pBuf, 1024);	
	int index = 0;

	if(m_iHP <= 0)	{
		Dead(pIOCP);
		return;
	}

	if(m_sStepCount != 0)	{
		if(m_fPrevX < 0 || m_fPrevZ < 0)	{
			TRACE("### Npc-NpcBack Fail-1 : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fPrevX, m_fPrevZ);
		}
		else	{
			m_fCurX = m_fPrevX;		m_fCurZ = m_fPrevZ; 
		}
	}
	
	if(IsMovingEnd())	{				// If the movement is over
		m_fCurX = m_fPrevX;		m_fCurZ = m_fPrevZ; 
		if(m_fCurX < 0 || m_fCurZ < 0)
			TRACE("Npc-NpcBack-2 : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ);

		::ZeroMemory(pBuf, 1024);	index = 0;	
		SetByte(pBuf, MOVE_RESULT, index);
		SetByte(pBuf, SUCCESS, index);
		SetShort(pBuf, m_sNid+NPC_BAND, index);
		Setfloat(pBuf, m_fCurX, index);
		Setfloat(pBuf, m_fCurZ, index);
		Setfloat(pBuf, m_fCurY, index);
		Setfloat(pBuf, 0, index);
//		TRACE("NpcBack end --> nid = %d, cur=[x=%.2f, y=%.2f, metor=%d], prev=[x=%.2f, z=%.2f], frame=%d, speed = %d \n", m_sNid, m_fCurX, m_fCurZ, 0, m_fPrevX, m_fPrevZ, m_sStepCount, 0);
		SendAll(pIOCP, pBuf, index);   // thread 에서 send

//		TRACE("** End of NpcBack movement, to stand\n");
		m_NpcState = NPC_STANDING;

		//Shorten standing time if out of range...
		m_Delay = m_sStandTime;
		m_fDelayTime = TimeGet();

		if(m_Delay < 0)	{
			m_Delay = 0;	
			m_fDelayTime = TimeGet();
		}

		return;
	}

	if(m_bPathFlag == FALSE)	{
		if(StepMove(1, pIOCP) == FALSE)	{
			m_NpcState = NPC_STANDING;
			m_Delay = m_sStandTime;
			m_fDelayTime = TimeGet();
			return;
		}	
	}
	else if(m_bPathFlag == TRUE)	{
		if(StepNoPathMove(1) == FALSE)	{
			m_NpcState = NPC_STANDING;
			m_Delay = m_sStandTime;
			m_fDelayTime = TimeGet();
			return;
		}	
	}

	float fMoveSpeed = 0.0f;

	SetByte(pBuf, MOVE_RESULT, index);
	SetByte(pBuf, SUCCESS, index);
	SetShort(pBuf, m_sNid+NPC_BAND, index);
	Setfloat(pBuf, m_fPrevX, index);
	Setfloat(pBuf, m_fPrevZ, index);
	Setfloat(pBuf, m_fPrevY, index);
	fMoveSpeed = m_fSecForRealMoveMetor / ((double)m_sSpeed / 1000);
	Setfloat(pBuf, fMoveSpeed, index);
	//Setfloat(pBuf, m_fSecForRealMoveMetor, index);

//	TRACE("NpcBack --> nid = %d, cur=[x=%.2f, z=%.2f], prev=[x=%.2f, z=%.2f, metor = %.2f], frame=%d, speed = %d \n", m_sNid, m_fCurX, m_fCurZ, m_fPrevX, m_fPrevZ, m_fSecForRealMoveMetor, m_sStepCount, m_sSpeed);
	SendAll(pIOCP, pBuf, index);   // send from thread

	m_Delay = m_sSpeed;//STEP_DELAY;	*/
	m_fDelayTime = TimeGet();
}

///////////////////////////////////////////////////////////////////////
// Handle when an NPC first appears or dies and comes back to life
//
BOOL CNpc::SetLive(CIOCPort* pIOCP)
{
	//TRACE("**** Npc SetLive ***********\n");
	// NPC's HP, PP reset ----------------------//	
	int i = 0, j = 0;
	m_iHP = m_iMaxHP;
	m_sMP = m_sMaxMP;
	m_sPathCount = 0;
	m_iPattenFrame = 0;
	m_byResetFlag = 0;
	m_byActionFlag = NO_ACTION;
	m_byMaxDamagedNation = KARUS_MAN;

	m_iRegion_X = -1;	m_iRegion_Z = -1;
	m_fAdd_x = 0.0f;	m_fAdd_z = 0.0f;
	m_fStartPoint_X = 0.0f;		m_fStartPoint_Y = 0.0f; 
	m_fEndPoint_X = 0.0f;		m_fEndPoint_Y = 0.0f;
	m_min_x = m_min_y = m_max_x = m_max_y = 0;

	InitTarget();
	ClearPathFindData();
	InitUserList();					//Initialize the list for the target.
	//InitPos();

	CNpc* pNpc = NULL;

	/* If the Event Monster comes back to life, kill the Event Monster. Set the pointer to NULL in the event thread as well */
	if(m_lEventNpc == 1 && !m_bFirstLive)	{
		for(int i = 0; i < NPC_NUM; i++)	{
			pNpc = m_pMain->m_arEventNpcThread[0]->m_ThreadInfo.pNpc[i];
			if(m_pMain->m_arEventNpcThread[0]->m_ThreadInfo.pNpc[i] != NULL)	{
				if(m_pMain->m_arEventNpcThread[0]->m_ThreadInfo.pNpc[i]->m_sNid == m_sNid)	{
					m_pMain->m_arEventNpcThread[0]->m_ThreadInfo.m_byNpcUsed[i] = 0;
					m_lEventNpc = 0;
					m_pMain->m_arEventNpcThread[0]->m_ThreadInfo.pNpc[i] = NULL;
					TRACE("Return summon monster pointer,, thread index=%d, nid=%d\n", i, m_sNid+NPC_BAND);
					return TRUE;
				}
			}
		}
		return TRUE;
	}

	// Determining the initial NPC location ------------------------//
	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size())	{
		TRACE("#### Npc-SetLive ZoneIndex Fail : [nid=%d,sid=%d,name=%s], th_num=%d, zoneindex=%d #####\n", m_sNid+NPC_BAND, m_sSid, m_strName, m_sThreadNumber, m_ZoneIndex);
		return FALSE;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(pMap == NULL)	return FALSE;

	if(m_bFirstLive)	{	// If NPC comes alive for the first time	
		m_nInitX = m_fPrevX = m_fCurX;
		m_nInitY = m_fPrevY = m_fCurY;
		m_nInitZ = m_fPrevZ = m_fCurZ;
	}

	if(m_fCurX < 0 || m_fCurZ < 0)	{
		TRACE("Npc-SetLive-1 : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ);
	}

	int dest_x = (int)m_nInitX / TILE_SIZE;
	int dest_z = (int)m_nInitZ / TILE_SIZE;

	BOOL bMove = pMap->IsMovable(dest_x, dest_z);

	if(m_tNpcType != NPCTYPE_MONSTER || m_lEventNpc == 1)	{
		m_fCurX = m_fPrevX = m_nInitX;
		m_fCurY = m_fPrevY = m_nInitY;
		m_fCurZ = m_fPrevZ = m_nInitZ;
	}
	else	{
		int nX = 0;
		int nZ = 0;
		int nTileX = 0;
		int nTileZ = 0;
		int nRandom = 0;

		while(1)	{
			i++;
			nRandom = abs(m_nInitMinX - m_nInitMaxX);
			if(nRandom <= 1)	nX = m_nInitMinX;
			else	{
				if(m_nInitMinX < m_nInitMaxX)
					nX = myrand(m_nInitMinX, m_nInitMaxX);
				else
					nX = myrand(m_nInitMaxX, m_nInitMinX);
			}
			nRandom = abs(m_nInitMinY - m_nInitMaxY);
			if(nRandom <= 1)	nZ = m_nInitMinY;
			else	{
				if(m_nInitMinY < m_nInitMaxY)
					nZ = myrand(m_nInitMinY, m_nInitMaxY);
				else
					nZ = myrand(m_nInitMaxY, m_nInitMinY);
			}

			nTileX = nX / TILE_SIZE;
			nTileZ = nZ / TILE_SIZE;

			if(nTileX >= (pMap->m_sizeMap.cx-1))
				nTileX = pMap->m_sizeMap.cx-1;
			if(nTileZ >= (pMap->m_sizeMap.cy-1))
				nTileZ = pMap->m_sizeMap.cy-1;

			if(nTileX < 0 || nTileZ < 0)	{
				TRACE("#### Npc-SetLive() Fail : nTileX=%d, nTileZ=%d #####\n", nTileX, nTileZ);
				return FALSE;
			}

			if(pMap->m_pMap[nTileX][nTileZ].m_sEvent <= 0)	{
				if(i >= 500)	{
					m_nInitX = m_fPrevX = m_fCurX;
					m_nInitY = m_fPrevY = m_fCurY;
					m_nInitZ = m_fPrevZ = m_fCurZ;
					//TRACE("### fail : sid = %d, nid = %d, zone=%d, loop = %d 나 설자리가 이상해... 고쳐줘... x = %d, y = %d\n", m_sSid, m_sNid+NPC_BAND, m_sCurZone, i, nX, nZ);
					return FALSE;
			
				}
				continue;
			}

			m_nInitX = m_fPrevX = m_fCurX = (float)nX;
			m_nInitZ = m_fPrevZ = m_fCurZ = (float)nZ;

			if(m_fCurX < 0 || m_fCurZ < 0)	{
				TRACE("Npc-SetLive-2 : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ);
			}

			break;
		}	
	}

	//SetUid(m_fCurX, m_fCurZ, m_sNid + NPC_BAND);

	// Status information reset
	m_fHPChangeTime = TimeGet();
	m_fFaintingTime = 0.0f;
	InitMagicValuable();

	if(m_bFirstLive)	{	// When the NPC comes alive for the first time
		NpcTypeParser();
		m_bFirstLive = FALSE;

		InterlockedIncrement(&m_pMain->m_CurrentNPC);

		//TRACE("Npc - SerLive :  cur = %d\n", m_pMain->m_CurrentNPC);
		if(m_pMain->m_TotalNPC == m_pMain->m_CurrentNPC)	// If the total number of monsters and the number of initialized monsters are the same
		{
			CString logstr;
			logstr.Format("Monster All Init Success - %d", m_pMain->m_CurrentNPC);
			m_pMain->m_StatusList.AddString( logstr );
			TRACE("Npc - SerLive : GameServerAcceptThread, cur = %d\n", m_pMain->m_CurrentNPC);
			m_pMain->GameServerAcceptThread();				// Game Server Accept
		}
		//TRACE("Npc - SerLive : CurrentNpc = %d\n", m_pMain->m_CurrentNPC);
	}
	
	// NPC's initial looking direction,
	int degree = 0;
	degree = myrand( 0, 360 );
	m_fDir = D3DXToRadian( degree );
	// To do: determine the NPC's initial direction,,..
	if(m_byMoveType == 3 && m_sMaxPathCount == 2)	// For NPCs, the initial direction is important.
	{
		__Vector3 vS, vE, vDir;
		vS.Set((float)m_PathList.pPattenPos[0].x, 0, (float)m_PathList.pPattenPos[0].z);
		vE.Set((float)m_PathList.pPattenPos[1].x, 0, (float)m_PathList.pPattenPos[1].z);
		vDir = vE - vS;
		vDir.Normalize();
		Yaw2D(vDir.x, vDir.z, m_fDir);	
	}

	if( m_bySpecialType == 5 && m_byChangeType == 0)	{			// Monsters that die first and then come back to life
		return FALSE;
	}
	else if( m_bySpecialType == 5 && m_byChangeType == 3)	{		// appearance of monsters,
	//else if( m_byChangeType == 3)	{		// appearance of monsters,
		char notify[50];	memset(notify, 0x00, 50);
		//wsprintf( notify, "** Notice: %s monster has appeared **", m_strName);
		//m_pMain->SendSystemMsg( notify, m_sCurZone, PUBLIC_CHAT, SEND_ALL);
	}

	SetUid(m_fCurX, m_fCurZ, m_sNid + NPC_BAND);
	m_byDeadType = 0;
	CTime t = CTime::GetCurrentTime();
	TRACE("NPC Init(nid=%d, sid=%d, th_num=%d, name=%s) - %.2f %.2f, gate = %d, m_byDeadType=%d, time=%d:%d-%d\n", m_sNid+NPC_BAND, m_sSid, m_sThreadNumber, m_strName, m_fCurX, m_fCurZ, m_byGateOpen, m_byDeadType, t.GetHour(), t.GetMinute(), t.GetSecond());						

	int modify_index = 0;
	char modify_send[2048];		::ZeroMemory(modify_send, sizeof(modify_send));

	FillNpcInfo(modify_send, modify_index, INFO_MODIFY);
	SendAll(pIOCP, modify_send, modify_index);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	If there are no nearby enemies or passive mobs, find your way to a random point and then move.
//
BOOL CNpc::RandomMove()
{
	// When moving normally, it adjusts to walking speed...
	m_fSecForMetor = m_fSpeed_1;

	if(m_bySearchRange == 0) return FALSE;

	if(m_byMoveType == 0)	return FALSE;	// NPC standing in place
		
	/* Check the area here, and if there is no user within the visual range of the npc, it will be in a standing state...
	If there is, process it to act according to the pattern or node... */
	if(!GetUserInView())		return FALSE;

	float fDestX = -1.0f, fDestZ = -1.0f;
	if(m_ZoneIndex < 0)		return FALSE;
	int max_xx = m_pMain->g_arZone[m_ZoneIndex]->m_sizeMap.cx;
	int max_zz = m_pMain->g_arZone[m_ZoneIndex]->m_sizeMap.cy;
	int x = 0, y = 0;

	__Vector3 vStart, vEnd, vNewPos;
	float fDis = 0.0f;
 
	int nPathCount = 0;

	int random_x = 0, random_z = 0;
	BOOL bPeedBack = FALSE;

	if(m_byMoveType == 1)	{// Randomly moving NPCs
		bPeedBack = IsInRange( (int)m_fCurX, (int)m_fCurZ);
		if( bPeedBack == FALSE )	{	
			//TRACE("out of initial position,,,  patten=%d \n", m_iPattenFrame);
		}

		if(m_iPattenFrame == 0)		{		// To return to the first position...
			m_pPattenPos.x = (short)m_nInitX;
			m_pPattenPos.z = (short)m_nInitZ;
		}

		random_x = myrand(3, 7);
		random_z = myrand(3, 7);

		fDestX = m_fCurX + (float)random_x;
		fDestZ = m_fCurZ + (float)random_z;

		if(m_iPattenFrame == 2)	{
			fDestX = m_pPattenPos.x;
			fDestZ = m_pPattenPos.z;
			m_iPattenFrame = 0;
		}
		else	{
			m_iPattenFrame++;
		}

		vStart.Set(m_fCurX, m_fCurY, m_fCurZ);
		vEnd.Set(fDestX, 0, fDestZ);
		fDis = GetDistance(vStart, vEnd);
		if(fDis > 50)	{	// If the initial effective distance is out of 50m
			vNewPos = GetVectorPosition(vStart, vEnd, 40);
			fDestX = vNewPos.x;
			fDestZ = vNewPos.z;
			m_iPattenFrame = 2;
			bPeedBack = TRUE;
			//TRACE("&&& RandomMove departure from initial position.. %d,%s ==> x=%.2f, z=%.2f,, init_x=%.2f, init_z=%.2f\n", m_sNid+NPC_BAND, m_strName, fDestX, fDestZ, m_nInitX, m_nInitZ); 
		}

//		TRACE("&&& RandomMove ==> x=%.2f, z=%.2f,, dis=%.2f, patten = %d\n", fDestX, fDestZ, fDis, m_iPattenFrame); 
	}
	else if(m_byMoveType == 2)  {  // PathLine을 따라서 움직이는 NPC	
		if(m_sPathCount == m_sMaxPathCount)		m_sPathCount = 0;

		// If my position is far from the path list, the position of the current m_sPathCount or the next m_sPathCount
		// Determine and find your way to a nearby location,
		if(m_sPathCount != 0 && IsInPathRange() == FALSE)	{
			m_sPathCount--;
			nPathCount = GetNearPathPoint();

			// If the npc is moved too far away to be able to move,, kill the npc and bring it back to life..
			// Either warp the npc to its initial location..
			if(nPathCount  == -1)	{
				TRACE("##### RandomMove Fail : [nid = %d, sid=%d], path = %d/%d, 이동할 수 있는 거리에서 너무 멀어졌당,, 어케해 #####\n", m_sNid+NPC_BAND, m_sSid, m_sPathCount, m_sMaxPathCount);
				// It is processed to move 40m in the direction of position 0 unconditionally..
				vStart.Set(m_fCurX, m_fCurY, m_fCurZ);
				fDestX = (float)m_PathList.pPattenPos[0].x + m_fBattlePos_x;
				fDestZ = (float)m_PathList.pPattenPos[0].z + m_fBattlePos_z;
				vEnd.Set(fDestX, 0, fDestZ);
				vNewPos = GetVectorPosition(vStart, vEnd, 40);
				fDestX = vNewPos.x;
				fDestZ = vNewPos.z;
				//m_sPathCount++;
				//return FALSE;	// Now standing...
			}
			else	{
				//m_byPathCount; add the number
				if(nPathCount < 0)	return FALSE;
				fDestX = (float)m_PathList.pPattenPos[nPathCount].x + m_fBattlePos_x;
				fDestZ = (float)m_PathList.pPattenPos[nPathCount].z + m_fBattlePos_z;
				m_sPathCount = nPathCount;
			}
		}
		else	{
			if(m_sPathCount < 0)	return FALSE;
			fDestX = (float)m_PathList.pPattenPos[m_sPathCount].x + m_fBattlePos_x;
			fDestZ = (float)m_PathList.pPattenPos[m_sPathCount].z + m_fBattlePos_z;
		}

		//TRACE("RandomMove move along the way : [nid=%d, sid=%d, name=%s], path=%d/%d, nPathCount=%d, curx=%.2f, z=%.2f -> dest_X=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_sSid, m_strName, m_sPathCount, m_sMaxPathCount, nPathCount, m_fCurX, m_fCurZ, fDestX, fDestZ);
		m_sPathCount++;
	}
	else if(m_byMoveType == 3)	{	// NPC moving along PathLine
		if(m_sPathCount == m_sMaxPathCount)	{
			m_byMoveType = 0;
			m_sPathCount = 0;
			return FALSE;
		}

		// If my position is far from the path list, the position of the current m_sPathCount or the next m_sPathCount
		// Determine and find your way to a nearby location,
		if(m_sPathCount != 0 && IsInPathRange() == FALSE)	{
			m_sPathCount--;
			nPathCount = GetNearPathPoint();

			// If the npc is moved too far away to be able to move,, kill the npc and bring it back to life..
			// Warp the npc to the initial location..
			if(nPathCount  == -1)	{
				// It is processed to move 40m in the direction of position 0 unconditionally..
				TRACE("##### RandomMove Fail : [nid = %d, sid=%d], path = %d/%d, It's too far from the distance I can move,, what can I do? #####\n", m_sNid+NPC_BAND, m_sSid, m_sPathCount, m_sMaxPathCount);
				vStart.Set(m_fCurX, m_fCurY, m_fCurZ);
				fDestX = (float)m_PathList.pPattenPos[0].x + m_fBattlePos_x;
				fDestZ = (float)m_PathList.pPattenPos[0].z + m_fBattlePos_z;
				vEnd.Set(fDestX, 0, fDestZ);
				vNewPos = GetVectorPosition(vStart, vEnd, 40);
				fDestX = vNewPos.x;
				fDestZ = vNewPos.z;
				//return FALSE;	// Now standing...
			}
			else	{
				if(nPathCount < 0)	return FALSE;
				fDestX = (float)m_PathList.pPattenPos[nPathCount].x + m_fBattlePos_x;
				fDestZ = (float)m_PathList.pPattenPos[nPathCount].z + m_fBattlePos_x;
				m_sPathCount = nPathCount;
			}
		}
		else	{
			if(m_sPathCount < 0)	return FALSE;
			fDestX = (float)m_PathList.pPattenPos[m_sPathCount].x + m_fBattlePos_x;
			fDestZ = (float)m_PathList.pPattenPos[m_sPathCount].z + m_fBattlePos_x;
		}

		m_sPathCount++;
	}

	vStart.Set(m_fCurX, 0, m_fCurZ);
	vEnd.Set(fDestX, 0, fDestZ);

	if(m_fCurX < 0 || m_fCurZ < 0 || fDestX < 0 || fDestZ < 0)	{
		TRACE("##### RandomMove Fail : value is negative.. [nid = %d, name=%s], cur_x=%.2f, z=%.2f, dest_x=%.2f, dest_z=%.2f#####\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ, fDestX, fDestZ);
		return FALSE;
	}

	int mapWidth = ( max_xx - 1) * m_pMain->g_arZone[m_ZoneIndex]->m_fUnitDist;

	if(m_fCurX > mapWidth || m_fCurZ > mapWidth || fDestX > mapWidth || fDestZ > mapWidth)	{
		TRACE("##### RandomMove Fail : value is overflow .. [nid = %d, name=%s], cur_x=%.2f, z=%.2f, dest_x=%.2f, dest_z=%.2f#####\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ, fDestX, fDestZ);
		return FALSE;
	}

	// To work on: Routine to check that dungeon monsters do not leave a certain area
    if ( m_tNpcType == NPC_DUNGEON_MONSTER )	{
		if( IsInRange( (int)fDestX, (int)fDestZ ) == FALSE)
			return FALSE;	
    }

	fDis = GetDistance(vStart, vEnd);
	if(fDis > NPC_MAX_MOVE_RANGE)	{						// If it is wider than 100 meters, it is in a standing state.
		if(m_byMoveType == 2 || m_byMoveType == 3)	{
			m_sPathCount--;
			if(m_sPathCount <= 0) m_sPathCount=0;
		}
		TRACE("##### RandomMove Fail : NPC_MAX_MOVE_RANGE overflow  .. [nid = %d, name=%s], cur_x=%.2f, z=%.2f, dest_x=%.2f, dest_z=%.2f, fDis=%.2f#####\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ, fDestX, fDestZ, fDis);
		return FALSE;
	}

	if(fDis <= m_fSecForMetor)		{		// If the target point is within the movement distance, it will move immediately...
		ClearPathFindData();
		m_fStartPoint_X = m_fCurX;		m_fStartPoint_Y = m_fCurZ;
		m_fEndPoint_X = fDestX;			m_fEndPoint_Y = fDestZ;
		m_bPathFlag = TRUE;
		m_iAniFrameIndex = 1;
		m_pPoint[0].fXPos = m_fEndPoint_X;
		m_pPoint[0].fZPos = m_fEndPoint_Y;
		//TRACE("** Npc Random Direct Move  : [nid = %d], fDis <= %d, %.2f **\n", m_sNid, m_fSecForMetor, fDis);
		return TRUE;
	}

	float fTempRange = (float)fDis+2;				// correct temporarily.
	int min_x = (int)(m_fCurX - fTempRange)/TILE_SIZE;	if(min_x < 0) min_x = 0;
	int min_z = (int)(m_fCurZ - fTempRange)/TILE_SIZE;	if(min_z < 0) min_z = 0;
	int max_x = (int)(m_fCurX + fTempRange)/TILE_SIZE;	if(max_x >= max_xx) max_x = max_xx - 1;
	int max_z = (int)(m_fCurZ + fTempRange)/TILE_SIZE;	if(min_z >= max_zz) min_z = max_zz - 1;

	CPoint start, end;
	start.x = (int)(m_fCurX/TILE_SIZE) - min_x;
	start.y = (int)(m_fCurZ/TILE_SIZE) - min_z;
	end.x = (int)(fDestX/TILE_SIZE) - min_x;
	end.y = (int)(fDestZ/TILE_SIZE) - min_z;

	if(start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)	return FALSE;

	m_fStartPoint_X = m_fCurX;		m_fStartPoint_Y = m_fCurZ;
	m_fEndPoint_X = fDestX;	m_fEndPoint_Y = fDestZ;

	m_min_x = min_x;
	m_min_y = min_z;
	m_max_x = max_x;
	m_max_y = max_z;

	// Monsters or NPCs following the path move directly to the real coordinates without path finding.
	if(m_byMoveType == 2 || m_byMoveType == 3 || bPeedBack == TRUE) 	{
		IsNoPathFind(m_fSecForMetor);
		return TRUE;
	}

	int nValue = PathFind(start, end, m_fSecForMetor);
	if(nValue == 1)		return TRUE;

	return FALSE;
}

// It moves randomly in the opposite direction to the target user.
BOOL CNpc::RandomBackMove()
{
	m_fSecForMetor = m_fSpeed_2;		// Even when running away.. Match the speed to running speed..

	if(m_bySearchRange == 0) return FALSE;
	
	float fDestX = -1.0f, fDestZ = -1.0f;
	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size()) 
	{
		TRACE("#### Npc-RandomBackMove ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return FALSE;
	}

	int max_xx = m_pMain->g_arZone[m_ZoneIndex]->m_sizeMap.cx;
	int max_zz = m_pMain->g_arZone[m_ZoneIndex]->m_sizeMap.cy;
	int x = 0, y = 0;
	float fTempRange = (float)m_bySearchRange*2;				// correct temporarily.
	int min_x = (int)(m_fCurX - fTempRange)/TILE_SIZE;	if(min_x < 0) min_x = 0;
	int min_z = (int)(m_fCurZ - fTempRange)/TILE_SIZE;	if(min_z < 0) min_z = 0;
	int max_x = (int)(m_fCurX + fTempRange)/TILE_SIZE;	if(max_x >= max_xx) max_x = max_xx - 1;
	int max_z = (int)(m_fCurZ + fTempRange)/TILE_SIZE;	if(max_z >= max_zz) max_z = max_zz - 1;

	__Vector3 vStart, vEnd, vEnd22;
	float fDis = 0.0f;
	vStart.Set(m_fCurX, m_fCurY, m_fCurZ);

	int nID = m_Target.id;					// Find the Target.
	CUser* pUser = NULL;

	int iDir = 0;

	int iRandomX = 0, iRandomZ = 0, iRandomValue=0;
	iRandomValue = rand() % 2;
	
	if(nID >= USER_BAND && nID < NPC_BAND)	// If Target is User
	{
		pUser = m_pMain->GetUserPtr(nID - USER_BAND);
		if(pUser == NULL)	
			return FALSE;
		// Determine the direction to escape, first in the x-axis
		if((int)pUser->m_curx != (int)m_fCurX)  
		{
			iRandomX = myrand((int)m_bySearchRange, (int)(m_bySearchRange*1.5));
			iRandomZ = myrand(0, (int)m_bySearchRange);

			if((int)pUser->m_curx > (int)m_fCurX)  
				iDir = 1;
			else
				iDir = 2;
		}
		else	// in the z-axis
		{
			iRandomZ = myrand((int)m_bySearchRange, (int)(m_bySearchRange*1.5));
			iRandomX = myrand(0, (int)m_bySearchRange);
			if((int)pUser->m_curz > (int)m_fCurZ)  
				iDir = 3;
			else
				iDir = 4;
		}

		switch(iDir)
		{
		case 1:
			fDestX = m_fCurX - iRandomX;
			if(iRandomValue == 0)
				fDestZ = m_fCurZ - iRandomX;
			else
				fDestZ = m_fCurZ + iRandomX;
			break;
		case 2:
			fDestX = m_fCurX + iRandomX;
			if(iRandomValue == 0)
				fDestZ = m_fCurZ - iRandomX;
			else
				fDestZ = m_fCurZ + iRandomX;
			break;
		case 3:
			fDestZ = m_fCurZ - iRandomX;
			if(iRandomValue == 0)
				fDestX = m_fCurX - iRandomX;
			else
				fDestX = m_fCurX + iRandomX;
			break;
		case 4:
			fDestZ = m_fCurZ - iRandomX;
			if(iRandomValue == 0)
				fDestX = m_fCurX - iRandomX;
			else
				fDestX = m_fCurX + iRandomX;
			break;
		}

		vEnd.Set(fDestX, 0, fDestZ);
		fDis = GetDistance(vStart, vEnd);
		if(fDis > 20)	// If it is more than 20 meters, set it to 20 meters,,
		{
			vEnd22 = GetVectorPosition(vStart, vEnd, 20);
			fDestX = vEnd22.x;
			fDestZ = vEnd22.z;
		}
	}
	else if(nID >= NPC_BAND && m_Target.id < INVALID_BAND)	// If Target is NPC
	{
	}

	CPoint start, end;
	start.x = (int)(m_fCurX/TILE_SIZE) - min_x;
	start.y = (int)(m_fCurZ/TILE_SIZE) - min_z;
	end.x = (int)(fDestX/TILE_SIZE) - min_x;
	end.y = (int)(fDestZ/TILE_SIZE) - min_z;

	if(start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
	{
		return FALSE;
	}

	m_fStartPoint_X = m_fCurX;		m_fStartPoint_Y = m_fCurZ;
	m_fEndPoint_X = fDestX;	m_fEndPoint_Y = fDestZ;

	m_min_x = min_x;
	m_min_y = min_z;
	m_max_x = max_x;
	m_max_y = max_z;

	int nValue = PathFind(start, end, m_fSecForMetor);
	if(nValue == 1)
		return TRUE;

	return FALSE;
}

BOOL CNpc::IsInPathRange()
{
	if(m_sNid == test_id)
	{
		int x=0;
	}
	float fPathRange = 40.0f;	// error pass range
	__Vector3 vStart, vEnd;
	float fDistance = 0.0f;
	vStart.Set(m_fCurX, m_fCurY, m_fCurZ);
	if(m_sPathCount < 0)	return FALSE;
	vEnd.Set((float)m_PathList.pPattenPos[m_sPathCount].x + m_fBattlePos_x, 0, (float)m_PathList.pPattenPos[m_sPathCount].z + m_fBattlePos_z);
	fDistance = GetDistance(vStart, vEnd);

	if((int)fDistance <= (int)fPathRange+1)
		return TRUE;

	return FALSE;
}

int CNpc::GetNearPathPoint()
{
	__Vector3 vStart, vEnd;
	float fMaxPathRange = (float)NPC_MAX_MOVE_RANGE;
	float fDis1 = 0.0f, fDis2 = 0.0f;
	int nRet = -1;
	vStart.Set(m_fCurX, m_fCurY, m_fCurZ);
	vEnd.Set((float)m_PathList.pPattenPos[m_sPathCount].x + m_fBattlePos_x, 0, (float)m_PathList.pPattenPos[m_sPathCount].z + m_fBattlePos_z);
	fDis1 = GetDistance(vStart, vEnd);

	if(m_sPathCount+1 >= m_sMaxPathCount)	{
		if(m_sPathCount-1 > 0)	{
			vEnd.Set((float)m_PathList.pPattenPos[m_sPathCount-1].x + m_fBattlePos_x, 0, (float)m_PathList.pPattenPos[m_sPathCount-1].z + m_fBattlePos_z);
			fDis2 = GetDistance(vStart, vEnd);
		}
		else {
			vEnd.Set((float)m_PathList.pPattenPos[0].x + m_fBattlePos_x, 0, (float)m_PathList.pPattenPos[0].z + m_fBattlePos_z);
			fDis2 = GetDistance(vStart, vEnd);
		}
	}
	else
	{
		vEnd.Set((float)m_PathList.pPattenPos[m_sPathCount+1].x + m_fBattlePos_x, 0, (float)m_PathList.pPattenPos[m_sPathCount+1].z + m_fBattlePos_z);
		fDis2 = GetDistance(vStart, vEnd);
	}

	if(fDis1 <= fDis2)
	{
		if(fDis1 <= fMaxPathRange)
			nRet =  m_sPathCount;
	}
	else
	{
		if(fDis2 <= fMaxPathRange)
			nRet =  m_sPathCount+1;
	}

	return nRet;
}

/////////////////////////////////////////////////////////////////////////////////////
//	Check if the NPC is within the initial spawn location
//
BOOL CNpc::IsInRange(int nX, int nZ)
{
	// Determine if the NPC is out of the initial location.
	BOOL bFlag_1 = FALSE, bFlag_2 = FALSE;
	if( m_nLimitMinX < m_nLimitMaxX )	{
		if( COMPARE(nX, m_nLimitMinX, m_nLimitMaxX) )		bFlag_1 = TRUE;
	}
	else	{
		if( COMPARE(nX, m_nLimitMaxX, m_nLimitMinX) )		bFlag_1 = TRUE;
	}

	if( m_nLimitMinZ < m_nLimitMaxZ )	{
		if( COMPARE(nZ, m_nLimitMinZ, m_nLimitMaxZ) )		bFlag_2 = TRUE;
	}
	else	{
		if( COMPARE(nZ, m_nLimitMaxZ, m_nLimitMinZ) )		bFlag_2 = TRUE;
	}

	if( bFlag_1 == TRUE && bFlag_2 == TRUE )	return TRUE;

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//	Execute PathFind.
//
int CNpc::PathFind(CPoint start, CPoint end, float fDistance)
{
	ClearPathFindData();

	if(start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
	{
		return -1;
	}

	if(start.x == end.x && start.y == end.y)	// Within the same tile, if there was some movement,
	{
		m_bPathFlag = TRUE;
		m_iAniFrameIndex = 1;
		m_pPoint[0].fXPos = m_fEndPoint_X;
		m_pPoint[0].fZPos = m_fEndPoint_Y;
		TRACE("같은 영역안에서 조금 움직임.... x=%.2f, z=%.2f\n", m_pPoint[0].fXPos, m_pPoint[0].fZPos);
		return 1;
	}
	
	// Determine whether to execute passfind from here or go directly to the target point..
	if(IsPathFindCheck(fDistance) == TRUE)
	{
		m_bPathFlag = TRUE;
		return 1;
	}


	int i,j;
	int min_x, max_x;
	int min_y, max_y;

	min_x = m_min_x;
	min_y = m_min_y;
	max_x = m_max_x;
	max_y = m_max_y;

	m_vMapSize.cx = max_x - min_x + 1;		
	m_vMapSize.cy = max_y - min_y + 1;

	for(i = 0; i < m_vMapSize.cy; i++)
	{
		for(j = 0; j < m_vMapSize.cx; j++)
		{
			if((min_x+j) < 0 || (min_y+i) < 0)	return 0;
			if(m_pOrgMap[min_x + j][min_y + i].m_sEvent == 0 )
			{
				if((j*m_vMapSize.cy+i) < 0) return 0;
				m_pMap[j*m_vMapSize.cy + i] = 1;
			}
			else
			{
				if((j*m_vMapSize.cy+i) < 0) return 0;
				m_pMap[j*m_vMapSize.cy + i] = 0;
			}
		}
	}

	m_pPath = NULL;
	m_vPathFind.SetMap(m_vMapSize.cx, m_vMapSize.cy, m_pMap);
	m_pPath = m_vPathFind.FindPath(end.x, end.y, start.x, start.y);
	int count = 0;

	while(m_pPath)
	{
		m_pPath = m_pPath->Parent;
		if(m_pPath == NULL)			break;
		m_pPoint[count].pPoint.x = m_pPath->x+min_x;		m_pPoint[count].pPoint.y = m_pPath->y+min_y;
		//m_pPath = m_pPath->Parent;
		count++;
	}	
	
	if(count <= 0 || m_pPath == NULL || count >= MAX_PATH_LINE)	{	
		//TRACE("#### PathFind Fail : nid=%d,%s, count=%d ####\n", m_sNid+NPC_BAND, m_strName, count);
		return 0;
	}

	m_iAniFrameIndex = count-1;
	
	float x1 = 0.0f;
	float z1 = 0.0f;

	int nAdd = GetDir(m_fStartPoint_X, m_fStartPoint_Y, m_fEndPoint_X, m_fEndPoint_Y);

	for(i=0; i<count; i++)
	{
		if(i==1)
		{
			if(i==(count-1))
			{
				m_pPoint[i].fXPos = m_fEndPoint_X;
				m_pPoint[i].fZPos = m_fEndPoint_Y;
			}
			else
			{
				x1 = (float)(m_pPoint[i].pPoint.x * TILE_SIZE + m_fAdd_x);
				z1 = (float)(m_pPoint[i].pPoint.y * TILE_SIZE + m_fAdd_z);
				m_pPoint[i].fXPos = x1;
				m_pPoint[i].fZPos = z1;
			}
		}
		else if(i==(count-1))
		{
			m_pPoint[i].fXPos = m_fEndPoint_X;
			m_pPoint[i].fZPos = m_fEndPoint_Y;
		}
		else
		{
			x1 = (float)(m_pPoint[i].pPoint.x * TILE_SIZE + m_fAdd_x);
			z1 = (float)(m_pPoint[i].pPoint.y * TILE_SIZE + m_fAdd_z);
			m_pPoint[i].fXPos = x1;
			m_pPoint[i].fZPos = z1;
		}
	}

	return 1;
}


//	NPC death handling
void CNpc::Dead(CIOCPort* pIOCP, int iDeadType)
{
	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size())	{
		TRACE("#### Npc-Dead ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return;
	}

	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(pMap == NULL)	return;

	m_iHP = 0;
	m_NpcState = NPC_DEAD;
	m_Delay = m_sRegenTime;
	m_fDelayTime = TimeGet();
	m_bFirstLive = FALSE;
	m_byDeadType = 100;		// If you die during a war event

	if(m_iRegion_X > pMap->GetXRegionMax() || m_iRegion_Z > pMap->GetZRegionMax())	{
		TRACE("#### Npc-Dead() Fail : [nid=%d, sid=%d], nRX=%d, nRZ=%d #####\n", m_sNid+NPC_BAND, m_sSid, m_iRegion_X, m_iRegion_Z);
		return;
	}
	// Delete my information from the region on the map..
	//pMap->m_ppRegion[m_iRegion_X][m_iRegion_Z].DeleteNpc(m_sNid);
	if (m_iRegion_X<0 || m_iRegion_Z<0 || m_iRegion_X > pMap->m_sizeRegion.cx || m_iRegion_Z > pMap->m_sizeRegion.cy) {
	}
	else {
		pMap->RegionNpcRemove(m_iRegion_X, m_iRegion_Z, m_sNid + NPC_BAND);
		//TRACE("-- Npc-SetUid RegionRemove : [nid=%d, name=%s], nRX=%d, nRZ=%d \n", m_sNid+NPC_BAND, m_strName, nOld_RX, nOld_RZ);

	}

	//TRACE("-- Npc-Dead RegionRemove : [nid=%d, name=%s], nRX=%d, nRZ=%d \n", m_sNid+NPC_BAND, m_strName, m_iRegion_X, m_iRegion_Z);
	CTime t = CTime::GetCurrentTime();
	//TRACE("****** (%s,%d) Dead regentime = %d , m_byDeadType=%d, dungeonfam=%d, time=%d:%d-%d ****************\n", m_strName, m_sNid+NPC_BAND, m_sRegenTime, m_byDeadType, m_byDungeonFamily, t.GetHour(), t.GetMinute(), t.GetSecond());

	if(iDeadType == 1)	{	// Because it is not killed by the user... Dead packet transmission to the client...
		char buff[256];			int send_index = 0;
		SetByte(buff, AG_DEAD, send_index );
		SetShort(buff, m_sNid+NPC_BAND, send_index );
		SendAll(pIOCP, buff, send_index);
	}

	// Dungeon Work : In the case of a monster that changes, it is treated as changed..
	if( m_bySpecialType == 1 || m_bySpecialType == 4)	{
		if( m_byChangeType == 0 )	{
			m_byChangeType = 1;
			//ChangeMonsterInfomation( 1 );
		}
		else if( m_byChangeType == 2 )	{		// Death of the captain monster (must also check if the monster type is a captain monster)
			if( m_byDungeonFamily < 0 || m_byDungeonFamily >= MAX_DUNGEON_BOSS_MONSTER )	{
				TRACE("#### Npc-Dead() m_byDungeonFamily Fail : [nid=%d, name=%s], m_byDungeonFamily=%d #####\n", m_sNid+NPC_BAND, m_strName, m_byDungeonFamily);
				return;
			}
//			pMap->m_arDungeonBossMonster[m_byDungeonFamily] = 0;
		}
	}
	else	{
		m_byChangeType = 100;
	}

	
/*
	if( m_byDungeonFamily < 0 || m_byDungeonFamily >= MAX_DUNGEON_BOSS_MONSTER )	{
		TRACE("#### Npc-Dead() m_byDungeonFamily Fail : [nid=%d, name=%s], m_byDungeonFamily=%d #####\n", m_sNid+NPC_BAND, m_strName, m_byDungeonFamily);
		return;
	}
	if( pMap->m_arDungeonBossMonster[m_byDungeonFamily] == 0 )	{
		m_byRegenType = 2;				// So that there is no regen...
	}	*/

	// monster summon test
	//if(m_sNid == 0)
	//	m_pMain->MonsterSummon("Kryptodon", m_sCurZone, 2605.0, 1375.0);
}

//	Search for enemies near NPCs.
BOOL CNpc::FindEnemy()
{
	if(m_tNpcType == NPC_DOOR || m_tNpcType == NPC_ARTIFACT || m_tNpcType == NPC_PHOENIX_GATE || m_tNpcType == NPC_GATE_LEVER || m_tNpcType == NPC_DOMESTIC_ANIMAL || m_tNpcType == NPC_SPECIAL_GATE || m_tNpcType == NPC_DESTORY_ARTIFACT )		return FALSE;

	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size())	{
		TRACE("#### Npc-FindEnemy ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return FALSE;
	}

/*	BOOL bCheckRange = FALSE;
	if( m_NpcState == NPC_STANDING )	{
		bCheckRange = IsInRange( (int)m_fCurX, (int)m_fCurZ);
		if( bCheckRange )	{	// If you are in the active area
			if( m_tNpcAttType != m_tNpcOldAttType )	{
				m_tNpcAttType = ATROCITY_ATTACK_TYPE;	// with aggression
				//TRACE("Aggressiveness changed to first attack\n");
			}
		}
		else	{
			if( m_tNpcAttType == ATROCITY_ATTACK_TYPE )	{
				m_tNpcAttType = TENDER_ATTACK_TYPE;
				//TRACE("Attack tendency changed to back attack\n");
			}
		}
	}	*/

	// Healer Npc
	int iMonsterNid = 0;
	if( m_tNpcType == NPC_HEALER )	{		// Heal
		iMonsterNid = FindFriend( 2 );
		if( iMonsterNid != 0 )	return TRUE;
	}

	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(pMap == NULL)	return FALSE;
	CUser *pUser = NULL;
	CNpc *pNpc = NULL;

	int target_uid = 0;
	__Vector3 vUser, vNpc;
	float fDis = 0.0f;
	float fCompareDis = 0.0f;
	vNpc.Set(m_fCurX, m_fCurY, m_fCurZ);

	float fSearchRange = (float)m_bySearchRange;

	int iExpand = FindEnemyRegion();

	// Search the UserArray in your own region first, and determine if there is a user nearby.
	if(m_iRegion_X > pMap->GetXRegionMax() || m_iRegion_Z > pMap->GetZRegionMax() || m_iRegion_X < 0 || m_iRegion_Z < 0)	{
	//	TRACE("#### Npc-FindEnemy() Fail : [nid=%d, sid=%d, name=%s, th_num=%d, cur_x=%.2f, cur_z=%.2f], nRX=%d, nRZ=%d #####\n", m_sNid+NPC_BAND, m_sSid, m_strName, m_sThreadNumber, m_fCurX, m_fCurZ, m_iRegion_X, m_iRegion_Z);
		return FALSE;
	}

	fCompareDis = FindEnemyExpand(m_iRegion_X, m_iRegion_Z, fCompareDis, 1);

	int x=0, y=0;

	// Search for neighboring Regions, and move towards the User closest to the location of the monster..
	for(int l=0; l<4; l++)	{
		if(m_iFind_X[l] == 0 && m_iFind_Y[l] == 0)		continue;

		x = m_iRegion_X + (m_iFind_X[l]);
		y = m_iRegion_Z + (m_iFind_Y[l]);

		// Need to fix this part,
		if(x < 0 || y < 0 || x > pMap->GetXRegionMax() || y > pMap->GetZRegionMax())		continue;

		fCompareDis = FindEnemyExpand(x, y, fCompareDis, 1);
	}

	if(m_Target.id >= 0 && (fCompareDis <= fSearchRange))		return TRUE;

	fCompareDis = 0.0f;

	// If the type is a guard, attack the monster if it is not a monster from the same country.
	if(m_tNpcType == NPC_GUARD || m_tNpcType == NPC_PATROL_GUARD || m_tNpcType == NPC_STORE_GUARD) // || m_tNpcType == NPCTYPE_MONSTER)	
	{
		fCompareDis = FindEnemyExpand(m_iRegion_X, m_iRegion_Z, fCompareDis, 2);

		int x=0, y=0;

		// Search for neighboring Regions, and move towards the User closest to the location of the monster..
		for(int l=0; l<4; l++)	{
			if(m_iFind_X[l] == 0 && m_iFind_Y[l] == 0)			continue;

			x = m_iRegion_X + (m_iFind_X[l]);
			y = m_iRegion_Z + (m_iFind_Y[l]);

			// Need to fix this part,
			if(x < 0 || y < 0 || x > pMap->GetXRegionMax() || y > pMap->GetZRegionMax())	continue;

			fCompareDis = FindEnemyExpand(x, y, fCompareDis, 2);
		}
	}

	if(m_Target.id >= 0 && (fCompareDis <= fSearchRange))	return TRUE;

	// Since there is no one, initialize the user managed in the list.
	InitUserList();		
	InitTarget();
	return FALSE;
}

// When an NPC searches for a user, it determines which Region to search for.
int CNpc::FindEnemyRegion()
{
	/*
        1	2	3
		4	0	5
		6	7	8
	*/
	int iRetValue = 0;
	int  iSX = m_iRegion_X * VIEW_DIST;
	int  iSZ = m_iRegion_Z * VIEW_DIST;
	int  iEX = (m_iRegion_X+1) * VIEW_DIST;
	int  iEZ = (m_iRegion_Z+1) * VIEW_DIST;
	int  iSearchRange = m_bySearchRange;
	int iCurSX = (int)m_fCurX - m_bySearchRange;
	int iCurSY = (int)m_fCurX - m_bySearchRange;
	int iCurEX = (int)m_fCurX + m_bySearchRange;
	int iCurEY = (int)m_fCurX + m_bySearchRange;
	
	int iMyPos = GetMyField();

	switch(iMyPos)
	{
	case 1:
		if((iCurSX < iSX) && (iCurSY < iSZ))
			iRetValue = 1;
		else if((iCurSX > iSX) && (iCurSY < iSZ))
			iRetValue = 2;
		else if((iCurSX < iSX) && (iCurSY > iSZ))
			iRetValue = 4;
		else if((iCurSX >= iSX) && (iCurSY >= iSZ))
			iRetValue = 0;
		break;
	case 2:
		if((iCurEX < iEX) && (iCurSY < iSZ))
			iRetValue = 2;
		else if((iCurEX > iEX) && (iCurSY < iSZ))
			iRetValue = 3;
		else if((iCurEX <= iEX) && (iCurSY >= iSZ))
			iRetValue = 0;
		else if((iCurEX > iEX) && (iCurSY > iSZ))
			iRetValue = 5;
		break;
	case 3:
		if((iCurSX < iSX) && (iCurEY < iEZ))
			iRetValue = 4;
		else if((iCurSX >= iSX) && (iCurEY <= iEZ))
			iRetValue = 0;
		else if((iCurSX < iSX) && (iCurEY > iEZ))
			iRetValue = 6;
		else if((iCurSX > iSX) && (iCurEY > iEZ))
			iRetValue = 7;
		break;
	case 4:
		if((iCurEX <= iEX) && (iCurEY <= iEZ))
			iRetValue = 0;
		else if((iCurEX > iEX) && (iCurEY < iEZ))
			iRetValue = 5;
		else if((iCurEX < iEX) && (iCurEY > iEZ))
			iRetValue = 7;
		else if((iCurEX > iEX) && (iCurEY > iEZ))
			iRetValue = 8;
		break;
	}

	if(iRetValue <= 0) // For temporary correction (in case of problems),,..
		iRetValue = 0;

	switch(iRetValue)
	{
	case 0:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 0;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 1:
		m_iFind_X[0] = -1;  m_iFind_Y[0] = -1;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = -1;
		m_iFind_X[2] = -1;  m_iFind_Y[2] = 0;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 2:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = -1;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 0;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 3:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 1;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 1;
		m_iFind_X[3] = 1;  m_iFind_Y[3] = 1;
		break;
	case 4:
		m_iFind_X[0] = -1;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 0;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 5:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 1;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 0;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 6:
		m_iFind_X[0] = -1;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = -1;  m_iFind_Y[2] = 1;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 1;
		break;
	case 7:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 0;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 1;
		m_iFind_X[3] = 0;  m_iFind_Y[3] = 0;
		break;
	case 8:
		m_iFind_X[0] = 0;  m_iFind_Y[0] = 0;
		m_iFind_X[1] = 1;  m_iFind_Y[1] = 0;
		m_iFind_X[2] = 0;  m_iFind_Y[2] = 1;
		m_iFind_X[3] = 1;  m_iFind_Y[3] = 1;
		break;
	}

	return iRetValue;
}

float CNpc::FindEnemyExpand(int nRX, int nRZ, float fCompDis, int nType)
{
	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size())	{
		TRACE("#### Npc-FindEnemyExpand ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return 0.0f;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	float fDis = 0.0f;
	if(pMap == NULL)	return fDis;
	float fComp = fCompDis;
	float fSearchRange = (float)m_bySearchRange;
	int target_uid = -1;
	__Vector3 vUser, vNpc, vMon;
	vNpc.Set(m_fCurX, m_fCurY, m_fCurZ);
	int* pIDList = NULL;
	int iLevelComprison = 0;
	
	if(nType == 1)	{		// When targeting users
		int nUserid = 0, count = 0;
		CUser* pUser = NULL;

		EnterCriticalSection( &g_region_critical );
		map < int, int* >::iterator		Iter1;
		map < int, int* >::iterator		Iter2;
		
		Iter1 = pMap->m_ppRegion[nRX][nRZ].m_RegionUserArray.m_UserTypeMap.begin();
		Iter2 = pMap->m_ppRegion[nRX][nRZ].m_RegionUserArray.m_UserTypeMap.end();

		int nUser= pMap->m_ppRegion[nRX][nRZ].m_RegionUserArray.GetSize();
		//TRACE("FindEnemyExpand type1,, region_x=%d, region_z=%d, user=%d, mon=%d\n", nRX, nRZ, nUser, nMonster);
		if( nUser == 0 )	{
			LeaveCriticalSection( &g_region_critical );
			return 0.0f;
		}

		pIDList = new int[nUser];
		for( ; Iter1 != Iter2; Iter1++ ) {
			nUserid = *( (*Iter1).second );
			pIDList[count] = nUserid;
			count++;
		}
		LeaveCriticalSection( &g_region_critical );

		for(int i=0 ; i<nUser; i++ ) {
			nUserid = pIDList[i];
			if( nUserid < 0 )	continue;
			pUser = (CUser*)m_pMain->GetUserPtr(nUserid);
			if( pUser != NULL && pUser->m_bLive == USER_LIVE)	{
				// Players from the same country should not attack...
				if(m_byGroup == pUser->m_bNation)	continue;
				if(pUser->m_byIsOP == MANAGER_USER)	continue;	// ignore operator

				vUser.Set(pUser->m_curx, pUser->m_cury, pUser->m_curz); 
				fDis = GetDistance(vUser, vNpc);

				// Task: Determine whether the user is in my attack range here
				if(fDis <= fSearchRange)	{
					if(fDis >= fComp)	{	// 
						target_uid = pUser->m_iUserId;
						fComp = fDis;

						//Back mob...
						if(!m_tNpcAttType)	{	// Find the guy who attacked me.
							if(IsDamagedUserList(pUser) || (m_tNpcGroupType && m_Target.id == target_uid))	{
								m_Target.id	= target_uid;
								m_Target.failCount = 0;
								m_Target.x	= pUser->m_curx;
								m_Target.y	= pUser->m_cury;
								m_Target.z	= pUser->m_curz;
							}
						}
						else	{
							iLevelComprison = pUser->m_sLevel - m_sLevel;
							// What to work on: Aggression according to type..
							//if(iLevelComprison > ATTACK_LIMIT_LEVEL)	continue;

							m_Target.id	= target_uid;
							m_Target.failCount = 0;
							m_Target.x	= pUser->m_curx;
							m_Target.y	= pUser->m_cury;
							m_Target.z	= pUser->m_curz;
							//TRACE("Npc-FindEnemyExpand - target_x = %.2f, z=%.2f\n", m_Target.x, m_Target.z);
						}
					}
				}	
				else continue;
			}
		}
	}
	else if(nType == 2)		{		// When guards target monsters
		int nNpcid = 0, count = 0;
		CNpc* pNpc = NULL;
	
		EnterCriticalSection( &g_region_critical );
		map < int, int* >::iterator		Iter1;
		map < int, int* >::iterator		Iter2;
		
		Iter1 = pMap->m_ppRegion[nRX][nRZ].m_RegionNpcArray.m_UserTypeMap.begin();
		Iter2 = pMap->m_ppRegion[nRX][nRZ].m_RegionNpcArray.m_UserTypeMap.end();

		int nMonster= pMap->m_ppRegion[nRX][nRZ].m_RegionNpcArray.GetSize();
		//TRACE("FindEnemyExpand type1,, region_x=%d, region_z=%d, user=%d, mon=%d\n", nRX, nRZ, nUser, nMonster);
		if( nMonster == 0 )	{
			LeaveCriticalSection( &g_region_critical );
			return 0.0f;
		}

		pIDList = new int[nMonster];
		for( ; Iter1 != Iter2; Iter1++ ) {
			nNpcid = *( (*Iter1).second );
			pIDList[count] = nNpcid;
			count++;
		}
		LeaveCriticalSection( &g_region_critical );

		//TRACE("FindEnemyExpand type2,, region_x=%d, region_z=%d, user=%d, mon=%d\n", nRX, nRZ, nUser, nMonster);

		for(int i=0 ; i<nMonster; i++ ) {
			nNpcid = pIDList[i];
			if( nNpcid < NPC_BAND )	continue;
			pNpc = (CNpc*)m_pMain->m_arNpc.GetData(nNpcid - NPC_BAND);

			if(m_sNid == pNpc->m_sNid)	continue;

			if( pNpc != NULL && pNpc->m_NpcState != NPC_DEAD && pNpc->m_sNid != m_sNid)	{
				// Monsters from the same country should not attack...
				if(m_byGroup == pNpc->m_byGroup)	continue;

				vMon.Set(pNpc->m_fCurX, pNpc->m_fCurY, pNpc->m_fCurZ); 
				fDis = GetDistance(vMon, vNpc);

				// Task: Determine whether the user is in my attack range here
				if(fDis <= fSearchRange)	{
					if(fDis >= fComp)	{	// 
						target_uid = nNpcid;
						fComp = fDis;
						m_Target.id	= target_uid;
						m_Target.failCount = 0;
						m_Target.x	= pNpc->m_fCurX;
						m_Target.y	= pNpc->m_fCurY;
						m_Target.z	= pNpc->m_fCurZ;
					//	TRACE("Npc-IsCloseTarget - target_x = %.2f, z=%.2f\n", m_Target.x, m_Target.z);
					}
				}	
				else continue;
			}
		}
	}

	if(pIDList)	{
		delete [] pIDList;
		pIDList = NULL;
	}

	return fComp;
}

// Divide the region into 4 parts to determine which part of the region the current position of the monster falls into
int CNpc::GetMyField()
{
	int iRet = 0;
	int iX = m_iRegion_X * VIEW_DIST;
	int iZ = m_iRegion_Z * VIEW_DIST;
	int iAdd = VIEW_DIST / 2;
	int iCurX = (int)m_fCurX;	// monster current position_x
	int iCurZ = (int)m_fCurZ;
	if(COMPARE(iCurX, iX, iX+iAdd) && COMPARE(iCurZ, iZ, iZ+iAdd))
		iRet = 1;
	else if(COMPARE(iCurX, iX+iAdd, iX+VIEW_DIST) && COMPARE(iCurZ, iZ, iZ+iAdd))
		iRet = 2;
	else if(COMPARE(iCurX, iX, iX+iAdd) && COMPARE(iCurZ, iZ+iAdd, iZ+VIEW_DIST))
		iRet = 3;
	else if(COMPARE(iCurX, iX+iAdd, iX+VIEW_DIST) && COMPARE(iCurZ, iZ+iAdd, iZ+VIEW_DIST))
		iRet = 4;

	return iRet;
}

//	Find out if there is a user nearby who attacked you
BOOL CNpc::IsDamagedUserList(CUser *pUser)
{
	if(pUser == NULL) return FALSE;

	for(int i = 0; i < NPC_HAVE_USER_LIST; i++)
	{
		if(strcmp(m_DamagedUserList[i].strUserID, pUser->m_strUserID) == 0) return TRUE;
	}

	return FALSE;
}

//	If the target is surrounded, it searches for the next target.
int CNpc::IsSurround(CUser* pUser)
{
	if(m_tNpcLongType) return 0;		//long distance passes

	if(pUser == NULL)	return -2;		// Target designation failed because there is no user..
	int nDir = pUser->IsSurroundCheck(m_fCurX, 0.0f, m_fCurZ, m_sNid+NPC_BAND);
	if(nDir != 0)
	{
		m_byAttackPos = nDir;
		return nDir;
	}
	return -1;					// Target is surrounded...
}

//	Determine if x, y are movable coordinates
BOOL CNpc::IsMovable(float x, float z)
{
	if(x < 0 || z < 0 ) return FALSE;

	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size()) 
	{
		TRACE("#### Npc-IsMovable ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return FALSE;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(!pMap ) return FALSE;
	if(!pMap->m_pMap) return FALSE;
	if(x >= pMap->m_sizeMap.cx || z >= pMap->m_sizeMap.cy) return FALSE;
	if(pMap->m_pMap[(int)x][(int)z].m_sEvent == 0) return FALSE;

	return TRUE;
}

//	Determine whether you have moved all the way found by Path Find
BOOL CNpc::IsMovingEnd()
{
	//if(m_fCurX == m_fEndPoint_X && m_fCurZ == m_fEndPoint_Y) 
	if(m_fPrevX == m_fEndPoint_X && m_fPrevZ == m_fEndPoint_Y) 
	{
		//m_sStepCount = 0;
		m_iAniFrameCount = 0;
		return TRUE;
	}

	return FALSE;
}

//	Move toward the target as much as the number of steps.
BOOL CNpc::StepMove(int nStep, CIOCPort* pIOCP)
{
	if(m_NpcState != NPC_MOVING && m_NpcState != NPC_TRACING && m_NpcState != NPC_BACK) return FALSE;
	
	POINT ptPre={-1,-1};

	__Vector3 vStart, vEnd, vDis;
	float fDis;

	float fOldCurX = 0.0f, fOldCurZ = 0.0f;

	if(m_sStepCount == 0)	{
		fOldCurX = m_fCurX;  fOldCurZ = m_fCurZ;
	}
	else	{
		fOldCurX = m_fPrevX; fOldCurZ = m_fPrevZ;
	}

	vStart.Set(fOldCurX, 0, fOldCurZ);
	vEnd.Set(m_pPoint[m_iAniFrameCount].fXPos, 0, m_pPoint[m_iAniFrameCount].fZPos);

	// safe code...
	if(m_pPoint[m_iAniFrameCount].fXPos < 0 || m_pPoint[m_iAniFrameCount].fZPos < 0)
	{
		m_fPrevX = m_fEndPoint_X;
		m_fPrevZ = m_fEndPoint_Y;
		TRACE("##### Step Move Fail : [nid = %d,%s] m_iAniFrameCount=%d/%d ######\n", m_sNid+NPC_BAND, m_strName, m_iAniFrameCount, m_iAniFrameIndex);
		SetUid(m_fPrevX, m_fPrevZ, m_sNid + NPC_BAND);
		return FALSE;	
	}

	fDis = GetDistance(vStart, vEnd);

	if(fDis >= m_fSecForMetor)
	{
		vDis = GetVectorPosition(vStart, vEnd, m_fSecForMetor);
		m_fPrevX = vDis.x;
		m_fPrevZ = vDis.z;
	}
	else
	{
		m_iAniFrameCount++;
		if(m_iAniFrameCount == m_iAniFrameIndex)
		{
			vEnd.Set(m_pPoint[m_iAniFrameCount].fXPos, 0, m_pPoint[m_iAniFrameCount].fZPos);
			fDis = GetDistance(vStart, vEnd);
			// The last coordinate can also be moved between m_fSecForMetor and m_fSecForMetor+1
			if(fDis > m_fSecForMetor)
			{
				vDis = GetVectorPosition(vStart, vEnd, m_fSecForMetor);
				m_fPrevX = vDis.x;
				m_fPrevZ = vDis.z;
				m_iAniFrameCount--;
			}
			else
			{
				m_fPrevX = m_fEndPoint_X;
				m_fPrevZ = m_fEndPoint_Y;
			}
		}
		else
		{
			vEnd.Set(m_pPoint[m_iAniFrameCount].fXPos, 0, m_pPoint[m_iAniFrameCount].fZPos);
			fDis = GetDistance(vStart, vEnd);
			if(fDis >= m_fSecForMetor)
			{
				vDis = GetVectorPosition(vStart, vEnd, m_fSecForMetor);
				m_fPrevX = vDis.x;
				m_fPrevZ = vDis.z;
			}
			else
			{
				m_fPrevX = m_fEndPoint_X;
				m_fPrevZ = m_fEndPoint_Y;
			}
		}
	}

	vStart.Set(fOldCurX, 0, fOldCurZ);
	vEnd.Set(m_fPrevX, 0, m_fPrevZ);

	m_fSecForRealMoveMetor = GetDistance(vStart, vEnd);

	if(m_fSecForRealMoveMetor > m_fSecForMetor+1)
	{
		TRACE("#### move fail : [nid = %d], m_fSecForMetor = %.2f\n", m_sNid+NPC_BAND, m_fSecForRealMoveMetor);
	}

	if(m_sStepCount == 0)
	{
		m_sStepCount++;
	}
	else
	{
		m_sStepCount++;
		m_fCurX = fOldCurX;		 m_fCurZ = fOldCurZ;
		if(m_fCurX < 0 || m_fCurZ < 0)
		{
			TRACE("Npc-StepMove : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ);
		}
		
		if(SetUid(m_fCurX, m_fCurZ, m_sNid + NPC_BAND))
		{
			return TRUE;
		}
		else return FALSE;	
		
	}

	return TRUE;
}

BOOL CNpc::StepNoPathMove(int nStep)
{
	if(m_NpcState != NPC_MOVING && m_NpcState != NPC_TRACING && m_NpcState != NPC_BACK) return FALSE;
	
	__Vector3 vStart, vEnd;
	float fOldCurX = 0.0f, fOldCurZ = 0.0f;

	if(m_sStepCount == 0)	{
		fOldCurX = m_fCurX; fOldCurZ = m_fCurZ;
	}
	else	{
		fOldCurX = m_fPrevX; fOldCurZ = m_fPrevZ;
	}

	
	if(m_sStepCount < 0 || m_sStepCount >= m_iAniFrameIndex)	{	
		TRACE("#### IsNoPtahfind Fail : nid=%d,%s, count=%d/%d ####\n", m_sNid+NPC_BAND, m_strName, m_sStepCount, m_iAniFrameIndex);
		return FALSE;
	}

	vStart.Set(fOldCurX, 0, fOldCurZ);
	m_fPrevX = m_pPoint[m_sStepCount].fXPos;
	m_fPrevZ = m_pPoint[m_sStepCount].fZPos;
	vEnd.Set(m_fPrevX, 0, m_fPrevZ);

	if(m_fPrevX == -1 || m_fPrevZ == -1)	{
		TRACE("##### StepNoPath Fail : nid=%d,%s, x=%.2f, z=%.2f #####\n", m_sNid+NPC_BAND, m_strName, m_fPrevX, m_fPrevZ);
		return FALSE;
	}

	m_fSecForRealMoveMetor = GetDistance(vStart, vEnd);
	
	if(m_sStepCount == 0)	{
		m_sStepCount++;
	}
	else	{
		m_sStepCount++;
		if(fOldCurX < 0 || fOldCurZ < 0)	{
			TRACE("#### Npc-StepNoPathMove Fail : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, fOldCurX, fOldCurZ);
			return FALSE;
		}
		else	{
			m_fCurX = fOldCurX;	 m_fCurZ = fOldCurZ;
		}
		
		if(SetUid(m_fCurX, m_fCurZ, m_sNid + NPC_BAND))	{
			return TRUE;
		}
		else return FALSE;	
	}

	return TRUE;
}

//	Determine if the distance between the NPC and the target is smaller than the specified range
int CNpc::IsCloseTarget(int nRange, int Flag)
{
	__Vector3 vUser, vWillUser, vNpc, vDistance;
	CUser* pUser = NULL;
	CNpc* pNpc = NULL;
	float fDis = 0.0f, fWillDis = 0.0f, fX = 0.0f, fZ = 0.0f;
	BOOL  bUserType = FALSE;	// TRUE if the target is a user
	vNpc.Set(m_fCurX, m_fCurY, m_fCurZ);

	if(m_Target.id >= USER_BAND && m_Target.id < NPC_BAND)	{	// If Target is User
		pUser = m_pMain->GetUserPtr(m_Target.id - USER_BAND);
		if(pUser == NULL)	{
			InitTarget();
			return -1;
		}
		vUser.Set(pUser->m_curx, pUser->m_cury, pUser->m_curz); 
		vWillUser.Set(pUser->m_fWill_x, pUser->m_fWill_y, pUser->m_fWill_z); 
		fX = pUser->m_curx;		
		fZ = pUser->m_curz;

		vDistance = vWillUser - vNpc;
		fWillDis = vDistance.Magnitude();	
		fWillDis = fWillDis - m_fBulk;
		bUserType = TRUE;
	}
	else if(m_Target.id >= NPC_BAND && m_Target.id < INVALID_BAND)	// If Target is mon
	{
		pNpc = m_pMain->m_arNpc.GetData(m_Target.id - NPC_BAND);
		if(pNpc == NULL) 
		{
			InitTarget();
			return -1;
		}
		vUser.Set(pNpc->m_fCurX, pNpc->m_fCurY, pNpc->m_fCurZ); 
		fX = pNpc->m_fCurX;
		fZ = pNpc->m_fCurZ;
	}
	else	{
		return -1;
	}
	
	vDistance = vUser - vNpc;
	fDis = vDistance.Magnitude();	

	fDis = fDis - m_fBulk;

	// To work on: Routine to check that dungeon monsters do not leave a certain area
    if ( m_tNpcType == NPC_DUNGEON_MONSTER )	{
		if( IsInRange( (int)vUser.x, (int)vUser.z ) == FALSE)
			return -1;	
    }		

	if(Flag == 1)	{
		m_byResetFlag = 1;
		if(pUser)	{
			if(m_Target.x == pUser->m_curx && m_Target.z == pUser->m_curz)
				m_byResetFlag = 0;
		}
		//TRACE("NpcTracing-IsCloseTarget - target_x = %.2f, z=%.2f, dis=%.2f, Flag=%d\n", m_Target.x, m_Target.z, fDis, m_byResetFlag);
	}
	
	if((int)fDis > nRange)	{
		if(Flag == 2)			{
			//TRACE("NpcFighting-IsCloseTarget - target_x = %.2f, z=%.2f, dis=%.2f\n", m_Target.x, m_Target.z, fDis);
			m_byResetFlag = 1;
			m_Target.x = fX;
			m_Target.z = fZ;
		}
		return 0; 
	}

	/* Update the target coordinates to the latest, and update the last pointer coordinates,, */
	m_fEndPoint_X = m_fCurX;
	m_fEndPoint_Y = m_fCurZ;
	m_Target.x = fX;
	m_Target.z = fZ;

	//if( m_tNpcLongType && m_tNpcType != NPC_BOSS_MONSTER)	{		// A long-range attack is judged by the attack distance.
	if( m_tNpcLongType == 1 )	{		// A long-range attack is judged by the attack distance.
		if(fDis < LONG_ATTACK_RANGE)	return 1;
		else if(fDis > LONG_ATTACK_RANGE && fDis <= nRange) return 2;
	}
	else	{					// short range (direct attack)
		if( Flag == 1 )		{	// When checking this distance while moving the monster
			if(fDis < (SHORT_ATTACK_RANGE+m_fBulk) )	return 1;
			if(fDis > (SHORT_ATTACK_RANGE+m_fBulk) && fDis <= nRange) return 2;				// Based on the user's current coordinates
			if( bUserType == TRUE )	{	// Only when you are a user, based on Will coordinates
				if(fWillDis > (SHORT_ATTACK_RANGE+m_fBulk) && fWillDis <= nRange) return 2;		// Based on the user's Will diagram
			}
		}
		else {
			if(fDis < (SHORT_ATTACK_RANGE+m_fBulk) )	return 1;
			else if(fDis > (SHORT_ATTACK_RANGE+m_fBulk) && fDis <= nRange) return 2;
		}
	}


	//TRACE("Npc-IsCloseTarget - target_x = %.2f, z=%.2f\n", m_Target.x, m_Target.z);
	return 0;
}

//	Path Finding between Target and NPC is performed.
int CNpc::GetTargetPath(int option)
{
	int nInitType = m_byInitMoveType;
	if(m_byInitMoveType >= 100)	{
		nInitType = m_byInitMoveType - 100;
	}
	// Behavior type modification
	if(m_tNpcType != 0)	{
		//if(m_byMoveType != m_byInitMoveType)
		//	m_byMoveType = m_byInitMoveType;	// to get back to your place...
		if(m_byMoveType != nInitType)	m_byMoveType = nInitType;	// to get back to your place...
	}
	// ~sungyong 2002.06.12

	// When chasing, match your running speed...
	m_fSecForMetor = m_fSpeed_2;
	CUser* pUser = NULL;
	CNpc* pNpc = NULL;
	float iTempRange = 0.0f;
	__Vector3 vUser, vNpc, vDistance, vEnd22;
	float fDis = 0.0f;
	float fDegree = 0.0f, fTargetDistance = 0.0f;
	float fSurX = 0.0f, fSurZ = 0.0f;

	// 1m
	//float surround_fx[8] = {0.0f, -0.7071f, -1.0f, -0.7083f,  0.0f,  0.7059f,  1.0000f, 0.7083f};
	//float surround_fz[8] = {1.0f,  0.7071f,  0.0f, -0.7059f, -1.0f, -0.7083f, -0.0017f, 0.7059f};
	// 2m
	float surround_fx[8] = {0.0f, -1.4142f, -2.0f, -1.4167f,  0.0f,  1.4117f,  2.0000f, 1.4167f};
	float surround_fz[8] = {2.0f,  1.4142f,  0.0f, -1.4167f, -2.0f, -1.4167f, -0.0035f, 1.4117f};

	if(m_Target.id >= USER_BAND && m_Target.id < NPC_BAND)	{	// If Target is User
		pUser = m_pMain->GetUserPtr(m_Target.id - USER_BAND);
		if(pUser == NULL)	{
			InitTarget();
			return -1;
		}
		if(pUser->m_sHP <= 0 /*|| pUser->m_state != STATE_GAMESTARTED*/ || pUser->m_bLive == FALSE)	{
			InitTarget();
			return -1;
		}
		if(pUser->m_curZone != m_sCurZone)	{
			InitTarget();
			return -1;
		}

		if(option == 1)	{	// If you are attacked with magic or a bow...
			vNpc.Set(m_fCurX, m_fCurY, m_fCurZ);
			vUser.Set(pUser->m_curx, pUser->m_cury, pUser->m_curz); 
			fDis = GetDistance(vNpc, vUser);
			if(fDis >= NPC_MAX_MOVE_RANGE)		return -1;	// Too far away to be traced...
			iTempRange = fDis + 10;
		}
		else	{
			iTempRange = (float)m_bySearchRange;				// correct temporarily.
			if(IsDamagedUserList(pUser)) iTempRange = (float)m_byTracingRange;	// Increases search range when attacked.
			else iTempRange += 2;
		}
	}
	else if(m_Target.id >= NPC_BAND && m_Target.id < INVALID_BAND)	{	// If Target is mon
		pNpc = m_pMain->m_arNpc.GetData(m_Target.id - NPC_BAND);
		if(pNpc == NULL) {
			InitTarget();
			return FALSE;
		}
		if(pNpc->m_iHP <= 0 || pNpc->m_NpcState == NPC_DEAD)	{
			InitTarget();
			return -1;
		}

		iTempRange = (float)m_byTracingRange;				// correct temporarily.
	}

	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size())	{
		TRACE("#### Npc-GetTargetPath ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return -1;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(pMap == NULL) return -1;

	int max_xx = pMap->m_sizeMap.cx;
	int max_zz = pMap->m_sizeMap.cy;

	int min_x = (int)(m_fCurX - iTempRange)/TILE_SIZE;	if(min_x < 0) min_x = 0;
	int min_z = (int)(m_fCurZ - iTempRange)/TILE_SIZE;	if(min_z < 0) min_z = 0;
	int max_x = (int)(m_fCurX + iTempRange)/TILE_SIZE;	if(max_x >= max_xx) max_x = max_xx - 1;
	int max_z = (int)(m_fCurZ + iTempRange)/TILE_SIZE;	if(min_z >= max_zz) min_z = max_zz - 1;

	if(m_Target.id >= USER_BAND && m_Target.id < NPC_BAND)	{	// If Target is User
		// Inspect whether the target point does not deviate from the search range
		CRect r = CRect(min_x, min_z, max_x+1, max_z+1);
		if(r.PtInRect(CPoint((int)pUser->m_curx/TILE_SIZE, (int)pUser->m_curz/TILE_SIZE)) == FALSE)	{
			TRACE("### Npc-GetTargetPath() User Fail return -1: [nid=%d] t_Name=%s, AttackPos=%d ###\n", m_sNid+NPC_BAND, pUser->m_strUserID, m_byAttackPos);
			return -1;
		}

		m_fStartPoint_X = m_fCurX;		m_fStartPoint_Y = m_fCurZ;
	
		vNpc.Set(m_fCurX, m_fCurY, m_fCurZ);
		vUser.Set(pUser->m_curx, pUser->m_cury, pUser->m_curz); 
		
		// Determine which direction the user will attack from here... (Setting)
		// In this part, find out the attack point of the NPC and attack it,,
		IsSurround(pUser);	//Ignore it if it is surrounded.

		//vEnd22 = CalcAdaptivePosition(vNpc, vUser, 2.0+m_fBulk);

		//TRACE("Npc-GetTargetPath() : [nid=%d] t_Name=%s, AttackPos=%d\n", m_sNid, pUser->m_strUserID, m_byAttackPos);

		if(m_byAttackPos > 0 && m_byAttackPos < 9)	{
			fDegree = (m_byAttackPos-1)*45;
			fTargetDistance = 2.0f+m_fBulk;
			vEnd22 = ComputeDestPos(vUser, 0.0f, fDegree, fTargetDistance);
			fSurX = vEnd22.x - vUser.x;			fSurZ = vEnd22.z - vUser.z;
			//m_fEndPoint_X = vUser.x + surround_fx[m_byAttackPos-1];	m_fEndPoint_Y = vUser.z + surround_fz[m_byAttackPos-1];
			m_fEndPoint_X = vUser.x + fSurX;	m_fEndPoint_Y = vUser.z + fSurZ;
		}
		else	{
			vEnd22 = CalcAdaptivePosition(vNpc, vUser, 2.0+m_fBulk);
			m_fEndPoint_X = vEnd22.x;	m_fEndPoint_Y = vEnd22.z;
		}
	}
	else if(m_Target.id >= NPC_BAND && m_Target.id < INVALID_BAND)	{
		// Inspect whether the target point does not deviate from the search range
		CRect r = CRect(min_x, min_z, max_x+1, max_z+1);
		if(r.PtInRect(CPoint((int)pNpc->m_fCurX/TILE_SIZE, (int)pNpc->m_fCurZ/TILE_SIZE)) == FALSE)	{
			TRACE("### Npc-GetTargetPath() Npc Fail return -1: [nid=%d] t_Name=%s, AttackPos=%d ###\n", m_sNid+NPC_BAND, pNpc->m_strName, m_byAttackPos);
			return -1;
		}

		m_fStartPoint_X = m_fCurX;		m_fStartPoint_Y = m_fCurZ;
	
		vNpc.Set(m_fCurX, m_fCurY, m_fCurZ);
		vUser.Set(pNpc->m_fCurX, pNpc->m_fCurY, pNpc->m_fCurZ); 
		
		vEnd22 = CalcAdaptivePosition(vNpc, vUser, 2.0f+m_fBulk);
		m_fEndPoint_X = vEnd22.x;	m_fEndPoint_Y = vEnd22.z;
	}

	vDistance = vEnd22 - vNpc;
	fDis = vDistance.Magnitude();

	if(fDis <= m_fSecForMetor)	{
		ClearPathFindData();
		m_bPathFlag = TRUE;
		m_iAniFrameIndex = 1;
		m_pPoint[0].fXPos = m_fEndPoint_X;
		m_pPoint[0].fZPos = m_fEndPoint_Y;
		//TRACE("** Npc Direct Trace Move  : [nid = %d], fDis <= %d, %.2f **\n", m_sNid, m_fSecForMetor, fDis);
		return TRUE;
	}
	
	if((int)fDis > iTempRange)	{
		TRACE("Npc-GetTargetPath() searchrange over Fail return -1: [nid=%d,%s]\n", m_sNid+NPC_BAND, m_strName);
		return -1; 
	}

	
	if( m_tNpcType != NPC_DUNGEON_MONSTER )	{		// Dungeon monsters must always pass find..
		// If there is an attacking target, it goes directly to the target without pathfinding.
		if(m_Target.id != -1)	return 0;	
	}

	CPoint start, end;
	start.x = (int)(m_fCurX/TILE_SIZE) - min_x;
	start.y = (int)(m_fCurZ/TILE_SIZE) - min_z;
	end.x = (int)(vEnd22.x/TILE_SIZE) - min_x;
	end.y = (int)(vEnd22.z/TILE_SIZE) - min_z;

	// To work on: Routine to check that dungeon monsters do not leave a certain area
    if ( m_tNpcType == NPC_DUNGEON_MONSTER )	{
		if( IsInRange( (int)vEnd22.x, (int)vEnd22.z ) == FALSE)
			return -1;	
    }

	m_min_x = min_x;
	m_min_y = min_z;
	m_max_x = max_x;
	m_max_y = max_z;

	// Run Path Find ---------------------------------------------//
	return PathFind(start, end, m_fSecForMetor);
}

int CNpc::Attack(CIOCPort* pIOCP)
{
	if(!pIOCP) return 10000;

	// Teleport possible,, (randomly,, )
	int nRandom = 0, nPercent=1000;
	int send_index = 0;
	BOOL bTeleport = FALSE;
	char buff[256];
	memset( buff, 0x00, 256 );

/*	nRandom = myrand(1, 10000);
	if( COMPARE( nRandom, 8000, 10000) )	{
		bTeleport = Teleport( pIOCP );
		if( bTeleport )		return m_Delay;
	}	*/

	//if( m_tNpcLongType==1 && m_tNpcType != NPC_BOSS_MONSTER )	{
	if( m_tNpcLongType == 1 )	{		// A long-range attack is judged by the attack distance.
		m_Delay = LongAndMagicAttack(pIOCP);
		return m_Delay;
	}
		
	int ret = 0;
	int nStandingTime = m_sStandTime;

	ret = IsCloseTarget(m_byAttackRange, 2);

	if(ret == 0)   {
		if(m_tNpcType == NPC_DOOR || m_tNpcType == NPC_ARTIFACT || m_tNpcType == NPC_PHOENIX_GATE || m_tNpcType == NPC_GATE_LEVER || m_tNpcType == NPC_DOMESTIC_ANIMAL || m_tNpcType == NPC_SPECIAL_GATE || m_tNpcType == NPC_DESTORY_ARTIFACT ) // 고정 경비병은 추적을 하지 않도록..
		{
			m_NpcState = NPC_STANDING;
			InitTarget();
			return 0;
		}	
		m_sStepCount = 0;
		m_byActionFlag = ATTACK_TO_TRACE;
		m_NpcState = NPC_TRACING;			// To catch up with attacking and running away users (faster reaction)
		return 0;							// Update user x, y values in IsCloseTarget() and zoom to Delay = 0
	}	
	else if( ret == 2 )	{
		//if(m_tNpcType == NPC_BOSS_MONSTER)	{		// If it's the boss monster...
		if(m_tNpcLongType == 2)	{		// Since it is a monster capable of direct and indirect (long) attacks, it is capable of long-range attacks.
			m_Delay = LongAndMagicAttack(pIOCP);
			return m_Delay;
		}
		else	{
			if(m_tNpcType == NPC_DOOR || m_tNpcType == NPC_ARTIFACT || m_tNpcType == NPC_PHOENIX_GATE || m_tNpcType == NPC_GATE_LEVER || m_tNpcType == NPC_DOMESTIC_ANIMAL || m_tNpcType == NPC_SPECIAL_GATE || m_tNpcType == NPC_DESTORY_ARTIFACT ) // 고정 경비병은 추적을 하지 않도록..
			{
				m_NpcState = NPC_STANDING;
				InitTarget();
				return 0;
			}	
			m_sStepCount = 0;
			m_byActionFlag = ATTACK_TO_TRACE;
			m_NpcState = NPC_TRACING;			// To catch up with attacking and running away users (faster reaction)
			return 0;							// Update user x, y values in IsCloseTarget() and zoom to Delay = 0
		}
	}
	else if( ret == -1 )	{
		m_NpcState = NPC_STANDING;
		InitTarget();
		return 0;
	}

	CNpc*	pNpc		= NULL;	
	CUser*	pUser		= NULL;
	int		nDamage		= 0;
	int nID = m_Target.id;

	// Evasion value/hit judgment/damage calculation -----------------------------------------//
	if(nID >= USER_BAND && nID < NPC_BAND)	{
		pUser = m_pMain->GetUserPtr(nID - USER_BAND);
		
		if(pUser == NULL)	{
			InitTarget();
			m_NpcState = NPC_STANDING;
			return nStandingTime;
		}

		if(pUser->m_bLive == USER_DEAD)	{
			//SendAttackSuccess(pIOCP, ATTACK_TARGET_DEAD_OK, pUser->m_iUserId, 0, pUser->m_iHP);
			SendAttackSuccess(pIOCP, ATTACK_TARGET_DEAD_OK, pUser->m_iUserId, 0, 0);
			InitTarget();
			m_NpcState = NPC_STANDING;
			return nStandingTime;
		}

		if(pUser->m_state == STATE_DISCONNECTED)	{
			InitTarget();
			m_NpcState = NPC_STANDING;
			return nStandingTime;
		}

		if(pUser->m_byIsOP == MANAGER_USER)	{	// Operators don't attack.
			InitTarget();
			m_NpcState = NPC_MOVING;
			return nStandingTime;
		}
		// Comparing the HP of the NPC and the user... determining whether to run away...
	/*	if(m_byNpcEndAttType)	{
			if(IsCompStatus(pUser) == TRUE)	{
				m_NpcState = NPC_BACK;
				return 0;
			}	
		}	*/

		//if(m_tNpcType == NPC_BOSS_MONSTER)	{		// If it's the boss monster...
		if(m_byWhatAttackType == 4 || m_byWhatAttackType == 5)	{		// If it's a local magic-using monster...
			nRandom = myrand(1, 10000);
			if(nRandom < nPercent)	{				// Area Magic Attack...
				memset( buff, 0x00, 256 );	send_index = 0;
				SetByte( buff, MAGIC_EFFECTING, send_index );		
				SetDWORD( buff, m_iMagic2, send_index );				// Area Magic
				SetShort( buff, m_sNid+NPC_BAND, send_index );
				SetShort( buff, -1, send_index );						// tid must be -1
				SetShort( buff, (short)m_fCurX, send_index );		    // terget point
				SetShort( buff, (short)m_fCurY, send_index );	
				SetShort( buff, (short)m_fCurZ, send_index );	
				SetShort( buff, 0, send_index );	
				SetShort( buff, 0, send_index );	
				SetShort( buff, 0, send_index );

				m_MagicProcess.MagicPacket(buff, send_index, pIOCP);
				//TRACE("++++ AreaMagicAttack --- sid=%d, magicid=%d\n", m_sNid+NPC_BAND, m_iMagic2);
				return m_sAttackDelay + 1000;	// Local magic takes a little time...
			}
		}
		else	{
			if(m_byWhatAttackType == 2)	{	// If it's a poison attacking monster... (with 10% attack)
				nRandom = myrand(1, 10000);

				// sungyong test
				//nRandom = 100;

				if(nRandom < nPercent)	{
					memset( buff, 0x00, 256 );	send_index = 0;
					SetByte( buff, AG_MAGIC_ATTACK_RESULT, send_index );
					SetByte( buff, MAGIC_EFFECTING, send_index );		
					SetDWORD( buff, m_iMagic1, send_index );				// FireBall
					SetShort( buff, m_sNid+NPC_BAND, send_index );
					SetShort( buff, pUser->m_iUserId, send_index );	
					SetShort( buff, 0, send_index );					// data0
					SetShort( buff, 0, send_index );	
					SetShort( buff, 0, send_index );	
					SetShort( buff, 0, send_index );	
					SetShort( buff, 0, send_index );	
					SetShort( buff, 0, send_index );	

					//m_MagicProcess.MagicPacket(buff, send_index, pIOCP);
					SendAll(pIOCP, buff, send_index);

					//TRACE("LongAndMagicAttack --- sid=%d, tid=%d\n", m_sNid+NPC_BAND, pUser->m_iUserId);
					return m_sAttackDelay;
				}
			}
		}

		nDamage = GetFinalDamage(pUser);
		if( m_pMain->m_byTestMode )		nDamage = 10;	// sungyong test
		//TRACE("Npc-Attack() : [mon: x=%.2f, z=%.2f], [user : x=%.2f, z=%.2f]\n", m_fCurX, m_fCurZ, pUser->m_curx, pUser->m_curz);
		
		if(nDamage > 0) {
			pUser->SetDamage(nDamage, m_sNid+NPC_BAND);
			if(pUser->m_bLive != USER_DEAD)	{
			//	TRACE("Npc-Attack Success : [npc=%d, %s]->[user=%d, %s]\n", m_sNid+NPC_BAND, m_strName, pUser->m_iUserId, pUser->m_strUserID);
				SendAttackSuccess(pIOCP, ATTACK_SUCCESS, pUser->m_iUserId, nDamage, pUser->m_sHP);
			}
		}
		else
			SendAttackSuccess(pIOCP, ATTACK_FAIL, pUser->m_iUserId, nDamage, pUser->m_sHP);

		// Decreased durability of defense
	}
	else if(nID >= NPC_BAND && m_Target.id < INVALID_BAND)	{
		pNpc = m_pMain->m_arNpc.GetData(nID - NPC_BAND);
	
		if(pNpc == NULL)	{
			InitTarget();
			m_NpcState = NPC_STANDING;
			return nStandingTime;
		}

		if( m_tNpcType == NPC_HEALER && pNpc->m_byGroup == m_byGroup )	{	// If you are a healer and an NPC of the same country, you will be healed.
			m_NpcState = NPC_HEALING;
			return 0;
		}

		if(pNpc->m_iHP <= 0 || pNpc->m_NpcState == NPC_DEAD)	{
			SendAttackSuccess(pIOCP, ATTACK_TARGET_DEAD, pNpc->m_sNid+NPC_BAND, 0, 0);
			InitTarget();
			m_NpcState = NPC_STANDING;
			return nStandingTime;
		}

		// Comparing the HP of the NPC and the user... determining whether to run away...
	/*	if(IsCompStatus(pUser) == TRUE)	{
			m_NpcState = NPC_BACK;
			return 0;
		}	*/

		// MoveAttack
		//MoveAttack(pIOCP);

		nDamage = GetNFinalDamage(pNpc);

		if(pUser)
		{
			//TRACE("Npc-Attack() : [mon: x=%.2f, z=%.2f], [user : x=%.2f, z=%.2f]\n", m_fCurX, m_fCurZ, pUser->m_curx, pUser->m_curz);
		}
		
		if(nDamage > 0)	{
			pNpc->SetDamage(0, nDamage, m_strName, m_sNid+NPC_BAND, pIOCP);
			//if(pNpc->m_iHP > 0)
			SendAttackSuccess(pIOCP, ATTACK_SUCCESS, pNpc->m_sNid+NPC_BAND, nDamage, pNpc->m_iHP);
		}
		else
			SendAttackSuccess(pIOCP, ATTACK_FAIL, pNpc->m_sNid+NPC_BAND, nDamage, pNpc->m_iHP);
	}

	return m_sAttackDelay;
}

int CNpc::LongAndMagicAttack(CIOCPort* pIOCP)
{
	if(!pIOCP) return 10000;
	
	int ret = 0;
	int nStandingTime = m_sStandTime;
	int send_index = 0;
	char buff[256];
	memset( buff, 0x00, 256 );

	ret = IsCloseTarget(m_byAttackRange, 2);

	if(ret == 0)	{
		m_sStepCount = 0;
		m_byActionFlag = ATTACK_TO_TRACE;
		m_NpcState = NPC_TRACING;			// To catch up with attacking and running away users (faster reaction)
		return 0;							// Update user x, y values in IsCloseTarget() and zoom to Delay = 0
	}	
	else if( ret == 2 )	{
		//if(m_tNpcType != NPC_BOSS_MONSTER)	{		// If it's the boss monster...
		if(m_tNpcLongType == 1)	{		// If it's a long range monster...
			m_sStepCount = 0;
			m_byActionFlag = ATTACK_TO_TRACE;
			m_NpcState = NPC_TRACING;			// To catch up with attacking and running away users (faster reaction)
			return 0;							// Update user x, y values in IsCloseTarget() and zoom to Delay = 0
		}
	}
	if( ret == -1 )	{
		m_NpcState = NPC_STANDING;
		InitTarget();
		return 0;
	}

	CNpc*	pNpc		= NULL;	
	CUser*	pUser		= NULL;
	int		nDamage		= 0;
	int nID = m_Target.id;

	// Evasion value/hit judgment/damage calculation -----------------------------------------//
	if(nID >= USER_BAND && nID < NPC_BAND)	{
		pUser = m_pMain->GetUserPtr(nID - USER_BAND);
		
		if(pUser == NULL)	{
			InitTarget();
			m_NpcState = NPC_STANDING;
			return nStandingTime;
		}

		if(pUser->m_bLive == USER_DEAD)	{
			SendAttackSuccess(pIOCP, ATTACK_TARGET_DEAD_OK, pUser->m_iUserId, 0, 0);
			InitTarget();
			m_NpcState = NPC_STANDING;
			return nStandingTime;
		}

		if(pUser->m_state == STATE_DISCONNECTED)	{
			InitTarget();
			m_NpcState = NPC_STANDING;
			return nStandingTime;
		}

		if(pUser->m_byIsOP == MANAGER_USER)	{		// Operators don't attack.
			InitTarget();
			m_NpcState = NPC_MOVING;
			return nStandingTime;
		}
		// Comparing the HP of the NPC and the user... determining whether to run away...
	/*	if(m_byNpcEndAttType)
		{
			if(IsCompStatus(pUser) == TRUE)
			{
				m_NpcState = NPC_BACK;
				return 0;
			}	
		}	*/

		// Determine the conditions and use magic attack (let's use only magic 1 for now)
		SetByte( buff, MAGIC_CASTING, send_index );		
		SetDWORD( buff, m_iMagic1, send_index );				// FireBall
		SetShort( buff, m_sNid+NPC_BAND, send_index );
		SetShort( buff, pUser->m_iUserId, send_index );	
		SetShort( buff, 0, send_index );					// data0
		SetShort( buff, 0, send_index );	
		SetShort( buff, 0, send_index );	
		SetShort( buff, 0, send_index );	
		SetShort( buff, 0, send_index );	
		SetShort( buff, 0, send_index );	

		m_MagicProcess.MagicPacket(buff, send_index, pIOCP);

		//TRACE("**** LongAndMagicAttack --- sid=%d, tid=%d\n", m_sNid+NPC_BAND, pUser->m_iUserId);
	}
	else if(nID >= NPC_BAND && m_Target.id < INVALID_BAND)	{
		pNpc = m_pMain->m_arNpc.GetData(nID - NPC_BAND);
		//pNpc = m_pMain->m_arNpc[nID - NPC_BAND];
		
		if(pNpc == NULL)	{
			InitTarget();
			m_NpcState = NPC_STANDING;
			return nStandingTime;
		}

		if(pNpc->m_iHP <= 0 || pNpc->m_NpcState == NPC_DEAD)	{
			SendAttackSuccess(pIOCP, ATTACK_TARGET_DEAD, pNpc->m_sNid+NPC_BAND, 0, 0);
			InitTarget();
			m_NpcState = NPC_STANDING;
			return nStandingTime;
		}

		// Comparing the HP of the NPC and the user... determining whether to run away...
	/*	if(IsCompStatus(pUser) == TRUE)
		{
			m_NpcState = NPC_BACK;
			return 0;
		}	*/

		/*
		SetByte( buff, AG_LONG_MAGIC_ATTACK, send_index );
		SetByte( buff, type, send_index );
		SetDWORD( buff, magicid, send_index );
		SetShort( buff, m_sNid+NPC_BAND, send_index );
		SetShort( buff, pNpc->m_sNid+NPC_BAND, send_index );	*/
	}

	return m_sAttackDelay;
}

int CNpc::TracingAttack(CIOCPort* pIOCP)		// 0:attack fail, 1:attack success
{
	if(!pIOCP) return 0;
	
	CNpc*	pNpc		= NULL;	
	CUser*	pUser		= NULL;

	int		nDamage		= 0;

	int nID = m_Target.id;

	if(nID >= USER_BAND && nID < NPC_BAND)	{
		pUser = m_pMain->GetUserPtr(nID - USER_BAND);
		if(pUser == NULL)	return 0;
		if(pUser->m_bLive == USER_DEAD)		{
			SendAttackSuccess(pIOCP, ATTACK_TARGET_DEAD_OK, pUser->m_iUserId, 0, 0);
			return 0;
		}
		if(pUser->m_state == STATE_DISCONNECTED)	return 0;
		if(pUser->m_byIsOP == MANAGER_USER)		return 0;

		nDamage = GetFinalDamage(pUser);
		if( m_pMain->m_byTestMode )		nDamage = 1;	// sungyong test
		
		if(nDamage > 0)		{
			pUser->SetDamage(nDamage, m_sNid+NPC_BAND);
			if(pUser->m_bLive != USER_DEAD)	{
			//	TRACE("Npc-Attack Success : [npc=%d, %s]->[user=%d, %s]\n", m_sNid+NPC_BAND, m_strName, pUser->m_iUserId, pUser->m_strUserID);
				SendAttackSuccess(pIOCP, ATTACK_SUCCESS, pUser->m_iUserId, nDamage, pUser->m_sHP);
			}
		}
		else
			SendAttackSuccess(pIOCP, ATTACK_FAIL, pUser->m_iUserId, nDamage, pUser->m_sHP);
	}
	else if(nID >= NPC_BAND && m_Target.id < INVALID_BAND)	{
		pNpc = m_pMain->m_arNpc.GetData(nID - NPC_BAND);
		
		if(pNpc == NULL)	return 0;

		if(pNpc->m_iHP <= 0 || pNpc->m_NpcState == NPC_DEAD)	{
			SendAttackSuccess(pIOCP, ATTACK_TARGET_DEAD, pNpc->m_sNid+NPC_BAND, 0, 0);
			return 0;
		}

		nDamage = GetNFinalDamage(pNpc);
		//TRACE("Npc-Attack() : [mon: x=%.2f, z=%.2f], [user : x=%.2f, z=%.2f]\n", m_fCurX, m_fCurZ, pUser->m_curx, pUser->m_curz);
		
		if(nDamage > 0)		{
			if(pNpc->SetDamage(0, nDamage, m_strName, m_sNid+NPC_BAND, pIOCP) == TRUE) {
				SendAttackSuccess(pIOCP, ATTACK_SUCCESS, pNpc->m_sNid+NPC_BAND, nDamage, pNpc->m_iHP);
			}
			else{
				SendAttackSuccess(pIOCP, ATTACK_SUCCESS, pNpc->m_sNid+NPC_BAND, nDamage, pNpc->m_iHP);
				SendAttackSuccess(pIOCP, ATTACK_TARGET_DEAD, pNpc->m_sNid+NPC_BAND, nDamage, pNpc->m_iHP);
				return 0;
			}
		}
		else{
			SendAttackSuccess(pIOCP, ATTACK_FAIL, pNpc->m_sNid+NPC_BAND, nDamage, pNpc->m_iHP);
		}
	}

	return 1;
}

void CNpc::MoveAttack(CIOCPort* pIOCP)
{
	char pBuf[1024];
	::ZeroMemory(pBuf, 1024);	
	int index = 0;
	__Vector3 vUser;
	__Vector3 vNpc;
	__Vector3 vDistance;
	__Vector3 vEnd22;
	CUser* pUser = NULL;
	CNpc* pNpc = NULL;
	float fDis = 0.0f;
	float fX = 0.0f, fZ = 0.0f;
	vNpc.Set(m_fCurX, m_fCurY, m_fCurZ);

	// 1m
	//float surround_fx[8] = {0.0f, -0.7071f, -1.0f, -0.7083f,  0.0f,  0.7059f,  1.0000f, 0.7083f};
	//float surround_fz[8] = {1.0f,  0.7071f,  0.0f, -0.7059f, -1.0f, -0.7083f, -0.0017f, 0.7059f};
	// 2m
	float surround_fx[8] = {0.0f, -1.4142f, -2.0f, -1.4167f,  0.0f,  1.4117f,  2.0000f, 1.4167f};
	float surround_fz[8] = {2.0f,  1.4142f,  0.0f, -1.4167f, -2.0f, -1.4167f, -0.0035f, 1.4117f};

	if(m_Target.id >= USER_BAND && m_Target.id < NPC_BAND)
	{
		pUser = m_pMain->GetUserPtr(m_Target.id - USER_BAND);
		if(pUser == NULL) 
		{
			InitTarget();
			return;
		}
		vUser.Set(pUser->m_curx, pUser->m_cury, pUser->m_curz); 

		vEnd22 = CalcAdaptivePosition(vNpc, vUser, 2);

		if(m_byAttackPos > 0 && m_byAttackPos < 9)
		{
			fX = vUser.x + surround_fx[m_byAttackPos-1];	fZ = vUser.z + surround_fz[m_byAttackPos-1];
			vEnd22.Set(fX, 0, fZ);
			//TRACE("MoveAttack 11 - nid(%s, %d), fx=%.2f, fz=%.2f, attackpos=%d\n", m_strName, m_sNid+NPC_BAND, fX, fZ, m_byAttackPos);
		}
		else
		{
			fX = vEnd22.x;	fZ = vEnd22.z;
			//TRACE("MoveAttack 22 - nid(%s, %d), fx=%.2f, fz=%.2f, attackpos=%d\n", m_strName, m_sNid+NPC_BAND, fX, fZ, m_byAttackPos);
		}
	}
	else if(m_Target.id >= NPC_BAND && m_Target.id < INVALID_BAND)
	{
		pNpc = m_pMain->m_arNpc.GetData(m_Target.id - NPC_BAND);		
		//pNpc = m_pMain->m_arNpc[m_Target.id - NPC_BAND];
		if(pNpc == NULL) 
		{
			InitTarget();
			return;
		}
		vUser.Set(pNpc->m_fCurX, pNpc->m_fCurY, pNpc->m_fCurZ); 

		vEnd22 = CalcAdaptivePosition(vNpc, vUser, 2);
		fX = vEnd22.x;	fZ = vEnd22.z;
	}
	
	vDistance = vUser - vNpc;
	fDis = vDistance.Magnitude();	
	
	if((int)fDis < 3) return;	// If the distance from the target is less than 3 meters, it is an attack while stopped.
/*	if(m_tNpcLongType)		// A long-range attack is judged by the attack distance.
	{
		if((int)fDis > nRange) return FALSE; 
	}	
	else					// short range (direct attack)
	{
		if(fDis > 2.5) return FALSE;			// Task: Temporarily modify the attack distance to 2.5.
	}	*/

	vDistance = vEnd22 - vNpc;
	fDis = vDistance.Magnitude();	
	m_fCurX = vEnd22.x;
	m_fCurZ = vEnd22.z;

	if(m_fCurX < 0 || m_fCurZ < 0)
	{
		TRACE("Npc-MoveAttack : nid=(%d, %s), x=%.2f, z=%.2f\n", m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ);
	}

	// moving attack...
	::ZeroMemory(pBuf, 1024);	index = 0;	
	SetByte(pBuf, MOVE_RESULT, index);
	SetByte(pBuf, SUCCESS, index);
	SetShort(pBuf, m_sNid+NPC_BAND, index);
	Setfloat(pBuf, m_fCurX, index);
	Setfloat(pBuf, m_fCurZ, index);
	Setfloat(pBuf, m_fCurY, index);
	Setfloat(pBuf, fDis, index);
	//TRACE("Npc moveattack --> nid = %d, cur=[x=%.2f, y=%.2f, metor=%.2f]\n", m_sNid+NPC_BAND, m_fCurX, m_fCurZ, fDis);
	SendAll(pIOCP, pBuf, index);

	// move end
	::ZeroMemory(pBuf, 1024);	index = 0;	
	SetByte(pBuf, MOVE_RESULT, index);
	SetByte(pBuf, SUCCESS, index);
	SetShort(pBuf, m_sNid+NPC_BAND, index);
	Setfloat(pBuf, m_fCurX, index);
	Setfloat(pBuf, m_fCurZ, index);
	Setfloat(pBuf, m_fCurY, index);
	Setfloat(pBuf, 0, index);
	//TRACE("Npc moveattack end --> nid = %d, cur=[x=%.2f, y=%.2f, metor=%d]\n", m_sNid+NPC_BAND, m_fCurX, m_fCurZ, 0);
	SendAll(pIOCP, pBuf, index);

	SetUid(m_fCurX, m_fCurZ, m_sNid + NPC_BAND);
	
	/* Update the target coordinates to the latest, and update the last pointer coordinates,, */
	m_fEndPoint_X = m_fCurX;
	m_fEndPoint_Y = m_fCurZ;

	//m_Target.x = fX;
	//m_Target.z = fZ;
}

int CNpc::GetNFinalDamage(CNpc *pNpc)
{
	short damage = 0;
	float Attack = 0;
	float Avoid = 0;
	short Hit = 0;
	short Ac = 0;
	int random = 0;
	BYTE result;

	if(pNpc == NULL)	return damage;

	Attack = (float)m_sHitRate;

	Avoid = (float)pNpc->m_sEvadeRate;

	Hit = m_sDamage;
	
	Ac = (short)pNpc->m_sDefense;

	// Finding the Hitting Ratio
	result = GetHitRate(Attack/Avoid);		

	switch(result)
	{
//	case GREAT_SUCCESS:
//		damage = (short)(0.6 * (2 * Hit));
//		if(damage <= 0){
//			damage = 0;
//			break;
//		}
//		damage = myrand(0, damage);
//		damage += (short)(0.7 * (2 * Hit));
//		break;
	case GREAT_SUCCESS:
		damage = (short)(0.6 * Hit);
		if(damage <= 0){
			damage = 0;
			break;
		}
		damage = myrand(0, damage);
		damage += (short)(0.7 * Hit);
		break;
	case SUCCESS:
	case NORMAL:
		if(Hit - Ac > 0)
		{
			damage = (short)(0.6 * (Hit - Ac));
			if(damage <= 0){
				damage = 0;
			break;
			}
			damage = myrand(0, damage);
			damage += (short)(0.7 * (Hit - Ac));
		}
		else
			damage = 0;
		break;
	case FAIL:
		damage = 0;
		break;
	}
	
	return damage;	
}

BOOL CNpc::IsCompStatus(CUser* pUser)
{
	
	if(IsHPCheck(pUser->m_sHP) == TRUE)
	{
		if(RandomBackMove() == FALSE)
			return FALSE;
		else 
			return TRUE;
	}

	return FALSE;
}

//	Determine if the location of the target has changed enough to find the way again
BOOL CNpc::IsChangePath(int nStep)
{
	// With the last coordinates of Pathfind,, Judging whether the Target is within my attack range,,
//	if(!m_pPath) return TRUE;

	float fCurX=0.0f, fCurZ=0.0f;
	GetTargetPos(fCurX, fCurZ);

	__Vector3 vStart, vEnd;
	vStart.Set(m_fEndPoint_X, 0, m_fEndPoint_Y);
	vEnd.Set(fCurX, 0, fCurZ);
	
	float fDis = GetDistance(vStart, vEnd);
	float fCompDis = 3.0f;

	//if(fDis <= m_byAttackRange)
	if(fDis < fCompDis)
	{
		//TRACE("#### Npc-IsChangePath() : [nid=%d] -> attack range in #####\n", m_sNid);
		return FALSE;
	}

	 //TRACE("#### IsChangePath() - [Mon - cur:x=%.2f, z=%.2f, target point:x=%.2f, z=%.2f], [target : x=%.2f, z=%.2f]\n", 
	//	 m_fCurX, m_fCurZ, m_fEndPoint_X, m_fEndPoint_Y, fCurX, fCurZ);
	return TRUE;
}

//	Get the current position of Target.
BOOL CNpc::GetTargetPos(float& x, float& z)
{
	if(m_Target.id >= USER_BAND && m_Target.id < NPC_BAND)	// If Target is User
	{
		CUser* pUser = m_pMain->GetUserPtr(m_Target.id - USER_BAND);

		if(!pUser) return FALSE;

		x = pUser->m_curx;
		z = pUser->m_curz;
	}
	else if(m_Target.id >= NPC_BAND && m_Target.id < INVALID_BAND)
	{
		CNpc* pNpc = m_pMain->m_arNpc.GetData(m_Target.id - NPC_BAND);
		//CNpc* pNpc = m_pMain->m_arNpc[m_Target.id - NPC_BAND];
		if(!pNpc) return FALSE;

		x = pNpc->m_fCurX;
		z = pNpc->m_fCurZ;

	}

	return TRUE;
}

//	Re-route between Target and NPC.
BOOL CNpc::ResetPath()
{
	float cur_x, cur_z;
	GetTargetPos(cur_x, cur_z);

//	TRACE("ResetPath : user pos ,, x=%.2f, z=%.2f\n", cur_x, cur_z);

	m_Target.x = cur_x;
	m_Target.z = cur_z;

	int nValue = GetTargetPath();
	if(nValue == -1)		// As the target disappears or becomes distant...
	{
		TRACE("Npc-ResetPath Fail - target_x = %.2f, z=%.2f, value=%d\n", m_Target.x, m_Target.z, nValue);
		return FALSE;
	}
	else if(nValue == 0)	// Go straight to the target.
	{
		m_fSecForMetor = m_fSpeed_2;	// When attacking, run at speed...
		IsNoPathFind(m_fSecForMetor);
	}

	//TRACE("Npc-ResetPath - target_x = %.2f, z=%.2f, value=%d\n", m_Target.x, m_Target.z, nValue);

	return TRUE;	
}

int CNpc::GetFinalDamage(CUser *pUser, int type)
{
	short damage = 0;
	float Attack = 0;
	float Avoid = 0;
	short Hit = 0;
	short Ac = 0;
	short HitB = 0;
	int random = 0;
	BYTE result;

	if(pUser == NULL)	return damage;
	
	Attack = (float)m_sHitRate;		// attack agility
	Avoid = (float)pUser->m_fAvoidrate;		// defensive agility
	Hit = m_sDamage;	// Attacker Hit
//	Ac = (short)pUser->m_sAC ;	// Defender Ac

//	Ac = (short)pUser->m_sItemAC + (short)pUser->m_sLevel ;	// Defender Ac
//	Ac = (short)pUser->m_sAC - (short)pUser->m_sLevel ;	// Defender Ac. Ying... I hate Mr. Seongrae TT.TT
	Ac = (short)pUser->m_sItemAC + (short)pUser->m_sLevel + (short)(pUser->m_sAC - pUser->m_sLevel - pUser->m_sItemAC);

//	ASSERT(Ac != 0);
//	short kk = (short)pUser->m_sItemAC;
//	short tt = (short)pUser->m_sLevel;
//	Ac = kk + tt;

	HitB = (int)((Hit * 200) / (Ac + 240)) ;

	int nMaxDamage = (int)(2.6 * m_sDamage);

	// Finding the Hitting Ratio
	result = GetHitRate(Attack/Avoid);	
	
//	TRACE("Hitrate : %d     %f/%f\n", result, Attack, Avoid);

	switch(result)
	{
	case GREAT_SUCCESS:
/*
		damage = (short)(0.6 * (2 * Hit));
		if(damage <= 0){
			damage = 0;
			break;
		}
		damage = myrand(0, damage);
		damage += (short)(0.7 * (2 * Hit));
		break;
*/	
//		damage = 0;
//		break;

		damage = (short)HitB;
		if(damage <= 0){
			damage = 0;
			break;
		}

		damage = (int)(0.3f * myrand(0, damage));
		damage += (short)(0.85f * (float)HitB);
//		damage = damage * 2;
		damage = (damage * 3) / 2;
		break;

	case SUCCESS:
/*
		damage = (short)(0.6f * Hit);
		if(damage <= 0){
			damage = 0;
			break;
		}
		damage = myrand(0, damage);
		damage += (short)(0.7f * Hit);
		break;
*/
/*
		damage = (short)(0.3f * (float)HitB);
		if(damage <= 0){
			damage = 0;
			break;
		}
		damage = myrand(0, damage);
		damage += (short)(0.85f * (float)HitB);
*/
/*
		damage = (short)HitB;
		if(damage <= 0){
			damage = 0;
			break;
		}
		damage = (int)(0.3f * myrand(0, damage));
		damage += (short)(0.85f * (float)HitB);
		damage = damage * 2;

		break;
*/
	case NORMAL:
		/*
		if(Hit - Ac > 0){
			damage = (short)(0.6f * (Hit - Ac));
			if(damage <= 0){
				damage = 0;
				break;
			}
			damage = myrand(0, damage);
			damage += (short)(0.7f * (Hit - Ac));

		}
		else
			damage = 0;
		*/
/*
		damage = (short)(0.3f * (float)HitB);
		if(damage <= 0){
			damage = 0;
			break;
		}
		damage = myrand(0, damage);
		damage += (short)(0.85f * (float)HitB);
*/
		damage = (short)HitB;
		if(damage <= 0){
			damage = 0;
			break;
		}
		damage = (int)(0.3f * myrand(0, damage));
		damage += (short)(0.85f * (float)HitB);

		break;

	case FAIL:
		damage = 0;

		break;
	}
	
//	TRACE ("%d\n", damage) ;

	if(damage > nMaxDamage)	{
		TRACE("#### Npc-GetFinalDamage Fail : nid=%d, result=%d, damage=%d, maxdamage=%d\n", m_sNid+NPC_BAND, result, damage, nMaxDamage);
		damage = nMaxDamage;
	}

	// sungyong test
	return damage;	
	//return 1;
}

//	Target the user who attacked me. (Criteria: Selected based on level and HP)
void CNpc::ChangeTarget(int nAttackType, CUser *pUser, CIOCPort* pIOCP)
{
	int preDamage, lastDamage;
	__Vector3 vUser, vNpc;
	float fDistance1 = 0.0f, fDistance2 = 0.0f;
	int iRandom = myrand(0, 100);

	if(pUser == NULL) return;
	if(pUser->m_bLive == USER_DEAD) return;
	if(pUser->m_bNation == m_byGroup)	return;		// The same country should not attack...
	if(pUser->m_byIsOP == MANAGER_USER) return;
	if(m_tNpcType == NPC_DOOR || m_tNpcType == NPC_ARTIFACT || m_tNpcType == NPC_PHOENIX_GATE || m_tNpcType == NPC_GATE_LEVER || m_tNpcType == NPC_DOMESTIC_ANIMAL || m_tNpcType == NPC_SPECIAL_GATE || m_tNpcType == NPC_DESTORY_ARTIFACT )		return;		// 성문 NPC는 공격처리 안하게
	if(m_NpcState == NPC_FAINTING)	return;		// Ignore if you're in a state of fainting.

	CUser *preUser = NULL;
	if(m_Target.id >= 0 && m_Target.id < NPC_BAND)	
		preUser = m_pMain->GetUserPtr(m_Target.id - USER_BAND);

	if(pUser == preUser)	{
		if(m_tNpcGroupType)	 {			// If you are a family type, target the same type within your line of sight.
			m_Target.failCount = 0;
			if(m_tNpcType == NPC_BOSS_MONSTER)	FindFriend(1);
			else		FindFriend();
		}
		else	{
			if(m_tNpcType == NPC_BOSS_MONSTER)	{
				m_Target.failCount = 0;
				FindFriend(1);
			}
		}
		return;
	}
	if(preUser != NULL/* && preUser->m_state == STATE_GAMESTARTED */)
	{
		preDamage = 0; lastDamage = 0;

		if(iRandom >= 0 && iRandom < 50)	{			// The user who hit the monster the hardest
			preDamage = preUser->GetDamage(m_sNid+NPC_BAND);
			lastDamage = pUser->GetDamage(m_sNid+NPC_BAND);
			//TRACE("Npc-changeTarget 111 - iRandom=%d, pre=%d, last=%d\n", iRandom, preDamage, lastDamage);
			if(preDamage > lastDamage) return;
		}
		else if(iRandom >= 50 && iRandom < 80)	{		// nearest player
			vNpc.Set(m_fCurX, m_fCurY, m_fCurZ);
			vUser.Set(preUser->m_curx, 0, preUser->m_curz);
			fDistance1 = GetDistance(vNpc, vUser);
			vUser.Set(pUser->m_curx, 0, pUser->m_curz);
			fDistance2 = GetDistance(vNpc, vUser);
			//TRACE("Npc-changeTarget 222 - iRandom=%d, preDis=%.2f, lastDis=%.2f\n", iRandom, fDistance1, fDistance2);
			if(fDistance2 > fDistance1)	return;
		}
		if(iRandom >= 80 && iRandom < 95)		{		// The user whose monster can hit the user the most
			preDamage = GetFinalDamage(preUser, 0);
			lastDamage = GetFinalDamage(pUser, 0); 
			//TRACE("Npc-changeTarget 333 - iRandom=%d, pre=%d, last=%d\n", iRandom, preDamage, lastDamage);
			if(preDamage > lastDamage) return;
		}
		if(iRandom >= 95 && iRandom < 101)		{		// Users who have used Heal Magic
		}
	}
	else if(preUser == NULL && nAttackType == 1004)		return;		// Do not react to heal magic..
		
	m_Target.id	= pUser->m_iUserId + USER_BAND;
	m_Target.x	= pUser->m_curx;
	m_Target.y	= pUser->m_cury;
	m_Target.z  = pUser->m_curz;

	//TRACE("Npc-changeTarget - target_x = %.2f, z=%.2f\n", m_Target.x, m_Target.z);

	int nValue = 0;
	// It's wandering around, but if you attack, counterattack right away
	if(m_NpcState == NPC_STANDING || m_NpcState == NPC_MOVING || m_NpcState == NPC_SLEEPING)
	{									// If you are close, it will lead to a counterattack
		if(IsCloseTarget(pUser, m_byAttackRange) == TRUE)
		{
			m_NpcState = NPC_FIGHTING;
			m_Delay = 0;
			m_fDelayTime = TimeGet();
		}
		else							// If you run away right away, update coordinates and track	
		{
			nValue = GetTargetPath(1);
			if(nValue == 1)	// There is a slight delay after counterattack action	
			{
				m_NpcState = NPC_TRACING;
				m_Delay = 0;
				m_fDelayTime = TimeGet();
			}
			else if(nValue == -1)
			{
				m_NpcState = NPC_STANDING;
				m_Delay = 0;
				m_fDelayTime = TimeGet();
			}
			else if(nValue == 0)
			{
				m_fSecForMetor = m_fSpeed_2;	// When attacking, run at speed...
				IsNoPathFind(m_fSecForMetor);
				m_NpcState = NPC_TRACING;
				m_Delay = 0;
				m_fDelayTime = TimeGet();
			}
		}
	}
//	else m_NpcState = NPC_ATTACKING;	// Attack for a long time, but if someone interrupts, change the target

	if(m_tNpcGroupType)	 {			// If you are a family type, target the same type within your line of sight.
		m_Target.failCount = 0;
		if(m_tNpcType == NPC_BOSS_MONSTER)	FindFriend(1);
		else		FindFriend();
	}
	else	{
		if(m_tNpcType == NPC_BOSS_MONSTER)	{
			m_Target.failCount = 0;
			FindFriend(1);
		}
	}
}

//	Target the NPC that attacked me. (Criteria: Selected based on level and HP)
void CNpc::ChangeNTarget(CNpc *pNpc, CIOCPort* pIOCP)
{
	int preDamage, lastDamage;
	__Vector3 vMonster, vNpc;
	float fDist = 0.0f;

	if(pNpc == NULL) return;
	if(pNpc->m_NpcState == NPC_DEAD) return;

	CNpc *preNpc = NULL;
	if(m_Target.id >= 0 && m_Target.id < NPC_BAND)
	{
	}
	else if(m_Target.id >= NPC_BAND && m_Target.id < INVALID_BAND)
	{
		preNpc = m_pMain->m_arNpc.GetData(m_Target.id - NPC_BAND);
	}

	if(pNpc == preNpc) return;

	if(preNpc != NULL)
	{
		preDamage = 0; lastDamage = 0;
		preDamage = GetNFinalDamage(preNpc);
		lastDamage = GetNFinalDamage(pNpc); 

		// Search conditions,, (by judging the distance and user's attack power,,)
		vNpc.Set(m_fCurX, m_fCurY, m_fCurZ);
		vMonster.Set(preNpc->m_fCurX, 0, preNpc->m_fCurZ);
		fDist = GetDistance(vNpc, vMonster);
		preDamage = (int)((double)preDamage/fDist + 0.5);
		vMonster.Set(pNpc->m_fCurX, 0, pNpc->m_fCurZ);
		fDist = GetDistance(vNpc, vMonster);
		lastDamage = (int)((double)lastDamage/fDist + 0.5);

		if(preDamage > lastDamage) return;
	}
		
	m_Target.id	= pNpc->m_sNid + NPC_BAND;
	m_Target.x	= pNpc->m_fCurX;
	m_Target.y	= pNpc->m_fCurZ;
	m_Target.z  = pNpc->m_fCurZ;

	int nValue = 0;
	// It's wandering around, but if you attack, counterattack right away
	if(m_NpcState == NPC_STANDING || m_NpcState == NPC_MOVING || m_NpcState == NPC_SLEEPING)
	{									// If you are close, it will lead to a counterattack
		if(IsCloseTarget(m_byAttackRange) == 1)
		{
			m_NpcState = NPC_FIGHTING;
			m_Delay = 0;
			m_fDelayTime = TimeGet();
		}
		else							// If you run away right away, update coordinates and track
		{
			nValue = GetTargetPath();
			if(nValue == 1)	// There is a slight delay after counterattack action	
			{
				m_NpcState = NPC_TRACING;
				m_Delay = 0;
				m_fDelayTime = TimeGet();
			}
			else if(nValue == -1)
			{
				m_NpcState = NPC_STANDING;
				m_Delay = 0;
				m_fDelayTime = TimeGet();
			}
			else if(nValue == 0)
			{
				m_fSecForMetor = m_fSpeed_2;	// When attacking, run at speed...
				IsNoPathFind(m_fSecForMetor);
				m_NpcState = NPC_TRACING;
				m_Delay = 0;
				m_fDelayTime = TimeGet();
			}
		}
	}
//	else m_NpcState = NPC_ATTACKING;	// Attack for a long time, but if someone interrupts, change the target

	if(m_tNpcGroupType)					// If you are a family type, target the same type within your line of sight.
	{
		m_Target.failCount = 0;
		FindFriend();
	}
}


void CNpc::ToTargetMove(CIOCPort* pIOCP, CUser* pUser)
{
	TRACE("### ToTargetMove() 유저 길찾기 실패 ### \n");
}

//	Retrieves the NPC's defense.
int CNpc::GetDefense()
{
	return m_sDefense;
}

//	Damage calculation, death if m_iHP is less than 0
BOOL CNpc::SetDamage(int nAttackType, int nDamage, TCHAR *id, int uid, CIOCPort* pIOCP)
{
	int i=0, len=0;
	int userDamage = 0;
	BOOL bFlag = FALSE;
	_ExpUserList *tempUser = NULL;

	if(m_NpcState == NPC_DEAD) return TRUE;
	if(m_iHP <= 0) return TRUE;
	if(nDamage < 0) return TRUE;
	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size()) {		// Returns if the npc's pointer is invalid.
		TRACE("#### Npc-Setdamage ZoneIndex Fail : [name=%d,%s], zoneindex=%d #####\n", m_sNid+NPC_BAND, m_strName, m_ZoneIndex);
		return TRUE;
	}

	CUser* pUser = NULL;
	CNpc* pNpc = NULL;
	char strDurationID[MAX_ID_SIZE+1];
	memset(strDurationID, 0x00, MAX_ID_SIZE+1);

	if(uid >= USER_BAND && uid < NPC_BAND)	{
		pUser = m_pMain->GetUserPtr(uid);
		if(pUser == NULL) return TRUE;
	}
	else if(uid >= NPC_BAND && m_Target.id < INVALID_BAND)	{
		pNpc = m_pMain->m_arNpc.GetData(uid - NPC_BAND);
		if(pNpc == NULL) return TRUE;
		userDamage = nDamage;		
		goto go_result;
	}

	userDamage = nDamage;		
													// Excess damage is useless.
	if( (m_iHP - nDamage) < 0 ) userDamage = m_iHP;

	for(i = 0; i < NPC_HAVE_USER_LIST; i++)	{
		if(m_DamagedUserList[i].iUid == uid)	{
			if(_stricmp("**duration**", id) == 0) {
				bFlag = TRUE;
				strcpy(strDurationID, pUser->m_strUserID);
				if(_stricmp(m_DamagedUserList[i].strUserID, strDurationID) == 0)	{
					m_DamagedUserList[i].nDamage += userDamage; 
					goto go_result;
				}
			}
			else if(_stricmp(m_DamagedUserList[i].strUserID, id) == 0) 
			{ 
				m_DamagedUserList[i].nDamage += userDamage; 
				goto go_result;
			}
		}
	}

	for(i = 0; i < NPC_HAVE_USER_LIST; i++)				// Does the number limit affect the final damage?
	{
		if(m_DamagedUserList[i].iUid == -1)
		{
			if(m_DamagedUserList[i].nDamage <= 0)
			{
				len = strlen(id);
				if( len > MAX_ID_SIZE || len <= 0 ) {
					TRACE("###  Npc SerDamage Fail ---> uid = %d, name=%s, len=%d, id=%s  ### \n", m_sNid+NPC_BAND, m_strName, len, id);
					continue;
				}
				if(bFlag == TRUE)	strcpy(m_DamagedUserList[i].strUserID, strDurationID);
				else	{
					if(_stricmp("**duration**", id) == 0) {
						strcpy(m_DamagedUserList[i].strUserID, pUser->m_strUserID);
					}
					else strcpy(m_DamagedUserList[i].strUserID, id);
				}
				m_DamagedUserList[i].iUid = uid;
				m_DamagedUserList[i].nDamage = userDamage;
				m_DamagedUserList[i].bIs = FALSE;
				break;
			}
		}
	}

go_result:
	m_TotalDamage += userDamage;
	m_iHP -= nDamage;	

	if( m_iHP <= 0 )
	{
	//	m_ItemUserLevel = pUser->m_sLevel;
		m_iHP = 0;
		Dead(pIOCP);
		return FALSE;
	}

	int iRandom = myrand(1, 100);
	int iLightningR = 0;

	if(uid >= USER_BAND && uid < NPC_BAND)
	{
		if(nAttackType == 3 && m_NpcState != NPC_FAINTING)	{			// If you used a stun skill...
			// Probability Calculation..
			iLightningR = 10 + (40 - 40 * ( (double)m_byLightningR / 80) );
			if( COMPARE(iRandom, 0, iLightningR) )	{
				m_NpcState = NPC_FAINTING;
				m_Delay = 0;
				m_fDelayTime = TimeGet();
				m_fFaintingTime = TimeGet();
			}
			else	ChangeTarget(nAttackType, pUser, pIOCP);
		}
		else	{
			ChangeTarget(nAttackType, pUser, pIOCP);
		}
	}
	if(uid >= NPC_BAND && m_Target.id < INVALID_BAND)
	{
		ChangeNTarget(pNpc, pIOCP);
	}

	return TRUE;
}

// Heal-type magic attack
BOOL CNpc::SetHMagicDamage(int nDamage, CIOCPort* pIOCP)
{
	if(m_NpcState == NPC_DEAD) return FALSE;
	if(m_iHP <= 0) return FALSE;
	if(nDamage <= 0) return FALSE;
	if( m_iHP < 1 )	return FALSE;	// You can't recover when you're on the verge of death...
	if(pIOCP == NULL)	return FALSE;

	char buff[256];
	memset( buff, 0x00, 256 );
	int send_index = 0, oldHP = 0;

	oldHP = m_iHP;
	m_iHP += nDamage;
	if( m_iHP < 0 )
		m_iHP = 0;
	else if ( m_iHP > m_iMaxHP )
		m_iHP = m_iMaxHP;

	TRACE("Npc - SetHMagicDamage(), nid=%d,%s, oldHP=%d -> curHP=%d\n", m_sNid+NPC_BAND, m_strName, oldHP, m_iHP);

	SetByte( buff, AG_USER_SET_HP, send_index );
	SetShort( buff, m_sNid+NPC_BAND, send_index );
	SetDWORD( buff, m_iHP, send_index );
	SendAll(pIOCP, buff, send_index);  

	return TRUE;
}

//	Calculate the distribution of experience points when dealing with NPC death.
void CNpc::SendExpToUserList()
{
	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size()) {
		TRACE("#### Npc-SendExpToUserList() ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return;
	}
	int i=0;
	int nExp = 0;
	int nPartyExp = 0;
	int nLoyalty = 0;
	int nPartyLoyalty = 0;
	double totalDamage = 0;
	double CompDamage = 0;
	double TempValue = 0;
	int nPartyNumber = -1;
	int nUid = -1;
	CUser* pUser = NULL;
	CUser* pPartyUser = NULL;
	CUser* pMaxDamageUser = NULL;
	_PARTY_GROUP* pParty = NULL;
	char strMaxDamageUser[MAX_ID_SIZE+1];	memset(strMaxDamageUser, 0x00, MAX_ID_SIZE+1 );
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(pMap == NULL) return;

	IsUserInSight();	// User settings within the field of view..
				
	for(i = 0; i < NPC_HAVE_USER_LIST; i++)				// Once you search the list.
	{
		if(m_DamagedUserList[i].iUid < 0 || m_DamagedUserList[i].nDamage<= 0) continue;
		if(m_DamagedUserList[i].bIs == TRUE) pUser = m_pMain->GetUserPtr(m_DamagedUserList[i].iUid);
		if(pUser == NULL) continue;
		
		if(pUser->m_byNowParty == 1)			// party affiliation
		{	
			totalDamage = GetPartyDamage(pUser->m_sPartyNumber);
			if(totalDamage == 0 || m_TotalDamage == 0)
				nPartyExp = 0;
			else	{
				if( CompDamage < totalDamage )	{	// 
					CompDamage = totalDamage;
					m_sMaxDamageUserid = m_DamagedUserList[i].iUid;
					pMaxDamageUser = m_pMain->GetUserPtr(m_DamagedUserList[i].iUid);
					if(pMaxDamageUser == NULL)	{
						m_byMaxDamagedNation = pUser->m_bNation;
						strcpy( strMaxDamageUser, pUser->m_strUserID );
					}
					else	{
						m_byMaxDamagedNation = pMaxDamageUser->m_bNation;
						strcpy( strMaxDamageUser, pMaxDamageUser->m_strUserID );
					}
				}

				TempValue = m_iExp * (totalDamage / m_TotalDamage);
				nPartyExp = (int)TempValue;
				if(TempValue > nPartyExp)	nPartyExp = nPartyExp + 1;
			}
			if(m_iLoyalty == 0 || totalDamage == 0 || m_TotalDamage == 0)
				nPartyLoyalty = 0;
			else	{
				TempValue = m_iLoyalty * (totalDamage / m_TotalDamage);
				nPartyLoyalty = (int)TempValue;
				if(TempValue > nPartyLoyalty)	nPartyLoyalty = nPartyLoyalty + 1;
			}
			// Distribute experience points to all party members
			if(i != 0)
			{
				BOOL bFlag = FALSE;
				int count = 0;
				for(int j=0; j<i; j++)
				{
					if(m_DamagedUserList[j].iUid < 0 || m_DamagedUserList[j].nDamage<= 0) continue;
					if(m_DamagedUserList[j].bIs == TRUE) pPartyUser = m_pMain->GetUserPtr(m_DamagedUserList[j].iUid);
					if(pPartyUser == NULL) continue;
					if(pUser->m_sPartyNumber == pPartyUser->m_sPartyNumber)	continue;
					count++;
				}

				if(count == i)	bFlag = TRUE;

				if(bFlag == TRUE)	{
					int uid = 0;
					pParty = m_pMain->m_arParty.GetData( pUser->m_sPartyNumber );
					if( pParty ) {	
						int nTotalMan = 0;
						int nTotalLevel = 0;
						for(int j=0; j<8; j++)	{
							uid = pParty->uid[j];
							pPartyUser = m_pMain->GetUserPtr(uid);
							if(pPartyUser)	{
								nTotalMan++;
								nTotalLevel += pPartyUser->m_sLevel;
							}
						}

						nPartyExp = GetPartyExp( nTotalLevel, nTotalMan, nPartyExp );
						//TRACE("* PartyUser GetPartyExp total_level=%d, total_man = %d, exp=%d *\n", nTotalLevel, nTotalMan, nPartyExp);

						for(int k=0; k<8; k++)	{
							uid = pParty->uid[k];
							pPartyUser = m_pMain->GetUserPtr(uid);
							if(pPartyUser)
							{
								if( IsInExpRange(pPartyUser) == TRUE)
								{
									TempValue = ( nPartyExp * ( 1+0.3*( nTotalMan-1 ) ) ) * (double)pPartyUser->m_sLevel / (double)nTotalLevel;
									//TempValue = ( nPartyExp * ( 1+0.3*( nTotalMan-1 ) ) );
									nExp = (int)TempValue;
									if(TempValue > nExp)	nExp = nExp + 1;
									if(nPartyLoyalty <= 0)
										nLoyalty = 0;
									else	{
										TempValue = ( nPartyLoyalty * ( 1+0.2*( nTotalMan-1 ) ) ) * (double)pPartyUser->m_sLevel / (double)nTotalLevel;
										nLoyalty = (int)TempValue;
										if(TempValue > nLoyalty)	nLoyalty = nLoyalty + 1;
									}
									//TRACE("* PartyUser Exp id=%s, damage=%d, total=%d, exp=%d, loral=%d, level=%d/%d *\n", pPartyUser->m_strUserID, (int)totalDamage, m_TotalDamage, nExp, nLoyalty, pPartyUser->m_sLevel, nTotalLevel);
									//pPartyUser->SetExp(nExp, nLoyalty, m_sLevel);
									pPartyUser->SetPartyExp(nExp, nLoyalty, nTotalLevel, nTotalMan);
								}
							}
						}	
					}
				}
			}
			else if(i==0)
			{
				int uid = 0;
				pParty = m_pMain->m_arParty.GetData( pUser->m_sPartyNumber );
				if( pParty ) {	
					int nTotalMan = 0;
					int nTotalLevel = 0;
					for(int j=0; j<8; j++)	{
						uid = pParty->uid[j];
						pPartyUser = m_pMain->GetUserPtr(uid);
						if(pPartyUser)	{
							nTotalMan++;
							nTotalLevel += pPartyUser->m_sLevel;
						}
					}

					nPartyExp = GetPartyExp( nTotalLevel, nTotalMan, nPartyExp );
					//TRACE("* PartyUser GetPartyExp total_level=%d, total_man = %d, exp=%d *\n", nTotalLevel, nTotalMan, nPartyExp);

					for(int k=0; k<8; k++)	{
						uid = pParty->uid[k];
						pPartyUser = m_pMain->GetUserPtr(uid);
						if(pPartyUser)
						{
							if( IsInExpRange(pPartyUser) == TRUE)
							{
								TempValue = ( nPartyExp * ( 1+0.3*( nTotalMan-1 ) ) ) * (double)pPartyUser->m_sLevel / (double)nTotalLevel;
								//TempValue = ( nPartyExp * ( 1+0.3*( nTotalMan-1 ) ) );
								nExp = (int)TempValue;
								if(TempValue > nExp)	nExp = nExp + 1;
								if(nPartyLoyalty <= 0)
									nLoyalty = 0;
								else	{
									TempValue = ( nPartyLoyalty * ( 1+0.2*( nTotalMan-1 ) ) ) * (double)pPartyUser->m_sLevel / (double)nTotalLevel;
									nLoyalty = (int)TempValue;
									if(TempValue > nLoyalty)	nLoyalty = nLoyalty + 1;
								}
								//TRACE("* PartyUser Exp id=%s, damage=%d, total=%d, exp=%d, loral=%d, level=%d/%d *\n", pPartyUser->m_strUserID, (int)totalDamage, m_TotalDamage, nExp, nLoyalty, pPartyUser->m_sLevel, nTotalLevel);
								//pPartyUser->SetExp(nExp, nLoyalty, m_sLevel);
								pPartyUser->SetPartyExp(nExp, nLoyalty, nTotalLevel, nTotalMan);
							}
						}
					}	
				}
			}
		}	
		else if(pUser->m_byNowParty == 2)
		{	
			
		}	
		else
		{
			totalDamage = m_DamagedUserList[i].nDamage;
			
			if(totalDamage == 0 || m_TotalDamage == 0)	{
				nExp = 0;
				nLoyalty = 0;
			}
			else	{

				if( CompDamage < totalDamage )	{	// 
					CompDamage = totalDamage;
					m_sMaxDamageUserid = m_DamagedUserList[i].iUid;
					pMaxDamageUser = m_pMain->GetUserPtr(m_DamagedUserList[i].iUid);
					if(pMaxDamageUser == NULL)	{
						m_byMaxDamagedNation = pUser->m_bNation;
						strcpy( strMaxDamageUser, pUser->m_strUserID );
					}
					else	{
						m_byMaxDamagedNation = pMaxDamageUser->m_bNation;
						strcpy( strMaxDamageUser, pMaxDamageUser->m_strUserID );
					}
				}

				TempValue = m_iExp * ( totalDamage / m_TotalDamage );
				nExp = (int)TempValue;
				if(TempValue > nExp)	nExp = nExp + 1;

				if(m_iLoyalty == 0) nLoyalty = 0;
				else	{
					TempValue = m_iLoyalty * ( totalDamage / m_TotalDamage );
					nLoyalty = (int)TempValue;
					if(TempValue > nLoyalty)	nLoyalty = nLoyalty + 1;
				}

				//TRACE("* User Exp id=%s, damage=%d, total=%d, exp=%d, loral=%d *\n", pUser->m_strUserID, (int)totalDamage, m_TotalDamage, nExp, nLoyalty);
				pUser->SetExp(nExp, nLoyalty, m_sLevel);
			}
		}
	}

	if( m_pMain->m_byBattleEvent == BATTLEZONE_OPEN )	{	// at war
		if( m_bySpecialType >= 90 && m_bySpecialType <= 100 )	{					// Please record the user who inflicted the most damage upon death.
			if( strlen( strMaxDamageUser) != 0 )	{		// Sends the name of the user who did the most damage to the monster
				char send_buff[100];	memset(send_buff, 0x00, 100 );
				int send_index = 0;
				SetByte( send_buff, AG_BATTLE_EVENT, send_index );
				SetByte( send_buff, BATTLE_EVENT_MAX_USER, send_index );
				if( m_bySpecialType == 100 )		SetByte( send_buff, 1, send_index );
				else if( m_bySpecialType == 90 )	{
					SetByte( send_buff, 3, send_index );
					m_pMain->m_sKillKarusNpc++;
				}
				else if( m_bySpecialType == 91 )	{
					SetByte( send_buff, 4, send_index );
					m_pMain->m_sKillKarusNpc++;
				}
				else if( m_bySpecialType == 92 )	{
					SetByte( send_buff, 5, send_index );
					m_pMain->m_sKillElmoNpc++;
				}
				else if( m_bySpecialType == 93 )	{
					SetByte( send_buff, 6, send_index );
					m_pMain->m_sKillElmoNpc++;
				}
				else if( m_bySpecialType == 98 )	{
					SetByte( send_buff, 7, send_index );
					m_pMain->m_sKillKarusNpc++;
				}
				else if( m_bySpecialType == 99 )	{
					SetByte( send_buff, 8, send_index );
					m_pMain->m_sKillElmoNpc++;
				}

				SetByte( send_buff, strlen(strMaxDamageUser), send_index );
				SetString( send_buff, strMaxDamageUser, strlen(strMaxDamageUser), send_index );
				m_pMain->Send( send_buff, send_index, m_sCurZone );
				TRACE("@@@ MaxDamageUser - %s @@@\n", strMaxDamageUser);

				memset(send_buff, 0x00, 100 );	send_index = 0;
				if( m_pMain->m_sKillKarusNpc == pMap->m_sKarusRoom )	{
					SetByte( send_buff, AG_BATTLE_EVENT, send_index );
					SetByte( send_buff, BATTLE_EVENT_RESULT, send_index );
					SetByte( send_buff, ELMORAD_ZONE, send_index );
					SetByte( send_buff, strlen(strMaxDamageUser), send_index );
					SetString( send_buff, strMaxDamageUser, strlen(strMaxDamageUser), send_index );
					m_pMain->Send( send_buff, send_index, m_sCurZone );
					TRACE("@@@ Karus Victory - %d, %d @@@\n", m_pMain->m_sKillKarusNpc, pMap->m_sKarusRoom);
				}
				else if( m_pMain->m_sKillElmoNpc == pMap->m_sElmoradRoom )	{
					SetByte( send_buff, AG_BATTLE_EVENT, send_index );
					SetByte( send_buff, BATTLE_EVENT_RESULT, send_index );
					SetByte( send_buff, KARUS_ZONE, send_index );
					SetByte( send_buff, strlen(strMaxDamageUser), send_index );
					SetString( send_buff, strMaxDamageUser, strlen(strMaxDamageUser), send_index );
					m_pMain->Send( send_buff, send_index, m_sCurZone );
					TRACE("@@@ Elmorad Victory - %d, %d @@@\n", m_pMain->m_sKillElmoNpc, pMap->m_sElmoradRoom);
				}
			}
		}
	}
}

int CNpc::SendDead(CIOCPort* pIOCP, int type)
{
	if(!pIOCP) return 0;
	if(m_NpcState != NPC_DEAD || m_iHP > 0) return 0;

	if(type) GiveNpcHaveItem(pIOCP);	// Drop an item (do not drop if it is guarded)

	return m_sRegenTime;
}

//	Determine if the distance between the NPC and the target is smaller than the specified range
BOOL CNpc::IsCloseTarget(CUser *pUser, int nRange)
{
	if(pUser == NULL)
	{
		return FALSE;
	}
	if(pUser->m_sHP <= 0 ||/* pUser->m_state != STATE_GAMESTARTED ||*/ pUser->m_bLive == FALSE)
	{
		return FALSE;
	}

	__Vector3 vUser;
	__Vector3 vNpc;
	float fDis = 0.0f;
	vNpc.Set(m_fCurX, m_fCurY, m_fCurZ);
	vUser.Set(pUser->m_curx, pUser->m_cury, pUser->m_curz); 
	fDis = GetDistance(vNpc, vUser);
	
	// Since it is under attack, the distance detection area is doubled,
	if((int)fDis > nRange * 2) return FALSE; 

	//InitTarget();

	m_Target.id = pUser->m_iUserId + USER_BAND;
	m_Target.x = pUser->m_curx;
	m_Target.y = pUser->m_cury;
	m_Target.z = pUser->m_curz;

	return TRUE;
}

/////////////////////////////////////////////// //////////////////////////
// Find allies within the field of view.
// type = 0: Helps only the same family type in the same group, 1: Helps regardless of group or family,
// 2: For the purpose of the priest NPC checking the status of the same ally and treating them,, returns the NID of the ally to be cured.
int CNpc::FindFriend(int type)
{
	CNpc* pNpc = NULL;
	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size()) {
		TRACE("#### Npc-FindFriend ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return 0;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(pMap == NULL) return 0;

	if(m_bySearchRange == 0) return 0;
	if(type != 2)	{
		if(m_Target.id == -1) return 0;
	}

	int max_xx = pMap->m_sizeRegion.cx;
	int max_zz = pMap->m_sizeRegion.cy;

	int min_x = (int)(m_fCurX - m_bySearchRange)/VIEW_DIST;	if(min_x < 0) min_x = 0;
	int min_z = (int)(m_fCurZ - m_bySearchRange)/VIEW_DIST;	if(min_z < 0) min_z = 0;
	int max_x = (int)(m_fCurX + m_bySearchRange)/VIEW_DIST;	if(max_x >= max_xx) max_x = max_xx - 1;
	int max_z = (int)(m_fCurZ + m_bySearchRange)/VIEW_DIST;	if(min_z >= max_zz) min_z = max_zz - 1;

	int search_x = max_x - min_x + 1;		
	int search_z = max_z - min_z + 1;	
	
	int i, j, count = 0;
	_TargetHealer arHealer[9];
	for(i=0; i<9; i++)	{
		arHealer[i].sNID = -1;
		arHealer[i].sValue = 0;
	}

	for(i = 0; i < search_x; i++)	{
		for(j = 0; j < search_z; j++)	{
			FindFriendRegion(min_x+i, min_z+j, pMap, &arHealer[count], type);
			//FindFriendRegion(min_x+i, min_z+j, pMap, type);
		}
	}

	int iValue = 0, iMonsterNid = 0;
	for(i=0; i<9; i++)	{
		if(iValue < arHealer[i].sValue)	{
			iValue = arHealer[i].sValue;
			iMonsterNid = arHealer[i].sNID;
		}
	}

	if(iMonsterNid != 0)	{
		m_Target.id = iMonsterNid;

		return iMonsterNid;
	}

	return 0;
}

void CNpc::FindFriendRegion(int x, int z, MAP* pMap, _TargetHealer* pHealer, int type)
//void CNpc::FindFriendRegion(int x, int z, MAP* pMap, int type)
{
	// Search the UserArray in your own region first, and determine if there is a user nearby.
	if(x < 0 || z < 0 || x > pMap->GetXRegionMax() || z > pMap->GetZRegionMax())	{
		TRACE("#### Npc-FindFriendRegion() Fail : [nid=%d, sid=%d], nRX=%d, nRZ=%d #####\n", m_sNid+NPC_BAND, m_sSid, x, z);
		return;
	}

	int* pNpcIDList = NULL;
	int total_mon = 0, count = 0, nid = 0;

	EnterCriticalSection( &g_region_critical );
	map < int, int* >::iterator		Iter1;
	map < int, int* >::iterator		Iter2;

	Iter1 = pMap->m_ppRegion[x][z].m_RegionNpcArray.m_UserTypeMap.begin();
	Iter2 = pMap->m_ppRegion[x][z].m_RegionNpcArray.m_UserTypeMap.end();

	total_mon = pMap->m_ppRegion[x][z].m_RegionNpcArray.GetSize();
	pNpcIDList = new int[total_mon];
	for( ; Iter1 != Iter2; Iter1++ ) {
		nid = *( (*Iter1).second );
		pNpcIDList[count] = nid;
		count++;
	}
	LeaveCriticalSection( &g_region_critical );

	CNpc* pNpc = NULL;
	__Vector3 vStart, vEnd;
	float fDis = 0.0f;
	// Because it is under attack.. Double the search range..
	float fSearchRange = 0.0f;
	if( type == 2)	fSearchRange = (float)m_byAttackRange;
	else fSearchRange = (float)m_byTracingRange;
	vStart.Set(m_fCurX, m_fCurY, m_fCurZ);
	int iValue = 0, iCompValue = 0, iHP = 0;

	for(int i=0 ; i<total_mon; i++ ) {
		nid = pNpcIDList[i];
		if( nid < NPC_BAND )	continue;
		pNpc = (CNpc*)m_pMain->m_arNpc.GetData(nid - NPC_BAND);

		if( pNpc != NULL && pNpc->m_NpcState != NPC_DEAD && pNpc->m_sNid != m_sNid)	{
			vEnd.Set(pNpc->m_fCurX, pNpc->m_fCurY, pNpc->m_fCurZ); 
			fDis = GetDistance(vStart, vEnd);

			// Determine whether the user is in my attack range from here
			if(fDis <= fSearchRange)	{
				if(type == 1)	{
					if(m_sNid != pNpc->m_sNid)	{
						if(pNpc->m_Target.id > -1 && pNpc->m_NpcState == NPC_FIGHTING) continue;
						pNpc->m_Target.id = m_Target.id;		// Ask all colleagues for help.
						pNpc->m_Target.x = m_Target.x;			// Let's attack the same target...
						pNpc->m_Target.y = m_Target.y;
						pNpc->m_Target.z = m_Target.z;
						pNpc->m_Target.failCount = 0;
						pNpc->NpcStrategy(NPC_ATTACK_SHOUT);
					}
				}
				else if(type == 0)	{
					if(pNpc->m_tNpcGroupType && m_sNid != pNpc->m_sNid && pNpc->m_byFamilyType == m_byFamilyType)	{
						if(pNpc->m_Target.id > -1 && pNpc->m_NpcState == NPC_FIGHTING) continue;
						pNpc->m_Target.id = m_Target.id;		// Ask for help from colleagues of the same type.
						pNpc->m_Target.x = m_Target.x;			// Let's attack the same target...
						pNpc->m_Target.y = m_Target.y;
						pNpc->m_Target.z = m_Target.z;
						pNpc->m_Target.failCount = 0;
						pNpc->NpcStrategy(NPC_ATTACK_SHOUT);
					}
				}
				else if(type == 2)	{
					if(pHealer == NULL) continue;
					// Check HP status
					iHP = pNpc->m_iMaxHP * 0.9;
					if(pNpc->m_iHP <= iHP)	{		// HP check
						iCompValue = (pNpc->m_iMaxHP - pNpc->m_iHP) / (pNpc->m_iMaxHP * 0.01);
						if(iValue < iCompValue)		{
							iValue = iCompValue;
							pHealer->sNID = pNpc->m_sNid+NPC_BAND;
							pHealer->sValue = iValue;
						}
					}
				}
			}	
			else continue;
		}
	}

	if(pNpcIDList)	{
		delete [] pNpcIDList;
		pNpcIDList = NULL;
	}
}

void CNpc::NpcStrategy(BYTE type)
{
	switch(type)
	{
	case NPC_ATTACK_SHOUT:
		m_NpcState = NPC_TRACING;
		m_Delay = m_sSpeed;//STEP_DELAY;
		m_fDelayTime = TimeGet();
		break;
	}
}

//	Store NPC information in a buffer.
void CNpc::FillNpcInfo(char *temp_send, int &index, BYTE flag)
{
	SetByte(temp_send, AG_NPC_INFO, index );
	if( m_bySpecialType == 5 && m_byChangeType == 0)	
		SetByte(temp_send, 0, index );					// do not register in the region
	else
		SetByte(temp_send, 1, index );					// Register in region
	SetShort(temp_send, m_sNid+NPC_BAND, index );
	SetShort(temp_send, m_sPid, index );
	SetShort(temp_send, m_sSize, index );
	SetInt(temp_send, m_iWeapon_1, index );
	SetInt(temp_send, m_iWeapon_2, index );
	SetShort(temp_send, m_sCurZone, index);
	SetShort(temp_send, m_ZoneIndex, index);
	SetVarString(temp_send, m_strName, _tcslen(m_strName), index);
	SetByte(temp_send, m_byGroup, index);
	SetByte(temp_send, (BYTE)m_sLevel, index);
	Setfloat(temp_send, m_fCurX, index);
	Setfloat(temp_send, m_fCurZ, index);
	Setfloat(temp_send, m_fCurY, index);
	Setfloat(temp_send, m_fDir, index);

	if(m_iHP <= 0) SetByte(temp_send, 0x00, index);
	else SetByte(temp_send, 0x01, index);

	SetByte(temp_send, m_tNpcType, index);
	SetInt(temp_send, m_iSellingGroup, index);
	SetDWORD(temp_send, m_iMaxHP, index);
	SetDWORD(temp_send, m_iHP, index);
	SetByte(temp_send, m_byGateOpen, index);
	SetShort(temp_send, m_sHitRate, index);
	SetByte(temp_send, m_byObjectType, index);
}

// Send all npc information to the game server...
void CNpc::SendNpcInfoAll(char *temp_send, int &index, int count)
{
	if( m_bySpecialType == 5 && m_byChangeType == 0)	
		SetByte(temp_send, 0, index );					// do not register in the region
	else
		SetByte(temp_send, 1, index );					// Register in region		
	SetShort(temp_send, m_sNid+NPC_BAND, index );
	SetShort(temp_send, m_sSid, index);
	SetShort(temp_send, m_sPid, index );
	SetShort(temp_send, m_sSize, index );
	SetInt(temp_send, m_iWeapon_1, index );
	SetInt(temp_send, m_iWeapon_2, index );
	SetShort(temp_send, m_sCurZone, index);
	SetShort(temp_send, m_ZoneIndex, index);
	SetVarString(temp_send, m_strName, _tcslen(m_strName), index);
	SetByte(temp_send, m_byGroup, index);
	SetByte(temp_send, (BYTE)m_sLevel, index);
	Setfloat(temp_send, m_fCurX, index);
	Setfloat(temp_send, m_fCurZ, index);
	Setfloat(temp_send, m_fCurY, index);
	Setfloat(temp_send, m_fDir, index);
	SetByte(temp_send, m_tNpcType, index);
	SetInt(temp_send, m_iSellingGroup, index);
	SetDWORD(temp_send, m_iMaxHP, index);
	SetDWORD(temp_send, m_iHP, index);
	SetByte(temp_send, m_byGateOpen, index);
	SetShort(temp_send, m_sHitRate, index);
	SetByte(temp_send, m_byObjectType, index);

	//TRACE("monster info all = %d, name=%s, count=%d \n", m_sNid+NPC_BAND, m_strName, count);
}

int CNpc::GetDir(float x1, float z1, float x2, float z2)
{
	int nDir;					//	3 4 5
								//	2 8 6
								//	1 0 7

	int nDirCount = 0;

	int x11 = (int)x1 / TILE_SIZE;
	int y11 = (int)z1 / TILE_SIZE; 
	int x22 = (int)x2 / TILE_SIZE;
	int y22 = (int)z2 / TILE_SIZE; 

	int deltax = x22 - x11;
	int deltay = y22 - y11;

	int fx = ((int)x1/TILE_SIZE) * TILE_SIZE;
	int fy = ((int)z1/TILE_SIZE) * TILE_SIZE;

	float add_x = x1 - fx;
	float add_y = z1 - fy;

	if (deltay==0) {
		if (x22>x11) nDir = DIR_RIGHT;		
		else nDir = DIR_LEFT;	
		goto result_value;
	}
	if (deltax==0) 
	{
		if (y22>y11) nDir = DIR_DOWN;	
		else nDir = DIR_UP;		
		goto result_value;
	}
	else 
	{
		if (y22>y11) 
		{
			if (x22>x11) 
			{
				nDir = DIR_DOWNRIGHT;
			}
			else 
			{
				nDir = DIR_DOWNLEFT;
			}
		}
		else
		{
			if (x22 > x11) 
			{
				nDir = DIR_UPRIGHT;		
			}
			else 
			{
				nDir = DIR_UPLEFT;
			}
		}
	}

result_value:

	switch(nDir)
	{
	case DIR_DOWN:
		m_fAdd_x = add_x;
		m_fAdd_z = 3;
		break;
	case DIR_DOWNLEFT:
		m_fAdd_x = 1;
		m_fAdd_z = 3;
		break;
	case DIR_LEFT:
		m_fAdd_x = 1;
		m_fAdd_z = add_y;
		break;
	case DIR_UPLEFT:
		m_fAdd_x = 1;
		m_fAdd_z = 1;
		break;
	case DIR_UP:
		m_fAdd_x = add_x;
		m_fAdd_z = 1;
		break;
	case DIR_UPRIGHT:
		m_fAdd_x = 3;
		m_fAdd_z = 1;
		break;
	case DIR_RIGHT:
		m_fAdd_x = 3;
		m_fAdd_z = add_y;
		break;
	case DIR_DOWNRIGHT:
		m_fAdd_x = 3;
		m_fAdd_z = 3;
		break;
	}

	return nDir;
}

inline float CNpc::RandomGenf(float max, float min)
{
	if ( max == min ) return max;
	if ( min > max ) { float b = min; min = max; max = b; }
	int k = rand()%(int)((max*100-min*100));	

	return (float)((float)(k*0.01f)+min);
}


//////////////////////////////////////////////////////////////////////
// Factors: current position, direction, random angle, movement distance
// return value: final position
//////////////////////////////////////////////////////////////////////
__Vector3 CNpc::MyFunc(__Vector3 vCur, __Vector3 vDir, float fYDegree, float fDistance)
{
	float fYRandom = RandomGenf( fYDegree/2.0f, -fYDegree/2.0f);

	vDir.Normalize();
	__Matrix44 mtx; mtx.Identity();
	__Quaternion qt;

	qt.RotationAxis( 0.0f, 1.0f, 0.0f, D3DXToRadian(fYRandom) );
	D3DXMatrixRotationQuaternion(&mtx, &qt);

	vDir *= mtx;
	vDir *= fDistance;

	__Vector3 vResult;
	vResult = vCur + vDir;
	return vResult;
}

__Vector3 CNpc::GetDirection(__Vector3 vStart, __Vector3 vEnd)
{
	__Vector3 vDir = vEnd - vStart;
	vDir.Normalize();
	return vDir;
}

void CNpc::SendAll(CIOCPort* pIOCP, TCHAR *pBuf, int nLength)
{
	if(nLength <= 0 || nLength >= SOCKET_BUFF_SIZE) return;

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if(pNewData == NULL) return;

	pNewData->sCurZone = m_sCurZone;
	pNewData->sLength = nLength;
	::CopyMemory(pNewData->pBuf, pBuf, nLength);

	EnterCriticalSection( &(pIOCP->m_critSendData) );
	pIOCP->m_SendDataList.push_back( pNewData );
	LeaveCriticalSection( &(pIOCP->m_critSendData) );

	PostQueuedCompletionStatus( pIOCP->m_hSendIOCP, 0, 0, NULL );
}

void CNpc::NpcTrace(TCHAR *pMsg)
{
	//if(g_bDebug == FALSE) return;

	CString szMsg = _T("");
	szMsg.Format(_T("%s : uid = %d, name = %s, xpos = %f, zpos = %f\n"), pMsg, m_sNid+NPC_BAND, m_strName, m_fCurX, m_fCurZ);
	TRACE(szMsg);
}

void CNpc::NpcMoveEnd(CIOCPort* pIOCP)
{
	SetUid(m_fCurX, m_fCurZ, m_sNid + NPC_BAND);

	char pBuf[1024];
	::ZeroMemory(pBuf, 1024);	
	int index = 0;
	SetByte(pBuf, MOVE_RESULT, index);
	SetByte(pBuf, SUCCESS, index);
	SetShort(pBuf, m_sNid+NPC_BAND, index);
	Setfloat(pBuf, m_fCurX, index);
	Setfloat(pBuf, m_fCurZ, index);
	Setfloat(pBuf, m_fCurY, index);
	Setfloat(pBuf, 0, index);

	int rx=m_fCurX / VIEW_DIST;
	int rz=m_fCurZ / VIEW_DIST;
	//TRACE("NpcMoveEnd() --> nid = %d, x=%f, y=%f, rx=%d,rz=%d, frame=%d, speed = %d \n", m_sNid, m_fCurX, m_fCurZ, rx,rz, m_iAniFrameCount, m_sSpeed);
	SendAll(pIOCP, pBuf, index);
}

__Vector3 CNpc::GetVectorPosition(__Vector3 vOrig, __Vector3 vDest, float fDis)
{
	__Vector3 vOff;
	vOff = vDest - vOrig;
	float ttt = vOff.Magnitude();
	vOff.Normalize();
	vOff *= fDis;
	return vOrig + vOff;
}

float CNpc::GetDistance(__Vector3 vOrig, __Vector3 vDest)
{
	__Vector3 vDis = vOrig - vDest;
	return vDis.Magnitude();
}

BOOL CNpc::GetUserInView()
{
	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size())	{
		TRACE("#### Npc-GetUserInView ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return FALSE;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(pMap == NULL)	return FALSE;
	//if( m_ZoneIndex > 5 || m_ZoneIndex < 0) return FALSE;		// Temporary code ( 2002.03.24 )
	int max_xx = pMap->m_sizeRegion.cx;
	int max_zz = pMap->m_sizeRegion.cy;
	int min_x = (int)(m_fCurX - NPC_VIEW_RANGE)/VIEW_DIST;	if(min_x < 0) min_x = 0;
	int min_z = (int)(m_fCurZ - NPC_VIEW_RANGE)/VIEW_DIST;	if(min_z < 0) min_z = 0;
	int max_x = (int)(m_fCurX + NPC_VIEW_RANGE)/VIEW_DIST;	if(max_x >= max_xx) max_x = max_xx - 1;
	int max_z = (int)(m_fCurZ + NPC_VIEW_RANGE)/VIEW_DIST;	if(max_z >= max_zz) max_z = max_zz - 1;

	int search_x = max_x - min_x + 1;		
	int search_z = max_z - min_z + 1;	
	
	BOOL bFlag = FALSE;
	int i, j;

	for(i = 0; i < search_x; i++)	{
		for(j = 0; j < search_z; j++)	{
			bFlag = GetUserInViewRange(min_x+i, min_z+j);
			if(bFlag == TRUE)	return TRUE;
		}
	}

	return FALSE;
}

BOOL CNpc::GetUserInViewRange(int x, int z)
{
	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size())	{
		TRACE("#### Npc-GetUserInViewRange ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return FALSE;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];

	if(x < 0 || z < 0 || x > pMap->GetXRegionMax() || z > pMap->GetZRegionMax())	{
		TRACE("#### Npc-GetUserInViewRange() Fail : [nid=%d, sid=%d], x1=%d, z1=%d #####\n", m_sNid+NPC_BAND, m_sSid, x, z);
		return FALSE;
	}

	EnterCriticalSection( &g_region_critical );
	CUser* pUser = NULL;
	__Vector3 vStart, vEnd;
	vStart.Set(m_fCurX, 0, m_fCurZ);
	float fDis = 0.0f; 
	int nUserid = 0;

	map < int, int* >::iterator		Iter1;
	map < int, int* >::iterator		Iter2;
	Iter1 = pMap->m_ppRegion[x][z].m_RegionUserArray.m_UserTypeMap.begin();
	Iter2 = pMap->m_ppRegion[x][z].m_RegionUserArray.m_UserTypeMap.end();

	for( ; Iter1 != Iter2; Iter1++ )	{
		nUserid = *( (*Iter1).second );
		if( nUserid < 0 )			continue;
		pUser = (CUser*)m_pMain->GetUserPtr(nUserid);

		if( !pUser ) continue;
		// View distance calculation
		vEnd.Set(pUser->m_curx, 0, pUser->m_curz);
		fDis = GetDistance(vStart, vEnd);
		if(fDis <= NPC_VIEW_RANGE)	{
			LeaveCriticalSection( &g_region_critical );
			return TRUE;		
		}
	}
	
	LeaveCriticalSection( &g_region_critical );
	return FALSE;
}

void CNpc::SendAttackSuccess(CIOCPort* pIOCP, BYTE byResult, int tuid, short sDamage, int nHP, BYTE byFlag, short sAttack_type)
{
	int send_index = 0;
	int sid = -1, tid = -1;
	BYTE type, result=0;
	char buff[256];
	memset( buff, 0x00, 256 );
	float rx=0.0f, ry=0.0f, rz=0.0f;

	if( byFlag == 0 )	{
		type = 0x02;
		sid = m_sNid+NPC_BAND;
		tid = tuid;

		SetByte( buff, AG_ATTACK_RESULT, send_index );
		SetByte( buff, type, send_index );
		SetByte( buff, byResult, send_index );
		SetShort( buff, sid, send_index );
		SetShort( buff, tid, send_index );
		SetShort( buff, sDamage, send_index );
		SetDWORD( buff, nHP, send_index );	
		SetByte( buff, sAttack_type, send_index );
	}
	else	{
		type = 0x01;
		sid = tuid;
		tid = m_sNid+NPC_BAND;

		SetByte( buff, AG_ATTACK_RESULT, send_index );
		SetByte( buff, type, send_index );
		SetByte( buff, byResult, send_index );
		SetShort( buff, sid, send_index );
		SetShort( buff, tid, send_index );
		SetShort( buff, sDamage, send_index );
		SetDWORD( buff, nHP, send_index );	
		SetByte( buff, sAttack_type, send_index );
	}

	//TRACE("Npc - SendAttackSuccess() : [sid=%d, tid=%d, result=%d], damage=%d, hp = %d\n", sid, tid, byResult, sDamage, sHP);
	//SetShort( buff, sMaxHP, send_index );

	SendAll(pIOCP, buff, send_index);
}

__Vector3 CNpc::CalcAdaptivePosition(__Vector3 vPosOrig, __Vector3 vPosDest, float fAttackDistance)
{
	__Vector3 vTemp, vReturn;
	vTemp = vPosOrig - vPosDest;	vTemp.Normalize();	vTemp *= fAttackDistance;
	vReturn = vPosDest + vTemp;
	return vReturn;
}

//	Determines whether the current mob is within the range of the screen
void CNpc::IsUserInSight()
{
	CUser* pUser = NULL;
	// Experience is given only to those who are within 50 meters of the NPC and the user.
	int iSearchRange = NPC_EXP_RANGE;		

	int i,j;
	__Vector3 vStart, vEnd;
	float fDis = 0.0f;

	vStart.Set(m_fCurX, m_fCurY, m_fCurZ);

	for(j = 0; j < NPC_HAVE_USER_LIST; j++)
	{
		m_DamagedUserList[j].bIs = FALSE;
	}

	for(i = 0; i < NPC_HAVE_USER_LIST; i++)
	{
		pUser = m_pMain->GetUserPtr(m_DamagedUserList[i].iUid);
		if(pUser == NULL)	continue;

		vEnd.Set(pUser->m_curx, pUser->m_cury, pUser->m_curz);
		fDis = GetDistance(vStart, vEnd);

		if((int)fDis <= iSearchRange)
		{
			// If it is the same as the user on the list you have
			if(m_DamagedUserList[i].iUid == pUser->m_iUserId)
			{
				// Compare the final IDs if they are the same
				if(_stricmp(m_DamagedUserList[i].strUserID, pUser->m_strUserID) == 0) 
				{ 
					// Indicate that it exists only at this time
					m_DamagedUserList[i].bIs = TRUE;
				}
			}
		}
	}
}

BYTE CNpc::GetHitRate(float rate)
{
/*
	BYTE result;
	int random = 0;
	random = myrand(1, 10000);

	if( rate >= 5.0 )
	{
		if( random >= 1 && random <= 3500)
			result = GREAT_SUCCESS;
		else if( random >= 3501 && random <= 7500)
			result = SUCCESS;
		else if( random >= 7501 && random <= 9500)
			result = NORMAL;
		else if( random >= 9501 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 5.0 && rate >= 3.0)
	{
		if( random >= 1 && random <= 2500)
			result = GREAT_SUCCESS;
		else if( random >= 2501 && random <= 6000)
			result = SUCCESS;
		else if( random >= 6001 && random <= 9000)
			result = NORMAL;
		else if( random >= 9001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 3.0 && rate >= 2.0)
	{
		if( random >= 1 && random <= 2000)
			result = GREAT_SUCCESS;
		else if( random >= 2001 && random <= 5000)
			result = SUCCESS;
		else if( random >= 5001 && random <= 9000)
			result = NORMAL;
		else if( random >= 9001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 2.0 && rate >= 1.25)
	{
		if( random >= 1 && random <= 1500)
			result = GREAT_SUCCESS;
		else if( random >= 1501 && random <= 4000)
			result = SUCCESS;
		else if( random >= 4001 && random <= 8500)
			result = NORMAL;
		else if( random >= 8501 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 1.25 && rate >= 0.8)
	{
		if( random >= 1 && random <= 1000)
			result = GREAT_SUCCESS;
		else if( random >= 1001 && random <= 3000)
			result = SUCCESS;
		else if( random >= 3001 && random <= 8000)
			result = NORMAL;
		else if( random >= 8001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}	
	else if ( rate < 0.8 && rate >= 0.5)
	{
		if( random >= 1 && random <= 800)
			result = GREAT_SUCCESS;
		else if( random >= 801 && random <= 2400)
			result = SUCCESS;
		else if( random >= 2401 && random <= 7000)
			result = NORMAL;
		else if( random >= 7001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 0.5 && rate >= 0.33)
	{
		if( random >= 1 && random <= 600)
			result = GREAT_SUCCESS;
		else if( random >= 601 && random <= 1800)
			result = SUCCESS;
		else if( random >= 1801 && random <= 6000)
			result = NORMAL;
		else if( random >= 6001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 0.33 && rate >= 0.2)
	{
		if( random >= 1 && random <= 400)
			result = GREAT_SUCCESS;
		else if( random >= 401 && random <= 1300)
			result = SUCCESS;
		else if( random >= 1301 && random <= 5000)
			result = NORMAL;
		else if( random >= 5001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else
	{
		if( random >= 1 && random <= 200)
			result = GREAT_SUCCESS;
		else if( random >= 201 && random <= 800)
			result = SUCCESS;
		else if( random >= 801 && random <= 4000)
			result = NORMAL;
		else if( random >= 4001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}

	//TRACE("Rate : %f, random : %d, result : %d\n", rate, random, result);

	return result; 
*/

	BYTE result;
	int random = 0;
	random = myrand(1, 10000);

	if( rate >= 5.0 )
	{
		if( random >= 1 && random <= 3500)
			result = GREAT_SUCCESS;
		else if( random >= 3501 && random <= 7500)
			result = SUCCESS;
		else if( random >= 7501 && random <= 9800)
			result = NORMAL;
		else if( random >= 9801 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 5.0 && rate >= 3.0)
	{
		if( random >= 1 && random <= 2500)
			result = GREAT_SUCCESS;
		else if( random >= 2501 && random <= 6000)
			result = SUCCESS;
		else if( random >= 6001 && random <= 9600)
			result = NORMAL;
		else if( random >= 9601 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 3.0 && rate >= 2.0)
	{
		if( random >= 1 && random <= 2000)
			result = GREAT_SUCCESS;
		else if( random >= 2001 && random <= 5000)
			result = SUCCESS;
		else if( random >= 5001 && random <= 9400)
			result = NORMAL;
		else if( random >= 9401 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 2.0 && rate >= 1.25)
	{
		if( random >= 1 && random <= 1500)
			result = GREAT_SUCCESS;
		else if( random >= 1501 && random <= 4000)
			result = SUCCESS;
		else if( random >= 4001 && random <= 9200)
			result = NORMAL;
		else if( random >= 9201 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 1.25 && rate >= 0.8)
	{
		if( random >= 1 && random <= 1000)
			result = GREAT_SUCCESS;
		else if( random >= 1001 && random <= 3000)
			result = SUCCESS;
		else if( random >= 3001 && random <= 9000)
			result = NORMAL;
		else if( random >= 9001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}	
	else if ( rate < 0.8 && rate >= 0.5)
	{
		if( random >= 1 && random <= 800)
			result = GREAT_SUCCESS;
		else if( random >= 801 && random <= 2500)
			result = SUCCESS;
		else if( random >= 2501 && random <= 8000)
			result = NORMAL;
		else if( random >= 8001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 0.5 && rate >= 0.33)
	{
		if( random >= 1 && random <= 600)
			result = GREAT_SUCCESS;
		else if( random >= 601 && random <= 2000)
			result = SUCCESS;
		else if( random >= 2001 && random <= 7000)
			result = NORMAL;
		else if( random >= 7001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else if ( rate < 0.33 && rate >= 0.2)
	{
		if( random >= 1 && random <= 400)
			result = GREAT_SUCCESS;
		else if( random >= 401 && random <= 1500)
			result = SUCCESS;
		else if( random >= 1501 && random <= 6000)
			result = NORMAL;
		else if( random >= 6001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	else
	{
		if( random >= 1 && random <= 200)
			result = GREAT_SUCCESS;
		else if( random >= 201 && random <= 1000)
			result = SUCCESS;
		else if( random >= 1001 && random <= 5000)
			result = NORMAL;
		else if( random >= 5001 && random <= 10000)
			result = FAIL;
		else
			result = FAIL;
	}
	
	return result;	
}

BOOL CNpc::IsLevelCheck(int iLevel)
{
	// If it is lower than the level of the monster,,, immediately attack
	if(iLevel <= m_sLevel)
		return FALSE;

	int compLevel = 0;

	compLevel = iLevel - m_sLevel;

	// Compare levels and attack immediately if lower than 8
	if(compLevel < 8)	
		return FALSE;

	return TRUE;
}

BOOL CNpc::IsHPCheck(int iHP)
{
	if(m_iHP < (m_iMaxHP*0.2))
	{
//		if(iHP > 1.5*m_iHP)
		return TRUE;
	}

	return FALSE;
}

// A routine that checks whether path find is to be performed.
BOOL CNpc::IsPathFindCheck(float fDistance)
{
	int nX = 0, nZ = 0;
	__Vector3 vStart, vEnd, vDis, vOldDis;
	float fDis = 0.0f;
	vStart.Set(m_fStartPoint_X, 0, m_fStartPoint_Y);
	vEnd.Set(m_fEndPoint_X, 0, m_fEndPoint_Y);
	vDis.Set(m_fStartPoint_X, 0, m_fStartPoint_Y);
	int count = 0;
	int nError = 0;

	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size()) 
	{
		TRACE("#### Npc-IsPathFindCheck ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return FALSE;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];

	nX = (int)(vStart.x / TILE_SIZE);
	nZ = (int)(vStart.z / TILE_SIZE);
	if(pMap->IsMovable(nX, nZ) == TRUE)
	{
		nError = -1;
		return FALSE;
	}
	nX = (int)(vEnd.x / TILE_SIZE);
	nZ = (int)(vEnd.z / TILE_SIZE);
	if(pMap->IsMovable(nX, nZ) == TRUE)
	{
		nError = -1;
		return FALSE;
	}

	while(1)
	{
		vOldDis.Set(vDis.x, 0, vDis.z);
		vDis = GetVectorPosition(vDis, vEnd, fDistance);
		fDis = GetDistance(vOldDis, vEnd);

		if(fDis > NPC_MAX_MOVE_RANGE)
		{
			nError = -1;
			break;
		}
		
		if(fDis <= fDistance)
		{
			nX = (int)(vDis.x / TILE_SIZE);
			nZ = (int)(vDis.z / TILE_SIZE);
			if(pMap->IsMovable(nX, nZ) == TRUE)
			{
				nError = -1;
				break;
			}

			if(count >= MAX_PATH_LINE)
			{
				nError = -1;
				break;
			}
			m_pPoint[count].fXPos = vEnd.x;
			m_pPoint[count].fZPos = vEnd.z;
			count++;
			break;
		}
		else
		{
			nX = (int)(vDis.x / TILE_SIZE);
			nZ = (int)(vDis.z / TILE_SIZE);
			if(pMap->IsMovable(nX, nZ) == TRUE)
			{
				nError = -1;
				break;
			}

			if(count >= MAX_PATH_LINE)
			{
				nError = -1;
				break;
			}
			m_pPoint[count].fXPos = vDis.x;
			m_pPoint[count].fZPos = vDis.z;
		}

		count++;
	}

	m_iAniFrameIndex = count;

	if(nError == -1)
		return FALSE;

	return TRUE;
}

// A routine to go to the target without pass find..
void CNpc::IsNoPathFind(float fDistance)
{
	ClearPathFindData();
	m_bPathFlag = TRUE;

	int nX = 0, nZ = 0;
	__Vector3 vStart, vEnd, vDis, vOldDis;
	float fDis = 0.0f;
	vStart.Set(m_fStartPoint_X, 0, m_fStartPoint_Y);
	vEnd.Set(m_fEndPoint_X, 0, m_fEndPoint_Y);
	vDis.Set(m_fStartPoint_X, 0, m_fStartPoint_Y);
	int count = 0;
	int nError = 0;

	fDis = GetDistance(vStart, vEnd);	
	if(fDis > NPC_MAX_MOVE_RANGE)	{						// If it is wider than 100 meters, it is in a standing state.
		ClearPathFindData();
		TRACE("#### Npc-IsNoPathFind Fail : NPC_MAX_MOVE_RANGE overflow  .. [nid = %d, name=%s], cur_x=%.2f, z=%.2f, dest_x=%.2f, dest_z=%.2f, fDis=%.2f#####\n", m_sNid+NPC_BAND, m_strName, m_fStartPoint_X, m_fStartPoint_Y, m_fEndPoint_X, m_fEndPoint_Y, fDis);
		return;
	}

	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size())	{
		ClearPathFindData();
		TRACE("#### Npc-IsNoPathFind ZoneIndex Fail : [nid=%d, name=%s], zoneindex=%d #####\n", m_sNid+NPC_BAND, m_strName, m_ZoneIndex);
		return;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];

	while(1)
	{
		vOldDis.Set(vDis.x, 0, vDis.z);
		vDis = GetVectorPosition(vDis, vEnd, fDistance);
		fDis = GetDistance(vOldDis, vEnd);
		
		if(fDis <= fDistance)
		{
			nX = (int)(vDis.x / TILE_SIZE);
			nZ = (int)(vDis.z / TILE_SIZE);
			if(count < 0 || count >= MAX_PATH_LINE)	{	
				ClearPathFindData();
				TRACE("#### Npc-IsNoPathFind index overflow Fail 1 :  count=%d ####\n", count);
				return;
			}	
			m_pPoint[count].fXPos = vEnd.x;
			m_pPoint[count].fZPos = vEnd.z;
			count++;
			break;
		}
		else
		{
			if(count < 0 || count >= MAX_PATH_LINE)	{	
				ClearPathFindData();
				TRACE("#### Npc-IsNoPathFind index overflow Fail 2 :  count=%d ####\n", count);
				return;
			}	
			nX = (int)(vDis.x / TILE_SIZE);
			nZ = (int)(vDis.z / TILE_SIZE);
			m_pPoint[count].fXPos = vDis.x;
			m_pPoint[count].fZPos = vDis.z;
		}
		count++;
	}

	if(count <= 0 || count >= MAX_PATH_LINE)	{	
		ClearPathFindData();
		TRACE("#### IsNoPtahfind Fail : nid=%d,%s, count=%d ####\n", m_sNid+NPC_BAND, m_strName, count);
		return;
	}
	m_iAniFrameIndex = count;

}

//	Drops items from NPCs.
void CNpc::GiveNpcHaveItem(CIOCPort* pIOCP)
{
	char pBuf[1024];
	::ZeroMemory(pBuf, 1024);	
	char logfile[256];	memset(logfile, 0x00, 256);
	int index = 0;
	int temp = 0;
	int iPer = 0, iMakeItemCode = 0, iMoney = 0;
	int iRandom;
	int nCount = 1, i =0;
	CString string;

/*	if( m_byMoneyType == 1 )	{
		SetByte(pBuf, AG_NPC_EVENT_ITEM, index);
		SetShort(pBuf, m_sMaxDamageUserid, index);	
		SetShort(pBuf, m_sNid+NPC_BAND, index);
		SetDWORD(pBuf, TYPE_MONEY_SID, index);
		SetDWORD(pBuf, m_iMoney, index);
		return;
	}	*/

	iRandom = myrand(70, 100);
	iMoney = m_iMoney * iRandom / 100;
	//m_iMoney, m_iItem;
	_NpcGiveItem m_GiveItemList[NPC_HAVE_ITEM_LIST];			// ItemList of NPCs
	if( iMoney <= 0 )	{
		nCount = 0;
	}
	else	{
		m_GiveItemList[0].sSid = TYPE_MONEY_SID;
		if( iMoney > 32767 ) {
			iMoney = 32000;						// Sungyong : Because it is a short type,,
			m_GiveItemList[0].count = iMoney;
		}
		else	m_GiveItemList[0].count = iMoney;
	}
	

	for(i = 0; i < m_pMain->m_NpcItem.m_nRow; i++)	{
		if(m_pMain->m_NpcItem.m_ppItem[i][0] != m_iItem) continue;
		for(int j=1; j<m_pMain->m_NpcItem.m_nField; j+=2)	{
			if(m_pMain->m_NpcItem.m_ppItem[i][j] == 0) continue;
			iRandom = myrand(1, 10000);
			iPer = m_pMain->m_NpcItem.m_ppItem[i][j+1];
			if(iPer == 0) continue;
			if(iRandom <= iPer)	{				// First, to refer to the base table
				if(j == 1)	{					// Create item...
					iMakeItemCode = ItemProdution(m_pMain->m_NpcItem.m_ppItem[i][j]);
					if(iMakeItemCode == 0)	continue;

					m_GiveItemList[nCount].sSid = iMakeItemCode;
					m_GiveItemList[nCount].count = 1;
				}
				else	{
					m_GiveItemList[nCount].sSid = m_pMain->m_NpcItem.m_ppItem[i][j];
					if( COMPARE(m_GiveItemList[nCount].sSid, ARROW_MIN, ARROW_MAX) )		// If it's an arrow
						m_GiveItemList[nCount].count = 20;
					else	
						m_GiveItemList[nCount].count = 1;
				}
				nCount++;
			}
		}
	}

	if( m_sMaxDamageUserid < 0 || m_sMaxDamageUserid > MAX_USER )	{
		//TRACE("### Npc-GiveNpcHaveItem() User Array Fail : [nid - %d,%s], userid=%d ###\n", m_sNid+NPC_BAND, m_strName, m_sMaxDamageUserid);
		return;
	}

	SetByte(pBuf, AG_NPC_GIVE_ITEM, index);
	SetShort(pBuf, m_sMaxDamageUserid, index);	
	SetShort(pBuf, m_sNid+NPC_BAND, index);
	SetShort(pBuf, m_sCurZone, index);
	SetShort(pBuf, (short)m_iRegion_X, index);
	SetShort(pBuf, (short)m_iRegion_Z, index);
	Setfloat(pBuf, m_fCurX, index);
	Setfloat(pBuf, m_fCurZ, index);
	Setfloat(pBuf, m_fCurY, index);
	SetByte(pBuf, nCount, index);
	for(i=0; i<nCount; i++)	{
		SetInt(pBuf, m_GiveItemList[i].sSid, index);
		SetShort(pBuf, m_GiveItemList[i].count, index);

		if( m_GiveItemList[i].sSid != TYPE_MONEY_SID )	{
			//sprintf( logfile, "%d\r\n", m_GiveItemList[i].sSid);
			string.Format("%d\r\n", m_GiveItemList[i].sSid);
			EnterCriticalSection( &g_LogFileWrite );
			m_pMain->m_ItemLogFile.Write(string, string.GetLength());
			LeaveCriticalSection( &g_LogFileWrite );
			//LogFileWrite( logfile );
		}
		//TRACE("Npc-GiveNpcHaveItem() : [nid - %d,%s,  giveme=%d, count=%d, num=%d], list=%d, count=%d\n", m_sNid+NPC_BAND, m_strName, m_sMaxDamageUserid, nCount, i, m_GiveItemList[i].sSid, m_GiveItemList[i].count);
	}

	SendAll(pIOCP, pBuf, index);
}


void CNpc::Yaw2D(float fDirX, float fDirZ, float& fYawResult)
{
	if ( fDirX >= 0.0f ) 
	{ 
		if ( fDirZ >= 0.0f ) 
			fYawResult = (float)(asin(fDirX)); 
		else 
			fYawResult = D3DXToRadian(90.0f) + (float)(acos(fDirX)); 
	}
	else 
	{ 
		if ( fDirZ >= 0.0f ) 
			fYawResult = D3DXToRadian(270.0f) + (float)(acos(-fDirX)); 
		else 
			fYawResult = D3DXToRadian(180.0f) + (float)(asin(-fDirX)); 
	}
}
  
__Vector3 CNpc::ComputeDestPos( __Vector3 vCur, float fDegree, float fDegreeOffset, float fDistance)
{
	__Vector3 vReturn, vDir;
	vDir.Set( 0.0f, 0.0f, 1.0f );
	__Matrix44 mtxRot; mtxRot.RotationY(D3DXToRadian(fDegree+fDegreeOffset));
	vDir *= mtxRot;
	vDir *= fDistance;
	vReturn = (vCur+vDir);
	return vReturn;
}

int	CNpc::GetPartyDamage(int iNumber)
{
	int i=0;
	int nDamage = 0;
	CUser* pUser = NULL;
	for(i = 0; i < NPC_HAVE_USER_LIST; i++)				// Once you search the list.
	{
		if(m_DamagedUserList[i].iUid < 0 || m_DamagedUserList[i].nDamage<= 0) continue;
		if(m_DamagedUserList[i].bIs == TRUE) pUser = m_pMain->GetUserPtr(m_DamagedUserList[i].iUid);
		if(pUser == NULL) continue;
		
		if(pUser->m_sPartyNumber != iNumber)	continue;

		nDamage += m_DamagedUserList[i].nDamage;
	}

	return nDamage;
}

void CNpc::NpcTypeParser()
{
	// Decide whether to go first or second
	switch(m_byActType)
	{
	case 1:
		m_tNpcAttType = m_tNpcOldAttType = 0;
		break;
	case 2:
		m_tNpcAttType = m_tNpcOldAttType = 0;
		m_byNpcEndAttType = 0;			
		break;
	case 3:
		m_tNpcGroupType = 1;
		m_tNpcAttType = m_tNpcOldAttType = 0;
		break;
	case 4:
		m_tNpcGroupType = 1;
		m_tNpcAttType = m_tNpcOldAttType = 0;
		m_byNpcEndAttType = 0;			
		break;
	case 6:
		m_byNpcEndAttType = 0;			
		break;
	case 5:
	case 7:
		m_tNpcAttType = m_tNpcOldAttType = 1;
		break;
	default :
		m_tNpcAttType = m_tNpcOldAttType = 1;
	}
}

void CNpc::HpChange(CIOCPort* pIOCP)
{
	m_fHPChangeTime = TimeGet();

	//if(m_NpcState == NPC_FIGHTING || m_NpcState == NPC_DEAD)	return;
	if(m_NpcState == NPC_DEAD)	return;
	if( m_iHP < 1 )	return;	// You can't recover when you're on the verge of death...
	if( m_iHP == m_iMaxHP)  return;	// Because HP is full...
	if(pIOCP == NULL)	return;
	
	//int amount =  (int)(m_sLevel*(1+m_sLevel/60.0) + 1) ;
	int amount =  (int)(m_iMaxHP / 20) ;

	char buff[256];
	memset( buff, 0x00, 256 );
	int send_index = 0;

	char logstr[256];
	memset( logstr, 0x00, 256 );
	sprintf( logstr, "Npc-HpChange : id=%d, cur_HP=%d, damage=%d\r\n", m_sNid+NPC_BAND, m_iHP, amount);
	//TRACE(logstr);

	m_iHP += amount;
	if( m_iHP < 0 )
		m_iHP = 0;
	else if ( m_iHP > m_iMaxHP )
		m_iHP = m_iMaxHP;

	memset( logstr, 0x00, 256 );
	sprintf( logstr, "Npc-HpChange-22 : id=%d, cur_HP=%d, damage=%d\r\n", m_sNid+NPC_BAND, m_iHP, amount);
	//if(m_iHP != m_iMaxHP)
	//	TRACE(logstr);

	SetByte( buff, AG_USER_SET_HP, send_index );
	SetShort( buff, m_sNid+NPC_BAND, send_index );
	SetDWORD( buff, m_iHP, send_index );
	SetDWORD( buff, m_iMaxHP, send_index );

	SendAll(pIOCP, buff, send_index);
}

BOOL CNpc::IsInExpRange(CUser* pUser)
{
	// Experience is given only to those who are within 50 meters of the NPC and the user.
	int iSearchRange = NPC_EXP_RANGE;		
	__Vector3 vStart, vEnd;
	float fDis = 0.0f;

	vStart.Set(m_fCurX, m_fCurY, m_fCurZ);
	vEnd.Set(pUser->m_curx, pUser->m_cury, pUser->m_curz);
	fDis = GetDistance(vStart, vEnd);
	if((int)fDis <= iSearchRange)
	{
		if(m_sCurZone == pUser->m_curZone)
			return TRUE;
	}

	return FALSE;
}

BOOL CNpc::CheckFindEnermy()
{
	// Excluding guards because they also attack monsters.
	if(m_tNpcType == NPC_GUARD || m_tNpcType == NPC_PATROL_GUARD || m_tNpcType == NPC_STORE_GUARD ) // || m_tNpcType == NPCTYPE_MONSTER)
		return TRUE;

	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size()) {
		TRACE("#### Npc-CheckFindEnermy ZoneIndex Fail : [name=%s], zoneindex=%d #####\n", m_strName, m_ZoneIndex);
		return FALSE;
	}

	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];

	if(m_iRegion_X > pMap->GetXRegionMax() || m_iRegion_Z > pMap->GetZRegionMax() || m_iRegion_X < 0 || m_iRegion_Z < 0)	{
		//TRACE("#### CheckFindEnermy Fail : [nid=%d, sid=%d], nRX=%d, nRZ=%d #####\n", m_sNid+NPC_BAND, m_sSid, m_iRegion_X, m_iRegion_Z);
		return FALSE;
	}

	if(pMap->m_ppRegion[m_iRegion_X][m_iRegion_Z].m_byMoving == 1)
		return TRUE;

	return FALSE;
}

void CNpc::MSpChange(int type, int amount)
{
	if( type == 2 ) {
		m_sMP += amount;
		if( m_sMP < 0 )
			m_sMP = 0;
		else if ( m_sMP > m_sMaxMP )
			m_sMP = m_sMaxMP;
	}
	else if( type == 3 ) {	// Monsters do not have SP.
	}
}

void CNpc::ItemWoreOut( int type, int damage )
{
	// Monster's armor durability reduction logic..
}

int	CNpc::ItemProdution(int item_number)							// crafting an item
{
	int iItemNumber = 0, iRandom = 0, i=0, iItemGrade = 0, iItemLevel = 0;
	int iDefault = 0, iItemCode=0, iItemKey=0, iRand2=0, iRand3=0, iRand4=0, iRand5=0;
	int iTemp1 = 0, iTemp2 = 0, iTemp3 = 0;

	iRandom = myrand(1, 10000);
	//iRandom = myrand(1, 4000);

	//TRACE("ItemProdution : nid=%d, sid=%d, name=%s, item_number = %d\n", m_sNid+NPC_BAND, m_sSid, m_strName, item_number);
	iItemGrade = GetItemGrade(item_number);
	//TRACE("ItemProdution : GetItemGrade() = %d\n", iItemGrade);
	if(iItemGrade == 0)		return 0;
	iItemLevel = m_sLevel / 5;

	if( COMPARE( iRandom, 1, 4001) )	{			// weapon item
		iDefault = 100000000;
		iRandom = myrand( 1, 10000 );				// Determine the type of weapon (dagger, sword, axe,,,,)
		if( COMPARE ( iRandom, 1, 701 ) )			iRand2 = 10000000;
		else if( COMPARE ( iRandom, 701, 1401 ) )	iRand2 = 20000000;
		else if( COMPARE ( iRandom, 1401, 2101 ) )	iRand2 = 30000000;
		else if( COMPARE ( iRandom, 2101, 2801 ) )	iRand2 = 40000000;
		else if( COMPARE ( iRandom, 2801, 3501 ) )	iRand2 = 50000000;
		else if( COMPARE ( iRandom, 3501, 5501 ) )	iRand2 = 60000000;
		else if( COMPARE ( iRandom, 5501, 6501 ) )	iRand2 = 70000000;
		else if( COMPARE ( iRandom, 6501, 8501 ) )	iRand2 = 80000000;
		else if( COMPARE ( iRandom, 8501, 10001 ) )	iRand2 = 90000000;

		iTemp1 = GetWeaponItemCodeNumber( 1 );
		//TRACE("ItemProdution : GetWeaponItemCodeNumber() = %d, iRand2=%d\n", iTemp1, iRand2);
		if( iTemp1 == 0 )	return 0;
		iItemCode = iTemp1 * 100000;	// Refer to routing distribution table

		iRand3 = myrand(1, 10000);					// Race (Elmo, Carus)
		if( COMPARE( iRand3, 1, 5000) )	iRand3 = 10000;
		else	iRand3 = 50000;
		iRand4 = myrand(1, 10000);					// Determines whether it is a one-handed or two-handed weapon
		if( COMPARE( iRand4, 1, 5000) )	iRand4 = 0;
		else	iRand4 = 5000000;
		
		iRandom = GetItemCodeNumber(iItemLevel, 1);	// Apply Ray Magic Table
		//TRACE("ItemProdution : GetItemCodeNumber() = %d, iRand2=%d, iRand3=%d, iRand4=%d\n", iRandom, iRand2, iRand3, iRand4);
		if(iRandom == -1)	{						// Invalid item creation failed
			return 0;
		}
		iRand5 = iRandom * 10;
		iItemNumber = iDefault + iItemCode + iRand2 + iRand3 + iRand4 + iRand5 + iItemGrade;

		//TRACE("ItemProdution : Weapon Success item_number = %d, default=%d, itemcode=%d, iRand2=%d, iRand3=%d, iRand4=%d, iRand5, iItemGrade=%d\n", iItemNumber, iDefault, iItemCode, iRand2, iRand3, iRand4, iRand5, iItemGrade);
	}
	else if( COMPARE( iRandom, 4001, 8001) )	{		// armor item
		iDefault = 200000000;			
		
		iTemp1 = GetWeaponItemCodeNumber( 2 );
		//TRACE("ItemProdution : GetWeaponItemCodeNumber() = %d\n", iTemp1 );
		if( iTemp1 == 0 )	return 0;
		iItemCode = iTemp1 * 1000000;		// Refer to routing distribution table

		if( m_byMaxDamagedNation == KARUS_MAN )	{
			iRandom = myrand(0, 10000);					// Decide on your class armor	
			if( COMPARE( iRandom, 0, 2000) )	{		
				iRand2 = 0;	
				iRand3 = 10000;							// Only Aktuarek should have the warrior armor
			}
			else if( COMPARE( iRandom, 2000, 4000) )	{
				iRand2 = 40000000;
				iRand3 = 20000;							// Rogue armor should only have Touareg
			}
			else if( COMPARE( iRandom, 4000, 6000) )	{
				iRand2 = 60000000;
				iRand3 = 30000;							// Sorcerer's armor should only have Wrinkle Touareg
			}
			else if( COMPARE( iRandom, 6000, 10001) )	{
				iRand2 = 80000000;
				iRandom = myrand(0, 10000);
				if( COMPARE( iRandom, 0, 5000) )	iRand3 = 20000;	// Priest's Armor is Touareg
				else								iRand3 = 40000;	// Priest's armor is Puritoarek
			}
		}
		else if( m_byMaxDamagedNation == ELMORAD_MAN )	{
			iRandom = myrand(0, 10000);					// Decide on your class armor		
			if( COMPARE( iRandom, 0, 3300) )	{		
				iRand2 = 0;	
				iItemKey = myrand(0, 10000);			// All races have warrior armor.
				if( COMPARE( iItemKey, 0, 3333) )			iRand3 = 110000;
				else if( COMPARE( iItemKey, 3333, 6666) )	iRand3 = 120000;
				else if( COMPARE( iItemKey, 6666, 10001) )	iRand3 = 130000;
			}
			else if( COMPARE( iRandom, 3300, 5600) )	{
				iRand2 = 40000000;
				iItemKey = myrand(0, 10000);			// Rogue armor can only be worn by men and women.
				if( COMPARE( iItemKey, 0, 5000) )	iRand3 = 120000;
				else								iRand3 = 130000;
			}
			else if( COMPARE( iRandom, 5600, 7800) )	{
				iRand2 = 60000000;
				iItemKey = myrand(0, 10000);			// Mage Armor is only for men and women.
				if( COMPARE( iItemKey, 0, 5000) )	iRand3 = 120000;
				else								iRand3 = 130000;
			}
			else if( COMPARE( iRandom, 7800, 10001) )	{
				iRand2 = 80000000;
				iItemKey = myrand(0, 10000);			// Priestly armor only available to men and women
				if( COMPARE( iItemKey, 0, 5000) )	iRand3 = 120000;
				else								iRand3 = 130000;
			}
			
		}
		
		iTemp2 = myrand(0, 10000);					// body part item determination
		if( COMPARE( iTemp2, 0, 2000) )				iRand4 = 1000;
		else if( COMPARE( iTemp2, 2000, 4000) )		iRand4 = 2000;
		else if( COMPARE( iTemp2, 4000, 6000) )		iRand4 = 3000;
		else if( COMPARE( iTemp2, 6000, 8000) )		iRand4 = 4000;
		else if( COMPARE( iTemp2, 8000, 10001) )	iRand4 = 5000;
		iRandom = GetItemCodeNumber(iItemLevel, 2);				// Apply Ray Magic Table
		if(iRandom == -1)	{		// Invalid item creation failed
			return 0;
		}
		iRand5 = iRandom * 10;
		iItemNumber = iDefault + iRand2 + iItemCode + iRand3 + iRand4 + iRand5 + iItemGrade;	// iItemGrade : Apply item grade generation table
		//TRACE("ItemProdution : Defensive Success item_number = %d, default=%d, iRand2=%d, itemcode=%d, iRand3=%d, iRand4=%d, iRand5, iItemGrade=%d\n", iItemNumber, iDefault, iRand2, iItemCode, iRand3, iRand4, iRand5, iItemGrade);
	}
	else if( COMPARE( iRandom, 8001, 10001) )	{       // accessory item
		iDefault = 300000000;
		iRandom = myrand(0, 10000);					// Determining the type of accessories (earrings, necklaces, rings, belts)
		if( COMPARE( iRandom, 0, 2500) )			iRand2 = 10000000;
		else if( COMPARE( iRandom, 2500, 5000) )	iRand2 = 20000000;
		else if( COMPARE( iRandom, 5000, 7500) )	iRand2 = 30000000;
		else if( COMPARE( iRandom, 7500, 10001) )	iRand2 = 40000000;
		iRand3 = myrand(1, 10000);					// Race (Elmorad, Carus)
		if( COMPARE( iRand3, 1, 5000) )	iRand3 = 110000;
		else	iRand3 = 150000;
		iRandom = GetItemCodeNumber(iItemLevel, 3);	// Apply Ray Magic Table
		//TRACE("ItemProdution : GetItemCodeNumber() = %d\n", iRandom);
		if(iRandom == -1)	{		// Invalid item creation failed
			return 0;
		}
		iRand4 = iRandom * 10;
		iItemNumber = iDefault + iRand2 + iRand3 + iRand4 + iItemGrade;
		//TRACE("ItemProdution : Accessary Success item_number = %d, default=%d, iRand2=%d, iRand3=%d, iRand4=%d, iItemGrade=%d\n", iItemNumber, iDefault, iRand2, iRand3, iRand4, iItemGrade);
	}
	
	return iItemNumber;
}

int  CNpc::GetItemGrade(int item_grade)
{
	int iPercent = 0, iRandom = 0, i=0;
	int iItemGrade[9];
	_MAKE_ITEM_GRADE_CODE* pItemData = NULL;

	iRandom = myrand(1, 1000);
	pItemData = m_pMain->m_MakeGradeItemArray.GetData(item_grade); 
	if(pItemData == NULL)	return 0;

	iItemGrade[0] = pItemData->sGrade_1;	iItemGrade[1] = pItemData->sGrade_2;
	iItemGrade[2] = pItemData->sGrade_3;	iItemGrade[3] = pItemData->sGrade_4;
	iItemGrade[4] = pItemData->sGrade_5;	iItemGrade[5] = pItemData->sGrade_6;
	iItemGrade[6] = pItemData->sGrade_7;	iItemGrade[7] = pItemData->sGrade_8;
	iItemGrade[8] = pItemData->sGrade_9;	
	
	for(i=0; i<9; i++)	{
		if(i == 0)	{
			if(iItemGrade[i] == 0)	{
				iPercent += iItemGrade[i];
				continue;
			}
			if( COMPARE( iRandom, 0, iItemGrade[i]) )	return i+1;
			else	{
				iPercent += iItemGrade[i];
				continue;
			}
		}
		else	{
			if(iItemGrade[i] == 0)	{
				iPercent += iItemGrade[i];
				continue;
			}

			if( COMPARE( iRandom, iPercent, iPercent+iItemGrade[i]) )	return i+1;
			else	{
				iPercent += iItemGrade[i];
				continue;
			}
		}
		
	}

	return 0;
}

int  CNpc::GetWeaponItemCodeNumber(int item_type)
{
	int iPercent = 0, iRandom = 0, i=0, iItem_level = 0;
	_MAKE_WEAPON* pItemData = NULL;

	iRandom = myrand(0, 1000);
	if( item_type == 1 )	{		// weapons
		iItem_level = m_sLevel / 10;
		pItemData = m_pMain->m_MakeWeaponItemArray.GetData(iItem_level); 
	}
	else if( item_type == 2 )	{	// armor
		iItem_level = m_sLevel / 10;
		pItemData = m_pMain->m_MakeDefensiveItemArray.GetData(iItem_level); 
	}

	if(pItemData == NULL)	return 0;

	for(i=0; i<MAX_UPGRADE_WEAPON; i++)	{
		if(i == 0)	{
			if(pItemData->sClass[i] == 0)	{
				iPercent += pItemData->sClass[i];
				continue;
			}
			if( COMPARE( iRandom, 0, pItemData->sClass[i]) )	return i+1;
			else	{
				iPercent += pItemData->sClass[i];
				continue;
			}
		}
		else	{
			if(pItemData->sClass[i] == 0)	{
				iPercent += pItemData->sClass[i];
				continue;
			}

			if( COMPARE( iRandom, iPercent, iPercent+pItemData->sClass[i]) )	return i+1;
			else	{
				iPercent += pItemData->sClass[i];
				continue;
			}
		}
	}

	return 0;
}

int  CNpc::GetItemCodeNumber(int level, int item_type)
{
	int iItemCode = 0, iRandom = 0, i=0, iItemType = 0, iPercent = 0;
	int iItemPercent[3];
	_MAKE_ITEM_LARE_CODE* pItemData = NULL;

	iRandom = myrand(0, 1000);
	pItemData = m_pMain->m_MakeLareItemArray.GetData(level); 
	if(pItemData == NULL)	return -1;
	iItemPercent[0] = pItemData->sLareItem;
	iItemPercent[1] = pItemData->sMagicItem;
	iItemPercent[2] = pItemData->sGereralItem;

	for(i=0; i<3; i++)	{
		if(i == 0)	{
			if( COMPARE( iRandom, 0, iItemPercent[i]) )	{
				iItemType = i+1;
				break;
			}
			else	{
				iPercent += iItemPercent[i];
				continue;
			}
		}
		else	{
			if( COMPARE( iRandom, iPercent, iPercent+iItemPercent[i]) )	{
				iItemType = i+1;
				break;
			}
			else	{
				iPercent += iItemPercent[i];
				continue;
			}
		}
	}

	switch(iItemType)	{
		case 0:						// wrong item
			iItemCode = 0;
			break;
		case 1:						// lare item
			if(item_type == 1)	{			// weapons
				iItemCode = myrand(16, 24);
			}
			else if(item_type == 2)	{		// armor
				iItemCode = myrand(12, 24);
			}
			else if(item_type == 3)	{		// Accessories
				iItemCode = myrand(0, 10);
			}
			break;
		case 2:						// magic item
			if(item_type == 1)	{			// weapons
				iItemCode = myrand(6, 15);
			}
			else if(item_type == 2)	{		// armor
				iItemCode = myrand(6, 11);
			}
			else if(item_type == 3)	{		// Accessories
				iItemCode = myrand(0, 10);
			}
			break;
		case 3:						// general item
			if(item_type == 1)	{			// weapons
				iItemCode = 5;
			}
			else if(item_type == 2)	{		// armor
				iItemCode = 5;
			}
			else if(item_type == 3)	{		// Accessories
				iItemCode = myrand(0, 10);
			}
			break;	
	}

	return iItemCode;
}

void CNpc::DurationMagic_4(CIOCPort* pIOCP, float currenttime)
{
	int send_index = 0, buff_type = 0 ;					
	char send_buff[128] ;
	memset( send_buff, 0x00, 128 ) ;

	// Dungeon Work : In the case of a dungeon monster, if the corresponding leader monster dies, my status is changed to a dead state...
	if(m_ZoneIndex < 0 || m_ZoneIndex > m_pMain->g_arZone.size())	{
		TRACE("#### Npc-DurationMagic_4() ZoneIndex Fail : [nid=%d, name=%s], zoneindex=%d #####\n", m_sNid+NPC_BAND, m_strName, m_ZoneIndex);
		return;
	}
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(pMap == NULL)	return;

	if( m_byDungeonFamily > 0)	{

		CRoomEvent* pRoom = NULL;
		//if( m_byDungeonFamily < 0 || m_byDungeonFamily >= MAX_DUNGEON_BOSS_MONSTER )	{
		if( m_byDungeonFamily < 0 || m_byDungeonFamily > pMap->m_arRoomEventArray.GetSize()+1 )	{
			TRACE("#### Npc-DurationMagic_4() m_byDungeonFamily Fail : [nid=%d, name=%s], m_byDungeonFamily=%d #####\n", m_sNid+NPC_BAND, m_strName, m_byDungeonFamily);
			//return;
		}
		else	{
			pRoom = pMap->m_arRoomEventArray.GetData( m_byDungeonFamily );
			if( !pRoom )	{
				TRACE("#### Npc-DurationMagic_4() room Fail : [nid=%d, name=%s], m_byDungeonFamily=%d #####\n", m_sNid+NPC_BAND, m_strName, m_byDungeonFamily);
			}
			else	{
				//if( pMap->m_arDungeonBossMonster[m_byDungeonFamily] == 0 )	{	// When the captain monster dies
				if( pRoom->m_byStatus == 3 )	{	// When the room is cleared
					if( m_NpcState != NPC_DEAD )	{
						if( m_byRegenType == 0 )	{		
							m_byRegenType = 2;      // So that it does not become a regen,
							Dead( pIOCP, 1 );
							return;
						}
					}
					//return;
				}
			}
		}
	}

	for(int i=0; i<MAX_MAGIC_TYPE4; i++)	{
		if (m_MagicType4[i].sDurationTime) {
			if (currenttime > (m_MagicType4[i].fStartTime + m_MagicType4[i].sDurationTime)) {
				m_MagicType4[i].sDurationTime = 0;		
				m_MagicType4[i].fStartTime = 0.0f;
				m_MagicType4[i].byAmount = 0;
				buff_type = i+1 ;
				if(i == 5)	{					// Regarding speed... stats...
					m_fSpeed_1 = m_fOldSpeed_1;
					m_fSpeed_2 = m_fOldSpeed_2;
				}
			}
		}
	}
/*
	if (buff_type) {
		SetByte( send_buff, AG_MAGIC_ATTACK_RESULT, send_index );
		SetByte( send_buff, MAGIC_TYPE4_END, send_index );	
		SetByte( send_buff, buff_type, send_index ); 
		SendAll(pIOCP, send_buff, send_index ); 
	}	*/
}

// It changes the information of the monster that changes...
void CNpc::ChangeMonsterInfomation(int iChangeType)
{
	// sungyong test
	//m_sChangeSid = 500;		m_byChangeType = 2;

	if( m_sChangeSid == 0 || m_byChangeType == 0 )	return;			// Monsters that don't change
	if( m_NpcState != NPC_DEAD )	return;		// dead state
	
	CNpcTable*	pNpcTable = NULL;
	if(m_byInitMoveType >= 0 && m_byInitMoveType < 100)	{
		if(iChangeType == 1)	// Transform into another monster..
			pNpcTable = m_pMain->m_arMonTable.GetData(m_sChangeSid);
		else if(iChangeType == 2)	// Transform into the original monster..
			pNpcTable = m_pMain->m_arMonTable.GetData(m_sSid);
		if(pNpcTable == NULL)	{
			TRACE("##### ChangeMonsterInfomation Sid Fail -- Sid = %d #####\n", m_sChangeSid);
		}
	}
	else if(m_byInitMoveType >= 100)	{
		if(iChangeType == 1)
			pNpcTable = m_pMain->m_arNpcTable.GetData(m_sChangeSid);
		else if(iChangeType == 2)
			pNpcTable = m_pMain->m_arNpcTable.GetData(m_sSid);
		if(pNpcTable == NULL)	{
			TRACE("##### ChangeMonsterInfomation Sid Fail -- Sid = %d #####\n", m_sChangeSid);
		}
	}

	_tcscpy(m_strName, pNpcTable->m_strName);	// MONSTER(NPC) Name
	m_sPid		= pNpcTable->m_sPid;		// MONSTER(NPC) Picture ID
	m_sSize		= pNpcTable->m_sSize;		// Character proportions (by 100 percent)
	m_iWeapon_1		= pNpcTable->m_iWeapon_1;
	m_iWeapon_2		= pNpcTable->m_iWeapon_2;
	m_byGroup		= pNpcTable->m_byGroup;		// belonging group
	m_byActType		= pNpcTable->m_byActType;	// behavior pattern
	m_byRank		= pNpcTable->m_byRank;		// title
	m_byTitle		= pNpcTable->m_byTitle;		// status
	m_iSellingGroup = pNpcTable->m_iSellingGroup;
	m_sLevel		= pNpcTable->m_sLevel;		// level
	m_iExp			= pNpcTable->m_iExp;
	m_iLoyalty		= pNpcTable->m_iLoyalty;	// loyalty
	m_iHP			= pNpcTable->m_iMaxHP;
	m_iMaxHP		= pNpcTable->m_iMaxHP;
	m_sMP			= pNpcTable->m_sMaxMP;
	m_sMaxMP		= pNpcTable->m_sMaxMP;
	m_sAttack		= pNpcTable->m_sAttack;
	m_sDefense		= pNpcTable->m_sDefense;
	m_sHitRate		= pNpcTable->m_sHitRate;
	m_sEvadeRate	= pNpcTable->m_sEvadeRate;
	m_sDamage		= pNpcTable->m_sDamage;
	m_sAttackDelay	= pNpcTable->m_sAttackDelay;
	m_sSpeed		= pNpcTable->m_sSpeed;
	m_fSpeed_1		= (float)pNpcTable->m_bySpeed_1;	// basic movement type
	m_fSpeed_2		= (float)pNpcTable->m_bySpeed_2;	// Jumping movement type.
	m_fOldSpeed_1	= (float)pNpcTable->m_bySpeed_1;	// basic movement type
	m_fOldSpeed_2	= (float)pNpcTable->m_bySpeed_2;	// Jumping movement type.
	m_sStandTime	= pNpcTable->m_sStandTime;	// standing time
	m_iMagic1		= pNpcTable->m_iMagic1;
	m_iMagic2		= pNpcTable->m_iMagic2;
	m_iMagic3		= pNpcTable->m_iMagic3;
	m_byFireR		= pNpcTable->m_byFireR;
	m_byColdR		= pNpcTable->m_byColdR;
	m_byLightningR	= pNpcTable->m_byLightningR;
	m_byMagicR		= pNpcTable->m_byMagicR;
	m_byDiseaseR	= pNpcTable->m_byDiseaseR;
	m_byPoisonR		= pNpcTable->m_byPoisonR;
	m_byLightR		= pNpcTable->m_byLightR;
	m_fBulk			= (float)( ((double)pNpcTable->m_sBulk / 100) * ((double)pNpcTable->m_sSize / 100) );
	m_bySearchRange	= pNpcTable->m_bySearchRange;
	m_byAttackRange	= pNpcTable->m_byAttackRange;
	m_byTracingRange	= pNpcTable->m_byTracingRange;
	m_sAI				= pNpcTable->m_sAI;				// artificial intelligence index
	m_tNpcType		= pNpcTable->m_tNpcType;		// NPC Type
	m_byFamilyType	= pNpcTable->m_byFamilyType;		// Determines family relationships among mobs.
	m_iMoney		= pNpcTable->m_iMoney;
	m_iItem			= pNpcTable->m_iItem;			// falling items
	m_tNpcLongType	= pNpcTable->m_byDirectAttack;	
	m_byWhatAttackType = pNpcTable->m_byMagicAttack;
}

void CNpc::DurationMagic_3(CIOCPort* pIOCP, float currenttime)
{
	int duration_damage = 0;

	for(int i=0; i<MAX_MAGIC_TYPE3; i++)	{
		if (m_MagicType3[i].byHPDuration) {
			if (currenttime >= (m_MagicType3[i].fStartTime + m_MagicType3[i].byHPInterval)) {		// every 2 seconds
				m_MagicType3[i].byHPInterval += 2;
				//TRACE("DurationMagic_3,, [%d] curtime = %.2f, dur=%.2f, nid=%d, damage=%d\n", i, currenttime, m_MagicType3[i].fStartTime, m_sNid+NPC_BAND, m_MagicType3[i].sHPAmount);

				if( m_MagicType3[i].sHPAmount >= 0 )	{
				}
				else {
					duration_damage = m_MagicType3[i].sHPAmount;
					duration_damage = abs(duration_damage);
					if( SetDamage(0, duration_damage, (TCHAR*)("**duration**"), m_MagicType3[i].sHPAttackUserID , pIOCP ) == FALSE )	{
						SendExpToUserList();
						SendDead(pIOCP);
						SendAttackSuccess(pIOCP, MAGIC_ATTACK_TARGET_DEAD, m_MagicType3[i].sHPAttackUserID, duration_damage, m_iHP, 1, DURATION_ATTACK);
						//TRACE("&&&& Duration Magic attack .. pNpc->m_byHPInterval[%d] = %d &&&& \n", i, m_MagicType3[i].byHPInterval);
						m_MagicType3[i].fStartTime = 0.0f;
						m_MagicType3[i].byHPDuration = 0;
						m_MagicType3[i].byHPInterval = 2;
						m_MagicType3[i].sHPAmount = 0;
						m_MagicType3[i].sHPAttackUserID = -1; 
						duration_damage = 0;
					}
					else	{
						SendAttackSuccess(pIOCP, ATTACK_SUCCESS, m_MagicType3[i].sHPAttackUserID, duration_damage, m_iHP, 1, DURATION_ATTACK);	
						//TRACE("&&&& Duration Magic attack .. pNpc->m_byHPInterval[%d] = %d &&&& \n", i, m_MagicType3[i].byHPInterval);
					}
				}

				if (currenttime >= (m_MagicType3[i].fStartTime + m_MagicType3[i].byHPDuration)) {	// total attack time.
					m_MagicType3[i].fStartTime = 0.0f;
					m_MagicType3[i].byHPDuration = 0;
					m_MagicType3[i].byHPInterval = 2;
					m_MagicType3[i].sHPAmount = 0;
					m_MagicType3[i].sHPAttackUserID = -1;
					duration_damage = 0;
				}
			}
		}
	}	
}

/////////////////////////////////////////////////////////////////////////////
//	When the NPC is sleeping.
//
void CNpc::NpcSleeping(CIOCPort* pIOCP)
{
	//if(cur_test)		NpcTrace(_T("NpcSleeping()"));

	// sungyong test~
	/*
	if(m_byChangeType == 1)	{
		Dead(pIOCP, 1);
		ChangeMonsterInfomation(1);
		return;
	}	*/
	// ~sungyong test


	if(m_pMain->m_byNight == 1)	{
		m_NpcState = NPC_STANDING;
		m_Delay = 0;
	}
	else	{
		m_NpcState = NPC_SLEEPING;
		m_Delay = m_sStandTime;
	}
	
	m_fDelayTime = TimeGet();
}

/////////////////////////////////////////////////////////////////////////////
// The monster is stunned...
void CNpc::NpcFainting(CIOCPort* pIOCP, float currenttime)
{
	//if(cur_test)	NpcTrace(_T("NpcFainting()"));
	// After being stunned for 2 seconds,, in a standing state...
	if (currenttime > (m_fFaintingTime + FAINTING_TIME)) {
		m_NpcState = NPC_STANDING;
		m_Delay = 0;
		m_fDelayTime = TimeGet();
		m_fFaintingTime = 0.0f;
	}
}

/////////////////////////////////////////////////////////////////////////////
// The monster is in a healing state...
void CNpc::NpcHealing(CIOCPort* pIOCP)
{
	//if(cur_test)	NpcTrace(_T("NpcHealing()"));

	if( m_tNpcType != NPC_HEALER )	{
		InitTarget();
		m_NpcState = NPC_STANDING;
		m_Delay = m_sStandTime;
		m_fDelayTime = TimeGet();
		return;
	}

	// Determine whether the treatment target has completed treatment.
	CNpc* pNpc = NULL;
	int nID = m_Target.id;
	BOOL bFlag = FALSE;
	char buff[256];
	int send_index = 0, iHP = 0;
	memset( buff, 0x00, 256 );	send_index = 0;

	int ret = 0;
	int nStandingTime = m_sStandTime;

	ret = IsCloseTarget(m_byAttackRange, 2);

	if(ret == 0)   {
		if(m_tNpcType == NPC_DOOR || m_tNpcType == NPC_ARTIFACT || m_tNpcType == NPC_PHOENIX_GATE || m_tNpcType == NPC_GATE_LEVER || m_tNpcType == NPC_DOMESTIC_ANIMAL || m_tNpcType == NPC_SPECIAL_GATE || m_tNpcType == NPC_DESTORY_ARTIFACT ) // 고정 경비병은 추적을 하지 않도록..
		{
			m_NpcState = NPC_STANDING;
			InitTarget();
			m_Delay = 0;
			m_fDelayTime = TimeGet();
			return;
		}	
		m_sStepCount = 0;
		m_byActionFlag = ATTACK_TO_TRACE;
		m_NpcState = NPC_TRACING;			// To catch up with attacking and running away users (faster reaction)
		m_Delay = 0;
		m_fDelayTime = TimeGet();
		return;							// Update user x, y values in IsCloseTarget() and zoom to Delay = 0
	}	
	else if( ret == 2 )	{
		//if(m_tNpcType == NPC_BOSS_MONSTER)	{		// If it's the boss monster...
		if(m_tNpcLongType == 2)	{		// Since it is a monster capable of direct and indirect (long) attacks, it is capable of long-range attacks.
			m_Delay = LongAndMagicAttack(pIOCP);
			m_fDelayTime = TimeGet();
			return;
		}
		else	{
			if(m_tNpcType == NPC_DOOR || m_tNpcType == NPC_ARTIFACT || m_tNpcType == NPC_PHOENIX_GATE || m_tNpcType == NPC_GATE_LEVER || m_tNpcType == NPC_DOMESTIC_ANIMAL || m_tNpcType == NPC_SPECIAL_GATE || m_tNpcType == NPC_DESTORY_ARTIFACT ) // 고정 경비병은 추적을 하지 않도록..
			{
				m_NpcState = NPC_STANDING;
				InitTarget();
				m_Delay = 0;
				m_fDelayTime = TimeGet();
				return;
			}	
			m_sStepCount = 0;
			m_byActionFlag = ATTACK_TO_TRACE;
			m_NpcState = NPC_TRACING;			// To catch up with attacking and running away users (faster reaction)
			m_Delay = 0;
			m_fDelayTime = TimeGet();
			return;								// Update user x, y values in IsCloseTarget() and zoom to Delay = 0
		}
	}
	else if( ret == -1 )	{
		m_NpcState = NPC_STANDING;
		InitTarget();
		m_Delay = 0;
		m_fDelayTime = TimeGet();
		return;
	}

	if(nID >= NPC_BAND && nID < INVALID_BAND)	{
		pNpc = m_pMain->m_arNpc.GetData(nID - NPC_BAND);

		if(pNpc == NULL)	{
			InitTarget();
		}

		if(pNpc->m_iHP <= 0 || pNpc->m_NpcState == NPC_DEAD)	{
			InitTarget();
		}

		iHP = pNpc->m_iMaxHP * 0.9;
		if( pNpc->m_iHP >= iHP)	{
			InitTarget();
		}
		else	{
			memset( buff, 0x00, 256 );	send_index = 0;
			//SetByte( buff, AG_MAGIC_ATTACK_RESULT, send_index );
			SetByte( buff, MAGIC_EFFECTING, send_index );		
			SetDWORD( buff, m_iMagic3, send_index );				// FireBall
			SetShort( buff, m_sNid+NPC_BAND, send_index );
			SetShort( buff, nID, send_index );	
			SetShort( buff, 0, send_index );					// data0
			SetShort( buff, 0, send_index );	
			SetShort( buff, 0, send_index );	
			SetShort( buff, 0, send_index );	
			SetShort( buff, 0, send_index );	
			SetShort( buff, 0, send_index );	
			m_MagicProcess.MagicPacket(buff, send_index, pIOCP);

			m_Delay = m_sAttackDelay;
			m_fDelayTime = TimeGet();
			return;
			//SendAll(pIOCP, buff, send_index);
		}
	}


	// Find a new treatment target and heal it
	int iMonsterNid = FindFriend( 2 );

	if(iMonsterNid == 0)	{
		InitTarget();
		m_NpcState = NPC_STANDING;
		m_Delay = m_sStandTime;
		m_fDelayTime = TimeGet();
		return;
	}

	memset( buff, 0x00, 256 );	send_index = 0;
	//SetByte( buff, AG_MAGIC_ATTACK_RESULT, send_index );
	SetByte( buff, MAGIC_EFFECTING, send_index );		
	SetDWORD( buff, m_iMagic3, send_index );				// FireBall
	SetShort( buff, m_sNid+NPC_BAND, send_index );
	SetShort( buff, iMonsterNid, send_index );	
	SetShort( buff, 0, send_index );					// data0
	SetShort( buff, 0, send_index );	
	SetShort( buff, 0, send_index );	
	SetShort( buff, 0, send_index );	
	SetShort( buff, 0, send_index );	
	SetShort( buff, 0, send_index );	

	m_MagicProcess.MagicPacket(buff, send_index, pIOCP);
	//SendAll(pIOCP, buff, send_index);

	m_Delay = m_sAttackDelay;
	m_fDelayTime = TimeGet();
}

int CNpc::GetPartyExp( int party_level, int man, int nNpcExp )
{
	int nPartyExp = 0;
	int nLevel = party_level / man;
	double TempValue = 0;
	nLevel = m_sLevel - nLevel;

	//TRACE("GetPartyExp ==> party_level=%d, man=%d, exp=%d, nLevle=%d, mon=%d\n", party_level, man, nNpcExp, nLevel, m_sLevel);

	if(nLevel < 2)	{
		nPartyExp = nNpcExp * 1;
	}
	else if(nLevel >= 2 && nLevel < 5)	{
		TempValue = nNpcExp * 1.2;
		nPartyExp = (int)TempValue;
		if(TempValue > nPartyExp)  nPartyExp=nPartyExp+1;
	}
	else if(nLevel >= 5 && nLevel < 8)	{
		TempValue = nNpcExp * 1.5;
		nPartyExp = (int)TempValue;
		if(TempValue > nPartyExp)  nPartyExp=nPartyExp+1;
	}
	else if(nLevel >= 8)	{
		nPartyExp = nNpcExp * 2;
	}	

	return nPartyExp;
}

void CNpc::ChangeAbility(int iChangeType)	// iChangeType - 0: ability level down, 1: ability level recovery
{
	if( iChangeType > 2 )	return;			// 

	int nHP = 0, nAC=0, nDamage=0, nLightR=0, nMagicR=0, nDiseaseR=0, nPoisonR=0, nLightningR=0, nFireR=0, nColdR=0;
	CNpcTable*	pNpcTable = NULL;
	if(m_byInitMoveType >= 0 && m_byInitMoveType < 100)	{
		TRACE("##### ChangeAbility Fail -- nid = %d, name=%s #####\n", m_sNid+NPC_BAND, m_strName);
		return;
	}
	else if(m_byInitMoveType >= 100)	{
		pNpcTable = m_pMain->m_arNpcTable.GetData(m_sSid);
		if(pNpcTable == NULL)	{
			TRACE("##### ChangeAbility Sid Fail -- nid=%d, Sid = %d, name=%s #####\n", m_sNid+NPC_BAND, m_sChangeSid, m_strName);
			return;
		}
	}

	// Changing information......
	if( iChangeType == BATTLEZONE_OPEN )	{		// ability down
		nHP = pNpcTable->m_iMaxHP * 0.5;
		nAC = pNpcTable->m_sDefense * 0.2;
		nDamage = pNpcTable->m_sDamage * 0.3;
		nLightR = pNpcTable->m_byLightR*0.5;
		nMagicR = pNpcTable->m_byMagicR*0.5;
		nDiseaseR = pNpcTable->m_byDiseaseR*0.5;
		nPoisonR = pNpcTable->m_byPoisonR*0.5;
		nLightningR = pNpcTable->m_byLightningR*0.5;
		nFireR = pNpcTable->m_byFireR*0.5;
		nColdR = pNpcTable->m_byColdR*0.5;
		m_iMaxHP = nHP;
		if( m_iHP > nHP )	{	// I'll have to change my HP too.
			HpChange( &m_pMain->m_Iocport );
		}
		m_sDefense = nAC;
		m_sDamage = nDamage;
		m_byFireR		= nFireR;
		m_byColdR		= nColdR;
		m_byLightningR	= nLightningR;
		m_byMagicR		= nMagicR;
		m_byDiseaseR	= nDiseaseR;
		m_byPoisonR		= nPoisonR;
		m_byLightR		= nLightR;
		//TRACE("-- ChangeAbility down : nid=%d, name=%s, hp:%d->%d, damage=%d->%d\n", m_sNid+NPC_BAND, m_strName, pNpcTable->m_iMaxHP, nHP, pNpcTable->m_sDamage, nDamage); 
	}
	else if( iChangeType == BATTLEZONE_CLOSE )	{
		m_iMaxHP		= pNpcTable->m_iMaxHP;
		//TRACE("++ ChangeAbility up : nid=%d, name=%s, hp:%d->%d, damage=%d->%d\n", m_sNid+NPC_BAND, m_strName, m_iHP, m_iMaxHP, pNpcTable->m_sDamage, nDamage); 
		if( m_iMaxHP > m_iHP )	{
			m_iHP = m_iMaxHP - 50;
			HpChange( &m_pMain->m_Iocport );
		}
		m_sDamage		= pNpcTable->m_sDamage;
		m_sDefense		= pNpcTable->m_sDefense;
		m_byFireR		= pNpcTable->m_byFireR;
		m_byColdR		= pNpcTable->m_byColdR;
		m_byLightningR	= pNpcTable->m_byLightningR;
		m_byMagicR		= pNpcTable->m_byMagicR;
		m_byDiseaseR	= pNpcTable->m_byDiseaseR;
		m_byPoisonR		= pNpcTable->m_byPoisonR;
		m_byLightR		= pNpcTable->m_byLightR;
	}
}

BOOL CNpc::Teleport(CIOCPort* pIOCP)
{
	int send_index = 0, i=0;
	char buff[256];	memset( buff, 0x00, 256 );
	int nX=0, nZ=0, nTileX=0, nTileZ=0;
	MAP* pMap = m_pMain->g_arZone[m_ZoneIndex];
	if(pMap == NULL)	return FALSE;

	while(1)	{
		i++;
		nX = myrand(0, 10);
		nX = myrand(0, 10);
		nX = (int)m_fCurX + nX;
		nZ = (int)m_fCurZ + nZ;
		nTileX = nX / TILE_SIZE;
		nTileZ = nZ / TILE_SIZE;

		if(nTileX >= (pMap->m_sizeMap.cx-1))		nTileX = pMap->m_sizeMap.cx-1;
		if(nTileZ >= (pMap->m_sizeMap.cy-1))		nTileZ = pMap->m_sizeMap.cy-1;

		if(nTileX < 0 || nTileZ < 0)	{
			TRACE("#### Npc-SetLive() Fail : nTileX=%d, nTileZ=%d #####\n", nTileX, nTileZ);
			return FALSE;
		}

		if(pMap->m_pMap[nTileX][nTileZ].m_sEvent <= 0)	{
			if(i >= 500)	{
				TRACE("### Teleport fail : sid = %d, nid = %d, zone=%d, loop = %d 나 이동자리가 이상해... 고쳐줘... x = %d, y = %d\n", m_sSid, m_sNid+NPC_BAND, m_sCurZone, i, nX, nZ);
				return FALSE;
			}
			continue;
		}
		break;
	}	

	SetByte( buff, AG_NPC_INOUT, send_index );
	SetByte( buff, NPC_OUT, send_index );
	SetShort( buff, m_sNid+NPC_BAND, send_index );
	Setfloat(buff, m_fCurX, send_index);
	Setfloat(buff, m_fCurZ, send_index);
	Setfloat(buff, m_fCurY, send_index);
	SendAll(pIOCP, buff, send_index);

	m_fCurX = nX;	m_fCurZ = nZ;

	memset( buff, 0x00, 256 );	send_index = 0;
	SetByte( buff, AG_NPC_INOUT, send_index );
	SetByte( buff, NPC_IN, send_index );
	SetShort( buff, m_sNid+NPC_BAND, send_index );
	Setfloat(buff, m_fCurX, send_index);
	Setfloat(buff, m_fCurZ, send_index);
	Setfloat(buff, 0, send_index);
	SendAll(pIOCP, buff, send_index);

	SetUid(m_fCurX, m_fCurZ, m_sNid + NPC_BAND);

	return TRUE;
}