#ifndef sysconfig_h
#define sysconfig_h

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef _WIN32
#include <sys/timeb.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif 


#if defined(_MS_VER)
#if defined(ARTOOLKIT_BUILD_DLL) 
	#define ARTOOLKIT_API extern "C" __declspec(dllexport)
#else
	#define ARTOOLKIT_API extern "C" __declspec(dllimport)
#endif
#else
	#define ARTOOLKIT_API		extern
#endif


#endif
