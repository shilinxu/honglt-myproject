//
// dllmodul.cpp
//

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

// global data



// The following symbol used to force inclusion of this module for _USRDLL

#ifdef _X86_

extern "C" { int _afxForceUSRDLL; }

#else

extern "C" { int __afxForceUSRDLL; }

#endif
