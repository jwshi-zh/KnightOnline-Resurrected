#pragma once

#include <vector>
#include <map>

#include "LogWriter.h"

template <typename Type> class CN3TableBase
{
public:
	CN3TableBase();
	virtual ~CN3TableBase();

protected:
	enum DATA_TYPE {DT_NONE, DT_CHAR, DT_BYTE, DT_SHORT, DT_WORD, DT_INT, DT_DWORD, DT_STRING, DT_FLOAT, DT_DOUBLE};
	
	std::vector<DATA_TYPE> m_DataTypes;
	std::map<unsigned int, Type> m_Datas;

// Operations
public:
	void	Release();
	Type*	Find(unsigned int dwID)
	{
		auto it = m_Datas.find(dwID);
		if(it == m_Datas.end()) return nullptr;
		else return &(it->second);
	}
	int		GetSize() { return m_Datas.size(); }
	Type*	GetIndexedData(int index)
	{
		if(index < 0 || m_Datas.empty()) return nullptr;
		if(index >= m_Datas.size()) return nullptr;
		
		auto it = m_Datas.begin();
		for(int i = 0; i < index; i++, it++);
		return &(it->second);
	}
	int		IDToIndex(unsigned int dwID)
	{
		auto it = m_Datas.find(dwID);
		if(it == m_Datas.end()) return -1;

		auto itSkill = m_Datas.begin();
		const int iSize = m_Datas.size();
		for(int i = 0; i < iSize; i++, itSkill++)
			if (itSkill == it)	return i;

		return -1;
	}
	BOOL	LoadFromFile(const std::string& szFN);
protected:
	BOOL	Load(HANDLE hFile);
	BOOL	WriteData(HANDLE hFile, DATA_TYPE DataType, const char* lpszData);
	BOOL	ReadData(HANDLE hFile, DATA_TYPE DataType, void* pData);

	int		SizeOf(DATA_TYPE DataType) const;
	BOOL	MakeOffsetTable(std::vector<int>& offsets);
};

template <class Type>
CN3TableBase<Type>::CN3TableBase()
{
}

template <class Type>
CN3TableBase<Type>::~CN3TableBase()
{
	Release();
}

template <class Type>
void CN3TableBase<Type>::Release()
{
	m_DataTypes.clear();
	m_Datas.clear();
}

template <class Type>
BOOL CN3TableBase<Type>::WriteData(HANDLE hFile, DATA_TYPE DataType, const char* lpszData)
{
	DWORD dwNum;
	switch(DataType)
	{
	case DT_CHAR:
		{
			char cWrite;
			if (isdigit(lpszData[0]))
			{
				const int iTemp = atoi(lpszData);
				if (iTemp < -127 || iTemp > 128) return FALSE;
				cWrite = (char)iTemp;
			}
			else return FALSE;

			WriteFile(hFile, &cWrite, sizeof(cWrite), &dwNum, nullptr);
		}
		break;
	case DT_BYTE:
		{
			BYTE byteWrite;
			if (isdigit(lpszData[0]))
			{
				const int iTemp = atoi(lpszData);
				if (iTemp < 0 || iTemp > 255) return FALSE;
				byteWrite = (BYTE)iTemp;
			}
			else return FALSE;

			WriteFile(hFile, &byteWrite, sizeof(byteWrite), &dwNum, nullptr);
		}
		break;
	case DT_SHORT:
		{
			short iWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] )
			{
				const int iTemp = atoi(lpszData);
				if (iTemp < -32767 || iTemp > 32768) return FALSE;
				iWrite = (short)iTemp;
			}
			else return FALSE;

			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, nullptr);
		}
		break;
	case DT_WORD:
		{
			WORD iWrite;
			if (isdigit(lpszData[0]) )
			{
				const int iTemp = atoi(lpszData);
				if (iTemp < 0 || iTemp > 65535) return FALSE;
				iWrite = (short)iTemp;
			}
			else return FALSE;

			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, nullptr);
		}
		break;
	case DT_INT:
		{
			int iWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] )	iWrite = atoi(lpszData);
			else return FALSE;

			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, nullptr);
		}
		break;
	case DT_DWORD:
		{
			DWORD iWrite;
			if (isdigit(lpszData[0]) )	iWrite = strtoul(lpszData, nullptr, 10);
			else return FALSE;

			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, nullptr);
		}
		break;
	case DT_STRING:
		{
			const std::string& szString = *((std::string*)lpszData);
			const int iStrLen = szString.size();
			WriteFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, nullptr);
			if (iStrLen>0) WriteFile(hFile, &(szString[0]), iStrLen, &dwNum, nullptr);
		}
		break;
	case DT_FLOAT:
		{
			float fWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] ||
				'.' == lpszData[0] )	fWrite = (float)atof(lpszData);
			else return FALSE;
			WriteFile(hFile, &fWrite, sizeof(fWrite), &dwNum, nullptr);
		}
		break;
	case DT_DOUBLE:
		{
			double dWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] ||
				'.' == lpszData[0] )	dWrite = atof(lpszData);
			WriteFile(hFile, &dWrite, sizeof(dWrite), &dwNum, nullptr);
		}
		break;

	case DT_NONE:
	default:
		__ASSERT(0,"");
	}
	return TRUE;
}

template <class Type>
BOOL CN3TableBase<Type>::ReadData(HANDLE hFile, DATA_TYPE DataType, void* pData)
{
	DWORD dwNum;
	switch(DataType)
	{
	case DT_CHAR:
		{
			ReadFile(hFile, pData, sizeof(char), &dwNum, nullptr);
		}
		break;
	case DT_BYTE:
		{
			ReadFile(hFile, pData, sizeof(BYTE), &dwNum, nullptr);
		}
		break;
	case DT_SHORT:
		{
			ReadFile(hFile, pData, sizeof(short), &dwNum, nullptr);
		}
		break;
	case DT_WORD:
		{
			ReadFile(hFile, pData, sizeof(WORD), &dwNum, nullptr);
		}
		break;
	case DT_INT:
		{
			ReadFile(hFile, pData, sizeof(int), &dwNum, nullptr);
		}
		break;
	case DT_DWORD:
		{
			ReadFile(hFile, pData, sizeof(DWORD), &dwNum, nullptr);
		}
		break;
	case DT_STRING:
		{
			std::string& szString = *((std::string*)pData);
			
			int iStrLen = 0;
			ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, nullptr);

			szString = "";
			if (iStrLen>0)
			{
				szString.assign(iStrLen, ' ');
				ReadFile(hFile, &(szString[0]), iStrLen, &dwNum, nullptr);
			}
		}
		break;
	case DT_FLOAT:
		{
			ReadFile(hFile, pData, sizeof(float), &dwNum, nullptr);
		}
		break;
	case DT_DOUBLE:
		{
			ReadFile(hFile, pData, sizeof(double), &dwNum, nullptr);
		}
		break;

	case DT_NONE:
	default:
		__ASSERT(0,"");
		return FALSE;
	}
	return TRUE;
}

template <class Type>
BOOL CN3TableBase<Type>::LoadFromFile(const std::string& szFN)
{
	if(szFN.empty()) return FALSE;

	HANDLE hFile = ::CreateFile(szFN.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if(INVALID_HANDLE_VALUE == hFile)
	{
#ifdef _N3GAME
		CLogWriter::Write("N3TableBase - Can't open file(read) File Handle error (%s)", szFN.c_str());
#endif
		return FALSE;
	}

	const std::string szFNTmp = szFN + ".tmp";
	DWORD dwSizeHigh = 0;
	const DWORD dwSizeLow = ::GetFileSize(hFile, &dwSizeHigh);
	if(dwSizeLow <= 0)
	{
		CloseHandle(hFile);
		::remove(szFNTmp.c_str());
		return FALSE;
	}

	// 원래 파일을 읽고..
	BYTE* pDatas = new BYTE[dwSizeLow];
	DWORD dwRWC = 0;
	::ReadFile(hFile, pDatas, dwSizeLow, &dwRWC, nullptr);
	CloseHandle(hFile);

	WORD key_r = 0x0816;
	const WORD key_c1 = 0x6081;
	const WORD key_c2 = 0x1608;

	for(int i = 0; i < dwSizeLow; i++)
	{
		const BYTE byData = (pDatas[i] ^ (key_r>>8));
		key_r = (pDatas[i] + key_r) * key_c1 + key_c2;
		pDatas[i] = byData;
	}

	hFile = ::CreateFile(szFNTmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	::WriteFile(hFile, pDatas, dwSizeLow, &dwRWC, nullptr);
	CloseHandle(hFile);
	delete [] pDatas; pDatas = nullptr;

	hFile = ::CreateFile(szFNTmp.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr); // 임시 파일 읽기 모드로 열기.


	const BOOL bResult = Load(hFile);

	CloseHandle(hFile);

	if (FALSE == bResult)
	{
#ifdef _N3GAME
		CLogWriter::Write("N3TableBase - incorrect table (%s)", szFN.c_str());
#endif
	}

	::remove(szFNTmp.c_str());

	return bResult;
}

template <class Type>
BOOL CN3TableBase<Type>::Load(HANDLE hFile)
{
	Release();

	DWORD dwNum;
	int i, j, iDataTypeCount = 0;
	ReadFile(hFile, &iDataTypeCount, 4, &dwNum, nullptr);

	std::vector<int> offsets;
	__ASSERT(iDataTypeCount>0, "Data Type 이 0 이하입니다.");
	if (iDataTypeCount>0)
	{
		m_DataTypes.insert(m_DataTypes.begin(), iDataTypeCount, DT_NONE);
		ReadFile(hFile, &(m_DataTypes[0]), sizeof(DATA_TYPE)*iDataTypeCount, &dwNum, nullptr);

		if(FALSE == MakeOffsetTable(offsets))
		{
			__ASSERT(0, "can't make offset table");
			return FALSE;
		}

		const int iSize = offsets[iDataTypeCount];
		if (sizeof(Type) != iSize ||
			DT_DWORD != m_DataTypes[0] )
		{
			m_DataTypes.clear();
			__ASSERT(0, "DataType is mismatch or DataSize is incorrect!!");
			return FALSE;
		}
	}

	int iRC;
	ReadFile(hFile, &iRC, sizeof(iRC), &dwNum, nullptr);
	Type Data;
	for (i=0; i<iRC; ++i)
	{
		for (j=0; j<iDataTypeCount; ++j)
		{
			ReadData(hFile, m_DataTypes[j], (char*)(&Data) + offsets[j]);
		}

		unsigned int dwKey = *((unsigned int*)(&Data));
		auto pt = m_Datas.insert(std::make_pair(dwKey, Data));

		__ASSERT(pt.second, "CN3TableBase<Type> : Key.");
	}
	return TRUE;
}

template <class Type>
int CN3TableBase<Type>::SizeOf(DATA_TYPE DataType) const
{
	switch(DataType)
	{
	case DT_CHAR:
		return sizeof(char);
	case DT_BYTE:
		return sizeof(BYTE);
	case DT_SHORT:
		return sizeof(short);
	case DT_WORD:
		return sizeof(WORD);
	case DT_INT:
		return sizeof(int);
	case DT_DWORD:
		return sizeof(DWORD);
	case DT_STRING:
		return sizeof(std::string);
	case DT_FLOAT:
		return sizeof(float);
	case DT_DOUBLE:
		return sizeof(double);
	}
	__ASSERT(0,"");
	return 0;
}

template <class Type>
BOOL CN3TableBase<Type>::MakeOffsetTable(std::vector<int>& offsets)
{	
	if (m_DataTypes.empty()) return false;

	int i, iDataTypeCount = m_DataTypes.size();
	offsets.clear();
	offsets.assign(iDataTypeCount+1, 0);
	offsets[0] = 0;
	int iPrevDataSize = SizeOf(m_DataTypes[0]);
	for (i=1; i<iDataTypeCount; ++i)
	{
		const int iCurDataSize = SizeOf(m_DataTypes[i]);
		if (1 == iCurDataSize%4)
		{
			offsets[i] = offsets[i-1] + iPrevDataSize;
		}
		else if (2 == iCurDataSize%4)
		{
			if (0 == ((offsets[i-1]+iPrevDataSize) % 2))
				offsets[i] = offsets[i-1] + iPrevDataSize;
			else
				offsets[i] = offsets[i-1] + iPrevDataSize+1;
		}
		else if (0 == iCurDataSize%4)
		{
			if (0 == ((offsets[i-1]+iPrevDataSize) % 4))
				offsets[i] = offsets[i-1] + iPrevDataSize;
			else
				offsets[i] = ((int)(offsets[i-1] + iPrevDataSize + 3)/4)*4;
		}
		else __ASSERT(0,"");
		iPrevDataSize = iCurDataSize;
	}

	offsets[iDataTypeCount] = ((int)(offsets[iDataTypeCount-1] + iPrevDataSize + 3)/4)*4;

	return true;
}
