//
// Typedefs.h
//

#if !defined(TYPEDEFS_H__INCLUDED_)
#define TYPEDEFS_H__INCLUDED_

//
// 64-bit type
//

typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef unsigned __int64 QWORD;

//
// string type
//

#include <string>
typedef CString std::string;

//
// Tristate type
//

typedef int TRISTATE;

#define TS_UNKNOWN	0
#define TS_FALSE	1
#define TS_TRUE		2

// State type

enum
{
	State_InService,
	State_OutOfService,
	State_EmergencyOnly,
	State_PowerOff,
};

//
// Event type
//

const enum
{
	EventLevel_Info				= 0,
	EventLevel_Normal			= 1,
	EventLevel_Warning			= 2,
	EventLevel_Exception		= 3,
	EventLevel_Debug			= 4,
	EventLevel_Ultimate			= 5,
};

const enum
{
	EventType_System			= 0,
	EventType_Platform			= 1,
};

const LPCTSTR TTraceName[EventLevel_Ultimate] =
{
	TEXT("系统"),
	TEXT("常规"),
	TEXT("警告"),
	TEXT("异常"),
	TEXT("调试"),
};

const COLORREF RTraceColour[EventLevel_Ultimate] =
{
	RGB( 0, 0, 0 ),
	RGB( 0, 0, 127 ),
	RGB( 0, 0, 255 ),
	RGB( 255, 0, 0 ),
	RGB( 192, 192, 192 ),
};

//
// Message
//

enum
{
	KM_MESSAGE_0				= ( WM_USER + 1000 ),
	KM_MESSAGE_LOG				= ( KM_MESSAGE_0 + 1 ),

	KM_MESSAGE_CONNECT			= ( KM_MESSAGE_0 + 2 ),
};

//
// Error type
//

enum
{
	E_SYS_NoError	= 0x0,

	E_SYS_TimeOut	= 0x4000,
	E_FAIL_Connect	= 0x4001,
};

#endif // !defined(TYPEDEFS_H__INCLUDED_)
