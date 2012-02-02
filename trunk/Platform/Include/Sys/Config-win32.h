//
// Config-win32.h
//

#if !defined (CONFIG_WIN32_H__INCLUDED_)
#define CONFIG_WIN32_H__INCLUDED_

//
// Compiler
//

#if !defined (WIN32) && !defined (_WIN32_WCE)
#error Please define WIN32 in your project settings.
#endif

#if !defined (_MSC_VER)
#error Compiler is not supported
#endif // _MSC_VER

#if !defined (_MT)
#error You must link against multi-threaded libraries when using AKM (check your project settings)
#endif

//
// Configuration
//

#define WINVER			0x0500		// Windows Version
#define _WIN32_WINDOWS	0x0500		// Windows Version
#define _WIN32_WINNT	0x0500		// NT Version
#define _WIN32_IE		0x0500		// IE Version

//
// 64-bit type
//

typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

//
// Utility
//

#define Platform_Proper_Export  __declspec(dllexport)
#define Platform_Proper_Import  __declspec(dllimport)

//
// Debug assert
//

#ifndef ASSERT
#include <assert.h>
#define ASSERT(f) assert((f))
#endif

#endif // !defined (CONFIG_WIN32_H__INCLUDED_)
