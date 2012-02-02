//
// stdafx.h
//

#pragma once

#pragma warning(disable: 4267 4251)
#pragma warning(disable: 4018 4114 4146 4245)

//
// Configuration
//

#define WINVER			0x0500	// Windows Version
#define _WIN32_WINDOWS	0x0500	// Windows Version
#define _WIN32_WINNT	0x0500	// NT Version
#define _WIN32_IE		0x0500	// IE Version
#define _WIN32_DCOM				// DCOM
#define _RICHEDIT_VER	0x0200

#define _CRT_SECURE_NO_WARNINGS
// #define _CRT_NON_CONFORMING_SWPRINTFS

//
// WIN32
//

#include <atlbase.h>
#include <atlcoll.h>
#include <atlstr.h>

#define _WTL_NO_CSTRING

#include <wtl/atlapp.h>
#include <wtl/atlframe.h>
#include <wtl/atlctrls.h>
// #include <wtl/atlctrlx.h>
#include <wtl/atldlgs.h>
// #include <wtl/atlcrack.h>
#include <wtl/atlmisc.h>

//
// Utility
//

#include "ComService.h"
