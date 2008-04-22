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

# if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
# if defined(ARTOOLKIT_BUILD_LIB)
#      define ARTOOLKIT_API __declspec(dllexport)
#   else
#   if defined(ARTOOLKIT_USE_DLL)
#      define ARTOOLKIT_API __declspec(dllimport)
#   else
#      define ARTOOLKIT_API extern
#   endif 
# endif
#else
#    define ARTOOLKIT_API
#endif

#endif
