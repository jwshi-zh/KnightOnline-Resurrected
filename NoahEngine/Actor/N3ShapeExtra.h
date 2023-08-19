#pragma once

#include "N3Shape.h"
#include <vector>

class CN3ShapeExtra : public CN3Shape  
{
protected:
	struct __Rotation
	{
		float			fRadianPerSec;
		__Vector3		vAxis;
		float			fRadianCur;
		float			fRadianToReach;

		__Rotation()
		{
			memset(this, 0, sizeof(__Rotation));
			vAxis.Set(0,1,0);
		}
	};

	std::vector<__Rotation>	m_Rotations;

public:
	void RotateTo(int iPart, const __Vector3& vAxis, float fRadianToReach, float fRadianPerSec, bool bImmediately = false); // Rotates the desired part to a specified angle along an axis at a specified speed.

	bool Load(HANDLE hFile);
	void Tick(float fFrm);
	
	void Release();
	CN3ShapeExtra();
	virtual ~CN3ShapeExtra();

};
