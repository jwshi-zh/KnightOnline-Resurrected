// IOCPSocket2.cpp: implementation of the CIOCPSocket2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOCPSocket2.h"
#include "CircularBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOCPSocket2::CIOCPSocket2()
{
	m_pBuffer = new CCircularBuffer(SOCKET_BUFF_SIZE);
	m_Socket = INVALID_SOCKET;

	m_pIOCPort = NULL;
	m_Type = TYPE_ACCEPT;
}

CIOCPSocket2::~CIOCPSocket2()
{
	delete m_pBuffer;
}


BOOL CIOCPSocket2::Create( UINT nSocketPort, int nSocketType, long lEvent, LPCTSTR lpszSocketAddress)
{
	int ret;

	m_Socket = socket( AF_INET, nSocketType/*SOCK_STREAM*/, 0 );
	if( m_Socket == INVALID_SOCKET ) {
		ret = WSAGetLastError();
		TRACE("Socket Create Fail! - %d\n", ret);
		return FALSE;
	}

	m_hSockEvent = WSACreateEvent();
	if( m_hSockEvent == WSA_INVALID_EVENT ) {
		ret = WSAGetLastError();
		TRACE("Event Create Fail! - %d\n", ret);
		return FALSE;
	}

	return TRUE;
}

BOOL CIOCPSocket2::Connect( CIOCPort* pIocp, LPCTSTR lpszHostAddress, UINT nHostPort )
{
	struct sockaddr_in addr;

	memset((void *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(lpszHostAddress);
	addr.sin_port = htons(nHostPort);

	int result = connect( m_Socket,(struct sockaddr *)&addr,sizeof(addr) );
	if ( result == SOCKET_ERROR )
	{
		int err = WSAGetLastError();
//		TRACE("CONNECT FAIL : %d\n", err);
		closesocket( m_Socket );
		return FALSE;
	}

	ASSERT( pIocp );

	InitSocket( pIocp );

	m_Sid = m_pIOCPort->GetClientSid();
	if( m_Sid < 0 )
		return FALSE;

	m_pIOCPort->m_ClientSockArray[m_Sid] = this;
	
	if ( !m_pIOCPort->Associate(this, m_pIOCPort->m_hClientIOCPort) )
	{
		TRACE("Socket Connecting Fail - Associate\n");
		return FALSE;
	}

	m_ConnectAddress = lpszHostAddress;
	m_State = STATE_CONNECTED;
	m_Type = TYPE_CONNECT;

	Receive();

	return TRUE;
}

int CIOCPSocket2::Send(char *pBuf, long length, int dwFlag)
{
	int ret_value = 0;
	WSABUF out;
	DWORD sent = 0;
	OVERLAPPED *pOvl;
	HANDLE	hComport = NULL;

	if( length > MAX_PACKET_SIZE )
		return 0;

	char pTBuf[MAX_PACKET_SIZE];
	memset( pTBuf, 0x00, MAX_PACKET_SIZE );
	int index = 0;

	pTBuf[index++] = (BYTE)PACKET_START1;
	pTBuf[index++] = (BYTE)PACKET_START2;
	memcpy( pTBuf+index, &length, 2 );
	index += 2;
	memcpy( pTBuf+index, pBuf, length );
	index += length;
	pTBuf[index++] = (BYTE)PACKET_END1;
	pTBuf[index++] = (BYTE)PACKET_END2;

	out.buf = pTBuf;
	out.len = index;
	
	pOvl = &m_SendOverlapped;
	pOvl->Offset = OVL_SEND;
	pOvl->OffsetHigh = out.len;

	ret_value = WSASend( m_Socket, &out, 1, &sent, dwFlag, pOvl, NULL);
	
	if ( ret_value == SOCKET_ERROR )
	{
		int last_err;
		last_err = WSAGetLastError();

		if ( last_err == WSA_IO_PENDING ) {
			TRACE("SEND : IO_PENDING[SID=%d]\n", m_Sid);
			m_nPending++;
			if( m_nPending > 3 )
				goto close_routine;
			sent = length; 
		}
		else if ( last_err == WSAEWOULDBLOCK )
		{
			TRACE("SEND : WOULDBLOCK[SID=%d]\n", m_Sid);

			m_nWouldblock++;
			if( m_nWouldblock > 3 )
				goto close_routine;
			return 0;
		}
		else
		{
			TRACE("SEND : ERROR [SID=%d] - %d\n", m_Sid, last_err);
			m_nSocketErr++;
			goto close_routine;
		}
	}
	else if ( !ret_value )
	{
		m_nPending = 0;
		m_nWouldblock = 0;
		m_nSocketErr = 0;
	}

	return sent;

close_routine:
	pOvl = &m_RecvOverlapped;
	pOvl->Offset = OVL_CLOSE;
	
	if( m_Type == TYPE_ACCEPT )
		hComport = m_pIOCPort->m_hServerIOCPort;
	else
		hComport = m_pIOCPort->m_hClientIOCPort;
	
	PostQueuedCompletionStatus( hComport, (DWORD)0, (DWORD)m_Sid, pOvl );
	
	return -1;
}

int CIOCPSocket2::Receive()
{
	int RetValue;
	WSABUF in;
	DWORD insize, dwFlag=0;
	OVERLAPPED *pOvl;
	HANDLE	hComport = NULL;

	memset(m_pRecvBuff, NULL, MAX_PACKET_SIZE );
	in.len = MAX_PACKET_SIZE;
	in.buf = m_pRecvBuff;

	pOvl = &m_RecvOverlapped;
	pOvl->Offset = OVL_RECEIVE;

	RetValue = WSARecv( m_Socket, &in, 1, &insize, &dwFlag, pOvl, NULL );

 	if ( RetValue == SOCKET_ERROR )
	{
		int last_err;
		last_err = WSAGetLastError();

		if ( last_err == WSA_IO_PENDING ) {
//			TRACE("RECV : IO_PENDING[SID=%d]\n", m_Sid);
//			m_nPending++;
//			if( m_nPending > 3 )
//				goto close_routine;
			return 0;
		}
		else if ( last_err == WSAEWOULDBLOCK )
		{
			TRACE("RECV : WOULDBLOCK[SID=%d]\n", m_Sid);

			m_nWouldblock++;
			if( m_nWouldblock > 3 )
				goto close_routine;
			return 0;
		}
		else
		{
			TRACE("RECV : ERROR [SID=%d] - %d\n", m_Sid, last_err);

			m_nSocketErr++;
			if( m_nSocketErr == 2 )
				goto close_routine;
			return -1;
		}
	}

	return (int)insize;

close_routine:
	pOvl = &m_RecvOverlapped;
	pOvl->Offset = OVL_CLOSE;
	
	if( m_Type == TYPE_ACCEPT )
		hComport = m_pIOCPort->m_hServerIOCPort;
	else
		hComport = m_pIOCPort->m_hClientIOCPort;
	
	PostQueuedCompletionStatus( hComport, (DWORD)0, (DWORD)m_Sid, pOvl );
	
	return -1;
}

void CIOCPSocket2::ReceivedData(int length)
{
	if(!length) return;

	int len = 0;

	if( !strlen(m_pRecvBuff) )		// 