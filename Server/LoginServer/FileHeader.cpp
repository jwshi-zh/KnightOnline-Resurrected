// FileHeader.cpp: implementation of the CFileHeader class.
//
////////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2000 Tadeusz Dracz.
//  For conditions of distribution and use, see copyright notice in ZipArchive.h
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileHeader.h"
#include "zlib.h"
#include "AutoBuffer.h"

#define FILEHEADERSIZE	46
#define LOCALFILEHEADERSIZE	30
#define VERSIONMADEBY 20
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char CFileHeader::m_gszSignature[] = {0x50, 0x4b, 0x01, 0x02};
char CFileHeader::m_gszLocalSignature[] = {0x50, 0x4b, 0x03, 0x04};
CFileHeader::CFileHeader()
{
	m_uExternalAttr = FILE_ATTRIBUTE_ARCHIVE;
	m_uModDate = m_uModTime = m_uInternalAttr = 0;
	m_uMethod = Z_DEFLATED;
}

CFileHeader::~CFileHeader()
{

}

// read the header from the central dir
bool CFileHeader::Read(CZipStorage *pStorage)
{
	CAutoBuffer buf(FILEHEADERSIZE);
	pStorage->Read(buf, FILEHEADERSIZE, true);		
	memcpy(&m_szSignature,		buf, 4);
	memcpy(&m_uVersionMadeBy,	buf + 4, 2);
	memcpy(&m_uVersionNeeded,	buf + 6, 2);
	memcpy(&m_uFlag,			buf + 8, 2);
	memcpy(&m_uMethod,			buf + 10, 2);
	memcpy(&m_uModTime,			buf + 12, 2);
	memcpy(&m_uModDate,			buf + 14, 2);
	memcpy(&m_uCrc32,			buf + 16, 4);
	memcpy(&m_uComprSize,		buf + 20, 4);
	memcpy(&m_uUncomprSize,		buf + 24, 4);
	memcpy(&m_uFileNameSize,	buf + 28, 2);
	memcpy(&m_uExtraFieldSize,	buf + 30, 2);
	memcpy(&m_uCommentSize,		buf + 32, 2);
	memcpy(&m_uDiskStart,		buf + 34, 2);
	memcpy(&m_uInternalAttr,	buf + 36, 2);
	memcpy(&m_uExternalAttr,	buf + 38, 4);
	memcpy(&m_uOffset,			buf + 42, 4);
	buf.Release();

	if (memcmp(m_szSignature, m_gszSignature, 4) != 0)
		return false;

	int iCurDsk = pStorage->GetCurrentDisk();
	pStorage->m_file.Read(m_szFileName.GetBuffer(m_uFileNameSize), m_uFileNameSize);
	m_szFileName.ReleaseBuffer(m_uFileNameSize);
	if (m_uExtraFieldSize)
	{
		ASSERT(!m_pExtraField.IsAllocated());
		m_pExtraField.Allocate(m_uExtraFieldSize);
		pStorage->m_file.Read(m_pExtraField, m_uExtraFieldSize);
	}
	if (m_uCommentSize)
	{
		pStorage->m_file.Read(m_szComment.GetBuffer(m_uCommentSize), m_uCommentSize);
		m_szComment.ReleaseBuffer(m_uCommentSize);
	}

	return pStorage->GetCurrentDisk() == iCurDsk; // check that the while header is on the one disk
}

// return CTime representation of m_uModDate, m_uModTime
CTime CFileHeader::GetTime()
{
	return CTime(m_uModDate, m_uModTime);
}

// write the header to the central dir
DWORD CFileHeader::Write(CZipStorage *pStorage)
{
	DWORD iSize = GetSize();
	CAutoBuffer buf(iSize);
	memcpy(buf, &m_szSignature, 4);
	memcpy(buf + 4, &m_uVersionMadeBy, 2);
	memcpy(buf + 6, &m_uVersionNeeded, 2);
	memcpy(buf + 8, &m_uFlag, 2);
	memcpy(buf + 10, &m_uMethod, 2);
	memcpy(buf + 12, &m_uModTime, 2);
	memcpy(buf + 14, &m_uModDate, 2);
	memcpy(buf + 16, &m_uCrc32, 4);
	memcpy(buf + 20, &m_uComprSize, 4);
	memcpy(buf + 24, &m_uUncomprSize, 4);
	memcpy(buf + 28, &m_uFileNameSize, 2);
	memcpy(buf + 30, &m_uExtraFieldSize, 2);
	memcpy(buf + 32, &m_uCommentSize, 2);
	memcpy(buf + 34, &m_uDiskStart, 2);
	memcpy(buf + 36, &m_uInternalAttr, 2);
	memcpy(buf + 38, &m_uExternalAttr, 4);
	memcpy(buf + 42, &m_uOffset, 4);

	ASSERT(m_uFileNameSize == m_szFileName.GetLength());
	ASSERT(m_uExtraFieldSize == m_pExtraField.GetSize());
	ASSERT(m_uCommentSize == m_szComment.GetLength());
	memcpy(buf + 46, (LPCTSTR)m_szFileName, m_uFileNameSize);

	if (m_uExtraFieldSize)
		memcpy(buf + 46 + m_uFileNameSize, m_pExtraField, m_uExtraFieldSize);

	if (m_uCommentSize)
		memcpy(buf + 46 + m_uFileNameSize + m_uExtraFieldSize, (LPCTSTR)m_szComment, m_uCommentSize);

	pStorage->Write(buf, iSize, true);
	return iSize;
}

// read local header
bool CFileHeader::ReadLocal(CZipStorage *pStorage, WORD& iLocExtrFieldSize, bool& bIsDataDescr)
{
	char buf[LOCALFILEHEADERSIZE];
	pStorage->Read(buf, LOCALFILEHEADERSIZE, true);
	if (memcmp(buf, m_gszLocalSignature, 4) != 0)
		return false;

	// data descriptor present
	bIsDataDescr = (((WORD)*(buf + 6)) & 8) != 0;

	if ((memcmp(buf + 8, &m_uMethod, 2) != 0)
		|| (m_uMethod && (m_uMethod != Z_DEFLATED))
		|| (memcmp(buf + 26, &m_uFileNameSize, 2) != 0))
		return false;

// jeszcze mo