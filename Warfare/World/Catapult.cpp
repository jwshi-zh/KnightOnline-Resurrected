#include "pch.h"
#include "Catapult.h"

CCatapult::CCatapult()
{
	m_Thrower.pStone = nullptr;
	Release();
}

CCatapult::~CCatapult()
{
	Release();
}

void CCatapult::Release()
{
	CMachineBase::Release();
	if (m_Thrower.pStone) { delete m_Thrower.pStone; m_Thrower.pStone = nullptr;}
	ZeroMemory(&m_Thrower, sizeof(m_Thrower));
}

void CCatapult::ReCalcMatrix4AnimatedPart()
{
	CMachineBase::ReCalcMatrix4AnimatedPart();

	if (m_Thrower.bFire == TRUE)	// if firing
	{
		const int iSize = m_Parts.size();
		for(int i = 0; i < iSize; i++)
		{
			if (m_Parts[i] == m_Thrower.pThrowerPart)
			{
				m_Parts[i]->m_Matrix.RotationX(m_Thrower.fCurRadian);
				m_Parts[i]->m_Matrix.PosSet(m_Parts[i]->m_vPivot);
				m_Parts[i]->m_Matrix *= m_Matrix;
				m_bSkipCalcPartMtx[i] = TRUE;
				break;
			}
		}
	}
}

void CCatapult::Render()
{
	CMachineBase::Render();

	__ASSERT(m_Thrower.pStone, "No rocks in catapult");
	if (m_Thrower.bDontRenderStone == FALSE ) m_Thrower.pStone->Render();
}

void CCatapult::Tick(float fFrm)
{
/*	if (m_bDontRender)
	{
		CN3Shape::Tick(fFrm);
		if (m_bDontRender) return;
	}

	BOOL IsStoneThrown = FALSE;
	if (m_Thrower.bFire == TRUE) // if fire state
	{
		m_Thrower.fTime += s_fSecPerFrm;
		float fReloadTime = m_Thrower.fTime - (m_Thrower.fReleaseTime + m_Thrower.fReloadDelayTime);
		if (fReloadTime<0) // firing or delaying...
		{
			float fVibrationTime = m_Thrower.fTime - m_Thrower.fReleaseTime;
			if (fVibrationTime<0)
			{
				m_Thrower.fCurRadian = -m_Thrower.fRadianAccel*(m_Thrower.fTime*m_Thrower.fTime);
			}
			else // Vibrating due to recoil after firing
			{
				if (m_Thrower.bDontRenderStone == FALSE)
				{
					m_Thrower.bDontRenderStone = TRUE; // don't draw boulders

					// The stone throwing message should occur (once only)
					IsStoneThrown = TRUE;
				}

				float fFactor = fVibrationTime*20.0f;
				// Only 5 vibrations...
				if ( fFactor < D3DX_PI*8) m_Thrower.fCurRadian = -m_Thrower.fLimitRadian - sinf(fFactor) * (m_Thrower.fRecoilRadian/(10*(1.0f+fVibrationTime)*(1.0f+fVibrationTime)));
				else m_Thrower.fCurRadian = -m_Thrower.fLimitRadian;
			}
		}
		else
		{
			m_Thrower.bDontRenderStone = TRUE; // don't draw boulders

			m_Thrower.fCurRadian = -(m_Thrower.fLimitRadian - fReloadTime*m_Thrower.fRadianSpeed2Reload);
			if (m_Thrower.fCurRadian > 0)
			{
				m_Thrower.bFire = FALSE;
				m_Thrower.fCurRadian = 0;
				m_Thrower.fTime = 0;
				m_Thrower.bDontRenderStone = FALSE;
			}
		}
	}
	CMachineBase::Tick(fFrm);

	// Calculate the position of the boulder
	if (m_Thrower.bDontRenderStone == FALSE)
	{
		__Matrix44& mtx = m_Thrower.pStone->m_Matrix;
		mtx.Identity();
		mtx.PosSet(m_Thrower.vStoneOffset);
		mtx *= m_Thrower.pThrowerPart->m_Matrix;
		m_Thrower.pStone->ReCalcPartMatrix();
		m_Thrower.pStone->PosSet(mtx._41, mtx._42, mtx._43);
		m_Thrower.pStone->Tick(fFrm);
	}

	if (IsStoneThrown)
	{
		__Matrix44 mtx; // Get the rotation matrix...
		mtx.Identity();
		mtx.RotationX(m_Thrower.fLimitRadian);
		mtx.PosSet(m_Thrower.pThrowerPart->m_vPivot);
		mtx *= m_Matrix;

		// generate stone throwing message
		// trigger - 08.25 Create and send a direct message..
		LocalMsg Msg;
		Msg.s_MagicNum			= MAGIC_NUM;
		Msg.s_iAddressLen		= CN3GameBase::s_pMsgRouter->GetThisClassLevel(ID_FX_MGR);
		Msg.s_iAddIndex			= 0;
		Msg.s_ipAddress[0]		= ID_ARITH;
		Msg.s_ipAddress[1]		= ID_FX_MGR;

		Msg.s_iGameID			= ID_THROW_STONE;
		Msg.s_vPos				= m_Thrower.vStoneOffset * mtx; // location
		__Vector3 vVel; vVel.Set(0, sinf(m_fFireRadian), cosf(m_fFireRadian)); // Calculate direction.
		mtx = m_Matrix; mtx.PosSet(0,0,0);
		Msg.s_vDir= vVel*mtx; Msg.s_vDir.Normalize();// direction
		Msg.s_fVelocity = m_fFireSpeed; // flying speed...
		Msg.s_pShape			= m_Thrower.pStone; // stone shape
		
		CN3GameBase::s_pMsgRouter->m_cMsgQueue.RouteLocalMsg(Msg); // Message sent..
	}
*/
}

void CCatapult::LoadMachine(FILE* stream)
{
	if (stream == nullptr) return;
	Release();
	CMachineBase::LoadMachine(stream);

	char szThrowerName[_MAX_PATH];	// Thrower pmeshfilename
	char szStoneShapeName[_MAX_PATH];	// name of stone shape

	int result;
	float x, y, z;
	result = fscanf(stream, "Thrower_Name = %s\n", &szThrowerName);						__ASSERT(result != EOF, "Invalid machine setting file");
	result = fscanf(stream, "Thrower_LimitRadian = %f\n", &(m_Thrower.fLimitRadian));	__ASSERT(result != EOF, "Invalid machine setting file");
	result = fscanf(stream, "Thrower_RadianAccel = %f\n", &(m_Thrower.fRadianAccel));	__ASSERT(result != EOF, "Invalid machine setting file");
	result = fscanf(stream, "Thrower_RadianSpeed2Reload = %f\n", &(m_Thrower.fRadianSpeed2Reload));	__ASSERT(result != EOF, "Invalid machine setting file");
	result = fscanf(stream, "Thrower_ReloadDelayTime = %f\n", &(m_Thrower.fReloadDelayTime));		__ASSERT(result != EOF, "Invalid machine setting file");
	result = fscanf(stream, "Thrower_RecoilRadian = %f\n", &(m_Thrower.fRecoilRadian));				__ASSERT(result != EOF, "Invalid machine setting file");
	result = fscanf(stream, "Thrower_StoneShapeName = %s\n", szStoneShapeName);						__ASSERT(result != EOF, "Invalid machine setting file");
	result = fscanf(stream, "Thrower_StoneOffset = %f %f %f\n", &x, &y, &z);						__ASSERT(result != EOF, "Invalid machine setting file");

	__ASSERT(m_Thrower.pStone == NULL, "Possibility of catapult memory leak");
	m_Thrower.pStone = new CN3Shape;
	m_Thrower.pStone->Load(szStoneShapeName);
	m_Thrower.vStoneOffset.Set(x, y, z);

	// Calculate how long it takes to launch
	m_Thrower.fReleaseTime = sqrtf(m_Thrower.fLimitRadian/m_Thrower.fRadianAccel);

	// Find Thrower
	m_Thrower.pThrowerPart = GetPartByPMeshName(szThrowerName);
}

void CCatapult::Fire()
{
	if (m_Thrower.bFire == TRUE) return;	// It is already in launch condition.

	m_Thrower.bFire = TRUE;
	m_Thrower.fTime = 0;
	m_Thrower.fCurRadian = 0;
	m_Thrower.bDontRenderStone = FALSE;	// stone drawing
}
