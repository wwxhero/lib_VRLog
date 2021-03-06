// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_VRLOG_H
#define PCH_VRLOG_H

// add headers that you want to pre-compile here
#if defined _WINDOWS
#	define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#	include <windows.h>
#endif

// DLL export C functions:
//		https://docs.microsoft.com/en-us/cpp/build/exporting-from-a-dll-using-declspec-dllexport?view=msvc-160
// calling convention:
//		https://en.wikipedia.org/wiki/X86_calling_conventions#Microsoft_x64_calling_convention
#ifdef LIB_VRLOG_EXPORTS
#define VRLOGLIB(rtype, func)\
	__declspec(dllexport) rtype __stdcall func
#else
#define VRLOGLIB(rtype, func)\
	__declspec(dllimport) rtype __stdcall func
#endif



#ifdef _DEBUG

typedef struct _HLOG
{
	void* p;
} HLOG;

typedef struct _HLOG_i
{
	void* p;
} HLOG_i;

#else

typedef void* HLOG;
typedef void* HLOG_i;

#endif



#endif //PCH_H
