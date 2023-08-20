#pragma once

#include "N3UIBase.h"
#include "GameDef.h"

#define MB_CANCEL                 0x00000006L

class CUIMessageBox : public CN3UIBase  
{
protected:
	CN3UIButton*	m_pBtn_OK;
	CN3UIButton*	m_pBtn_Yes;
	CN3UIButton*	m_pBtn_No;
	CN3UIButton*	m_pBtn_Cancel;
	CN3UIString*	m_pText_Message;
	CN3UIString*	m_pText_Title;
	CN3UIEdit*		m_pEdit_Common;
public:
	e_Behavior	m_eBehavior;	// What to do when you click the OK button...
	int			m_iStyle;		// message box style

public:
	bool OnKeyPress(int iKey);
	void SetVisible(bool bVisible);
	void SetBoxStyle(int iStyle);
	void SetVisibleEditControl(bool bVisible); // Edit Control Visible
	void SetTitle(const std::string& szTitle);
	void SetText(const std::string& szMsg) const;
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);
	
	CUIMessageBox();
	virtual ~CUIMessageBox();

};
