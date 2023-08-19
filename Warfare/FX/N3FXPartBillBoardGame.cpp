#include "pch.h"
#include "GameProcedure.h"
#include "N3WorldManager.h"
#include "N3FXPartBillBoardGame.h"

float CN3FXPartBillBoardGame::GetGroundHeight(float x, float z)
{
	const float fHeight = CGameBase::ACT_WORLD->GetHeightWithTerrain(x, z) + 0.1f;
	return fHeight;
}
