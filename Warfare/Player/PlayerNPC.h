#pragma once

#include "PlayerBase.h"
#include "GameBase.h"

// 
// Other Player..
#include "PlayerBase.h"

class CPlayerNPC : public CPlayerBase
{
	friend class CPlayerOtherMgr;
public:
	void			MoveTo(float fPosX, float fPosY, float fPosZ, float fMoveSpeed, int iMoveMode); // Go to this location...
	virtual	void	Tick();

	CPlayerNPC();
	virtual ~CPlayerNPC();
};
