//
// CString.cpp
//

#include "ComService.h"
#include "CString.h"

///////////////////////////////////////////////////////////////////////////////
// CString construction

CString::CString()
{
}

CString::CString(CHAR pCh)
{
    *this += pCh;
}

CString::CString(LPCSTR pszSrc)
{
    if ( pszSrc ) this->append( pszSrc );
}

CString::CString(LPCSTR pszSrc, int nLength)
{
    if ( pszSrc ) this->append( pszSrc, nLength );
}

CString::CString(const CString& strSrc)
{
    this->append( strSrc );
}

CString::~CString()
{
}

