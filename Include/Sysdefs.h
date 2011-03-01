//
// Sysdefs.h
//

#ifndef _INC_SYSDEFS
#define _INC_SYSDEFS

//
// Configuration
//

#include <stdlib.h>

//
// Definitions
//

#ifndef WIN32

typedef bool BOOL;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef unsigned long DWORD;

typedef char CHAR;
typedef wchar_t WCHAR;
typedef unsigned char UCHAR;

typedef /* [string] */ CHAR *LPSTR;
typedef /* [string] */ const CHAR *LPCSTR;

typedef /* [string] */ WCHAR *LPWSTR;
typedef /* [string] */ const WCHAR *LPCWSTR;

/* Boolean macros */

#ifndef FALSE
#define FALSE   0
#endif // FALSE

#ifndef TRUE
#define TRUE    1
#endif // FALSE

#endif // WIN32

/* string macros */

#ifndef _INC_TCHAR

#ifdef UNICODE
#define TCHAR           WCHAR
#define LPTSTR          LPWSTR
#define LPCTSTR         LPCWSTR
#else
#define TCHAR           CHAR
#define LPTSTR          LPSTR
#define LPCTSTR         LPCSTR
#endif // UNICODE

#ifdef UNICODE
#define lstrlen         wcslen
#define _stprintf       swprintf
#define _sntprintf      snwprintf

#define __TEXT(quote)   L##quote

#else
#define lstrlen         strlen
#define _stprintf       sprintf
#define _sntprintf      snprintf

#define __TEXT(quote)   quote
#endif // UNICODE

#define TEXT(quote) __TEXT(quote)

#endif // _INC_TCHAR

/* Minimum and maximum macros */

#ifndef WIN32

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))

#define MoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define CopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define FillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

#endif // WIN32

//
// 64-bit type
//

typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef unsigned long long QWORD;

#ifndef WIN32
typedef long long __int64;
#endif // WIN32

#endif // _INC_SYSDEFS

