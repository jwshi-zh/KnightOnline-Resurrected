// SharedMem.cpp: implementation of the CSharedMemQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SharedMem.h"
#include <process.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void aa() {};		// nop function

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSharedMemQueue::CSharedMemQueue()
{
	m_hMMFile = NULL;
	m_lpMMFile = NULL;
	m_bMMFCreate = FALSE;
	m_nMaxCount = 0;
	m_wOffset = 0;
	m_pHeader = NULL;
}

CSharedMemQueue::~CSharedMemQueue()
{
	if( m_lpMMFile )
		UnmapViewOfFile(m_lpMMFile);
	if( m_hMMFile )
		CloseHandle(m_hMMFile);
}

BOOL CSharedMemQueue::InitailizeMMF(DWORD dwOffsetsize, int maxcount, LPCTSTR lpname, BOOL bCreate )
{
	char logstr[256];
	memset( logstr, 0x00, 256);
	if( maxcount < 1 )
		return FALSE;
	DWORD dwfullsize = dwOffsetsize * maxcount + sizeof(_SMQ_HEADER);

	m_nMaxCount = maxcount;
	m_wOffset = dwOffsetsize;

	if( bCreate )
		m_hMMFile = CreateFileMapping( (HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, dwfullsize, lpname );
	else
		m_hMMFile = OpenFileMapping( FILE_MAP_ALL_ACCESS, TRUE, lpname );
	
	if( m_hMMFile == NULL ) {
		strcpy( logstr , "Shared Memory Open Fail!!\r\n" );
		LogFileWrite( logstr );
		return FALSE;
	}

    m_lpMMFile = (char *)MapViewOfFile(m_hMMFile, FILE_MAP_WRITE, 0, 0, 0);
	if( !m_lpMMFile )
		return FALSE;
	TRACE("%s Address : %x\n", lpname, m_lpMMFile);

	m_bMMFCreate = bCreate;
	m_pHeader = (_SMQ_HEADER *)m_lpMMFile;
	m_lReference = (LONG)(m_lpMMFile + sizeof(_SMQ_HEADER));		// 珥