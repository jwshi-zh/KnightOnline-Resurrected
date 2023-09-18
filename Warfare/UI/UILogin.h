#pragma once

#include <string>
#include "N3UIBase.h"
#include "N3UIString.h"
#include "N3UIImage.h"

struct __GameServerInfo
{
	std::string szName;
	std::string szIP;
	int	iConcurrentUserCount;

	void Init() { szName = ""; szIP = ""; iConcurrentUserCount = 0; }
	bool operator () (const __GameServerInfo& x, const __GameServerInfo& y) const 
	{
		return (x.iConcurrentUserCount >= y.iConcurrentUserCount);
	}

	__GameServerInfo() { this->Init(); };
	__GameServerInfo(const std::string szName2, const std::string szIP2, int iConcurrentUserCount2)
	{
		szName = szName2;
		szIP = szIP2;
		iConcurrentUserCount = iConcurrentUserCount2;
	}
};

class CUILogIn : public CN3UIBase  
{
public:
	CN3UIEdit*	m_pEdit_id;
	CN3UIEdit*	m_pEdit_pw;
	
	CN3UIButton* m_pBtn_LogIn;
	CN3UIButton* m_pBtn_Connect;
	CN3UIButton* m_pBtn_Cancel;
	CN3UIButton* m_pBtn_Option;

	CN3UIBase*	m_pGroup_Notice[3];
	CN3UIBase*	m_pGroup_ServerList;
	CN3UIBase*	m_pGroup_LogIn;

	CN3UIBase*	m_pBaseServer[20];
	CN3UIString*	m_pBaseServerlists[20];
	CN3UIImage* m_pBaseServerlistsArrows[12];
	CN3UIList*	m_pList_Server[20];
	
	std::vector<__GameServerInfo> m_ListServerInfos;

	bool	m_bOpenningNow; // Swipe from top to bottom...if you have to open it...
	float 	m_fMoveDelta;
	bool	m_bLogIn; // Avoid duplicate logins..



public:
	void SetRequestedLogIn(bool bLogIn) { m_bLogIn = bLogIn; }
	bool OnKeyPress(int iKey);
	void SetVisibleLogInUIs(bool bEnable); // Hide UIs required for account LogIn.
	void OpenServerList();
	void Tick();

	void InitEditControls();
	void FocusCircular();
	void FocusToID();
	bool Load(HANDLE hFile);
	bool ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg); // Receives a message. Sender, msg

	int		ServerInfoCount() const { return m_ListServerInfos.size(); }
	bool	ServerInfoAdd(const __GameServerInfo& GSI);
	bool	ServerInfoGet(int iIndex, __GameServerInfo& GSI);
	bool	ServerInfoGetCur(__GameServerInfo& GSI);
	void	ServerInfoUpdate(int servercount);

	void ShowImage(int i);
	void HideImage(int i);

	void AccountIDGet(std::string& szID) const;
	void AccountPWGet(std::string& szPW);

	void ConnectButtonSetEnable(bool bEnable);

	CUILogIn();
	virtual ~CUILogIn();
};
