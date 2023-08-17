#include "pch.h"
#include "N3WorldManager.h"

#include "N3TerrainManager.h"
#include "DungeonManager.h"

#include "APISocket.h"
#include "PacketDef.h"

#include "GameProcedure.h"
#include "PlayerMySelf.h"

#include "GameEng.h"

#include "N3SndObj.h"
#include "N3SndObjStream.h"
#include "N3SndMgr.h"

CN3WorldManager::CN3WorldManager()
{
	m_pActiveWorld = NULL;
	m_bIndoor = true;
}

CN3WorldManager::~CN3WorldManager()
{
	if (m_pActiveWorld)
		delete m_pActiveWorld;
	m_pActiveWorld = NULL;
}

void CN3WorldManager::InitWorld(int iZoneID, const __Vector3& vPosPlayer)
{
	CLogWriter::Write("CN3WorldManager::InitWorld Pre delete"); // TmpLog_11_22

	if (m_pActiveWorld)
		delete m_pActiveWorld;

	// Zone 선택..
	if (iZoneID != 51)	// N3Terrain..
	{
		CLogWriter::Write("CN3WorldManager::InitWorld Pre new Terrain ZoneID(%d)", iZoneID); // TmpLog_11_22

		m_pActiveWorld = new CN3TerrainManager();	
		m_bIndoor = false;
	}
	else
	{
		CLogWriter::Write("CN3WorldManager::InitWorld Pre new Dungeon ZoneID(%d)", iZoneID); // TmpLog_11_22

		m_pActiveWorld = new CDungeonManager();
		m_bIndoor = true;
	}

	// Zone 초기화..
	m_pActiveWorld->InitWorld(iZoneID, vPosPlayer);
}

void CN3WorldManager::Tick()
{
	if (m_pActiveWorld)
		m_pActiveWorld->Tick();
}







