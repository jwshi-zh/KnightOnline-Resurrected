#pragma once

#include "N3Base.h"

enum {	GEW_CLEAR=0,			// ����
		GEW_DRIZZLE_RAIN=1,		// ������
		GEW_RAINY=2,			// ���緮�� ��
		GEW_HEAVY_RAIN=3,		// ����
		GEW_SNOW1=11,			// ������ ��
		GEW_SNOW2=12,			// ������ ��
		GEW_HEAVY_SNOW=13		// ����
};

class CN3GERain;
class CN3GESnow;
class CN3GlobalEffectMng : public CN3Base  
{
public:
	CN3GlobalEffectMng();
	virtual ~CN3GlobalEffectMng();

// Attributes
public:
protected:
	float			m_fCellSize;	// ���� �� ũ��
	POINT			m_CurCellPos;	// ���� ���μ� ��ǥ
	__Vector3		m_vPos[9];		// �ֺ� �� 9ĭ�� �߽���ǥ��

	// ȿ����..
	CN3GERain*		m_pGERain;		// ��
	CN3GESnow*		m_pGESnow;		// ��

// Operations
public:
	virtual void	Release();
	void			Tick();
	void			Render();
//	void			SetWeather(int iWeather);	// ���� �����ϱ�
	void			WeatherSetRainy(int iPercent);
	void			WeatherSetSnow(int iPercent);
	void			WeatherSetClean();

protected:

};
