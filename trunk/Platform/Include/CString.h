//
// CString.h
//

#ifndef _CSTRING_H__INCLUDED_
#define _CSTRING_H__INCLUDED_

//
// Compiler
//

#if defined (WIN32) || defined (_WIN32_WCE)
#error CString custom not supported.
#endif

//
// Definitions
//

#include <string>

#ifndef COM_SERVICE_H__INCLUDED_
#include "ComService.h"
#endif // COM_SERVICE_H__INCLUDED_

class COM_SERVICE_CLASS CString : public std::string
{
// Construction
public:
    CString();
    CString(CHAR pCh);
    CString(LPCSTR pszSrc);
    CString(LPCSTR pszSrc, int nLength);
};

#endif // _CSTRING_H__INCLUDED_
