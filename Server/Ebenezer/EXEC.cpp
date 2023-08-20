// EXEC.cpp: implementation of the EXEC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ebenezer.h"
#include "Define.h"
#include "EXEC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EXEC::EXEC()
{

}

EXEC::~EXEC()
{

}

void EXEC::Parse(char *pBuf)
{
	int index = 0, i = 0;
	char temp[1024];

	index += ParseSpace( temp, pBuf+index );


	if( !strcmp( temp, "SAY" ) )
	{
		m_Exec = EXEC_SAY;

		index += ParseSpace( temp, pBuf+index );	m_ExecInt[i++] = atoi( temp );		// 吏