#include "pch.h"
#include "N3Texture.h"

#ifdef _N3TOOL
#include "BitmapFile.h"
#endif

CN3Texture::CN3Texture()
{
	m_dwType |= OBJ_TEXTURE;

	memset(&m_Header, 0, sizeof(m_Header));
	m_lpTexture = nullptr;
	m_iLOD = 0;
}

CN3Texture::~CN3Texture()
{
	if(m_lpTexture)
	{
		const int nRefCount = m_lpTexture->Release();
		if(nRefCount == 0) m_lpTexture = nullptr;
	}
}

void CN3Texture::Release()
{
	if(32 == m_Header.nWidth && 32 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_32X32--;
	else if(64 == m_Header.nWidth && 64 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_64X64--;
	else if(128 == m_Header.nWidth && 128 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_128X128--;
	else if(256 == m_Header.nWidth && 256 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_256X256--;
	else if(512 == m_Header.nWidth && 512 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_512X512--;
	else if(512 < m_Header.nWidth && 512 < m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_Huge--;
	else s_ResrcInfo.nTexture_Loaded_OtherSize--;

	memset(&m_Header, 0, sizeof(m_Header));
	if(m_lpTexture && m_lpTexture->Release() == 0) m_lpTexture = nullptr;
	m_iLOD = 0;

	CN3BaseFileAccess::Release();
}

bool CN3Texture::Create(int nWidth, int nHeight, D3DFORMAT Format, BOOL bGenerateMipMap)
{
	if(nWidth != nHeight)

	if(nWidth <= 1 || nHeight <= 1 || D3DFMT_UNKNOWN == Format) return false;
	if(m_lpTexture != nullptr) this->Release();

	if(s_dwTextureCaps & TEX_CAPS_POW2)
	{
		int nW, nH;

		for(nW = 1; nW <= nWidth; nW *= 2); nW /= 2;
		for(nH = 1; nH <= nHeight; nH *= 2); nH /= 2;

		nWidth = nW;
		nHeight = nH;
	}

	if((s_dwTextureCaps & TEX_CAPS_SQUAREONLY) && nWidth != nHeight)
	{
		if(nWidth > nHeight) nWidth = nHeight;
		else nHeight = nWidth;
	}

	if(nWidth > 256 && CN3Base::s_DevCaps.MaxTextureWidth <= 256) nWidth = CN3Base::s_DevCaps.MaxTextureWidth;
	if(nHeight > 256 && CN3Base::s_DevCaps.MaxTextureHeight <= 256) nHeight = CN3Base::s_DevCaps.MaxTextureHeight;

	memset(&m_Header, 0, sizeof(m_Header));

	int nMMC = 1;
	if(bGenerateMipMap)
	{
		nMMC = 0;
		for(int nW = nWidth, nH = nHeight; nW >=4 && nH >= 4; nW /=2, nH /= 2) nMMC++;
	}

	const HRESULT rval = s_lpD3DDev->CreateTexture(nWidth, nHeight, nMMC, 0, Format, D3DPOOL_MANAGED, &m_lpTexture, nullptr);

#ifdef _N3GAME
	if(rval == D3DERR_INVALIDCALL)
	{
		CLogWriter::Write("N3Texture: createtexture err D3DERR_INVALIDCALL(%s)", m_szFileName.c_str());
		return false;
	}
	if(rval == D3DERR_OUTOFVIDEOMEMORY)
	{
		CLogWriter::Write("N3Texture: createtexture err D3DERR_OUTOFVIDEOMEMORY(%s)", m_szFileName.c_str());
		return false;
	}
	if(rval == E_OUTOFMEMORY)
	{
		CLogWriter::Write("N3Texture: createtexture err E_OUTOFMEMORY(%s)", m_szFileName.c_str());
		return false;
	}
#endif
	if(nullptr == m_lpTexture)
	{
		__ASSERT(m_lpTexture, "Texture pointer is NULL!");
		return false;
	}


	m_Header.nWidth = nWidth;
	m_Header.nHeight = nHeight;
	m_Header.Format = Format;
	m_Header.bMipMap = bGenerateMipMap;

	if(32 == m_Header.nWidth && 32 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_32X32++;
	else if(64 == m_Header.nWidth && 64 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_64X64++;
	else if(128 == m_Header.nWidth && 128 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_128X128++;
	else if(256 == m_Header.nWidth && 256 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_256X256++;
	else if(512 == m_Header.nWidth && 512 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_512X512++;
	else if(512 < m_Header.nWidth && 512 < m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_Huge++;
	else s_ResrcInfo.nTexture_Loaded_OtherSize++;

	return true;
}

#ifdef _N3TOOL
bool CN3Texture::CreateFromSurface(LPDIRECT3DSURFACE9 lpSurf, D3DFORMAT Format, BOOL bGenerateMipMap)
{
	if(lpSurf == NULL) return false;

	D3DSURFACE_DESC sd;
	lpSurf->GetDesc(&sd);

	if(this->Create(sd.Width, sd.Height, Format, bGenerateMipMap) == false) return false;
	if(bGenerateMipMap)
	{
		this->GenerateMipMap(lpSurf);
	}
	
	return true;
}
#endif // end of _N3TOOL

bool CN3Texture::LoadFromFile(const std::string& szFileName)
{
	if(m_lpTexture != nullptr) this->Release();

	this->FileNameSet(szFileName);
	std::string szFullPath;
	if(-1 != m_szFileName.find(':') || -1 != m_szFileName.find("\\\\") || -1 != m_szFileName.find("//"))
	{
		szFullPath = m_szFileName;
	}
	else
	{
		if(NULL != s_szPath[0]) szFullPath = s_szPath;
		szFullPath += m_szFileName;
	}

	const int nFNL = szFullPath.size();
	if(lstrcmpi(&szFullPath[nFNL-3], "DXT") == 0)
	{
		HANDLE hFile = ::CreateFile(szFullPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(hFile == INVALID_HANDLE_VALUE)
		{
#ifdef _N3GAME
			CLogWriter::Write("invalid file handle(%d) - Can't open texture file(%s)", (int)hFile, szFullPath.c_str());
#endif
			return false;
		}
		this->Load(hFile);
		CloseHandle(hFile);
	}
	else
	{
		D3DXIMAGE_INFO ImgInfo;
		const HRESULT rval = D3DXCreateTextureFromFileEx(	s_lpD3DDev, 
			                                                 szFullPath.c_str(),
			                                                 D3DX_DEFAULT, 
			                                                 D3DX_DEFAULT, 
			                                                 D3DX_DEFAULT, 
			                                                 0,
			                                                 D3DFMT_UNKNOWN,
			                                                 D3DPOOL_MANAGED,
			                                                 D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
			                                                 D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
			                                                 0,
			                                                 &ImgInfo,
			                                                 nullptr,
			                                                 &m_lpTexture );
		if(rval == D3D_OK)
		{
			D3DSURFACE_DESC sd;
			m_lpTexture->GetLevelDesc(0, &sd);

			m_Header.nWidth = sd.Width;
			m_Header.nHeight = sd.Height;
			m_Header.Format = sd.Format;
		}
		else
		{
#ifdef _N3GAME
			CLogWriter::Write("N3Texture - Failed to load texture(%s)", szFullPath.c_str());
#endif
		}

		if(32 == m_Header.nWidth && 32 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_32X32++;
		else if(64 == m_Header.nWidth && 64 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_64X64++;
		else if(128 == m_Header.nWidth && 128 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_128X128++;
		else if(256 == m_Header.nWidth && 256 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_256X256++;
		else if(512 == m_Header.nWidth && 512 == m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_512X512++;
		else if(512 < m_Header.nWidth && 512 < m_Header.nHeight) s_ResrcInfo.nTexture_Loaded_Huge++;
		else s_ResrcInfo.nTexture_Loaded_OtherSize++;
	}

	if(nullptr == m_lpTexture)
	{
		this->Release();
		return false;
	}
	return true;
}

bool CN3Texture::Load(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);

	DWORD dwRWC = 0;

	__DXT_HEADER HeaderOrg;
	ReadFile(hFile, &HeaderOrg, sizeof(HeaderOrg), &dwRWC, nullptr);
	if(	'N' != HeaderOrg.szID[0] || 'T' != HeaderOrg.szID[1] || 'F' != HeaderOrg.szID[2] || 3 != HeaderOrg.szID[3] )
	{
#ifdef _N3GAME
		CLogWriter::Write("N3Texture Warning - Old format DXT file (%s)", m_szFileName.c_str());
#endif
	}

	bool bDXTSupport = FALSE;
	D3DFORMAT fmtNew = HeaderOrg.Format;
	if(D3DFMT_DXT1 == HeaderOrg.Format) 
	{
		if(s_dwTextureCaps & TEX_CAPS_DXT1) bDXTSupport = true;
		else fmtNew = D3DFMT_A1R5G5B5;
	}
	else if(D3DFMT_DXT2 == HeaderOrg.Format)
	{
		if(s_dwTextureCaps & TEX_CAPS_DXT2) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}
	else if(D3DFMT_DXT3 == HeaderOrg.Format)
	{
		if(s_dwTextureCaps & TEX_CAPS_DXT3) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}
	else if(D3DFMT_DXT4 == HeaderOrg.Format)
	{
		if(s_dwTextureCaps & TEX_CAPS_DXT4) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}
	else if(D3DFMT_DXT5 == HeaderOrg.Format)
	{
		if(s_dwTextureCaps & TEX_CAPS_DXT5) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}

	int iWCreate = HeaderOrg.nWidth, iHCreate = HeaderOrg.nHeight;
	if(fmtNew != HeaderOrg.Format) { iWCreate /= 2; iHCreate /= 2; }
	if(m_iLOD > 0 && m_iLOD <= 2 && HeaderOrg.nWidth >= 16 && HeaderOrg.nHeight >= 16)
	{
		for(int i = 0; i < m_iLOD; i++)
		{
			iWCreate /= 2;
			iHCreate /= 2;
		}
	}
	else m_iLOD = 0;

	int iLODPrev = m_iLOD;
	this->Create(iWCreate, iHCreate, fmtNew, HeaderOrg.bMipMap);
	m_iLOD = iLODPrev;

	if(m_lpTexture == nullptr)
	{
#ifdef _N3GAME
		CLogWriter::Write("N3Texture error - Can't create texture (%s)", m_szFileName.c_str());
#endif
		return false;
	}

	D3DSURFACE_DESC sd;
	D3DLOCKED_RECT LR;
	int iMMC = m_lpTexture->GetLevelCount();

	if(	D3DFMT_DXT1 == HeaderOrg.Format || 
		D3DFMT_DXT2 == HeaderOrg.Format || 
		D3DFMT_DXT3 == HeaderOrg.Format || 
		D3DFMT_DXT4 == HeaderOrg.Format || 
		D3DFMT_DXT5 == HeaderOrg.Format )
	{
		if(TRUE == bDXTSupport)
		{
			if(iMMC > 1)
			{
				if(m_iLOD > 0)
				{
					int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
					for(int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2)
					{
						if(D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += iWTmp * iHTmp / 2;
						else iSkipSize += iWTmp * iHTmp;
					}
					::SetFilePointer(hFile, iSkipSize, nullptr, FILE_CURRENT);
				}

				for(int i = 0; i < iMMC; i++)
				{
					m_lpTexture->GetLevelDesc(i, &sd);

					int iTexSize = 0;
					switch (HeaderOrg.Format) {
					case D3DFMT_DXT1: {
						iTexSize = (sd.Width * sd.Height / 2);
					} break;
					case D3DFMT_DXT2: {
						printf("ERROR: D3DFMT_DXT2\n");
						system("pause");
						exit(-1);
					} break;
					case D3DFMT_DXT3: {
						iTexSize = (sd.Width * sd.Height);
					} break;
					case D3DFMT_DXT4: {
						printf("ERROR: D3DFMT_DXT4\n");
						system("pause");
						exit(-1);
					} break;
					case D3DFMT_DXT5: {
						iTexSize = (sd.Width * sd.Height * 2);
						printf("ERROR: D3DFMT_DXT5\n");
						system("pause");
						exit(-1);
					} break;
					}

					m_lpTexture->LockRect(i, &LR, nullptr, NULL);
					ReadFile(hFile, (BYTE*)LR.pBits, iTexSize, &dwRWC, nullptr);
					m_lpTexture->UnlockRect(i);
				}

				int iWTmp = HeaderOrg.nWidth / 2, iHTmp = HeaderOrg.nHeight / 2;
				for(; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2)
					::SetFilePointer(hFile, iWTmp * iHTmp * 2, nullptr, FILE_CURRENT);
			}
			else
			{
				m_lpTexture->GetLevelDesc(0, &sd);

				int iTexSize = 0;
				switch (HeaderOrg.Format) {
				case D3DFMT_DXT1: {
					iTexSize = (sd.Width * sd.Height / 2);
				} break;
				case D3DFMT_DXT2: {
					printf("ERROR: D3DFMT_DXT2\n");
					system("pause");
					exit(-1);
				} break;
				case D3DFMT_DXT3: {
					iTexSize = (sd.Width * sd.Height);
				} break;
				case D3DFMT_DXT4: {
					printf("ERROR: D3DFMT_DXT4\n");
					system("pause");
					exit(-1);
				} break;
				case D3DFMT_DXT5: {
					iTexSize = (sd.Width * sd.Height * 2);
					printf("ERROR: D3DFMT_DXT5\n");
					system("pause");
					exit(-1);
				} break;
				}

				m_lpTexture->LockRect(0, &LR, nullptr, NULL);
				ReadFile(hFile, (BYTE*)LR.pBits, iTexSize, &dwRWC, nullptr);
				m_lpTexture->UnlockRect(0);

				::SetFilePointer(hFile, HeaderOrg.nWidth * HeaderOrg.nHeight / 4, nullptr, FILE_CURRENT);
				if(HeaderOrg.nWidth >= 1024) SetFilePointer(hFile, 256 * 256 * 2, nullptr, FILE_CURRENT);
			}
		}
		else
		{
			if(iMMC > 1)
			{
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				for(; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2)
				{
					if(D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += iWTmp * iHTmp / 2;
					else iSkipSize += iWTmp * iHTmp;
				}
				::SetFilePointer(hFile, iSkipSize, nullptr, FILE_CURRENT);

				iWTmp = HeaderOrg.nWidth / 2; iHTmp = HeaderOrg.nHeight / 2; iSkipSize = 0;
				if(m_iLOD > 0)
				{
					for(int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2)
						iSkipSize += iWTmp * iHTmp * 2;
				}

				for(; iWTmp > s_DevCaps.MaxTextureWidth || iHTmp > s_DevCaps.MaxTextureHeight; iWTmp /= 2, iHTmp /= 2)
					iSkipSize += iWTmp * iHTmp * 2;
				if(iSkipSize) ::SetFilePointer(hFile, iSkipSize, nullptr, FILE_CURRENT);

				for(int i = 0; i < iMMC; i++)
				{
					m_lpTexture->GetLevelDesc(i, &sd);
					m_lpTexture->LockRect(i, &LR, nullptr, NULL);
					int nH = sd.Height;
					for(int y = 0; y < nH; y++)
						ReadFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, 2 * sd.Width, &dwRWC, nullptr);
					m_lpTexture->UnlockRect(i);
				}
			}
			else
			{
				// Skip compressed data..
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				if(D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize = iWTmp * iHTmp / 2; // The DXT1 format compresses 1/4 of the 16-bit format.
				else iSkipSize = iWTmp * iHTmp; // DXT2 to DXT5 formats are compressed by 1/2 compared to 16-bit formats.

			}
		}
	}
	else
	{
		int iPixelSize = 0;
		if(	fmtNew == D3DFMT_A1R5G5B5 || fmtNew == D3DFMT_A4R4G4B4) iPixelSize = 2;
		else if(fmtNew == D3DFMT_R8G8B8) iPixelSize = 3;
		else if(fmtNew == D3DFMT_A8R8G8B8 || fmtNew == D3DFMT_X8R8G8B8) iPixelSize = 4;
		else 
		{
			__ASSERT(0, "Not supported texture format");
		}

		if(iMMC > 1)
		{
			if(m_iLOD > 0)
			{
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				for(int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2) iSkipSize += iWTmp * iHTmp * iPixelSize;
				::SetFilePointer(hFile, iSkipSize, nullptr, FILE_CURRENT);
			}

			int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
			for(; iWTmp > s_DevCaps.MaxTextureWidth || iHTmp > s_DevCaps.MaxTextureHeight; iWTmp /= 2, iHTmp /= 2)
				iSkipSize += iWTmp * iHTmp * iPixelSize;
			if(iSkipSize) ::SetFilePointer(hFile, iSkipSize, nullptr, FILE_CURRENT);

			for(int i = 0; i < iMMC; i++)
			{
				m_lpTexture->GetLevelDesc(i, &sd);
				m_lpTexture->LockRect(i, &LR, nullptr, NULL);
				for(int y = 0; y < (int)sd.Height; y++)
					ReadFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, iPixelSize * sd.Width, &dwRWC, nullptr);
				m_lpTexture->UnlockRect(i);
			}
		}
		else
		{
			if(HeaderOrg.nWidth >= 512 && m_Header.nWidth <= 256)
				::SetFilePointer(hFile, HeaderOrg.nWidth * HeaderOrg.nHeight * iPixelSize, nullptr, FILE_CURRENT);

			m_lpTexture->GetLevelDesc(0, &sd);
			m_lpTexture->LockRect(0, &LR, nullptr, NULL);
			for(int y = 0; y < (int)sd.Height; y++)
				ReadFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, iPixelSize * sd.Width, &dwRWC, nullptr);
			m_lpTexture->UnlockRect(0);

			if(m_Header.nWidth >= 512 && m_Header.nHeight >= 512)
				SetFilePointer(hFile, 256 * 256 * 2, nullptr, FILE_CURRENT);
		}
	}
//	this->GenerateMipMap();
	return true;
}

bool CN3Texture::SkipFileHandle(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);

	__DXT_HEADER HeaderOrg;
	DWORD dwRWC = 0;
	ReadFile(hFile, &HeaderOrg, sizeof(HeaderOrg), &dwRWC, nullptr);
	if(	'N' != HeaderOrg.szID[0] || 'T' != HeaderOrg.szID[1] || 'F' != HeaderOrg.szID[2] || 3 != HeaderOrg.szID[3] )
	{
#ifdef _N3GAME
		CLogWriter::Write("N3Texture Warning - Old format DXT file (%s)", m_szFileName.c_str());
#endif
	}

	if(	D3DFMT_DXT1 == HeaderOrg.Format || 
		D3DFMT_DXT2 == HeaderOrg.Format || 
		D3DFMT_DXT3 == HeaderOrg.Format || 
		D3DFMT_DXT4 == HeaderOrg.Format || 
		D3DFMT_DXT5 == HeaderOrg.Format )
	{
		int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
		if(HeaderOrg.bMipMap)
		{
			for(; iWTmp >= 4 && iHTmp >= 4; iWTmp/=2, iHTmp/=2)
			{
				if(D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += iWTmp * iHTmp / 2;
				else iSkipSize += iWTmp * iHTmp;
			} 
			iWTmp = HeaderOrg.nWidth / 2; iHTmp = HeaderOrg.nHeight / 2;
			for(; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2)
				iSkipSize += iWTmp * iHTmp * 2;
		}
		else
		{
			if(D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += HeaderOrg.nWidth * HeaderOrg.nHeight / 2;
			else iSkipSize += iSkipSize += HeaderOrg.nWidth * HeaderOrg.nHeight;

			iSkipSize += HeaderOrg.nWidth * HeaderOrg.nHeight * 2;
			if(HeaderOrg.nWidth >= 1024) iSkipSize += 256 * 256 * 2;
		}

		::SetFilePointer(hFile, iSkipSize, nullptr, FILE_CURRENT);
	}
	else
	{
		int iPixelSize = 0;
		if(	HeaderOrg.Format == D3DFMT_A1R5G5B5 || HeaderOrg.Format == D3DFMT_A4R4G4B4) iPixelSize = 2;
		else if(HeaderOrg.Format == D3DFMT_R8G8B8) iPixelSize = 3;
		else if(HeaderOrg.Format == D3DFMT_A8R8G8B8 || HeaderOrg.Format == D3DFMT_X8R8G8B8) iPixelSize = 4;
		else 
		{
			__ASSERT(0, "Not supported texture format");
		}

		int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
		if(HeaderOrg.bMipMap)
		{
			for(; iWTmp >= 4 && iHTmp >= 4; iWTmp/=2, iHTmp/=2)
				iSkipSize += iWTmp * iHTmp * iPixelSize;
		}
		else
		{
			iSkipSize += iWTmp * iHTmp * iPixelSize;
			if(HeaderOrg.nWidth >= 512) iSkipSize += 256 * 256 * 2;
		}
		
		::SetFilePointer(hFile, iSkipSize, nullptr, FILE_CURRENT);
	}
	return true;
}


#ifdef _N3TOOL
bool CN3Texture::SaveToFile()
{
	char szExt[_MAX_EXT];
	_splitpath(m_szFileName.c_str(), NULL, NULL, NULL, szExt);
	if(lstrcmpi(szExt, ".dxt")!=0) return false;

	return CN3BaseFileAccess::SaveToFile();
}
#endif // end of _N3TOOL


#ifdef _N3TOOL
bool CN3Texture::SaveToFile(const std::string& szFileName)
{
	this->FileNameSet(szFileName);
	return this->SaveToFile();
}
#endif // end of _N3TOOL


#ifdef _N3TOOL
bool CN3Texture::Save(HANDLE hFile)
{
	if(NULL == m_lpTexture) return false;

	CN3BaseFileAccess::Save(hFile);

	DWORD dwRWC = 0;

	D3DSURFACE_DESC sd;
	m_lpTexture->GetLevelDesc(0, &sd);

	int nMMC = m_lpTexture->GetLevelCount();
	(nMMC > 1) ? m_Header.bMipMap = TRUE : m_Header.bMipMap = FALSE;
	if(TRUE == m_Header.bMipMap)
	{
		int nMMC2 = 0;
		for(int nW = sd.Width, nH = sd.Height; nW >=4 && nH >= 4; nW /=2, nH /= 2) nMMC2++;
		if(nMMC < nMMC2)
		{
#ifdef _N3GAME
			CLogWriter::Write("N3Texture save warning - Invalid MipMap Count (%s)", m_szFileName.c_str());
#endif
			m_Header.bMipMap = FALSE;
			nMMC = 1;
		}
		else
		{
			nMMC = nMMC2;
		}
	}

	m_Header.szID[0] = 'N';
	m_Header.szID[1] = 'T';
	m_Header.szID[2] = 'F';
	m_Header.szID[3] = 3;
	m_Header.nWidth = sd.Width;
	m_Header.nHeight = sd.Height;
	m_Header.bMipMap = (nMMC > 1) ? TRUE : FALSE;
	
	WriteFile(hFile, &m_Header, sizeof(m_Header), &dwRWC, NULL);

	if(m_lpTexture == NULL) return false;

	if(	D3DFMT_DXT1 == sd.Format || 
		D3DFMT_DXT2 == sd.Format || 
		D3DFMT_DXT3 == sd.Format || 
		D3DFMT_DXT4 == sd.Format || 
		D3DFMT_DXT5 == sd.Format )
	{
		D3DLOCKED_RECT LR;

		for(int i = 0; i < nMMC; i++)
		{
			m_lpTexture->GetLevelDesc(i, &sd);

			m_lpTexture->LockRect(i, &LR, NULL, NULL);
			WriteFile(hFile, (BYTE*)LR.pBits, sd.Size, &dwRWC, NULL);
			m_lpTexture->UnlockRect(i);
		}

		// Extra uncompressed format. It's half the size.
		// Save the uncompressed format at a lower resolution.
		LPDIRECT3DSURFACE9 lpSurfSrc = NULL, lpSurfDest = NULL;
		D3DFORMAT fmtExtra = D3DFMT_UNKNOWN;
		if(D3DFMT_DXT1 == sd.Format) fmtExtra = D3DFMT_A1R5G5B5;
		else fmtExtra = D3DFMT_A4R4G4B4;
		
		int nMMC2 = nMMC - 1;
		if(nMMC == 1) nMMC2 = nMMC;
		for(auto i = 0; i < nMMC2; i++)
		{
			m_lpTexture->GetLevelDesc(i, &sd);
			m_lpTexture->GetSurfaceLevel(i, &lpSurfSrc);
			int nW = sd.Width / 2, nH = sd.Height / 2;
			s_lpD3DDev->CreateImageSurface(nW, nH, fmtExtra, &lpSurfDest);
			D3DXLoadSurfaceFromSurface(lpSurfDest, NULL, NULL, lpSurfSrc, NULL, NULL, D3DX_FILTER_TRIANGLE, 0); // 서피스 복사.
			int nPixelSize = 2;
			lpSurfDest->LockRect(&LR, NULL, NULL);
			for(int y = 0; y < nH; y++)
			{
				WriteFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, nW * 2, &dwRWC, NULL);
			}
			lpSurfDest->UnlockRect();
			lpSurfDest->Release(); lpSurfDest = NULL; 
			lpSurfSrc->Release(); lpSurfSrc = NULL;
		}

		if(nMMC == 1 && m_Header.nWidth >= 1024)
		{
			m_lpTexture->GetLevelDesc(0, &sd);
			m_lpTexture->GetSurfaceLevel(0, &lpSurfSrc);
			int nW = 256, nH = 256;
			s_lpD3DDev->CreateImageSurface(nW, nH, fmtExtra, &lpSurfDest);
			D3DXLoadSurfaceFromSurface(lpSurfDest, NULL, NULL, lpSurfSrc, NULL, NULL, D3DX_FILTER_TRIANGLE, 0);
			int nPixelSize = 2;
			lpSurfDest->LockRect(&LR, NULL, NULL);
			for(int y = 0; y < nH; y++)
			{
				WriteFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, nW * 2, &dwRWC, NULL);
			}
			lpSurfDest->UnlockRect();
			lpSurfDest->Release(); lpSurfDest = NULL; 
			lpSurfSrc->Release(); lpSurfSrc = NULL;
		}
	}
	else
	{
		int nPixelSize = 0;
		if(	D3DFMT_A1R5G5B5 == sd.Format ||
			D3DFMT_A4R4G4B4 == sd.Format) nPixelSize = 2;
		else if(D3DFMT_R8G8B8 == sd.Format) nPixelSize = 3;
		else if(D3DFMT_A8R8G8B8 == sd.Format || D3DFMT_X8R8G8B8 == sd.Format) nPixelSize = 4;
		else 
		{
			__ASSERT(0, "this Texture Format Not Supported");
		}

		D3DLOCKED_RECT LR;
		for(int i = 0; i < nMMC; i++)
		{
			m_lpTexture->GetLevelDesc(i, &sd);
			m_lpTexture->LockRect(i, &LR, NULL, 0);
			int nH = sd.Height;
			for(int y = 0; y < nH; y++)
				WriteFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, sd.Width * nPixelSize, &dwRWC, NULL);
			m_lpTexture->UnlockRect(i);
		}

		if(nMMC == 1 && m_Header.nWidth >= 512)
		{
			LPDIRECT3DSURFACE9 lpSurfSrc = NULL, lpSurfDest = NULL;

			m_lpTexture->GetLevelDesc(0, &sd);
			m_lpTexture->GetSurfaceLevel(0, &lpSurfSrc);
			int nW = 256, nH = 256;
			s_lpD3DDev->CreateImageSurface(nW, nH, sd.Format, &lpSurfDest);
			HRESULT rval = D3DXLoadSurfaceFromSurface(lpSurfDest, NULL, NULL, lpSurfSrc, NULL, NULL, D3DX_FILTER_TRIANGLE, 0);
			lpSurfDest->LockRect(&LR, NULL, NULL);
			for(int y = 0; y < nH; y++)
			{
				WriteFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, nW * 2, &dwRWC, NULL);
			}
			lpSurfDest->UnlockRect();
			lpSurfDest->Release(); lpSurfDest = NULL; 
			lpSurfSrc->Release(); lpSurfSrc = NULL;
		}
	}

	return true;
}
#endif // end of _N3TOOL

#ifdef _N3TOOL
bool CN3Texture::Convert(D3DFORMAT Format, int nWidth, int nHeight, BOOL bGenerateMipMap)
{
	if(m_lpTexture == NULL) return false;

	D3DSURFACE_DESC dsd;
	m_lpTexture->GetLevelDesc(0, &dsd);
	if(0 >= nWidth || 0 >= nHeight)
	{
		nWidth = dsd.Width;
		nHeight = dsd.Height;
	}

	LPDIRECT3DTEXTURE9 lpTexOld = m_lpTexture;

	m_lpTexture = NULL;
	if(this->Create(nWidth, nHeight, Format, bGenerateMipMap) == false) return false;
	if(bGenerateMipMap)
	{
		LPDIRECT3DSURFACE9 lpTSOld;
		lpTexOld->GetSurfaceLevel(0, &lpTSOld);
		this->GenerateMipMap(lpTSOld);
		lpTSOld->Release();
	}
	else
	{
		LPDIRECT3DSURFACE9 lpTSNew;
		LPDIRECT3DSURFACE9 lpTSOld;
		m_lpTexture->GetSurfaceLevel(0, &lpTSNew);
		lpTexOld->GetSurfaceLevel(0, &lpTSOld);
		D3DXLoadSurfaceFromSurface(lpTSNew, NULL, NULL, lpTSOld, NULL, NULL, D3DX_FILTER_NONE, 0);
		lpTSOld->Release();
		lpTSNew->Release();
	}

	lpTexOld->Release(); lpTexOld = NULL;
	
	return true;
}
#endif // end of _N3TOOL

#ifdef _N3TOOL
bool CN3Texture::GenerateMipMap(LPDIRECT3DSURFACE9 lpSurfSrc)
{
	if(m_lpTexture == NULL) return false;

	int nMMC = m_lpTexture->GetLevelCount();
	int nMMC2 = 0;
	for(int nW = m_Header.nWidth, nH = m_Header.nHeight; nW >=4 && nH >= 4; nW /=2, nH /= 2) nMMC2++;

	bool bNeedReleaseSurf = false;
	if(NULL == lpSurfSrc) 
	{
		bNeedReleaseSurf = true;
		if(D3D_OK != m_lpTexture->GetSurfaceLevel(0, &lpSurfSrc)) return false;
	}

	HRESULT rval = D3D_OK;
	if(nMMC < nMMC2)
	{
		LPDIRECT3DTEXTURE9 lpTexOld = m_lpTexture;
		m_lpTexture = NULL;
		rval = this->CreateFromSurface(lpSurfSrc, m_Header.Format, TRUE);
		if(bNeedReleaseSurf) { lpSurfSrc->Release(); lpSurfSrc = NULL; }
		lpTexOld->Release(); lpTexOld = NULL;

		if(D3D_OK == rval)
		{
			m_Header.bMipMap = TRUE;
			return true;
		}
		else
		{
			m_Header.bMipMap = FALSE;
			return FALSE;
		}
	}
	else
	{
		if(false == bNeedReleaseSurf)
		{
			LPDIRECT3DSURFACE9 lpSurfDest;
			m_lpTexture->GetSurfaceLevel(0, &lpSurfDest);
			DWORD dwFilter = D3DX_FILTER_TRIANGLE;
			HRESULT rval = D3DXLoadSurfaceFromSurface(lpSurfDest, NULL, NULL, lpSurfSrc, NULL, NULL, dwFilter, 0);
			lpSurfDest->Release(); lpSurfDest = NULL;
		}

		for(int i = 1; i < nMMC2; i++)
		{
			LPDIRECT3DSURFACE9 lpSurfDest, lpSurfUp;
			m_lpTexture->GetSurfaceLevel(i-1, &lpSurfUp);
			m_lpTexture->GetSurfaceLevel(i, &lpSurfDest);
			DWORD dwFilter = D3DX_FILTER_TRIANGLE;
			HRESULT rval = D3DXLoadSurfaceFromSurface(lpSurfDest, NULL, NULL, lpSurfUp, NULL, NULL, dwFilter, 0);
			lpSurfDest->Release();
			lpSurfUp->Release();
		}

		if(bNeedReleaseSurf) { lpSurfSrc->Release(); lpSurfSrc = NULL; }
		if(D3D_OK == rval)
		{
			m_Header.bMipMap = TRUE;
			return true;
		}
		else
		{
			m_Header.bMipMap = FALSE;
			return FALSE;
		}
	}
}
#endif // end of _N3TOOL

void CN3Texture::UpdateRenderInfo()
{
}

#ifdef _N3TOOL
bool CN3Texture::SaveToBitmapFile(const std::string& szFN)
{
	if(szFN.empty()) return false;
	if(NULL == m_lpTexture) return false;

	LPDIRECT3DSURFACE9 lpSurfSrc = NULL;
	m_lpTexture->GetSurfaceLevel(0, &lpSurfSrc);

	if(NULL == lpSurfSrc) return false;

	LPDIRECT3DSURFACE9 lpSurfDest = NULL;
	s_lpD3DDev->CreateImageSurface(m_Header.nWidth, m_Header.nHeight, D3DFMT_A8R8G8B8, &lpSurfDest);

	if(NULL == lpSurfDest) return false;
	if(D3D_OK != D3DXLoadSurfaceFromSurface(lpSurfDest, NULL, NULL, lpSurfSrc, NULL, NULL, D3DX_FILTER_TRIANGLE, 0))
	{
		lpSurfDest->Release(); lpSurfDest = NULL;
		lpSurfSrc->Release(); lpSurfSrc = NULL;
	}

	CBitMapFile bmpf;
	bmpf.Create(m_Header.nWidth, m_Header.nHeight);

	D3DLOCKED_RECT LR;
	lpSurfDest->LockRect(&LR, NULL, 0);
	for(int y = 0; y < m_Header.nHeight; y++)
	{
		BYTE* pPixelsSrc = ((BYTE*)LR.pBits) + y * LR.Pitch;
		BYTE* pPixelsDest = (BYTE*)(bmpf.Pixels(0, y));
		for(int x = 0; x < m_Header.nWidth; x++)
		{
			pPixelsDest[0] = pPixelsSrc[0];
			pPixelsDest[1] = pPixelsSrc[1];
			pPixelsDest[2] = pPixelsSrc[2];
			
			pPixelsSrc += 4;
			pPixelsDest += 3;
		}
	}
	lpSurfDest->UnlockRect();
	
	lpSurfDest->Release(); lpSurfDest = NULL;
	lpSurfSrc->Release(); lpSurfSrc = NULL;

	return bmpf.SaveToFile(szFN.c_str());
}
#endif // end of _N3TOOL
