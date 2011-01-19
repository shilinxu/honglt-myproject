//
// ComService.h
//

#ifndef COM_SERVICE_H__INCLUDED_
#define COM_SERVICE_H__INCLUDED_

//
// Compiler
//

#if !defined (WIN32) && !defined (_WIN32_WCE)
#include "CString.h"
#endif

//
// Configuration
//

#if defined(WIN32)

#ifdef COM_SERVICE_Export
#define COM_SERVICE_CLASS       Platform_Proper_Export
#else
#define COM_SERVICE_CLASS       Platform_Proper_Import
#endif // COM_SERVICE_Export

#else

#define COM_SERVICE_CLASS
#endif // WIN32

class COM_SERVICE_CLASS CString : public std::string
{
// Construction
public:
    CString();
};

#enidf // _CSTRING_H__INCLUDED_

