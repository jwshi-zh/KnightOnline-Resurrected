// AISocket.cpp: implementation of the CAISocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ebenezer.h"
#include "AISocket.h"
#include "EbenezerDlg.h"
#include "define.h"
#include "PacketDefine.h"
#include "AiPacket.h"
#include "Npc.h"
#include "user.h"
#include "Map.h"
#include "Compress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern CRITICAL_SECTION g_LogFile_critical;

CAISocket::CAISocket(int zonenum)
{
	m_iZoneNum = zonenum;
}

CAISocket::~CAISocket()
{

}

void CAISocket::Initialize()
{
	m_pMain = (CEbenezerDlg*)AfxGetMainWnd();
	m_MagicProcess.m_pMain = m_pMain;
}

void CAISocket::Parsing( int len, char* pData )
{
	int index = 0;

	BYTE command = GetByte(pData, index);

	//TRACE("Parsing - command=%d, length = %d\n", command, len);

	switch( command )
	{
		case AG_CHECK_ALIVE_REQ:
			RecvCheckAlive(pData+index);
			break;
		case AI_SERVER_CONNECT:
			LoginProcess(pData+index);
			break;
		case AG_SERVER_INFO:
			RecvServerInfo(pData+index);
			break;
		case NPC_INFO_ALL:
//			Sleep(730);
			RecvNpcInfoAll(pData+index);
			break;
		case MOVE_RESULT:
			RecvNpcMoveResult(pData+1);
			break;
		case MOVE_END_RESULT:
			break;
		case AG_ATTACK_RESULT:
			RecvNpcAttack(pData+1);
			break;
		case AG_MAGIC_ATTACK_RESULT:
			RecvMagicAttackResult(pData+1);
			break;
		case AG_NPC_INFO:
			RecvNpcInfo(pData+1);
			break;
		case AG_USER_SET_HP:
			RecvUserHP(pData+1);
			break;
		case AG_USER_EXP:
			RecvUserExp(pData+1);
			break;
		case AG_SYSTEM_MSG:
			RecvSystemMsg(pData+1);
			break;
		case AG_NPC_GIVE_ITEM:
			RecvNpcGiveItem(pData+1);
			break;
		case AG_USER_FAIL:
			RecvUserFail(pData+1);
			break;
		case AG_COMPRESSED_DATA:
			RecvCompressedData(pData+1);
			break;
		case AG_NPC_GATE_DESTORY:
			RecvGateDestory(pData+1);
			break;
		case AG_DEAD:
			RecvNpcDead(pData+1);
			break;
		case AG_NPC_INOUT:
			RecvNpcInOut(pData+1);
			break;
		case AG_BATTLE_EVENT:
			RecvBattleEvent(pData+1);
			break;
		case AG_NPC_EVENT_ITEM:
			RecvNpcEventItem(pData+1);
			break;
		case AG_NPC_GATE_OPEN:
			RecvGateOpen(pData+1);
			break;
	}
}

void CAISocket::CloseProcess()
{
	CString logstr;
	CTime time = CTime::GetCurrentTime();
	logstr.Format("*** CloseProcess - socketID=%d...  ***  %d-%d-%d, %d:%d]\r\n", m_Sid, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute() );
	LogFileWrite( logstr );

	Initialize();

	CIOCPSocket2::CloseProcess();
}

// sungyong 2002.05.23
void CAISocket::LoginProcess( char* pBuf )
{
	int index = 0;
	float fReConnectEndTime = 0.0f;
	BYTE ver = GetByte(pBuf, index);
	BYTE byReConnect = GetByte(pBuf, index);	// 0 : 泥