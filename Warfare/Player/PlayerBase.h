#pragma once

#include "GameBase.h"
#include "GameDef.h"
#include "Bitset.h"
#include "N3Chr.h"
#include <deque>

// By : Ecli666 ( On 2002-07-22 9:59:19 AM )
//
#define SHADOW_SIZE 32			// Only multipliers of 2.
#define SHADOW_PLANE_SIZE 4.6f	
#define SHADOW_COLOR 0xa;		// Hexadecimal single digit.. alpha
// ~(By Ecli666 On Jul 22, 2002 9:59:19 am )

const float TIME_CORPSE_REMAIN = 90.0f;	// The time the body remains...
const float TIME_CORPSE_REMOVE = 10.0f; // Time to clear it up...

class CDFont;
class CN3SndObj;

class CPlayerBase : public CGameBase
{
	friend class CPlayerBase;
	friend class CPlayerOtherMgr;

protected:
	e_PlayerType				m_ePlayerType; // Player Type ... Base, NPC, OTher, MySelf
	
	std::deque<e_Ani>			m_AnimationDeque;		// Animation cue... If you insert it here, ticks will be rotated in turn.
	bool						m_bAnimationChanged;	// It is set only when the cued animation changes.

	CN3Chr						m_Chr;									// Character Basic Object...
	__TABLE_PLAYER_LOOKS*		m_pLooksRef;							// Basic reference table - resource information about the character, joint locations, sound files and more..
	__TABLE_ITEM_BASIC*			m_pItemPartBasics[PART_POS_COUNT];	// Weapons attached to the character.
	__TABLE_ITEM_EXT*			m_pItemPartExts[PART_POS_COUNT];		// Weapons attached to the character.
	__TABLE_ITEM_BASIC*			m_pItemPlugBasics[PLUG_POS_COUNT];	// Weapons attached to the character.
	__TABLE_ITEM_EXT*			m_pItemPlugExts[PLUG_POS_COUNT];		// Weapons attached to the character.

	// ID
	CDFont*				m_pClanFont;			// Font used for taking clan or knights.. names.. -.-;
	CDFont*				m_pIDFont;				// Font used to take ID.. -.-;
	CDFont*				m_pInfoFont;			// Recruiting party members, etc.. Displaying other information..
	CDFont*				m_pBalloonFont;			// Show balloons...
	float				m_fTimeBalloon;			// Bubble display time..

	e_StateAction		m_eState;				// state of action...
	e_StateAction		m_eStatePrev;			// The action state set immediately before..
	e_StateAction		m_eStateNext;			// The action state set immediately before..
	e_StateMove			m_eStateMove;			// moving state...
	e_StateDying		m_eStateDying;			// How do you die when you die..??
	float				m_fTimeDying;			// Time to get into a dying motion...

	// by tigger
	// By : Ecli666 ( On 2002-03-29 4:22:25 PM )
	//
	// CN3Texture* m_pTexShadow; // shadow texture...
	// __VertexXyzT1 m_vShadows[4]; // shadow polygon..

	// ~(By Ecli666 On Mar 29 2002 4:22:25 PM )
	
	D3DCOLORVALUE	m_cvDuration;			// persistent color value
	float			m_fDurationColorTimeCur;// current time..
	float			m_fDurationColorTime;	// duration..

	float 			m_fFlickeringFactor;	// flicker...
	float			m_fFlickeringTime;		// Blink time...

	float			m_fRotRadianPerSec;		// Rotation radians per second
	float			m_fMoveSpeedPerSec;		// Movement value per second.. This value is the default value and is added or subtracted depending on the state (walking, running, back, cursing, etc.).

	float			m_fYawCur;				// Current rotation value..
	float			m_fYawToReach;			// It rotates in Tick aiming at this rotation value.

	float			m_fYNext;				// Height value according to object or terrain collision check.
	float			m_fGravityCur;			// Gravity value...

	float			m_fScaleToSet;			// Gradually change the scale value..
	float			m_fScalePrev;

public:
	class CN3ShapeExtra*	m_pShapeExtraRef;			// If this NPC is in the form of an object such as a gate or a house, this pointer is set and used.

	int					m_iMagicAni;
	int					m_iIDTarget;			// Target ID...
	int					m_iDroppedItemID;		// Items dropped after death
	bool				m_bGuardSuccess;		// A flag for whether the defense was successful.
	bool				m_bVisible;				// can you see??

	__InfoPlayerBase	m_InfoBase;				// Character information...
	__Vector3			m_vPosFromServer;		// The current location recently received from the server.

	float				m_fTimeAfterDeath;		// Last time since death - 5 seconds is enough?? If you get hit before that, you die immediately.

	int					m_iSkillStep;			// Non-zero if the current skill is being used...
	float				m_fAttackDelta;			// Attack speed that changes by skill or magic.. 1.0 is the default, and the higher it is, the faster it attacks.
	float				m_fMoveDelta;			// Movement speed that changes by skill or magic is 1.0 by default, and the higher it is, the faster it moves.
	__Vector3			m_vDirDying;			// The direction you move when you die.

	// sound..
	bool			m_bSoundAllSet;
	CN3SndObj*		m_pSnd_Attack_0;
	// CN3SndObj* m_pSnd_Attack_1;
	CN3SndObj*		m_pSnd_Move;
	CN3SndObj*		m_pSnd_Struck_0;
	// CN3SndObj* m_pSnd_Struck_1;
	// CN3SndObj* m_pSnd_Dead_0;
	// CN3SndObj* m_pSnd_Dead_1;
	CN3SndObj*		m_pSnd_Breathe_0;
	// CN3SndObj* m_pSnd_Breathe_1;

	CN3SndObj*		m_pSnd_Blow;

	float			m_fCastFreezeTime;

	// function...
	// By : Ecli666 ( On 2002-03-29 1:32:12 PM )
	// 
	CBitset				m_bitset[SHADOW_SIZE];			// Used in Quake3.. ^^
	__VertexT1			m_pvVertex[4];
	unsigned short		m_pIndex[6];
	__VertexT1			m_vTVertex[4];
	float				m_fShaScale;
	CN3Texture			m_N3Tex; 
	static CN3SndObj*	m_pSnd_MyMove;
		
	const __Matrix44 CalcShadowMtxBasicPlane(__Vector3 vOffs);
	void			CalcPart(CN3CPart* pPart, int nLOD, __Vector3 vLP);
	void			CalcPlug(CN3CPlugBase* pPlug, const __Matrix44* pmtxJoint, __Matrix44& mtxMV, __Vector3 vLP);

protected:
	void			RenderShadow(float fSunAngle);
	// ~(By Ecli666 On Mar 29 2002 1:32:12 PM )

	virtual bool	ProcessAttack(CPlayerBase* pTarget); // Attack routine processing.. Get target pointer, check for collision, and return true if collided..

public:
	const __Matrix44*	JointMatrixGet(int nJointIndex) const { return m_Chr.MatrixGet( nJointIndex); }
	bool 				JointPosGet(int iJointIdx, __Vector3& vPos);
	
	e_PlayerType	PlayerType() { return m_ePlayerType; }
	e_Race			Race() { return m_InfoBase.eRace; }
	e_Nation		Nation() { return m_InfoBase.eNation; }
	virtual void	SetSoundAndInitFont();
	void			SetSoundPlug(__TABLE_ITEM_BASIC* pItemBasic);
	void			ReleaseSoundAndFont();
	void			RegenerateCollisionMesh(); // Find the maximum and minimum values again and recreate the collision mesh.
	e_StateAction	State() { return m_eState; } // action state...
	e_StateMove		StateMove() { return m_eStateMove; } // moving state

	e_ItemClass		ItemClass_RightHand()
	{
		if(m_pItemPlugBasics[PLUG_POS_RIGHTHAND]) return (e_ItemClass)(m_pItemPlugBasics[PLUG_POS_RIGHTHAND]->byClass); // Item Type - Right Hand
		else return ITEM_CLASS_UNKNOWN;
	}
	e_ItemClass		ItemClass_LeftHand()
	{
		if(m_pItemPlugBasics[PLUG_POS_LEFTHAND]) return (e_ItemClass)(m_pItemPlugBasics[PLUG_POS_LEFTHAND]->byClass); // Item Type - Right Hand
		else return ITEM_CLASS_UNKNOWN;
	}
	
	e_Ani	JudgeAnimationBreath(); // Determine the breathing motion.. Returns different animation indexes depending on the item you have and whether there is a target.
	e_Ani	JudgeAnimationWalk(); // Determining the walking mode.. Returns different animation indexes depending on the item you have and whether there is a target.
	e_Ani	JudgeAnimationRun(); // Determining the walking mode.. Returns different animation indexes depending on the item you have and whether there is a target.
	e_Ani	JudgeAnimationWalkBackward(); // Determining the walking mode.. Returns different animation indexes depending on the item you have and whether there is a target.
	e_Ani	JudgeAnimationAttack(); // Determining the attack motion.. Returns different animation indexes depending on the item you have.
	e_Ani	JudgeAnimationStruck(); // It just distinguishes between NPC and user and returns the animation index.
	e_Ani	JudgeAnimationGuard(); // Evaluate the blocking action. It just distinguishes between NPC and user and returns the animation index.
	e_Ani	JudgeAnimationDying(); // It just distinguishes between NPC and user and returns the animation index.
	e_Ani	JudgetAnimationSpellMagic(); // magic action

	bool			IsDead() { return ( PSA_DYING == m_eState || PSA_DEATH == m_eState ); } // is it dead
	bool			IsAlive() { return !IsDead(); } // Are you alive?
	bool			IsMovingNow() { if(PSM_WALK == m_eStateMove || PSM_RUN == m_eStateMove || PSM_WALK_BACKWARD == m_eStateMove) return true; return false; } // are you moving?

	void			AnimationAdd(e_Ani eAni, bool bImmediately);
	void			AnimationClear() { m_AnimationDeque.clear(); }
	int				AnimationCountRemain() { return m_AnimationDeque.size() + 1; }
	bool			IsAnimationChange() { return m_bAnimationChanged; }	// It is set only when the cued animation changes.

	bool			Action(e_StateAction eState, bool bLooping, CPlayerBase* pTarget = nullptr, bool bForceSet = false); // Take action according to the action table.
	bool			ActionMove(e_StateMove eMove); // moving...
	void			ActionDying(e_StateDying eSD, const __Vector3& vDir); // Deciding how to die...

	float			Yaw() { return m_fYawCur; } // rotation value.
	float			MoveSpeed() { return m_fMoveSpeedPerSec; }
	__Vector3		Position() const { return m_Chr.Pos(); }
	void			PositionSet(const __Vector3& vPos, bool bForcely) { m_Chr.PosSet(vPos); if(bForcely) m_fYNext = vPos.y; }
	float			Distance(const __Vector3& vPos) { return (m_Chr.Pos() - vPos).Magnitude(); }
	__Vector3		Scale() { return m_Chr.Scale(); }
	void			ScaleSet(float fScale) { m_fScaleToSet = m_fScalePrev = fScale; m_Chr.ScaleSet(fScale, fScale, fScale); }
	void			ScaleSetGradually(float fScale) { m_fScaleToSet = fScale; m_fScalePrev = m_Chr.Scale().y; } // Scale changes gradually.
	__Vector3		Direction() const;
	__Quaternion	Rotation() { return m_Chr.Rot(); }
	void			RotateTo(float fYaw, bool bImmediately);
	void			RotateTo(CPlayerBase* pOther); // looking at this
	float			Height();
	float			Radius();
	__Vector3		HeadPosition(); // Brings always changing head position.
	__Vector3		RootPosition() { if(!m_Chr.m_MtxJoints.empty()) return m_Chr.m_MtxJoints[0].Pos(); return __Vector3(0,0,0); }
	int				LODLevel() { return m_Chr.m_nLOD; }

	__Vector3		Max();
	__Vector3		Min();
	__Vector3		Center();

	void			DurationColorSet(const _D3DCOLORVALUE& color, float fDurationTime); // It returns to the original color while maintaining the color for the set time.
	void			FlickerFactorSet(float fAlpha);

	void			InfoStringSet(const std::string& szInfo, D3DCOLOR crFont);
	void			BalloonStringSet(const std::string& szBalloon, D3DCOLOR crFont);
	void			IDSet(int iID, const std::string& szID, D3DCOLOR crID);
	virtual void	KnightsInfoSet(int iID, const std::string& szName, int iGrade, int iRank);
	const std::string&	IDString() { return m_InfoBase.szID; } // ID is replaced by the name of the Character pointer.
	int				IDNumber() { return m_InfoBase.iID; }
	CPlayerBase*	TargetPointerCheck(bool bMustAlive);

	// 
	// Collision checking functions...
	bool				CheckCollisionByBox(const __Vector3& v0, const __Vector3& v1, __Vector3* pVCol, __Vector3* pVNormal);
	bool				CheckCollisionToTargetByPlug(CPlayerBase* pTarget, int nPlug, __Vector3* pVCol);

	virtual bool			InitChr(__TABLE_PLAYER_LOOKS* pTbl);
	virtual void			InitHair() {};
	virtual void			InitFace() {};
	CN3CPart*				Part(e_PartPosition ePos) { return m_Chr.Part(ePos); }
	CN3CPlugBase*			Plug(e_PlugPosition ePos) { return m_Chr.Plug(ePos); }
	virtual CN3CPart*		PartSet(e_PartPosition ePos, const std::string& szFN, __TABLE_ITEM_BASIC* pItemBasic, __TABLE_ITEM_EXT* pItemExt);
	virtual CN3CPlugBase*	PlugSet(e_PlugPosition ePos, const std::string& szFN, __TABLE_ITEM_BASIC* pItemBasic, __TABLE_ITEM_EXT* pItemExt);
	virtual void			DurabilitySet(e_ItemSlot eSlot, int iDurability);

	void TickYaw();				// Rotation value handling.
	void TickAnimation();		// animation processing.
	void TickDurationColor();	// Handling character color changes.
	void TickSound();			// Sound processing..

	virtual void		Tick();
	virtual void		Render(float fSunAngle);
	virtual void		RenderCollisionMesh() { m_Chr.RenderCollisionMesh(); }
	void				RenderChrInRect(CN3Chr* pChr, const RECT& Rect);	// Add Dino, draw a character in the specified rectangle.

	void				Release();

	CPlayerBase();
	virtual				~CPlayerBase();

	int					GetNPCOriginID() {	if (m_pLooksRef) return m_pLooksRef->dwID; else return -1;	}
};
