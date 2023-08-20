// EVENT.cpp: implementation of the EVENT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ebenezer.h"
#include "Define.h"
#include "EVENT.h"
#include "EVENT_DATA.h"
#include "EXEC.h"
#include "LOGIC_ELSE.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EVENT::EVENT()
{

}

EVENT::~EVENT()
{
	DeleteAll();
}

BOOL EVENT::LoadEvent(int zone)
{
	DWORD		length, count;
	CString		filename;
	CFile		pFile;
	BYTE		byte;
	char		buf[4096];
	char		first[1024];
	char		temp[1024];
	int			index = 0;
	int			t_index = 0;
	int			event_num;

	EVENT_DATA	*newData = NULL;
	EVENT_DATA	*eventData = NULL;

	filename.Format( ".\\MAP\\%d.evt", zone);

	m_Zone = zone;

	if( !pFile.Open( filename, CFile::modeRead) ) return TRUE;

	length = pFile.GetLength();
	
	CArchive in(&pFile, CArchive::load);

	count = 0;

	while(count < length)
	{
		in >> byte;	count ++;

		if( (char)byte != '\r' && (char)byte != '\n' ) buf[index++] = byte;

		if(((char)byte == '\n' || count == length ) && index > 1 )
		{
			buf[index] = (BYTE) 0;

			t_index = 0;

			if( buf[t_index] == ';' || buf[t_index] == '/' )		// 二쇱