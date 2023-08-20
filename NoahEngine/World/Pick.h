#pragma once

#include "N3Mesh.h"

class CPick
{
public:
	CPick();
	virtual ~CPick();

// Attributes
public:
	CN3Mesh m_MeshBox; // Picking Box

protected:
    __Vector3		m_vPickRayDir;			// Direction of the straight line to be checked for collision
    __Vector3		m_vPickRayOrig;			// The starting point of the straight line to be checked for collision

// Operations
public:
	BOOL PickByBox(__Vector3 &vMin, __Vector3 &vMax, __Vector3& vIntersect);
	void			SetPickXY(long x, long y); // Enter the client coordinates.

	// [in] Coordinates of the three points v0, v1, v2
	// [out] The larger t is, the farther the line meets the plane. If you find t*dir + orig, you can find the intersection point.
	// [out] uv coordinates v0 (0,0), v1 (1,0), v2 (0,1) < (U, V) coordinates in parentheses&gt; Indicates which point is closest to this way
	// The intersection point can be obtained by calculating v0 + (v1-v0)*u + (v2-v0)*v.
	// [out] 3D coordinates of the location taken with the vPos mouse
	// If the return value is FALSE, it means that the three points do not meet within the triangle, and tuv vPos are all invalid values and should not be used.
	bool IntersectTriangle(__Vector3& v0, __Vector3& v1, __Vector3& v2, float& t, float& u, float& v, __Vector3* pVPos) const
	{
		return ::_IntersectTriangle( m_vPickRayOrig, m_vPickRayDir, v0, v1, v2, t, u, v, pVPos );
	}
};
