// User.h: interface for the CUser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USER_H__1CC8CB68_CF95_4849_8E89_134826B1FAC2__INCLUDED_)
#define AFX_USER_H__1CC8CB68_CF95_4849_8E89_134826B1FAC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "iocport.h"
#include "MagicProcess.h"
class CServerDlg;

#include "extern.h"
#include "STLMap.h"
#include <list>

typedef std::list <_USERLOG*>		UserLogList;

class CUser  
{
public:
	CServerDlg* m_pMain;

	CIOCPort* m_pIocport;	
	CMagicProcess m_MagicProcess;

	UserLogList	m_UserLogList;

	char m_strUserID[MAX_ID_SIZE+1];
	int		m_iUserId;
	BYTE	m_bLive;

	float			m_curx;
	float			m_cury;
	float			m_curz;
	float			m_fWill_x;
	float			m_fWill_y;
	float			m_fWill_z;
	short			m_sSpeed;
	BYTE 			m_curZone;
	short			m_sZoneIndex;

	BYTE	m_bNation;
	short	m_sLevel;

	short	m_sHP;
	short	m_sMP;
	short	m_sSP;
	short	m_sMaxHP;
	short	m_sMaxMP;
	short	m_sMaxSP;

	BYTE	m_state;

	short	m_sRegionX;
	short	m_sRegionZ;
	short	m_sOldRegionX;
	short	m_sOldRegionZ;

	BYTE	m_bResHp;
	BYTE	m_bResMp;
	BYTE	m_bResSta;

	BYTE    m_byNowParty;
	BYTE	m_byPartyTotalMan;
	short   m_sPartyTotalLevel;
	short	m_sPartyNumber;

	short	m_sHitDamage;
	float	m_fHitrate;	
	float	m_fAvoidrate;
	short	m_sAC;
	short   m_sItemAC;
	

	short  m_sSurroundNpcNumber[8];

	BYTE   m_byIsOP;					// Determine if you are an operator.
	long   m_lUsed;						// Use of pointer.. (1: In use.. Access not allowed. 0: It is okay to use)

	BOOL		m_bLogOut;				// Are you logging out?

	// 2002.7.10 - Yookozuna
	BYTE    m_bMagicTypeLeftHand;			// The type of magic item in user's left hand  
	BYTE    m_bMagicTypeRightHand;			// The type of magic item in user's right hand
	short   m_sMagicAmountLeftHand;         // The amount of magic item in user's left hand
	short	m_sMagicAmountRightHand;        // The amount of magic item in user's left hand

public:
	void InitUserLog();
	void WriteUserLog();
	short GetMagicDamage(int damage, short tid);
	void Initialize();
	void InitNpcAttack();
	void Attack(int sid, int tid);
	void SetDamage(int damage, int tid);
	void Dead(int tid, int nDamage);
	void SetExp(int iNpcExp, int iLoyalty, int iLevel);
	void SetPartyExp(int iNpcExp, int iLoyalty, int iPartyLevel, int iMan);
	short GetDamage(int tid, int magicid=0);
	BYTE GetHitRate(float rate);
	int IsSurroundCheck(float fX, float fY, float fZ, int NpcID);
	void HealMagic();
	void HealAreaCheck(int rx, int rz);

	void SendAttackSuccess(int tuid, BYTE result, short sDamage, int nHP=0, short sAttack_type=1);
	void SendMagicAttackResult(int tuid, BYTE result, short sDamage, short sHP=0);
	void SendHP();
	void SendExp(int iExp, int iLoyalty, int tType = 1);
	void SendSystemMsg(TCHAR *pMsg, BYTE type, int nWho);
	void SendAll(TCHAR *pBuf, int nLength);
	BOOL IsOpIDCheck(char* szName);

	CUser();
	virtual ~CUser();
};

#endif // !defined(AFX_USER_H__1CC8CB68_CF95_4849_8E89_134826B1FAC2__INCLUDED_)
