#include "pch.h"
#include "BirdMng.h"
#include "Bird.h"

CBirdMng::CBirdMng()
{
	m_pBird = nullptr;
	Release();
}

CBirdMng::~CBirdMng()
{
	Release();
}

void CBirdMng::Release()
{
	if(m_pBird)
	{
		delete[] m_pBird;
		m_pBird = nullptr;
	}
	m_iBirdCount = 0;
}

void CBirdMng::LoadFromFile(const std::string& szFN)
{
	Release();

	if(szFN.empty()) return;
	FILE* stream = fopen(szFN.c_str(), "r"); // make a text file

	if(nullptr == stream)
	{
#if _DEBUG
		char szErr[512];
		wsprintf(szErr, "failed to open file - %s", szFN.c_str());
		__ASSERT(stream, szErr);
#endif
		return;
	}

	int i;
	char szRrcName[_MAX_PATH];
	int result = fscanf(stream, "count = %d\n", &m_iBirdCount);			__ASSERT(result != EOF, "Invalid machine setting file");

	if(m_iBirdCount>0) m_pBird = new CBird[m_iBirdCount];

	for (i=0; i<m_iBirdCount; i++)
	{
		result = fscanf(stream, "%s\n", szRrcName);	__ASSERT(result != EOF, "Invalid bird list settings file");
		m_pBird[i].LoadBird(szRrcName);		
	}
	fclose(stream);
}

void CBirdMng::Tick() const
{
	for(int i=0; i<m_iBirdCount; i++)
	{
		m_pBird[i].Tick();
	}
}

void CBirdMng::Render()
{
	for(int i=0; i<m_iBirdCount; i++)
	{
		m_pBird[i].Render();
	}
}