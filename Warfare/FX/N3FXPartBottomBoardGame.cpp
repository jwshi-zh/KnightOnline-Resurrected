#include "pch.h"
#include "GameProcedure.h"
#include "N3WorldManager.h"
#include "N3FXPartBottomBoardGame.h"

CN3FXPartBottomBoardGame::CN3FXPartBottomBoardGame()
{
}

CN3FXPartBottomBoardGame::~CN3FXPartBottomBoardGame()
{
	if(m_pVB)
	{
		delete[] m_pVB;
		m_pVB = nullptr;
	}

	if(m_vUnit)
	{
		delete[] m_vUnit;
		m_vUnit = nullptr;
	}
}

float CN3FXPartBottomBoardGame::GetGroundHeight(float x, float z)
{
	float fHeight = CGameBase::ACT_WORLD->GetHeightWithTerrain(x, z) + 0.1f;
	return fHeight;
}
