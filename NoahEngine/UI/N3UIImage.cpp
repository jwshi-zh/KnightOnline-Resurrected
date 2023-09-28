#include "pch.h"
#include "N3UIImage.h"
#include "N3Texture.h"

CN3UIImage::CN3UIImage()
{
	m_eType = UI_TYPE_IMAGE;

	m_pVB = nullptr;
	m_pTexRef = nullptr;
	m_szTexFN = "";
	m_pAnimImagesRef = nullptr;

	ZeroMemory(&m_frcUVRect, sizeof(m_frcUVRect));
	m_Color = D3DCOLOR_ARGB(0xff,0xff,0xff,0xff);
	m_fAnimFrame = 30.0f;
	m_iAnimCount = 0;
	m_fCurAnimFrame = 0.0f;
}

CN3UIImage::~CN3UIImage()
{
	if(m_pVB) {	m_pVB->Release();	m_pVB = nullptr;}
	s_MngTex.Delete(&m_pTexRef);
	if (m_pAnimImagesRef) {delete [] m_pAnimImagesRef; m_pAnimImagesRef = nullptr;}
}

void CN3UIImage::Release()
{
	CN3UIBase::Release();
	if(m_pVB) {	m_pVB->Release();	m_pVB = nullptr;}
	s_MngTex.Delete(&m_pTexRef);
	m_szTexFN = "";

	ZeroMemory(&m_frcUVRect, sizeof(m_frcUVRect));
	m_Color = D3DCOLOR_ARGB(0xff,0xff,0xff,0xff);
	m_fAnimFrame = 30.0f;
	m_iAnimCount = 0;
	m_fCurAnimFrame = 0.0f;
	if (m_pAnimImagesRef) {delete [] m_pAnimImagesRef; m_pAnimImagesRef = nullptr;}
}

void CN3UIImage::Init(CN3UIBase* pParent)
{
	CN3UIBase::Init(pParent);
	CreateVB();
}

bool CN3UIImage::CreateVB()
{
	HRESULT hr;
	if (m_pVB) {m_pVB->Release(); m_pVB = nullptr;}
	hr = s_lpD3DDev->CreateVertexBuffer( 4*sizeof(__VertexTransformed), 0, FVF_TRANSFORMED, D3DPOOL_MANAGED, &m_pVB, nullptr );
	return SUCCEEDED(hr);
}

void CN3UIImage::SetVB()
{
	if (UISTYLE_IMAGE_ANIMATE & m_dwStyle)	// Release vertex buffer if animate image
	{
		if (m_pVB) {m_pVB->Release(); m_pVB = nullptr;}
	}
	else
	{
		if(m_pVB)
		{
			__VertexTransformed* pVertices;
			m_pVB->Lock( 0, 0, (void**)&pVertices, 0 );

			DWORD dwColor = 0xffffffff;
			const float fRHW = 1.0f;
			// If -0.5f is not given, sometimes the image is pushed by one dot (not sure why).
			pVertices[0].Set((float)m_rcRegion.left-0.5f,	(float)m_rcRegion.top-0.5f,		UI_DEFAULT_Z, fRHW, m_Color, m_frcUVRect.left,		m_frcUVRect.top);
			pVertices[1].Set((float)m_rcRegion.right-0.5f,	(float)m_rcRegion.top-0.5f,		UI_DEFAULT_Z, fRHW, m_Color, m_frcUVRect.right,	m_frcUVRect.top);
			pVertices[2].Set((float)m_rcRegion.right-0.5f,	(float)m_rcRegion.bottom-0.5f,	UI_DEFAULT_Z, fRHW, m_Color, m_frcUVRect.right,	m_frcUVRect.bottom);
			pVertices[3].Set((float)m_rcRegion.left-0.5f,	(float)m_rcRegion.bottom-0.5f,	UI_DEFAULT_Z, fRHW, m_Color, m_frcUVRect.left,		m_frcUVRect.bottom);
			
			m_pVB->Unlock();
		}
	}
}


void CN3UIImage::SetTex(const std::string& szFN)
{
	m_szTexFN = szFN;
	s_MngTex.Delete(&m_pTexRef);
	// Specify texture only for animated image
	if (!(UISTYLE_IMAGE_ANIMATE & m_dwStyle)) m_pTexRef = s_MngTex.Get(szFN);
}

void CN3UIImage::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		(*itor)->SetRegion(Rect);
	}
	SetVB();
}

void CN3UIImage::SetUVRect(float left, float top, float right, float bottom)
{
	m_frcUVRect.left = left;	m_frcUVRect.top = top;
	m_frcUVRect.right = right;	m_frcUVRect.bottom = bottom;
	SetVB();
}

void CN3UIImage::Tick()
{
	CN3UIBase::Tick();
	if (m_iAnimCount>0)		// Calculate the current frame when it is an animated image
	{
		m_fCurAnimFrame += (s_fSecPerFrm * m_fAnimFrame);
		while (m_fCurAnimFrame >= (float)m_iAnimCount)
		{
			m_fCurAnimFrame -= ((float)m_iAnimCount);
		}
	}
}

void CN3UIImage::Render()
{
	if(!m_bVisible) return;

	if (UISTYLE_IMAGE_ANIMATE & m_dwStyle) // If the image is animated
	{
		__ASSERT(m_fCurAnimFrame>=0.0f && m_fCurAnimFrame < (float)m_iAnimCount, "Animate image works abnormally");
		__ASSERT(m_pAnimImagesRef, "Initialization anomaly");
		m_pAnimImagesRef[(int)m_fCurAnimFrame]->Render();
	}
	else
	{
		if (m_pVB && m_pTexRef)
		{
			s_lpD3DDev->SetStreamSource( 0, m_pVB, 0, sizeof(__VertexTransformed) );
			s_lpD3DDev->SetFVF( FVF_TRANSFORMED );

			s_lpD3DDev->SetTexture( 0, m_pTexRef->Get());
			s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,    D3DTOP_MODULATE );
			s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
			s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2,  D3DTA_DIFFUSE );

			s_lpD3DDev->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
		} 

		CN3UIBase::Render();
	}
}

void CN3UIImage::RenderIconWrapper()
{
	if(!m_bVisible) return;

	if (m_pVB)
	{
		s_lpD3DDev->SetStreamSource( 0, m_pVB, 0, sizeof(__VertexTransformed) );
		s_lpD3DDev->SetFVF( FVF_TRANSFORMED );
		s_lpD3DDev->SetTexture( 0, nullptr);

		s_lpD3DDev->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
	}

	CN3UIBase::Render();
}

BOOL CN3UIImage::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (FALSE == CN3UIBase::MoveOffset(iOffsetX, iOffsetY)) return FALSE;
	SetVB();
	return TRUE;
}

void CN3UIImage::SetColor(D3DCOLOR color)
{
	if (m_Color == color) return;
	m_Color = color;
	if ((UISTYLE_IMAGE_ANIMATE & m_dwStyle) && m_pAnimImagesRef)
	{
		int i;
		for(auto i =0; i<m_iAnimCount; ++i) m_pAnimImagesRef[i]->SetColor(color);
	}
	SetVB();
}

bool CN3UIImage::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;
	DWORD dwNum;
	// texture information
	__ASSERT(NULL == m_pTexRef, "not initialized before load.");
	int	iStrLen = 0;
	ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, nullptr);			// file name length
	char szFName[MAX_PATH] = "";
	if (iStrLen>0)
	{
		ReadFile(hFile, szFName, iStrLen, &dwNum, nullptr);		// file name
		szFName[iStrLen]='\0';
		this->SetTex(szFName);
	} 

	ReadFile(hFile, &m_frcUVRect, sizeof(m_frcUVRect), &dwNum, nullptr);	// uv coordinates
	ReadFile(hFile, &m_fAnimFrame, sizeof(m_fAnimFrame), &dwNum, nullptr);

	// If it is an animated image, set related variables
	m_iAnimCount = 0; // Set the number of animated images
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		if(UI_TYPE_IMAGE == (*itor)->UIType()) m_iAnimCount++;
	}

	if ((UISTYLE_IMAGE_ANIMATE & m_dwStyle) && m_iAnimCount > 0)
	{
		m_pAnimImagesRef = new CN3UIImage*[m_iAnimCount];
		ZeroMemory(m_pAnimImagesRef, sizeof(CN3UIImage*)*m_iAnimCount);
		int i=0;
		for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
		{
			if(UI_TYPE_IMAGE == (*itor)->UIType()) m_pAnimImagesRef[i] = (CN3UIImage*)(*itor);
			__ASSERT(m_pAnimImagesRef[i]->GetReserved() == (DWORD)i, "animate Image load fail");	// Fails if not properly aligned.
			++i;
		}
	}

	SetVB();	// vertex settings
	return true;
}

void CN3UIImage::operator = (const CN3UIImage& other)
{
	CN3UIBase::operator = (other);

	m_Color = other.m_Color;
	m_fAnimFrame = other.m_fAnimFrame;
	m_fCurAnimFrame = other.m_fCurAnimFrame;
	m_frcUVRect = other.m_frcUVRect;
	m_iAnimCount = other.m_iAnimCount;

	if (other.m_pTexRef) m_pTexRef = s_MngTex.Get(other.m_pTexRef->FileName());
	m_szTexFN = other.m_szTexFN;

	// If the image is to be animated, set related variables
	m_iAnimCount = m_Children.size();	// Set the number of animated images
	if ((UISTYLE_IMAGE_ANIMATE & m_dwStyle) && m_iAnimCount>0)
	{
		m_pAnimImagesRef = new CN3UIImage*[m_iAnimCount];
		ZeroMemory(m_pAnimImagesRef, sizeof(CN3UIImage*)*m_iAnimCount);
		int i=0;
		for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
		{
			__ASSERT(UI_TYPE_IMAGE == (*itor)->UIType(), "UI type of animate image child is not image.");
			m_pAnimImagesRef[i] = (CN3UIImage*)(*itor);
			__ASSERT(m_pAnimImagesRef[i]->GetReserved() == (DWORD)i, "animate Image load fail");	// Fails if not properly aligned.
			++i;
		}
	}

	SetVB();	// vertex settings
}

#ifdef _N3TOOL
bool CN3UIImage::Save(HANDLE hFile)
{
	ReorderChildImage();	// Sort child images in order
	if (false == CN3UIBase::Save(hFile)) return false;
	DWORD dwNum;
	// texture information
	if (m_pTexRef) m_szTexFN = m_pTexRef->FileName();
	int iStrLen = m_szTexFN.size();
	WriteFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			// file length
	if (iStrLen>0)	WriteFile(hFile, m_szTexFN.c_str(), iStrLen, &dwNum, NULL);	// file name

	WriteFile(hFile, &m_frcUVRect, sizeof(m_frcUVRect), &dwNum, NULL);		// uv coordinates
	WriteFile(hFile, &m_fAnimFrame, sizeof(m_fAnimFrame), &dwNum, NULL);	// Animate frame

	return true;
}

void CN3UIImage::ChangeImagePath(const std::string& szPathOld, const std::string& szPathNew)
{
	CN3UIBase::ChangeImagePath(szPathOld, szPathNew);

	std::string szOld = szPathOld, szNew = szPathNew;

	if(!szOld.empty()) ::CharLower(&(szOld[0]));
	if(!szNew.empty()) ::CharLower(&(szNew[0]));
	if(!m_szTexFN.empty()) ::CharLower(&(m_szTexFN[0]));

	if(m_pTexRef) m_szTexFN = m_pTexRef->FileName();
	int i = m_szTexFN.find(szOld);
	if(i >= 0) 
	{
		std::string szF = m_szTexFN.substr(0, i);
		std::string szL = m_szTexFN.substr(i + szOld.size());
		m_szTexFN = szF + szNew + szL;
		s_MngTex.Delete(&m_pTexRef);
		m_pTexRef = s_MngTex.Get(m_szTexFN);
	}
}

void CN3UIImage::GatherImageFileName(std::set<std::string>& setImgFile)
{
	CN3UIBase::GatherImageFileName(setImgFile); // child information
	
	std::string szImgFN = m_szTexFN;
	if(!szImgFN.empty())
	{
		::CharLower(&(szImgFN[0]));
		setImgFile.insert(szImgFN);
	}
}

// The child images are rearranged according to the numerical order in m_dwReserved.
void CN3UIImage::ReorderChildImage()
{
	if (m_iAnimCount<=0) return;
	CN3UIBase** pNewList = new CN3UIBase*[m_iAnimCount];
	ZeroMemory(pNewList, sizeof(CN3UIBase*)*m_iAnimCount);

	int i;
	for (i=0; i<m_iAnimCount; ++i)
	{
		CN3UIBase* pSelChild = NULL;
		for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
		{
			CN3UIBase* pChild = (*itor);
			__ASSERT(UI_TYPE_IMAGE == pChild->UIType(), "There is a child that is not an image.");
			if (NULL == pSelChild) pSelChild = pChild;
			else if (pSelChild->GetReserved() > pChild->GetReserved()) pSelChild = pChild;
		}
		__ASSERT(pSelChild,"There is no child with the smallest m_dwReserved.");
		pNewList[i] = pSelChild;
		RemoveChild(pSelChild);
	}
	
	for (i=0; i<m_iAnimCount; ++i) m_Children.push_back(pNewList[i]);	// put in small order

	delete [] pNewList;
}

CN3UIImage* CN3UIImage::GetChildImage(int iIndex)
{
	if (iIndex>=0 && iIndex < m_iAnimCount)	return m_pAnimImagesRef[iIndex];
	return NULL;
}

void CN3UIImage::SetAnimImage(int iAnimCount)
{
	// Clear if already set
	int i;
	if (m_pAnimImagesRef)
	{
		for (i=0; i<m_iAnimCount; ++i)
		{	// clear git
			if (m_pAnimImagesRef[i]) {delete m_pAnimImagesRef[i]; m_pAnimImagesRef[i] = NULL;}
		}
		delete [] m_pAnimImagesRef; m_pAnimImagesRef = NULL;
	}
	m_iAnimCount = iAnimCount;

	// Set to 0 to convert to normal image
	if (0 == m_iAnimCount)
	{
		SetStyle(m_dwStyle & (~UISTYLE_IMAGE_ANIMATE));
		CreateVB();
		SetVB();
	}
	else
	{
		SetStyle(m_dwStyle | UISTYLE_IMAGE_ANIMATE);
		s_MngTex.Delete(&m_pTexRef);
		SetVB();

		m_pAnimImagesRef = new CN3UIImage*[m_iAnimCount];
		ZeroMemory(m_pAnimImagesRef, sizeof(CN3UIImage*)*m_iAnimCount);
		for (i=0; i<m_iAnimCount; ++i)
		{
			m_pAnimImagesRef[i] = new CN3UIImage();
			m_pAnimImagesRef[i]->Init(this);
			m_pAnimImagesRef[i]->SetReserved(i);
			m_pAnimImagesRef[i]->SetRegion(m_rcRegion);
		}
	}
}

bool CN3UIImage::ReplaceAllTextures(const std::string& strFind, const std::string& strReplace)
{
	if (strFind.size() <= 0 || strReplace.size() <= 0) return false;
	while (m_pTexRef)
	{
		char szFindDir[_MAX_DIR], szFindFName[_MAX_FNAME], szFindExt[_MAX_EXT];
		char szReplaceDir[_MAX_DIR], szReplaceFName[_MAX_FNAME], szReplaceExt[_MAX_EXT];
		char szTexDir[_MAX_DIR], szTexFName[_MAX_FNAME], szTexExt[_MAX_EXT];
		_splitpath(strFind.c_str(), NULL, szFindDir, szFindFName, szFindExt);
		_splitpath(strReplace.c_str(), NULL, szReplaceDir, szReplaceFName, szReplaceExt);
		_splitpath(m_pTexRef->FileName().c_str(), NULL, szTexDir, szTexFName, szTexExt);

		if (lstrlen(szFindFName) == 0 || lstrlen(szFindExt) == 0 ||
			lstrlen(szReplaceFName) == 0 || lstrlen(szReplaceExt) == 0) return false;

		std::string strNew(szTexDir);
		if (lstrcmpi(szFindFName, "*") == 0)
		{
			if (lstrcmpi(szFindExt, ".*") == 0)
			{	// *.* ->
				if (lstrcmpi(szReplaceFName, "*") == 0)	strNew += szTexFName;
				else strNew += szReplaceFName;
				if (lstrcmpi(szReplaceExt, ".*") == 0) strNew += szTexExt;
				else strNew += szReplaceExt;
			}
			else
			{	// *.tga ->
				if (lstrcmpi(szFindExt, szTexExt) != 0 ) break;	// Since the extensions are not the same, just return

				if (lstrcmpi(szReplaceFName, "*") == 0)	strNew += szTexFName;
				else strNew += szReplaceFName;
				if (lstrcmpi(szReplaceExt, ".*") == 0) strNew += szTexExt;
				else strNew += szReplaceExt;
			}
		}
		else
		{
			if (lstrcmpi(szFindFName, szTexFName) != 0 ) break;	// Since the names are not the same, just return

			if (lstrcmpi(szFindExt, ".*") == 0)
			{	// abc.* ->
				if (lstrcmpi(szReplaceFName, "*") == 0)	strNew += szFindFName;
				else strNew += szReplaceFName;
				if (lstrcmpi(szReplaceExt, ".*") == 0) strNew += szTexExt;
				else strNew += szReplaceExt;
			}
			else
			{	// If the file name and extension to be searched are specified // abc.tga ->
				if (lstrcmpi(szFindExt, szTexExt) != 0 ) break;	// Since the extensions are not the same, just return

				if (lstrcmpi(szReplaceFName, "*") == 0)	strNew += szFindFName;
				else strNew += szReplaceFName;
				if (lstrcmpi(szReplaceExt, ".*") == 0) strNew += szTexExt;
				else strNew += szReplaceExt;
			}
		}
		// Retexturing
		SetTex(strNew);
		break;
	}
	return CN3UIBase::ReplaceAllTextures(strFind, strReplace);
}
#endif
