#pragma once

class CBitMapFile
{
protected:
	BITMAPFILEHEADER m_bmfHeader;
	BITMAPINFOHEADER m_bmInfoHeader;

public:
	void* m_pPixels; // actual pixel data
	int Pitch() const { return ((int)((m_bmInfoHeader.biWidth*3 + 3)/4))*4; } // The actual width of the bitmap in bytes.
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
