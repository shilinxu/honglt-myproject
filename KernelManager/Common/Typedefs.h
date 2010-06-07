//
// Typedefs.h
//

#if !defined (TYPEDEFS_H__INCLUDED_)
#define TYPEDEFS_H__INCLUDED_

//
// 64-bit type
//

typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef unsigned __int64 QWORD;

//
// Tristate type
//

typedef int TRISTATE;

#define TS_UNKNOWN	0
#define TS_FALSE	1
#define TS_TRUE		2

#endif // !defined (TYPEDEFS_H__INCLUDED_)
