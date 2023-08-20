#include "N3VectorAndMatrix.h"

extern float __sfSs[3600]; // sine
extern float __sfCs[3600]; // consine
extern float __sfTs[3600]; // tangent

inline void InitTrigonometricFunction()
{
	float fRadian;
	for(int i = 0; i < 3600; i++)
	{
		fRadian = ((i/10.0f)/180.0f) * 3.141592654f;
		__sfSs[i] = sinf(fRadian);
		__sfCs[i] = cosf(fRadian);
		__sfTs[i] = tanf(fRadian);
	}
}

inline float __Sine(float fRadian) // 