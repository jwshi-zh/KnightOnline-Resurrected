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
			return 1;
		}

		WSAEnumNetworkEvents( pIocport->m_ListenSocket, pIocport->m_hListenEvent, &network_event);

		if(network_event.lNetworkEvents &FD_ACCEPT)
		{
			if(network_event.iErrorCode[FD_ACCEPT_BIT] == 0 ) 
			{
				sid = pIocport->GetNewSid();
				if(sid == -1) {
					TRACE("Accepting User Socket Fail - New Uid is -1\n");
					goto loop_pass_accept;
				}

				pSocket = pIocport->GetIOCPSocket( sid );
				if( !pSocket ) {
					TRACE("Socket Array has Broken...\n");
					sprintf( logstr, "Socket Array has Broken...[sid:%d]\r\n", sid);
					LogFileWrite( logstr );
//					pIocport->PutOldSid( sid );				// Invalid sid must forbidden to use
					goto loop_pass_accept;
				}

				len = sizeof(addr);
				if( !pSocket->Accept( pIocport->m_ListenSocket, (struct sockaddr *)&addr, &len ) ) {
					TRACE("Accept Fail %d\n", sid);
					pIocport->RidIOCPSocket( sid, pSocket );
					pIocport->PutOldSid( sid );
					goto loop_pass_accept;
				}

				pSocket->InitSocket( pIocport );

				if( !pIocport->Associate( pSocket, pIocport->m_hServerIOCPort ) ) {
					TRACE("Socket Associate Fail\n");
					pSocket->CloseProcess();
					pIocport->RidIOCPSocket( sid, pSocket );
					pIocport->PutOldSid( sid );
					goto loop_pass_accept;
				}

				pSocket->Receive();

				TRACE("Success Accepting...%d\n", sid);
			}

loop_pass_accept:
			continue;
		}
	}
	
	return 1;
}

DWORD WINAPI ReceiveWorkerThread(LPVOID lp)
{
	CIOCPort* pIocport = (CIOCPort*) lp;

	DWORD			WorkIndex;	
	BOOL			b;
	LPOVERLAPPED	pOvl;
	DWORD			nbytes;
	DWORD			dwFlag = 0;
	CIOCPSocket2*	pSocket = NULL;

	while (1)
	{
		b = GetQueuedCompletionStatus( 
									  pIocport->m_hServerIOCPort,
									  &nbytes,
									  &WorkIndex,
									  &pOvl,
									  INFINITE);
		if(b || pOvl) 
		{
			if(b)
			{
				if( WorkIndex > (DWORD)pIocport->m_SocketArraySize )
					goto loop_pass;
				pSocket = (CIOCPSocket2 *)pIocport->m_SockArray[WorkIndex];
				if( !pSocket )
					goto loop_pass;

				switch( pOvl->Offset )
				{
				case	OVL_RECEIVE:
					if( !nbytes ) {
						EnterCriticalSection( &g_critical );
						TRACE("User Closed By 0 byte Notify...%d\n", WorkIndex);
						pSocket->CloseProcess();
						pIocport->RidIOCPSocket( pSocket->GetSocketID(), pSocket );
						pIocport->PutOldSid( pSocket->GetSocketID() );
						LeaveCriticalSection( &g_critical );
						break;
					}

					pSocket->m_nPending = 0;
					pSocket->m_nWouldblock = 0;

					pSocket->ReceivedData((int)nbytes);
					pSocket->Receive();

					break;
				case	OVL_SEND:
					pSocket->m_nPending = 0;
					pSocket->m_nWouldblock = 0;

					break;
				case	OVL_CLOSE:
					EnterCriticalSection( &g_critical );
					TRACE("User Closed By Close()...%d\n", WorkIndex);

					pSocket->CloseProcess();
					pIocport->RidIOCPSocket( pSocket->GetSocketID(), pSocket );
					pIocport->PutOldSid( pSocket->GetSocketID() );

					LeaveCriticalSection( &g_critical );
					break;
				default:
					break;
				}
			}
			else {
				if( pOvl ) {
					if( WorkIndex > (DWORD)pIocport->m_SocketArraySize )
						goto loop_pass;
					pSocket = (CIOCPSocket2 *)pIocport->m_SockArray[WorkIndex];
					if( !pSocket )
						goto loop_pass;

					EnterCriticalSection( &g_critical );

					TRACE("User Closed By Abnormal Termination...%d\n", WorkIndex);
					pSocket->CloseProcess();
					pIocport->RidIOCPSocket( pSocket->GetSocketID(), pSocket );
					pIocport->PutOldSid( pSocket->GetSocketID() );
					
					LeaveCriticalSection( &g_critical );
				}
			}
		}

loop_pass:
		continue;
	}

	return 1;
}

DWORD WINAPI ClientWorkerThread(LPVOID lp)
{
	CIOCPort* pIocport = (CIOCPort*) lp;

	DWORD			WorkIndex;	
	BOOL			b;
	LPOVERLAPPED	pOvl;
	DWORD			nbytes;
	DWORD			dwFlag = 0;
	CIOCPSocket2*	pSocket = NULL;

	while (1)
	{
		b = GetQueuedCompletionStatus( 
									  pIocport->m_hClientIOCPort,
									  &nbytes,
									  &WorkIndex,
									  &pOvl,
									  INFINITE);
		if(b || pOvl) 
		{
			if(b)
			{
				if( WorkIndex > (DWORD)pIocport->m_ClientSockSize )
					goto loop_pass;
				pSocket = (CIOCPSocket2 *)pIocport->m_ClientSockArray[WorkIndex];
				if( !pSocket )
					goto loop_pass;

				switch( pOvl->Offset )
				{
				case	OVL_RECEIVE:
					EnterCriticalSection( &g_critical );
					if( !nbytes ) {
						TRACE("AISocket Closed By 0 Byte Notify\n" );
						pSocket->CloseProcess();
						pIocport->RidIOCPSocket( pSocket->GetSocketID(), pSocket );
//						pIocport->PutOldSid( pSocket->GetSocketID() );		// 