#pragma once

// // Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>

// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#pragma comment(lib, "NoahEngine.lib")

#include "../../NoahEngine/Base/pch.h"

#include "../StringIds.h"

#include <sqlcipher/sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>
