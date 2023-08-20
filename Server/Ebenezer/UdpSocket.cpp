// UdpSocket.cpp: implementation of the CUdpSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ebenezer.h"
#include "UdpSocket.h"
#include "define.h"
#include "packetdefine.h"
#include "EbenezerDlg.h"
#include "AiPacket.h"
#include "Knights.h"
#include "User.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DWORD WINAPI RecvUDPThread( LPVOID lp );

DWORD WINAPI RecvUDPThread( LPVOID lp )
{
	CUdpSocket *pUdp = (CUdpSocket*)lp;
	int ret = 0, addrlen = sizeof(pUdp->m_ReplyAddress);

	while(1) {
		ret = recvfrom( pUdp->m_hUDPSocket, pUdp->m_pRecvBuf, 1024, 0, (LPSOCKADDR)&pUdp->m_ReplyAddress, &addrlen );

		if(ret == SOCKET_ERROR) {
			int err = WSAGetLastError(); 
			getpeername(pUdp->m_hUDPSocket, (SOCKADDR*)&pUdp->m_ReplyAddress, &addrlen);
			TRACE("recvfrom() error : %d IP : %s\n", err, inet_ntoa(pUdp->m_ReplyAddress.sin_addr));

			// 