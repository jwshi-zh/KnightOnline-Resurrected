#pragma once

class CBitMapFile
{
protected:
	BITMAPFILEHEADER m_bmfHeader;
	BITMAPINFOHEADER m_bmInfoHeader;

public:
	void* m_pPixels; // 실제 픽셀 데이터
	int Pitch() const { return ((int)((m_bmInfoHeader.biWidth*3 + 3)/4))*4; } // 비트맵의 실제 너비(byte 단위)..
	bool Create(int nWidth, int nHeight, int nBPP = 24);
	bool SaveRectToFile(const std::string& szFN, RECT rc);
	void* Pixels(int x = 0, int y = 0) const;
	BITMAPINFOHEADER * GetBitmapInfoHeader() { return &m_bmInfoHeader;}
	BITMAPFILEHEADER * GetBitmapFileHeader() { return &m_bmfHeader;}
	bool Load(HANDLE hFile);
	bool Save(HANDLE hFile);
	bool LoadFromFile(const char* pszFN);
	bool SaveToFile(const char* pszFN);

	int Width() { return m_bmInfoHeader.biWidth; }
	int Height() { return m_bmInfoHeader.biHeight; }
	
	void Release();
	CBitMapFile();
	virtual ~CBitMapFile();

};
