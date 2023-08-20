#include "pch.h"
#include "GameProcOption.h"

CGameProcOption::CGameProcOption()
{
}

CGameProcOption::~CGameProcOption()
{
}

void CGameProcOption::Init()
{
	CGameProcedure::Init();
}

void CGameProcOption::Release()
{
	CGameProcedure::Release();
}

void CGameProcOption::Render()
{
	CGameProcedure::Render(); // Render UI and other basic stuff..
}

void CGameProcOption::Tick()
{
	CGameProcedure::Tick();	// keys, mouse input, etc.
}
