// IOCPSocket2.cpp: implementation of the CIOCPSocket2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOCPSocket2.h"
#include "CircularBuffer.h"
#include "Compress.h"
#include "Packetdefine.h"
#include "define.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void bb() {};		// nop function

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOCPSocket2::CIOCPSocket2()
{
	m_pBuffer = new CCircularBuffer(SOCKET_BUFF_SIZE);
	m_pRegionBuffer = new _REGION_BUFFER;
	m_pCompressMng = new CCompressMng;
	m_Socket = INVALID_SOCKET;

	m_pIOCPort = NULL;
	m_Type = TYPE_ACCEPT;

	// Cryption
	m_CryptionFlag = 0;
}

CIOCPSocket2::~CIOCPSocket2()
{
	delete m_pBuffer;
	delete m_pRegionBuffer;
	delete m_pCompressMng;
}


BOOL CIOCPSocket2::Create( UINT nSocketPort, int nSocketType, long lEvent, LPCTSTR lpszSocketAddress)
{
	int ret;
	struct linger lingerOpt;

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

	// Linger off -> close socket immediately regardless of existance of data 
	//
	lingerOpt.l_onoff = 0;
	lingerOpt.l_linger = 0;

	setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char *)&lingerOpt, sizeof(lingerOpt));

	int socklen;

	socklen = SOCKET_BUFF_SIZE*4;
	setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&socklen, sizeof(socklen));
	
	socklen = SOCKET_BUFF_SIZE*4;
	setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&socklen, sizeof(socklen));
	
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

	BYTE pTBuf[MAX_PACKET_SIZE];
	BYTE pTIBuf[MAX_PACKET_SIZE];
	BYTE pTOutBuf[MAX_PACKET_SIZE];
	memset( pTBuf, 0x00, MAX_PACKET_SIZE );
	memset( pTIBuf, 0x00, MAX_PACKET_SIZE );
	memset( pTOutBuf, 0x00, MAX_PACKET_SIZE );
	int index = 0;

	if( m_CryptionFlag )
	{
		unsigned short len = length + sizeof(WORD)+1+1;

		m_Sen_val++;
		m_Sen_val &= 0x00ffffff;

		pTIBuf[0] = 0xfc; // 