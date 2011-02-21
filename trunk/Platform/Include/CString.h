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

inline void Assadd(std::string& strDst, LPCSTR pszSrc)
{
    if ( pszSrc ) strDst.append( pszSrc );
}

inline void Assadd(std::string& strDst, LPCWSTR pszSrc)
{
    USES_CONVERSION;
    if ( pszSrc ) strDst.append( W2CA(pszSrc) );
}

//
// struct NotSpace
//

template <typename TYPE>
struct NotSpace : public std::unary_function<TYPE, bool>
{
    const std::locale& loc;
    NotSpace(const std::locale&locArd) : loc(locArd) {}
};

//
// class CStringT
//

template <typename TYPE>
class CStringT : public std::basic_string<TYPE>
{
// Definitions
private:
    typedef typename std::basic_string<TYPE>    BASE_T;
//    typedef CStringT<TYPE>                      StringT;
    typedef typename BASE_T::pointer            PSTR_T;
    typedef typename BASE_T::const_pointer      PCSTR_T;
    typedef typename BASE_T::iterator           PITER_T;
    typedef typename BASE_T::const_iterator     PCITER_T;
    typedef typename BASE_T::reverse_iterator   PRITER_T;
    typedef typename BASE_T::size_type          SIZE_T;

// Construction
public:
    CStringT();
    CStringT(LPCSTR pszSrc);
    CStringT(LPCWSTR pszSrc);
    CStringT(PCSTR_T pszSrc, SIZE_T nLength);
//    CStringT(const StringT& strSrc);
//    CString(const std::string& strSrc);
//    CString(const std::wstring& strSrc);

// Operations
public:
    TYPE            GetAt(int nIndex) const;
    TYPE*           GetBuffer(int nMinLength);
    void            SetAt(int nIndex, TYPE ch);
    TYPE*           SetBuffer(int nLength);
    //CStringT<TYPE>& ToUpper();
public:
    void            Empty() { BASE_T::erase(); }
    bool            IsEmpty() const { return BASE_T::empty(); }
    int             GetLength() const { return static_cast<int>(BASE_T::length()); }
public:
    CStringT<TYPE>  Mid(int nFirst) const;
    CStringT<TYPE>  Mid(int nFirst, int nCount) const;
    CStringT<TYPE>  Left(int nCount) const;
    CStringT<TYPE>  Right(int nCount) const;
    int             Delete(int nIndex, int nCount = 1);
    int             Insert(int nIndex, TYPE ch);
    int             Insert(int nIndex, PCSTR_T pszSrc);
public:
    int             Find(TYPE ch) const;
    int             Find(TYPE ch, int nStart) const;
    int             Find(PCSTR_T pszSrc) const;
    int             Find(PCSTR_T pszSrc, int nStart) const;
    int             FindOneOf(PCSTR_T szCharSet) const;
    int             ReverseFind(TYPE ch) const;
    int             ReverseFind(PCSTR_T pszSrc, SIZE_T pos=BASE_T::npos) const;
    int             Remove(TYPE ch);
    void            ReleaseBuffer(int nNewLength = -1);
    void            FreeExtra();
public:
    CStringT<TYPE>& TrimLeft();
    CStringT<TYPE>& TrimLeft(TYPE chTarget);
    CStringT<TYPE>& TrimLeft(PCSTR_T pszTargets);
    CStringT<TYPE>& TrimRight();
    CStringT<TYPE>& TrimRight(TYPE chTarget);
    CStringT<TYPE>& TrimRight(PCSTR_T pszTargets);
    void            MakeLower() { BASE_T::_mbsupr( (UCHAR*)GetBuffer() ); }
    void            MakeUpper() { BASE_T::_mbsupr( (UCHAR*)GetBuffer() ); }
    void            MakeReverse() { BASE_T::reverse(BASE_T::begin(), BASE_T::end()); }
    CStringT<TYPE>  SpanExcluding(PCSTR_T szCharSet) const { return Left(find_first_of(szCharSet)); }
    CStringT<TYPE>  SpanIncluding(PCSTR_T szCharSet) const { return Left(find_first_not_of(szCharSet)); }
public:
    CStringT<TYPE>& operator=(LPCSTR pszSrc);
    CStringT<TYPE>& operator=(LPCWSTR pszSrc);
//    CStringT<TYPE>& operator=(const std::string& pszSrc);
//    CStringT<TYPE>& operator=(const std::wstring& pszSrc);
    CStringT<TYPE>& operator=(const CStringT<TYPE>& pszSrc);
    CStringT<TYPE>& operator+=(LPCSTR pszSrc);
    CStringT<TYPE>& operator+=(LPCWSTR pszSrc);
//    CStringT<TYPE>& operator+=(const std::string& pszSrc);
    CStringT<TYPE>& operator+=(const CStringT<TYPE>& pszSrc);
    const TYPE      operator[](int nIndex) const;
    operator const  TYPE*() const { return BASE_T::c_str(); }
public:
    friend CStringT<TYPE> operator+(const CStringT<TYPE>& strSrc, PCSTR_T pszAdd)
    {
        return CStringT<TYPE>(strSrc) + CStringT<TYPE>(pszAdd);
    }
//    friend CStringT<TYPE> operator+(const CStringT<TYPE>& strSrc, LPCWSTR pszAdd)
//    {
//        return CStringT<TYPE>(strSrc) + CStringT<TYPE>(pszAdd);
//    }
    friend CStringT<TYPE> operator+(const CStringT<TYPE>& strSrc, const CStringT<TYPE>& strAdd)
    {
        CStringT<TYPE> strRet(strSrc);
        strRet.append(strAdd); return strRet;
    }
};

//////////////////////////////////////////////////////////////////////
// CStringT construction

template <typename TYPE>
CStringT<TYPE>::CStringT()
{
}

template <typename TYPE>
CStringT<TYPE>::CStringT(LPCSTR pszSrc)
{
    if ( pszSrc ) *this = pszSrc;
}

template <typename TYPE>
CStringT<TYPE>::CStringT(PCSTR_T pszSrc, SIZE_T nLength) : BASE_T(pszSrc, nLength)
{
}

template <typename TYPE>
CStringT<TYPE>::CStringT(LPCWSTR pszSrc)
{
    if ( pszSrc ) *this = pszSrc;
}
//
//template <typename TYPE>
//CStringT<TYPE>::CStringT(const StringT& strSrc) : BASE_T(strSrc)
//{
//}
//
//template <typename TYPE>
//CStringT<TYPE>::CStringT(const std::string& strSrc)
//{
//    *this =
//}


//////////////////////////////////////////////////////////////////////
// CStringT attributes
//
//template <typename TYPE>
//int CStringT<TYPE>::GetLength() const
//{
//    return static_cast<int>(BASE_T::length());
//}

template <typename TYPE>
TYPE CStringT<TYPE>::GetAt(int nIndex) const
{
    return at(static_cast<SIZE_T>(nIndex));
}

template <typename TYPE>
TYPE* CStringT<TYPE>::GetBuffer(int nMinLength)
{
    if ( static_cast<int>(BASE_T::size()) < nMinLength )
        resize( static_cast<SIZE_T>(nMinLength) );
    return BASE_T::empty() ? const_cast<TYPE*>(BASE_T::data()) : &(BASE_T::at(0));
}

template <typename TYPE>
void CStringT<TYPE>::SetAt(int nIndex, TYPE ch)
{
    ASSERT(BASE_T::size() > static_cast<SIZE_T>(nIndex));
    at(static_cast<SIZE_T>(nIndex)) = ch;
}

template <typename TYPE>
TYPE* CStringT<TYPE>::SetBuffer(int nLength)
{
    ASSERT( nLength >= 0 );
    if ( BASE_T::capacity() < 1 && ! nLength ) BASE_T::resize(1);

    resize(static_cast<SIZE_T>(nLength) );
    return const_cast<TYPE*>(BASE_T::data());
}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::operator=(LPCSTR pszSrc)
{
    ::Assign(*this, pszSrc); return *this;
}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::operator=(LPCWSTR pszSrc)
{
    ::Assign(*this, pszSrc); return *this;
}
//
//template <typename TYPE>
//CStringT<TYPE>& CStringT<TYPE>::operator=(const std::string& pszSrc)
//{
//    ::Assign(*this, pszSrc); return *this;
//}
//
//template <typename TYPE>
//CStringT<TYPE>& CStringT<TYPE>::operator=(const std::wstring& pszSrc)
//{
//    ::Assign(*this, pszSrc); return *this;
//}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::operator=(const CStringT<TYPE>& pszSrc)
{
    ::Assign(*this, pszSrc); return *this;
}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::operator+=(LPCSTR pszSrc)
{
    ::Assadd(*this, pszSrc); return *this;
}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::operator+=(LPCWSTR pszSrc)
{
    ::Assadd(*this, pszSrc); return *this;
}
//
//template <typename TYPE>
//CStringT<TYPE>& CStringT<TYPE>::operator+=(const std::string& pszSrc)
//{
//    ::Assign(*this, pszSrc); return *this;
//}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::operator+=(const CStringT<TYPE>& pszSrc)
{
    ::Assign(*this, pszSrc); return *this;
}

template <typename TYPE>
const TYPE CStringT<TYPE>::operator[](int nIndex) const
{
    return BASE_T::operator[](static_cast<SIZE_T>(nIndex));
}

//////////////////////////////////////////////////////////////////////
// CStringT operations

template <typename TYPE>
int CStringT<TYPE>::Insert(int nIndex, TYPE ch)
{
    if ( static_cast<SIZE_T>(nIndex) > BASE_T::szie() - 1 ) append(1, ch);
    else insert(static_cast<SIZE_T>(nIndex), 1, ch);

    return GetLength();
}

template <typename TYPE>
int CStringT<TYPE>::Insert(int nIndex, PCSTR_T pszSrc)
{
    if ( nIndex >= BASE_T::size() ) append(pszSrc, std::char_traits<TYPE>::length(pszSrc));
    else insert(static_cast<SIZE_T>(nIndex), pszSrc);

    return GetLength();
}

template <typename TYPE>
int CStringT<TYPE>::Delete(int nIndex, int nCount)
{
    if ( nIndex < GetLength() ) erase(static_cast<SIZE_T>(nIndex), static_cast<SIZE_T>(nCount));
    return GetLength();
}

template <typename TYPE>
CStringT<TYPE> CStringT<TYPE>::Mid(int nFirst) const
{
    return substr(static_cast<SIZE_T>(nFirst));
}

template <typename TYPE>
CStringT<TYPE> CStringT<TYPE>::Mid(int nFirst, int nCount) const
{
    return substr(static_cast<SIZE_T>(nFirst), static_cast<SIZE_T>(nCount));
}

template <typename TYPE>
CStringT<TYPE> CStringT<TYPE>::Left(int nCount) const
{
    return substr(0, static_cast<SIZE_T>(nCount)).c_str();
}

template <typename TYPE>
CStringT<TYPE> CStringT<TYPE>::Right(int nCount) const
{
    nCount = __min(nCount, static_cast<int>(BASE_T::size()));
    return substr(BASE_T::size() - static_cast<SIZE_T>(nCount));
}

//template <typename TYPE>
//CStringT<TYPE>& CStringT<TYPE>::MakeUpper()
//{
//    BASE_T::_mbsupr( (UCHAR*)GetBuffer() ); return *this;
//}

template <typename TYPE>
int CStringT<TYPE>::Find(TYPE ch) const
{
    SIZE_T nIndex = find_first_of( ch );
    return static_cast<int>(BASE_T::npos == nIndex ? -1 : nIndex);
}

template <typename TYPE>
int CStringT<TYPE>::Find(TYPE ch, int nStart) const
{
    SIZE_T nIndex = find_first_of(ch, static_cast<SIZE_T>(nStart));
    return static_cast<int>(BASE_T::npos == nIndex ? -1 : nIndex);
}

template <typename TYPE>
int CStringT<TYPE>::Find(PCSTR_T pszSrc) const
{
    SIZE_T nIndex = find( pszSrc );
    return static_cast<int>(BASE_T::npos == nIndex ? -1 : nIndex);
}

template <typename TYPE>
int CStringT<TYPE>::Find(PCSTR_T pszSrc, int nStart) const
{
    SIZE_T nIndex = find(pszSrc, static_cast<SIZE_T>(nStart));
    return static_cast<int>(BASE_T::npos == nIndex ? -1 : nIndex);
}

template <typename TYPE>
int CStringT<TYPE>::FindOneOf(PCSTR_T szCharSet) const
{
    SIZE_T nIndex = find_first_of( szCharSet );
    return static_cast<int>(BASE_T::npos == nIndex ? -1 : nIndex);
}

template <typename TYPE>
int CStringT<TYPE>::ReverseFind(TYPE ch) const
{
    SIZE_T nIndex = find_last_of( ch );
    return static_cast<int>(BASE_T::npos == nIndex ? -1 : nIndex);
}

template <typename TYPE>
int CStringT<TYPE>::ReverseFind(PCSTR_T pszSrc, SIZE_T pos) const
{
    SIZE_T nIndex = rfind(NULL == pszSrc ? CStringT<TYPE>() : pszSrc, pos);
    return static_cast<int>(BASE_T::npos == nIndex ? -1 : nIndex);
}

template <typename TYPE>
int CStringT<TYPE>::Remove(TYPE ch)
{
    SIZE_T nIndex   = 0;
    int nRemoved    = 0;

    while( (nIndex=find_first_of(ch)) != BASE_T::npos )
    {
        erase(nIndex, 1); nRemoved++;
    }
    return nRemoved;
}

template <typename TYPE>
void CStringT<TYPE>::ReleaseBuffer(int nNewLength)
{
    resize(static_cast<SIZE_T>(nNewLength > -1 ? nNewLength : BASE_T::length()));
}

template <typename TYPE>
void CStringT<TYPE>::FreeExtra()
{
    CStringT<TYPE> mt; swap(mt);
    if( ! mt.empty() ) BASE_T::assign(mt, mt.size());
}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::TrimLeft()
{
    erase(BASE_T::begin(), BASE_T::find_if(BASE_T::begin(),\
          BASE_T::end(),NotSpace<TYPE>(std::locale())));
    return *this;
}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::TrimLeft(TYPE chTarget)
{
    erase(0, find_first_not_of(chTarget)); return *this;
}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::TrimLeft(PCSTR_T pszTargets)
{
    erase(0, find_first_not_of(pszTargets)); return *this;
}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::TrimRight()
{
    std::locale loc;
    PRITER_T it = BASE_T::find_if(BASE_T::rbegin(), BASE_T::rend(), NotSpace<TYPE>(loc));

    if ( BASE_T::rend() != it ) erase(BASE_T::rend() - it);
    erase(it != BASE_T::rend() ? find_last_of(*it) + 1 : 0);
    return *this;
}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::TrimRight(TYPE chTarget)
{
    SIZE_T nIndex = find_last_not_of(chTarget);
    erase(BASE_T::npos == nIndex ? 0 : ++nIndex);
    return *this;
}

template <typename TYPE>
CStringT<TYPE>& CStringT<TYPE>::TrimRight(PCSTR_T pszTargets)
{
    SIZE_T nIndex = find_last_not_of(pszTargets);
    erase(BASE_T::npos == nIndex ? 0 : ++nIndex);
    return *this;
}

//////////////////////////////////////////////////////////////////////
// CStringT friend functions
//
//template <typename TYPE>
//CStringT<TYPE> operator+(const CStringT<TYPE>& strSrc, const CStringT<TYPE>& strAdd)
//{
//    CStringT<TYPE> strRet(strSrc);
//    strRet.append(strAdd); return strRet;
//}
//
//template <typename TYPE>
//CStringT<TYPE> operator+(const CStringT<TYPE>& strSrc, LPCSTR pszAdd)
//{
//    return CStringT<TYPE>(strSrc) + CStringT<TYPE>(pszAdd);
//}
//
//template <typename TYPE>
//CStringT<TYPE> operator+(const CStringT<TYPE>& strSrc, LPCWSTR pszAdd)
//{
//    return CStringT<TYPE>(strSrc) + CStringT<TYPE>(pszAdd);
//}

//
// Configuration
//

typedef CStringT<CHAR>  CStringA;
typedef CStringT<WCHAR> CStringW;

#ifdef UNICODE
    #define CString     CStringW
#else
    #define CString     CStringA
#endif

#endif // _CSTRING_H__INCLUDED_
