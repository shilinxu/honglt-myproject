//
// Config-win32.h
//

#if !defined (CONFIG_WIN32_H__INCLUDED_)
#define CONFIG_WIN32_H__INCLUDED_

#if !defined (WIN32) && !defined (_WIN32_WCE)
# error Please define WIN32 in your project settings.
#endif

#if defined (_MSC_VER)
# if _MSC_VER >= 1400		// msvc 8
# elif _MSC_VER >= 1300		// msvc 7
# elif _MSC_VER >= 1200		// msvc 6
# else
# error This version of Microsoft Visual C++ not supported.
# endif

# define WINVER			0x0500		// Windows Version
# define _WIN32_WINDOWS	0x0500		// Windows Version
# define _WIN32_WINNT	0x0500		// NT Version
# define _WIN32_IE		0x0500		// IE Version

# include <windows.h>				// Windows standard
# include <tchar.h>					// ANSI v.s. UNICODE abstraction

# if !defined (_MT)
#  error You must link against multi-threaded libraries when using AKM (check your project settings)
# endif

#else
# error Compiler is not supported
#endif // _MSC_VER

//
// Utility
//

#include <iostream>
using namespace std;

#if defined (AKM_EXPORT) && !defined (AKM_Export)
#define AKM_Export __declspec (dllexport)
#else
#undef AKM_Import
#define AKM_Import __declspec (dllexport)
#define AKM_Export AKM_Import
#endif // AKM_EXPORT && !AKM_Export

#if defined (AKM_EXPORT) && !defined (_WINDLL)
#define _WINDLL
#endif // AKM_EXPORT && !_WINDLL

#endif // !defined (CONFIG_H__INCLUDED_)
