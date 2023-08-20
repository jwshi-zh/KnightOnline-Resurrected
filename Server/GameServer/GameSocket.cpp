// GameSocket.cpp: implementation of the CGameSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Server.h"
#include "GameSocket.h"
#include "ServerDlg.h"
#include "User.h"
#include "Map.h"
#include "region.h"
#include "Party.h"

#include "extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CRITICAL_SECTION g_region_critical;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
     ** Repent AI Server 