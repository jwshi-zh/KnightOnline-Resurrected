#pragma once

#include "GameProcedure.h"

class CGameProcLogIn : public CGameProcedure
{
public:
	class CUILogIn*		m_pUILogIn;

	bool m_bLogIn; // Avoid duplicate logins..

public:
	void	MsgRecv_GameServerGroupList(DataPack* pDataPack, int& iOffset) const;
	void	MsgRecv_AccountLogIn(int iCmd, DataPack* pDataPack, int& iOffset);
	int		MsgRecv_VersionCheck(DataPack* pDataPack, int& iOffset); // virtual
	int		MsgRecv_GameServerLogIn(DataPack* pDataPack, int& iOffset); // virtual - Returns the country code.


	void Release();
	void Init();
	void Tick();
	void Render();

protected:
	virtual bool ProcessPacket(DataPack* pDataPack, int& iOffset);

public:

	void ConnectToGameServer(); // Connect to the game server of your choice
	CGameProcLogIn();
	virtual ~CGameProcLogIn();
	// void PacketSend_MGameLogin();
};
