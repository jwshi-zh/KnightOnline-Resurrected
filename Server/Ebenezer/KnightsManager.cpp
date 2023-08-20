// KnightsManager.cpp: implementation of the CKnightsManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ebenezer.h"
#include "KnightsManager.h"
//#include "Knights.h"
#include "User.h"
#include "GameDefine.h"
#include "PacketDefine.h"
#include "EbenezerDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKnightsManager::CKnightsManager()
{
	m_pMain = NULL;
	
/*	CString strConnect;
	strConnect.Format (_T("ODBC;DSN=%s;UID=%s;PWD=%s"), "KN_Online", "knight", "knight");
	m_KnightsDB.SetLoginTimeout (10);
	if( !m_KnightsDB.Open(NULL,FALSE,FALSE,strConnect) )
		AfxMessageBox("KnightsDB Connection Fail...");	*/
}

CKnightsManager::~CKnightsManager()
{

}

void CKnightsManager::PacketProcess(CUser *pUser, char *pBuf)
{
	int index = 0;

	BYTE command = GetByte( pBuf, index );
	
	if( !pUser ) return;

	switch( command ) {
	case KNIGHTS_CREATE:
		CreateKnights( pUser, pBuf+index );
		break;
	case KNIGHTS_JOIN:
		JoinKnights( pUser, pBuf+index );
		break;
	case KNIGHTS_WITHDRAW:
		WithdrawKnights( pUser, pBuf+index );
		break;
	case KNIGHTS_REMOVE:
	case KNIGHTS_ADMIT:
	case KNIGHTS_REJECT:
	case KNIGHTS_CHIEF:
	case KNIGHTS_VICECHIEF:
	case KNIGHTS_OFFICER:
	case KNIGHTS_PUNISH:
		ModifyKnightsMember( pUser, pBuf+index, command );
		break;
	case KNIGHTS_DESTROY:
		DestroyKnights( pUser );
		break;
	case KNIGHTS_ALLLIST_REQ:
		AllKnightsList( pUser, pBuf+index );
		break;
	case KNIGHTS_MEMBER_REQ:
		AllKnightsMember( pUser, pBuf+index );
		break;
	case KNIGHTS_CURRENT_REQ:
		CurrentKnightsMember( pUser, pBuf+index );
		break;
	case KNIGHTS_STASH:
		break;
	case KNIGHTS_JOIN_REQ:
		JoinKnightsReq( pUser, pBuf+index );
		break;
	}
}

void CKnightsManager::CreateKnights(CUser* pUser, char *pBuf)
{
	int index = 0, send_index = 0, idlen = 0, knightindex = 0, ret_value = 3, week = 0;
	char idname[MAX_ID_SIZE+1]; memset( idname, 0x00, MAX_ID_SIZE+1 );
	CTime time = CTime::GetCurrentTime();

	char send_buff[256]; memset( send_buff, 0x00, 256 );

	if( !pUser ) return;

	idlen = GetShort( pBuf, index );
	if( idlen > MAX_ID_SIZE || idlen < 0 ) goto fail_return;
	GetString( idname, pBuf, idlen, index );

	if( !IsAvailableName( idname ) ) goto fail_return;
	if( pUser->m_pUserData->m_bKnights != 0 ) {			// 湲곗