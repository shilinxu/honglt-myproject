//
// XfaceTest.cpp
//

#include "stdafx.h"
#include "XfaceTest.h"
#include "XfaceTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CXfaceTestApp, CWinApp)
	//{{AFX_MSG_MAP(CXfaceTestApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXfaceTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXfaceTestApp construction

CXfaceTestApp::CXfaceTestApp()
{
	m_bInitialized	= FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CXfaceTestApp initialization

BOOL CXfaceTestApp::InitInstance()
{
	CWaitCursor pCursor;
	
	AfxOleInit();
	AfxEnableControlContainer();
	
	WSADATA wsaData;
	WSAStartup( 0x0202, &wsaData );
	
	m_pMainWnd = new CXfaceTestDlg();
	m_pMainWnd->ShowWindow( SW_SHOW );
	
	pCursor.Restore();
	
	return m_bInitialized = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXfaceTestApp termination

int CXfaceTestApp::ExitInstance() 
{
	CWaitCursor pCursor;
	
	WSACleanup();

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CXfaceTestApp help (suppress F1)

void CXfaceTestApp::WinHelp(DWORD dwData, UINT nCmd) 
{
}

#ifdef _DEBUG	// for debug break
BOOL CXfaceTestApp::OnIdle(LONG lCount)
{
	return CWinApp::OnIdle(lCount);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CXfaceTestApp get error string

CString CXfaceTestApp::GetErrorString()
{
	LPTSTR pszMessage = NULL;
	CString strMessage;
	
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&pszMessage, 0, NULL );
	
	if ( pszMessage != NULL )
	{
		strMessage = pszMessage;
		LocalFree( pszMessage );
	}
	
	return strMessage;
}

/////////////////////////////////////////////////////////////////////////////
// String functions

BOOL LoadString(CString& str, UINT nID)
{
	if ( str.LoadString( nID ) ) return TRUE;
	
	str.Empty(); return FALSE;
}
/*
void Replace(CString& strBuffer, LPCTSTR pszFind, LPCTSTR pszReplace)
{
	while ( TRUE )
	{
		int nPos = strBuffer.Find( pszFind );
		if ( nPos < 0 ) break;

		strBuffer = strBuffer.Left( nPos ) + pszReplace + strBuffer.Mid( nPos + _tcslen( pszFind ) );
	}
}
*/
//////////////////////////////////////////////////////////////////////
// String conversion
/*
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
*/
CString ToASCII(BYTE* pBuffer, DWORD nLength)
{
	CString strDump;
	
	LPTSTR pszDump = strDump.GetBuffer( nLength + 1 );
	
	for ( DWORD i = 0 ; i < nLength ; i++ )
	{
		int nChar = pBuffer[i];
		*pszDump++ = ( nChar >= 32 ? nChar : '.' );
	}
	
	*pszDump = 0;
	strDump.ReleaseBuffer();
	
	return strDump;
}

/////////////////////////////////////////////////////////////////////////////
// Case independent string search

LPCTSTR _tcsistr(LPCTSTR pszString, LPCTSTR pszPattern)
{
	LPCTSTR pptr, sptr, start;
	DWORD slen, plen;

	for (	start	= pszString,
			pptr	= pszPattern,
			slen	= _tcslen( pszString ),
			plen	= _tcslen( pszPattern ) ;
			slen >= plen ; start++, slen-- )
	{
		while ( toupper( *start ) != toupper( *pszPattern ) )
		{
			start++;
			slen--;

			if ( slen < plen ) return NULL;
		}

		sptr = start;
		pptr = pszPattern;

		while ( toupper( *sptr ) == toupper( *pptr ) )
		{
			sptr++;
			pptr++;

			if ( ! *pptr) return start;
		}
	}

	return NULL;
}

LPCTSTR _tcsnistr(LPCTSTR pszString, LPCTSTR pszPattern, DWORD plen)
{
	LPCTSTR pptr, sptr, start;
	DWORD slen, plen2;

	for (	start	= pszString,
			pptr	= pszPattern,
			slen	= _tcslen( pszString ) ;
			slen >= plen ; start++, slen-- )
	{
		while ( toupper( *start ) != toupper( *pszPattern ) )
		{
			start++;
			slen--;

			if ( slen < plen ) return NULL;
		}

		sptr = start;
		pptr = pszPattern;
		plen2 = plen;

		while ( toupper( *sptr ) == toupper( *pptr ) )
		{
			sptr++;
			pptr++;

			if ( ! --plen2 ) return start;
		}
	}

	return NULL;
}
