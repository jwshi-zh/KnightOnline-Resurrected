#pragma once

#include <list>
#include "PlayerBase.h"

// 
class CPlayerMySelf : public CPlayerBase
{
protected:
	bool			m_bRunning; // Are you running?
	float			m_fAttackTimeRecent;	// Last attack time.

	__Vector3		m_vCollisionOffsets[3]; // 2 collision checks for the waist + 1 collision check for the head..

public:	
	__InfoPlayerMySelf	m_InfoExt;
	CN3Chr				m_ChrInv;				// Character Basic Object...
	bool				m_bMoveContinous;		// Do you keep moving?
	bool				m_bAttackContinous;		// Are you still under attack?
	bool				m_bSitDown;				// Are you sitting...
	bool				m_bRecruitParty;		// Looking for a party??

	bool				m_bStun;				// faint..
	float				m_fStunTime;			// embarrassing time...

	int					m_iSendRegeneration;	// If it is sent once, it will not be seen until it dies again. When the flag is 0, a message box is displayed.
	bool				m_bTempMoveTurbo;		// Temporary code put in for development.. Makes it run really fast.. See Tick.

	DWORD				m_dwMagicID;			// The magic you&#39;re using...
	float				m_fCastingTime;			// Time to cast a spell...
	
	class CN3Shape*		m_pObjectTarget;		// target object pointer.

	bool				m_bTargetOrPosMove;		// whether to move target or point
	int					m_iMoveTarget;			// target id
	__Vector3			m_vTargetPos;			// point to move to
	void				SetMoveTargetID(int iID);
	void				SetMoveTargetPos(__Vector3 vPos);

public:	
	void			TargetOrPosMove();
	void			Stun(float fTime); // To stun for a certain amount of time.
	void			StunRelease(); // Get rid of fainting..

	float			MoveSpeedCalculationAndCheckCollision(); // Get the speed and do a collision check with that speed. If the return value is 0, it is a collision.

	void			InitFace(); // Initialize the face with the information you have.
	void			InitHair(); // Initialize the hair with the information you have.
	void			KnightsInfoSet(int iID, const std::string& szName, int iGrade, int iRank);
	void			SetSoundAndInitFont();

	float			AttackableDistance(CPlayerBase* pTarget);
	float			DistanceExceptRadius(CPlayerBase* pTarget);
	bool			IsAttackableTarget(CPlayerBase* pTarget, bool bMesureAngle = true); // Are you within attack range?
	bool			IsRunning() const { return m_bRunning; }
	bool			CheckCollision();		// It does motion processing and collision checking. If something collides with it, it doesn&#39;t move.
	// ..
	bool			InitChr(__TABLE_PLAYER_LOOKS* pTblUPC);
	CN3CPart*		PartSet(e_PartPosition ePos, const std::string& szFN, __TABLE_ITEM_BASIC* pItemBasic, __TABLE_ITEM_EXT* pItemExt);
	CN3CPlugBase*	PlugSet(e_PlugPosition ePos, const std::string& szFN, __TABLE_ITEM_BASIC* pItemBasic, __TABLE_ITEM_EXT* pItemExt);

	void			Tick();
	void			Render(float fSunAngle);

	__Vector3		NextPos(float fTimeAfter) const;
	void			RotAdd(const float fRotRadianPerSec);			// Enter the rotation speed per second around the y-axis.
	
	bool			ToggleAttackContinous();
	bool			ToggleRunMode();
	void			ToggleMoveMode();

	// Inventory..
	void			InventoryChrRender(const RECT& Rect);
	void			ChrCreateChrRender(const RECT& Rect);
	void			InventoryChrTick();
	void			InventoryChrAnimationInitialize();


	void			Release();
	CPlayerMySelf();
	virtual	~CPlayerMySelf();
};
