#pragma once

#include "N3UIBase.h"
#include "N3UIString.h"

#include "GameDef.h"

#include "GameProcedure.h"
#include "N3UIWndBase.h"

// 

#define MAX_TOOLTIP_COUNT 30

class CUIImageTooltipDlg    : public CN3UIBase  
{
	const D3DCOLOR		m_CYellow;	// rare...
	const D3DCOLOR		m_CBlue;	// Magic...
	const D3DCOLOR		m_CGold;	// unique...
	const D3DCOLOR		m_CIvory;	// upgrade..
	const D3DCOLOR		m_CGreen;	// Options... Resistance, stat increase, magic attribute...
	const D3DCOLOR		m_CWhite;	// common...
	const D3DCOLOR		m_CRed;		// When something goes wrong...

public:
	CN3UIString*		m_pStr[MAX_TOOLTIP_COUNT];			// Stroop.. ^^
	std::string			m_pstdstr[MAX_TOOLTIP_COUNT];
	CN3UIImage*			m_pImg;

	int					m_iPosXBack, m_iPosYBack;
	__IconItemSkill*	m_spItemBack;

protected:
	bool				SetTooltipTextColor(int iMyValue, int iTooltipValue);
	bool				SetTooltipTextColor(e_Race eMyValue, e_Race eTooltipValue);
	bool				SetTooltipTextColor(e_Class eMyValue, e_Class eTooltipValue);
	int					CalcTooltipStringNumAndWrite(__IconItemSkill* spItem, bool bPrice, bool bBuy);
	void				SetPosSomething(int xpos, int ypos, int iNum);

public:
	CUIImageTooltipDlg();
	virtual ~CUIImageTooltipDlg();
	void	Release();
	void	InitPos();

	void	DisplayTooltipsEnable(int xpos, int ypos, __IconItemSkill* spItem, bool bPrice = false, bool bBuy = true);
	void	DisplayTooltipsDisable();
};
