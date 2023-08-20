#pragma once

#include "GameDef.h"
#include "PacketDef.h"
#include "APISocket.h"
#include "N3UIBase.h"
#include "UIMessageBox.h"
#include "UITradeExplanation.h"
#include <list>

struct __InfoTradeSellBBS // Party support bulletin board structure..
{
	std::string szID;			// party name string
	std::string szTitle;		// title
	std::string szExplanation;	// explanation
	int			iPrice;			// price
	short		sIndex;			// registration index
	short		sID;			// character id

	void Init()
	{
		szID			= "";
		szTitle			= "";
		szExplanation	= "";
		iPrice			= 0;
		sIndex			= -1;
	};

	__InfoTradeSellBBS()
	{
		this->Init();
	}
};

class CUITradeSellBBS : public CN3UIBase  
{
protected:
	// class CN3UIList*		m_pList_Infos;

	class CN3UIButton*		m_pBtn_PageUp;
	class CN3UIButton*		m_pBtn_PageDown;
	class CN3UIButton*		m_pBtn_Refresh;
	class CN3UIButton*		m_pBtn_Close;
	class CN3UIButton*		m_pBtn_Register;
	class CN3UIButton*		m_pBtn_RegisterCancel;
	class CN3UIButton*		m_pBtn_Whisper;
	class CN3UIButton*		m_pBtn_Trade;

	class CN3UIImage*		m_pImage_Sell;
	class CN3UIImage*		m_pImage_Buy;
	class CN3UIImage*		m_pImage_Sell_Title;
	class CN3UIImage*		m_pImage_Buy_Title;

	class CN3UIString*		m_pString_Page;
	class CN3UIString*		m_pText[69];

	CUIMessageBox			m_MsgBox;
	CUITradeExplanation		m_UIExplanation;

	std::list<__InfoTradeSellBBS>	m_Datas;
	__InfoTradeSellBBS				m_ITSB;

	int			m_iCurPage; // Current page...
	int			m_iMaxPage;	// total page..
	bool		m_bProcessing;
	BYTE		m_byBBSKind;
	int			m_iCurIndex;
	float		m_fTime;

public:
	void SetContentString();
	void ResetContent();
	void Render();
	void RenderSelectContent() const;
	bool OnKeyPress(int iKey);
	void MsgSend_PerTrade();
	void OnListExplanation();
	void RefreshExplanation(bool bPageUp = true);
	void OnButtonTrade();
	void OnButtonWhisper();
	void OnButtonRegisterCancel();
	void OnButtonRegister();
	void CallBackProc(int iID, DWORD dwFlag);
	void MsgSend_RegisterCancel(short sIndex);
	void MsgSend_Register();
	void MsgSend_RefreshData(int iCurPage);
	void RefreshPage();
	bool SelectedString(CN3UIBase* pSender, int& iID);
	void MsgRecv_RefreshData(DataPack* pDataPack, int &iOffset);
	void MsgRecv_TradeBBS(DataPack* pDataPack, int &iOffset);
	void SetContentString(int iIndex, std::string szID, int iPrice, std::string szTitle);

	void SetBBSKind(BYTE byKind){ m_byBBSKind = byKind; }
	bool Load(HANDLE hFile);
	bool ReceiveMessage(class CN3UIBase* pSender, DWORD dwMsg);
	void SetVisible(bool bVisible);

	CUITradeSellBBS();
	virtual ~CUITradeSellBBS();

};
