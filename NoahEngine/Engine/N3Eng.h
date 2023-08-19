#pragma once

#include "N3BaseFileAccess.h"
#include "ddraw.h"

class CN3Eng : public CN3Base
{
public:
	typedef struct __D3DDEV_INFO
	{
		char	szDeviceName[128];
		char	szDeviceDesc[128];
		int		nAdapter;
		int		nDevice;
		
		D3DDEVTYPE		DevType;
		int				nModeCount;
		D3DDISPLAYMODE* pModes;
	} __D3DDevInfo;

	LPDIRECT3D9			m_lpD3D;
	LPDIRECTDRAW		m_lpDD;

protected:
	int		m_nModeActive;
	int		m_nAdapterCount;
	__D3DDEV_INFO m_DeviceInfo;
	
public:
	
	HKEY RegistryOpen(const std::string& szKey);
	bool RegistryClose(HKEY& hKey);
	bool RegistryValueGet(HKEY hKey, const std::string& szName, std::string& szValue);
	bool RegistryValueGet(HKEY hKey, const std::string& szName, void* pValue, DWORD dwBytes);
	bool RegistryValueSet(HKEY hKey, const std::string& szName, std::string& szValue);
	bool RegistryValueSet(HKEY hKey, const std::string& szName, void* pValue, DWORD dwBytes);
	
	void SetDefaultEnvironment();
	bool Reset(BOOL bWindowed, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP);
	static void Clear(D3DCOLOR crFill, RECT* pRC = nullptr);
	static void ClearAuto(RECT* pRC = nullptr);
	static void ClearZBuffer(const RECT* pRC = nullptr);
	static void Present(HWND hWnd, RECT* pRC = nullptr);

	void LookAt(__Vector3& vEye, __Vector3& vAt, __Vector3& vUp);
	void SetProjection(float fNear, float fFar, float fLens, float fAspect);
	void SetViewPort(RECT& pRC);

	void Release();

//	void InitEnv();
	bool Init(BOOL bWindowed, HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, BOOL bUseHW);
	BOOL FindDepthStencilFormat(UINT iAdapter, D3DDEVTYPE DeviceType, D3DFORMAT TargetFormat, D3DFORMAT* pDepthStencilFormat) const;

	static LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	CN3Eng();
	virtual ~CN3Eng();
};
