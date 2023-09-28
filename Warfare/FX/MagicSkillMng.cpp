#include "pch.h"

#include "GameProcMain.h"
#include "APISocket.h"
#include "PacketDef.h"
#include "PlayerMySelf.h"
#include "PlayerOtherMgr.h"

#include "N3FXMgr.h"

#include "UIStateBar.h"
#include "UIInventory.h"
#include "UIVarious.h"
#include "UIPartyOrForce.h"
#include "MagicSkillMng.h"
#include "N3SndObj.h"
#include "N3SndObjStream.h"
#include "N3ShapeExtra.h"

CMagicSkillMng::CMagicSkillMng()
{
	m_pGameProcMain = nullptr;
	m_dwRegionMagicState = 0;
	m_dwCastingStateNonAction = 0;
	m_fCastTimeNonAction = 0.0f;
	m_dwNonActionMagicID = 0;
	m_iNonActionMagicTarget = -1;
	m_fRecastTimeNonAction = 0.0f;

	m_iMyRegionTargetFXID = 0;

	Init();
}

CMagicSkillMng::CMagicSkillMng(CGameProcMain* pGameProcMain)
{
	m_pGameProcMain = pGameProcMain;
	m_dwRegionMagicState = 0;
	m_dwCastingStateNonAction = 0;
	m_fCastTimeNonAction = 0.0f;
	m_dwNonActionMagicID = 0;
	m_iNonActionMagicTarget = -1;
	m_fRecastTimeNonAction = 0.0f;

	m_iMyRegionTargetFXID = 0;

	Init();
}

void CMagicSkillMng::Init()
{
	m_pTbl_Type_1 = new CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_1>;
	m_pTbl_Type_1->LoadFromFile("Data\\Skill_Magic_1.tbl");

	m_pTbl_Type_2 = new CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_2>;
	m_pTbl_Type_2->LoadFromFile("Data\\Skill_Magic_2.tbl");

	m_pTbl_Type_3 = new CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_3>;
	m_pTbl_Type_3->LoadFromFile("Data\\Skill_Magic_3.tbl");

	m_pTbl_Type_4 = new CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_4>;
	m_pTbl_Type_4->LoadFromFile("Data\\Skill_Magic_4.tbl");

	// m_pTbl_Type_5 = new CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_5>;
	// m_pTbl_Type_5->LoadFromFile("Data\\Skill_Magic_5.tbl");

	// m_pTbl_Type_6 = new CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_6>;
	// m_pTbl_Type_6->LoadFromFile("Data\\Skill_Magic_6.tbl");

	// m_pTbl_Type_7 = new CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_7>;
	// m_pTbl_Type_7->LoadFromFile("Data\\Skill_Magic_7.tbl");

	// m_pTbl_Type_8 = new CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_8>;
	// m_pTbl_Type_8->LoadFromFile("Data\\Skill_Magic_8.tbl");

	// m_pTbl_Type_9 = new CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_9>;
	// m_pTbl_Type_9->LoadFromFile("Data\\Skill_Magic_9.tbl");
		
	// m_pTbl_Type_10 = new CN3TableBase<struct __TABLE_UPC_SKILL_TYPE_10>;
	// m_pTbl_Type_10->LoadFromFile("Data\\Skill_Magic_10.tbl");

	m_MySelf.clear();

	m_iTarget = -1;
	m_vTargetPos.Set(0,0,0);

	m_iComboSkillID = 0;
	m_iCurrStep = 0;
	m_iNumStep = 0;
	
	m_fRecastTime = 0.0f;
	m_fDelay = 0.0f;

	m_dwRegionMagicState = 0;

	InitType4();

	__InfoPlayerBase* pInfoBase = &(s_pPlayer->m_InfoBase);

	/*
	CLASS_KA_WARRIOR = 101, CLASS_KA_ROGUE, CLASS_KA_WIZARD, CLASS_KA_PRIEST, // Basic occupations up to here
	CLASS_KA_BERSERKER = 105, CLASS_KA_GUARDIAN, CLASS_KA_HUNTER = 107, CLASS_KA_PENETRATOR, 
	CLASS_KA_SORCERER = 109, CLASS_KA_NECROMANCER, CLASS_KA_SHAMAN = 111, CLASS_KA_DARKPRIEST, 
	
	CLASS_EL_WARRIOR = 201, CLASS_EL_ROGUE, CLASS_EL_WIZARD, CLASS_EL_PRIEST, // Default job so far
	CLASS_EL_BLADE = 205, CLASS_EL_PROTECTOR, CLASS_EL_RANGER = 207, CLASS_EL_ASSASIN,
	CLASS_EL_MAGE = 209, CLASS_EL_ENCHANTER, CLASS_EL_CLERIC = 211, CLASS_EL_DRUID,
	*/

	m_iMyRegionTargetFXID = 0;
	/*
	if( pInfoBase->eClass==CLASS_KA_ROGUE || pInfoBase->eClass==CLASS_KA_HUNTER || pInfoBase->eClass==CLASS_KA_PENETRATOR )
	{
		m_iMyRegionTargetFXID = FXID_REGION_TARGET_KA_ROGUE;
	}
	else if( pInfoBase->eClass==CLASS_KA_WIZARD || pInfoBase->eClass==CLASS_KA_SORCERER || pInfoBase->eClass==CLASS_KA_NECROMANCER )
	{
		m_iMyRegionTargetFXID = FXID_REGION_TARGET_KA_WIZARD;
	}
	else if( pInfoBase->eClass==CLASS_KA_PRIEST || pInfoBase->eClass==CLASS_KA_SHAMAN || pInfoBase->eClass==CLASS_KA_DARKPRIEST )
	{
		m_iMyRegionTargetFXID = FXID_REGION_TARGET_KA_PRIEST;
	}
	else if( pInfoBase->eClass==CLASS_EL_ROGUE || pInfoBase->eClass==CLASS_EL_RANGER || pInfoBase->eClass==CLASS_EL_ASSASIN )
	{
		m_iMyRegionTargetFXID = FXID_REGION_TARGET_EL_ROGUE;
	}
	else if( pInfoBase->eClass==CLASS_EL_WIZARD || pInfoBase->eClass==CLASS_EL_MAGE || pInfoBase->eClass==CLASS_EL_ENCHANTER )
	{
		m_iMyRegionTargetFXID = FXID_REGION_TARGET_EL_WIZARD;
	}
	else if( pInfoBase->eClass==CLASS_EL_PRIEST || pInfoBase->eClass==CLASS_EL_CLERIC || pInfoBase->eClass==CLASS_EL_DRUID )
	{
		m_iMyRegionTargetFXID = FXID_REGION_TARGET_EL_PRIEST;
	}
	*/
}

CMagicSkillMng::~CMagicSkillMng()
{
	m_pGameProcMain = nullptr;

	if(m_pTbl_Type_1) { delete m_pTbl_Type_1; m_pTbl_Type_1 = nullptr; }
	if(m_pTbl_Type_2) { delete m_pTbl_Type_2; m_pTbl_Type_2 = nullptr; }
	if(m_pTbl_Type_3) { delete m_pTbl_Type_3; m_pTbl_Type_3 = nullptr; }
	if(m_pTbl_Type_4) { delete m_pTbl_Type_4; m_pTbl_Type_4 = nullptr; }
	// if(m_pTbl_Type_5) { delete m_pTbl_Type_5; m_pTbl_Type_5 = NULL; }
	// if(m_pTbl_Type_6) { delete m_pTbl_Type_6; m_pTbl_Type_6 = NULL; }
	// if(m_pTbl_Type_7) { delete m_pTbl_Type_7; m_pTbl_Type_7 = NULL; }
	// if(m_pTbl_Type_8) { delete m_pTbl_Type_8; m_pTbl_Type_8 = NULL; }
	// if(m_pTbl_Type_9) { delete m_pTbl_Type_9; m_pTbl_Type_9 = NULL; }
	// if(m_pTbl_Type_10) { delete m_pTbl_Type_10; m_pTbl_Type_10 = NULL; }
}


//
//
//
bool CMagicSkillMng::IsCasting() const
{
	if(s_pPlayer->State() == PSA_SPELLMAGIC ||
		m_iCurrStep != 0 ||
		s_pPlayer->m_dwMagicID != 0xffffffff ||
		s_pPlayer->m_bStun == true ||
		m_fDelay > 0.0f ) return true;	
	return false;
}

bool CMagicSkillMng::CheckValidSkillMagic(__TABLE_UPC_SKILL* pSkill)
{
	const __InfoPlayerBase* pInfoBase = &(s_pPlayer->m_InfoBase);
	const __InfoPlayerMySelf* pInfoExt = &(s_pPlayer->m_InfoExt);

	const e_Class_Represent Class = CGameProcedure::GetRepresentClass(pInfoBase->eClass);
	if(pInfoExt->iMSP < pSkill->iExhaustMSP)
	{
		if(Class==CLASS_REPRESENT_PRIEST || Class==CLASS_REPRESENT_WIZARD)
		{
			return false;
		}
	}

	if(pSkill->dw1stTableType==1 || pSkill->dw1stTableType==2)
	{
		if(Class==CLASS_REPRESENT_WARRIOR || Class==CLASS_REPRESENT_ROGUE)
		{
			const int ExhaustSP = pInfoExt->iAttack * pSkill->iExhaustMSP / 100;
			if(pInfoExt->iMSP < ExhaustSP)
			{
				return false;
			}
		}
	}
	else if(pInfoExt->iMSP < pSkill->iExhaustMSP)
	{
		if(Class==CLASS_REPRESENT_WARRIOR || Class==CLASS_REPRESENT_ROGUE)
		{
			return false;
		}
	}

	const int LeftItem = s_pPlayer->ItemClass_LeftHand();
	const int RightItem = s_pPlayer->ItemClass_RightHand();

	if(pSkill->iNeedSkill==1055 || pSkill->iNeedSkill==2055)
	{
		if((LeftItem != ITEM_CLASS_SWORD && LeftItem != ITEM_CLASS_AXE && LeftItem != ITEM_CLASS_MACE ) ||
			(RightItem != ITEM_CLASS_SWORD && RightItem != ITEM_CLASS_AXE && RightItem != ITEM_CLASS_MACE) )
		{
			return false;
		}
	}
	else if(pSkill->iNeedSkill==1056 || pSkill->iNeedSkill==2056)
	{
		if(	RightItem != ITEM_CLASS_SWORD_2H && RightItem != ITEM_CLASS_AXE_2H &&
			RightItem != ITEM_CLASS_MACE_2H && RightItem != ITEM_CLASS_POLEARM )
		{
			return false;
		}
	}

	if(pInfoBase->iHP < pSkill->iExhaustHP) return false;

	const int LeftItem1 = LeftItem/10;
	const int RightItem1 = RightItem/10;
	
	if(pSkill->dwNeedItem!=0 && pSkill->dwNeedItem != LeftItem1 && pSkill->dwNeedItem != RightItem1)
	{
		return false;
	}
	if(pSkill->dwNeedItem==0 && (pSkill->dw1stTableType==1 || pSkill->dw2ndTableType==1))
	{
		if(LeftItem!=11 && (LeftItem1<1 || LeftItem1>5) && RightItem1!=11 && (RightItem1<1 || RightItem1>5) )
		{
			return false;
		}		
	}

	if(pSkill->dwExhaustItem>0)
	{
		const int NumItem = m_pGameProcMain->m_pUIInventory->GetCountInInvByID(pSkill->dwExhaustItem);
		
		if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)
		{
			const __TABLE_UPC_SKILL_TYPE_2* pType2 = m_pTbl_Type_2->Find(pSkill->dwID);
			if (!pType2) return false;
			if(NumItem < pType2->iNumArrow)
			{
				return false;
			}
		}
		else 
		{
			if(NumItem < 1) return false;
		}

		const __TABLE_ITEM_BASIC* pItem = nullptr;														// Item table structure pointer..
		const __TABLE_ITEM_EXT* pItemExt = nullptr;														// Item table structure pointer..

		pItem = s_pTbl_Items_Basic->Find(pSkill->dwExhaustItem/1000*1000);	// Get column data...
		if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
			pItemExt = s_pTbl_Items_Exts[pItem->byExtIndex]->Find(pSkill->dwExhaustItem%1000);	// Get column data...
		if (nullptr == pItem || nullptr == pItemExt )
		{
			__ASSERT(0, "NULL Item");
			CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", pSkill->dwExhaustItem);
			return false;	// If there are no items...
		}

		if (pItem->byAttachPoint == ITEM_LIMITED_EXHAUST)
		{
			// Race check...
			switch ( pItem->byNeedRace )
			{
				case 0:
					break;

				default:
					if ( pItem->byNeedRace != CGameBase::s_pPlayer->m_InfoBase.eRace )
						return false;
					break;
			}

			// Job check...
			if (pItem->byNeedClass != 0)
			{
				switch (pItem->byNeedClass)
				{
					case CLASS_KINDOF_WARRIOR:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_WARRIOR:
							case CLASS_KA_BERSERKER:
							case CLASS_KA_GUARDIAN:
							case CLASS_EL_WARRIOR:
							case CLASS_EL_BLADE:
							case CLASS_EL_PROTECTOR:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_ROGUE:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_ROGUE:
							case CLASS_KA_HUNTER:
							case CLASS_KA_PENETRATOR:
							case CLASS_EL_ROGUE:
							case CLASS_EL_RANGER:
							case CLASS_EL_ASSASIN:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_WIZARD:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_WIZARD:
							case CLASS_KA_SORCERER:
							case CLASS_KA_NECROMANCER:
							case CLASS_EL_WIZARD:
							case CLASS_EL_MAGE:
							case CLASS_EL_ENCHANTER:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_PRIEST:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_PRIEST:
							case CLASS_KA_SHAMAN:
							case CLASS_KA_DARKPRIEST:
							case CLASS_EL_PRIEST:
							case CLASS_EL_CLERIC:
							case CLASS_EL_DRUID:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_ATTACK_WARRIOR:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_BERSERKER:
							case CLASS_EL_BLADE:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_DEFEND_WARRIOR:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_GUARDIAN:
							case CLASS_EL_PROTECTOR:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_ARCHER:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_HUNTER:
							case CLASS_EL_RANGER:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_ASSASSIN:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_PENETRATOR:
							case CLASS_EL_ASSASIN:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_ATTACK_WIZARD:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_SORCERER:
							case CLASS_EL_MAGE:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_PET_WIZARD:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_NECROMANCER:
							case CLASS_EL_ENCHANTER:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_HEAL_PRIEST:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_SHAMAN:
							case CLASS_EL_CLERIC:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_CURSE_PRIEST:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_DARKPRIEST:
							case CLASS_EL_DRUID:
								break;
							default:
								return false;
						}
						break;

					default:
						if (CGameBase::s_pPlayer->m_InfoBase.eClass != pItem->byNeedClass)
							return false;
						break;
				}						
			}

			// Check the required level...
			if ( CGameBase::s_pPlayer->m_InfoBase.iLevel < pItem->cNeedLevel+pItemExt->siNeedLevel )
				return false;

			// Check required skills.
			int iNeedValue;
			iNeedValue = pItem->byNeedStrength;
			if (iNeedValue != 0)
				iNeedValue += pItemExt->siNeedStrength;
			if( iNeedValue > 0 && CGameBase::s_pPlayer->m_InfoExt.iStrength < iNeedValue )		
				return false;

			iNeedValue = pItem->byNeedStamina;
			if (iNeedValue != 0)
				iNeedValue += pItemExt->siNeedStamina;
			if( iNeedValue > 0 && CGameBase::s_pPlayer->m_InfoExt.iStamina < iNeedValue )		
				return false;

			iNeedValue = pItem->byNeedDexterity;
			if (iNeedValue != 0)
				iNeedValue += pItemExt->siNeedDexterity;
			if( iNeedValue > 0 && CGameBase::s_pPlayer->m_InfoExt.iDexterity < iNeedValue )		
				return false;

			iNeedValue = pItem->byNeedInteli;
			if (iNeedValue != 0)
				iNeedValue += pItemExt->siNeedInteli;
			if( iNeedValue > 0 && CGameBase::s_pPlayer->m_InfoExt.iIntelligence < iNeedValue )	
				return false;

			iNeedValue = pItem->byNeedMagicAttack;
			if (iNeedValue != 0)
				iNeedValue += pItemExt->siNeedMagicAttack;
			if( iNeedValue > 0 && CGameBase::s_pPlayer->m_InfoExt.iAttack < iNeedValue )				
				return false;
		}
	}

	return true;
}

#include "N3WorldManager.h"

bool CMagicSkillMng::CheckValidCondition(int iTargetID, __TABLE_UPC_SKILL* pSkill)
{
	__InfoPlayerBase* pInfoBase = &(s_pPlayer->m_InfoBase);
	__InfoPlayerMySelf* pInfoExt = &(s_pPlayer->m_InfoExt);

	// Find out if your skills are right for the job...
	e_Class_Represent Class = CGameProcedure::GetRepresentClass(pInfoBase->eClass);
	
	if(pSkill->iNeedSkill!=0)
	{
		if(Class == CLASS_REPRESENT_WARRIOR)
		{
			int NeedSkill = pSkill->iNeedSkill / 10;
			if(NeedSkill != CLASS_KA_WARRIOR && NeedSkill != CLASS_KA_BERSERKER && NeedSkill != CLASS_KA_GUARDIAN &&
				NeedSkill != CLASS_EL_WARRIOR && NeedSkill != CLASS_EL_BLADE && NeedSkill != CLASS_EL_PROTECTOR)
			{
				std::string buff;
				::_LoadStringFromResource(IDS_SKILL_FAIL_DIFFURENTCLASS, buff);
				m_pGameProcMain->MsgOutput(buff, 0xffffff00);
				return false;
			}
		}
		else if(Class == CLASS_REPRESENT_ROGUE)
		{
			int NeedSkill = pSkill->iNeedSkill / 10;
			if(NeedSkill != CLASS_KA_ROGUE && NeedSkill != CLASS_KA_HUNTER && NeedSkill != CLASS_KA_PENETRATOR &&
				NeedSkill != CLASS_EL_ROGUE && NeedSkill != CLASS_EL_RANGER && NeedSkill != CLASS_EL_ASSASIN)
			{
				std::string buff;
				::_LoadStringFromResource(IDS_SKILL_FAIL_DIFFURENTCLASS, buff);
				m_pGameProcMain->MsgOutput(buff, 0xffffff00);
				return false;
			}
		}
		else if(Class == CLASS_REPRESENT_WIZARD)
		{
			int NeedSkill = pSkill->iNeedSkill / 10;
			if(NeedSkill != CLASS_KA_WIZARD && NeedSkill != CLASS_KA_SORCERER && NeedSkill != CLASS_KA_NECROMANCER &&
				NeedSkill != CLASS_EL_WIZARD && NeedSkill != CLASS_EL_MAGE && NeedSkill != CLASS_EL_ENCHANTER)
			{
				std::string buff;
				::_LoadStringFromResource(IDS_SKILL_FAIL_DIFFURENTCLASS, buff);
				m_pGameProcMain->MsgOutput(buff, 0xffffff00);
				return false;
			}
		}
		else if(Class == CLASS_REPRESENT_PRIEST)
		{
			int NeedSkill = pSkill->iNeedSkill / 10;
			if(NeedSkill != CLASS_KA_PRIEST && NeedSkill != CLASS_KA_DARKPRIEST && NeedSkill != CLASS_KA_SHAMAN &&
				NeedSkill != CLASS_EL_PRIEST && NeedSkill != CLASS_EL_CLERIC && NeedSkill != CLASS_EL_DRUID)
			{
				std::string buff;
				::_LoadStringFromResource(IDS_SKILL_FAIL_DIFFURENTCLASS, buff);
				m_pGameProcMain->MsgOutput(buff, 0xffffff00);
				return false;
			}
		}
	}

	if(pInfoExt->iMSP < pSkill->iExhaustMSP)
	{
		if(Class==CLASS_REPRESENT_PRIEST || Class==CLASS_REPRESENT_WIZARD)
		{
			std::string buff;
			::_LoadStringFromResource(IDS_MSG_CASTING_FAIL_LACK_MP, buff);
			m_pGameProcMain->MsgOutput(buff, 0xffffff00);
			return false;
		}
	}

	if(pSkill->dw1stTableType==1 || pSkill->dw1stTableType==2)
	{
		if(Class==CLASS_REPRESENT_WARRIOR || Class==CLASS_REPRESENT_ROGUE)
		{
			int ExhaustSP = pInfoExt->iAttack * pSkill->iExhaustMSP / 100;
			if(pInfoExt->iMSP < ExhaustSP)
			{
				std::string buff;
				::_LoadStringFromResource(IDS_SKILL_FAIL_LACK_SP, buff);
				m_pGameProcMain->MsgOutput(buff, 0xffffff00);
				return false;
			}
		}
	}
	else if(pInfoExt->iMSP < pSkill->iExhaustMSP)
	{
		if(Class==CLASS_REPRESENT_WARRIOR || Class==CLASS_REPRESENT_ROGUE)
		{
			std::string buff;
			::_LoadStringFromResource(IDS_SKILL_FAIL_LACK_SP, buff);
			m_pGameProcMain->MsgOutput(buff, 0xffffff00);
			return false;
		}
	}

	int LeftItem = s_pPlayer->ItemClass_LeftHand();
	int RightItem = s_pPlayer->ItemClass_RightHand();

	if(pSkill->iNeedSkill==1055 || pSkill->iNeedSkill==2055)
	{
		if((LeftItem != ITEM_CLASS_SWORD && LeftItem != ITEM_CLASS_AXE && LeftItem != ITEM_CLASS_MACE ) ||
			(RightItem != ITEM_CLASS_SWORD && RightItem != ITEM_CLASS_AXE && RightItem != ITEM_CLASS_MACE) )
		{
			std::string buff;
			::_LoadStringFromResource(IDS_SKILL_FAIL_INVALID_ITEM, buff);
			m_pGameProcMain->MsgOutput(buff, 0xffffff00);
			return false;
		}
	}
	else if(pSkill->iNeedSkill==1056 || pSkill->iNeedSkill==2056)
	{
		if(	RightItem != ITEM_CLASS_SWORD_2H && RightItem != ITEM_CLASS_AXE_2H &&
			RightItem != ITEM_CLASS_MACE_2H && RightItem != ITEM_CLASS_POLEARM )
		{
			std::string buff;
			::_LoadStringFromResource(IDS_SKILL_FAIL_INVALID_ITEM, buff);
			m_pGameProcMain->MsgOutput(buff, 0xffffff00);
			return false;
		}
	}

	if(pInfoBase->iHP < pSkill->iExhaustHP)
	{
		std::string buff;
		::_LoadStringFromResource(IDS_SKILL_FAIL_LACK_HP, buff);
		m_pGameProcMain->MsgOutput(buff, 0xffffff00);
		return false;
	}

	int LeftItem1 = LeftItem/10;
	int RightItem1 = RightItem/10;
	
	if(pSkill->dwNeedItem!=0 && pSkill->dwNeedItem != LeftItem1 && pSkill->dwNeedItem != RightItem1)
	{
		std::string buff;
		::_LoadStringFromResource(IDS_SKILL_FAIL_INVALID_ITEM, buff);
		m_pGameProcMain->MsgOutput(buff, 0xffffff00);
		return false;
	}
	if(pSkill->dwNeedItem==0 && (pSkill->dw1stTableType==1 || pSkill->dw2ndTableType==1))
	{
		if(LeftItem!=11 && (LeftItem1<1 || LeftItem1>5) && RightItem1!=11 && (RightItem1<1 || RightItem1>5) )
		{
			std::string buff;
			::_LoadStringFromResource(IDS_SKILL_FAIL_INVALID_ITEM, buff);
			m_pGameProcMain->MsgOutput(buff, 0xffffff00);
			return false;
		}		
	}

	if(pSkill->dwExhaustItem>0)
	{
		int NumItem = m_pGameProcMain->m_pUIInventory->GetCountInInvByID(pSkill->dwExhaustItem);
		if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)
		{
			__TABLE_UPC_SKILL_TYPE_2* pType2 = m_pTbl_Type_2->Find(pSkill->dwID);
			if(NumItem < pType2->iNumArrow)
			{
				std::string szMsg; ::_LoadStringFromResource(IDS_SKILL_FAIL_LACK_ITEM, szMsg);
				m_pGameProcMain->MsgOutput(szMsg, 0xffffff00);
				return false;
			}
		}
		else 
		{
			if(NumItem < 1)
			{
				std::string szMsg; ::_LoadStringFromResource(IDS_SKILL_FAIL_LACK_ITEM, szMsg);
				m_pGameProcMain->MsgOutput(szMsg, 0xffffff00);
				return false;
			}
		}

		__TABLE_ITEM_BASIC* pItem = nullptr;														// Item table structure pointer..
		__TABLE_ITEM_EXT* pItemExt = nullptr;														// Item table structure pointer..

		pItem = s_pTbl_Items_Basic->Find(pSkill->dwExhaustItem/1000*1000);	// Get column data...
		if(pItem && pItem->byExtIndex >= 0 && pItem->byExtIndex < MAX_ITEM_EXTENSION)
			pItemExt = s_pTbl_Items_Exts[pItem->byExtIndex]->Find(pSkill->dwExhaustItem%1000);	// Get column data...
		if (nullptr == pItem || nullptr == pItemExt )
		{
			__ASSERT(0, "NULL Item");
			CLogWriter::Write("MyInfo - Inv - Unknown Item %d, IDNumber", pSkill->dwExhaustItem);
			return false;	// If there are no items...
		}

		if (pItem->byAttachPoint == ITEM_LIMITED_EXHAUST)
		{
			// Race check...
			switch ( pItem->byNeedRace )
			{
				case 0:
					break;

				default:
					if ( pItem->byNeedRace != CGameBase::s_pPlayer->m_InfoBase.eRace )
						return false;
					break;
			}

			// job check...
			if (pItem->byNeedClass != 0)
			{
				switch (pItem->byNeedClass)
				{
					case CLASS_KINDOF_WARRIOR:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_WARRIOR:
							case CLASS_KA_BERSERKER:
							case CLASS_KA_GUARDIAN:
							case CLASS_EL_WARRIOR:
							case CLASS_EL_BLADE:
							case CLASS_EL_PROTECTOR:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_ROGUE:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_ROGUE:
							case CLASS_KA_HUNTER:
							case CLASS_KA_PENETRATOR:
							case CLASS_EL_ROGUE:
							case CLASS_EL_RANGER:
							case CLASS_EL_ASSASIN:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_WIZARD:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_WIZARD:
							case CLASS_KA_SORCERER:
							case CLASS_KA_NECROMANCER:
							case CLASS_EL_WIZARD:
							case CLASS_EL_MAGE:
							case CLASS_EL_ENCHANTER:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_PRIEST:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_PRIEST:
							case CLASS_KA_SHAMAN:
							case CLASS_KA_DARKPRIEST:
							case CLASS_EL_PRIEST:
							case CLASS_EL_CLERIC:
							case CLASS_EL_DRUID:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_ATTACK_WARRIOR:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_BERSERKER:
							case CLASS_EL_BLADE:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_DEFEND_WARRIOR:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_GUARDIAN:
							case CLASS_EL_PROTECTOR:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_ARCHER:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_HUNTER:
							case CLASS_EL_RANGER:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_ASSASSIN:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_PENETRATOR:
							case CLASS_EL_ASSASIN:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_ATTACK_WIZARD:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_SORCERER:
							case CLASS_EL_MAGE:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_PET_WIZARD:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_NECROMANCER:
							case CLASS_EL_ENCHANTER:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_HEAL_PRIEST:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_SHAMAN:
							case CLASS_EL_CLERIC:
								break;
							default:
								return false;
						}
						break;

					case CLASS_KINDOF_CURSE_PRIEST:
						switch (CGameBase::s_pPlayer->m_InfoBase.eClass)
						{
							case CLASS_KA_DARKPRIEST:
							case CLASS_EL_DRUID:
								break;
							default:
								return false;
						}
						break;

					default:
						if (CGameBase::s_pPlayer->m_InfoBase.eClass != pItem->byNeedClass)
							return false;
						break;
				}						
			}

			// Check the required level...
			if ( CGameBase::s_pPlayer->m_InfoBase.iLevel < pItem->cNeedLevel+pItemExt->siNeedLevel )
				return false;

			// Check the skill level required.
			int iNeedValue;
			iNeedValue = pItem->byNeedStrength;
			if (iNeedValue != 0)
				iNeedValue += pItemExt->siNeedStrength;
			if( iNeedValue > 0 && CGameBase::s_pPlayer->m_InfoExt.iStrength < iNeedValue )		
				return false;

			iNeedValue = pItem->byNeedStamina;
			if (iNeedValue != 0)
				iNeedValue += pItemExt->siNeedStamina;
			if( iNeedValue > 0 && CGameBase::s_pPlayer->m_InfoExt.iStamina < iNeedValue )		
				return false;

			iNeedValue = pItem->byNeedDexterity;
			if (iNeedValue != 0)
				iNeedValue += pItemExt->siNeedDexterity;
			if( iNeedValue > 0 && CGameBase::s_pPlayer->m_InfoExt.iDexterity < iNeedValue )		
				return false;

			iNeedValue = pItem->byNeedInteli;
			if (iNeedValue != 0)
				iNeedValue += pItemExt->siNeedInteli;
			if( iNeedValue > 0 && CGameBase::s_pPlayer->m_InfoExt.iIntelligence < iNeedValue )	
				return false;

			iNeedValue = pItem->byNeedMagicAttack;
			if (iNeedValue != 0)
				iNeedValue += pItemExt->siNeedMagicAttack;
			if( iNeedValue > 0 && CGameBase::s_pPlayer->m_InfoExt.iAttack < iNeedValue )				
				return false;
		}
	}

	if((pSkill->dw1stTableType==3 || pSkill->dw2ndTableType==3) &&
		pSkill->iTarget==SKILLMAGIC_TARGET_SELF)
	{
		__TABLE_UPC_SKILL_TYPE_3* pType3 = m_pTbl_Type_3->Find(pSkill->dwID);
		if(!pType3) return false;

		int key = 0;
		if(pType3->iStartDamage>0 || (pType3->iStartDamage==0 && pType3->iDuraDamage>0) ) key = DDTYPE_TYPE3_DUR_OUR;
		else key = DDTYPE_TYPE3_DUR_ENEMY;

		key += pType3->iDDType;

		if(key==DDTYPE_TYPE3_DUR_OUR)
		{
			std::multimap<int, DWORD>::iterator it, itend;
			itend = m_ListBuffTypeID.end();
			it = m_ListBuffTypeID.find(key);
			if(it!=itend) return false;
		}
	}

	if( (pSkill->dw1stTableType==4 || pSkill->dw2ndTableType==4) && 
		( (pSkill->iTarget==SKILLMAGIC_TARGET_SELF) || (iTargetID==s_pPlayer->IDNumber()) ) )
	{
		__TABLE_UPC_SKILL_TYPE_4* pType4 = m_pTbl_Type_4->Find(pSkill->dwID);
		if(!pType4) return false;
		
		switch(pType4->iBuffType)
		{
		case BUFFTYPE_MAXHP:
			if(m_iMaxHP != 0) return false;
			break;
		case BUFFTYPE_AC:
			if(m_iAC != 0) return false;
			break;
		case BUFFTYPE_ATTACK:
			if(m_iAttack != 0) return false;
			break;
		case BUFFTYPE_ATTACKSPEED:
			if(m_fAttackSpeed != 1.0f) return false;
			break;
		case BUFFTYPE_SPEED:
			if(m_fSpeed != 1.0f) return false;
			break;
		case BUFFTYPE_ABILITY:
			if(	m_iStr != 0 || m_iSta != 0 || m_iDex != 0 || m_iInt != 0 || m_iMAP != 0) return false;
			break;
		case BUFFTYPE_RESIST:
			if(	m_iFireR != 0 || m_iColdR != 0 || m_iLightningR != 0 || m_iMagicR != 0 || m_iDeseaseR != 0 || m_iPoisonR != 0) return false;
			break;
		}
	}

	// 
	// Object check when using skill
	CPlayerBase* pTarget = m_pGameProcMain->CharacterGetByID(iTargetID, false);
	if(pTarget == nullptr) return true;

	__Vector3 vNormal, vMyPos, vGap, vDir, vSkillPos;

	vMyPos = s_pPlayer->Position();
	vMyPos.y += s_pPlayer->Height() / 2;

	vDir = (pTarget->Position() + pTarget->Height()/2) - vMyPos;
	vGap = vDir;
	vDir.Normalize();


	bool bColShape = ACT_WORLD->CheckCollisionWithShape(vMyPos, vDir, vGap.Magnitude(), &vSkillPos, &vNormal);

	switch(pSkill->iTarget)
	{
	case SKILLMAGIC_TARGET_SELF:
		{
			break;
		}
	case SKILLMAGIC_TARGET_FRIEND_WITHME:
		{
			// if(pTarget->m_InfoBase.eNation==pInfoBase->eNation)
			// {
				// if(bColShape)
				// {
					// std::string szMsg; ::_LoadStringFromResource(IDS_SKILL_FAIL_OBJECT_BLOCK, szMsg);
					// m_pGameProcMain->MsgOutput(szMsg, 0xffffff00);
					// return false;
				// }
			// }
			break;
		}
	case SKILLMAGIC_TARGET_FRIEND_ONLY:
		{
			// if(pTarget->m_InfoBase.eNation==pInfoBase->eNation)
			// {
				// if(bColShape)
				// {
					// std::string szMsg; ::_LoadStringFromResource(IDS_SKILL_FAIL_OBJECT_BLOCK, szMsg);
					// m_pGameProcMain->MsgOutput(szMsg, 0xffffff00);
					// return false;
				// }
			// }
			break;
		}
	case SKILLMAGIC_TARGET_PARTY:
		{
		// __InfoPartyOrForce* pInfo = (__InfoPartyOrForce*)m_pGameProcMain->m_pUIPartyOrForce->MemberInfoGetSelected();
		// if(!pInfo && iTargetID==-1)
			// return true;

			// int iMemberIndex = -1;
			// if( m_pGameProcMain->m_pUIPartyOrForce->MemberInfoGetByID(pTarget->IDNumber(), iMemberIndex) )
			// {
				// if(bColShape)
				// {
					// std::string szMsg; ::_LoadStringFromResource(IDS_SKILL_FAIL_OBJECT_BLOCK, szMsg);
					// m_pGameProcMain->MsgOutput(szMsg, 0xffffff00);
					// return false;
				// }
			// }
			break;
		}
	case SKILLMAGIC_TARGET_NPC_ONLY:
		{
			if(bColShape)
			{
				std::string szMsg; ::_LoadStringFromResource(IDS_SKILL_FAIL_OBJECT_BLOCK, szMsg);
				m_pGameProcMain->MsgOutput(szMsg, 0xffffff00);
				return false;
			}
			break;
		}
	case SKILLMAGIC_TARGET_PARTY_ALL:
		{
			break;
		}
	case SKILLMAGIC_TARGET_ENEMY_ONLY:
		{
			if(pTarget->m_InfoBase.eNation!=pInfoBase->eNation)
			{
				if(bColShape)
				{
					std::string szMsg; ::_LoadStringFromResource(IDS_SKILL_FAIL_OBJECT_BLOCK, szMsg);
					m_pGameProcMain->MsgOutput(szMsg, 0xffffff00);
					return false;
				}
			}
			break;
		}
	case SKILLMAGIC_TARGET_ALL:
		{
			if(bColShape)
			{
				std::string szMsg; ::_LoadStringFromResource(IDS_SKILL_FAIL_OBJECT_BLOCK, szMsg);
				m_pGameProcMain->MsgOutput(szMsg, 0xffffff00);
				return false;
			}
			break;
		}
	case SKILLMAGIC_TARGET_AREA:
	case SKILLMAGIC_TARGET_AREA_ENEMY:
	case SKILLMAGIC_TARGET_AREA_FRIEND:
	case SKILLMAGIC_TARGET_AREA_ALL:
		{
			break;
		}
	case SKILLMAGIC_TARGET_DEAD_FRIEND_ONLY:
		{
			if(pTarget->m_InfoBase.eNation==pInfoBase->eNation && pTarget->IsDead())
			{
				if(bColShape)
				{
					std::string szMsg; ::_LoadStringFromResource(IDS_SKILL_FAIL_OBJECT_BLOCK, szMsg);
					m_pGameProcMain->MsgOutput(szMsg, 0xffffff00);
					return false;
				}
			}
			break;
		}
	default:
		break;
	}

	// Object check when using skill
	// 

	return true;
}


//
// 
//
bool CMagicSkillMng::MsgSend_MagicProcess(int iTargetID, __TABLE_UPC_SKILL* pSkill)
{
	// if (m_fRecastTime > 0.0f) return;//Recast time is not yet..^^
	if(s_pPlayer->IsDead()) return false; // It's dead.. ^^

	// 
	// Check if the condition to use the skill is met...
	if(pSkill->iSelfAnimID1 >= 0)
	{
		if(IsCasting() || m_fRecastTime > 0.0f) return false;
	}
	else // Magic without casting action..
	{
		if( m_dwCastingStateNonAction != 0 || m_fRecastTimeNonAction > 0.0f ) return false;

		m_dwCastingStateNonAction = 0;
		m_fCastTimeNonAction = 0.0f;
		m_dwNonActionMagicID = 0;
		m_iNonActionMagicTarget = -1;
	}
		
	if(!pSkill) return false;
	if(!CheckValidCondition(iTargetID, pSkill)) return false;
	// TRACE("magic success state : %d time %.2f\n", s_pPlayer->State(), CN3Base::TimeGet());
	// End of the test to see if the conditions to use the skill are met...
	// 
	const __InfoPlayerBase* pInfoBase = &(s_pPlayer->m_InfoBase);
	__InfoPlayerMySelf* pInfoExt = &(s_pPlayer->m_InfoExt);
	CPlayerBase* pTarget = m_pGameProcMain->CharacterGetByID(iTargetID, false);

	// Reset area magic target..
	CGameProcedure::s_pFX->Stop(s_pPlayer->IDNumber(), s_pPlayer->IDNumber(), m_iMyRegionTargetFXID, m_iMyRegionTargetFXID, true);
	m_dwRegionMagicState = 0;	
	if(m_iMyRegionTargetFXID == 0)
	{
		if(pInfoBase->eNation == NATION_KARUS)
		{
			m_iMyRegionTargetFXID = FXID_REGION_TARGET_KA_WIZARD;
		}
		else if(pInfoBase->eNation == NATION_ELMORAD)
		{
			m_iMyRegionTargetFXID = FXID_REGION_TARGET_EL_WIZARD;
		}
	}
	// 

	// if(!pTarget) return false;//Return if temporarily dead.

	float fDist = s_pPlayer->Radius() + 1.0f; // Attack distance limit.
	if(pTarget) fDist += pTarget->Radius();

	switch(pSkill->iTarget)
	{
	case SKILLMAGIC_TARGET_SELF:
		{
			StartSkillMagicAtTargetPacket(pSkill, (short)s_pPlayer->IDNumber());
			return true;
		}
	case SKILLMAGIC_TARGET_FRIEND_WITHME:
		{
			if(!pTarget)
			{
				StartSkillMagicAtTargetPacket(pSkill, (short)s_pPlayer->IDNumber());
				return true;
			}
			else if(pTarget->m_InfoBase.eNation==pInfoBase->eNation)
			{
				if( !CheckValidDistance(pSkill, pTarget->Position(), fDist) ) return false;
				StartSkillMagicAtTargetPacket(pSkill, (short)pTarget->IDNumber());
				return true;
			}
			break;
		}
	case SKILLMAGIC_TARGET_FRIEND_ONLY:
		{
			if(pTarget && pTarget->m_InfoBase.eNation==pInfoBase->eNation)
			{
				if( !CheckValidDistance(pSkill, pTarget->Position(), fDist) ) return false;
				StartSkillMagicAtTargetPacket(pSkill, (short)pTarget->IDNumber());
				return true;
			}
			break;
		}
	case SKILLMAGIC_TARGET_PARTY:
		{
			const __InfoPartyOrForce* pInfo = (__InfoPartyOrForce*)m_pGameProcMain->m_pUIPartyOrForce->MemberInfoGetSelected();
			if(!pInfo && iTargetID==-1) pTarget = (CPlayerBase*)s_pPlayer;

			int iMemberIndex  = -1;
			if(pTarget && 
				( m_pGameProcMain->m_pUIPartyOrForce->MemberInfoGetByID(pTarget->IDNumber(), iMemberIndex) ||
				pTarget->IDNumber() == s_pPlayer->IDNumber() ) )
			{
				if( !CheckValidDistance(pSkill, pTarget->Position(), fDist) ) return false;
				StartSkillMagicAtTargetPacket(pSkill, (short)pTarget->IDNumber());
				return true;
			}
			else if(pInfo)	// When used on party members regardless of distance...
			{
				StartSkillMagicAtTargetPacket(pSkill, (short)pInfo->iID);
				return true;
			}
			break;
		}
	case SKILLMAGIC_TARGET_NPC_ONLY:
		{
			if(pTarget && s_pOPMgr->NPCGetByID(pTarget->IDNumber(), true))
			{
				if( !CheckValidDistance(pSkill, pTarget->Position(), fDist) ) return false;
				StartSkillMagicAtTargetPacket(pSkill, (short)pTarget->IDNumber());
				return true;
			}
			break;
		}
	case SKILLMAGIC_TARGET_PARTY_ALL:
		{
			StartSkillMagicAtPosPacket(pSkill, s_pPlayer->Position());
			return true;
		}
	case SKILLMAGIC_TARGET_ENEMY_ONLY:
		{
			if(pTarget && pTarget->m_InfoBase.eNation!=pInfoBase->eNation)
			{
				if( !CheckValidDistance(pSkill, pTarget->Position(), fDist) ) return false;
				StartSkillMagicAtTargetPacket(pSkill, (short)pTarget->IDNumber());
				// CLogWriter::Write("send msg : %.4f", CN3Base::TimeGet());
				// TRACE("send msg : %.4f\n", CN3Base::TimeGet());
				return true;
			}
			break;
		}
	case SKILLMAGIC_TARGET_ALL:
		{
			if(pTarget)
			{
				if( !CheckValidDistance(pSkill, pTarget->Position(), fDist) ) return false;
				StartSkillMagicAtTargetPacket(pSkill, (short)pTarget->IDNumber());
				return true;
			}
			break;
		}
	case SKILLMAGIC_TARGET_AREA:
		{
			StartSkillMagicAtPosPacket(pSkill, s_pPlayer->Position());
			return true;
		}
	case SKILLMAGIC_TARGET_AREA_ENEMY:
	case SKILLMAGIC_TARGET_AREA_FRIEND:
	case SKILLMAGIC_TARGET_AREA_ALL:
		{
			m_dwRegionMagicState = 1;
			m_dwRegionSkill = (*pSkill);
			// CGameProcedure::s_pFX->TriggerBundle(s_pPlayer->IDNumber(), 0, m_iMyRegionTargetFXID, m_pGameProcMain->m_vMouseLBClickedPos, m_iMyRegionTargetFXID);	//전격무기...
			CGameProcedure::s_pFX->TriggerBundle(s_pPlayer->IDNumber(), 0, m_iMyRegionTargetFXID, m_pGameProcMain->m_vMouseSkillPos, m_iMyRegionTargetFXID);	// Blitz weapon...
			return true;
		}
	case SKILLMAGIC_TARGET_DEAD_FRIEND_ONLY:
		{
			if(pTarget && pTarget->m_InfoBase.eNation==pInfoBase->eNation && pTarget->IsDead())
			{
				if( !CheckValidDistance(pSkill, pTarget->Position(), fDist) ) return false;
				StartSkillMagicAtTargetPacket(pSkill, (short)pTarget->IDNumber());
				return true;
			}
			break;
		}
	default:
		break;
	}

	return false;
}


bool CMagicSkillMng::CheckValidDistance(__TABLE_UPC_SKILL* pSkill, __Vector3 vTargetPos, float fTargetRadius)
{
	const float fDist = (vTargetPos - s_pPlayer->Position()).Magnitude(); // Find the attack distance.

	if(pSkill->iValidDist > 0 && fDist <= (pSkill->iValidDist+fTargetRadius + 1.0f)) return true;

	// type1
	if(pSkill->dw1stTableType==1 || pSkill->dw2ndTableType==1)
	{
		const __IconItemSkill* pItemIcon = m_pGameProcMain->m_pUIInventory->m_pMySlot[ITEM_SLOT_HAND_RIGHT];
		if(pItemIcon)
		{
			const float fValidDist = (pItemIcon->pItemBasic->siAttackRange/10.0f) + fTargetRadius + 1.0f;
			if(fValidDist >= fDist) return true;
		}
	}

	// When shooting an arrow...
	if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)
	{
		const __IconItemSkill* pItemIcon1 = m_pGameProcMain->m_pUIInventory->m_pMySlot[ITEM_SLOT_HAND_LEFT];
		const __IconItemSkill* pItemIcon2 = m_pGameProcMain->m_pUIInventory->m_pMySlot[ITEM_SLOT_HAND_RIGHT];
		float ItemDistance = 0.0f;

		if(pItemIcon2) ItemDistance = pItemIcon2->pItemBasic->siAttackRange/10.0f;
		if(pItemIcon1) ItemDistance = pItemIcon1->pItemBasic->siAttackRange/10.0f;

		float fValidDist = ItemDistance + fTargetRadius + 1.0f;
		const __TABLE_UPC_SKILL_TYPE_2* pType2 = m_pTbl_Type_2->Find(pSkill->dwID);
		fValidDist *= (float)pType2->iAddDist;
		fValidDist /= 100.0f;

		if(fValidDist >= fDist) return true;
	}

	char szBuff[80];
	std::string buff;
	::_LoadStringFromResource(IDS_SKILL_FAIL_SOFAR, buff);
	sprintf(szBuff, buff.c_str(), pSkill->szName.c_str());
	m_pGameProcMain->MsgOutput(szBuff, 0xffffff00);

	return false;
}


void CMagicSkillMng::StartSkillMagicAtPosPacket(__TABLE_UPC_SKILL* pSkill, __Vector3 vPos)
{
	if(!pSkill) return;
	const int SourceID = s_pPlayer->IDNumber();

	if(pSkill->iSelfAnimID1<0)
	{
		m_dwCastingStateNonAction = 1;
		m_fCastTimeNonAction = (float)pSkill->iCastTime / 10.0f;
		m_dwNonActionMagicID = pSkill->dwID;
		m_iNonActionMagicTarget = -1;
		m_fRecastTimeNonAction = (float)(pSkill->iReCastTime) / 10.0f;

		const int spart1 = pSkill->iSelfPart1 % 1000;
		int spart2 = pSkill->iSelfPart1 / 1000;
		spart2 = abs(spart2);

		CGameProcedure::s_pFX->TriggerBundle(SourceID, spart1, pSkill->iSelfFX1, SourceID, spart1, -1);
		if(spart2!=0) CGameProcedure::s_pFX->TriggerBundle(SourceID, spart2, pSkill->iSelfFX1, SourceID, spart2, -2);
		return;
	}
	m_pGameProcMain->CommandSitDown(false, false); // If you are sitting, raise me up..

	if(pSkill->iCastTime==0)
	{
		char szBuff[80];
		std::string buff;
		::_LoadStringFromResource(IDS_SKILL_USE, buff);
		sprintf(szBuff, buff.c_str(), pSkill->szName.c_str());
		m_pGameProcMain->MsgOutput(szBuff, 0xffffff00);

		BYTE byBuff[32];
		int iOffset=0;
		CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
		CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
		CAPISocket::MP_AddDword(byBuff, iOffset, (int)pSkill->dwID);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)SourceID);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)-1);

		CAPISocket::MP_AddShort(byBuff, iOffset, (short)vPos.x);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)vPos.y);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)vPos.z);
		
		CAPISocket::MP_AddShort(byBuff, iOffset, 0); 
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		
		CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
		return;
	}

	// 
	// 
	s_pPlayer->m_dwMagicID = pSkill->dwID;
	s_pPlayer->m_fCastingTime = 0.0f;
	m_iTarget = -1;
	m_vTargetPos = vPos;

	const int spart1 = pSkill->iSelfPart1 % 1000;
	int spart2 = pSkill->iSelfPart1 / 1000;
	spart2 = abs(spart2);

	CGameProcedure::s_pFX->TriggerBundle(SourceID, spart1, pSkill->iSelfFX1, SourceID, spart1, -1);
	if(spart2!=0) CGameProcedure::s_pFX->TriggerBundle(SourceID, spart2, pSkill->iSelfFX1, SourceID, spart2, -2);

	s_pPlayer->m_iIDTarget = -1;			
	s_pPlayer->m_iSkillStep = 1;
	// s_pPlayer->ActionMove(PSM_STOP);
	m_pGameProcMain->CommandMove(MD_STOP, true);

	s_pPlayer->m_iMagicAni = pSkill->iSelfAnimID1;
	if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)
	{
		const int LeftItem = s_pPlayer->ItemClass_LeftHand();
		const int RightItem = s_pPlayer->ItemClass_RightHand();
		if(RightItem == ITEM_CLASS_BOW_CROSS || LeftItem==ITEM_CLASS_BOW_CROSS)
		{
			s_pPlayer->m_iMagicAni = ANI_SHOOT_QUARREL_A;
		}
	}
	s_pPlayer->m_fCastFreezeTime = 10.0f;
	s_pPlayer->Action(PSA_SPELLMAGIC, false, nullptr);

	// 

	BYTE byBuff[32];
	int iOffset=0;
	CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
	CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_CASTING);
	CAPISocket::MP_AddDword(byBuff, iOffset, (int)pSkill->dwID);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)SourceID);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)-1);

	CAPISocket::MP_AddShort(byBuff, iOffset, (short)vPos.x);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)vPos.y);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)vPos.z);
	
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	
	CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..


	if(pSkill->iTarget == SKILLMAGIC_TARGET_ENEMY_ONLY) m_pGameProcMain->PlayBGM_Battle();
}

void CMagicSkillMng::StartSkillMagicAtTargetPacket(__TABLE_UPC_SKILL* pSkill, short TargetID)
{
	if(!pSkill) return;
	const int SourceID = s_pPlayer->IDNumber();
	if(pSkill->iSelfAnimID1<0)
	{
		m_dwCastingStateNonAction = 1;
		m_fCastTimeNonAction = (float)(pSkill->iCastTime) / 10.0f;
		m_dwNonActionMagicID = pSkill->dwID;
		m_iNonActionMagicTarget = TargetID;
		m_fRecastTimeNonAction = (float)(pSkill->iReCastTime) / 10.0f;

		const int spart1 = pSkill->iSelfPart1 % 1000;
		int spart2 = pSkill->iSelfPart1 / 1000;
		spart2 = abs(spart2);

		CGameProcedure::s_pFX->TriggerBundle(SourceID, spart1, pSkill->iSelfFX1, SourceID, spart1, -1);
		if(spart2!=0) CGameProcedure::s_pFX->TriggerBundle(SourceID, spart2, pSkill->iSelfFX1, SourceID, spart2, -2);
		return;
	}

	m_pGameProcMain->CommandSitDown(false, false); // If you are sitting, raise me up..
	
	if((pSkill->dw1stTableType==1 || pSkill->dw2ndTableType==1) && pSkill->iCastTime==0)
	{
		CPlayerBase* pTarget = m_pGameProcMain->CharacterGetByID(TargetID, true);
		if(!pTarget) return;

		// Go straight to skill..^^
		// Casting packet is not sent.. Effect packet is sent immediately..

		// Tech Animation Degas...=^^=
		// If it works, go with me..
		__TABLE_UPC_SKILL_TYPE_1* pType1 = m_pTbl_Type_1->Find(pSkill->dwID);
		if(!pType1) return;

		// Changes the black color.
		// D3DCOLOR crTrace = TraceColorGet(pSkill); // Depending on the type of skill, the color of the sword is determined.
		// s_pPlayer->PlugTraceColorRemake(crTrace); // Apply black color..

		s_pPlayer->RotateTo(pTarget);
		s_pPlayer->m_iSkillStep = 1;
		m_iCurrStep = 1;

		m_iNumStep = pType1->iNumCombo;
		m_fComboTime = 0.0f;
		m_iTarget = TargetID;
		m_iComboSkillID = pSkill->dwID;

		for(int i=0;i<pType1->iNumCombo;i++)
		{
			const bool bImmediately = ((0 == i) ? true : false); // Put the first one right in.
			s_pPlayer->AnimationAdd((e_Ani)pType1->iAct[i], bImmediately);
		}			
		
		char szBuff[80];
		std::string buff;
		::_LoadStringFromResource(IDS_SKILL_USE, buff);
		sprintf(szBuff, buff.c_str(), pSkill->szName.c_str());
		m_pGameProcMain->MsgOutput(szBuff, 0xffffff00);
		
		BYTE byBuff[32];
		int iOffset=0;
		CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
		CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
		CAPISocket::MP_AddDword(byBuff, iOffset, (int)pSkill->dwID);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)SourceID);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)TargetID);

		CAPISocket::MP_AddShort(byBuff, iOffset, 1);
		CAPISocket::MP_AddShort(byBuff, iOffset, 1);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		
		CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
		return;
	}

	if(pSkill->iCastTime==0)
	{
		char szBuff[80];
		std::string buff;
		::_LoadStringFromResource(IDS_SKILL_USE, buff);
		sprintf(szBuff, buff.c_str(), pSkill->szName.c_str());
		m_pGameProcMain->MsgOutput(szBuff, 0xffffff00);

		BYTE byBuff[32];
		int iOffset=0;
		CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
		CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
		CAPISocket::MP_AddDword(byBuff, iOffset, (int)pSkill->dwID);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)SourceID);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)TargetID);

		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		
		CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
		return;
	}

	// 
	// Mine is just processing while sending packets..
	s_pPlayer->m_dwMagicID = pSkill->dwID;
	s_pPlayer->m_fCastingTime = 0.0f;
	m_iTarget = TargetID;
	
	CPlayerBase* pTargetPlayer = m_pGameProcMain->CharacterGetByID(TargetID, false);

	const int spart1 = pSkill->iSelfPart1 % 1000;
	int spart2 = pSkill->iSelfPart1 / 1000;
	spart2 = abs(spart2);

	CGameProcedure::s_pFX->TriggerBundle(SourceID, spart1, pSkill->iSelfFX1, SourceID, spart1, -1);
	if(spart2!=0) CGameProcedure::s_pFX->TriggerBundle(SourceID, spart2, pSkill->iSelfFX1, SourceID, spart2, -2);

	s_pPlayer->m_iIDTarget = TargetID;			
	s_pPlayer->m_iSkillStep = 1;
	// s_pPlayer->ActionMove(PSM_STOP);
	m_pGameProcMain->CommandMove(MD_STOP, true);

	s_pPlayer->m_iMagicAni = pSkill->iSelfAnimID1;
	if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)
	{
		const int LeftItem = s_pPlayer->ItemClass_LeftHand();
		const int RightItem = s_pPlayer->ItemClass_RightHand();
		if(RightItem == ITEM_CLASS_BOW_CROSS || LeftItem==ITEM_CLASS_BOW_CROSS)
		{
			s_pPlayer->m_iMagicAni = ANI_SHOOT_QUARREL_A;
		}
	}
	s_pPlayer->m_fCastFreezeTime = 10.0f;
	s_pPlayer->Action(PSA_SPELLMAGIC, false, pTargetPlayer);

	BYTE byBuff[32];
	int iOffset=0;
	CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
	CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_CASTING);
	CAPISocket::MP_AddDword(byBuff, iOffset, (int)pSkill->dwID);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)SourceID);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)TargetID);

	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	
	CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
	
	if(pSkill->iTarget == SKILLMAGIC_TARGET_ENEMY_ONLY) m_pGameProcMain->PlayBGM_Battle();
	//
	// 
}


//
// If I'm casting, I need to process the casting, and also handle the flying effect..
//
void CMagicSkillMng::Tick()
{
	m_fRecastTime -= CN3Base::s_fSecPerFrm;
	m_fRecastTimeNonAction -= CN3Base::s_fSecPerFrm;
	m_fDelay -= CN3Base::s_fSecPerFrm;
	if(m_fDelay < 0.0f) m_fDelay = 0.0f;

	if(m_fRecastTimeNonAction < -30.0f) m_fRecastTimeNonAction = 0.0f;
	if(m_fRecastTime < -30.0f)
	{
		s_pPlayer->m_iSkillStep = 0;
		m_iCurrStep = 0;
		m_fRecastTime = 0.0f;
	}

	if(	m_iCurrStep!=0 )
	{
		m_fComboTime += CN3Base::s_fSecPerFrm;
		ProcessCombo();
	}
	ProcessCasting();
	// TRACE("skillmagic tick state : %d time %.2f\n", s_pPlayer->State(), CN3Base::TimeGet());

	if(m_dwRegionMagicState==2)
	{
		m_dwRegionMagicState = 0;
		CGameProcedure::s_pFX->Stop(s_pPlayer->IDNumber(), s_pPlayer->IDNumber(), m_iMyRegionTargetFXID, m_iMyRegionTargetFXID, true);
		// if( !CheckValidDistance(&m_dwRegionSkill, m_pGameProcMain->m_vMouseLBClickedPos, 0) ) return;
		// StartSkillMagicAtPosPacket(&m_dwRegionSkill, m_pGameProcMain->m_vMouseLBClickedPos);
		if( !CheckValidDistance(&m_dwRegionSkill, m_pGameProcMain->m_vMouseSkillPos, 0) ) return;
		StartSkillMagicAtPosPacket(&m_dwRegionSkill, m_pGameProcMain->m_vMouseSkillPos);		
	}

	if( m_dwCastingStateNonAction == 1)
	{
		m_fCastTimeNonAction -= CN3Base::s_fSecPerFrm;
		if(m_fCastTimeNonAction<0.0f)
		{
			const __TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(m_dwNonActionMagicID);
			if(!pSkill) 
			{
				m_dwCastingStateNonAction = 0;
				m_fCastTimeNonAction = 0.0f;
				m_dwNonActionMagicID = 0;
				m_iNonActionMagicTarget = -1;
				return;
			}

			BYTE byBuff[32];
			int iOffset=0;
			CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
			CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
			CAPISocket::MP_AddDword(byBuff, iOffset, (int)m_dwNonActionMagicID);
			CAPISocket::MP_AddShort(byBuff, iOffset, (short)s_pPlayer->IDNumber());
			CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_iNonActionMagicTarget);

			CAPISocket::MP_AddShort(byBuff, iOffset, 0);	// targetpos...
			CAPISocket::MP_AddShort(byBuff, iOffset, 0);
			CAPISocket::MP_AddShort(byBuff, iOffset, 0);
			
			CAPISocket::MP_AddShort(byBuff, iOffset, 0);
			CAPISocket::MP_AddShort(byBuff, iOffset, 0);
			CAPISocket::MP_AddShort(byBuff, iOffset, 0);
			
			CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

			m_dwCastingStateNonAction = 0;
			m_fCastTimeNonAction = 0.0f;
			m_dwNonActionMagicID = 0;
			m_iNonActionMagicTarget = -1;

			m_fRecastTimeNonAction = (float)(pSkill->iReCastTime) / 10.0f;

			char szBuff[80];
			std::string buff;
			::_LoadStringFromResource(IDS_SKILL_USE, buff);
			sprintf(szBuff, buff.c_str(), pSkill->szName.c_str());
			m_pGameProcMain->MsgOutput(szBuff, 0xffffff00);
		}
	}
	// if(s_pPlayer->State()==PSA_SPELLMAGIC)
}

void CMagicSkillMng::SuccessCast(__TABLE_UPC_SKILL* pSkill, CPlayerBase* pTarget)
{
	s_pPlayer->m_dwMagicID = 0xffffffff;
	s_pPlayer->m_fCastingTime = 0.0f;

	BYTE byBuff[32];
	int iOffset=0;
	CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);

	int idx = 0;
	if(pSkill->iFlyingFX==0) 
		CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
	else
	{
		CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_FLYING);
		if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)// shooting arrows...
		{
			int iNumArrow = 1;
			const __TABLE_UPC_SKILL_TYPE_2* pType2 = m_pTbl_Type_2->Find(pSkill->dwID);
			if(pType2) iNumArrow = pType2->iNumArrow;

			idx = AddIdx(pSkill->dwID, iNumArrow);
		}
		else idx = AddIdx(pSkill->dwID);				
	}

	if(pSkill->dw1stTableType==1 || pSkill->dw2ndTableType==1)
	{		
		// Go straight to skill..^^
		// Casting packet is not sent.. Effect packet is sent immediately..

		// Tech Animation Degas...=^^=
		// If it works, go with me..
		const __TABLE_UPC_SKILL_TYPE_1* pType1 = m_pTbl_Type_1->Find(pSkill->dwID);
		if(!pType1) return;

		s_pPlayer->RotateTo(pTarget);
		s_pPlayer->m_iSkillStep = 1;
		m_iCurrStep = 1;

		m_iNumStep = pType1->iNumCombo;

		for(int i=0;i<pType1->iNumCombo;i++)
		{
			const bool bImmediately = ((0 == i) ? true : false); // Put the first one in.
			s_pPlayer->AnimationAdd((const e_Ani)pType1->iAct[i], bImmediately);				
		}			
		
		char szBuff[80];
		std::string buff;
		::_LoadStringFromResource(IDS_SKILL_USE, buff);
		sprintf(szBuff, buff.c_str(), pSkill->szName.c_str());
		m_pGameProcMain->MsgOutput(szBuff, 0xffffff00);
		
		BYTE byBuff[32];
		int iOffset=0;
		// CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
		// CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
		CAPISocket::MP_AddDword(byBuff, iOffset, (int)pSkill->dwID);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)s_pPlayer->IDNumber());
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_iTarget);

		CAPISocket::MP_AddShort(byBuff, iOffset, 1);
		CAPISocket::MP_AddShort(byBuff, iOffset, 1);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		
		CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
	}
	else
	{
		char szBuff[80];
		std::string buff;
		::_LoadStringFromResource(IDS_SKILL_USE, buff);
		sprintf(szBuff, buff.c_str(), pSkill->szName.c_str());
		m_pGameProcMain->MsgOutput(szBuff, 0xffffff00);
		m_fRecastTime = (float)pSkill->iReCastTime / 10.0f;
		m_fDelay = 0.3f;
		s_pPlayer->m_iSkillStep = 0;

		CAPISocket::MP_AddDword(byBuff, iOffset, (int)pSkill->dwID);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)s_pPlayer->IDNumber());
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_iTarget);

		CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_vTargetPos.x);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_vTargetPos.y);
		CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_vTargetPos.z);

		CAPISocket::MP_AddShort(byBuff, iOffset, (short)idx);// If flying, idx...
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);
		CAPISocket::MP_AddShort(byBuff, iOffset, 0);

		CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..

		if(pSkill->iFlyingFX!=0)
		{
			// 
			// Dealing with flying...
			const int SourceID = s_pPlayer->IDNumber();
			
			s_pPlayer->m_iMagicAni = pSkill->iSelfAnimID2;
			if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)
			{
				m_fDelay = 0.1f;
				const int LeftItem = s_pPlayer->ItemClass_LeftHand();
				const int RightItem = s_pPlayer->ItemClass_RightHand();
				if(RightItem == ITEM_CLASS_BOW_CROSS || LeftItem==ITEM_CLASS_BOW_CROSS)
				{
					s_pPlayer->m_iMagicAni = ANI_SHOOT_QUARREL_B;
				}
			}
			s_pPlayer->m_fCastFreezeTime = 0.0f;
			s_pPlayer->Action(PSA_SPELLMAGIC, false);
			// s_pPlayer->Action(PSA_BASIC, false);
			s_pPlayer->m_iSkillStep = 0;

			CGameProcedure::s_pFX->Stop(SourceID, SourceID, pSkill->iSelfFX1, -1, true);
			CGameProcedure::s_pFX->Stop(SourceID, SourceID, pSkill->iSelfFX1, -2, true);

			if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)// shooting arrows...
			{
				short Data[6] = { m_vTargetPos.x, m_vTargetPos.y, m_vTargetPos.z, idx, 0, 0 };
				FlyingType2(pSkill, SourceID, m_iTarget, Data);
				return;
			}

			const CPlayerBase* pTarget = m_pGameProcMain->CharacterGetByID(m_iTarget, false);
			const int spart1 = pSkill->iSelfPart1 % 1000;
			if(!pTarget)
			{
				__Vector3 vTargetPos = s_pPlayer->Position() + s_pPlayer->Direction();
				CGameProcedure::s_pFX->TriggerBundle(SourceID, spart1, pSkill->iFlyingFX, m_vTargetPos,
															idx, FX_BUNDLE_MOVE_DIR_FIXEDTARGET);
			}
			else
			{
				CGameProcedure::s_pFX->TriggerBundle(SourceID, spart1, pSkill->iFlyingFX, m_iTarget,
												0, idx, FX_BUNDLE_MOVE_DIR_FLEXABLETARGET);
			}
			//
			// 
		}
		else
		{
			if(pSkill->iSelfAnimID2>=0)
			{
				s_pPlayer->m_iMagicAni = pSkill->iSelfAnimID2;
				s_pPlayer->m_fCastFreezeTime = 0.0f;
				s_pPlayer->Action(PSA_SPELLMAGIC, false);
				// s_pPlayer->Action(PSA_BASIC, false);
			}
			s_pPlayer->m_iSkillStep = 0;
		}

		if(pSkill->iSelfFX2>0)
			CGameProcedure::s_pFX->TriggerBundle(s_pPlayer->IDNumber(), pSkill->iSelfPart2, pSkill->iSelfFX2, s_pPlayer->IDNumber(), pSkill->iSelfPart2, -3);
	}
}

void CMagicSkillMng::FailCast(__TABLE_UPC_SKILL* pSkill)
{
	s_pPlayer->m_dwMagicID = 0xffffffff;
	s_pPlayer->m_fCastingTime = 0.0f;
	s_pPlayer->m_iSkillStep = 0;

	BYTE byBuff[32];
	int iOffset=0;
	CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
	CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_FAIL);
	CAPISocket::MP_AddDword(byBuff, iOffset, (int)pSkill->dwID);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)s_pPlayer->IDNumber());
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)s_pPlayer->IDNumber());

	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);

	CAPISocket::MP_AddShort(byBuff, iOffset, (short)SKILLMAGIC_FAIL_CASTING);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);

	CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
}

void CMagicSkillMng::ProcessCasting()
{
	// casting process...
	if(s_pPlayer->m_dwMagicID != 0xffffffff)
	{
		__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(s_pPlayer->m_dwMagicID);

		CPlayerBase* pTarget = m_pGameProcMain->CharacterGetByID(m_iTarget, true);
		if(pTarget) s_pPlayer->RotateTo(pTarget); // Once it turns towards the target.

		// Casting completed successfully...
		const float fCastingTime = ((float)pSkill->iCastTime) / 10.0f * s_pPlayer->m_fAttackDelta;

		if(pSkill)
		{
			bool bSuccess = false;
			if( s_pPlayer->m_fCastingTime >= fCastingTime && s_pPlayer->State()==PSA_SPELLMAGIC && s_pPlayer->StateMove()==PSM_STOP)
			{
				SuccessCast(pSkill, pTarget);
				bSuccess = true;
			}
			
			// Cast failed...
			if(bSuccess == false && (s_pPlayer->State()!=PSA_SPELLMAGIC || s_pPlayer->StateMove()!=PSM_STOP))
			{
				FailCast(pSkill);
			}
		}
		else s_pPlayer->m_dwMagicID = 0xffffffff;
	}	
}

void CMagicSkillMng::ProcessCombo()
{
	// If one of the combo moves is over...=^^=
	if(m_fComboTime > (s_pPlayer->m_fAttackDelta * 1.2f)) // s_pPlayer->IsAnimationChange())
	{
		if(m_iCurrStep==m_iNumStep)// The combo attack is over.
		{
			const __TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(m_iComboSkillID);
			if(pSkill) m_fRecastTime = (float)pSkill->iReCastTime / 10.0f;
			m_iCurrStep = -1;
			s_pPlayer->m_iSkillStep = -1;
			m_iNumStep = 0;			
		}	
		m_iCurrStep++;
		s_pPlayer->m_iSkillStep++;
		m_fComboTime = 0.0f;
	}
}

void CMagicSkillMng::MobCasting(__TABLE_UPC_SKILL* pSkill, int iSourceID)
{
	if(!pSkill) return;

	// Casting completed successfully...
	BYTE byBuff[32];
	int iOffset=0;
	CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);

	int idx = 0;
	if(pSkill->iFlyingFX==0) 
		CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
	else
	{
		CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_FLYING);
		if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)// shooting arrows...
		{
			int iNumArrow = 1;
			const __TABLE_UPC_SKILL_TYPE_2* pType2 = m_pTbl_Type_2->Find(pSkill->dwID);
			if(pType2) iNumArrow = pType2->iNumArrow;

			idx = AddIdx(pSkill->dwID, iNumArrow);
		}
		else idx = AddIdx(pSkill->dwID);
	}

	CAPISocket::MP_AddDword(byBuff, iOffset, (int)pSkill->dwID);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)iSourceID);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)s_pPlayer->IDNumber());

	CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_vTargetPos.x);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_vTargetPos.y);
	CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_vTargetPos.z);

	CAPISocket::MP_AddShort(byBuff, iOffset, (short)idx);// If flying, idx...
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);
	CAPISocket::MP_AddShort(byBuff, iOffset, 0);

	CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
}


void CMagicSkillMng::MsgRecv_Casting(DataPack* pDataPack, int& iOffset)
{
	// common.....//////////////////////////////////////////////////////////////
	// 
	const DWORD dwMagicID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	const int	iSourceID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const int	iTargetID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	short Data[6];
	for(int i=0;i<6;i++)
	{
		Data[i] = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	}

	// if(iSourceID<0) return;
	if(iSourceID<0 || iSourceID==s_pPlayer->IDNumber()) return;
	// 
	// common.....//////////////////////////////////////////////////////////////
	
	__Vector3	vTargetPos;
	if(iTargetID == -1) vTargetPos.Set((float)Data[0], (float)Data[1], (float)Data[2]);
	
	CPlayerBase* pPlayer = m_pGameProcMain->CharacterGetByID(iSourceID, true);
	if(!pPlayer) return;

	__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(dwMagicID);
	if(!pSkill) return;

	// When I write...
	if(iSourceID==s_pPlayer->IDNumber())
	{
		m_pGameProcMain->CommandSitDown(false, false); // If you are sitting, raise me up..
		
		s_pPlayer->m_dwMagicID = dwMagicID;
		s_pPlayer->m_fCastingTime = 0.0f;
		m_iTarget = iTargetID;
		m_vTargetPos = vTargetPos;
	}
	
	// When a monster shoots at me...
	if( s_pOPMgr->NPCGetByID(iSourceID, true) )
	{
		pPlayer->RotateTo((CPlayerBase*)s_pPlayer); // looking at this
		pPlayer->m_iIDTarget = iTargetID;
		pPlayer->ActionMove(PSM_STOP);
		pPlayer->m_iMagicAni = pSkill->iSelfAnimID1;
		pPlayer->Action(PSA_ATTACK, false, (CPlayerBase*)s_pPlayer);

		if(iTargetID==s_pPlayer->IDNumber()) 
			MobCasting(pSkill, iSourceID);
		return;
	}

	CPlayerBase* pTargetPlayer = m_pGameProcMain->CharacterGetByID(iTargetID, false);

	const int spart1 = pSkill->iSelfPart1 % 1000;
	int spart2 = pSkill->iSelfPart1 / 1000;
	spart2 = abs(spart2);

	CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iSelfFX1, iSourceID, spart1, -1);
	if(spart2!=0) CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart2, pSkill->iSelfFX1, iSourceID, spart2, -2);

	pPlayer->m_iIDTarget = iTargetID;			
	pPlayer->m_iSkillStep = 1;
	pPlayer->ActionMove(PSM_STOP);
	pPlayer->m_iMagicAni = pSkill->iSelfAnimID1;
	if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)
	{
		const int LeftItem = pPlayer->ItemClass_LeftHand();
		const int RightItem = pPlayer->ItemClass_RightHand();
		if(RightItem == ITEM_CLASS_BOW_CROSS || LeftItem==ITEM_CLASS_BOW_CROSS)
		{
			pPlayer->m_iMagicAni = ANI_SHOOT_QUARREL_A;
		}
	}
	pPlayer->m_fCastFreezeTime = 10.0f;
	pPlayer->Action(PSA_SPELLMAGIC, false, pTargetPlayer);

	// CLogWriter::Write("send casting : %.4f", CN3Base::TimeGet());
	// TRACE("recv casting : %.4f\n", CN3Base::TimeGet());

	if(pSkill->iTarget == SKILLMAGIC_TARGET_ENEMY_ONLY) m_pGameProcMain->PlayBGM_Battle();
}

void CMagicSkillMng::MsgRecv_Flying(DataPack* pDataPack, int& iOffset)
{
	// common.....//////////////////////////////////////////////////////////////
	// 
	const DWORD dwMagicID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	const int	iSourceID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const int	iTargetID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	short Data[6];
	for(int i=0;i<6;i++)
	{
		Data[i] = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	}

	// if(iSourceID<0) return;
	if(iSourceID<0 || iSourceID==s_pPlayer->IDNumber()) return;

	CPlayerBase* pPlayer = m_pGameProcMain->CharacterGetByID(iSourceID, true);
	if(!pPlayer) return;

	__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(dwMagicID);
	if(!pSkill) return;
	//
	// common.....//////////////////////////////////////////////////////////////

	// TRACE("recv flying : %.4f\n", CN3Base::TimeGet());

	if(pPlayer && pPlayer->State()==PSA_SPELLMAGIC)
	{
		pPlayer->m_iMagicAni = pSkill->iSelfAnimID2;
		if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)
		{
			const int LeftItem = pPlayer->ItemClass_LeftHand();
			const int RightItem = pPlayer->ItemClass_RightHand();
			if(RightItem == ITEM_CLASS_BOW_CROSS || LeftItem==ITEM_CLASS_BOW_CROSS)
			{
				pPlayer->m_iMagicAni = ANI_SHOOT_QUARREL_B;
			}
		}
		pPlayer->m_fCastFreezeTime = 0.0f;
		pPlayer->Action(PSA_SPELLMAGIC, false);
		pPlayer->m_iSkillStep = 0;
	}

	CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -1, true);
	CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -2, true);

	if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)// shooting arrows...
	{
		FlyingType2(pSkill, iSourceID, iTargetID, Data);
		return;
	}

	const CPlayerBase* pTarget = m_pGameProcMain->CharacterGetByID(iTargetID, false);

	const int spart1 = pSkill->iSelfPart1 % 1000;
	
	if(!pTarget)
	{
		if(pPlayer)
		{
			const __Vector3 vTargetPos = pPlayer->Position() + pPlayer->Direction();
			CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, vTargetPos,
													Data[3], FX_BUNDLE_MOVE_DIR_FIXEDTARGET);
		}
	}
	else
	{
		CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, iTargetID,
										0/*pSkill->iTargetPart*/, Data[3], FX_BUNDLE_MOVE_DIR_FLEXABLETARGET);
	}
}

void CMagicSkillMng::MsgRecv_Effecting(DataPack* pDataPack, int& iOffset)
{
	// common.....//////////////////////////////////////////////////////////////
	//
	const DWORD dwMagicID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	const int	iSourceID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const int	iTargetID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	short Data[6];
	for(int i=0;i<6;i++)
	{
		Data[i] = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	}

	CPlayerBase* pPlayer = m_pGameProcMain->CharacterGetByID(iSourceID, false);
	if(!pPlayer) return;

	const __TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(dwMagicID);
	if(!pSkill) return;
	//
	// common.....//////////////////////////////////////////////////////////////
		
	if(pPlayer && iSourceID!=s_pPlayer->IDNumber() && pPlayer->State()==PSA_SPELLMAGIC)
	{
		pPlayer->m_iMagicAni = pSkill->iSelfAnimID2;	// An arrow action...
		pPlayer->m_fCastFreezeTime = 0.0f;
		pPlayer->Action(PSA_SPELLMAGIC, false);
		pPlayer->m_iSkillStep = 0;
	}

	CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -1, true);
	CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -2, true);

	if(pSkill->dw1stTableType==1 || pSkill->dw2ndTableType==1)	// If you are type 1, it's just a skill.. There's also a combo.. You need to manage it a bit special..
	{
		if(!EffectingType1(dwMagicID, iSourceID, iTargetID, Data)) return;
	}

	if(pSkill->dw1stTableType==4 || pSkill->dw2ndTableType==4)
	{
		if(iTargetID==s_pPlayer->IDNumber()) EffectingType4(dwMagicID);
	}

	if(pSkill->dw1stTableType==3 || pSkill->dw2ndTableType==3)
	{
		if(iTargetID==s_pPlayer->IDNumber()) EffectingType3(dwMagicID);
	}

	if(pSkill->iFlyingFX!=0 && 
		(pSkill->iTarget < SKILLMAGIC_TARGET_AREA_ENEMY || pSkill->iTarget > SKILLMAGIC_TARGET_AREA) ) return;	// In the case of magic with flying, the effect is processed in fail.
	
	if(iTargetID==-1)
	{
		const __Vector3 vTargetPos(Data[0],Data[1],Data[2]);
		CGameProcedure::s_pFX->TriggerBundle(iSourceID, 0, pSkill->iTargetFX, vTargetPos);
	}
	else
		CGameProcedure::s_pFX->TriggerBundle(iSourceID, 0, pSkill->iTargetFX, iTargetID, pSkill->iTargetPart);
}

void CMagicSkillMng::MsgRecv_Fail(DataPack* pDataPack, int& iOffset)
{
	// common.....//////////////////////////////////////////////////////////////
	//
	const DWORD dwMagicID = CAPISocket::Parse_GetDword(pDataPack->m_pData, iOffset);
	const int	iSourceID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	const int	iTargetID = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);

	short Data[6];
	for(int i=0;i<6;i++)
	{
		Data[i] = CAPISocket::Parse_GetShort(pDataPack->m_pData, iOffset);
	}
	CPlayerBase* pPlayer = m_pGameProcMain->CharacterGetByID(iSourceID, false);
	if(!pPlayer) return;

	const __TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(dwMagicID);
	if(!pSkill) return;
	//
	// common.....//////////////////////////////////////////////////////////////
		
	if(pPlayer && iSourceID != s_pPlayer->IDNumber() && pPlayer->State()==PSA_SPELLMAGIC) 
	{
		pPlayer->m_iMagicAni = pSkill->iSelfAnimID2;
		pPlayer->m_fCastFreezeTime = 0.0f;
		pPlayer->Action(PSA_SPELLMAGIC, false);
		pPlayer->m_iSkillStep = 0;

		CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -1, true);
		CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -2, true);
	}

	if(Data[3]==SKILLMAGIC_FAIL_ATTACKZERO)
	{
		CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -1, true);
		CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -2, true);
		
		if(pPlayer) pPlayer->m_iSkillStep = 0;

		if(iSourceID == s_pPlayer->IDNumber())
		{
			s_pPlayer->m_dwMagicID = 0xffffffff;
			m_pGameProcMain->CommandSitDown(false, false); // If you're sitting down, raise me up.

			std::string buff;
			::_LoadStringFromResource(IDS_MSG_FMT_TARGET_ATTACK_FAILED, buff);
			char szBuff[256] = "";
			sprintf(szBuff, buff.c_str(), pSkill->szName.c_str());
			m_pGameProcMain->MsgOutput(szBuff, 0xffff3b3b);			
		}
		return;
	}

	if(Data[3]==SKILLMAGIC_FAIL_NOEFFECT)
	{
		CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -1, true);
		CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -2, true);
		
		if(pPlayer)
		{
			pPlayer->m_iSkillStep = 0;
			pPlayer->Action(PSA_BASIC, true);
		}

		if(iSourceID == s_pPlayer->IDNumber())
		{
			s_pPlayer->m_dwMagicID = 0xffffffff;
			m_pGameProcMain->CommandSitDown(false, false); // If you are sitting, raise me up..

			std::string buff;
			::_LoadStringFromResource(IDS_SKILL_FAIL_EFFECTING, buff);
			char szBuff[256] = "";
			sprintf(szBuff, buff.c_str(), pSkill->szName.c_str());
			m_pGameProcMain->MsgOutput(szBuff, 0xffff3b3b);			
		}
		return;
	}	

	if(Data[3]==SKILLMAGIC_FAIL_CASTING)// It's a casting failure.
	{
		CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -1, true);
		CGameProcedure::s_pFX->Stop(iSourceID, iSourceID, pSkill->iSelfFX1, -2, true);
		
		if(pPlayer)
		{
			pPlayer->m_iSkillStep = 0;
			pPlayer->Action(PSA_BASIC, true);
		}

		if(iSourceID == s_pPlayer->IDNumber())
		{
			s_pPlayer->m_dwMagicID = 0xffffffff;
			m_pGameProcMain->CommandSitDown(false, false); // If you are sitting, raise me up..
	
			std::string buff;
			::_LoadStringFromResource(IDS_SKILL_FAIL_CASTING, buff);
			m_pGameProcMain->MsgOutput(buff, 0xffff3b3b);
		}
		return;
	}

	if(Data[3]==SKILLMAGIC_FAIL_KILLFLYING)// Kill the flying effect.. and use the target effect in place..
	{
		if(iSourceID == s_pPlayer->IDNumber() ||
			((iTargetID==s_pPlayer->IDNumber() && s_pOPMgr->NPCGetByID(iSourceID, false)!= nullptr)))
		{
			RemoveIdx(Data[4]);
		}
		else CGameProcedure::s_pFX->Stop(iSourceID, iTargetID, pSkill->iFlyingFX, Data[4]);

		if(iTargetID==s_pPlayer->IDNumber())
		{
			if(pSkill->dw1stTableType==2 || pSkill->dw2ndTableType==2)
			{
				StopCastingByRatio();
			}
			else if(pSkill->dw1stTableType==3 || pSkill->dw2ndTableType==3)
			{
				__TABLE_UPC_SKILL_TYPE_3* pType3 = m_pTbl_Type_3->Find(dwMagicID);

				if(pType3->iAttribute==3)
				{
					StopCastingByRatio();
					StunMySelf(pType3);
				}
			}
		}

		const CPlayerBase* pTarget = m_pGameProcMain->CharacterGetByID(iTargetID, false);
		if(pTarget)
		{
			CGameProcedure::s_pFX->TriggerBundle(iSourceID, pSkill->iTargetPart, pSkill->iTargetFX, iTargetID, pSkill->iTargetPart);
			
		}
		else
		{
			const __Vector3 TargetPos(Data[0], Data[1], Data[2]);
			CGameProcedure::s_pFX->TriggerBundle(iSourceID, 0, pSkill->iTargetFX, TargetPos);
		}		
		return;
	}

	if(Data[3]==SKILLMAGIC_FAIL_ENDCOMBO)// combo is over.
	{
		if(pPlayer) pPlayer->m_iSkillStep = 0;
		return;
	}

	// There must be something else that needs a stop..
	// Then...
	// CGameProcedure::s_pFX->Stop(iSourceID, iTargetID, pSkill->iSelfFX, 0);
}


// release type4..
void CMagicSkillMng::MsgRecv_BuffType(DataPack* pDataPack, int& iOffset)
{
	const int iBuffType = CAPISocket::Parse_GetByte(pDataPack->m_pData, iOffset);
	__InfoPlayerBase* pInfoBase = &(s_pPlayer->m_InfoBase);
	__InfoPlayerMySelf* pInfoExt = &(s_pPlayer->m_InfoExt);

	const std::multimap<int, DWORD>::iterator it = m_ListBuffTypeID.find(iBuffType);
	if(it!= m_ListBuffTypeID.end())
	{
		__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(it->second);
		m_pGameProcMain->m_pUIStateBarAndMiniMap->DelMagic(pSkill);
		m_ListBuffTypeID.erase(it);
	}

	switch(iBuffType)
	{
	case BUFFTYPE_MAXHP:
		if(pInfoBase->iHP < pInfoBase->iHPMax)
		{
			pInfoBase->iHPMax -= m_iMaxHP;
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateHP(pInfoBase->iHP, pInfoBase->iHPMax);
		}
		m_iMaxHP = 0;
		break;
	case BUFFTYPE_AC:
		pInfoExt->iGuard_Delta -= m_iAC;
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateGuardPoint(pInfoExt->iGuard, pInfoExt->iGuard_Delta);
		m_iAC = 0;
		break;
	case BUFFTYPE_ATTACK:
		if(m_iAttack) pInfoExt->iAttack_Delta -= m_iAttack;
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateAttackPoint(pInfoExt->iAttack, pInfoExt->iAttack_Delta);
		m_iAttack = 0;
		break;
	case BUFFTYPE_ATTACKSPEED:
		s_pPlayer->m_fAttackDelta /= m_fAttackSpeed; 
		m_fAttackSpeed = 1.0f;
		break;
	case BUFFTYPE_SPEED:
		s_pPlayer->m_fMoveDelta /= m_fSpeed;
		m_fSpeed = 1.0f;
		// TRACE("Release sprint. MoveDelta = %f\n", s_pPlayer->m_fMoveDelta);
		break;
	case BUFFTYPE_ABILITY:
		pInfoExt->iStrength_Delta -= m_iStr;
		pInfoExt->iStamina_Delta -= m_iSta;
		pInfoExt->iDexterity_Delta -= m_iDex;
		pInfoExt->iIntelligence_Delta -= m_iInt;
		pInfoExt->iMagicAttak_Delta -= m_iMAP;

		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateStrength(pInfoExt->iStrength, pInfoExt->iStrength_Delta);
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateStamina(pInfoExt->iStamina, pInfoExt->iStamina_Delta);
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateDexterity(pInfoExt->iDexterity, pInfoExt->iDexterity_Delta);
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateIntelligence(pInfoExt->iIntelligence, pInfoExt->iIntelligence_Delta);
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateMagicAttak(pInfoExt->iMagicAttak, pInfoExt->iMagicAttak_Delta);

		m_iStr = 0;
		m_iSta = 0;
		m_iDex = 0;
		m_iInt = 0;
		m_iMAP = 0;
		break;
	case BUFFTYPE_RESIST:
		pInfoExt->iRegistFire_Delta -= m_iFireR;
		pInfoExt->iRegistCold_Delta -= m_iColdR;
		pInfoExt->iRegistLight_Delta -= m_iLightningR;
		pInfoExt->iRegistMagic_Delta -= m_iMagicR;
		pInfoExt->iRegistCurse_Delta -= m_iDeseaseR;
		pInfoExt->iRegistPoison_Delta -= m_iPoisonR;

		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistFire(pInfoExt->iRegistFire, pInfoExt->iRegistFire_Delta);
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistCold(pInfoExt->iRegistCold, pInfoExt->iRegistCold_Delta);
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistLight(pInfoExt->iRegistLight, pInfoExt->iRegistLight_Delta);
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistMagic(pInfoExt->iRegistMagic, pInfoExt->iRegistMagic_Delta);
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistCurse(pInfoExt->iRegistCurse, pInfoExt->iRegistCurse_Delta);
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistPoison(pInfoExt->iRegistPoison, pInfoExt->iRegistPoison_Delta);

		m_iFireR = 0;
		m_iColdR = 0;
		m_iLightningR = 0;
		m_iMagicR = 0;
		m_iDeseaseR = 0;
		m_iPoisonR = 0;
		break;
	}
}


//
//
//
void CMagicSkillMng::FlyingType2(__TABLE_UPC_SKILL* pSkill, int iSourceID, int iTargetID, short* pData)
{
	CPlayerBase* pPlayer = m_pGameProcMain->CharacterGetByID(iSourceID, true);
	if(!pPlayer) return;
	__TABLE_UPC_SKILL_TYPE_2* pType2 = m_pTbl_Type_2->Find(pSkill->dwID);
	if(!pType2) return;

	int LeftItem = pPlayer->ItemClass_LeftHand()/10;
	int RightItem = pPlayer->ItemClass_RightHand()/10;

	if(LeftItem == (ITEM_CLASS_BOW/10))// bow..
	{
		auto vPlayerPos = pPlayer->Position();
		CN3Base::s_SndMgr.PlayOnceAndRelease(ID_SOUND_SKILL_THROW_ARROW, &vPlayerPos);
	}
	else if(RightItem == (ITEM_CLASS_JAVELIN/10))// It's a javelin...pla
	{
	}
	
	int spart1 = pSkill->iSelfPart1 % 1000;

	__Vector3 vTargetPos(0,0,0);

	CPlayerBase* pTarget = m_pGameProcMain->CharacterGetByID(iTargetID, false);
	if(!pTarget)
	{
		vTargetPos = pPlayer->Position() + pPlayer->Direction();
		CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, vTargetPos+pPlayer->Position(), pData[3], pType2->iSuccessType);
		
		int NumArrow = (pType2->iNumArrow - 1) >>1;
		int idx = pData[3];
		
		__Vector3 vTargetPos2 = vTargetPos - pPlayer->Position();
		__Vector3 vTargetPos3;
		__Matrix44 mtx;
		for(int i=1;i<=NumArrow;i++)
		{
			float fAng = (__PI * (float)i) / 12.0f;	// Divide by 15...
				
			mtx.Identity();
			mtx.RotationY(-fAng);
			vTargetPos3 = vTargetPos2*mtx;
			CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, vTargetPos3+pPlayer->Position(), idx++, pType2->iSuccessType);

			mtx.Identity();
			mtx.RotationY(fAng);
			vTargetPos3 = vTargetPos2*mtx;
			CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, vTargetPos3+pPlayer->Position(), idx++, pType2->iSuccessType);
		}
	}
	else
	{
		vTargetPos = pTarget->Center();

		if(pType2->iSuccessType == FX_BUNDLE_MOVE_DIR_FIXEDTARGET)
		{
			CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, vTargetPos, pData[3], FX_BUNDLE_MOVE_DIR_FIXEDTARGET);
					
			int NumArrow = (pType2->iNumArrow - 1) >>1;

			int idx = pData[3];

			__Vector3 vTargetPos2 = vTargetPos - pPlayer->Position();
			__Vector3 vTargetPos3;
			__Matrix44 mtx;
			for(int i=1;i<=NumArrow;i++)
			{
				float fAng = (__PI * (float)i) / 12.0f;	// Divide by 15...
					
				mtx.Identity();
				mtx.RotationY(-fAng);
				vTargetPos3 = vTargetPos2*mtx;
				CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, vTargetPos3+pPlayer->Position(), idx++, FX_BUNDLE_MOVE_DIR_FIXEDTARGET);
				
				mtx.Identity();
				mtx.RotationY(fAng);
				vTargetPos3 = vTargetPos2*mtx;
				CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, vTargetPos3+pPlayer->Position(), idx++, FX_BUNDLE_MOVE_DIR_FIXEDTARGET);
			}
		}

		else 
		{
			CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, iTargetID, pSkill->iTargetPart, pData[3], pType2->iSuccessType);
					
			int NumArrow = (pType2->iNumArrow - 1) >>1;

			int idx = pData[3];

			__Vector3 vTargetPos2 = vTargetPos - pPlayer->Position();
			__Vector3 vTargetPos3;
			__Matrix44 mtx;
			for(int i=1;i<=NumArrow;i++)
			{
				float fAng = (__PI * (float)i) / 12.0f;	// Divide by 15...
					
				mtx.Identity();
				mtx.RotationY(-fAng);
				vTargetPos3 = vTargetPos2*mtx;
				CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, vTargetPos3+pPlayer->Position(), idx++, pType2->iSuccessType);


				mtx.Identity();
				mtx.RotationY(fAng);
				vTargetPos3 = vTargetPos2*mtx;
				CGameProcedure::s_pFX->TriggerBundle(iSourceID, spart1, pSkill->iFlyingFX, vTargetPos3+pPlayer->Position(), idx++, pType2->iSuccessType);
			}
		}
	}

	// __TABLE_UPC_SKILL_TYPE_2* pType2 = m_pTbl_Type_2->Find(pSkill->dwID);
	// if(pType2)	CGameProcedure::s_pFX->Trigger(iSourceID, spart1, pSkill->iFlyingFX, iTargetID, 	pSkill->iTargetPart, pData[3], pType2->iSuccessType);
}


//
//
//

bool CMagicSkillMng::EffectingType1(DWORD dwMagicID, int iSourceID, int iTargetID, short* pData)
{
	CPlayerBase* pTarget = m_pGameProcMain->CharacterGetByID(iTargetID, false);
	if(pTarget)
	{
		if(iSourceID != s_pPlayer->IDNumber()) // When I use my skills...
		{
			const __TABLE_UPC_SKILL_TYPE_1* pType1 = m_pTbl_Type_1->Find(dwMagicID);
			if(pType1)
			{
				CPlayerBase* pPlayer = m_pGameProcMain->CharacterGetByID(iSourceID, true);
				__ASSERT(pPlayer, "NULL Player Pointer!!");
				if(pPlayer)
				{
					// Changes the black color.
					// __TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(dwMagicID);
					// D3DCOLOR crTrace = TraceColorGet(pSkill); // Depending on the type of skill, the color of the sword is determined.
					// pPlayer->PlugTraceColorRemake(crTrace); // Apply black color..

					pPlayer->RotateTo(pTarget);
					pPlayer->m_iSkillStep = 1;

					for(int i=0;i<pType1->iNumCombo;i++)
					{
						const bool bImmediately = ((0 == i) ? true : false); // Put the first one right in.
						pPlayer->AnimationAdd((const e_Ani)pType1->iAct[i], bImmediately);
					}
				}
			}
		}
	}
	return true;
}
/*
bool CMagicSkillMng::EffectingType1(DWORD dwMagicID, int iSourceID, int iTargetID, short* pData)
{
	CPlayerBase* pTarget = m_pGameProcMain->CharacterGetByID(iTargetID, false);
	if(pTarget)
	{
		if (iSourceID == s_pPlayer->IDNumber()) // When I use a skill...
		{
			__TABLE_UPC_SKILL_TYPE_1* pType1 = m_pTbl_Type_1->Find(dwMagicID);

			if(pType1 && pData[0] <= pType1->iNumCombo)
			{
				m_iTarget = iTargetID;
				m_iComboSkillID = dwMagicID;

				if(m_iActionState[pData[0]-1] != -1)
				{
					if(pData[0]!=pType1->iNumCombo)	
					{
						BYTE byBuff[32];
						int iOffset=0;
						CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_MAGIC);
						CAPISocket::MP_AddByte(byBuff, iOffset, (BYTE)N3_SP_MAGIC_EFFECTING);
						CAPISocket::MP_AddDword(byBuff, iOffset, (int)m_iComboSkillID);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)iSourceID);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)m_iTarget);

						CAPISocket::MP_AddShort(byBuff, iOffset, (short)pData[0]);
						CAPISocket::MP_AddShort(byBuff, iOffset, (short)pData[1]);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						CAPISocket::MP_AddShort(byBuff, iOffset, 0);
						
						CGameProcedure::s_pSocket->Send(byBuff, iOffset); // send..
						// m_iActionState[pData[0]-1] = -1;
					}					
				}
				m_iResult[pData[0]-1] = pData[1];
			}// end of if(pType1 && pData[0] <= pType1->iNumCombo)
		}
		else if(pData[0]==1) // When another user uses a skill
		{
			__TABLE_UPC_SKILL_TYPE_1* pType1 = m_pTbl_Type_1->Find(dwMagicID);
			if(pType1)
			{
				CPlayerBase* pPlayer = m_pGameProcMain->CharacterGetByID(iSourceID, true);
				__ASSERT(pPlayer, "NULL Player Pointer!!");
				if(pPlayer)
				{
					// Change the black color.
					// __TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(dwMagicID);
					// D3DCOLOR crTrace = TraceColorGet(pSkill); // Set the color of the sword according to the type of skill.
					// pPlayer->PlugTraceColorRemake(crTrace); // Apply black color..

					pPlayer->RotateTo(pTarget);
					pPlayer->m_iSkillStep = 1;

					for(int i=0;i<pType1->iNumCombo;i++)
					{
						bool bImmediately = ((0 == i) ? true : false); // Insert the first one right away.
						pPlayer->AnimationAdd((const e_Ani)pType1->iAct[i], bImmediately);
					}
				}
			}
		}
	}
	if(pData[0]==1) return true;
	return false;	
}
*/

void CMagicSkillMng::EffectingType3(DWORD dwMagicID)
{
	__TABLE_UPC_SKILL_TYPE_3* pType3 = m_pTbl_Type_3->Find(dwMagicID);
	__ASSERT(pType3, "NULL type3 Pointer!!");
	if(!pType3) return;

	StunMySelf(pType3);

	int key = 0;
	if(pType3->iStartDamage>0 || (pType3->iStartDamage==0 && pType3->iDuraDamage>0) ) key = DDTYPE_TYPE3_DUR_OUR;
	else key = DDTYPE_TYPE3_DUR_ENEMY;

	if(key==DDTYPE_TYPE3_DUR_ENEMY && pType3->iAttribute==3) StopCastingByRatio();

	if(pType3->iDurationTime==0) return;
		
	__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(dwMagicID);
	m_pGameProcMain->m_pUIStateBarAndMiniMap->AddMagic(pSkill, (float)pType3->iDurationTime);
	m_ListBuffTypeID.insert(std::make_pair(key, dwMagicID));
}

//
//
//
void CMagicSkillMng::EffectingType4(DWORD dwMagicID)
{
	__TABLE_UPC_SKILL_TYPE_4* pType4 = m_pTbl_Type_4->Find(dwMagicID);
	__ASSERT(pType4, "NULL type4 Pointer!!");
	if(!pType4) return;

	__InfoPlayerBase* pInfoBase = &(s_pPlayer->m_InfoBase);
	__InfoPlayerMySelf* pInfoExt = &(s_pPlayer->m_InfoExt);

	const std::multimap<int, DWORD>::iterator it = m_ListBuffTypeID.find(pType4->iBuffType);
	if(it!= m_ListBuffTypeID.end())
	{
		__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(it->second);
		m_pGameProcMain->m_pUIStateBarAndMiniMap->DelMagic(pSkill);
		m_ListBuffTypeID.erase(it);
	}

	__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(dwMagicID);
	m_pGameProcMain->m_pUIStateBarAndMiniMap->AddMagic(pSkill, (float)pType4->iDuration);
	m_ListBuffTypeID.insert(std::make_pair(pType4->iBuffType,dwMagicID));

	// The same buff type of magic cannot be used repeatedly... Only the one used first is effective.
	if(pType4)
	{
		switch(pType4->iBuffType)
		{
		case BUFFTYPE_MAXHP:
			pInfoBase->iHPMax -= m_iMaxHP;
									
			m_iMaxHP = pType4->iMaxHP;
			pInfoBase->iHPMax += m_iMaxHP;
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateHP(pInfoBase->iHP,
																pInfoBase->iHPMax);
			break;
		case BUFFTYPE_AC:
			pInfoExt->iGuard_Delta -= m_iAC;

			m_iAC = pType4->iAC;
			pInfoExt->iGuard_Delta += m_iAC;
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateGuardPoint(pInfoExt->iGuard, pInfoExt->iGuard_Delta);
			break;
		case BUFFTYPE_ATTACK:
			pInfoExt->iAttack_Delta -= m_iAttack;

			m_iAttack = (pInfoExt->iAttack * pType4->iAttack / 100) - pInfoExt->iAttack;
			pInfoExt->iAttack_Delta += m_iAttack;
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateAttackPoint(pInfoExt->iAttack, pInfoExt->iAttack_Delta);
			break;
		case BUFFTYPE_ATTACKSPEED:
			s_pPlayer->m_fAttackDelta /= m_fAttackSpeed; 

			m_fAttackSpeed = (float)pType4->iAttackSpeed / 100.0f;
			s_pPlayer->m_fAttackDelta *= m_fAttackSpeed; 
			break;
		case BUFFTYPE_SPEED:
			s_pPlayer->m_fMoveDelta /= m_fSpeed;

			m_fSpeed = (float)pType4->iMoveSpeed / 100.0f;
			s_pPlayer->m_fMoveDelta *= m_fSpeed; 
			break;
		case BUFFTYPE_ABILITY:
			pInfoExt->iStrength_Delta -= m_iStr;
			pInfoExt->iStamina_Delta -= m_iSta;
			pInfoExt->iDexterity_Delta -= m_iDex;
			pInfoExt->iIntelligence_Delta -= m_iInt;
			pInfoExt->iMagicAttak_Delta -= m_iMAP;

			m_iStr = pType4->iStr;
			m_iSta = pType4->iSta;
			m_iDex = pType4->iDex;
			m_iInt = pType4->iInt;
			m_iMAP = pType4->iMAP;
			pInfoExt->iStrength_Delta += m_iStr;
			pInfoExt->iStamina_Delta += m_iSta;
			pInfoExt->iDexterity_Delta += m_iDex;
			pInfoExt->iIntelligence_Delta += m_iInt;
			pInfoExt->iMagicAttak_Delta += m_iMAP;

			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateStrength(pInfoExt->iStrength, pInfoExt->iStrength_Delta);
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateStamina(pInfoExt->iStamina, pInfoExt->iStamina_Delta);
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateDexterity(pInfoExt->iDexterity, pInfoExt->iDexterity_Delta);
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateIntelligence(pInfoExt->iIntelligence, pInfoExt->iIntelligence_Delta);
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateMagicAttak(pInfoExt->iMagicAttak, pInfoExt->iMagicAttak_Delta);
			break;
		case BUFFTYPE_RESIST:
			pInfoExt->iRegistFire_Delta -= m_iFireR;
			pInfoExt->iRegistCold_Delta -= m_iColdR;
			pInfoExt->iRegistLight_Delta -= m_iLightningR;
			pInfoExt->iRegistMagic_Delta -= m_iMagicR;
			pInfoExt->iRegistCurse_Delta -= m_iDeseaseR;
			pInfoExt->iRegistPoison_Delta -= m_iPoisonR;

			m_iFireR = pType4->iFireResist;
			m_iColdR = pType4->iColdResist;
			m_iLightningR = pType4->iLightningResist;
			m_iMagicR = pType4->iMagicResist;
			m_iDeseaseR = pType4->iDeseaseResist;
			m_iPoisonR = pType4->iPoisonResist;

			pInfoExt->iRegistFire_Delta += m_iFireR;
			pInfoExt->iRegistCold_Delta += m_iColdR;
			pInfoExt->iRegistLight_Delta += m_iLightningR;
			pInfoExt->iRegistMagic_Delta += m_iMagicR;
			pInfoExt->iRegistCurse_Delta += m_iDeseaseR;
			pInfoExt->iRegistPoison_Delta += m_iPoisonR;

			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistFire(pInfoExt->iRegistFire, pInfoExt->iRegistFire_Delta);
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistCold(pInfoExt->iRegistCold, pInfoExt->iRegistCold_Delta);
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistLight(pInfoExt->iRegistLight, pInfoExt->iRegistLight_Delta);
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistMagic(pInfoExt->iRegistMagic, pInfoExt->iRegistMagic_Delta);
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistCurse(pInfoExt->iRegistCurse, pInfoExt->iRegistCurse_Delta);
			m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistPoison(pInfoExt->iRegistPoison, pInfoExt->iRegistPoison_Delta);
			break;
		}
	}
}


//
// I manage the skills and magic I use by putting in an index.
// This is an indexing function.
//
int CMagicSkillMng::AddIdx(DWORD MagicID, int iNum)
{
	int idx = 0;
	std::map<int, DWORD>::iterator it;
	
	// If you create multiple linked indexes at once...
	if(iNum>1)
	{
		if(m_MySelf.size()!=0)
		{
			it = m_MySelf.end();
			it--;
			idx = it->first + 1;
		}
		else idx = 1;						

		for(int i=0;i<iNum;i++)
		{
			m_MySelf.insert(std::make_pair(idx+i, MagicID));
		}
		return idx;
	}

	// If you only create one index...
	for(it = m_MySelf.begin(); it!=m_MySelf.end(); it++)
	{
		if(it->first==idx)
		{
			idx++;
			continue;
		}
		else
		{
			m_MySelf.insert(std::make_pair(idx, MagicID));
			break;
		}
	}
	if(it==m_MySelf.end())
	{
		m_MySelf.insert(std::make_pair(idx, MagicID));
	}

	return idx;
}


//
//
//
void CMagicSkillMng::InitType4()
{
	m_iBuffType = 0;
	
	m_fAttackSpeed = 1.0f;
	m_fSpeed = 1.0f;

	m_iAC = 0;
	m_iAttack = 0;
	m_iMaxHP = 0;
	m_iStr = 0;
	m_iSta = 0;
	m_iDex = 0;
	m_iInt = 0;
	m_iMAP = 0;
	m_iFireR = 0;
	m_iColdR = 0;
	m_iLightningR = 0;
	m_iMagicR = 0;
	m_iDeseaseR = 0;
	m_iPoisonR = 0;


	std::multimap<int, DWORD>::iterator its, ite;
	its = m_ListBuffTypeID.begin();
	ite = m_ListBuffTypeID.end();

	for(;its != ite; its++)
	{
		__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find((*its).second);
		m_pGameProcMain->m_pUIStateBarAndMiniMap->DelMagic(pSkill);		
	}
	m_ListBuffTypeID.clear();
}


//
// This function removes the index.
//
void CMagicSkillMng::RemoveIdx(int idx)
{
	m_MySelf.erase(idx);
}

DWORD CMagicSkillMng::GetMagicID(int idx)
{
	const std::map<int, DWORD>::iterator it = m_MySelf.find(idx);
	return it->second;
}

D3DCOLOR CMagicSkillMng::TraceColorGet(__TABLE_UPC_SKILL* pSkill) // Depending on the type of skill, the color of the sword is determined.
{
	if(nullptr == pSkill) return 0xff404040;
	
	D3DCOLOR crTrace = 0xffff4040;
	switch(pSkill->dwNeedItem) // Depending on the required item...
	{
		case 1: crTrace = 0xff808080; // ITEM_CLASS_DAGGER = 11 // dagger
		case 2: crTrace = 0xff909090; // ITEM_CLASS_SWORD = 21, // onehandsword
		// case : crTrace = ; // ITEM_CLASS_SWORD_2H = 22, // 3 : (twohandsword)
		case 3: crTrace = 0xff7070ff; // ITEM_CLASS_AXE = 31, // onehandaxe
		// case : crTrace = ; // ITEM_CLASS_AXE_2H = 32, // (twohandaxe)
		case 4: crTrace = 0xffa07070; // ITEM_CLASS_MACE = 41, // Mace
		// case: crTrace = ; // ITEM_CLASS_MACE_2H = 42, // Twohandmace
		case 5: crTrace = 0xffff7070; // ITEM_CLASS_SPEAR = 51, // (spear)
		// case : crTrace = ; // ITEM_CLASS_POLEARM = 52, // (polearm)
		default: crTrace = 0xff4040ff;
	}
	
	return crTrace;
}

bool CMagicSkillMng::IsPositiveMagic(DWORD dwMagicID)
{
	const __TABLE_UPC_SKILL* pSkill = CGameBase::s_pTbl_Skill->Find(dwMagicID);
	if(!pSkill) return true;

	if(pSkill->dw1stTableType==3 || pSkill->dw2ndTableType==3)
	{
		const __TABLE_UPC_SKILL_TYPE_3* pType3 = m_pTbl_Type_3->Find(dwMagicID);
		if(!pType3) return true;

		int key = 0;
		if(pType3->iStartDamage>0 || (pType3->iStartDamage==0 && pType3->iDuraDamage>0) ) key = DDTYPE_TYPE3_DUR_OUR;
		else key = DDTYPE_TYPE3_DUR_ENEMY;

		key += pType3->iDDType;
		if(key==DDTYPE_TYPE3_DUR_OUR) return true;
		return false;
	}
	if(pSkill->dw1stTableType==4 || pSkill->dw2ndTableType==4)
	{
		const __TABLE_UPC_SKILL_TYPE_4* pType4 = m_pTbl_Type_4->Find(dwMagicID);
		if(!pType4) return true;

		switch(pType4->iBuffType)
		{
		case BUFFTYPE_MAXHP:
			if(pType4->iMaxHP>0) return true;
			break;
		case BUFFTYPE_AC:
			if(pType4->iAC>0) return true;
			break;
		case BUFFTYPE_RESIZE:
			return true;
		case BUFFTYPE_ATTACK:
			if(pType4->iAttack>100) return true;
			break;
		case BUFFTYPE_ATTACKSPEED:
			if(pType4->iAttackSpeed>100) return true;
			break;
		case BUFFTYPE_SPEED:
			if(pType4->iMoveSpeed>100) return true;
			break;
		case BUFFTYPE_ABILITY:
			if(pType4->iStr>0 || pType4->iSta>0 || pType4->iDex>0 || pType4->iInt>0 || pType4->iMAP>0) return true;
			break;
		case BUFFTYPE_RESIST:
			if(pType4->iFireResist>0 || pType4->iColdResist>0 || pType4->iLightningResist>0 ||
				pType4->iMagicResist>0 || pType4->iDeseaseResist>0 || pType4->iPoisonResist>0) return true;
			break;
		}		
	}
	return false;
}

void CMagicSkillMng::ClearDurationalMagic()
{
	__InfoPlayerBase* pInfoBase = &(s_pPlayer->m_InfoBase);
	__InfoPlayerMySelf* pInfoExt = &(s_pPlayer->m_InfoExt);

	if(pInfoBase->iHP < pInfoBase->iHPMax)
	{
		pInfoBase->iHPMax -= m_iMaxHP;
		m_pGameProcMain->m_pUIVar->m_pPageState->UpdateHP(pInfoBase->iHP, pInfoBase->iHPMax);
	}
	m_iMaxHP = 0;

	pInfoExt->iGuard_Delta -= m_iAC;
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateGuardPoint(pInfoExt->iGuard, pInfoExt->iGuard_Delta);
	m_iAC = 0;

	if(m_iAttack) pInfoExt->iAttack_Delta -= m_iAttack;
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateAttackPoint(pInfoExt->iAttack, pInfoExt->iAttack_Delta);
	m_iAttack = 0;
	
	s_pPlayer->m_fAttackDelta /= m_fAttackSpeed; 
	m_fAttackSpeed = 1.0f;
	
	s_pPlayer->m_fMoveDelta /= m_fSpeed;
	m_fSpeed = 1.0f;
	
	pInfoExt->iStrength_Delta -= m_iStr;
	pInfoExt->iStamina_Delta -= m_iSta;
	pInfoExt->iDexterity_Delta -= m_iDex;
	pInfoExt->iIntelligence_Delta -= m_iInt;
	pInfoExt->iMagicAttak_Delta -= m_iMAP;
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateStrength(pInfoExt->iStrength, pInfoExt->iStrength_Delta);
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateStamina(pInfoExt->iStamina, pInfoExt->iStamina_Delta);
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateDexterity(pInfoExt->iDexterity, pInfoExt->iDexterity_Delta);
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateIntelligence(pInfoExt->iIntelligence, pInfoExt->iIntelligence_Delta);
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateMagicAttak(pInfoExt->iMagicAttak, pInfoExt->iMagicAttak_Delta);
	m_iStr = 0;
	m_iSta = 0;
	m_iDex = 0;
	m_iInt = 0;
	m_iMAP = 0;	
	
	pInfoExt->iRegistFire_Delta -= m_iFireR;
	pInfoExt->iRegistCold_Delta -= m_iColdR;
	pInfoExt->iRegistLight_Delta -= m_iLightningR;
	pInfoExt->iRegistMagic_Delta -= m_iMagicR;
	pInfoExt->iRegistCurse_Delta -= m_iDeseaseR;
	pInfoExt->iRegistPoison_Delta -= m_iPoisonR;
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistFire(pInfoExt->iRegistFire, pInfoExt->iRegistFire_Delta);
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistCold(pInfoExt->iRegistCold, pInfoExt->iRegistCold_Delta);
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistLight(pInfoExt->iRegistLight, pInfoExt->iRegistLight_Delta);
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistMagic(pInfoExt->iRegistMagic, pInfoExt->iRegistMagic_Delta);
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistCurse(pInfoExt->iRegistCurse, pInfoExt->iRegistCurse_Delta);
	m_pGameProcMain->m_pUIVar->m_pPageState->UpdateRegistPoison(pInfoExt->iRegistPoison, pInfoExt->iRegistPoison_Delta);
	m_iFireR = 0;
	m_iColdR = 0;
	m_iLightningR = 0;
	m_iMagicR = 0;
	m_iDeseaseR = 0;
	m_iPoisonR = 0;

	InitType4();
}

void CMagicSkillMng::StopCastingByRatio()
{
	m_pGameProcMain->CommandSitDown(false, false); // stand up
	if(IsCasting())
	{
		__TABLE_UPC_SKILL* pSkill = s_pTbl_Skill->Find(s_pPlayer->m_dwMagicID);
		if(pSkill)
		{
			const int SuccessValue = rand()%100;
			if(SuccessValue >= pSkill->iPercentSuccess) // Failure according to the probability shown in the skill table.
			{
				FailCast(pSkill);
				// if( s_pPlayer->Action(PSA_BASIC, false, NULL, true); // Cancel casting, forcibly set as default action..
			}				
		}
	}
}

void CMagicSkillMng::StunMySelf(__TABLE_UPC_SKILL_TYPE_3* pType3)
{
	if(pType3->iAttribute!=3) return;

	const int sample = rand()%101;

	const __InfoPlayerMySelf* pInfoExt = &(s_pPlayer->m_InfoExt);

	float Regist = pInfoExt->iRegistLight + pInfoExt->iRegistLight_Delta;
	if(Regist>80.0f) Regist = 80.0f;

	const float Prob = (30.0f+(40.0f-( 40.0f*(Regist/80.0f) )));

	if(sample < (int)Prob) // freeze...
	{
		m_pGameProcMain->CommandSitDown(false, false); // stand up
		s_pPlayer->Stun(STUN_TIME);
	}
}