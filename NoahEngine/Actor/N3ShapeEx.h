#pragma once

#include "N3Shape.h"

const DWORD AF_POS = 0x00000001;		// Pos ��ȭ�� �ִ�.
const DWORD AF_SCALE = 0x00000002;		// Scale ��ȭ�� �ִ�.
const DWORD AF_ROTATION = 0x00000004;		// Rotation ��ȭ�� �ִ�.

struct __ActionInfo
{
	DWORD				dwActionFlag;			// Pos, Scale, Rot ��ȭ�� �ִ°�?
	std::vector<__Vector3>		ActionStateInfos_Pos;		// ������ ���� ������(�̵� ���� �ִٸ� m_iActionStateCount��)
	std::vector<__Vector3>		ActionStateInfos_Scale;	// Ȯ�����
	std::vector<__Quaternion>	ActionStateInfos_Rot;		// ȸ��
//	__Vector3			vCurAction_Pos;	// ���� ������ ���� ����(tick�� ���� ��ȭ�Ѵ�)
//	__Vector3			vCurAction_Scale;	// ���� ������ ���� ����(tick�� ���� ��ȭ�Ѵ�)
//	__Quaternion		qtCurAction_Rot;	// ���� ������ ���� ����(tick�� ���� ��ȭ�Ѵ�)
};

class CN3SPartEx : public CN3SPart
{
	friend class CN3ShapeEx;
public:
	CN3SPartEx();
	virtual ~CN3SPartEx();
// Attributes
public:
protected:
	__ActionInfo		m_ActionInfo;	// �����ӿ� ���� ����
// Operations
public:
	virtual void Release();
	virtual bool Load(HANDLE hFile);
#ifdef _N3TOOL
	virtual bool Save(HANDLE hFile);
#endif // end of _N3TOOL

protected:
	void	Interpolate(const __Matrix44& mtxParent, int iCurActionState, int iNextActionState, float fPercentage);
};

class CN3ShapeEx : public CN3Shape  
{
public:
	CN3ShapeEx();
	virtual ~CN3ShapeEx();

// Structures
protected:

// Attributes
public:
protected:
	int		m_iActionStateCount;		// ���°� ��� �ִ��� ��Ÿ����.
	int		m_iCurActionState;		// ���� ����
	int		m_iPrevActionState;		// ���� ����(���ο� ���¸� �������ָ� Animation�Ǵ� ���� �������¸� �����صд�.
	float	m_fActionTimeChanged;		// ���°� ������ �ٲ�� �ð�
	float	m_fActionTimeChanging;	// ���°� �ٲ�� ���۽ð����� ���ݱ����� ��� �ð�

	__ActionInfo		m_ActionInfo;	// �����ӿ� ���� ����

// Operations
public:
	BOOL	SetState(int iState, float fSec);	// fSec�ð����� ���¸� �ٲ۴�. (fSec�� 0�ϰ�� ��� �ٲ��.)
	virtual void	Release();
	virtual void	ReCalcMatrix();
	virtual void	Tick(float fFrm = FRAME_SELFPLAY);
	virtual bool	Load(HANDLE hFile);
protected:
};
