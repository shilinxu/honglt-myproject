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
#include <string.h>

#include "alloca.h"

#pragma pack(push,8)

#ifdef _CONVERSION_USES_THREAD_LOCALE
	#ifndef _DEBUG
		#define USES_CONVERSION int _convert; _convert; /*UINT _acp = GetACP(); _acp; */LPCWSTR _lpw; _lpw; LPCSTR _lpa; _lpa
	#else
		#define USES_CONVERSION int _convert = 0; _convert; /*UINT _acp = GetACP(); _acp; */LPCWSTR _lpw = NULL; _lpw; LPCSTR _lpa = NULL; _lpa
	#endif
#else
	#ifndef _DEBUG
		#define USES_CONVERSION int _convert; _convert; /*UINT _acp = CP_ACP; _acp; */LPCWSTR _lpw; _lpw; LPCSTR _lpa; _lpa
	#else
		#define USES_CONVERSION int _convert = 0; _convert; /*UINT _acp = CP_ACP; _acp; */LPCWSTR _lpw = NULL; _lpw; LPCSTR _lpa = NULL; _lpa
	#endif
#endif

//
// UNICODE<>ANSI helpers
//

inline LPWSTR A2WHelper(LPWSTR lpw, LPCSTR lpa, int nChars/*, UINT acp*/)
{
    ASSERT( lpa != NULL );
    ASSERT( lpw != NULL );
    lpw[0] = '\0';

    for( size_t i = 0; i < ::strlen(lpa)+1; i++ )
    {
        CHAR* t = const_cast<CHAR*> (lpa);
        lpw[i] = static_cast<WCHAR> (*((BYTE*)(t + i)));
    }
    return lpw;
}

inline LPSTR W2AHelper(LPSTR lpa, LPCWSTR lpw, int nChars/*, UINT acp*/)
{
    ASSERT( lpa != NULL );
    ASSERT( lpw != NULL );
    lpa[0] = '\0';

    for( size_t i = 0; i < ::wcslen(lpw)+1; i++ )
    {
        WCHAR* wt = const_cast<WCHAR*> (lpw);
        lpa[i] = static_cast<CHAR> (*(wt + i));
    }
    return lpa;
}

#ifdef _CONVERSION_USES_THREAD_LOCALE
    #define A2W(lpa) (\
            ((_lpa = lpa) == NULL) ? NULL : (\
                _convert = (strlen(_lpa)+1),\
                A2WHelper((LPWSTR) _alloca(_convert*2), _lpa, _convert, _acp)))
#else
    #define A2W(lpa) (\
            ((_lpa = lpa) == NULL) ? NULL : (\
                _convert = (wcslen(_lpa)+1),\
                A2WHelper((LPWSTR) _alloca(_convert*2), _lpa, _convert)))
#endif

#ifdef _CONVERSION_USES_THREAD_LOCALE
    #define W2A(lpw) (\
            ((_lpw = lpw) == NULL) ? NULL : (\
                _convert = (wcslen(_lpw)+1)*2,\
                W2AHelper((LPSTR) _alloca(_convert), _lpw, _convert, _acp)))
#else
    #define W2A(lpw) (\
            ((_lpw = lpw) == NULL) ? NULL : (\
                _convert = (wcslen(_lpw)+1)*2,\
                W2AHelper((LPSTR) alloca(_convert), _lpw, _convert)))
#endif

#define A2CW(lpa) ((PCWSTR)A2W(lpa))
#define W2CA(lpw) ((LPCSTR)W2A(lpw))

#pragma pack(pop)

template<typename TYPE> inline int _tcslen(const TYPE* pszStr)
{
    return ( pszStr ) ? std::char_traits<TYPE>::length(pszStr) : 0;
}

inline void Assign(std::string& strDst, LPCSTR pszSrc)
{
    if ( ! pszSrc )
    {
        strDst.erase();
    }
    else if ( pszSrc >= strDst.c_str() && pszSrc <= strDst.c_str() + strDst.size() )
    {
        strDst = strDst.substr(static_cast<std::string::size_type>(pszSrc - strDst.c_str()));
    }
    else
    {
        strDst.assign( pszSrc );
    }
}

inline void Assign(std::string& strDst, LPCWSTR pszSrc)
{
    USES_CONVERSION;
    strDst.assign(pszSrc ? W2CA(pszSrc) : "");
}

//
// class CStringBase
//

template <typename TYPE>
class CStringBase : public std::basic_string<TYPE>
{
// Construction
public:
    CStringBase();
    CStringBase(LPCSTR pszSrc);
    CStringBase(LPCWSTR pszSrc);

// Operations
public:
    CStringBase<TYPE>& operator=(LPCSTR pszSrc);
    CStringBase<TYPE>& operator=(LPCWSTR pszSrc);
//    int         GetLength() const;
};

//////////////////////////////////////////////////////////////////////
// CStringBase construction

template <typename TYPE>
CStringBase<TYPE>::CStringBase()
{
}

template <typename TYPE>
CStringBase<TYPE>::CStringBase(LPCSTR pszSrc)
{
    if ( pszSrc ) *this = pszSrc;
}

template <typename TYPE>
CStringBase<TYPE>::CStringBase(LPCWSTR pszSrc)
{
    if ( pszSrc ) this->append( pszSrc );
}

//////////////////////////////////////////////////////////////////////
// CStringBase attributes

template <typename TYPE>
CStringBase<TYPE>& CStringBase<TYPE>::operator=(LPCSTR pszSrc)
{
    ::Assign(*this, pszSrc); return *this;
}

template <typename TYPE>
CStringBase<TYPE>& CStringBase<TYPE>::operator=(LPCWSTR pszSrc)
{
    ::Assign(*this, pszSrc); return *this;
}

//
// Configuration
//

typedef CStringBase<CHAR> CString;

#endif // _CSTRING_H__INCLUDED_
