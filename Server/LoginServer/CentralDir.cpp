// CentralDir.cpp: implementation of the CCentralDir class.
//
////////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2000 Tadeusz Dracz.
//  For conditions of distribution and use, see copyright notice in ZipArchive.h
////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "CentralDir.h"
#include "ZipArchive.h"

struct CAutoHandle
{
	HANDLE m_hFileMap;
	LPVOID m_pFileMap;
	CAutoHandle()
	{
		m_hFileMap = NULL;
		m_pFileMap = NULL;
	}
	bool CreateMapping(HANDLE hFile)
	{
		m_hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE,
			0, 0, "ZipArchive Mapping File");
		if (!m_hFileMap)
			return false;
		// Get pointer to memory representing file
		m_pFileMap = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0);
		return (m_pFileMap != NULL);
	}
	void RemoveMapping()
	{
		if (m_pFileMap)
		{
			UnmapViewOfFile(m_pFileMap);
			m_pFileMap = NULL;
		}
		if (m_hFileMap)
		{
			CloseHandle(m_hFileMap);
			m_hFileMap = NULL;
		}
		
	}
	~CAutoHandle()
	{
		RemoveMapping();
	}
};

#define CENTRALDIRSIZE	22

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
char CCentralDir::m_gszSignature[] = {0x50, 0x4b, 0x05, 0x06};
CCentralDir::CCentralDir()
{
	m_pStorage = NULL;
	m_pOpenedFile = NULL;
	m_iBufferSize = 32768;
	
}

void CCentralDir::Init()
{
	m_bOnDisk = false;
	m_uBytesBeforeZip = m_uCentrDirPos = 0;
	m_pOpenedFile = NULL;
	m_szComment.Empty();
}

CCentralDir::~CCentralDir()
{
	Clear();
}

void CCentralDir::Read()
{
	ASSERT(m_pStorage);

	m_uCentrDirPos = Locate();
	m_pStorage->m_file.Seek(m_uCentrDirPos, CFile::begin);
	CAutoBuffer buf(CENTRALDIRSIZE);
	m_pStorage->m_file.Read(buf, CENTRALDIRSIZE);
	memcpy(&m_szSignature,		buf, 4);
	memcpy(&m_uThisDisk,		buf + 4, 2);
	memcpy(&m_uDiskWithCD,		buf + 6, 2);
	memcpy(&m_uDiskEntriesNo,	buf + 8, 2);
	memcpy(&m_uEntriesNumber,	buf + 10, 2);
	memcpy(&m_uSize,			buf + 12, 4);
	memcpy(&m_uOffset,			buf + 16, 4);
	memcpy(&m_uCommentSize,		buf + 20, 2);
	buf.Release();


	m_pStorage->UpdateSpanMode(m_uThisDisk);
	// if m_uThisDisk is not zero, it is enough to say that it is multi archive
	ASSERT((!m_uThisDisk && (m_uEntriesNumber == m_uDiskEntriesNo) && !m_uDiskWithCD) || m_uThisDisk);

			

	if (!m_pStorage->IsSpanMode() && ((DWORD)m_uCentrDirPos < m_uOffset + m_uSize))
		ThrowError(ZIP_BADZIPFILE);

	if (m_uCommentSize)
	{
		m_pStorage->m_file.Read(m_szComment.GetBuffer(m_uCommentSize), m_uCommentSize);
		m_szComment.ReleaseBuffer(m_uCommentSize);
	}
	
	m_uBytesBeforeZip = m_pStorage->IsSpanMode() ? 0 : m_uCentrDirPos - m_uSize - m_uOffset;

	if ((!m_uSize && m_uEntriesNumber) || (!m_uEntriesNumber && m_uSize))
		ThrowError(ZIP_BADZIPFILE);

	m_bOnDisk = true;
	m_pStorage->ChangeDisk(m_uDiskWithCD);

	if (!m_uSize)
		return;

	ReadHeaders();
}

// return the location of the beginning of the "end" record in the file
DWORD CCentralDir::Locate()
{

	// maximum size of end of central dir record
	long uMaxRecordSize = 0xffff + CENTRALDIRSIZE;
	DWORD uFileSize = m_pStorage->m_file.GetLength();

	if ((DWORD)uMaxRecordSize > uFileSize)
		uMaxRecordSize = uFileSize;

	CAutoBuffer buf(m_iBufferSize);

	long uPosInFile = 0;
	int uRead = 0;
	// backward reading
	while (uPosInFile < uMaxRecordSize)
	{
		uPosInFile = uRead + m_iBufferSize;
		if (uPosInFile > uMaxRecordSize)
			uPosInFile = uMaxRecordSize;

		int iToRead = uPosInFile - uRead;

		m_pStorage->m_file.Seek(-uPosInFile, CFile::end);
		m_pStorage->m_file.Read(buf, iToRead);
		// search from the very last bytes to prevent an error if inside archive 
		// there are packed other arhives
		for (int i = iToRead - 4; i >=0 ; i--)
			if (!memcmp((char*)buf + i, m_gszSignature, 4))	
				return uFileSize - (uPosInFile - i);

		uRead += iToRead - 3;

	}
	
	ThrowError(ZIP_BADZIPFILE);
	return 0;
}

void CCentralDir::ThrowError(int err)
{
	AfxThrowZipException(err, m_pStorage->m_file.GetFilePath());
}


void CCentralDir::ReadHeaders()
{
	m_pStorage->m_file.Seek(m_uOffset + m_uBytesBeforeZip, CFile::begin);

	for (int i = 0; i < m_uEntriesNumber; i++)
	{
		CFileHeader* pHeader = new CFileHeader;
		m_headers.Add(pHeader); // bezpo