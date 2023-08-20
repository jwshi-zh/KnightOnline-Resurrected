// IOCPort.cpp: implementation of the CIOCPort class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOCPort.h"
#include "IOCPSocket2.h"
#include "Define.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CRITICAL_SECTION g_critical;

DWORD WINAPI AcceptThread(LPVOID lp);
DWORD WINAPI ReceiveWorkerThread(LPVOID lp);
DWORD WINAPI ClientWorkerThread(LPVOID lp);
DWORD WINAPI SendWorkerThread(LPVOID lp);

DWORD WINAPI AcceptThread(LPVOID lp)
{
	CIOCPort* pIocport = (CIOCPort*) lp;

	WSANETWORKEVENTS	network_event;
	DWORD				wait_return;
	int					sid;
	CIOCPSocket2*		pSocket = NULL;
	char				logstr[1024];
	memset( logstr, NULL, 1024 );

	struct sockaddr_in  addr;
	int					len;

	while(1)
	{
		wait_return = WaitForSingleObject( pIocport->m_hListenEvent, INFINITE);

		if(wait_return == WAIT_FAILED)
		{
			TRACE("Wait failed Error %d\n", GetLastError());
			char logstr[1024]; memset( logstr, NULL, 1024 );
			sprintf( logstr, "Wait failed Error %d\r\n", GetLastError());
			LogFileWrite( logstr );
			return 1;
		}

		WSAEnumNetworkEvents( pIocport->m_ListenSocket, pIocport->m_hListenEvent, &network_event);

		if(network_event.lNetworkEvents &FD_ACCEPT)
		{
			if(network_event.iErrorCode[FD_ACCEPT_BIT] == 0 ) 
			{
				sid = pIocport->GetNewSid();
				if(sid < 0) {
					TRACE("Accepting User Socket Fail - New Uid is -1\n");
					char logstr[1024]; memset( logstr, NULL, 1024 );
					sprintf( logstr, "Accepting User Socket Fail - New Uid is -1\r\n");
					LogFileWrite( logstr );
					goto loop_pass_accept;
				}

				pSocket = pIocport->GetIOCPSocket( sid );
				if( !pSocket ) {
					TRACE("Socket Array has Broken...\n");
					char logstr[1024]; memset( logstr, NULL, 1024 );
					sprintf( logstr, "Socket Array has Broken...\r\n");
					LogFileWrite( logstr );
//					pIocport->PutOldSid( sid );				// Invalid sid must forbidden to use
					goto loop_pass_accept;
				}

				len = sizeof(addr);
				if( !pSocket->Accept( pIocport->m_ListenSocket, (struct sockaddr *)&addr, &len ) ) {
					TRACE("Accept Fail %d\n", sid);
					char logstr[1024]; memset( logstr, NULL, 1024 );
					sprintf( logstr, "Accept Fail %d\r\n", sid);
					LogFileWrite( logstr );
					pIocport->RidIOCPSocket( sid, pSocket );
					pIocport->PutOldSid( sid );
					goto loop_pass_accept;
				}

				pSocket->InitSocket( pIocport );

				if( !pIocport->Associate( pSocket, pIocport->m_hServerIOCPort ) ) {
					TRACE("Socket Associate Fail\n");
					char logstr[1024]; memset( logstr, NULL, 1024 );
					sprintf( logstr, "Socket Associate Fail\r\n");
					LogFileWrite( logstr );
					pSocket->CloseProcess();
					pIocport->RidIOCPSocket( sid, pSocket );
					pIocport->PutOldSid( sid );
					goto loop_pass_accept;
				}

				// Crytion
				//pSocket->SendCryptionKey();	// 