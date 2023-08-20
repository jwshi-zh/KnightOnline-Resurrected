#include "stdafx.h"
#include "math.h"
#include "Npc.h"
#include "Serverdlg.h"
#include "Gamesocket.h"
#include "Region.h"
#include "Party.h"

int surround_x[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
int surround_z[8] = {0, -1, -1, -1, 0, 1, 1, 1};


int test_id = 1056;
int cur_test = 0;	// 1 = test以