#pragma once

#include "N3Shape.h"
#include <vector>

class CN3ShapeExtra : public CN3Shape  
{
protected:
	struct __Rotation
	{
		float			fRadianPerSec;	// �ʴ� ȸ��.. Radian...
		__Vector3		vAxis;			// ȸ����..
		float			fRadianCur;		// ���� ȸ����..
		float			fRadianToReach;	// ȸ����ų��..

		__Rotation()
		{
			memset(this, 0, sizeof(__Rotation));
			vAxis.Set(0,1,0);
		}
	};

	std::vector<__Rotation>	m_Rotations;

public:
	void RotateTo(int iPart, const __Vector3& vAxis, float fRadianToReach, float fRadianPerSec, bool bImmediately = false); // ���ϴ� ��Ʈ�� �࿡ ���� ������ �������� ������ �ӵ��� ȸ����Ų��..

	bool Load(HANDLE hFile);
	void Tick(float fFrm);
	
	void Release();
	CN3ShapeExtra();
	virtual ~CN3ShapeExtra();

};
