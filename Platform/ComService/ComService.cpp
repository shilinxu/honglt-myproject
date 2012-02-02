//
// ComService.cpp
//

#include "stdafx.h"
#include "ComService.h"

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason==DLL_PROCESS_ATTACH)
	{
// 		CoInitialize(NULL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
// 		::CoUninitialize();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// String conversion

BEGIN_VERSIONED_NAMESPACE_DECL(Convert)

CString ToHex(BYTE* pBuffer, DWORD nLength)
{
	LPCTSTR pszHex = _T("0123456789ABCDEF");
	CString strDump;

	LPTSTR pszDump = strDump.GetBuffer( nLength * 3 );

	for ( DWORD i = 0 ; i < nLength ; i++ )
	{
		int nChar = pBuffer[i];
		if ( i ) *pszDump++ = ' ';
		*pszDump++ = pszHex[ nChar >> 4 ];
		*pszDump++ = pszHex[ nChar & 0x0F ];
	}

	*pszDump = 0;
	strDump.ReleaseBuffer();

	return strDump;
}

CString ToASCII(BYTE* pBuffer, DWORD nLength)
{
	CString strDump;

	LPTSTR pszDump = strDump.GetBuffer( nLength + 1 );

	for ( DWORD i = 0 ; i < nLength ; i++ )
	{
		int nChar = pBuffer[i];
		*pszDump++ = WCHAR( nChar >= 32 ? nChar : '.' );
	}

	*pszDump = 0;
	strDump.ReleaseBuffer();

	return strDump;
}

END_VERSIONED_NAMESPACE_DECL()
