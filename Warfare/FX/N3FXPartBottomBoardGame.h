#pragma once

#include "N3FXPartBottomBoard.h"

class CN3FXPartBottomBoardGame : public CN3FXPartBottomBoard
{
public:
	float	GetGroundHeight(float x, float z);

public:
	CN3FXPartBottomBoardGame();
	virtual ~CN3FXPartBottomBoardGame();
};
