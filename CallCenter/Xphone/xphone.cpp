//
// Xphone.cpp
//

#include "stdafx.h"
#include "Xphone.h"
#include "Settings.h"
#include "Skin.h"

#include "DlgConnect.h"
#include "DlgSplash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Registry
const TCHAR	gcszCopyright[]		= _T("未来科技");
const TCHAR	gcszWindowName[]	= _T("CallCenter.Xphone");
const TCHAR	gcszWindowClass[]	= _T("CallCenter.XphoneEx");

BEGIN_MESSAGE_MAP(CXphoneApp, CWinApp)
	//{{AFX_MSG_MAP(CXphoneApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXphoneApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXphoneApp construction

CXphoneApp::CXphoneApp() : m_pMutex( FALSE, gcszWindowName )
{
	m_bInitialized	= FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CXphoneApp initialization

BOOL CXphoneApp::InitInstance()
{
	CWaitCursor pCursor;
	
	if ( ! SetFirstInstance() ) return FALSE;
	
	CString xUserId, xPassword;
	if ( ! CheckUsage( xUserId, xPassword ) ) return FALSE;

	Enable3dControls();

	SetRegistryKey( gcszCopyright );
	GetVersionNumber();

	AfxOleInit();
	AfxEnableControlContainer();
	
	CSplashDlg* dlgSplash = new CSplashDlg;
		dlgSplash->Topmost();
	
	dlgSplash->Step( _T("Winsock") );
		WSADATA wsaData;
		if ( WSAStartup( 0x0101, &wsaData ) ) return FALSE;

	dlgSplash->Step( _T("注册表") );
		Settings.Load();
		Skin.Apply();
	
	dlgSplash->Step( _T("GUI") );
		m_pMainWnd = new CConnectDlg;
		CoolMenu.EnableHook();
		
		dlgSplash->Topmost();
		m_pMainWnd->ShowWindow( SW_SHOWNORMAL );
//		m_pMainWnd->BringWindowToTop();
		
	dlgSplash->Step( _T("连接") );
		m_pMainWnd->SendMessage( WM_COMMAND, ID_NETWORK_CONNECT );
		
	dlgSplash->Hide();
	
	pCursor.Restore();
	
	return m_bInitialized = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXphoneApp termination

int CXphoneApp::ExitInstance() 
{
	CWaitCursor pCursor;
	
	if ( m_bInitialized ) Settings.Save();
	WSACleanup();

	return CWinApp::ExitInstance();
}

BOOL CXphoneApp::SetFirstInstance()
{
	if ( m_pMutex.Lock( 0 ) ) return TRUE;

	if ( CWnd* pWnd = CWnd::FindWindow( gcszWindowClass, NULL ) )
	{
		pWnd->SendMessage( WM_SYSCOMMAND, SC_RESTORE );
		pWnd->ShowWindow( SW_SHOWNORMAL );
		pWnd->BringWindowToTop();
		pWnd->SetForegroundWindow();
	}

	return FALSE;
}

BOOL CXphoneApp::CheckUsage(CString& xUserId, CString& xPassword)
{
	CString xCmdLine = m_lpCmdLine;
	xCmdLine.MakeLower();
	
	if ( m_lpCmdLine && xCmdLine.Find( "-u:") >= 0 )
	{
		xUserId = xCmdLine.Mid( xCmdLine.Find( "-u:" ) + 3 );
		xUserId	= xUserId.SpanExcluding( _T("|;, ") );
	}
	if ( m_lpCmdLine && xCmdLine.Find( "-p:") >= 0 )
	{
		xPassword = xCmdLine.Mid( xCmdLine.Find( "-p:" ) + 3 );
		xPassword = xPassword.SpanExcluding( _T("|;, ") );
	}
	
	return ! (xUserId.IsEmpty() | xPassword.IsEmpty());
}

/////////////////////////////////////////////////////////////////////////////
// CXphoneApp help (suppress F1)

void CXphoneApp::WinHelp(DWORD dwData, UINT nCmd) 
{
}

/////////////////////////////////////////////////////////////////////////////
// CXphoneApp version

void CXphoneApp::GetVersionNumber()
{
	TCHAR szPath[128];
	DWORD dwSize;
	
	m_nVersion[0] = m_nVersion[1] = m_nVersion[2] = m_nVersion[3] = 0;
	
	GetModuleFileName( NULL, szPath, 128 );
	dwSize = GetFileVersionInfoSize( szPath, &dwSize );
	
	if ( dwSize )
	{
		BYTE* pBuffer = new BYTE[ dwSize ];
		
		if ( GetFileVersionInfo( szPath, NULL, dwSize, pBuffer ) )
		{
			VS_FIXEDFILEINFO* pTable;
			
			if ( VerQueryValue( pBuffer, _T("\\"), (VOID**)&pTable, (UINT*)&dwSize ) )
			{
				m_nVersion[0] = (WORD)( pTable->dwFileVersionMS >> 16 );
				m_nVersion[1] = (WORD)( pTable->dwFileVersionMS & 0xFFFF );
				m_nVersion[2] = (WORD)( pTable->dwFileVersionLS >> 16 );
				m_nVersion[3] = (WORD)( pTable->dwFileVersionLS & 0xFFFF );
			}
		}
		
		delete [] pBuffer;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXphoneApp get error string

CString CXphoneApp::GetErrorString()
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

void Replace(CString& strBuffer, LPCTSTR pszFind, LPCTSTR pszReplace)
{
	while ( TRUE )
	{
		int nPos = strBuffer.Find( pszFind );
		if ( nPos < 0 ) break;

		strBuffer = strBuffer.Left( nPos ) + pszReplace + strBuffer.Mid( nPos + _tcslen( pszFind ) );
	}
}

//////////////////////////////////////////////////////////////////////
// String conversion

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
