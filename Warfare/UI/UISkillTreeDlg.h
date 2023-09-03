#pragma once

#include "GameDef.h"
#include "N3UIWndBase.h"

#include "N3UIButton.h"
#include "N3UIImage.h"
#include "N3UIString.h"

const int SKILL_DEF_BASIC = 0;
const int SKILL_DEF_SPECIAL0 = 1;
const int SKILL_DEF_SPECIAL1 = 2;
const int SKILL_DEF_SPECIAL2 = 3;
const int SKILL_DEF_SPECIAL3 = 4;


// MAX_SKILL_KIND_OF : 1 Base Skill, 4 Professional Skills..
// MAX_SKILL_IN_PAGE: The number of icons in one page..
// MAX_SKILL_PAGE_NUM: The maximum number of pages for one skill.


// 

class CUISkillTreeDlg    : public CN3UIWndBase  
{
protected:
	bool		m_bOpenningNow; // is opening...
	bool		m_bClosingNow;	// it's closing...
	float		m_fMoveDelta; // Floating point is used to calculate the current position to make it open and close smoothly.

	int			m_iRBtnDownOffs;

	CN3UIString*		m_pStr_info;
	CN3UIString*		m_pStr_skill_mp;
	CN3UIString*		m_pStr_skill_item0;
	CN3UIString*		m_pStr_skill_item1;

public:
	int					m_iCurKindOf;
	int					m_iCurSkillPage;

	int					m_iSkillInfo[MAX_SKILL_FROM_SERVER];										// Slot information received from server..
	__IconItemSkill*	m_pMySkillTree[MAX_SKILL_KIND_OF][MAX_SKILL_PAGE_NUM][MAX_SKILL_IN_PAGE];	// Total skill information.
	int					m_iCurInPageOffset[MAX_SKILL_KIND_OF];										// Current page offset per skill..

protected:
	void				AllClearImageByName(const std::string& szFN, bool bTrueOrNot);
	RECT				GetSampleRect();
	void				PageButtonInitialize();

public:
	void SetVisible(bool bVisible);
	CUISkillTreeDlg();
	virtual ~CUISkillTreeDlg();

	void				SetVisibleWithNoSound(bool bVisible, bool bWork = false, bool bReFocus = false);
	bool				OnKeyPress(int iKey);
	virtual	void		Release();
	virtual void		Tick();
	virtual DWORD		MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual bool		ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);
	void				Render();
	void				Open();
	void				Close();

	void				InitIconWnd(e_UIWND eWnd);	
	void				InitIconUpdate();

	__IconItemSkill*	GetHighlightIconItem(CN3UIIcon* pUIIcon);
	int					GetSkilliOrder(__IconItemSkill* spSkill);

	void				AddSkillToPage(__TABLE_UPC_SKILL* pUSkill, int iOffset = 0);

	void				SetPageInIconRegion(int iKindOf, int iPageNum);		// Set the current page in the icon area..
	void				SetPageInCharRegion();								// Set current page in text area..

	CN3UIImage*		   GetChildImageByName(const std::string& szFN);
	CN3UIBase*			GetChildBaseByName(const std::string &szFN);	
	CN3UIButton*		GetChildButtonByName(const std::string& szFN);

	void				PageLeft();
	void				PageRight();

	void				PointPushUpButton(int iValue);

	bool				HasIDSkill(int iID) const;
	void				ButtonVisibleStateSet();

	void				TooltipRenderEnable(__IconItemSkill* spSkill);
	void				TooltipRenderDisable();
	void				CheckButtonTooltipRenderEnable();
	void				ButtonTooltipRender(int iIndex);

	void				UpdateDisableCheck();
	int					  GetIndexInArea(POINT pt);
};
