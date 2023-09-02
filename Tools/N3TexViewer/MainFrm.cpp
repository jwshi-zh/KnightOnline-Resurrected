// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "N3TexViewer.h"

#include "MainFrm.h"
#include "N3TexViewerDoc.h"
#include "DlgFormat.h"
#include "ProgressBar.h"
#include "../N3Base/BitMapFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_ADJUST_WINDOW_SIZE, OnAdjustWindowSize)
	ON_COMMAND(ID_FILE_CONVERT, OnFileConvert)
	ON_COMMAND(ID_TOOL_CONVERT_FILES_AUTOMATICALY, OnToolConvertFilesAutomaticaly)
	ON_COMMAND(ID_TOOL_CONVERT_FILES_MANUALLY, OnToolConvertFilesManually)
	ON_COMMAND(ID_TOOL_CUT_BMP, OnToolCutBmp)
	ON_COMMAND(ID_FILE_OPEN_NEXT, OnFileOpenNext)
	ON_COMMAND(ID_FILE_OPEN_PREV, OnFileOpenPrev)
	ON_COMMAND(ID_FILE_OPEN_FIRST, OnFileOpenFirst)
	ON_COMMAND(ID_FILE_OPEN_LAST, OnFileOpenLast)
	ON_COMMAND(ID_TOOL_SAVE_REPEAT, OnToolSaveRepeat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	if(m_Eng.Init(TRUE, m_hWnd, 64, 64, 0, TRUE) == false) return -1;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnAdjustWindowSize() 
{
	this->AdjustWindowSize();
}

void CMainFrame::AdjustWindowSize() 
{	
	// Adjust the window size to the picture size.
	DWORD dwStyle = this->GetStyle();
	CRect rcFrm, rcView;
	this->GetWindowRect(rcFrm);
	GetActiveView()->GetClientRect(rcView);
	CN3TexViewerDoc* pDoc = (CN3TexViewerDoc*)GetActiveDocument();

	int nW = rcFrm.Width() - rcView.Width() + pDoc->m_pTex->Width();
	int nH = rcFrm.Height() - rcView.Height() + pDoc->m_pTex->Height();

	if(pDoc->m_pTex->Width() <= 0) nW += 256;
	if(pDoc->m_pTex->Height() <= 0) nH += 256;

	if(nW < 256) nW = 256;

	SetWindowPos(NULL, 0, 0, nW, nH, SWP_NOMOVE | SWP_NOZORDER);
}

void CMainFrame::OnFileConvert() 
{
	CN3TexViewerDoc* pDoc = (CN3TexViewerDoc*)GetActiveDocument();
	if(NULL == pDoc->m_pTex || NULL == pDoc->m_pTex->Get()) return;

	CDlgFormat dlg;
	dlg.m_Fmt = pDoc->m_pTex->PixelFormat();
	dlg.m_nWidth = pDoc->m_pTex->Width();
	dlg.m_nHeight = pDoc->m_pTex->Height();
	dlg.m_bMipMap = pDoc->m_pTex->MipMapCount() > 1 ? TRUE : FALSE;
	int nOK = dlg.DoModal();

	if(IDOK != nOK) return;

	pDoc->m_pTex->Convert(dlg.m_Fmt, dlg.m_nWidth, dlg.m_nHeight, dlg.m_bMipMap);
	pDoc->SetTitle(""); // Change the title...
}

void CMainFrame::OnToolConvertFilesAutomaticaly() 
{
	char szBuff[102400] = "";
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT;
	CFileDialog dlg(TRUE, "dxt", NULL, dwFlags, "Generic Image Files(*.bmp, *.tga, *.jpg)|*.bmp;*.tga;*.jpg||", NULL);
	char szCurPath[256]; GetCurrentDirectory(256, szCurPath);
	dlg.m_ofn.lpstrInitialDir = szCurPath;
	dlg.m_ofn.nMaxFile = 102400;
	dlg.m_ofn.lpstrFile = szBuff;

	if(dlg.DoModal() == IDCANCEL) return;

	CN3Texture Tex;
	POSITION pos = dlg.GetStartPosition();
	CString FileName;
	while(pos != NULL)
	{
		FileName = dlg.GetNextPathName(pos);
		
		std::string szFN = FileName;
		Tex.LoadFromFile(szFN);

		if(Tex.Get()) 
		{
			char szFN2[_MAX_PATH], szDrv[_MAX_DRIVE], szDir[_MAX_DIR], szFN[_MAX_FNAME], szExt[_MAX_EXT];
			::_splitpath(FileName, szDrv, szDir, szFN, szExt);
			lstrcpy(szExt, ".DXT");
			::_makepath(szFN2, szDrv, szDir, szFN, szExt); // Rename filename extension to DXT...
	
			D3DFORMAT Fmt = Tex.PixelFormat();
			if(	Fmt == D3DFMT_R8G8B8 ||
				Fmt == D3DFMT_X8R8G8B8 ||
				Fmt == D3DFMT_R5G6B5 || 
				Fmt == D3DFMT_X1R5G5B5 )
			{
				Tex.Convert(D3DFMT_DXT1);
			}
			else if(	Fmt == D3DFMT_A8R8G8B8 ||
						Fmt == D3DFMT_A1R5G5B5 )
			{
				Tex.Convert(D3DFMT_DXT3);
			}

			Tex.m_szName = szFN;
			Tex.SaveToFile(szFN2);
			Tex.Release();
		}
	}
}

void CMainFrame::OnToolConvertFilesManually() 
{
	char szBuff[102400] = "";
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT;
	CFileDialog dlg(TRUE, "dxt", NULL, dwFlags, "Generic Image Files(*.dxt, *.bmp, *.tga, *.jpg)|*.dxt;*.bmp;*.tga;*.jpg|BitmapFile File(*.bmp)|*.bmp|Targa File(*.tga)|*.bmp|DXT File(*.dxt)|*.dxt||", NULL);
	char szCurPath[256]; GetCurrentDirectory(256, szCurPath);
	dlg.m_ofn.lpstrInitialDir = szCurPath;
	dlg.m_ofn.nMaxFile = 102400;
	dlg.m_ofn.lpstrFile = szBuff;

	if(dlg.DoModal() == IDCANCEL) return;

	CN3Texture Tex;
	std::string szFN;
	szFN = dlg.GetPathName();
	Tex.LoadFromFile(szFN); // I just read one...

	CDlgFormat dlgFormat; // Format Suspension...
	dlgFormat.m_nWidth = Tex.Width();
	dlgFormat.m_nHeight = Tex.Height();
	dlgFormat.m_bMipMap = Tex.MipMapCount() > 1 ? TRUE : FALSE;
	if(dlgFormat.DoModal() == IDCANCEL) return;

	POSITION pos = dlg.GetStartPosition();
	CString FileName;
	while(pos != NULL)
	{
		FileName = dlg.GetNextPathName(pos);
		
		szFN = FileName;
		Tex.LoadFromFile(szFN);

		if(Tex.Get()) 
		{
			char szFN2[_MAX_PATH] = "", szDrv[_MAX_DRIVE], szDir[_MAX_DIR], szFN[_MAX_FNAME], szExt[_MAX_EXT];
			::_splitpath(FileName, szDrv, szDir, szFN, szExt);
			lstrcpy(szExt, ".DXT");
			::_makepath(szFN2, szDrv, szDir, szFN, szExt); // Rename filename extension to DXT...
			
			Tex.Convert(dlgFormat.m_Fmt, dlgFormat.m_nWidth, dlgFormat.m_nHeight, dlgFormat.m_bMipMap);
			Tex.m_szName = szFN2;
			Tex.SaveToFile(szFN2);
		}
	}
}

void CMainFrame::OnToolCutBmp() 
{
	DWORD dwFlags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST;
	CFileDialog dlg(TRUE, "bmp", NULL, dwFlags, "Bitmap file(*.bmp)|*.bmp||", NULL);
	if(dlg.DoModal() == IDCANCEL) return;

	CDlgFormat dlgFmt; // Specify size and storage format.
	if(IDCANCEL == dlgFmt.DoModal()) return;

	int nW = dlgFmt.m_nWidth; // width
	int nH = dlgFmt.m_nHeight; // height
	D3DFORMAT fmtSave = dlgFmt.m_Fmt; // Format - Ignored for BitMap.

	int nYesNo = MessageBox("Do you want to save as DXT File??", "storage format", MB_YESNO);
	bool bSaveToDXT = false;
	if(IDYES == nYesNo)
	{
		bSaveToDXT = true;
	}

	BMPCutter(dlg.GetPathName(), nW, nH, bSaveToDXT, fmtSave);
}

BOOL CMainFrame::BMPCutter(LPCTSTR lpszFileName, int iWidth, int iHeight, bool bSaveToDXT, D3DFORMAT fmtDXT)
{
	CBitMapFile BMF;
	if(false == BMF.LoadFromFile(lpszFileName)) return FALSE;

	// file name to save
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFName[_MAX_FNAME];
	_splitpath(lpszFileName, szDrive, szDir, szFName, NULL);
	CreateDirectory(szFName, NULL);	// Create subfolders

	int xx = BMF.Width() / iWidth;
	int yy = BMF.Height() / iHeight;

	// progress bar
	CProgressBar ProgressBar;
	ProgressBar.Create("cutting bitmap..", 50, yy * xx);
	ProgressBar.SetStep(1);

	for(int y = 0; y < yy; y++)
	{
		for(int x = 0; x < xx; x++)
		{
			RECT rcDest = {x*iWidth, y*iHeight, (x+1)*iWidth, (y+1)*iHeight };
			char szDestFN[_MAX_PATH];
			
			if(bSaveToDXT)
			{
				if(false == BMF.SaveRectToFile("c:\\TempConvert.BMP", rcDest)) continue;
				
				CN3Texture Tex;
				if(false == Tex.LoadFromFile("c:\\TempConvert.BMP")) continue;

				Tex.Convert(fmtDXT);
				wsprintf(szDestFN, "%s%s%s\\%s_%02d%02d.DXT", szDrive, szDir, szFName, szFName, x, y);
				Tex.SaveToFile(szDestFN);
				DeleteFile("c:\\TempConvert.BMP");
			}
			else
			{
				wsprintf(szDestFN, "%s%s%s\\%s_%02d%02d.bmp", szDrive, szDir, szFName, szFName, x, y);
				BMF.SaveRectToFile(szDestFN, rcDest);
			}

			ProgressBar.StepIt();
		}
	}

	return TRUE;
/*
	if (lstrlen(lpszFileName) == 0 || iWidth<=0 || iHeight<=0)
	{
		MessageBox("Cannot be divided into bitmaps with an aspect ratio less than or equal to 0.", "error");
		return FALSE;
	}

	CFile file;
	CFileException fe;

	// Open file in read mode
	if (!file.Open(lpszFileName, CFile::modeRead|CFile::shareDenyWrite, &fe))
	{
		MessageBox("Could not open original bitmap.", "error");
		return FALSE;
	}

	// file length
	DWORD dwBitsSize;
	dwBitsSize = file.GetLength();

	// read file header
	BITMAPFILEHEADER bmfHeader;
	if (file.Read(&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
	{
		MessageBox("Original bitmap is strange.", "error");
		return FALSE;
	}

	// Check the "BM" marker indicating that it is a bmp file
	if (bmfHeader.bfType != 0x4D42)
	{
		MessageBox("Source file is not a bitmap file.", "error");
		return FALSE;
	}

	// GET BITMAPINFOHEADER
	BITMAPINFOHEADER bmInfoHeader;
	if (file.Read(&bmInfoHeader, sizeof(bmInfoHeader)) != sizeof(bmInfoHeader)) return FALSE;

	// Check bits per pixel
	WORD wBitCount = bmInfoHeader.biBitCount;
	if (24 != wBitCount)		// If it is not a 24-bit bmp, it is returned.
	{
		MessageBox("Source bitmap is not a 24bit file.", "error");
		return FALSE;
	}

	// Calculate the number of divisions horizontally and vertically
	int iCX, iCY;
	iCX = (bmInfoHeader.biWidth+iWidth-1) / iWidth;
	iCY = (bmInfoHeader.biHeight+iHeight-1) / iHeight;
	if (iCX <= 0 || iCY <= 0)
	{
		MessageBox("can't be divided.", "error");
		return FALSE;
	}

	// real image bit address
//	LPVOID pSrcImageBit;
//	pSrcImageBit = (LPVOID)((BYTE*)pSrcDIB + (bmfHeader.bfOffBits - sizeof(bmfHeader)));

	// Horizontal length captured in memory of the actual image (24bit)
	int iRealWidthSrc = ((int)((bmInfoHeader.biWidth*3 + 3)/4))*4;	

	// Allocate memory for the new image
	int iRealWidthDest = ((int)((iWidth*3 + 3)/4))*4;	
	int iDestDIBSize = sizeof(BITMAPINFOHEADER) + iRealWidthDest * iHeight;
	LPVOID pDestDIB;
	if ((pDestDIB = ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, iDestDIBSize )) == NULL )
	{
		MessageBox("Failed to allocate memory.", "error");
		return FALSE;
	}

	// Fill in the file header information for the image to be created
	BITMAPFILEHEADER bmfHeaderDest;
	memset(&bmfHeaderDest, 0, sizeof(bmfHeaderDest));
	bmfHeaderDest.bfType = 0x4D42; // "BM"
	bmfHeaderDest.bfSize = sizeof(bmfHeaderDest) + iDestDIBSize;
	bmfHeaderDest.bfOffBits = sizeof(bmfHeaderDest) + sizeof(BITMAPINFOHEADER);

	// Fill in the image bitmap info header information to create a new one
	BITMAPINFOHEADER bmInfoHeaderDest;
	memset(&bmInfoHeaderDest, 0, sizeof(bmInfoHeaderDest));
	bmInfoHeaderDest.biSize = sizeof(bmInfoHeaderDest);
	bmInfoHeaderDest.biWidth = iWidth;
	bmInfoHeaderDest.biHeight = iHeight;
	bmInfoHeaderDest.biPlanes = 1;
	bmInfoHeaderDest.biBitCount = 24;
	bmInfoHeaderDest.biSizeImage = iRealWidthDest * iHeight;
	memcpy(pDestDIB, &bmInfoHeaderDest, sizeof(bmInfoHeaderDest));

	// file name to save
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFName[_MAX_FNAME];
	char szFNameDest[_MAX_FNAME];
	_splitpath(lpszFileName, szDrive, szDir, szFName, NULL);
	CreateDirectory(szFName, NULL);	// Create subfolders

	// Save the split information in a tcd file
	DWORD dwNum;
	wsprintf(szFNameDest, "%s\\%s.tcd", szFName, szFName);
	HANDLE hFile = CreateFile(szFNameDest, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE != hFile)
	{
		WriteFile(hFile, &iCX, sizeof(iCX), &dwNum, NULL);
		WriteFile(hFile, &iCY, sizeof(iCX), &dwNum, NULL);
		CloseHandle(hFile);
	}

	// progress bar
	CProgressBar ProgressBar;
	ProgressBar.Create("cutting bitmap..", 50, iCY*iCX);
	ProgressBar.SetStep(1);

	// split and save
	BYTE *pTmpBitDest;
	pTmpBitDest = ((BYTE*)pDestDIB) + sizeof(BITMAPINFOHEADER);
	int i, j, y;
	for (j=0; j<iCY; ++j)
	{
		for (i=0; i<iCX; ++i)
		{
			memset(pTmpBitDest, 0, iDestDIBSize - sizeof(BITMAPINFOHEADER));
			for(y=0; y<iHeight; ++y)
			{
				if ( (iHeight*j + y) >= bmInfoHeader.biHeight) break;	// The bottom may be cut off

				// Set the file position of the part to be read in the original file accordingly.
				file.Seek(bmfHeader.bfOffBits + 
					iRealWidthSrc*(bmInfoHeader.biHeight - 1 - (iHeight*j + y)) + 
					3*(iWidth*i),
					CFile::begin);

				if (i == (iCX-1))
				{	// The far right end is likely cut off.
					file.Read(pTmpBitDest + iRealWidthDest*(iHeight-1-y), bmInfoHeader.biWidth*3 - iRealWidthDest*(iCX-1));
				}
				else
				{
					file.Read(pTmpBitDest + iRealWidthDest*(iHeight-1-y), iRealWidthDest);
				}
			}

			// save
			if(bSaveToDXT)
			{
				wsprintf(szFNameDest, "%s%s%s\\ConversionTmp.bmp", szDrive, szDir, szFName, szFName, i, iCY-1-j);
			}
			else
			{
				wsprintf(szFNameDest, "%s%s%s\\%s_%02d%02d.bmp", szDrive, szDir, szFName, szFName, i, iCY-1-j);
			}

			hFile = CreateFile(szFNameDest, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(INVALID_HANDLE_VALUE != hFile)
			{
				WriteFile(hFile, &bmfHeaderDest, sizeof(bmfHeaderDest), &dwNum, NULL);
				WriteFile(hFile, pDestDIB, iDestDIBSize, &dwNum, NULL);
				CloseHandle(hFile);
			}
			ProgressBar.StepIt();
			this->UpdateWindow();

			if(bSaveToDXT) // To save DXT...
			{
				char szDXT_FName[_MAX_PATH];
				wsprintf(szDXT_FName, "%s%s%s\\%s_%02d%02d.DXT", szDrive, szDir, szFName, szFName, i, iCY-1-j);

				CN3Texture TexTmp;
				TexTmp.LoadFromFile(szFNameDest); // loading
				if(true == TexTmp.Convert(fmtDXT)) // conversion
				{
					TexTmp.SaveToFile(szDXT_FName);
				}

				DeleteFile(szFNameDest); // Clear Temporary Bitmap Files...
			}
		}
	}

	// freeing up memory
	::GlobalFree(pDestDIB);
	file.Close();
	
	 return TRUE;
	 */
}

void CMainFrame::OnFileOpenNext() 
{
	CN3TexViewerDoc* pDoc = (CN3TexViewerDoc*)(this->GetActiveDocument());
	if(pDoc) pDoc->OpenNextFile();
	AdjustWindowSize();
}

void CMainFrame::OnFileOpenPrev() 
{
	CN3TexViewerDoc* pDoc = (CN3TexViewerDoc*)(this->GetActiveDocument());
	if(pDoc) pDoc->OpenPrevFile();
	AdjustWindowSize();
}

void CMainFrame::OnFileOpenFirst() 
{
	CN3TexViewerDoc* pDoc = (CN3TexViewerDoc*)(this->GetActiveDocument());
	if(pDoc) pDoc->OpenFirstFile();
	AdjustWindowSize();
}

void CMainFrame::OnFileOpenLast() 
{
	CN3TexViewerDoc* pDoc = (CN3TexViewerDoc*)(this->GetActiveDocument());
	if(pDoc) pDoc->OpenLastFile();
	AdjustWindowSize();
}

void CMainFrame::OnToolSaveRepeat() 
{
	char szBuff[102400] = "";
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT;
	CFileDialog dlg(TRUE, "dxt", NULL, dwFlags, "Noah Texture Files(*.dxt)|*.dxt||", NULL);
	char szCurPath[256]; GetCurrentDirectory(256, szCurPath);
	dlg.m_ofn.lpstrInitialDir = szCurPath;
	dlg.m_ofn.nMaxFile = 102400;
	dlg.m_ofn.lpstrFile = szBuff;

	if(dlg.DoModal() == IDCANCEL) return;

	CN3Texture Tex;
	POSITION pos = dlg.GetStartPosition();
	CStringArray FileNames;
	while(pos != NULL)
	{
		FileNames.Add(dlg.GetNextPathName(pos));
	}

	int nFNC = FileNames.GetSize();

	// progress bar
	CProgressBar ProgressBar;
	ProgressBar.Create("ReSave DXT Files..", 50, nFNC);
	ProgressBar.SetStep(1);

	for(int i = 0; i < nFNC; i++)
	{
		std::string szFN = FileNames[i];
		Tex.LoadFromFile(szFN);
		Tex.SaveToFile(szFN);

		ProgressBar.StepIt();
	}
}
