// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN

#define _CRT_SECURE_NO_WARNINGS

#include "My_3DStruct.h"

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

#if defined NDEBUG
#define TRACE( format, ... )
#define DXTRACE_ERR( format, ... )
#else
#define TRACE( format, ... )   printf( "%s::%s(%d)" format, __FILE__, __FUNCTION__,  __LINE__, __VA_ARGS__ )
#define DXTRACE_ERR( format, ... )   printf( "%s::%s(%d)" format, __FILE__, __FUNCTION__,  __LINE__, __VA_ARGS__ )
#endif

#endif //PCH_H
