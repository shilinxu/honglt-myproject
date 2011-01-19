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

class CString : public std::string
{
// Construction
public:
    CString();
};

#enidf // _CSTRING_H__INCLUDED_
