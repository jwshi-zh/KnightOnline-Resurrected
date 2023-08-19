#pragma once

#include <map>

#include "UIMessageBox.h"
#include "GameBase.h"

typedef std::map<std::string, CUIMessageBox*>::iterator it_UBM;
typedef std::map<std::string, CUIMessageBox*>::value_type val_UBM;
typedef std::map<std::string, CUIMessageBox*>::reverse_iterator	rit_UBM;

class CUIMessageBoxManager : public CGameBase
{
public:
	std::map<std::string, CUIMessageBox*>		m_UBMs;
	CUIMessageBox*								m_pMsgBoxLatestRef;

public:
	CUIMessageBox*	ReFocusMsgBox();
	CUIMessageBox*	GetFocusMsgBox() const { return m_pMsgBoxLatestRef; }
	void			Release();
	DWORD			MouseProcAndTick(DWORD& dwFlags, const POINT& ptCur, const POINT& ptOld );
	void			MessageBoxCloseAll();
	void			Render();
	int				GetCount() const;

	void			MessageBoxClose(const std::string& szMsg);
	std::string		MessageBoxPost(const std::string& szMsg, const std::string& szTitle, int iStyle, e_Behavior eBehavior = BEHAVIOR_NOTHING);

	CUIMessageBoxManager();
	virtual ~CUIMessageBoxManager();

};
