#include "pch.h"
#include "BitMapFile.h"

CBitMapFile::CBitMapFile()
{
	m_pPixels = nullptr;
	Release();
}

CBitMapFile::~CBitMapFile()
{
	Release();
}

void CBitMapFile::Release()
{
	memset(&m_bmfHeader, 0, sizeof(m_bmfHeader));
	memset(&m_bmInfoHeader, 0, sizeof(m_bmInfoHeader));
	::GlobalFree(m_pPixels);
	m_pPixels = nullptr;
}

bool CBitMapFile::Load(HANDLE hFile)
{
	this->Release();

	DWORD dwRWC = 0;

	ReadFile(hFile, &m_bmfHeader, sizeof(m_bmfHeader), &dwRWC, nullptr);

	// Check the "BM" marker indicating that it is a bmp file
	if (m_bmfHeader.bfType != 0x4D42)
	{
		MessageBox(::GetActiveWindow(), "원본 파일이 bitmap파일이 아닙니다.", "error", MB_OK);
		return FALSE;
	}

	ReadFile(hFile, &m_bmInfoHeader, sizeof(m_bmInfoHeader), &dwRWC, nullptr);

	// Check bits per pixel
	const WORD wBitCount = m_bmInfoHeader.biBitCount;
	if (24 != wBitCount || m_bmInfoHeader.biWidth <= 0 || m_bmInfoHeader.biHeight <= 0)		// If it is not a 24-bit bmp, it is returned.
	{
		MessageBox(::GetActiveWindow(), "The original bitmap has a wrong width or height, or is not a 24bit file.", "error", NULL);
		return FALSE;
	}

	// Horizontal length captured in memory of the actual image (24bit)
	const int iRealWidth = ((int)((m_bmInfoHeader.biWidth*3 + 3)/4))*4;	

	// Allocate memory for the new image
	const int iDIBSize = iRealWidth * m_bmInfoHeader.biHeight;

	if ((m_pPixels = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, iDIBSize )) == nullptr)
	{
		MessageBox(::GetActiveWindow(), "메모리를 할당하지 못했습니다.", "error", MB_OK);
		return FALSE;
	}

	for(int y = m_bmInfoHeader.biHeight - 1; y >= 0; y--)
	{
		ReadFile(hFile, (BYTE*)m_pPixels + y * iRealWidth, iRealWidth, &dwRWC, nullptr);
	}

	return TRUE;
}

void* CBitMapFile::Pixels(int x, int y) const
{
	if(24 != m_bmInfoHeader.biBitCount) return nullptr;

	const int nPitch = this->Pitch();
	const int nPixelSize = 3;
	return (char*)m_pPixels + y * nPitch + x * nPixelSize;
}

bool CBitMapFile::Save(HANDLE hFile)
{
	DWORD dwRWC = 0;

	WriteFile(hFile, &m_bmfHeader, sizeof(m_bmfHeader), &dwRWC, nullptr);


	WriteFile(hFile, &m_bmInfoHeader, sizeof(m_bmInfoHeader), &dwRWC, nullptr);

	const int iRealWidth = this->Pitch();

	for(int y = m_bmInfoHeader.biHeight - 1; y >= 0; y--)
	{
		WriteFile(hFile, (BYTE*)m_pPixels + y * iRealWidth, iRealWidth, &dwRWC, nullptr);
	}

	return true;
}

bool CBitMapFile::SaveRectToFile(const std::string& szFN, RECT rc)
{
	if(szFN.empty()) return false;

	if(rc.right <= rc.left) return false;
	if(rc.bottom <= rc.top) return false;
	if(rc.left < 0) rc.left = 0;
	if(rc.top < 0) rc.top = 0;
	if(rc.right > m_bmInfoHeader.biWidth) rc.right = m_bmInfoHeader.biWidth;
	if(rc.bottom > m_bmInfoHeader.biHeight) rc.bottom = m_bmInfoHeader.biHeight;

	const int nWidth = rc.right - rc.left;
	const int nHeight = rc.bottom - rc.top;

	if (nWidth <=0 || nHeight <=0)
	{
		MessageBox(::GetActiveWindow(), "가로 세로가 0이하인 bitmap으로 저장할수 없습니다.", "error", MB_OK);
		return FALSE;
	}

	DWORD dwRWC = 0;
	const HANDLE hFile = ::CreateFile(szFN.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(::GetActiveWindow(), "원본 bitmap을 열 수 없습니다.", "error", MB_OK);
		return false;
	}

	const int iRealWidthDest = ((int)((nWidth*3 + 3)/4))*4;
	const int iDestDIBSize = sizeof(BITMAPINFOHEADER) + iRealWidthDest * nHeight;

	BITMAPFILEHEADER bmfHeaderDest = m_bmfHeader;
	bmfHeaderDest.bfType = 0x4D42;
	bmfHeaderDest.bfSize = sizeof(bmfHeaderDest) + iDestDIBSize;
	bmfHeaderDest.bfOffBits = sizeof(bmfHeaderDest) + sizeof(BITMAPINFOHEADER);

	BITMAPINFOHEADER bmInfoHeaderDest = m_bmInfoHeader;
	bmInfoHeaderDest.biSize = sizeof(bmInfoHeaderDest);
	bmInfoHeaderDest.biWidth = nWidth;
	bmInfoHeaderDest.biHeight = nHeight;
	bmInfoHeaderDest.biPlanes = 1;
	bmInfoHeaderDest.biSizeImage = iRealWidthDest * nHeight;

	WriteFile(hFile, &bmfHeaderDest, sizeof(bmfHeaderDest), &dwRWC, nullptr);

	WriteFile(hFile, &bmInfoHeaderDest, sizeof(bmInfoHeaderDest), &dwRWC, nullptr);

	const int iRealWidth = ((int)((m_bmInfoHeader.biWidth*3 + 3)/4))*4;	
	for(int y = rc.bottom - 1; y >= rc.top; y--)
	{
		const void* pPixelDest = ((byte*)m_pPixels) + iRealWidth * y + (rc.left * 3);
		WriteFile(hFile, pPixelDest, iRealWidthDest, &dwRWC, nullptr);
	}

	CloseHandle(hFile);
	return true;
}

bool CBitMapFile::LoadFromFile(const char* pszFN)
{
	if(nullptr == pszFN || lstrlen(pszFN) <= 0)
		return false;

	DWORD dwRWC = 0;
	const HANDLE hFile = ::CreateFile(pszFN, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if(INVALID_HANDLE_VALUE == hFile)
		return false;

	const bool bSuccess =	this->Load(hFile);

	CloseHandle(hFile);

	return bSuccess;
}

bool CBitMapFile::SaveToFile(const char* pszFN)
{
	if(nullptr == pszFN || lstrlen(pszFN) <= 0)
		return false;

	DWORD dwRWC = 0;
	const HANDLE hFile = ::CreateFile(pszFN, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	this->Save(hFile);

	CloseHandle(hFile);
	return true;
}

bool CBitMapFile::Create(int nWidth, int nHeight, int nBPP)
{
	if(nWidth <= 0 || nHeight <= 0) return false;
	this->Release();

	if(24 != nBPP) return FALSE;

	const int iRealWidth = ((nWidth*3 + 3)/4)*4;
	const int iDIBSize = iRealWidth * nHeight;

	if ((m_pPixels = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, iDIBSize )) == nullptr)
	{
		MessageBox(::GetActiveWindow(), "메모리를 할당하지 못했습니다.", "error", MB_OK);
		return FALSE;
	}

	memset(m_pPixels, 0, iDIBSize);

	m_bmfHeader.bfType = 0x4D42;
	m_bmfHeader.bfSize = sizeof(m_bmfHeader) + iDIBSize;
	m_bmfHeader.bfOffBits = sizeof(m_bmfHeader) + sizeof(BITMAPINFOHEADER);

	m_bmInfoHeader.biSize = sizeof(m_bmInfoHeader);
	m_bmInfoHeader.biWidth = nWidth;
	m_bmInfoHeader.biHeight = nHeight;
	m_bmInfoHeader.biBitCount = nBPP;
	m_bmInfoHeader.biPlanes = 1;
	m_bmInfoHeader.biSizeImage = iRealWidth * nHeight;

	return TRUE;
}
