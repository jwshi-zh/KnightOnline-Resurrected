#include "pch.h"
#include "N3AnimKey.h"

CN3AnimKey::CN3AnimKey()
{
	m_eType = KEY_VECTOR3;

	m_nCount = 0;
	m_fSamplingRate = 30.0;
	m_pDatas = nullptr;
}

CN3AnimKey::~CN3AnimKey()
{
	delete [] m_pDatas; m_pDatas = nullptr;
}

void CN3AnimKey::Release()
{
	m_nCount = 0;
	m_fSamplingRate = 30.0;
	delete [] m_pDatas; m_pDatas = nullptr;

	CN3Base::Release();
}

void CN3AnimKey::Alloc(int nCount, float fSamplingRate, ANIMATION_KEY_TYPE eType)
{
	if(nCount <= 0) return;
	if(KEY_VECTOR3 != eType && KEY_QUATERNION != eType) return;
	if(fSamplingRate <= 0.0f)
	{
		__ASSERT(0, "Animation Sampling Rate must be greater than 0.");
		return;
	}

	if(m_pDatas != nullptr)
	{
		delete [] m_pDatas; m_pDatas = nullptr;
		m_nCount = 0;
	}

	m_eType = eType;
	m_nCount = nCount;
	m_fSamplingRate = fSamplingRate;

	if(KEY_VECTOR3 == m_eType)
	{
		m_pDatas = new __Vector3[nCount+1];
		memset(m_pDatas, 0, sizeof(__Vector3) * (nCount+1));
	}
	else if(KEY_QUATERNION == m_eType)
	{
		m_pDatas = new __Quaternion[nCount+1];
		memset(m_pDatas, 0, sizeof(__Quaternion) * (nCount+1));
	}
}

bool CN3AnimKey::Load(HANDLE hFile)
{
	if(m_nCount > 0) this->Release();

	DWORD dwRWC = 0;
	ReadFile(hFile, &m_nCount, 4, &dwRWC, nullptr);

	if(m_nCount > 0)
	{
		ReadFile(hFile, &m_eType, 4, &dwRWC, nullptr);
		ReadFile(hFile, &m_fSamplingRate, 4, &dwRWC, nullptr);

		this->Alloc(m_nCount, m_fSamplingRate, m_eType);
		if(KEY_VECTOR3 == m_eType)
		{
			ReadFile(hFile, m_pDatas, sizeof(__Vector3)*m_nCount, &dwRWC, nullptr);
			auto* pKeys = (__Vector3*)m_pDatas;
			pKeys[m_nCount] = pKeys[m_nCount-1];
		}
		else if(KEY_QUATERNION == m_eType)
		{
			ReadFile(hFile, m_pDatas, sizeof(__Quaternion)*m_nCount, &dwRWC, nullptr);
			auto* pKeys = (__Quaternion*)m_pDatas;
			pKeys[m_nCount] = pKeys[m_nCount-1];
		}
	}

	return true;
}

#ifdef _N3TOOL
bool CN3AnimKey::Save(HANDLE hFile)
{
	DWORD dwRWC = 0;
	WriteFile(hFile, &m_nCount, 4, &dwRWC, NULL);

	if(m_nCount > 0)
	{
		WriteFile(hFile, &m_eType, 4, &dwRWC, NULL);
		WriteFile(hFile, &m_fSamplingRate, 4, &dwRWC, NULL);

		if(KEY_VECTOR3 == m_eType) WriteFile(hFile, m_pDatas, sizeof(__Vector3)*m_nCount, &dwRWC, NULL);
		else if(KEY_QUATERNION == m_eType) WriteFile(hFile, m_pDatas, sizeof(__Quaternion)*m_nCount, &dwRWC, NULL);
	}

	return true;
}
#endif // end of #ifdef _N3TOOL

/*
void CN3AnimKey::Translate(int nIndexStart, int nIndexEnd, float fDelta, int nKFlags)
{
	if(nIndexStart < 0 || nIndexStart >= m_nCount) return;
	if(nIndexEnd < 0 || nIndexEnd >= m_nCount) return;

	for(int i = nIndexStart; i <= nIndexEnd; i++)
	{
		if(nKFlags & 1) m_pDatas[i].x += fDelta;
		if(nKFlags & 2) m_pDatas[i].y += fDelta;
		if(nKFlags & 4) m_pDatas[i].z += fDelta;
	}
}
*/

#ifdef _N3TOOL
void CN3AnimKey::Add(CN3AnimKey &AKSrc, int nIndexS, int nIndexE)
{
	if(AKSrc.Count() <= 0) return;
	if(nIndexE < nIndexS) return;
	if(this->Type() != AKSrc.Type()) return;

	int nAddCount = nIndexE - nIndexS + 1;

	int nPrevCount = m_nCount;
	void* pVBackup = NULL;
	if(nPrevCount > 0) // backup
	{
		if(KEY_VECTOR3 == m_eType)
		{
			pVBackup = new __Vector3[nPrevCount];
			memcpy(pVBackup, m_pDatas, sizeof(__Vector3) * nPrevCount);
		}
		else if(KEY_QUATERNION == m_eType)
		{
			pVBackup = new __Quaternion[nPrevCount];
			memcpy(pVBackup, m_pDatas, sizeof(__Quaternion) * nPrevCount);
		}
	}

	this->Alloc(nPrevCount + nAddCount, AKSrc.SamplingRate(), AKSrc.Type());
	if(nPrevCount > 0)
	{
		if(KEY_VECTOR3 == m_eType)
		{
			memcpy(m_pDatas, pVBackup, sizeof(__Vector3) * nPrevCount);
		}
		else if(KEY_QUATERNION == m_eType)
		{
			memcpy(m_pDatas, pVBackup, sizeof(__Quaternion) * nPrevCount);
		}
	}

	if(KEY_VECTOR3 == m_eType)
	{
		for(int i = 0; i < nAddCount; i++)
		{
			__Vector3* pvTmp = (__Vector3*)AKSrc.DataGet(nIndexS + i);
			if(pvTmp) ((__Vector3*)m_pDatas)[nPrevCount + i] = *pvTmp;
			else ((__Vector3*)m_pDatas)[nPrevCount + i].Zero();
		}
	}
	else if(KEY_QUATERNION == m_eType)
	{
		for(int i = 0; i < nAddCount; i++)
		{
			__Quaternion* pvTmp = (__Quaternion*)AKSrc.DataGet(nIndexS + i);
			if(pvTmp) ((__Quaternion*)m_pDatas)[nPrevCount + i] = *pvTmp;
			else
			{
				((__Quaternion*)m_pDatas)[nPrevCount + i].x = 
				((__Quaternion*)m_pDatas)[nPrevCount + i].y = 
				((__Quaternion*)m_pDatas)[nPrevCount + i].z = 
				((__Quaternion*)m_pDatas)[nPrevCount + i].w = 0;
			}
		}
	}

	delete [] pVBackup; pVBackup = NULL;
}
#endif // end of _N3TOOL

void CN3AnimKey::Duplicate(CN3AnimKey* pSrc)
{
	if(m_nCount > 0) this->Release();

	m_nCount = pSrc->Count();

	if(m_nCount > 0)
	{
		m_eType = pSrc->Type();
		m_fSamplingRate = pSrc->SamplingRate();

		this->Alloc(m_nCount, m_fSamplingRate, m_eType);

		if(KEY_VECTOR3 == m_eType)
		{
			const char* pData = (char*)pSrc->GetDatas();
			memcpy(m_pDatas, pData, sizeof(__Vector3)*m_nCount);
		}
		else if(KEY_QUATERNION == m_eType)
		{
			const char* pData = (char*)pSrc->GetDatas();
			memcpy(m_pDatas, pData, sizeof(__Quaternion)*m_nCount);
		}
	}	
}

void CN3AnimKey::MultiplyDataVector(__Vector3 vM) const
{
	if(KEY_VECTOR3 != m_eType) return;

	for(int i=0;i<m_nCount;i++)
	{
		__Vector3 vT = ((__Vector3*)m_pDatas)[i];
		vT*=vM;
		((__Vector3*)m_pDatas)[i] = vT;
	}
}
