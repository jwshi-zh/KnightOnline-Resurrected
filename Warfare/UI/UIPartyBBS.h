#pragma once

#include "GameDef.h"
#include "PacketDef.h"
#include "APISocket.h"
#include "N3UIBase.h"
#include <list>

class CUIPartyBBS : public CN3UIBase // It is used as the same class as the UI for the party and the troops.
{
protected:
	class CN3UIButton*		m_pBtn_PageUp;
	class CN3UIButton*		m_pBtn_PageDown;
	class CN3UIButton*		m_pBtn_Refresh;
	
	class CN3UIButton*		m_pBtn_Close;
	class CN3UIButton*		m_pBtn_Register;
	class CN3UIButton*		m_pBtn_RegisterCancel;
	class CN3UIButton*		m_pBtn_Whisper;
	class CN3UIButton*		m_pBtn_Party;

	class CN3UIString*		m_pText_Page;
	class CN3UIString*		m_pText[69];

	std::list<__InfoPartyBBS>	m_Datas; // BBS Data
	int			m_iCurPage; // Current page...
	int			m_iMaxPage;	// total page..
	int			m_iCurIndex;
	bool		m_bProcessing;
	float		m_fTime;

public:
	bool OnKeyPress(int iKey);
	void SetVisible(bool bVisible);
	void RequestParty();
	void RequestWhisper();
	void SetStringColor(int iIndex, DWORD dwColor);
	void RenderSelectContent();
	void Render();
	void SetContentString(int iIndex, std::string szID, int iLevel, std::string szClass);
	void ResetContent();
	void MsgSend_Register();
	void MsgSend_RegisterCancel();
	void MsgSend_RefreshData(int iCurPage);

	void MsgRecv_RefreshData(class DataPack* pDataPack, int& iOffset);

	void PartyStringSet(BYTE byType);
	void RefreshPage();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(class CN3UIBase* pSender, DWORD dwMsg);
	bool SelectedString(CN3UIBase* pSender, int& iID) const;
	
	
	CUIPartyBBS();
	virtual ~CUIPartyBBS();
};
