// ZipFileHeader.cpp: implementation of the CZipFileHeader class.
//
////////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2000 Tadeusz Dracz.
//  For conditions of distribution and use, see copyright notice in ZipArchive.h
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZipFileHeader.h"
#include "zlib.h"
#include "ZipAutoBuffer.h"
#include "ZipArchive.h"

#define ZipFileHeaderSIZE	46
#define LOCALZipFileHeaderSIZE	30
#define VERSIONMADEBY 20
#define ENCR_HEADER_LEN 12
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char CZipFileHeader::m_gszSignature[] = {0x50, 0x4b, 0x01, 0x02};
char CZipFileHeader::m_gszLocalSignature[] = {0x50, 0x4b, 0x03, 0x04};
CZipFileHeader::CZipFileHeader()
{
	m_uExternalAttr = FILE_ATTRIBUTE_ARCHIVE;
	m_uModDate = m_uModTime = m_uInternalAttr = 0;
	m_uMethod = Z_DEFLATED;
}

CZipFileHeader::~CZipFileHeader()
{

}

// read the header from the central dir
bool CZipFileHeader::Read(CZipStorage *pStorage)
{
// 	// just in case
// 	m_pszComment.Release();
// 	m_pszFileName.Release();
	WORD uFileNameSize, uCommentSize, uExtraFieldSize;
	CZipAutoBuffer buf(ZipFileHeaderSIZE);
	pStorage->Read(buf, ZipFileHeaderSIZE, true);		
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
	memcpy(&uFileNameSize,		buf + 28, 2);
	memcpy(&uExtraFieldSize,	buf + 30, 2);
	memcpy(&uCommentSize,		buf + 32, 2);
	memcpy(&m_uDiskStart,		buf + 34, 2);
	memcpy(&m_uInternalAttr,	buf + 36, 2);
	memcpy(&m_uExternalAttr,	buf + 38, 4);
	memcpy(&m_uOffset,			buf + 42, 4);
	buf.Release();

	if (memcmp(m_szSignature, m_gszSignature, 4) != 0)
		return false;

	int iCurDsk = pStorage->GetCurrentDisk();
	m_pszFileName.Allocate(uFileNameSize); // don't add NULL at the end
	pStorage->m_pFile->Read(m_pszFileName, uFileNameSize);
	if (uExtraFieldSize)
	{
		ASSERT(!m_pExtraField.IsAllocated());
		m_pExtraField.Allocate(uExtraFieldSize);
		pStorage->m_pFile->Read(m_pExtraField, uExtraFieldSize);
	}
	if (uCommentSize)
	{
		m_pszComment.Allocate(uCommentSize);
		pStorage->m_pFile->Read(m_pszComment, uCommentSize);
	}

	return pStorage->GetCurrentDisk() == iCurDsk; // check that the while header is on the one disk
}

// return CTime representation of m_uModDate, m_uModTime
CTime CZipFileHeader::GetTime()
{
	return CTime(m_uModDate, m_uModTime);
}

// write the header to the central dir
DWORD CZipFileHeader::Write(CZipStorage *pStorage)
{
	WORD uFileNameSize = GetFileNameSize(), uCommentSize = GetCommentSize(),
		uExtraFieldSize = GetExtraFieldSize();
	DWORD iSize = GetSize();
	CZipAutoBuffer buf(iSize);
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
	memcpy(buf + 28, &uFileNameSize, 2);
	memcpy(buf + 30, &uExtraFieldSize, 2);
	memcpy(buf + 32, &uCommentSize, 2);
	memcpy(buf + 34, &m_uDiskStart, 2);
	memcpy(buf + 36, &m_uInternalAttr, 2);
	memcpy(buf + 38, &m_uExternalAttr, 4);
	memcpy(buf + 42, &m_uOffset, 4);

	memcpy(buf + 46, m_pszFileName, uFileNameSize);

	if (uExtraFieldSize)
		memcpy(buf + 46 + uFileNameSize, m_pExtraField, uExtraFieldSize);

	if (uCommentSize)
		memcpy(buf + 46 + uFileNameSize + uExtraFieldSize, m_pszComment, uCommentSize);

	pStorage->Write(buf, iSize, true);
	return iSize;
}

// read local header
bool CZipFileHeader::ReadLocal(CZipStorage *pStorage, WORD& iLocExtrFieldSize)
{
	char buf[LOCALZipFileHeaderSIZE];
	pStorage->Read(buf, LOCALZipFileHeaderSIZE, true);
	if (memcmp(buf, m_gszLocalSignature, 4) != 0)
		return false;

	bool bIsDataDescr = (((WORD)*(buf + 6)) & 8) != 0;

	WORD uFileNameSize = GetFileNameSize();
	if ((memcmp(buf + 6, &m_uFlag, 2) != 0)
		||(memcmp(buf + 8, &m_uMethod, 2) != 0)
		|| (m_uMethod && (m_uMethod != Z_DEFLATED))
		|| (memcmp(buf + 26, &uFileNameSize, 2) != 0))
		return false;

// jeszcze mo