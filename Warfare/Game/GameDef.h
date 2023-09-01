#pragma once

#include <string>
#include "DInput.h"

const int CURRENT_VERSION = 1068; // current version

const float PACKET_INTERVAL_MOVE = 1.5f;				// Periodic packet time interval
const float PACKET_INTERVAL_ROTATE = 4.0f;
const float PACKET_INTERVAL_REQUEST_TARGET_HP = 2.0f;

// Part of the keyboard shortcut
enum eKeyMap {	KM_HOTKEY1 = DIK_1, 
				KM_HOTKEY2 = DIK_2, 
				KM_HOTKEY3 = DIK_3, 
				KM_HOTKEY4 = DIK_4, 
				KM_HOTKEY5 = DIK_5, 
				KM_HOTKEY6 = DIK_6, 
				KM_HOTKEY7 = DIK_7, 
				KM_HOTKEY8 = DIK_8, 
				KM_TOGGLE_RUN = DIK_T, 
				KM_TOGGLE_MOVE_CONTINOUS = DIK_E, 
				KM_TOGGLE_ATTACK = DIK_R, 
				KM_TOGGLE_SITDOWN = DIK_C, 
				KM_TOGGLE_INVENTORY = DIK_I, 
				KM_TOGGLE_SKILL = DIK_K, 
				KM_TOGGLE_STATE = DIK_U, 
				KM_TOGGLE_MINIMAP = DIK_M, 
				KM_TOGGLE_HELP = DIK_F1, 
				KM_CAMERA_CHANGE = DIK_F9, 
				KM_DROPPED_ITEM_OPEN = DIK_F, 
				KM_MOVE_FOWARD = DIK_W, 
				KM_MOVE_BACKWARD = DIK_S, 
				KM_ROTATE_LEFT = DIK_A, 
				KM_ROTATE_RIGHT = DIK_D, 
				KM_TARGET_NEARST_ENEMY = DIK_Z, 
				KM_TARGET_NEARST_PARTY = DIK_X, 
				KM_TARGET_NEARST_FRIEND = DIK_V, 
				KM_TARGET_PARTY1 = DIK_F1, 
				KM_TARGET_PARTY2 = DIK_F2, 
				KM_TARGET_PARTY3 = DIK_F3, 
				KM_TARGET_PARTY4 = DIK_F4, 
				KM_TARGET_PARTY5 = DIK_F5, 
				KM_TARGET_PARTY6 = DIK_F6, 
				KM_TARGET_PARTY7 = DIK_F7, 
				KM_TARGET_PARTY8 = DIK_F8 };

enum e_PlayerType { PLAYER_BASE = 0, PLAYER_NPC = 1, PLAYER_OTHER = 2, PLAYER_MYSELF = 3 };

enum e_Race {	RACE_ALL = 0,
				RACE_KA_ARKTUAREK = 1, RACE_KA_TUAREK = 2, RACE_KA_WRINKLETUAREK = 3, RACE_KA_PURITUAREK = 4, 
				RACE_EL_BABARIAN = 11, RACE_EL_MAN = 12, RACE_EL_WOMEN = 13,
				//RACE_KA_NORMAL = 11, RACE_KA_WARRIOR = 12, RACE_KA_ROGUE = 13, RACE_KA_MAGE = 14,
				RACE_NPC = 100,
				RACE_UNKNOWN = 0xffffffff };

enum e_Class {	CLASS_KINDOF_WARRIOR = 1, CLASS_KINDOF_ROGUE, CLASS_KINDOF_WIZARD, CLASS_KINDOF_PRIEST,
				CLASS_KINDOF_ATTACK_WARRIOR, CLASS_KINDOF_DEFEND_WARRIOR, CLASS_KINDOF_ARCHER, CLASS_KINDOF_ASSASSIN, 
				CLASS_KINDOF_ATTACK_WIZARD, CLASS_KINDOF_PET_WIZARD, CLASS_KINDOF_HEAL_PRIEST, CLASS_KINDOF_CURSE_PRIEST,

				CLASS_KA_WARRIOR = 101, CLASS_KA_ROGUE, CLASS_KA_WIZARD, CLASS_KA_PRIEST, // Basic job so far
				CLASS_KA_BERSERKER = 105, CLASS_KA_GUARDIAN, CLASS_KA_HUNTER = 107, CLASS_KA_PENETRATOR, 
				CLASS_KA_SORCERER = 109, CLASS_KA_NECROMANCER, CLASS_KA_SHAMAN = 111, CLASS_KA_DARKPRIEST, 
				
				CLASS_EL_WARRIOR = 201, CLASS_EL_ROGUE, CLASS_EL_WIZARD, CLASS_EL_PRIEST, // Basic job so far 
				CLASS_EL_BLADE = 205, CLASS_EL_PROTECTOR, CLASS_EL_RANGER = 207, CLASS_EL_ASSASIN, 
				CLASS_EL_MAGE = 209, CLASS_EL_ENCHANTER, CLASS_EL_CLERIC = 211, CLASS_EL_DRUID,
				
				CLASS_UNKNOWN = 0xffffffff };

enum e_Class_Represent { CLASS_REPRESENT_WARRIOR = 0, CLASS_REPRESENT_ROGUE, CLASS_REPRESENT_WIZARD, CLASS_REPRESENT_PRIEST, CLASS_REPRESENT_UNKNOWN = 0xffffffff };

const float WEAPON_WEIGHT_STAND_SWORD = 5.0f; // Weight of the weapon Sword
const float WEAPON_WEIGHT_STAND_AXE = 5.0f; // Weight of the weapon Ax
const float WEAPON_WEIGHT_STAND_BLUNT = 8.0f; // Weight of the weapon Club type

enum e_Ani {	ANI_BREATH = 0, ANI_WALK, ANI_RUN, ANI_WALK_BACKWARD, ANI_STRUCK0, ANI_STRUCK1, ANI_STRUCK2, ANI_GUARD,
				ANI_DEAD_NEATLY = 8, ANI_DEAD_KNOCKDOWN, ANI_DEAD_ROLL, ANI_SITDOWN, ANI_SITDOWN_BREATH, ANI_STANDUP,
				ANI_ATTACK_WITH_WEAPON_WHEN_MOVE = 14, ANI_ATTACK_WITH_NAKED_WHEN_MOVE, 

				ANI_SPELLMAGIC0_A = 16, ANI_SPELLMAGIC0_B, 
				ANI_SPELLMAGIC1_A = 18, ANI_SPELLMAGIC1_B, 
				ANI_SPELLMAGIC2_A = 20, ANI_SPELLMAGIC2_B, 
				ANI_SPELLMAGIC3_A = 22, ANI_SPELLMAGIC3_B, 
				ANI_SPELLMAGIC4_A = 24, ANI_SPELLMAGIC4_B, 
				
				ANI_SHOOT_ARROW_A = 26, ANI_SHOOT_ARROW_B, 
				ANI_SHOOT_QUARREL_A = 28, ANI_SHOOT_QUARREL_B, 
				ANI_SHOOT_JAVELIN_A = 30, ANI_SHOOT_JAVELIN_B, 
				
				ANI_SWORD_BREATH_A = 32,	ANI_SWORD_ATTACK_A0, ANI_SWORD_ATTACK_A1,
				ANI_SWORD_BREATH_B,			ANI_SWORD_ATTACK_B0, ANI_SWORD_ATTACK_B1,		// One handed sword
				
				ANI_DAGGER_BREATH_A = 38,	ANI_DAGGER_ATTACK_A0, ANI_DAGGER_ATTACK_A1,
				ANI_DAGGER_BREATH_B,		ANI_DAGGER_ATTACK_B0, ANI_DAGGER_ATTACK_B1,		// Dagger
				
				ANI_DUAL_BREATH_A = 44,		ANI_DUAL_ATTACK_A0, ANI_DUAL_ATTACK_A1, 
				ANI_DUAL_BREATH_B,			ANI_DUAL_ATTACK_B0, ANI_DUAL_ATTACK_B1,			// Dual wield
				
				ANI_SWORD2H_BREATH_A = 50,	ANI_SWORD2H_ATTACK_A0, ANI_SWORD2H_ATTACK_A1, 
				ANI_SWORD2H_BREATH_B,		ANI_SWORD2H_ATTACK_B0, ANI_SWORD2H_ATTACK_B1,	// Two-handed sword
				
				ANI_BLUNT_BREATH_A = 56,	ANI_BLUNT_ATTACK_A0, ANI_BLUNT_ATTACK_A1, 
				ANI_BLUNT_BREATH_B,			ANI_BLUNT_ATTACK_B0, ANI_BLUNT_ATTACK_B1,		// Blunt - club ?
				
				ANI_BLUNT2H_BREATH_A = 62,	ANI_BLUNT2H_ATTACK_A0, ANI_BLUNT2H_ATTACK_A1, 
				ANI_BLUNT2H_BREATH_B,		ANI_BLUNT2H_ATTACK_B0, ANI_BLUNT2H_ATTACK_B1,	// A blunt weapon held with both hands - It is like a two-handed axe (club)
				
				ANI_AXE_BREATH_A = 68,		ANI_AXE_ATTACK_A0, ANI_AXE_ATTACK_A1, 
				ANI_AXE_BREATH_B,			ANI_AXE_ATTACK_B0, ANI_AXE_ATTACK_B1,			// One-handed ax
				
				ANI_SPEAR_BREATH_A = 74,	ANI_SPEAR_ATTACK_A0, ANI_SPEAR_ATTACK_A1, 
				ANI_SPEAR_BREATH_B,			ANI_SPEAR_ATTACK_B0, ANI_SPEAR_ATTACK_B1,		// Spear - It is just a spear without a blade.
				
				ANI_POLEARM_BREATH_A = 80,	ANI_POLEARM_ATTACK_A0, ANI_POLEARM_ATTACK_A1, 
				ANI_POLEARM_BREATH_B,		ANI_POLEARM_ATTACK_B0, ANI_POLEARM_ATTACK_B1,	// A spear with a blade held with both hands - Cheongryongdo/Blue Dragon Island ??
				
				ANI_NAKED_BREATH_A = 86,	ANI_NAKED_ATTACK_A0, ANI_NAKED_ATTACK_A1, 
				ANI_NAKED_BREATH_B,			ANI_NAKED_ATTACK_B0, ANI_NAKED_ATTACK_B1,		// Naked bare body
				
				ANI_BOW_BREATH = 92,		ANI_CROSS_BOW_BREATH, ANI_LAUNCHER_BREATH, 
				ANI_BOW_BREATH_B,			ANI_BOW_ATTACK_B0, ANI_BOW_ATTACK_B1,			// Bow attack
				
				ANI_SHIELD_BREATH_A = 98,	ANI_SHIELD_ATTACK_A0, ANI_SHIELD_ATTACK_A1, 
				ANI_SHIELD_BREATH_B,		ANI_SHIELD_ATTACK_B0, ANI_SHIELD_ATTACK_B1,		// Shield attack

				ANI_GREETING0 = 104, ANI_GREETING1, ANI_GREETING2, 
				ANI_WAR_CRY0 = 107, ANI_WAR_CRY1, ANI_WAR_CRY2, ANI_WAR_CRY3, ANI_WAR_CRY4, 

				ANI_SKILL_AXE0 = 112, ANI_SKILL_AXE1, ANI_SKILL_AXE2, ANI_SKILL_AXE3, 
				ANI_SKILL_DAGGER0 = 116, ANI_SKILL_DAGGER1,
				ANI_SKILL_DUAL0 = 118, ANI_SKILL_DUAL1,
				ANI_SKILL_BLUNT0 = 120, ANI_SKILL_BLUNT1, ANI_SKILL_BLUNT2, ANI_SKILL_BLUNT3, 
				ANI_SKILL_POLEARM0 = 124, ANI_SKILL_POLEARM1,
				ANI_SKILL_SPEAR0 = 126, ANI_SKILL_SPEAR1,
				ANI_SKILL_SWORD0 = 128, ANI_SKILL_SWORD1, ANI_SKILL_SWORD2, ANI_SKILL_SWORD3, 
				ANI_SKILL_AXE2H0 = 132, ANI_SKILL_AXE2H1,
				ANI_SKILL_SWORD2H0 = 134, ANI_SKILL_SWORD2H1,

				// From here on, NPC Animation
				ANI_NPC_BREATH = 0, ANI_NPC_WALK, ANI_NPC_RUN, ANI_NPC_WALK_BACKWARD,
				ANI_NPC_ATTACK0 = 4, ANI_NPC_ATTACK1, ANI_NPC_STRUCK0, ANI_NPC_STRUCK1, ANI_NPC_STRUCK2, ANI_NPC_GUARD, 
				ANI_NPC_DEAD0 = 10, ANI_NPC_DEAD1, ANI_NPC_TALK0, ANI_NPC_TALK1, ANI_NPC_TALK2, ANI_NPC_TALK3, 
				ANI_NPC_SPELLMAGIC0 = 16, ANI_NPC_SPELLMAGIC1, 

				ANI_UNKNOWN = 0xffffffff };


//MAX_INCLINE_CLIMB = sqrt( 1 - sin(90-maximum angle of inclination)^2 )
//const    float MAX_INCLINE_CLIMB = 0.5f;		// maximum slope that can be climbed = 30 Degree
const float MAX_INCLINE_CLIMB = 0.6430f;		// maximum slope that can be climbed = 40 Degree
//const    float MAX_INCLINE_CLIMB = 0.7071f;	// maximum slope that can be climbed = 45 Degree
//const    float MAX_INCLINE_CLIMB = 0.7660f;	// maximum slope that can be climbed = 50 Degree
//const    float MAX_INCLINE_CLIMB = 0.8660f;	// maximum slope that can be climbed = 60 Degree

enum e_MoveDirection { MD_STOP, MD_FOWARD, MD_BACKWARD, MD_UNKNOWN = 0xffffffff };

const float MOVE_DELTA_WHEN_RUNNING = 3.0f; // Variable multiplied when running
const float MOVE_SPEED_WHEN_WALK = 1.5f;    // Standard speed when players walk

// Current State-Status
enum e_StateMove {	PSM_STOP = 0,
					PSM_WALK,
					PSM_RUN,
					PSM_WALK_BACKWARD,
					PSM_COUNT };

enum e_StateAction {	PSA_BASIC = 0, 		// Doing nothing..
						PSA_ATTACK, 		// Attack status..
						PSA_GUARD, 			// successful defense - block..
						PSA_STRUCK, 		// Get hit.
						PSA_DYING, 			// Dying (falling down)
						PSA_DEATH, 			// dead and stretched..
						PSA_SPELLMAGIC,		// Casting magic spell...
						PSA_SITDOWN, 		// Sitting...
						PSA_COUNT};

enum e_StateDying {		PSD_DISJOINT = 0,	// Disintegrate-Disassemble-Decompose and die
						PSD_KNOCK_DOWN,		// Die by being blown away or pushed back
						PSD_KEEP_POSITION,	// Die on the spot
						PSD_COUNT,

						PSD_UNKNOWN = 0xffffffff };

enum e_StateParty {	PSP_NORMAL = 0,
					PSP_POISONING = 1,
					PSP_CURSED = 2,
					PSP_MAGIC_TAKEN = 4,
					PSP_BLESSED = 8,
					PSP_UNKNOWN = 0xffffffff };

enum e_PartPosition	{	PART_POS_UPPER = 0,
						PART_POS_LOWER,
						PART_POS_FACE,
						PART_POS_HANDS,
						PART_POS_FEET, 
						PART_POS_HAIR_HELMET,
						PART_POS_COUNT,
						PART_POS_UNKNOWN = 0xffffffff };

enum e_PlugPosition {	PLUG_POS_RIGHTHAND = 0,
						PLUG_POS_LEFTHAND, 
						PLUG_POS_BACK, 
						PLUG_POS_KNIGHTS_GRADE, 
						PLUG_POS_COUNT,
						PLUG_POS_UNKNOWN = 0xffffffff };

enum e_ItemAttrib	{
						ITEM_ATTRIB_GENERAL = 0,
						ITEM_ATTRIB_MAGIC	= 1,
						ITEM_ATTRIB_LAIR	= 2,
						ITEM_ATTRIB_CRAFT	= 3,
						ITEM_ATTRIB_UNIQUE	= 4,
						ITEM_ATTRIB_UPGRADE	= 5,		
						ITEM_ATTRIB_UNKNOWN = 0xffffffff };	

enum e_ItemClass	{	ITEM_CLASS_DAGGER = 11, // Dagger(dagger)
						ITEM_CLASS_SWORD = 21, // One-handedsword(onehandsword)
						ITEM_CLASS_SWORD_2H = 22, // 3 : Two-handedsword(twohandsword)
						ITEM_CLASS_AXE = 31, // One-handedax(onehandaxe)
						ITEM_CLASS_AXE_2H = 32, // Two-handedax(twohandaxe)
						ITEM_CLASS_MACE = 41, // One-handed mace(mace)
						ITEM_CLASS_MACE_2H = 42, // Two-handed mace(twohandmace)
						ITEM_CLASS_SPEAR = 51, // Spear(spear)
						ITEM_CLASS_POLEARM = 52, // Polearm(polearm)
						
						ITEM_CLASS_SHIELD = 60, // Shield(shield)

						ITEM_CLASS_BOW = 70, //  ShortBow(Shortbow)
						ITEM_CLASS_BOW_CROSS = 71, // Crossbow(crossbow)
						ITEM_CLASS_BOW_LONG = 80, // Longbow(longbow)

						ITEM_CLASS_EARRING = 91, // Earring
						ITEM_CLASS_AMULET = 92, // Necklace
						ITEM_CLASS_RING = 93, // Ring jewelry
						ITEM_CLASS_BELT = 94, // Waist Belt
						ITEM_CLASS_CHARM = 95, // Items you have in your inventory
						ITEM_CLASS_JEWEL = 96, // Type of jewelry
						ITEM_CLASS_POTION = 97, // Liquid medicine-Potion
						ITEM_CLASS_SCROLL = 98, // Scroll

						ITEM_CLASS_LAUNCHER = 100, // Items used when throwing spears
						
						ITEM_CLASS_STAFF = 110, // Staff-Wand(staff)
						ITEM_CLASS_ARROW = 120, // Arrow(Arrow)
						ITEM_CLASS_JAVELIN = 130, // Javelin
						
						ITEM_CLASS_ARMOR_WARRIOR = 210, // Warrior armor
						ITEM_CLASS_ARMOR_ROGUE = 220, // Rogue armor
						ITEM_CLASS_ARMOR_MAGE = 230, // Wizard armor
						ITEM_CLASS_ARMOR_PRIEST = 240, // Priest armor

						ITEM_CLASS_ETC = 251, // etc

						ITEM_CLASS_UNKNOWN = 0xffffffff }; // 

enum e_Nation { NATION_NOTSELECTED = 0, NATION_KARUS, NATION_ELMORAD, NATION_UNKNOWN = 0xffffffff };

struct __TABLE_ITEM_BASIC;
struct __TABLE_ITEM_EXT;
struct __TABLE_PLAYER;

enum e_Authority { AUTHORITY_MANAGER = 0, AUTHORITY_USER, AUTHORITY_NOCHAT, AUTHORITY_NPC, AUTHORITY_BLOCK_USER = 0xff };

struct __InfoPlayerBase
{
	int			iID;			// Unique ID
	std::string szID;			// Name
	D3DCOLOR	crID;			// Name color
	e_Race		eRace;			// Race based on character skeleton
	e_Nation	eNation;		// Nation
	e_Class		eClass;			// Job-Profession
	int			iLevel;			// Level
	int			iHPMax;	
	int			iHP; 
	int			iAuthority;		// Authority - 0 administrator, 1 - general user, 255 - blocked user

	bool		bRenderID;		// Show ID on the screen

	void Init()
	{
		iID = 0;					// Unique ID
		szID = "";					// Name
		crID = 0;					// Name color
		eRace = RACE_UNKNOWN;		// Race based on character skeleton
		eNation = NATION_UNKNOWN;	// Nation
		eClass = CLASS_UNKNOWN;		// Job-Profession
		iLevel = 0;					// Level
		iHPMax = 0;	
		iHP = 0; 
		iAuthority = 1;				// Authority - 0 administrator, 1 - general user, 255 - blocked user...
		bRenderID = true;
	}
};

struct __InfoPlayerOther
{
	int			iFace;			// Face shape
	int			iHair;			// Hair shape

	int			iCity;			// Affiliated city
	int			iKnightsID;		// Affiliated Knights (Guild) ID
	std::string szKnights;		// Affiliated Knights (Guild) Name
	int			iKnightsGrade;	// Affiliated Knights (Guild) Rank
	int			iKnightsRank;	// Affiliated Order of Knights (Guild) Ranking

	int			iRank;			// Title - count, duke - classification according to authority
	int			iTitle;			// Position - lord, lord - simple status -> guild leader

	void Init()
	{
		iFace = 0;			// Face shape
		iHair = 0;			// Hair shape
		iCity;				// Affiliated city
		iKnightsID = 0;		// Affiliated Knights (Guild) ID
		szKnights = "";		// Affiliated Knights (Guild) Name
		iKnightsGrade = 0;	// Affiliated Knights (Guild) Rank
		iKnightsRank = 0;	// Affiliated Order of Knights (Guild) Ranking
		iTitle = 0;			// Position - lord, lord - simple status -> guild leader
	}
};

// Templar position
enum e_KnightsDuty {	KNIGHTS_DUTY_UNKNOWN = 0,		// ????? kicked out??
						KNIGHTS_DUTY_CHIEF = 1,			// Chief of Staff
						KNIGHTS_DUTY_VICECHIEF = 2,		// Vice Captain
						KNIGHTS_DUTY_PUNISH = 3,		// Being disciplined
						KNIGHTS_DUTY_TRAINEE = 4,		// Knight Apprentice
						KNIGHTS_DUTY_KNIGHT = 5,		// General Knight
						KNIGHTS_DUTY_OFFICER = 6		// Officer
					};

#define VICTORY_ABSENCE		0
#define VICTORY_KARUS		1
#define VICTORY_ELMORAD		2

struct __InfoPlayerMySelf : public __InfoPlayerOther
{
	int					iBonusPointRemain;	// Remaining bonus points
	int					iLevelPrev; 		// Previous level...

	int					iMSPMax; 
	int					iMSP; 
			
	int					iTargetHPPercent;
	int					iGold;
	int					iExpNext;
	int					iExp; 
	int					iRealmPoint;		// National contribution
	e_KnightsDuty		eKnightsDuty;		// Templar Rights
	int					iWeightMax;			// Total weight of the item
	int					iWeight;			// Maximum weight you can carry-lift
	int					iStrength;			// Strength
	int					iStrength_Delta;	// A value added or subtracted by an item or magic
	int					iStamina;			// Health Points
	int					iStamina_Delta;		// A value added or subtracted by an item or magic
	int					iDexterity;			// Dex
	int					iDexterity_Delta;	// A value added or subtracted by an item or magic
	int					iIntelligence;		// INT
	int					iIntelligence_Delta; // A value added or subtracted by an item or magic
	int 				iMagicAttak;		// MP Magical Attack (charm)
	int 				iMagicAttak_Delta;	// A value added or subtracted by an item or magic
	
	int 				iAttack;		// Attack power
	int 				iAttack_Delta;	// Value added or subtracted by magic
	int 				iGuard;			// Defense
	int 				iGuard_Delta;	// Value added or subtracted by magic

	int 				iRegistFire;			// Fire Resistance
	int 				iRegistFire_Delta;		// Value added or subtracted by magic
	int 				iRegistCold;			// Ice Resistance
	int 				iRegistCold_Delta;		// Value added or subtracted by magic
	int 				iRegistLight;			// Lightning Resistance
	int 				iRegistLight_Delta;		// Value added or subtracted by magic
	int 				iRegistMagic;			// Magic Resistance
	int 				iRegistMagic_Delta;		// Value added or subtracted by magic
	int 				iRegistCurse;			// Curse Resistance
	int 				iRegistCurse_Delta;		// Value added or subtracted by magic
	int 				iRegistPoison;			// Poison Resistance
	int 				iRegistPoison_Delta;	// Value added or subtracted by magic

	int					iZoneInit;				// The first zone number received from the server
	int					iZoneCur;				// Current zone
	int					iVictoryNation;			// 0: Draw 1: Elmorad wins 2: Karus wins

	void Init()
	{
		__InfoPlayerOther::Init();

		iBonusPointRemain = 0; // Remaining bonus points
		iLevelPrev = 0; // Previous level

		iMSPMax = 0; 
		iMSP = 0; 
		
		iTargetHPPercent = 0;
		iGold = 0;
		iExpNext = 0;
		iExp = 0; 
		iRealmPoint = 0;		// National contribution
		eKnightsDuty = KNIGHTS_DUTY_UNKNOWN;		// Templar Rights
		iWeightMax = 0;			// Total weight of the item
		iWeight = 0;			// Maximum weight you can carry-lift
		iStrength = 0;			// Strength
		iStrength_Delta = 0;	// Value added or subtracted by magic
		iStamina = 0;			// Health Points
		iStamina_Delta = 0;		// Value added or subtracted by magic
		iDexterity = 0;			// Dex
		iDexterity_Delta = 0;	// Value added or subtracted by magic
		iIntelligence = 0;		// Int
		iIntelligence_Delta = 0; // Value added or subtracted by magic
		iMagicAttak = 0;		// MP Magical Attack (charm)
		iMagicAttak_Delta = 0;	// Value added or subtracted by magic
		
		iAttack = 0;		// Attack power
		iAttack_Delta = 0;	// Value added or subtracted by magic
		iGuard = 0;			// Defense power
		iGuard_Delta = 0;	// Value added or subtracted by magic

		iRegistFire = 0;			// Fire resistance
		iRegistFire_Delta = 0;		// Value added or subtracted by magic
		iRegistCold = 0;			// Ice resistance
		iRegistCold_Delta = 0;		// Value added or subtracted by magic
		iRegistLight = 0;			// Light resistance
		iRegistLight_Delta = 0;		// Value added or subtracted by magic
		iRegistMagic = 0;			// Magic resistance
		iRegistMagic_Delta = 0;		// Value added or subtracted by magic
		iRegistCurse = 0;			// Curse resistance
		iRegistCurse_Delta = 0;		// Value added or subtracted by magic
		iRegistPoison = 0;			// Poison resistance
		iRegistPoison_Delta = 0;	// Value added or subtracted by magic

		iZoneInit = 0x01;			// The first zone number received from the server
		iZoneCur = 0;				// Current Zone
		iVictoryNation = -1;		// 0: Draw 1: Elmorad wins 2: Karus wins
	}
};

const int MAX_PARTY_OR_FORCE = 8;

struct __InfoPartyOrForce
{
    int         iID;             // party member ID
    int         iLevel;          // Level
    e_Class     eClass;          // Job
    int         iHP;             // Hit Point
    int         iHPMax;          // Hit Point Max
    bool        bSufferDown_HP;  // Status - Low HP - Falling - Fall
    bool        bSufferDown_Etc; // Status - When affected by curses
    std::string szID;            // Party name string

	void Init()
	{
		iID = -1;
		iLevel = 0;
		eClass = CLASS_UNKNOWN;
		iHP = 0;
		iHPMax = 0;
		szID = "";

		bSufferDown_HP = false;			// Status - Low HP - Falling - Fall
		bSufferDown_Etc = false;		// Status - When affected by curses
	};

	__InfoPartyOrForce()
	{
		this->Init();
	}
};

enum e_PartyStatus { PARTY_STATUS_DOWN_HP = 1, PARTY_STATUS_DOWN_ETC = 2 };

struct __InfoPartyBBS // Party support bulletin board structure
{
    std::string szID;   // Party name string
    int         iID;    // Party member ID
    int         iLevel; // Level
    e_Class     eClass; // Job
    int         iMemberCount;

	void Init()
	{
		szID = "";
		iID = -1;
		iLevel = 0;
		eClass = CLASS_UNKNOWN;
		iMemberCount = 0;
	};

	__InfoPartyBBS()
	{
		this->Init();
	}
};

enum {	RESRC_UI_LOGIN = 0, RESRC_UI_CHARACTERSELECT, RESRC_UI_CHARACTERCREATE, RESRC_UI_INVENTORY, RESRC_UI_CHAT,
		RESRC_UI_MYINFORMATION, RESRC_UI_TARGET, RESRC_UI_HOTKEY, RESRC_UI_FUNCTION };

typedef struct __TABLE_ZONE
{
	DWORD			dwID; 								// zone ID
	std::string		szTerrainFN; 						// Terrain FileName - Terrain, Tile Map, ColorMap
	std::string 	szZoneName;
	std::string		szColorMapFN;
	std::string		szLightMapFN;
	std::string		szObjectPostDataFN; 				// Object location information - Collision check data is also included in this
	std::string		szObjectPostDataExtFN; 				// Todo implement
	std::string		szMiniMapFN;
	std::string 	szSkySetting; 						// A setting information file that adjusts the sky and atmosphere
	uint32_t		bIndicateEnemyPlayer;				// Do you show enemy players?
	uint32_t		iFixedSundDirection;				// Fixed direction of the sun
	std::string 	szLightObjFN;						// Light information file placed on the terrain

	std::string 	szGevFN;         					// TODO: implement
	uint32_t    	iAbyssSomething; 					// TODO: implement
	std::string 	szEnsFN;         					// TODO: implement
	float			fFOV;            					// TODO: implement
	std::string 	szFlagFN;        					// TODO: implement

	uint32_t 		iIdk1;
	uint32_t 		iIdk2;
	uint32_t 		iIdk3;
	uint32_t 		iIdk4;

	std::string		szOpdSubFN;
	uint32_t 		iIdk5;
	std::string		szEvtSubFN;
} TABLE_ZONE;

typedef struct __TABLE_UI_RESRC
{
	uint32_t	dwID;					// 0 (Karus/Human)
	std::string szLogIn;				// 1
	std::string szCmd;					// 2
	std::string szChat;					// 3
	std::string szMsgOutput;			// 4
	std::string szStateBar;				// 5
	std::string szVarious;				// 6 
	std::string szState;				// 7 
	std::string szKnights;				// 8 
	std::string szQuest;				// 9 
	std::string szFriends;				// 10 
	std::string szInventory;			// 11
	std::string szTransaction;			// 12
	std::string szDroppedItem;			// 13
	std::string szTargetBar;			// 14
	std::string szTargetSymbolShape;	// 15
	std::string szSkillTree;			// 16
	std::string szHotKey;				// 17
	std::string szMiniMap;				// 18
	std::string szPartyOrForce;			// 19
	std::string szPartyBBS;				// 20
	std::string szHelp;					// 21
	std::string szNotice;				// 22
	std::string szCharacterCreate;		// 23
	std::string szCharacterSelect;		// 24
	std::string szToolTip;				// 25
	std::string szMessageBox;			// 26
	std::string szLoading;				// 27
	std::string szItemInfo;				// 28
	std::string szPersonalTrade;		// 29
	std::string szPersonalTradeEdit;	// 30
	std::string szNpcEvent;				// 31
	std::string szZoneChangeOrWarp;		// 32
	std::string szExchangeRepair;		// 33
	std::string szRepairTooltip;		// 34
	std::string szNpcTalk;				// 35
	std::string szNpcExchangeList;		// 36
	std::string szKnightsOperation;		// 37
	std::string szClassChange;			// 38
	std::string szEndingDisplay;		// 39
	std::string szWareHouse;			// 40
	std::string szChangeClassInit;		// 41
	std::string szChangeInitBill;		// 42
	std::string szInn;					// 43
	std::string szInputClanName;		// 44
	std::string szTradeBBS;				// 45
	std::string szTradeBBSSelector;		// 46
	std::string szTradeExplanation;		// 47
	std::string szTradeMemolist;		// 48
	std::string szQuestMenu;			// 49
	std::string szQuestTalk;			// 50
	std::string szQuestEdit;			// 51
	std::string szDead;					// 52
	std::string szElLoading;			// 53
	std::string szKaLoading;			// 54
	std::string szNationSelect;			// 55
	std::string szChat2;				// 56
	std::string szMsgOutput2;			// 57
	std::string szItemUpgrade;			// 58
	std::string szDuelCreate;			// 59
	std::string szDuelList;				// 60
	std::string szDuelMsg;				// 61
	std::string szDuelMsgEdit;			// 62
	std::string szDuelLobby;			// 63
	std::string szQuestContent;			// 64
	std::string szDuelItemCnt;			// 65
	std::string szTradeInv;				// 66
	std::string szTradeBuyInv;			// 67
	std::string szTradeItemDisplay;		// 68
	std::string szTradePrice;			// 69
	std::string szTradeCnt;				// 70
	std::string szTradeMsgBox;			// 71
	std::string szClanPage;				// 72
	std::string szAllyPage;				// 73
	std::string szAlly2Page;			// 74
	std::string szCmdList;				// 75
	std::string szCmdEdit;				// 76
	std::string szClanLogo;				// 77
	std::string szShopMall;				// 78
	std::string szLvlGuide;				// 79
	std::string szCSWNpc;				// 80
	std::string szKCSWPetition;			// 81
	std::string szCSWAlly;				// 82
	std::string szCSWSchedule;			// 83
	std::string szExitMenu;				// 84
	std::string szResurrect;			// 85
	std::string szNameChange;			// 86
	std::string szNameEditBox;			// 87
	std::string szNameCheck;			// 88
	std::string szCSWAdmin;				// 89
	std::string szCSWTax;				// 90
	std::string szCSWCapeList;			// 91
	std::string szKnightCapeShop;		// 92
	std::string szCSWTaxCollection;		// 93
	std::string szCSWTaxRate;			// 94
	std::string szCSWTaxRateMsg;		// 95
	std::string szCatapult;				// 96
	std::string szDisguiseRing;			// 97
	std::string szMsgBoxOk;				// 98
	std::string szMsgBoxOkCancel;		// 99
	std::string szOpenChat;				// 100
	std::string szCloseChat;			// 101
	std::string szChrClanLogo;			// 102
	std::string szWarning;				// 103
	std::string szConvo;				// 104
	std::string szBlog;					// 105
	std::string szInnPass;				// 106
	std::string szNoviceTips;			// 107
	std::string szWebpage;				// 108
	std::string szPartyMsgBox;			// 109
	std::string szClanLogo2;			// 110
	std::string szRentalNpc;			// 111
	std::string szRentalTransaction;	// 112
	std::string szRentalEntry;			// 113
	std::string szRentalItem;			// 114
	std::string szRentalMsg;			// 115
	std::string szRentalCnt;			// 116
	std::string szNetDIO;				// 117
	std::string szLoginIntro;			// 118
	std::string szSubLoginIntro;		// 119
	std::string szCharSelect;			// 120
	std::string szCharCreate;			// 121
	std::string szOtherState;			// 122
	std::string szPPCardBegin;			// 123
	std::string szPPCardList;			// 124
	std::string szPPCardReg;			// 125
	std::string szPPCardMsg;			// 126
	std::string szPPCardBuyList;		// 127
	std::string szPPCardMyInfo;			// 128
	std::string szNationSelect2;		// 129
	std::string szUSALogo;				// 130
	std::string szMonster;				// 131
	std::string szNationTaxNPC;			// 132
	std::string szNationTaxRate;		// 133
	std::string szKingMsgBoxOk;			// 134
	std::string szKingMsgBoxOkCancel;	// 135
	std::string szKingElectionBoard;	// 136
	std::string szKingElectionList;		// 137
	std::string szKingElectionMain;		// 138
	std::string szKingNominate;			// 139
	std::string szKingRegister;			// 140
	std::string szUpgradeRing;			// 141
	std::string szUpgradeSelect;		// 142
	std::string szTradeMsg;				// 143
	std::string szShowIcon;				// 144

} TABLE_UI_RESRC;

typedef struct __TABLE_ITEM_BASIC // Resource records for equipped items
{
	DWORD		dwID;				// 00 Item number code
	BYTE 		byExtIndex;			// 01 Extended index
	std::string	szName;				// 02 Name
	std::string	szRemark;			// 03 Item description	
	DWORD       dwTODO1;    		// 04 TODO: implement introduced in 1264
	BYTE        byTODO2;    		// 05 TODO: implement introduced in 1264
	DWORD		dwIDResrc;			// 06 Item resource number
	DWORD		dwIDIcon;			// 07 Item icon resource number
	DWORD		dwSoundID0;			// 08 Sound ID - If 0, no sound
	DWORD		dwSoundID1;			// 09 Sound ID - If 0, no sound

	BYTE	byClass;			// 10 Refer to Item Type b enum e_ItemClass
	BYTE	byIsRobeType;		// 11 Robe type item with a whole top and bottom
	BYTE	byAttachPoint;		// 12 Mounting location
	BYTE	byNeedRace;			// 13 Race
	BYTE	byNeedClass;		// 14 Job

	short	siDamage;			// 15 Weapon attack-strike power
	short	siAttackInterval;	// 16 Attack time 100 is 1 second
	short	siAttackRange;		// 17 Effective distances in .1 meter increments
	short	siWeight;			// 18 Weight .1 weight unit
	short	siMaxDurability;	// 19 Durability
	int		iPrice;				// 20 Purchase price
	int		iPriceSale;			// 21 Sell price
	short	siDefense;			// 22 Armor
	BYTE	byContable;			// 23 Is it an item with the number 21?

	DWORD	dwEffectID1;		// 24 magic effect ID1
	DWORD	dwEffectID2;		// 25 magic effect ID1 (ID2?)

	char	cNeedLevel;			// 26 Required level player's iLevel - may be negative..

	BYTE byTODO3; 				// 27 TODO: implement introduced in 1264

	BYTE	byNeedRank;			// 28 Required title player's iRank
	BYTE	byNeedTitle;		// 29 Required title player's iTitle
	BYTE	byNeedStrength;		// 30 Required Strength Player's iStrength
	BYTE	byNeedStamina;		// 31 Required Strength Player's iStamina..
	BYTE	byNeedDexterity;	// 32 Required Strength Player's iDexterity..
	BYTE	byNeedInteli;		// 33 Required Strength Player's iIntelligence..
	BYTE	byNeedMagicAttack;	// 34 Required Strength Player's iMagicAttak..
	BYTE	bySellGroup;		// 35 The shop-trader selling group
} TABLE_ITEM_BASIC;

const int MAX_ITEM_EXTENSION = 22; // Number of extended item tables.
const int LIMIT_FX_DAMAGE = 64;
const int ITEM_UNIQUE = 4;
const int ITEM_LIMITED_EXHAUST = 17;

typedef struct __TABLE_ITEM_EXT // Resource record for the equipped item extended item index
{
	DWORD		dwID;						// 00 Item number code
	std::string	szHeader;					// 01 Prefix
	DWORD       dwTODO1;					// 02 TODO: implement introduced in 1264 BaseID
	std::string	szRemark;					// 03 Item description
	DWORD       dwTODO2;      				// 04 TODO: implement introduced in 1097
	DWORD       dwTODO3;      				// 05 TODO: implement introduced in 1264 ResrcID
	DWORD       dwTODO4;     				// 06 TODO: implement introduced in 1264 IconID
	BYTE		byMagicOrRare;				// 07 Magic or rare item

	short	siDamage;						// 08 Weapon attack-strike power
	short	siAttackIntervalPercentage;		// 09 Attack time rate
	short	siHitRate;						// 10 Hit Rate - Percent
	short	siEvationRate;					// 11 Evasion Rate - "

	short	siMaxDurability;				// 12 Item Endurance
	short	siPriceMultiply;				// 13 Sell price multiplier
	short	siDefense;						// 14 Armor
	
	short	siDefenseRateDagger;			// 15 Dagger defense - percent
	short	siDefenseRateSword;				// 16 Sword defense - percent
	short	siDefenseRateBlow;				// 17 Hit-Blow-Strike-Club-Mace Defense - Percent
	short	siDefenseRateAxe;				// 18 Ax defense - percent
	short	siDefenseRateSpear;				// 19 Spear defense - percent
	short	siDefenseRateArrow;				// 20 Arrow defense - percent
	
	BYTE	byDamageFire;					// 21 Additional damage - fire
	BYTE	byDamageIce;					// 22 Additional damage - ice
	BYTE	byDamageThuner;					// 23 Additional damage - lightning
	BYTE	byDamagePoison;					// 24 Additional damage - Poison

	BYTE	byStillHP;						// 25 Absorp - HP
	BYTE	byDamageMP;						// 26 Damage - MP
	BYTE	byStillMP;						// 27 Absorb - MP
	BYTE	byReturnPhysicalDamage;			// 28 Physical Hit Reflection - Repel

	BYTE	bySoulBind;						// 29 Soul Bind - One-on-one item drop rate - Percentage - Currently not used.
	
	short	siBonusStr;						// 30 Bonus - strength
	short	siBonusSta;						// 31 Bonus - health points
	short	siBonusDex;						// 32 Bonus - dexterity
	short	siBonusInt;						// 33 Bonus - intelligence
	short	siBonusMagicAttak;				// 34 Bonus - magic attack mp
	short	siBonusHP;						// 35 Bonus - HP
	short	siBonusMSP;						// 36 Bonus - MSP - Mana

	short	siRegistFire;					// 37 Resistance - fire
	short	siRegistIce;					// 38 Resistance - ice
	short	siRegistElec;					// 39 Resistance - lightning
	short	siRegistMagic;					// 40 Resistance - magic
	short	siRegistPoison;					// 41 Resistance - poison
	short	siRegistCurse;					// 42 Resistance - curse
	
	DWORD	dwEffectID1;					// 43 Magic effect ID1
	DWORD	dwEffectID2;					// 44 Magic effect ID1

	short	siNeedLevel;					// 45 Required players stat points iLevel
	short	siNeedRank;						// 46 Required players stat points iRank
	short	siNeedTitle;					// 47 Required players stat points iTitle
	short	siNeedStrength;					// 48 Required players stat points iStrength
	short	siNeedStamina;					// 49 Required players stat points iStamina
	short	siNeedDexterity;				// 50 Required players stat points iDexterity
	short	siNeedInteli;					// 51 Required players stat points iIntelligence
	short	siNeedMagicAttack;				// 52 Required players stat points iMagicAttak
} TABLE_ITEM_EXT;

const int MAX_NPC_SHOP_ITEM = 30;
typedef struct __TABLE_NPC_SHOP
{
	DWORD		dwNPCID;
	std::string	szName;
	DWORD		dwItems[MAX_NPC_SHOP_ITEM];
} TABLE_NPC_SHOP;

enum e_ItemType { ITEM_TYPE_PLUG = 1, ITEM_TYPE_PART, ITEM_TYPE_ICONONLY, ITEM_TYPE_GOLD = 9, ITEM_TYPE_SONGPYUN = 10, ITEM_TYPE_UNKNOWN = 0xffffffff };

enum e_ItemPosition {	ITEM_POS_DUAL = 0,	ITEM_POS_RIGHTHAND, ITEM_POS_LEFTHAND,	ITEM_POS_TWOHANDRIGHT,	ITEM_POS_TWOHANDLEFT,
						ITEM_POS_UPPER = 5, ITEM_POS_LOWER,		ITEM_POS_HEAD,		ITEM_POS_GLOVES,		ITEM_POS_SHOES,
						ITEM_POS_EAR = 10,	ITEM_POS_NECK,		ITEM_POS_FINGER,	ITEM_POS_SHOULDER,		ITEM_POS_BELT,
						ITEM_POS_INVENTORY = 15, ITEM_POS_GOLD = 16, ITEM_POS_SONGPYUN = 17,
						ITEM_POS_UNKNOWN = 0xffffffff };
					
enum e_ItemSlot {	ITEM_SLOT_EAR_RIGHT = 0,	ITEM_SLOT_HEAD	= 1,	ITEM_SLOT_EAR_LEFT	= 2,
					ITEM_SLOT_NECK = 3,			ITEM_SLOT_UPPER	= 4,	ITEM_SLOT_SHOULDER	= 5,
					ITEM_SLOT_HAND_RIGHT = 6,	ITEM_SLOT_BELT	= 7,	ITEM_SLOT_HAND_LEFT = 8,
					ITEM_SLOT_RING_RIGHT = 9,	ITEM_SLOT_LOWER = 10,	ITEM_SLOT_RING_LEFT = 11,
					ITEM_SLOT_GLOVES = 12,		ITEM_SLOT_SHOES = 13, 
					ITEM_SLOT_COUNT = 14, ITEM_SLOT_UNKNOWN = 0xffffffff };


typedef struct __TABLE_PLAYER_LOOKS 		// Resource records for NPCs and Mobs
{
	DWORD		dwID; 						// NPC unique ID
	std::string	szName; 					// Character name
	std::string	szJointFN;					// Joint file name
	std::string	szAniFN; 					// Animation file name
	std::string	szPartFNs[7];				// Each Character Part - upper body, lower body, head, arms, legs, hair, cloak
	std::string skipn3cskin0;
	std::string skipn3char0;
	std::string skipFXPlug;
	int         iunknownplooks0;

	int			iJointRH;					// Right hand end joint number
	int			iJointLH;					// Left hand end joint number
	int			iJointLH2;					// Left forearm joint number
	int			iJointCloak;				// Joint number to attach the cloak to
	
	int			iSndID_Move;
	int			iSndID_Attack0;
	int			iSndID_Attack1;
	int			iSndID_Struck0;
	int			iSndID_Struck1;
	int			iSndID_Dead0;
	int			iSndID_Dead1;
	int			iSndID_Breathe0;
	int			iSndID_Breathe1;
	int			iSndID_Reserved0;
	int 		iunknownplooks1; 			// not sure if this one is supposed to be here
	int			iSndID_Reserved1;
	int 		iunknownplooks2; 			// not sure if this one is supposed to be here 
	BYTE 		byunknownplooks1;
	BYTE 		byunknownplooks2;
	BYTE 		byunknownplooks3;
} TABLE_PLAYER;

typedef struct __TABLE_EXCHANGE_QUEST 
{
    DWORD       dwID;              // Quest number
    DWORD       dwNpcNum;          // Npc number
    std::string szDesc;            // Description
    int         iCondition0;       // Condition 1
    int         iCondition1;       // Condition 2
    int         iCondition2;       // Condition 3
    int         iCondition3;       // Condition 4
    int         iNeedGold;         // Required Noah
    BYTE        bNeedLevel;        // Required level
    BYTE        bNeedClass;        // Required job
    BYTE        bNeedRank;         // Required title
    BYTE        bNeedExtra1;       // Required extra 1
    BYTE        bNeedExtra2;       // Required extra 2
    BYTE        bCreatePercentage; // Probability of creation
    int         iArkTuarek;        // Arc touareg
    int         iTuarek;           // Touareg
    int         iRinkleTuarek;     // Wrinkle touareg
    int         iBabarian;         // Barbarian
    int         iMan;              // Male
    int         iWoman;            // Female
} TABLE_EXCHANGE_QUEST;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Magic table...

typedef struct __TABLE_UPC_SKILL
{
    DWORD       dwID;       		// Skill unique ID
    std::string szEngName;  		// Skill english name
    std::string szName;     		// Skill Korean name
    std::string szDesc;      		// Skill description
    int         iSelfAnimID1; 		// Start caster action
    int         iSelfAnimID2; 		// End of caster action

	int			idwTargetAnimID;	// Target action
	int			iSelfFX1;			// Caster effect 1
	int			iSelfPart1;			// Location where the caster effect occurs 1
	int			iSelfFX2;			// Caster effect 2
	int			iSelfPart2;			// Location where the caster effect occurs 2
	int			iFlyingFX;			// Flight effect
	int			iTargetFX;			// Target effect

	int			iTargetPart;		// The location where the effect will occur
	int			iTarget;			// Target
	int			iNeedLevel;			// Required level
	int			iNeedSkill;			// Required skill
	int			iExhaustMSP;		// Consumed MSP
	
	int			iExhaustHP;			// Consumed HP
	DWORD		dwNeedItem;			// Refer to the requested item e_ItemClass It is the value divided by 10
	DWORD		dwExhaustItem;
	int			iCastTime;			// Casting time
	int			iReCastTime;		// Time taken before casting again

	float 		fUnkown1;        	// TODO: implement does this have to do with cooldown timers ??
	float 		fUnkown2;        	// TODO: implement does this have to do with cooldown timers ??
	int			iPercentSuccess;	// Success rate
	DWORD		dw1stTableType;		// First type
	DWORD		dw2ndTableType;		// Second type
	int			iValidDist;			// Valid range-distance
	int  		iUnkown1;        	// TODO: implement 
} TABLE_UPC_ATTACK_B;

typedef struct __TABLE_UPC_SKILL_TYPE_1
{
	DWORD 		dwID; 				// SKILL unique ID
    int 		iSuccessType; 		// success type.
    int			iSuccessRatio;		// success rate
    int			iPower;				// offense power
    int			iDelay;				// delay
    int			iComboType;			// combo type
    int			iNumCombo;			// combo count
    int			iComboDamage;		// combo damage
    int			iValidAngle;		// attack radius
    int			iAct[3];
} TABLE_UPC_SKILL_TYPE_1;

typedef struct __TABLE_UPC_SKILL_TYPE_2
{
	DWORD		dwID; 				// SKILL unique ID
	int			iSuccessType; 		// success type.
	int			iPower; 			// offense power
	int			skipall0iPowerPlus;	//adddamageplus
	int			iAddDist; 			// increment distance
	int			iNumArrow; 			// number of arrows required
} TABLE_UPC_SKILL_TYPE_2;

typedef struct __TABLE_UPC_SKILL_TYPE_3
{
	DWORD		dwID;					// Skill unique ID
	int   		iRadius;  				// TODO: Implement yo!
	int			iDDType;
	int			iStartDamage;
	int			iDuraDamage;
	int			iDurationTime;			// Duration
	int			iAttribute;
} TABLE_UPC_SKILL_TYPE_3;

typedef struct __TABLE_UPC_SKILL_TYPE_4
{
	DWORD		dwID;				// Serial Number
	int			iBuffType;			// Buff type
	int  		iRadius;			// TODO: implement
	int			iDuration;
	int			iAttackSpeed;		// Attack speed - rate
	int			iMoveSpeed;			// Movement speed
	int			iAC;				// Defense
	int   		iACPCT;				// TODO: implement
	int			iAttack;			// Offense power - attack
	int   		iMagicAttack;		// TODO: implement
	int			iMaxHP;				// MAXHP
	int   		iMaxHPPct;			// TODO: implement
	int   		iMaxMP;				// TODO: implement
	int   		iMaxMPPct;			// TODO: implement
	int			iStr;				// Strength
	int			iSta;				// Stamina - health points
	int			iDex;				// Dexterity
	int			iInt;				// Intelligence
	int			iMAP;				// Magical Attack Power - MP
	int			iFireResist;		// Resistance - Fire
	int			iColdResist;		// Resistance - Ice
	int			iLightningResist;	// Resistance - Lightning
	int			iMagicResist;		// Resistance - Magic
	int			iDeseaseResist;		// Resistance - Curse
	int			iPoisonResist;		// Resistance - Poison
	int   		iScrollMoveSpeed;	// TODO: implement
} TABLE_UPC_SKILL_TYPE_4;

typedef struct __TABLE_UPC_SKILL_TYPE_5
{
	DWORD		dwID;			// Serial Number
	DWORD		dwTarget;		// Target
	int			iSuccessRatio;	// Success rate
	int			iValidDist;		// Valid distance - range
	int			iRadius;		// Radius
	float		fCastTime;		// Casting time
	float		fRecastTime;	// Recast time
	int			iDurationTime;	// Duration time
	DWORD		dwExhaustItem;	// Consumable item
	DWORD		dwFX;			// Magic Effect
} TABLE_UPC_SKILL_TYPE_5;

typedef struct __TABLE_UPC_SKILL_TYPE_6 
{
    DWORD 		dwID;          // Serial Number
    DWORD 		dwTarget;      // Target
    int  		iSuccessRatio; // Success rate
    int			iValidDist;    // Valid distance - range
    int			iRadius;       // Radius
    float		fCastTime;     // Casting time
    float		fRecastTime;   // Recast time
    int			iDurationTime; // Duration time
    DWORD		dwExhaustItem; // Consumable item
    DWORD		dwFX;          // Magic effect
    DWORD		dwTranform;    // Transformation
} TABLE_UPC_SKILL_TYPE_6;

typedef struct __TABLE_UPC_SKILL_TYPE_7
{
	DWORD		dwID;			// Serial Number
	DWORD		dwTarget;		// Target
	DWORD		dwValidGroup;	// Effective group
	int			iSuccessRatio;	// Success rate
	int			iValidDist;		// Valid distance - range
	int			iRadius;		// Radius
	float		fCastTime;		// Casting time
	float		fRecastTime;	// Recast time
	int			iDurationTime;	// Duration time
	DWORD		dwExhaustItem;	// Consumable item
	DWORD		dwFX;			// Magic effect
} TABLE_UPC_SKILL_TYPE_7;

typedef struct __TABLE_UPC_SKILL_TYPE_8
{
	DWORD		dwID;			// Serial Number
	DWORD		dwTarget;		// Target
	int			iRadius;		// Radius
	DWORD		dwWarpType;		// Teleportation time
	float		fRefillEXP;		// Recovery experience
	DWORD		dwZone1;		// Zone Number 1
	DWORD		dwZone2;		// Zone Number 2
	DWORD		dwZone3;		// Zone Number 3
	DWORD		dwZone4;		// Zone Number 4
	DWORD		dwZone5;		// Zone Number 5
} TABLE_UPC_SKILL_TYPE_8;

typedef struct __TABLE_UPC_SKILL_TYPE_9
{
	DWORD		dwID;			// Serial Number
	DWORD		dwTarget;		// Target
	int			iSuccessRatio;	// Success rate
	int			iValidDist;		// Valid distance - range
	int			iRadius;		// Radius
	float		fCastTime;		// Casting time
	float		fRecastTime;	// Recast time
	int			iDurationTime;	// Duration time
	DWORD		dwExhaustItem;	// Consumable item
	DWORD		dwAttr;			// Property - attribute
	int			iDamage;		// Damage	
} TABLE_UPC_SKILL_TYPE_9;

typedef struct __TABLE_UPC_SKILL_TYPE_10
{
	DWORD		dwID;			// Serial Number
	DWORD		dwTarget;		// Target
	int			iSuccessRatio;	// Success rate
	int			iValidDist;		// Valid distance - range
	int			iRadius;		// Radius
	float		fCastTime;		// Casting time
	float		fRecastTime;	// Recast time
	DWORD		dwExhaustItem;	// Consumable item
	DWORD		dwRemoveAttr;	// Removal properties - attribute
} TABLE_UPC_SKILL_TYPE_10;

// Magic Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct __TABLE_QUEST_MENU
{
    DWORD       dwID;   // Serial Number
    std::string szMenu; // selection menu
} TABLE_QUEST_MENU;

typedef struct __TABLE_QUEST_TALK
{
    DWORD       dwID;   // Serial Number
    std::string szTalk; // Quest fingerprint
} TABLE_QUEST_TALK;

typedef struct __TABLE_HELP {
	int               iID;
	int               iLevelMin;
	int               iLevelMax;
	e_Class_Represent eRepresentClass;
	std::string       szTitle;
	std::string       szDescription;
} TABLE_HELP;

typedef struct __TABLE_TEXTS {
	DWORD       dwID;
	std::string szText;
} TABLE_TEXTS;

const int MAX_ITEM_SLOT_OPC = 8; // Equipped items - Other players (including NPCs) 0 ~ 4 upper body, lower body, helmet, arm, foot 5 cloak 6 right hand 7 left hand
const int MAX_ITEM_INVENTORY = 28;		// Owned item MAX (Inventory window)
const int MAX_ITEM_TRADE = 24;	// Trade with merchants
const int MAX_ITEM_TRADE_PAGE = 12;
const int MAX_ITEM_WARE_PAGE = 8;
const int MAX_ITEM_PER_TRADE = 12;	// Transactions with individuals	
const int MAX_ITEM_BUNDLE_DROP_PIECE = 6;
const int MAX_ITEM_EX_RE_NPC = 4; // Exchange, repair window NPC area

const int MAX_SKILL_FROM_SERVER = 9;		// The number of information slots received from the server	

const int MAX_SKILL_KIND_OF = 5;			// 1 Base Skill, 4 Specialist Skills
const int MAX_SKILL_IN_PAGE = 8;			// Number of icons in one page
const int MAX_SKILL_PAGE_NUM = 3;			// Maximum number of pages per skill		


const int MAX_SKILL_HOTKEY_PAGE = 8;		// Total number of pages of Hot Key			
const int MAX_SKILL_IN_HOTKEY = 8;			// The number of Hot Keys in the current page		
		
const int MAX_AVAILABLE_CHARACTER = 3;		// The maximum number of characters that can be selected per server

// Fighting By ecli666
const int ID_SOUND_ITEM_ETC_IN_INVENTORY	= 2000;
const int ID_SOUND_ITEM_IN_REPAIR			= 2001;
const int ID_SOUND_ITEM_WEAPON_IN_INVENTORY = 2002;
const int ID_SOUND_ITEM_ARMOR_IN_INVENTORY	= 2003;
const int ID_SOUND_GOLD_IN_INVENTORY		= 3000;
const int ID_SOUND_SKILL_THROW_ARROW		= 5500;
const int ID_SOUND_BGM_TOWN					= 20000;
const int ID_SOUND_BGM_KA_BATTLE			= 20002;
const int ID_SOUND_BGM_EL_BATTLE			= 20003;
const int ID_SOUND_CHR_SELECT_ROTATE		= 2501;

const float SOUND_RANGE_TO_SET = 10.0f;
const float SOUND_RANGE_TO_RELEASE = 20.0f;

const float STUN_TIME = 3.0f;

enum e_Behavior {	BEHAVIOR_NOTHING = 0,
					BEHAVIOR_EXIT,			// End the program
					BEHAVIOR_RESTART_GAME,	// Restart the game (select character)
					BEHAVIOR_REGENERATION,	// Respawn
					BEHAVIOR_CANCEL,		// Cancel Currently, when the user clicks cancel when requesting an item transaction

					BEHAVIOR_PARTY_PERMIT,	// Allow entry when the other party requests to join
					BEHAVIOR_PARTY_DISBAND, // Leave party
					BEHAVIOR_FORCE_PERMIT,	// Allow entry when the other party requests to join forces.
					BEHAVIOR_FORCE_DISBAND, // Leave unit

					BEHAVIOR_REQUEST_BINDPOINT, // As a binding point

					BEHAVIOR_DELETE_CHR,

					BEHAVIOR_KNIGHTS_CREATE,
					BEHAVIOR_KNIGHTS_DESTROY,	// Disband the Knights
					BEHAVIOR_KNIGHTS_WITHDRAW,	// Disband the Knights

					BEHAVIOR_PERSONAL_TRADE_FMT_WAIT,	// Private trade If I applied
					BEHAVIOR_PERSONAL_TRADE_PERMIT,		// Personal trade If I have applied

					BEHAVIOR_MGAME_LOGIN,
					
					BEHAVIOR_CLAN_JOIN,
					BEHAVIOR_PARTY_BBS_REGISTER,		// Register on the party board
					BEHAVIOR_PARTY_BBS_REGISTER_CANCEL, // Cancel registration on the party board

					BEHAVIOR_EXECUTE_OPTION,			// Exit the game and execute options
				
					BEHAVIOR_UNKNOWN = 0xffffffff
				};

enum e_SkillMagicTaget	{	SKILLMAGIC_TARGET_SELF = 1,				// Myself
							SKILLMAGIC_TARGET_FRIEND_WITHME = 2,	// One of our side (country) including me
							SKILLMAGIC_TARGET_FRIEND_ONLY = 3,		// One of my friends excluding me
							SKILLMAGIC_TARGET_PARTY = 4,			// One of our party including me
							SKILLMAGIC_TARGET_NPC_ONLY = 5,			// One of the NPCs
							SKILLMAGIC_TARGET_PARTY_ALL = 6,		// Everyone in my party who called me
							SKILLMAGIC_TARGET_ENEMY_ONLY = 7,		// One of all enemies except the enemy (including NPCs)
							SKILLMAGIC_TARGET_ALL = 8,				// One of everything that exists on the game
							
							SKILLMAGIC_TARGET_AREA_ENEMY = 10,		// Enemies in that area
							SKILLMAGIC_TARGET_AREA_FRIEND = 11,		// Friends in that area
							SKILLMAGIC_TARGET_AREA_ALL = 12,		// Everyone in that area
							SKILLMAGIC_TARGET_AREA = 13,			// Area centered on me
							SKILLMAGIC_TARGET_DEAD_FRIEND_ONLY = 25,	// One of our dead, minus me
							
							SKILLMAGIC_TARGET_UNKNOWN = 0xffffffff
						};


// Define fx
typedef struct __TABLE_FX		// FX resource record...
{
	DWORD		dwID;			// Unique id
	std::string szName;			// TODO: implement
	std::string	szFN;			// File name
	DWORD		dwSoundID;		// Sound ID for the effect
	BYTE		dwIdk;
} TABLE_FX;

const int	MAX_COMBO = 3;

const int	FXID_BLOOD = 10002;
const int	FXID_LEVELUP_KARUS			= 10012;
const int	FXID_LEVELUP_ELMORAD		= 10018;
const int	FXID_REGEN_ELMORAD			= 10019;
const int	FXID_REGEN_KARUS			= 10020;
const int	FXID_SWORD_FIRE_MAIN		= 10021;
const int	FXID_SWORD_FIRE_TAIL		= 10022;
const int	FXID_SWORD_FIRE_TARGET		= 10031;
const int	FXID_SWORD_ICE_MAIN			= 10023;
const int	FXID_SWORD_ICE_TAIL			= 10024;
const int	FXID_SWORD_ICE_TARGET		= 10032;
const int	FXID_SWORD_LIGHTNING_MAIN	= 10025;
const int	FXID_SWORD_LIGHTNING_TAIL	= 10026;
const int	FXID_SWORD_LIGHTNING_TARGET = 10033;
const int	FXID_SWORD_POISON_MAIN		= 10027;
const int	FXID_SWORD_POISON_TAIL		= 10028;
const int	FXID_SWORD_POISON_TARGET	= 10034;
//const int	FXID_GROUND_TARGET = 10035;
const int	FXID_REGION_TARGET_EL_ROGUE		= 10035;
const int	FXID_REGION_TARGET_EL_WIZARD	= 10036;
const int	FXID_REGION_TARGET_EL_PRIEST	= 10037;
const int	FXID_REGION_TARGET_KA_ROGUE		= 10038;
const int	FXID_REGION_TARGET_KA_WIZARD	= 10039;
const int	FXID_REGION_TARGET_KA_PRIEST	= 10040;
const int	FXID_CLAN_RANK_1				= 10041;
const int	FXID_REGION_POISON				= 10100;

//define skillmagic_type4_bufftype
enum e_SkillMagicType4	{	BUFFTYPE_MAXHP = 1,				// Change MaxHP
							BUFFTYPE_AC = 2,				// AC change
							BUFFTYPE_RESIZE = 3,			// Resize character
							BUFFTYPE_ATTACK = 4,			// Attack power
							BUFFTYPE_ATTACKSPEED = 5,		// Attack speed
							BUFFTYPE_SPEED = 6,				// Movement speed
							BUFFTYPE_ABILITY = 7,			// Five abilities (str, sta, cha, dex int)
							BUFFTYPE_RESIST = 8,			// Five resistances
							BUFFTYPE_HITRATE_AVOIDRATE = 9,	// Hitrate and avoidrate
							BUFFTYPE_TRANS = 10,			// Transform, invisible
							BUFFTYPE_SLEEP = 11,			// Sleep
							BUFFTYPE_EYE = 12				// Related to eyesight
};

enum e_SkillMagicType3	{	DDTYPE_TYPE3_DUR_OUR = 100,
							DDTYPE_TYPE3_DUR_ENEMY = 200
};



enum e_SkillMagicFailMsg {	SKILLMAGIC_FAIL_CASTING	= -100,		// Casting failed
							SKILLMAGIC_FAIL_KILLFLYING = -101,	// Kill the Flying object
							SKILLMAGIC_FAIL_ENDCOMBO = -102,	// Combo operation ended
							SKILLMAGIC_FAIL_NOEFFECT = -103,	// No effecting
							SKILLMAGIC_FAIL_ATTACKZERO = -104,	// Attack value 0 (mp is consumed and hit fails)

							SKILLMAGIC_FAIL_UNKNOWN = 0xffffffff
};

enum e_ObjectType	{	OBJECT_TYPE_BINDPOINT,
						OBJECT_TYPE_DOOR_LEFTRIGHT,
						OBJECT_TYPE_DOOR_TOPDOWN,
						OBJECT_TYPE_LEVER_TOPDOWN,
						OBJECT_TYPE_FLAG,
						OBJECT_TYPE_WARP_POINT,
						OBJECT_TYPE_UNKNOWN = 0xffffffff
					};

//definitions related clan....
const int	CLAN_LEVEL_LIMIT	= 20;
const int	CLAN_COST			= 500000;
const DWORD KNIGHTS_FONT_COLOR	= 0xffff0000; // Knights (clan) name font color

enum e_Cursor		{	CURSOR_ATTACK,
						CURSOR_EL_NORMAL,
						CURSOR_EL_CLICK,
						CURSOR_KA_NORMAL,
						CURSOR_KA_CLICK,
						CURSOR_PRE_REPAIR,
						CURSOR_NOW_REPAIR,
						CURSOR_COUNT,
						CURSOR_UNKNOWN = 0xffffffff
					};
