// MagicProcess.cpp: implementation of the CMagicProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ebenezer.h"
#include "MagicProcess.h"
#include "EbenezerDlg.h"
#include "User.h"
#include "Npc.h"
#include "PacketDefine.h"
#include "AiPacket.h"
#include "GameDefine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MORAL_SELF				1		// 