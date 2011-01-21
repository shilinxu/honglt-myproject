//
// Config-unix.h
//

#if !defined (CONFIG_UNIX_H__INCLUDED_)
#define CONFIG_UNIX_H__INCLUDED_

//
// Definitions
//

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef unsigned long DWORD;

typedef char CHAR;
typedef wchar_t WCHAR;

typedef /* [string] */ CHAR *LPSTR;
typedef /* [string] */ const CHAR *LPCSTR;

typedef /* [string] */ WCHAR *LPWSTR;
typedef /* [string] */ const WCHAR *LPCWSTR;

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

