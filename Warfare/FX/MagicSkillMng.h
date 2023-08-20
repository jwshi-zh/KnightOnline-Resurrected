#pragma once

#include "GameDef.h"
#include "GameBase.h"
#include <map>

class CMagicSkillMng : public CGameBase
{
public:
	class CGameProcMain* m_pGameProcMain;
	
	CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_1>*	m_pTbl_Type_1;
	CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_2>*	m_pTbl_Type_2;
	CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_3>*	m_pTbl_Type_3;
	CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_4>*	m_pTbl_Type_4;
	// CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_5>*	m_pTbl_Type_5;
	// CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_6>*	m_pTbl_Type_6;
	// CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_7>*	m_pTbl_Type_7;
	// CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_8>*	m_pTbl_Type_8;
	// CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_9>*	m_pTbl_Type_9;
	// CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_10>*	m_pTbl_Type_10;

	std::map<int, DWORD>	m_MySelf;

	int						m_iTarget;
	__Vector3				m_vTargetPos;

	// Regarding the handling of monsters that targeted me..
	std::map<int, DWORD>	m_MobList;	// int m_iMobID; DWORD m_dwMobMagicID;
	
	// related type4.....
	std::multimap<int, DWORD>	m_ListBuffTypeID;

	int						m_iBuffType;

	float					m_fAttackSpeed;
	float					m_fSpeed;

	int						m_iAC;
	int						m_iAttack;
	int						m_iMaxHP;
	int						m_iStr;
	int						m_iSta;
	int						m_iDex;
	int						m_iInt;
	int						m_iMAP; // Magic Attack Point

	int						m_iFireR;
	int						m_iColdR;
	int						m_iLightningR;
	int						m_iMagicR;
	int						m_iDeseaseR;
	int						m_iPoisonR;

	// combo...
	int						m_iComboSkillID;
	int						m_iCurrStep;
	int						m_iNumStep;
	float					m_fComboTime;
		
	// recast time...
	float					m_fRecastTime;
	float					m_fDelay;
		
	// related region magic...
	DWORD					m_dwRegionMagicState;	// 0:Finish..1:Selecting a region..2:Selecting a region.
	__TABLE_UPC_SKILL		m_dwRegionSkill;		// Selected area skill...

	// related non-casting action magic...
	DWORD					m_dwCastingStateNonAction;	// 0: A calm state with nothing 1: Casting
	float					m_fCastTimeNonAction;
	DWORD					m_dwNonActionMagicID;
	int						m_iNonActionMagicTarget;
	float					m_fRecastTimeNonAction;

	// local magic...
	int						m_iMyRegionTargetFXID;



protected:
	bool	CheckValidCondition(int iTargetID, __TABLE_UPC_SKILL* pSkill);
	bool	CheckValidDistance(__TABLE_UPC_SKILL* pSkill, __Vector3 vTargetPos, float fTargetRadius);
	
	void	InitType4();

	void	StartSkillMagicAtTargetPacket(__TABLE_UPC_SKILL* pSkill, short TargetID);
	void	StartSkillMagicAtPosPacket(__TABLE_UPC_SKILL* pSkill, __Vector3 vPos);
	
	void	ProcessCasting();
	void	ProcessCombo();

	void	MobCasting(__TABLE_UPC_SKILL* pSkill, int iSourceID);

	void	FlyingType2(__TABLE_UPC_SKILL* pSkill, int iSourceID, int iTargetID, short* pData);

	void	EffectingType4(DWORD dwMagicID);
	void	EffectingType3(DWORD dwMagicID);
	bool	EffectingType1(DWORD dwMagicID, int iSourceID, int iTargetID, short* pData);

	int		AddIdx(DWORD dwMagicID, int iNum=1);	// return value is index...
	void	RemoveIdx(int idx);

	void	SuccessCast(__TABLE_UPC_SKILL* pSkill, CPlayerBase* pTarget);
	void	FailCast(__TABLE_UPC_SKILL* pSkill);
	
public:
	bool	CheckValidSkillMagic(__TABLE_UPC_SKILL* pSkill);
	void	StunMySelf(__TABLE_UPC_SKILL_TYPE_3* pType3);
	void	StopCastingByRatio();
	void	ClearDurationalMagic();
	D3DCOLOR	TraceColorGet(__TABLE_UPC_SKILL* pSkill); // Depending on the type of skill, the color of the sword is determined.

	bool	IsPositiveMagic(DWORD dwMagicID);
	bool	IsCasting() const;
	DWORD	GetMagicID(int idx);
		
	bool	MsgSend_MagicProcess(int iTargetID, __TABLE_UPC_SKILL* pSkill);
	void	MsgRecv_Casting(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Flying(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Effecting(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_Fail(DataPack* pDataPack, int& iOffset);
	void	MsgRecv_BuffType(DataPack* pDataPack, int& iOffset);
	
	void	Init();
	void	Tick();	

	CMagicSkillMng();
	CMagicSkillMng(CGameProcMain* pGameProcMain);
	virtual ~CMagicSkillMng();
};
