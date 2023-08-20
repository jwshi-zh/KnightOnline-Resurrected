#include "pch.h"
#include "PlayerNPC.h"
#include "GameProcMain.h"
#include "N3WorldManager.h"
#include "N3Shape.h"
#include "N3ShapeMgr.h"
#include "N3SndObj.h"

CPlayerNPC::CPlayerNPC()
{
	m_ePlayerType = PLAYER_NPC; // Player Type ... Base, NPC, OTher, MySelf
	m_vPosFromServer = m_Chr.Pos(); // Location received from the server.
}

CPlayerNPC::~CPlayerNPC()
{
}

void CPlayerNPC::Tick()
{
	if(m_pShapeExtraRef) // If the object...
	{
		CPlayerBase::Tick();
		return;
	}

	if(m_fTimeAfterDeath > 0) // If you have to die or are dead...
	{
		if(m_fTimeAfterDeath > 3.0f)
			this->Action(PSA_DYING, false); // Dies after 5 seconds.
		CPlayerBase::Tick();  // Rotation, handling specified animation ticks and color assignments.. etc..
		return;
	}

	__Vector3 vPos = m_Chr.Pos();
	if(	m_vPosFromServer.x != vPos.x || m_vPosFromServer.z != vPos.z ) // A little more to go.
	{
		if(m_fMoveSpeedPerSec == 0)
		{
			__Vector3 vT1(m_vPosFromServer.x, 0, m_vPosFromServer.z), vT2(vPos.x, 0, vPos.z);
			m_fMoveSpeedPerSec = (vT2 - vT1).Magnitude();
		}

		__Vector3 vOffset = m_vPosFromServer - vPos; vOffset.y = 0;
		__Vector3 vDir = vOffset; vDir.Normalize();  // direction..

		const float fSpeedAbsolute = (m_fMoveSpeedPerSec > 0) ? m_fMoveSpeedPerSec : -m_fMoveSpeedPerSec; // absolute value of speed
		const float fDist = vOffset.Magnitude(); // distance
		if(fDist < fSpeedAbsolute * CN3Base::s_fSecPerFrm) // If you&#39;re close to moving...
		{
			vPos.x = m_vPosFromServer.x; // Fix the position...
			vPos.z = m_vPosFromServer.z; // Fix the position...
			// m_fMoveSpeedPerSec = 0; // set moving speed to 0!
			this->ActionMove(PSM_STOP); // Moving motion processing...
		}
		else 
		{
			const float fYaw = (m_fMoveSpeedPerSec < 0) ? ::_Yaw2D(-vDir.x, -vDir.z) : ::_Yaw2D(vDir.x, vDir.z); // Calculate the direction...
			this->RotateTo(fYaw, false); // Turn in the direction of...

			e_StateMove eMove = PSM_STOP; // movement...
			// The player&#39;s walking speed is the standard, the rest are inversely proportional to their size.
			const float fStandWalk = ((PLAYER_OTHER == m_ePlayerType) ? (MOVE_SPEED_WHEN_WALK * 2.0f) : (MOVE_SPEED_WHEN_WALK * m_Chr.Radius() * 2.0f));
			if(m_fMoveSpeedPerSec < 0) eMove = PSM_WALK_BACKWARD; // walking backwards...
			else if(m_fMoveSpeedPerSec < fStandWalk) eMove = PSM_WALK; // walking forward...
			else eMove = PSM_RUN; // if (fDN &gt; 5.0f) // If the distance to the next location is greater than a certain distance, run.
			this->ActionMove(eMove); // Moving motion processing...

			vPos += vDir * (fSpeedAbsolute * s_fSecPerFrm); // movement..
		}

		const float fYTerrain = ACT_WORLD->GetHeightWithTerrain(vPos.x, vPos.z); // The height of the ground...
		const float fYMesh = ACT_WORLD->GetHeightNearstPosWithShape(vPos, 1.0f); // The height value of the collision check object..
		if(fYMesh != -FLT_MAX && fYMesh > fYTerrain && fYMesh < m_fYNext + 1.0f) m_fYNext = fYMesh; // If it is an object that can be climbed and the height value is higher than the ground.
		else m_fYNext = fYTerrain;
		this->PositionSet(vPos, false); // Location final application..
	}

	if(PSA_ATTACK == m_eState || m_iSkillStep != 0) // If you are attacking or using a skill...
	{
		CPlayerBase* pTarget = this->TargetPointerCheck(false);
		CPlayerBase::ProcessAttack(pTarget); // Routine handling of attacks. It only handles animation settings and collisions, but does not process packets.
	}

	CPlayerBase::Tick(); // rotation. It handles movement, animation ticks, state changes, etc.
}

void CPlayerNPC::MoveTo(float fPosX, float fPosY, float fPosZ, float fSpeed, int iMoveMode)
{
	m_vPosFromServer.Set(fPosX, fPosY, fPosZ);
	if(m_pShapeExtraRef) return; // If it is an object type, it cannot be moved.

	// iMoveMode: Current moving state.. 0 - Stop 1 Start moving 2 - Continuous movement once per second..
	if(0 == iMoveMode) 
	{
	}
	else if(iMoveMode) // start moving.. // keep moving
	{
		m_fMoveSpeedPerSec = fSpeed;
		__Vector3 vPos = m_Chr.Pos(); vPos.y = 0;
		const __Vector3 vPosS(fPosX, 0, fPosZ);
	
		if(fSpeed) m_fMoveSpeedPerSec *= ((vPosS - vPos).Magnitude() / (fSpeed * PACKET_INTERVAL_MOVE)) * 0.85f; // Speed interpolation.. This is because of synchronization.. The reason for reducing it slightly is to prevent hesitation..
		else m_fMoveSpeedPerSec = ((vPosS - vPos).Magnitude() / (fSpeed * PACKET_INTERVAL_MOVE)) * 0.85f; // Speed interpolation.. This is because of synchronization.. The reason for reducing it slightly is to prevent hesitation..
		if(fSpeed < 0) m_fMoveSpeedPerSec *= -1.0f; // go back..
	}
	else
	{
		// __ASSERT(0, "Invalid Move Mode");
	}
}
