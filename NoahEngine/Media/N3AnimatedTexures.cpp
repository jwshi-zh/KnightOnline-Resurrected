#include "pch.h"
#include "N3Texture.h"
#include "N3AnimatedTexures.h"

CN3AnimatedTexures::CN3AnimatedTexures()
{
	m_fTexIndex = 0.0f;
	m_fTexFPS = 30.0f;
}

CN3AnimatedTexures::~CN3AnimatedTexures()
{
	const int iTC = m_TexRefs.size();
	for(int i = 0; i < iTC; i++) s_MngTex.Delete(&m_TexRefs[i]);
}

void CN3AnimatedTexures::Release()
{
	m_fTexIndex = 0.0f;
	m_fTexFPS = 30.0f;

	const int iTC = m_TexRefs.size();
	for(int i = 0; i < iTC; i++) s_MngTex.Delete(&m_TexRefs[i]);
	m_TexRefs.clear();
}

bool CN3AnimatedTexures::Load(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);

	DWORD dwRWC;

	ReadFile(hFile, &m_fTexFPS, 4, &dwRWC, nullptr);
	int iTC = 0;
	ReadFile(hFile, &iTC, 4, &dwRWC, nullptr);
	if(!m_TexRefs.empty())
	{
		const int iTCPrev = m_TexRefs.size();
		for(int i = 0; i < iTCPrev; i++) s_MngTex.Delete(&m_TexRefs[i]);
		m_TexRefs.clear();
	}
	
	int nL = 0;
	char szFN[256] = "";
	m_TexRefs.assign(iTC, nullptr);
	for(int i = 0; i < iTC; i++)
	{
		ReadFile(hFile, &nL, 4, &dwRWC, nullptr);
		if(nL > 0)
		{
			ReadFile(hFile, szFN, nL, &dwRWC, nullptr); szFN[nL] = NULL;
			m_TexRefs[i] = s_MngTex.Get(szFN);
		}
	}

	return true;
}

#ifdef _N3TOOL
bool CN3AnimatedTexures::Save(HANDLE hFile)
{
	CN3BaseFileAccess::Save(hFile);

	DWORD dwRWC;

	WriteFile(hFile, &m_fTexFPS, 4, &dwRWC, NULL);
	int iTC = m_TexRefs.size();
	WriteFile(hFile, &iTC, 4, &dwRWC, NULL);
	
	for(int i = 0; i < iTC; i++)
	{
		nL = 0;
		if(m_TexRefs[i]) nL = m_TexRefs[i].size();
		WriteFile(hFile, &nL, 4, &dwRWC, NULL);
		if(nL > 0)
		{
			WriteFile(hFile, m_TexRefs[i]->FileName().c_str(), nL, &dwRWC, NULL);
		}
	}

	return true;
}
#endif

void CN3AnimatedTexures::Tick()
{
	if(m_fTexFPS <= 0 || m_TexRefs.empty()) return;

	const int iTC = m_TexRefs.size();
	m_fTexIndex += CN3Base::s_fSecPerFrm * m_fTexFPS;
	if(m_fTexIndex >= iTC) m_fTexIndex -= (iTC * m_fTexIndex) / iTC;
}
