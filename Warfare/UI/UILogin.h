#pragma once

#include <string>
#include "N3UIBase.h"

struct __ServerListItem
{
	std::string m_szName{};
	std::string m_szIP{};

	uint16_t m_iConcurrentUserCount{};
	uint16_t m_iConcurrentUserCapacity{};

	CN3UIString* m_pStr_ServerName{ nullptr };
	std::array<CN3UIImage*, 12> m_vImage_Capacity{ nullptr };
	CN3UIImage* m_pImage_Arrow{ nullptr };
};

class CUILogIn : public CN3UIBase  
{
protected:
	CN3UIEdit*	m_pEdit_id;
	CN3UIEdit*	m_pEdit_pw;
	
	CN3UIButton* m_pBtn_LogIn;
	CN3UIButton* m_pBtn_Connect;
	CN3UIButton* m_pBtn_Cancel;
	CN3UIButton* m_pBtn_Option;
	CN3UIButton* m_pBtn_Homepage;
	CN3UIButton* m_pBtn_Notice1Ok;
	CN3UIButton* m_pBtn_Notice2Ok;
	CN3UIButton* m_pBtn_Notice3Ok;

	CN3UIBase*	m_pGroup_ServerList;
	CN3UIBase* m_pGroup_LogIn;
	CN3UIBase* m_pGroup_Notice1;
	CN3UIBase* m_pGroup_Notice2;
	CN3UIBase* m_pGroup_Notice3;

	CN3UIBase* m_pGroup_ActiveNotice{ nullptr };

	bool	m_bLogIn;
	uint8_t	m_iSelectedServerIdx{ 0 };

	std::array<__ServerListItem, 20> m_arrServerList;


public:
	void SetRequestedLogIn(bool bLogIn) { m_bLogIn = bLogIn; }
	bool OnKeyPress(int iKey);
	void SetVisibleLogInUIs(bool bEnable); // Hide UIs required for account LogIn.
	void OpenServerList();
	void HideLoginSubview();
	void OpenNotice(uint8_t iNoticeCount, std::vector<std::string> vSzNotice);
	void SelectServer(uint8_t idx);

	void InitEditControls();
	void FocusCircular();
	void FocusToID();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg); // Receives a message. Sender, msg

	bool	ServerInfoAdd(uint8_t iIndex, std::string szName, std::string szIP, uint16_t iConcurrentUserCount, uint16_t iConcurrentCapacity);
	__ServerListItem& ServerInfoGetSelected();

	void AccountIDGet(std::string& szID) const;
	void AccountPWGet(std::string& szPW);

	void ConnectButtonSetEnable(bool bEnable);

	CUILogIn();
	virtual ~CUILogIn();
};
