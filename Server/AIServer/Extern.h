#ifndef _EXTERN_H_
#define _EXTERN_H_

// -------------------------------------------------
// global object variable
// -------------------------------------------------
extern BOOL	g_bNpcExit;

struct	_PARTY_GROUP
{
	WORD wIndex;
	short uid[8];		// Up to 8 people can join a party
	_PARTY_GROUP() {
		for(int i=0;i<8;i++)
			uid[i] = -1;
	};
};

struct _MAKE_WEAPON
{
	BYTE	byIndex;		// based on mob level
	short	sClass[MAX_UPGRADE_WEAPON];		// 1st weapon probability
	_MAKE_WEAPON() {
		for(int i=0;i<MAX_UPGRADE_WEAPON;i++)
			sClass[i] = 0;
	};
};

struct _MAKE_ITEM_GRADE_CODE
{
	BYTE	byItemIndex;		// item grade
	short	sGrade_1;			// step-by-step probability
	short	sGrade_2;
	short	sGrade_3;
	short	sGrade_4;
	short	sGrade_5;
	short	sGrade_6;
	short	sGrade_7;
	short	sGrade_8;
	short	sGrade_9;
};

struct _MAKE_ITEM_LARE_CODE
{
	BYTE	byItemLevel;			// item level judgment
	short	sLareItem;				// Probability of lareitem
	short	sMagicItem;				// Probability of getting a magicitem
	short	sGereralItem;			// Probability of geralitem
};

struct _MAGIC_TABLE
{
	int		iNum;
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

struct _MAGIC_TYPE2
{
	int     iNum;
	BYTE    bHitType;
	short   sHitRate;
	short	sAddDamage;
	short   sAddRange;
	BYTE    bNeedArrow;
};

struct _MAGIC_TYPE3
{
	int		iNum;
	//BYTE	bDistance;
	BYTE	bRadius;
	short	sAngle;
	short	sFirstDamage;
	short	sEndDamage;
	short	sTimeDamage;
	BYTE	bDirectType;
	short	sDuration;
	BYTE	bAttribute;
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

struct	_USERLOG
{
	CTime t;
	BYTE  byFlag;	// 
	BYTE  byLevel;
	char  strUserID[MAX_ID_SIZE+1];		// ID (character name)
};

#endif