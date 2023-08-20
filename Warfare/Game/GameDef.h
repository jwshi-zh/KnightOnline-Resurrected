#pragma once

#include <string>
#include "DInput.h"

const int CURRENT_VERSION = 1068; // ÇöÀç ¹öÀü

const float PACKET_INTERVAL_MOVE = 1.5f;				// ¤±ÂÂûÂ¸·Î º¸³»´Â ĐÅ¶°£ °£°Italy..
const float PACKET_INTERVAL_ROTATE = 4.0f;
const float PACKET_INTERVAL_REQUEST_TARGET_HP = 2.0f;

// ´ÜÃàÅ° ÁöÁ¤ÇØ ³õÀº ºÎºÐ..
enum eKeyMap {
	KM_HOTKEY1 = DIK_1,
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
	KM_TARGET_PARTY8 = DIK_F8
};

enum e_PlayerType { PLAYER_BASE = 0, PLAYER_NPC = 1, PLAYER_OTHER = 2, PLAYER_MYSELF = 3 };

enum e_Race {
	RACE_ALL = 0,
	RACE_KA_ARKTUAREK = 1, RACE_KA_TUAREK = 2, RACE_KA_WRINKLETUAREK = 3, RACE_KA_PURITUAREK = 4,
	RACE_EL_BABARIAN = 11, RACE_EL_MAN = 12, RACE_EL_WOMEN = 13,
	// RACE_KA_NORMAL = 11, RACE_KA_WARRIOR = 12, RACE_KA_ROGUE = 13, RACE_KA_MAGE = 14,
	RACE_NPC = 100,
	RACE_UNKNOWN = 0xffffffff
};

enum e_Class {
	CLASS_KINDOF_WARRIOR = 1, CLASS_KINDOF_ROGUE, CLASS_KINDOF_WIZARD, CLASS_KINDOF_PRIEST,
	CLASS_KINDOF_ATTACK_WARRIOR, CLASS_KINDOF_DEFEND_WARRIOR, CLASS_KINDOF_ARCHER, CLASS_KINDOF_ASSASSIN,
	CLASS_KINDOF_ATTACK_WIZARD, CLASS_KINDOF_PET_WIZARD, CLASS_KINDOF_HEAL_PRIEST, CLASS_KINDOF_CURSE_PRIEST,

	CLASS_KA_WARRIOR = 101, CLASS_KA_ROGUE, CLASS_KA_WIZARD, CLASS_KA_PRIEST, // ¿©±â±îÁö ±âº» Á÷¾÷
	CLASS_KA_BERSERKER = 105, CLASS_KA_GUARDIAN, CLASS_KA_HUNTER = 107, CLASS_KA_PENETRATOR,
	CLASS_KA_SORCERER = 109, CLASS_KA_NECROMANCER, CLASS_KA_SHAMAN = 111, CLASS_KA_DARKPRIEST,

	CLASS_EL_WARRIOR = 201, CLASS_EL_ROGUE, CLASS_EL_WIZARD, CLASS_EL_PRIEST, // ¿©±â±îÁö ±âº» Á÷¾÷
	CLASS_EL_BLADE = 205, CLASS_EL_PROTECTOR, CLASS_EL_RANGER = 207, CLASS_EL_ASSASIN,
	CLASS_EL_MAGE = 209, CLASS_EL_ENCHANTER, CLASS_EL_CLERIC = 211, CLASS_EL_DRUID,

	CLASS_UNKNOWN = 0xffffffff
};

enum e_Class_Represent { CLASS_REPRESENT_WARRIOR = 0, CLASS_REPRESENT_ROGUE, CLASS_REPRESENT_WIZARD, CLASS_REPRESENT_PRIEST, CLASS_REPRESENT_UNKNOWN = 0xffffffff };

const float WEAPON_WEIGHT_STAND_SWORD = 5.0f; // ¹«±âÀÇ ¹«°Ô ±âÁØ...Ä®
const float WEAPON_WEIGHT_STAND_AXE = 5.0f; // ¹«±âÀÇ ¹«°Ô ±âÁØ...µµ³¢
const float WEAPON_WEIGHT_STAND_BLUNT = 8.0f; // ¹«±âÂÇ ¹«°O ±âÂØ...¸ùµÕàì A¾·ù

enum e_Ani {
	ANI_BREATH = 0, ANI_WALK, ANI_RUN, ANI_WALK_BACKWARD, ANI_STRUCK0, ANI_STRUCK1, ANI_STRUCK2, ANI_GUARD,
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

	ANI_SWORD_BREATH_A = 32, ANI_SWORD_ATTACK_A0, ANI_SWORD_ATTACK_A1,
	ANI_SWORD_BREATH_B, ANI_SWORD_ATTACK_B0, ANI_SWORD_ATTACK_B1,		// ¿Ñ¼Õ°Ë

	ANI_DAGGER_BREATH_A = 38, ANI_DAGGER_ATTACK_A0, ANI_DAGGER_ATTACK_A1,
	ANI_DAGGER_BREATH_B, ANI_DAGGER_ATTACK_B0, ANI_DAGGER_ATTACK_B1,		// ´Üµµ

	ANI_DUAL_BREATH_A = 44, ANI_DUAL_ATTACK_A0, ANI_DUAL_ATTACK_A1,
	ANI_DUAL_BREATH_B, ANI_DUAL_ATTACK_B0, ANI_DUAL_ATTACK_B1,			// ÀÌµµ·ù

	ANI_SWORD2H_BREATH_A = 50, ANI_SWORD2H_ATTACK_A0, ANI_SWORD2H_ATTACK_A1,
	ANI_SWORD2H_BREATH_B, ANI_SWORD2H_ATTACK_B0, ANI_SWORD2H_ATTACK_B1,	// ¾ç¼Õ°Ë

	ANI_BLUNT_BREATH_A = 56, ANI_BLUNT_ATTACK_A0, ANI_BLUNT_ATTACK_A1,
	ANI_BLUNT_BREATH_B, ANI_BLUNT_ATTACK_B0, ANI_BLUNT_ATTACK_B1,		// µÐ±â - ¸ùµÕÀÌ ?

	ANI_BLUNT2H_BREATH_A = 62, ANI_BLUNT2H_ATTACK_A0, ANI_BLUNT2H_ATTACK_A1,
	ANI_BLUNT2H_BREATH_B, ANI_BLUNT2H_ATTACK_B0, ANI_BLUNT2H_ATTACK_B1,	// ¾ç¼ÕÀ¸·Î Àâ´Â µÐ±â. - ¾ç¼Õ µµ³¢¿Í °°´Ù.

	ANI_AXE_BREATH_A = 68, ANI_AXE_ATTACK_A0, ANI_AXE_ATTACK_A1,
	ANI_AXE_BREATH_B, ANI_AXE_ATTACK_B0, ANI_AXE_ATTACK_B1,			// ÇÑ¼Õ µµ³¢

	ANI_SPEAR_BREATH_A = 74, ANI_SPEAR_ATTACK_A0, ANI_SPEAR_ATTACK_A1,
	ANI_SPEAR_BREATH_B, ANI_SPEAR_ATTACK_B0, ANI_SPEAR_ATTACK_B1,		// Ã¢ - º£´Â ³¯ÀÌ ¾ø´Â ±×³É Ã¢ÀÌ´Ù.

	ANI_POLEARM_BREATH_A = 80, ANI_POLEARM_ATTACK_A0, ANI_POLEARM_ATTACK_A1,
	ANI_POLEARM_BREATH_B, ANI_POLEARM_ATTACK_B0, ANI_POLEARM_ATTACK_B1,	// ¾ç¼ÕÀ¸·Î Àâ´Â ³¯ÀÖ´Â Ã¢ - Ã»·æµµ??

	ANI_NAKED_BREATH_A = 86, ANI_NAKED_ATTACK_A0, ANI_NAKED_ATTACK_A1,
	ANI_NAKED_BREATH_B, ANI_NAKED_ATTACK_B0, ANI_NAKED_ATTACK_B1,		// ¸Ç¸öÀ¸·Î ??

	ANI_BOW_BREATH = 92, ANI_CROSS_BOW_BREATH, ANI_LAUNCHER_BREATH,
	ANI_BOW_BREATH_B, ANI_BOW_ATTACK_B0, ANI_BOW_ATTACK_B1,			// È° °ø°Ý

	ANI_SHIELD_BREATH_A = 98, ANI_SHIELD_ATTACK_A0, ANI_SHIELD_ATTACK_A1,
	ANI_SHIELD_BREATH_B, ANI_SHIELD_ATTACK_B0, ANI_SHIELD_ATTACK_B1,		// ¹æÆÐ °ø°Ý

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

	// ¿©±âºÎÅÍ´Â NPC Animation
	ANI_NPC_BREATH = 0, ANI_NPC_WALK, ANI_NPC_RUN, ANI_NPC_WALK_BACKWARD,
	ANI_NPC_ATTACK0 = 4, ANI_NPC_ATTACK1, ANI_NPC_STRUCK0, ANI_NPC_STRUCK1, ANI_NPC_STRUCK2, ANI_NPC_GUARD,
	ANI_NPC_DEAD0 = 10, ANI_NPC_DEAD1, ANI_NPC_TALK0, ANI_NPC_TALK1, ANI_NPC_TALK2, ANI_NPC_TALK3,
	ANI_NPC_SPELLMAGIC0 = 16, ANI_NPC_SPELLMAGIC1,

	ANI_UNKNOWN = 0xffffffff
};


// MAX_INCLINE_CLIMB = sqrt( 1 - sin(90-ÃÖ´ë°æ»ç°¢)^2 )
// const float MAX_INCLINE_CLIMB = 0.5f; // ¿Ã¶ó°¥¼ö ÀÖ´Â ÃÖ´ë °æ»ç°ª = 30 µµ
const	float MAX_INCLINE_CLIMB = 0.6430f; // ¿Ã¶ó°¥¼ö ÀÖ´Â ÃÖ´ë °æ»ç°ª = 40 µµ
// const	float MAX_INCLINE_CLIMB = 0.7071f; // ¿Ã¶ó°¥¼ö ÀÖ´Â ÃÖ´ë °æ»ç°ª = 45 µµ
// const float MAX_INCLINE_CLIMB = 0.7660f; // ¿Ã¶ó°¥¼ö ÀÖ´Â ÃÖ´ë °æ»ç°ª = 50 µµ
// const float MAX_INCLINE_CLIMB = 0.8660f; // ¿Ã¶ó°¥¼ö ÀÖ´Â ÃÖ´ë °æ»ç°ª = 60 µµ


enum e_MoveDirection { MD_STOP, MD_FOWARD, MD_BACKWARD, MD_UNKNOWN = 0xffffffff };

const float MOVE_DELTA_WHEN_RUNNING = 3.0f; // ¶Û¶§ °öÇØÁö´Â º¯¼ö..
const float MOVE_SPEED_WHEN_WALK = 1.5f; // Player µéÀÌ °ÉÀ»¶§ Ç¥ÁØ¼Óµµ

// ÇöÀç »óÅÂ...
enum e_StateMove {
	PSM_STOP = 0,
	PSM_WALK,
	PSM_RUN,
	PSM_WALK_BACKWARD,
	PSM_COUNT
};

enum e_StateAction {
	PSA_BASIC = 0,		// ¾Æ¹«Áþµµ ¾ÈÇÏ°í ÀÖÀ½...
	PSA_ATTACK,			// °ø°Italy »óÅÂ..
	PSA_GUARD,			// ¹æ¾î¼º°ø - ¸·À½..
	PSA_STRUCK,			// ¾ò¾îÅÍÁü.
	PSA_DYING,			// Á×°í ÀÖ´ÂÁß(¾²·¯Áö´ÂÁß)
	PSA_DEATH,			// Á×¾î¼­ »¸À½..
	PSA_SPELLMAGIC,		// ¸¶¹ý ÁÖ¹® ¿Ü¿ì´Â Áß..
	PSA_SITDOWN, 		// ¾É¾Æ ÀÖ´ÂÁß...
	PSA_COUNT
};

enum e_StateDying {
	PSD_DISJOINT = 0,	// ºÐÇØµÇ¾î Á×´Â´Ù..
	PSD_KNOCK_DOWN,		// ³¯¾Æ°¡°Å³ª µÚ·Î ¹Ð¸®¸é¼ Á×´Â´Ù.
	PSD_KEEP_POSITION,	// Á¦ ÀÚ¸®¿¡¼ ÆûÀâ°í Á×´Â´Ù..
	PSD_COUNT,

	PSD_UNKNOWN = 0xffffffff
};

enum e_StateParty {
	PSP_NORMAL = 0,
	PSP_POISONING = 1,
	PSP_CURSED = 2,
	PSP_MAGIC_TAKEN = 4,
	PSP_BLESSED = 8,
	PSP_UNKNOWN = 0xffffffff
};

enum e_PartPosition {
	PART_POS_UPPER = 0,
	PART_POS_LOWER,
	PART_POS_FACE,
	PART_POS_HANDS,
	PART_POS_FEET,
	PART_POS_HAIR_HELMET,
	PART_POS_COUNT,
	PART_POS_UNKNOWN = 0xffffffff
};

enum e_PlugPosition {
	PLUG_POS_RIGHTHAND = 0,
	PLUG_POS_LEFTHAND,
	PLUG_POS_BACK,
	PLUG_POS_KNIGHTS_GRADE,
	PLUG_POS_COUNT,
	PLUG_POS_UNKNOWN = 0xffffffff
};

/*
enum e_ItemClass	{	ITEM_CLASS_DAGGER = 1, // ´Ü°Ë (dagger)
						ITEM_CLASS_SWORD, // 2 : ÇÑ¼Õ°Ë(onehandsword)
						ITEM_CLASS_SWORD_2H, // 3 : ¾ç¼Õ°Ë(twohandsword)
ITEM_CLASS_AXE, // 4 : ÇÑ¼Õµµ³¢(onehandaxe)
						ITEM_CLASS_AXE_2H, // 5 : µÎ¼Õµµ³¢(twohandaxe)
						ITEM_CLASS_MACE, // 6 : ÇÑ¼ÕÅ¸°Ý¹«±â(mace)
ITEM_CLASS_MACE_2H, // 7: °It«±â(twohandmace)
						ITEM_CLASS_SPEAR, // 8 : Ã¢ (spear)
ITEM_CLASS_POLEARM, // 9 : Æú¾Ï (pole arm)

						ITEM_CLASS_SHIELD_SMALL = 11, // 11 : ½º¸ô½¯µå(smallshield)
						ITEM_CLASS_SHIELD_LARGE, // 12 : ¶ó¾ÆÁö½¯µå(largeshield)
						ITEM_CLASS_SHIELD_KITE, // 13 : Ä«ÀÌÆ®½¯µå(kiteshield)
ITEM_CLASS_SHIELD_LARGETKITE, // 14: ¶ó¾ÆÁöÄ«ÀÌÆ®(largekite)
						ITEM_CLASS_SHIELD_PLATE, // 15 : ÇÃ·¹ÀÌÆ®½¯µå(plateshield)

						ITEM_CLASS_BOW_SHORT = 21, // 21 : ¼îÆ®º¸¿ì(Shortbow)
						ITEM_CLASS_BOW_WRAPT, // 22 : ·¦Æ®º¸¿ì(wraptbow)
						ITEM_CLASS_BOW_COMPOSITE, // 23 : ÄÞÆ÷ÁöÆ®º¸¿ì(compositebow)
						ITEM_CLASS_BOW_IRON, // 24 : ¾ÆÀÌ¾ðº¸¿ì(ironbow)
						ITEM_CLASS_BOW_LONG, // 25: ·Õº¸¿ì(longbow)
						ITEM_CLASS_BOW_CROSS, // 28: Å©·Î½ºº¸¿ì(crossbow)

						ITEM_CLASS_STAFF = 31, // 31 : AöÆÎàì (staff)
						ITEM_CLASS_ETC, // 32 : ±âÅ¸ ý °Ç°

						ITEM_CLASS_ARMOR_COTTON = 41, // 41 : Ãµ¹æ¾î±¸(cotton)
ITEM_CLASS_ARMOR_FUR, // 42 : ÅÐ°¡Á×(Fur)
						ITEM_CLASS_ARMOR_LEATHER, // 43 : °¡Á×¹æ¾î±¸(leather)
						ITEM_CLASS_ARMOR_HADLEATHER, // 44 : ÇÏµå·¹´õ¹æ¾î±¸(hardleather)
						ITEM_CLASS_ARMOR_RINGMAIL, // 45 : ¸µ¹æ¾î±¸(ringmail)
						ITEM_CLASS_ARMOR_SCALEMAIL, // 46 : ºñ´Ã¹æ¾î±¸(scaledmail)
ITEM_CLASS_ARMOR_HALFPLATE, // 47 : ÇÏÇÁ ÇÃ·¹ÀÌÆ® ¹æ¾î±¸
						ITEM_CLASS_ARMOR_FULLPLATE, // 48 : Ã¶ÆÇ¹æ¾î±¸(platemail)
ITEM_CLASS_ROBE, // 49 : ý»ç·Îºê(robe)

						ITEM_CLASS_ARROW = 101,

						ITEM_CLASS_UNKNOWN = 0xffffffff }; // 101: è»ì(arrow)
*/
enum e_ItemAttrib {
	ITEM_ATTRIB_GENERAL = 0,
	ITEM_ATTRIB_MAGIC = 1,
	ITEM_ATTRIB_LAIR = 2,
	ITEM_ATTRIB_CRAFT = 3,
	ITEM_ATTRIB_UNIQUE = 4,
	ITEM_ATTRIB_UPGRADE = 5,
	ITEM_ATTRIB_UNKNOWN = 0xffffffff
};

enum e_ItemClass {
	ITEM_CLASS_DAGGER = 11, // ´Ü°Ë (dagger)
	ITEM_CLASS_SWORD = 21, // ÇÑ¼Õ°Ë(onehandsword)
	ITEM_CLASS_SWORD_2H = 22, // 3 : ¾ç¼Õ°Ë(twohandsword)
	ITEM_CLASS_AXE = 31, // ÇÑ¼Õµµ³¢(onehandaxe)
	ITEM_CLASS_AXE_2H = 32, // µÎ¼Õµµ³¢(twohandaxe)
	ITEM_CLASS_MACE = 41, // ÇÑ¼ÕÅ¸°Ý¹«±â(mace)
	ITEM_CLASS_MACE_2H = 42, // µÎ¼ÕÅ¸°It«±â(twohandmace)
	ITEM_CLASS_SPEAR = 51, // Ã¢ (spear)
	ITEM_CLASS_POLEARM = 52, // Æú¾Ï (polearm)

	ITEM_CLASS_SHIELD = 60, // ½¯µå(shield)

	ITEM_CLASS_BOW = 70, // ¼îÆ®º¸¿ì(Shortbow)
	ITEM_CLASS_BOW_CROSS = 71, // Å©·Î½ºº¸¿ì(crossbow)
	ITEM_CLASS_BOW_LONG = 80, // ·Õº¸¿ì(longbow)

	ITEM_CLASS_EARRING = 91, // ±Í°ÉÀÌ
	ITEM_CLASS_AMULET = 92, // ¸ñ°ÉÀÌ
	ITEM_CLASS_RING = 93, // Italy
	ITEM_CLASS_BELT = 94, // Çã¸®¶ì
	ITEM_CLASS_CHARM = 95, // ÀÎº¥Åä¸®¿¡ Áö´Ï°í ÀÖ´Â ¾ÆÀÌÅÛ
	ITEM_CLASS_JEWEL = 96, // º¸¼®Á¾·ù
	ITEM_CLASS_POTION = 97, // ¹°¾à
	ITEM_CLASS_SCROLL = 98, // ½ºÅ©·Ñ

	ITEM_CLASS_LAUNCHER = 100, // Ã¢´øÁú¶§ ¾²´Â ¾ÆÀÌÅÛ..

	ITEM_CLASS_STAFF = 110, // AöÆÎÂì(staff)
	ITEM_CLASS_ARROW = 120, // Is»ì(Arrow)
	ITEM_CLASS_JAVELIN = 130, // õÃ¢

	ITEM_CLASS_ARMOR_WARRIOR = 210, // Àü»ç ¹æ¾î±¸
	ITEM_CLASS_ARMOR_ROGUE = 220, // ·Î±× ¹æ¾î±¸
	ITEM_CLASS_ARMOR_MAGE = 230, // ý»ç ¹æ¾î±¸
	ITEM_CLASS_ARMOR_PRIEST = 240, // »çÁ¦ ¹æ¾î±¸

	ITEM_CLASS_ETC = 251, // ±âÅ¸

	ITEM_CLASS_UNKNOWN = 0xffffffff
}; // 

enum e_Nation { NATION_NOTSELECTED = 0, NATION_KARUS, NATION_ELMORAD, NATION_UNKNOWN = 0xffffffff };

struct __TABLE_ITEM_BASIC;
struct __TABLE_ITEM_EXT;
struct __TABLE_PLAYER;

enum e_Authority { AUTHORITY_MANAGER = 0, AUTHORITY_USER, AUTHORITY_NOCHAT, AUTHORITY_NPC, AUTHORITY_BLOCK_USER = 0xff };

struct __InfoPlayerBase
{
	int			iID;			// °íÀ¯ ID
	std::string szID;			// ÀÌ¸§
	D3DCOLOR	crID;			// ÀÌ¸§ »ö±ò..
	e_Race		eRace;			// Ä³¸¯ÅÍ °ñ°Ý¿¡ µû¸¥ Á¾Á·
	e_Nation	eNation;		// ¼Ò¼Ó ±¹°¡..
	e_Class		eClass;			// Á÷¾÷
	int			iLevel;			// ·¹º§
	int			iHPMax;
	int			iHP;
	int			iAuthority;		// ±ÇÇÑ - 0 °ü¸®ÀÚ, 1 - ÀÏ¹ÝÀ¯Àú, 255 - ºí·°´çÇÑ À¯Àú...

	bool		bRenderID;		// È­¸é¿¡ ID ¸¦ Âï´ÂÁö..

	void Init()
	{
		iID = 0;					// °íÀ¯ ID
		szID = "";					// ÀÌ¸§
		crID = 0;					// ÀÌ¸§ »ö±ò..
		eRace = RACE_UNKNOWN;		// Ä³¸¯ÅÍ °ñ°Ý¿¡ µû¸¥ Á¾Á·
		eNation = NATION_UNKNOWN;	// ¼Ò¼Ó ±¹°¡..
		eClass = CLASS_UNKNOWN;		// Á÷¾÷
		iLevel = 0;					// ·¹º§
		iHPMax = 0;
		iHP = 0;
		iAuthority = 1;				// ±ÇÇÑ - 0 °ü¸®ÀÚ, 1 - ÀÏ¹ÝÀ¯Àú, 255 - ºí·°´çÇÑ À¯Àú...
		bRenderID = true;
	}
};

struct __InfoPlayerOther
{
	int			iFace;			// ¾ó±¼ ¸ð¾ç..
	int			iHair;			// ¸Ó¸®Ä«¶ô ¸ð¾ç..

	int			iCity;			// ¼Ò¼Óµµ½Ã
	int			iKnightsID;		// ¼Ò¼Ó ±â»ç´Ü(±æµå) ID
	std::string szKnights;		// ¼Ò¼Ó ±â»ç´Ü(±æµå) ÀÌ¸§
	int			iKnightsGrade;	// ¼Ò¼Ó ±â»ç´Ü(±æµå) µî±Þ
	int			iKnightsRank;	// ¼Ò¼Ó ±â»ç´Ü(±æµå) ¼øÀ§

	int			iRank;			// ÀÛÀ§ - ¹éÀÛ, °øÀÛ - ±ÇÇÑ¿¡ µû¸¥ ºÐ·ù
	int			iTitle;			// Á÷À§ - ¿µÁÖ, ¼ºÁÖ - ´Ü¼øÇÑ ½ÅºÐ -&gt; ±æµåÀå...

	void Init()
	{
		iFace = 0;			// ¾ó±¼ ¸ð¾ç..
		iHair = 0;			// ¸Ó¸®Ä«¶ô ¸ð¾ç..
		iCity;				// ¼Ò¼Óµµ½Ã
		iKnightsID = 0;		// ¼Ò¼Ó ±â»ç´Ü(±æµå)
		szKnights = "";		// ¼Ò¼Ó ±â»ç´Ü(±æµå)
		iKnightsGrade = 0;		// ¼Ò¼Ó ±â»ç´Ü(±æµå) »ö±ò
		iKnightsRank = 0;			// ÀÛÀ§ - ¹éÀÛ, °øÀÛ - ±ÇÇÑ¿¡ µû¸¥ ºÐ·ù
		iTitle = 0;			// Á÷À§ - ¿µÁÖ, ¼ºÁÖ - ´Ü¼øÇÑ ½ÅºÐ -&gt; ±æµåÀå...
	}
};

// ±â»ç´Ü Á÷À§..
enum e_KnightsDuty {
	KNIGHTS_DUTY_UNKNOWN = 0,		// ???? ÂÑ°Ü³²??
	KNIGHTS_DUTY_CHIEF = 1,			// ´ÜÀå
	KNIGHTS_DUTY_VICECHIEF = 2,		// ºÎ´ÜÀåå
	KNIGHTS_DUTY_PUNISH = 3,		// Â¡°èÁß.
	KNIGHTS_DUTY_TRAINEE = 4,		// °ß½À±â»ç
	KNIGHTS_DUTY_KNIGHT = 5,		// ÀÏ¹Ý±â»ç
	KNIGHTS_DUTY_OFFICER = 6		// Àå±³
};

#define VICTORY_ABSENCE		0
#define VICTORY_KARUS		1
#define VICTORY_ELMORAD		2

struct __InfoPlayerMySelf : public __InfoPlayerOther
{
	int					iBonusPointRemain; // ³²´Â º¸³Ê½º Æ÷ÀÎÆ®...
	int					iLevelPrev; // Á÷ÀüÀÇ ·¹º§...

	int					iMSPMax;
	int					iMSP;

	int					iTargetHPPercent;
	int					iGold;
	int					iExpNext;
	int					iExp;
	int					iRealmPoint;		// ±¹°¡ ±â¿©mm
	e_KnightsDuty		eKnightsDuty;		// ±â»ç´Ü ±ÇÇÑ
	int					iWeightMax;			// ¾ÆÀÌÅÛÀÇ ÃÑ¹«°Ô
	int					iWeight;			// µé¼ö ÀÖ´Â àìÅÛ ¹«°Oh..
	int					iStrength;			// It&#39;s
	int					iStrength_Delta;		// ¾ÆÂÅÛÂÂÂ³ª ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ°ª.
	int					iStamina;			// ½ºÅ×¹Ì³Ê
	int					iStamina_Delta;		// ¾ÆÂÅÛÂÂÂ³ª ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ°ª.
	int					iDexterity;			// ¹ÎÃ·¼º..
	int					iDexterity_Delta;	// ¾ÆÂÅÛÂÂÂ³ª ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ°ª.
	int					iIntelligence;		// Áö´É
	int					iIntelligence_Delta; // ¾ÆÂÅÛÂÂÂ³ª ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ°ª.
	int 				iMagicAttak;		// ¸¶·Â
	int 				iMagicAttak_Delta;	// ¾ÆÂÅÛÂÂÂ³ª ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ°ª.

	int 				iAttack;		// °ø°Italy·Â
	int 				iAttack_Delta;	// ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ °ª..
	int 				iGuard;			// ¹æ¾î·Â
	int 				iGuard_Delta;	// ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ °ª..

	int 				iRegistFire;			// ÀúÇ×·Â
	int 				iRegistFire_Delta;		// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..
	int 				iRegistCold;			// ÀúÇ×·Â
	int 				iRegistCold_Delta;		// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..
	int 				iRegistLight;			// ÀúÇ×·Â
	int 				iRegistLight_Delta;		// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..
	int 				iRegistMagic;			// ÀúÇ×·Â
	int 				iRegistMagic_Delta;		// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..
	int 				iRegistCurse;			// ÀúÇ×·Â
	int 				iRegistCurse_Delta;		// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..
	int 				iRegistPoison;			// ÀúÇ×·Â
	int 				iRegistPoison_Delta;	// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..

	int					iZoneInit;				// ¼­¹öÇÑÅ× Ã³À½¿¡ ¹ÞÀº Á¸¹øÈ£
	int					iZoneCur;				// ÇöÀç Á¸..
	int					iVictoryNation;			// 0: ¹«½ÂºÎ 1:¿¤¸ð¶óµå ½Â¸® 2:Ä«·ç½º ½Â¸®

	void Init()
	{
		__InfoPlayerOther::Init();

		iBonusPointRemain = 0; // ³²´Â º¸³Ê½º Æ÷ÀÎÆ®...
		iLevelPrev = 0; // Á÷ÀüÀÇ ·¹º§...

		iMSPMax = 0;
		iMSP = 0;

		iTargetHPPercent = 0;
		iGold = 0;
		iExpNext = 0;
		iExp = 0;
		iRealmPoint = 0;		// ±¹°¡ ±â¿©mm
		eKnightsDuty = KNIGHTS_DUTY_UNKNOWN;		// ±â»ç´Ü ±ÇÇÑ
		iWeightMax = 0;			// ¾ÆÀÌÅÛÀÇ ÃÑ¹«°Ô
		iWeight = 0;			// µé¼ö ÀÖ´Â àìÅÛ ¹«°Oh..
		iStrength = 0;			// It&#39;s
		iStrength_Delta = 0;	// ¾ÆÂÅÛÂÂÂ³ª ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ°ª.
		iStamina = 0;			// ½ºÅ×¹Ì³Ê
		iStamina_Delta = 0;		// ¾ÆÂÅÛÂÂÂ³ª ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ°ª.
		iDexterity = 0;			// ¹ÎÃ·¼º..
		iDexterity_Delta = 0;	// ¾ÆÂÅÛÂÂÂ³ª ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ°ª.
		iIntelligence = 0;		// Áö´É
		iIntelligence_Delta = 0; // ¾ÆÂÅÛÂÂÂ³ª ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ°ª.
		iMagicAttak = 0;		// ¸¶·Â
		iMagicAttak_Delta = 0;	// ¾ÆÂÅÛÂÂÂ³ª ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ°ª.

		iAttack = 0;		// °ø°Italy·Â
		iAttack_Delta = 0;	// ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ °ª..
		iGuard = 0;			// ¹æ¾î·Â
		iGuard_Delta = 0;	// ¸¶¹ý¿¡ ÀÇÇØ °¡°¨µÈ °ª..

		iRegistFire = 0;			// ÀúÇ×·Â
		iRegistFire_Delta = 0;		// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..
		iRegistCold = 0;			// ÀúÇ×·Â
		iRegistCold_Delta = 0;		// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..
		iRegistLight = 0;			// ÀúÇ×·Â
		iRegistLight_Delta = 0;		// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..
		iRegistMagic = 0;			// ÀúÇ×·Â
		iRegistMagic_Delta = 0;		// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..
		iRegistCurse = 0;			// ÀúÇ×·Â
		iRegistCurse_Delta = 0;		// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..
		iRegistPoison = 0;			// ÀúÇ×·Â
		iRegistPoison_Delta = 0;	// ¸¶¹ý¿¡ ÀÇÇÑ ÀúÇ×·Â °ª..

		iZoneInit = 0x01;			// ¼­¹öÇÑÅ× Ã³À½¿¡ ¹ÞÀº Á¸¹øÈ£
		iZoneCur = 0;				// ÇöÀç Á¸..
		iVictoryNation = -1;		// ÀüÀï¿¡¼ ÀÌ±ä ±¹°¡
	}
};

const int MAX_PARTY_OR_FORCE = 8;

struct __InfoPartyOrForce
{
	int			iID;			// ÆÄÆ¼¿ø ID
	int			iLevel;			// Level
	e_Class		eClass;			// Á÷¾÷
	int			iHP;			// Hit Point
	int			iHPMax;			// Hit Point Max
	bool		bSufferDown_HP;			// Status - HP ¶³¾îÁü...
	bool		bSufferDown_Etc;		// Status - ÀúÁÖ°è¿ ¿µÇâÀ» ¹ÞÀ»¶§
	std::string szID;		// ÂÂ

	void Init()
	{
		iID = -1;
		iLevel = 0;
		eClass = CLASS_UNKNOWN;
		iHP = 0;
		iHPMax = 0;
		szID = "";

		bSufferDown_HP = false;			// Status - HP ¶³¾îÁü...
		bSufferDown_Etc = false;		// Status - ÀúÁÖ°è¿ ¿µÇâÀ» ¹ÞÀ»¶§
	};

	__InfoPartyOrForce()
	{
		this->Init();
	}
};

enum e_PartyStatus { PARTY_STATUS_DOWN_HP = 1, PARTY_STATUS_DOWN_ETC = 2 };

struct __InfoPartyBBS // Æ¼ solution °Ô½ ±solution..
{
	std::string szID;			// ÂÂ
	int			iID;			// ÆÄÆ¼¿ø ID
	int			iLevel;			// Level
	e_Class		eClass;			// Á÷¾÷
	int			iMemberCount;

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

enum {
	RESRC_UI_LOGIN = 0, RESRC_UI_CHARACTERSELECT, RESRC_UI_CHARACTERCREATE, RESRC_UI_INVENTORY, RESRC_UI_CHAT,
	RESRC_UI_MYINFORMATION, RESRC_UI_TARGET, RESRC_UI_HOTKEY, RESRC_UI_FUNCTION
};

typedef struct __TABLE_ZONE
{
	DWORD		dwID; // zone ID
	std::string	szTerrainFN; // Terrain FileName - Terrain, Tile Map, ColorMap....
	std::string szZoneName;
	std::string	szColorMapFN;
	std::string	szLightMapFN;
	std::string	szObjectPostDataFN; // Object À§Ä¡ Á¤º¸ - ÀÌ¾È¿¡ Ãæµ¹ Ã¼Å© µ¥ÀÌÅÍµµ µé¾î ÀÖ´Ù.
	std::string	szObjectPostDataExtFN; // Object À§Ä¡ Á¤º¸ - ÀÌ¾È¿¡ Ãæµ¹ Ã¼Å© µ¥ÀÌÅÍµµ µé¾î ÀÖ´Ù.
	std::string	szMiniMapFN;
	std::string szSkySetting; // ÇÏ´Ã°ú ºÐÀ§±â Á¶Á¤ÇÑ ¼¼ÆÃ Á¤º¸ ÆÄÀÏ..
	uint32_t		bIndicateEnemyPlayer;	// Àû±¹ ÇÃ·¹ÀÌ¾î¸¦ Ç¥½ÃÇÏ³ª??
	uint32_t			iFixedSundDirection;	// ÇØÀÇ ¹æÇâÀ» °íÁ¤½ÃÅ°´ÂÁö..
	std::string szLightObjFN;		// ÁöÇü¿¡ ¹èÄ¡µÇ¾î ÀÖ´Â ¶óÀÌÆ®Á¤º¸ÆÄÀÏ..

	std::string szGevFN;         // TODO: implement
	uint32_t    iAbyssSomething; // TODO: implement
	std::string szEnsFN;         // TODO: implement
	float       fFOV;            // TODO: implement
	std::string szFlagFN;        // TODO: implement

	uint32_t iIdk1;
	uint32_t iIdk2;
	uint32_t iIdk3;
	uint32_t iIdk4;

	std::string	szOpdSubFN;
	uint32_t iIdk5;
	std::string	szEvtSubFN;
} TABLE_ZONE;

typedef struct __TABLE_UI_RESRC
{
	uint32_t	dwID;					// 0 (War/Human)
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

typedef struct __TABLE_ITEM_BASIC // ÀåÂø ¾ÆÀÌÅÛ¿¡ °üÇÑ ¸®¼Ò½º ·¹ÄÚµå...
{
	DWORD		dwID;				// 00 ÄÚµåÈ­µÈ ¾ÆÀÌÅÛ¹øÈ£ d - // 00 - Item Á¾·ù, 00 - Item ÀåÂø À§Ä¡(ÀåÂøÀ§Ä¡·Î Plug ÀÎÁö Part ÀÎÁö ÆÇ´ÜÀÌ °¡´ÉÇÏ´Ù.) - 0000 - ItemIndex
	BYTE 		byExtIndex;			// 01 È®Àå ÀÎµ¦½º
	std::string	szName;				// 02 ÀÌ¸§
	std::string	szRemark;			// 03 ¾ÆÀÌÅÛ ¼³¸í
	DWORD       dwTODO1;    // TODO: implement introduced in 1264
	BYTE        byTODO2;    // TODO: implement introduced in 1264
	DWORD		dwIDResrc;			// 04 ÄÚµåÈµÈ ¾ÆÀÌÅÛ ¸®¼Ò½º d
	DWORD		dwIDIcon;			// 05 ÄÚµåÈµÈ ¾ÆÀÌÅÛ ¾ÆÀÌÄÜ ¸®¼Ò½º d
	DWORD		dwSoundID0;			// 06 Sound ID - 0 ÀÌ¸é »ç¿îµå ¾ø´Ù~..
	DWORD		dwSoundID1;			// 07 Sound ID - 0 ÀÌ¸é »ç¿îµå ¾ø´Ù~..

	BYTE	byClass;			// 08 ¾ÆÀÌÅÛ ÇüÅÂ	b enum e_ItemClass ÂüÁ¶....
	BYTE	byIsRobeType;		// 09 À§¾Æ·¡°¡ ÅëÂ°·Î µÈ ·ÎºêÅ¸ÀÔÀÇ ¾ÆÀÌÅÛ.....
	BYTE	byAttachPoint;		// 10 ÀåÂø À§Ä¡ b
	BYTE	byNeedRace;			// 11 Á¾Á·	b
	BYTE	byNeedClass;		// 12 Á÷¾÷	b

	short	siDamage;			// 13 ¹«±âÅ¸°Italian b
	short	siAttackInterval;	// 14 °ø°Italy °£ 100 Àº 1 E
	short	siAttackRange;		// 15 À¯È¿ °Å¸®	.1 meter ´ÜÀ§
	short	siWeight;			// 16 ¹«°Ô .1 Áß·®´ÜÀ§
	short	siMaxDurability;	// 17 ³»±¸·Â
	int		iPrice;				// 18 ¸Å¼ö°¡
	int		iPriceSale;			// 19 ¸Åµµ°¡
	short	siDefense;			// 20 ¹æ¾î·Â
	BYTE	byContable;			// 21 ¼ýÀÚ°¡ ÀÖ´Â ¾ÆÀÌÅÛÀÎ°¡??

	DWORD	dwEffectID1;		// 22 ¸¶¹ý È¿°ú ID1
	DWORD	dwEffectID2;		// 23 ¸¶¹ý È¿°ú ID1

	char	cNeedLevel;		// 24 ¿ä±¸ ·¹º§ ÇÃ·¹ÀÌ¾îÀÇ iLeve.. - À½¼ö°ªÀÌ ÀÖÀ»¼öµµ ÀÖ´Ù..

	BYTE byTODO3; // TODO: implement introduced in 1264

	BYTE	byNeedRank;			// 25 ¿ä±¸ ÀÛÀ§ ÇÃ·¹ÀÌ¾îÀÇ iRank..
	BYTE	byNeedTitle;		// 26 ¿ä±¸ Å¸ÀÌÆ² ÇÃ·¹ÀÌ¾îÀÇ iTitle..
	BYTE	byNeedStrength;		// 27 ¿ä±¸ Èû ÇÃ·¹ÀÌ¾îÀÇ iStrength..
	BYTE	byNeedStamina;		// 28 ¿ä± ̧ Á¤·Â ÇÃ·1ÀÌ3⁄4îÀÇ iStamina.
	BYTE	byNeedDexterity;	// 29 ¿ä±¸ ¹ÎÃ¸ ÇÃ·¹ÀÌ¾îÀÇ iDexterity..
	BYTE	byNeedInteli;		// 30 ¿ä±¸ Áö´É ÇÃ·¹ÀÌ¾îÀÇ iIntelligence..
	BYTE	byNeedMagicAttack;	// 31 ¿ä±¸ ¸¶·Â ÇÃ·¹ÀÌ¾îÀÇ iMagicAttak..

	BYTE	bySellGroup;		// 32 »óàÎàì ÆÄ´Âµ¥¿¡ ´ëÇÑ ±×·ì..
} TABLE_ITEM_BASIC;

const int MAX_ITEM_EXTENSION = 22; // È®Àå ¾ÆÀÌÅÛ Å×ÀÌºí °¹¼ö.
const int LIMIT_FX_DAMAGE = 64;
const int ITEM_UNIQUE = 4;
const int ITEM_LIMITED_EXHAUST = 17;

typedef struct __TABLE_ITEM_EXT // ÀåÂø ¾ÆÀÌÅÛ¿¡ °üÇÑ ¸®¼Ò½º ·¹ÄÚµå...
{
	DWORD		dwID;				// 00 ÄÚµåÈ­µÈ ¾ÆÀÌÅÛ¹øÈ£ - // 00 - Item Á¾·ù, 00 - Item ÀåÂø À§Ä¡(ÀåÂøÀ§Ä¡·Î Plug ÀÎÁö Part ÀÎÁö ÆÇ´ÜÀÌ °¡´ÉÇÏ´Ù.) - 0000 - ItemIndex
	std::string	szHeader;			// 01 Á¢µÎ»ç
	DWORD       dwTODO1;       // TODO: implement introduced in 1264 BaseID
	std::string	szRemark;			// 02 ¾ÆÀÌÅÛ ¼³¸í
	DWORD       dwTODO2;       // TODO: implement introduced in 1097
	DWORD       dwTODO3;       // TODO: implement introduced in 1264 ResrcID
	DWORD       dwTODO4;       // TODO: implement introduced in 1264 IconID
	BYTE		byMagicOrRare;		// 03 ¸ÅÁ÷ È¤Àº ·¹¾î ¾ÆÀÌÅÛÀÎÁö...

	short	siDamage;				// 04 ¹«±âÅ¸°Italy
	short	siAttackIntervalPercentage;		// 05 °ø°Italy ½Ã°£ ñà²
	short	siHitRate;				// 06 Å¸°Ý·ü - ÆÛ¼¾Æ®
	short	siEvationRate;			// 07 È¸ÇÇÀ²	-   "

	short	siMaxDurability;		// 08 ³»±¸·Â
	short	siPriceMultiply;		// 09 ¸Å¼ö°¡ ¹èÀ²
	short	siDefense;				// 10 ¹æ¾î·Â

	short	siDefenseRateDagger;	// 11 ´Ü°Ë¹æ¾î - ÆÛ¼¾Æ®
	short	siDefenseRateSword;		// 12 °Ë¹æ¾î - ÆÛ¼¾Æ®
	short	siDefenseRateBlow;		// 13 °It - ®
	short	siDefenseRateAxe;		// 14 µ³¢¹æ¾î - ÆÛ¼¾Æ®
	short	siDefenseRateSpear;		// 15 Ã¢¹æ¾î - ÆÛ¼¾Æ®
	short	siDefenseRateArrow;		// 16 È»ì¹æ¾î - ÆÛ¼¾Æ®

	BYTE	byDamageFire;			// 17 Ãß°¡µ¥¹ÌÁö - ºÒ
	BYTE	byDamageIce;			// 18 Ãß°¡µ¥¹ÌÁö - ¾óÀ½
	BYTE	byDamageThuner;			// 19 Ãß°¡µ¥¹ÌÁö - Àü°Ý
	BYTE	byDamagePoison;			// 20 Ãß°¡µ¥¹ÌÁö - µ¶

	BYTE	byStillHP;				// 21 HP Èí¼ö
	BYTE	byDamageMP;				// 22 MP Damage
	BYTE	byStillMP;				// 23 MP Èí¼ö
	BYTE	byReturnPhysicalDamage;	// 24 °¸®Å¸°Italy Italy»ç

	BYTE	bySoulBind;				// 25¼Ò¿ï ¹ÙÀÎµå - ÀÏ´ëÀÏ½Ã ¾ÆÀÌÅÛÀ» ¶³±¸´Â Á¤µµ - ÆÛ¼¾Æ® - ÇöÀç´Â ¾È¾´´Ù.

	short	siBonusStr;				// 26 Èû º¸³Ê½º
	short	siBonusSta;				// 27 Ã¼·Â º¸³Ê½º
	short	siBonusDex;				// 28 ¹ÎÃ¸¼º º¸³Ê½º
	short	siBonusInt;				// 29 Áö´É º¸³Ê½º
	short	siBonusMagicAttak;		// 30 ¸¶·Â º¸³Ê½º
	short	siBonusHP;				// 31 HP E
	short	siBonusMSP;				// 32 MSP º¸³Ê½º

	short	siRegistFire;			// 33 úÇ×
	short	siRegistIce;			// 34 ³Ã±â ÀúÇ×
	short	siRegistElec;			// 35 Àü±â ÀúÇ×
	short	siRegistMagic;			// 36 ý ÀúÇ×
	short	siRegistPoison;			// 37 µ¶ ÀúÇ×
	short	siRegistCurse;			// 38 AúAÖ AúÇ×

	DWORD	dwEffectID1;		// 39 ¸¶¹ý È¿°ú ID1
	DWORD	dwEffectID2;		// 40 ¸¶¹ý È¿°ú ID2

	short	siNeedLevel;			// 41 ¿ä±¸ ·¹º§ ÇÃ·¹ÀÌ¾îÀÇ iLeve..
	short	siNeedRank;				// 42 ¿ä±¸ ÀÛÀ§ ÇÃ·¹ÀÌ¾îÀÇ iRank..
	short	siNeedTitle;			// 43 ¿ä±¸ Å¸ÀÌÆ² ÇÃ·¹ÀÌ¾îÀÇ iTitle..
	short	siNeedStrength;			// 44 ¿ä±¸ Èû ÇÃ·¹ÀÌ¾îÀÇ iStrength..
	short	siNeedStamina;			// 45 ¿ä± ̧ Á¤·Â ÇÃ·1ÀÌ3⁄4îÀÇ iStamina.
	short	siNeedDexterity;		// 46 ¿ä±¸ ¹ÎÃ¸ ÇÃ·¹ÀÌ¾îÀÇ iDexterity..
	short	siNeedInteli;			// 47 ¿ä±¸ Áö´É ÇÃ·¹ÀÌ¾îÀÇ iIntelligence..
	short	siNeedMagicAttack;		// 48 ¿ä±¸ ¸¶·Â ÇÃ·¹ÀÌ¾îÀÇ iMagicAttak..
} TABLE_ITEM_EXT;

const int MAX_NPC_SHOP_ITEM = 30;
typedef struct __TABLE_NPC_SHOP
{
	DWORD		dwNPCID;
	std::string	szName;
	DWORD		dwItems[MAX_NPC_SHOP_ITEM];
} TABLE_NPC_SHOP;

enum e_ItemType { ITEM_TYPE_PLUG = 1, ITEM_TYPE_PART, ITEM_TYPE_ICONONLY, ITEM_TYPE_GOLD = 9, ITEM_TYPE_SONGPYUN = 10, ITEM_TYPE_UNKNOWN = 0xffffffff };

enum e_ItemPosition {
	ITEM_POS_DUAL = 0, ITEM_POS_RIGHTHAND, ITEM_POS_LEFTHAND, ITEM_POS_TWOHANDRIGHT, ITEM_POS_TWOHANDLEFT,
	ITEM_POS_UPPER = 5, ITEM_POS_LOWER, ITEM_POS_HEAD, ITEM_POS_GLOVES, ITEM_POS_SHOES,
	ITEM_POS_EAR = 10, ITEM_POS_NECK, ITEM_POS_FINGER, ITEM_POS_SHOULDER, ITEM_POS_BELT,
	ITEM_POS_INVENTORY = 15, ITEM_POS_GOLD = 16, ITEM_POS_SONGPYUN = 17,
	ITEM_POS_UNKNOWN = 0xffffffff
};

enum e_ItemSlot {
	ITEM_SLOT_EAR_RIGHT = 0, ITEM_SLOT_HEAD = 1, ITEM_SLOT_EAR_LEFT = 2,
	ITEM_SLOT_NECK = 3, ITEM_SLOT_UPPER = 4, ITEM_SLOT_SHOULDER = 5,
	ITEM_SLOT_HAND_RIGHT = 6, ITEM_SLOT_BELT = 7, ITEM_SLOT_HAND_LEFT = 8,
	ITEM_SLOT_RING_RIGHT = 9, ITEM_SLOT_LOWER = 10, ITEM_SLOT_RING_LEFT = 11,
	ITEM_SLOT_GLOVES = 12, ITEM_SLOT_SHOES = 13,
	ITEM_SLOT_COUNT = 14, ITEM_SLOT_UNKNOWN = 0xffffffff
};


typedef struct __TABLE_PLAYER_LOOKS // NPC, Mob ¸ð½à °üÇÑ ¸®¼Ò½º ·¹ÄÚµå...
{
	DWORD		dwID; // NPC °íÀ¯ ID
	std::string	szName; // Ä³¸¯ÅÍ ÀÌ¸§
	std::string	szJointFN; // °üÀý ÆÄÀÏ ÀÌ¸§
	std::string	szAniFN; // ¿¡´Ï¸ÞÀÌ¼Ç ÆÄÀÏ ÀÌ¸§
	std::string	szPartFNs[7]; // °¢ Character Part - »óÃ¼, ÇÏÃ¼, ¸Ó¸®, ÆÈ, ´Ù¸®, ¸Ó¸®Ä«¶ô, ¸ÁÅä
	std::string skipn3cskin0;
	std::string skipn3char0;
	std::string skipFXPlug;
	int         iunknownplooks0;

	int			iJointRH;			// ¿À¸¥¼Õ ³¡ °üÀý¹øÈ£
	int			iJointLH;			// ¿Þ¼Õ ³¡ °üÀý¹øÈ£
	int			iJointLH2;			// ¿Þ¼Õ ÆÈ¶Ò °üÀý¹øÈ£
	int			iJointCloak;		// ¸ÁÅä ºÙÀ» °üÀý ¹øÈ£

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
	int iunknownplooks1; // not sure if this one is supposed to be here
	int			iSndID_Reserved1;
	int iunknownplooks2; // not sure if this one is supposed to be here
	BYTE byunknownplooks1;
	BYTE byunknownplooks2;
	BYTE byunknownplooks3;
} TABLE_PLAYER;

typedef struct __TABLE_EXCHANGE_QUEST
{
	DWORD		dwID;					// Äù½ºÆ® ¹øÈ£.. 0
	DWORD		dwNpcNum;				// npc ¹øÈ£..	1
	std::string szDesc;					// ¼³¸í..		2
	int			iCondition0;			// Á¶°Ç 1.. 3
	int			iCondition1;			// Á¶°Ç 2.. 4
	int			iCondition2;			// Á¶°Ç 3.. 5
	int			iCondition3;			// Á¶°Ç 4.. 6
	int			iNeedGold;				// ¿ä±¸ ³ë¾Æ..	7
	BYTE		bNeedLevel;				// ¿ä±¸ ·¹º§ 8
	BYTE		bNeedClass;				// ¿ä±¸ Á÷¾÷	9
	BYTE		bNeedRank;				// ¿ä±¸ ÀÛÀ§	10
	BYTE		bNeedExtra1;			// ¿ä±¸ ¿©ºÐ1 11
	BYTE		bNeedExtra2;			// ¿ä±¸ ¿©ºÐ2 12
	BYTE		bCreatePercentage;		// »ý¼º È®·ü 13
	int			iArkTuarek;				// ¾ÆÅ© Åõ¾Æ·º.. 14
	int			iTuarek;				// Åõ¾Æ·º.. 15
	int			iRinkleTuarek;			// ¸µÅ¬ Åõ¾Æ·º.. 16
	int			iBabarian;				// ¹Ù¹Ù¸®¾È.. 17
	int			iMan;					// Ugh.. 18
	int			iWoman;					// ¿©AU.. 19
} TABLE_EXCHANGE_QUEST;

// 
// ¸¶¹ýÅ×ÀÌºí...

typedef struct __TABLE_UPC_SKILL
{
	DWORD		dwID;				// SKILL °íÀ¯ ID
	std::string	szEngName;			// ½ºÅ³ ¿µ¾î ÀÌ¸§
	std::string	szName;				// ½ºÅ³ ÇÑ±Û ÀÌ¸§
	std::string	szDesc;				// ½ºÅ³ ÀÌ¸§
	int			iSelfAnimID1;		// ½ÃÀüÀÚ m¿ÀÛ½ÃÀÛ
	int			iSelfAnimID2;		// ½ÃÀüÀÚ µ¿ÀÛ³¡

	int			idwTargetAnimID;	// Å¸°Ù µ¿ÀÛ
	int			iSelfFX1;			// ½ÃÀüÀÚ È¿°ú 1
	int			iSelfPart1;			// ½ÃÀüÀÚ È¿°ú ÀÏ¾î³ª´Â À§Ä¡ 1
	int			iSelfFX2;			// ½ÃÀüÀÚ È¿°ú 2
	int			iSelfPart2;			// ½ÃÀüÀÚ È¿°ú ÀÏ¾î³ª´Â À§Ä¡ 2
	int			iFlyingFX;			// ñÇà °ú
	int			iTargetFX;			// Å¸°Ù È¿°ú

	int			iTargetPart;		// È¿°ú°¡ ÀÏ¾î³¯ À§Ä¡.
	int			iTarget;			// Å¸°Ù
	int			iNeedLevel;			// ¿ä±¸ ·¹º§
	int			iNeedSkill;			// ¿ä±¸ ½ºÅ³
	int			iExhaustMSP;		// ¼Ò¸ð MSP

	int			iExhaustHP;			// ¼Ò¸ð HP
	DWORD		dwNeedItem;			// ¿ä±¸ ¾ÆÀÌÅÛ e_ItemClass ÂüÁ¶.. 10À» ³ª´«°ªÀÌ´Ù..
	DWORD		dwExhaustItem;
	int			iCastTime;			// Ä³½ºÆÃ ½Ã°£
	int			iReCastTime;		// ´Ù½Ã Ä³½ºÆÃÇÒ¶§±îÁö °É¸®´Â ½Ã°£.

	float fUnkown1;        // TODO: implement does this have to do with cooldown timers ??
	float fUnkown2;        // TODO: implement does this have to do with cooldown timers ??
	int			iPercentSuccess;	// ¼º°ø·ü
	DWORD		dw1stTableType;		// Ã¹¹øÂ° Å¸ÀÔ.
	DWORD		dw2ndTableType;		// µÎ¹øÂ° Å¸ÀÔ.
	int			iValidDist;			// À¯È¿°Å¸®
	int   iUnkown1;        // TODO: implement

} TABLE_UPC_ATTACK_B;

typedef struct __TABLE_UPC_SKILL_TYPE_1
{
	DWORD		dwID;			// SKILL °íÀ¯ ID
	int			iSuccessType;	// ¼º°øÅ¸ÀÔ.
	int			iSuccessRatio;	// ¼º°ø·ü
	int			iPower;			// °ø°Italy·Â
	int			iDelay;			// µô·¹ÀÌ
	int			iComboType;		// Ah
	int			iNumCombo;		// ö
	int			iComboDamage;	// ÄÞº¸´ë¹ÌÁö
	int			iValidAngle;	// °ø°Italy°æ
	int			iAct[3];
} TABLE_UPC_SKILL_TYPE_1;

typedef struct __TABLE_UPC_SKILL_TYPE_2
{
	DWORD		dwID;			// SKILL °íÀ¯ ID
	int			iSuccessType;	// ¼º°øÅ¸ÀÔ.
	int			iPower;			// °ø°Italy·Â
	int            skipall0iPowerPlus;     // adddamageplus
	int			iAddDist;		// °Å¸®Áõ°¡
	int			iNumArrow;		// It is»ì¿ä±¸¼ö
} TABLE_UPC_SKILL_TYPE_2;

typedef struct __TABLE_UPC_SKILL_TYPE_3
{
	DWORD		dwID;			// SKILL °íÀ¯ ID
	int   iRadius;  // TODO: Implement me!
	int			iDDType;
	int			iStartDamage;
	int			iDuraDamage;
	int			iDurationTime;	// Áö¼Ó½Ã°£
	int			iAttribute;
} TABLE_UPC_SKILL_TYPE_3;

typedef struct __TABLE_UPC_SKILL_TYPE_4
{
	DWORD		dwID;			// ÀÏ·Ã¹øÈ£
	int			iBuffType;		// ¹öÇÁÅ¸ÀÔ
	int   iRadius;          // TODO: implement
	int			iDuration;
	int			iAttackSpeed;	// °ø°Ý¼Óµµ
	int			iMoveSpeed;		// ÀÌµ¿¼Óµµ
	int			iAC;			// ¹æ¾î·Â
	int   iACPCT;           // TODO: implement
	int			iAttack;		// °ø°Italy·Â
	int   iMagicAttack;     // TODO: implement
	int			iMaxHP;			// MAXHP
	int   iMaxHPPct;        // TODO: implement
	int   iMaxMP;           // TODO: implement
	int   iMaxMPPct;        // TODO: implement
	int			iStr;			// It&#39;s
	int			iSta;			// Ã¼·Â
	int			iDex;			// ¹ÎÃ¸
	int			iInt;			// Áö´É
	int			iMAP;			// ¸¶·Â
	int			iFireResist;	// ºÒÀúÇ×
	int			iColdResist;	// ³Ã±âÀúÇ×
	int			iLightningResist;// Àü±âÀúÇ×
	int			iMagicResist;	// ¸¶¹ýÀúÇ×
	int			iDeseaseResist;	// ÀúÁÖÀúÇ×
	int			iPoisonResist;	// µ¶ÀúÇ×
	int   iScrollMoveSpeed; // TODO: implement
} TABLE_UPC_SKILL_TYPE_4;

typedef struct __TABLE_UPC_SKILL_TYPE_5
{
	DWORD		dwID;			// ÀÏ·Ã¹øÈ£
	DWORD		dwTarget;		// Å¸°Ù
	int			iSuccessRatio;	// ¼º°ø·ü
	int			iValidDist;		// À¯È¿°Å¸®
	int			iRadius;		// ¹Ý°æ
	float		fCastTime;		// Ä³½ºÆÃÅ¸ÀÓ
	float		fRecastTime;	// ¸®Ä³½ºÆÃÅ¸ÀÓ
	int			iDurationTime;	// Áö¼Ó½Ã°£
	DWORD		dwExhaustItem;	// ¼Ò¸ð¾ÆÀÌÅÛ
	DWORD		dwFX;			// ¸¶¹ýÈ¿°ú
} TABLE_UPC_SKILL_TYPE_5;

typedef struct __TABLE_UPC_SKILL_TYPE_6
{
	DWORD		dwID;			// ÀÏ·Ã¹øÈ£
	DWORD		dwTarget;		// Å¸°Ù
	int			iSuccessRatio;	// ¼º°ø·ü
	int			iValidDist;		// À¯È¿°Å¸®
	int			iRadius;		// ¹Ý°æ
	float		fCastTime;		// Ä³½ºÆÃÅ¸ÀÓ
	float		fRecastTime;	// ¸®Ä³½ºÆÃÅ¸ÀÓ
	int			iDurationTime;	// Áö¼Ó½Ã°£
	DWORD		dwExhaustItem;	// ¼Ò¸ð¾ÆÀÌÅÛ
	DWORD		dwFX;			// ¸¶¹ýÈ¿°ú
	DWORD		dwTranform;		// º¯½Å
} TABLE_UPC_SKILL_TYPE_6;

typedef struct __TABLE_UPC_SKILL_TYPE_7
{
	DWORD		dwID;			// ÀÏ·Ã¹øÈ£
	DWORD		dwTarget;		// Å¸°Ù
	DWORD		dwValidGroup;	// À¯È¿±×·ì
	int			iSuccessRatio;	// ¼º°ø·ü
	int			iValidDist;		// À¯È¿°Å¸®
	int			iRadius;		// ¹Ý°æ
	float		fCastTime;		// Ä³½ºÆÃÅ¸ÀÓ
	float		fRecastTime;	// ¸®Ä³½ºÆÃÅ¸ÀÓ
	int			iDurationTime;	// Áö¼Ó½Ã°£
	DWORD		dwExhaustItem;	// ¼Ò¸ð¾ÆÀÌÅÛ
	DWORD		dwFX;			// ¸¶¹ýÈ¿°ú
} TABLE_UPC_SKILL_TYPE_7;

typedef struct __TABLE_UPC_SKILL_TYPE_8
{
	DWORD		dwID;			// ÀÏ·Ã¹øÈ£
	DWORD		dwTarget;		// Å¸°Ù
	int			iRadius;		// ¹Ý°æ
	DWORD		dwWarpType;		// ¼ø°£ÀÌµ¿Å¸ÀÓ
	float		fRefillEXP;		// °æÇèÄ¡È¸º¹
	DWORD		dwZone1;		// Á¸³Ñ¹ö1
	DWORD		dwZone2;		// Á¸³Ñ¹ö2
	DWORD		dwZone3;		// Á¸³Ñ¹ö3
	DWORD		dwZone4;		// Á¸³Ñ¹ö4
	DWORD		dwZone5;		// Á¸³Ñ¹ö5
} TABLE_UPC_SKILL_TYPE_8;

typedef struct __TABLE_UPC_SKILL_TYPE_9
{
	DWORD		dwID;			// ÀÏ·Ã¹øÈ£
	DWORD		dwTarget;		// Å¸°Ù
	int			iSuccessRatio;	// ¼º°ø·ü
	int			iValidDist;		// À¯È¿°Å¸®
	int			iRadius;		// ¹Ý°æ
	float		fCastTime;		// Ä³½ºÆÃÅ¸ÀÓ
	float		fRecastTime;	// ¸®Ä³½ºÆÃÅ¸ÀÓ
	int			iDurationTime;	// Áö¼Ó½Ã°£
	DWORD		dwExhaustItem;	// ¼Ò¸ð¾ÆÀÌÅÛ
	DWORD		dwAttr;			// ¼Ó¼º
	int			iDamage;		// ´ë¹ÌÁö
} TABLE_UPC_SKILL_TYPE_9;

typedef struct __TABLE_UPC_SKILL_TYPE_10
{
	DWORD		dwID;			// ÀÏ·Ã¹øÈ£
	DWORD		dwTarget;		// Å¸°Ù
	int			iSuccessRatio;	// ¼º°ø·ü
	int			iValidDist;		// À¯È¿°Å¸®
	int			iRadius;		// ¹Ý°æ
	float		fCastTime;		// Ä³½ºÆÃÅ¸ÀÓ
	float		fRecastTime;	// ¸®Ä³½ºÆÃÅ¸ÀÓ
	DWORD		dwExhaustItem;	// ¼Ò¸ð¾ÆÀÌÅÛ
	DWORD		dwRemoveAttr;	// Á¦°Å¼Ó¼º
} TABLE_UPC_SKILL_TYPE_10;

// ¸¶¹ýÅ×ÀÌºí...
// 

typedef struct __TABLE_QUEST_MENU
{
	DWORD		dwID;		// ÀÏ·Ã¹øÈ£
	std::string szMenu;		// ¼±ÅÃ ¸Þ´º
} TABLE_QUEST_MENU;

typedef struct __TABLE_QUEST_TALK
{
	DWORD		dwID;		// ÀÏ·Ã¹øÈ£
	std::string szTalk;		// Äù½ºÆ® Áö¹®
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

const int MAX_ITEM_SLOT_OPC = 8; // Âø¿ë ¾ÆÀÌÅÛ - ´Ù¸¥ ÇÃ·¹ÀÌ¾î(NPC Æ÷ÇÔ) 0 ~ 4 »óÃ¼,ÇÏÃ¼,Çï¸ä,ÆÈ,¹ß 5 ¸ÁÅä 6 ¿À¸¥¼Õ 7 ¿Þ¼Õ
const int MAX_ITEM_INVENTORY = 28;		// ¼ÒÀ¯ ¾ÆÅÛ MAX (ÀÎº¥Åä¸®Ã¢)
const int MAX_ITEM_TRADE = 24;	// »óÀÎ°ú °Å·¡..
const int MAX_ITEM_TRADE_PAGE = 12;
const int MAX_ITEM_WARE_PAGE = 8;
const int MAX_ITEM_PER_TRADE = 12;	// °³ÀÎ°ú °Å·¡..
const int MAX_ITEM_BUNDLE_DROP_PIECE = 6;
const int MAX_ITEM_EX_RE_NPC = 4; // ±³È¯, ¼ö¸®Ã¢ NPC ¿µ¿ª..

const int MAX_SKILL_FROM_SERVER = 9;		// ¼¹ö¿¡°Ô¼ ¹Þ´Â Á¤º¸ ½½·Ô °¹¼ö..

const int MAX_SKILL_KIND_OF = 5;			// Base Skill 1°³, Àü¹® ½ºÅ³ 4°³..
const int MAX_SKILL_IN_PAGE = 8;			// ÇÑ ÆäÀÌÁö ³»ÀÇ ¾ÆÀÌÄÜ °¹¼ö..
const int MAX_SKILL_PAGE_NUM = 3;			// ½ºÅ³ ÇÑ°³°¡ °¡Áö´Â ÃÖ´ë ÆäÀÌÁö¼ö..


const int MAX_SKILL_HOTKEY_PAGE = 8;		// Hot Key ÃÑ ÆäÀÌÁö ¼ö..
const int MAX_SKILL_IN_HOTKEY = 8;			// Hot Keys

const int MAX_AVAILABLE_CHARACTER = 3;		// ÇÑ ¼¹ö´ç ¼±ÅÃÇÒ¼ö ÀÖ´Â ÖÖ´ë Ä³¸¯ÅÍ ¼ö..

// ½Î¿îµµ.... By ecli666
const int ID_SOUND_ITEM_ETC_IN_INVENTORY = 2000;
const int ID_SOUND_ITEM_IN_REPAIR = 2001;
const int ID_SOUND_ITEM_WEAPON_IN_INVENTORY = 2002;
const int ID_SOUND_ITEM_ARMOR_IN_INVENTORY = 2003;
const int ID_SOUND_GOLD_IN_INVENTORY = 3000;
const int ID_SOUND_SKILL_THROW_ARROW = 5500;
const int ID_SOUND_BGM_TOWN = 20000;
const int ID_SOUND_BGM_KA_BATTLE = 20002;
const int ID_SOUND_BGM_EL_BATTLE = 20003;
const int ID_SOUND_CHR_SELECT_ROTATE = 2501;

const float SOUND_RANGE_TO_SET = 10.0f;
const float SOUND_RANGE_TO_RELEASE = 20.0f;

const float STUN_TIME = 3.0f;

enum e_Behavior {
	BEHAVIOR_NOTHING = 0,
	BEHAVIOR_EXIT,			// ÇÁ·Î±×·¥ ³¡³»±â..
	BEHAVIOR_RESTART_GAME,	// °ÔÀÓ ´Ù½Ã ½ÃÀÛÇÏ±â(Ä³¸¯ÅÍ ¼±ÅÃ)
	BEHAVIOR_REGENERATION,	// ºÎÈ°
	BEHAVIOR_CANCEL,		// Ãë¼Ò.. ÇöÀç´Â ¾ÆÀÌÅÛ °Å·¡ ½ÅÃ»½Ã À¯Àú°¡ Ãë¼Ò¸¦ ´·¶À»¶§..

	BEHAVIOR_PARTY_PERMIT,	// »ó´ë¹æÀÌ ÆÄÆ¼°¡ÀÔ ¿ä»½Ã µé¾î°¡´Â°ÍÀ» Çã¶ô.
	BEHAVIOR_PARTY_DISBAND, // ÆÄÆ¼ Å»Åð..
	BEHAVIOR_FORCE_PERMIT,	// »ó´ë¹æÀÌ ºÎ´ë°¡ÀÔ ¿äÃ»½Ã µé¾î°¡´Â°ÍÀ» Çã¶ô.
	BEHAVIOR_FORCE_DISBAND, // ºÎ´ë Å»Åð..

	BEHAVIOR_REQUEST_BINDPOINT, // ¹ÙÀÎµù Æ÷ÀÎÆ®·Î ...

	BEHAVIOR_DELETE_CHR,

	BEHAVIOR_KNIGHTS_CREATE,
	BEHAVIOR_KNIGHTS_DESTROY, // ±â»ç´Ü ÇØÃ¼..
	BEHAVIOR_KNIGHTS_WITHDRAW, // ±â»ç´Ü ÇØÃ¼..

	BEHAVIOR_PERSONAL_TRADE_FMT_WAIT,	// °³ÀÎ°Å·¡.. ³»°¡ ½ÅÃ»ÇÑ °æ¿ì..
	BEHAVIOR_PERSONAL_TRADE_PERMIT,		// °³ÀÎ°Å·¡.. ³»°¡ ½ÅÃ»¹ÞÀº °æ¿ì..

	BEHAVIOR_MGAME_LOGIN,

	BEHAVIOR_CLAN_JOIN,
	BEHAVIOR_PARTY_BBS_REGISTER,		// ÆÄÆ¼ °Ô½ÃÆÇ¿¡ µî·Ï
	BEHAVIOR_PARTY_BBS_REGISTER_CANCEL, // ÆÄÆ¼ °Ô½ÃÆÇ¿¡ µî·Ï ÇØÁ¦

	BEHAVIOR_EXECUTE_OPTION,			// °ÔÀÓ Á¾·áÇÏ°í ¿É¼Ç ½ÇÇà..

	BEHAVIOR_UNKNOWN = 0xffffffff
};

enum e_SkillMagicTaget {
	SKILLMAGIC_TARGET_SELF = 1,				// ³ª ÀÚ½Å..
	SKILLMAGIC_TARGET_FRIEND_WITHME = 2,	// ³ª¸¦ Æ÷ÇÔÇÑ ¿ì¸®Æí(±¹°¡) Áß ÇÏ³ª ..
	SKILLMAGIC_TARGET_FRIEND_ONLY = 3,		// ³ª¸¦ »« ¿ì¸®Æí Áß ÇÏ³ª
	SKILLMAGIC_TARGET_PARTY = 4,			// ³ª¸¦ Æ÷ÇÔÇÑ ¿ì¸®ÆÄÆ¼ Áß ÇÏ³ª..
	SKILLMAGIC_TARGET_NPC_ONLY = 5,			// NPCÁß ÇÏ³ª.
	SKILLMAGIC_TARGET_PARTY_ALL = 6,		// ³ª¸¦ È£ÇÔÇÑ ¿ì¸®ÆÄÆ¼ ¸ðµÎ..
	SKILLMAGIC_TARGET_ENEMY_ONLY = 7,		// ¿ïÆíÀ» Á¦¿ÜÇÑ ¸ðµç ÀûÁß ÇÏ³ª(NPCÆ÷ÇÔ)
	SKILLMAGIC_TARGET_ALL = 8,				// °×»ó¿¡ Á¸ÀçÇÏ´Â ¸ðµç °ÍÁß ÇÏ³ª.

	SKILLMAGIC_TARGET_AREA_ENEMY = 10,		// ±× Áö¿ª¿¡ ÀÖ´Â Àûµé...
	SKILLMAGIC_TARGET_AREA_FRIEND = 11,		// ±× Áö¿ª¿¡ ÀÖ´Â ¿ïÆí.
	SKILLMAGIC_TARGET_AREA_ALL = 12,		// ±× Áö¿ª¿¡ ÀÖ´Â¸ðµÎµé.
	SKILLMAGIC_TARGET_AREA = 13,			// ³ª¸¦ Áß½ÉÀ¸·Î ÇÑ Áö¿ª
	SKILLMAGIC_TARGET_DEAD_FRIEND_ONLY = 25,	// Á×Àº »ç¶÷Áß ³ª¸¦ »« ¿ì¸®Æí Áß ÇÏ³ª.

	SKILLMAGIC_TARGET_UNKNOWN = 0xffffffff
};


// define fx...
typedef struct __TABLE_FX	// FX ®¼Ò½º ·¹ÄUµå...
{
	DWORD		dwID;		// °íÀ¯ ID
	std::string szName;    // TODO: implement
	std::string	szFN;		// file name
	DWORD		dwSoundID;	// È¿°ú¿¡ ¾²´Â »ç¿îµå ¾Æµð.
	BYTE		dwIdk;
} TABLE_FX;

const int	MAX_COMBO = 3;

const int	FXID_BLOOD = 10002;
const int	FXID_LEVELUP_KARUS = 10012;
const int	FXID_LEVELUP_ELMORAD = 10018;
const int	FXID_REGEN_ELMORAD = 10019;
const int	FXID_REGEN_KARUS = 10020;
const int	FXID_SWORD_FIRE_MAIN = 10021;
const int	FXID_SWORD_FIRE_TAIL = 10022;
const int	FXID_SWORD_FIRE_TARGET = 10031;
const int	FXID_SWORD_ICE_MAIN = 10023;
const int	FXID_SWORD_ICE_TAIL = 10024;
const int	FXID_SWORD_ICE_TARGET = 10032;
const int	FXID_SWORD_LIGHTNING_MAIN = 10025;
const int	FXID_SWORD_LIGHTNING_TAIL = 10026;
const int	FXID_SWORD_LIGHTNING_TARGET = 10033;
const int	FXID_SWORD_POISON_MAIN = 10027;
const int	FXID_SWORD_POISON_TAIL = 10028;
const int	FXID_SWORD_POISON_TARGET = 10034;
// const int	FXID_GROUND_TARGET = 10035;
const int	FXID_REGION_TARGET_EL_ROGUE = 10035;
const int	FXID_REGION_TARGET_EL_WIZARD = 10036;
const int	FXID_REGION_TARGET_EL_PRIEST = 10037;
const int	FXID_REGION_TARGET_KA_ROGUE = 10038;
const int	FXID_REGION_TARGET_KA_WIZARD = 10039;
const int	FXID_REGION_TARGET_KA_PRIEST = 10040;
const int	FXID_CLAN_RANK_1 = 10041;
const int	FXID_REGION_POISON = 10100;

// define skillmagic_type4_bufftype
enum e_SkillMagicType4 {
	BUFFTYPE_MAXHP = 1,				// MaxHPº¯È­..
	BUFFTYPE_AC = 2,				// ANDº¯È..
	BUFFTYPE_RESIZE = 3,			// Ä³¸¯ÅÍ Å©±â Á¶Á¤..
	BUFFTYPE_ATTACK = 4,			// °ø°Italy·Â..
	BUFFTYPE_ATTACKSPEED = 5,		// °ø°Ý½ºÇÇµå..
	BUFFTYPE_SPEED = 6,				// ÀÌµ¿¼Óµµ..
	BUFFTYPE_ABILITY = 7,			// ´Ù¼¸°¡Áö ´É·Â(str, sta, cha, dex int)
	BUFFTYPE_RESIST = 8,			// ´Ù¼¸°¡Áö ÀúÇ×·Â..
	BUFFTYPE_HITRATE_AVOIDRATE = 9,	// hitrate n avoidrate
	BUFFTYPE_TRANS = 10,			// º¯½Å, Åõ¸í..
	BUFFTYPE_SLEEP = 11,			// ÀáÀç¿ì±â..
	BUFFTYPE_EYE = 12				// ½Ã·Â°ü·Ã...
};

enum e_SkillMagicType3 {
	DDTYPE_TYPE3_DUR_OUR = 100,
	DDTYPE_TYPE3_DUR_ENEMY = 200
};



enum e_SkillMagicFailMsg {
	SKILLMAGIC_FAIL_CASTING = -100,		// Ä³½ºÆÃ ½ÇÆÐ..
	SKILLMAGIC_FAIL_KILLFLYING = -101,	// Flying°´Ã¼ Á×¿©..
	SKILLMAGIC_FAIL_ENDCOMBO = -102,	// combo µ¿ÀÛ ³¡³µ´Ù.
	SKILLMAGIC_FAIL_NOEFFECT = -103,	// Effecting...
	SKILLMAGIC_FAIL_ATTACKZERO = -104,	// Å¸°ÝÄ¡ 0...(mp´Â ¼Ò¸ðÇÏ°í Å¸°Ý½ÇÆÐ..)

	SKILLMAGIC_FAIL_UNKNOWN = 0xffffffff
};

enum e_ObjectType {
	OBJECT_TYPE_BINDPOINT,
	OBJECT_TYPE_DOOR_LEFTRIGHT,
	OBJECT_TYPE_DOOR_TOPDOWN,
	OBJECT_TYPE_LEVER_TOPDOWN,
	OBJECT_TYPE_FLAG,
	OBJECT_TYPE_WARP_POINT,
	OBJECT_TYPE_UNKNOWN = 0xffffffff
};

// definitions related clan....
const int	CLAN_LEVEL_LIMIT = 20;
const int	CLAN_COST = 500000;
const DWORD KNIGHTS_FONT_COLOR = 0xffff0000; // ±â»ç´Ü(Å¬·£)ÀÌ¸§ ÆùÆ® ÄÃ·¯..

enum e_Cursor {
	CURSOR_ATTACK,
	CURSOR_EL_NORMAL,
	CURSOR_EL_CLICK,
	CURSOR_KA_NORMAL,
	CURSOR_KA_CLICK,
	CURSOR_PRE_REPAIR,
	CURSOR_NOW_REPAIR,
	CURSOR_COUNT,
	CURSOR_UNKNOWN = 0xffffffff
};
