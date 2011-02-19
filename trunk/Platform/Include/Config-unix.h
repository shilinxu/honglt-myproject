//
// Config-unix.h
//

#if !defined (CONFIG_UNIX_H__INCLUDED_)
#define CONFIG_UNIX_H__INCLUDED_

//
// Configuration
//

#include <stdlib.h>

//
// Definitions
//

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
#endif

#ifndef TRUE
#define TRUE    1
#endif

/* Minimum and maximum macros */

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))

#define MoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define CopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define FillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

//
// 64-bit type
//

typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef long long __int64;
typedef unsigned long long QWORD;

//
// Debug assert
//

#ifndef ASSERT
    #include <assert.h>
    #define ASSERT(f) assert((f))
#endif

#ifndef VERIFY
    #define VERIFY(f)
#endif

#endif // !defined (CONFIG_UNIX_H__INCLUDED_)

