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
	CGameProcedure::Render(); // UI �� �׹��� �⺻���� �͵� ������..
}

void CGameProcOption::Tick()
{
	CGameProcedure::Tick();	// Ű, ���콺 �Է� ���..
}
