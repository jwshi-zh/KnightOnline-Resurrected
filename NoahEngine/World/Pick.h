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
    __Vector3		m_vPickRayDir;			// �浹 üũ�� ������ ����
    __Vector3		m_vPickRayOrig;			// �浹 üũ�� ������ ������

// Operations
public:
	BOOL PickByBox(__Vector3 &vMin, __Vector3 &vMax, __Vector3& vIntersect);
	void			SetPickXY(long x, long y); // Ŭ���̾�Ʈ ��ǥ�� �־��ش�..

	// [in] v0, v1, v2 ������ ��ǥ
	// [out] t�� Ŭ���� ������ ���� ������ ���� �ִ�.	t*dir + orig �� ���ϸ� ������ ���� ���� �� �ִ�.
	// [out] uv ��ǥ  v0 (0,0), v1(1,0), v2(0,1) <��ȣ���� (U, V)��ǥ> �̷������� ��� ���� ������ ��Ÿ�� ��
	// v0 + (v1-v0)*u + (v2-v0)*v �� ���ϸ� ������ ���� ���� �� �ִ�.
	// [out] vPos ���콺�� ���� ���� 3D ��ǥ
	// ���� ���� FALSE�̸� ������ �̷�� �ﰢ�� �ȿ��� ������ �ʴ´ٴ� ��, t u v vPos ��� ��ȿ�� ���̹Ƿ� ���� �ȵȴ�.
	bool IntersectTriangle(__Vector3& v0, __Vector3& v1, __Vector3& v2, float& t, float& u, float& v, __Vector3* pVPos)
	{
		return ::_IntersectTriangle( m_vPickRayOrig, m_vPickRayDir, v0, v1, v2, t, u, v, pVPos );
	}
};
