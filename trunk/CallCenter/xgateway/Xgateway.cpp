//
// Xgateway.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Settings.h"
#include "Profiles.h"
#include "Skin.h"

#include "WndMain.h"
#include "DlgSplash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Registry
const TCHAR	gcszCopyright[]		= _T("未来科技");
const TCHAR	gcszWindowName[]	= _T("CallCenter.Xgateway");
const TCHAR	gcszWindowClass[]	= _T("CallCenter.XgatewayEx");

BEGIN_MESSAGE_MAP(CXgatewayApp, CWinApp)
	//{{AFX_MSG_MAP(CXgatewayApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXgatewayApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXgatewayApp construction

CXgatewayApp::CXgatewayApp() : m_pMutex( FALSE, gcszWindowName )
{
	m_bInitialized	= FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CXgatewayApp initialization

BOOL CXgatewayApp::InitInstance()
{
	CWaitCursor pCursor;
	
#ifndef _BEST
	if ( SetFirstInstance() == FALSE ) return FALSE;
#endif

#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	SetRegistryKey( gcszCopyright );
	GetVersionNumber();

	AfxOleInit();
	AfxEnableControlContainer();
	
	m_bInitialized = TRUE;
	CSplashDlg* dlgSplash = new CSplashDlg;
		dlgSplash->Topmost();
	
	dlgSplash->Step( _T("Winsock") );
		WSADATA wsaData;
		if ( WSAStartup( 0x0101, &wsaData ) ) return FALSE;

	dlgSplash->Step( _T("注册表") );
		Settings.Load();
		Profiles.Setup();
		Skin.Apply();
	
	dlgSplash->Step( _T("数据库") );
		m_bInitialized = OpenDatabase();

	dlgSplash->Step( _T(" 图形用户界面") );

		m_pMainWnd = new CMainWnd();
		CoolMenu.EnableHook();

		CString strCaption;
		strCaption.LoadString( AFX_IDS_APP_TITLE );
		m_pMainWnd->SetWindowText( (LPCTSTR)strCaption );

		dlgSplash->Topmost();
		m_pMainWnd->ShowWindow( SW_SHOWMAXIMIZED );
		m_pMainWnd->UpdateWindow();
	
	dlgSplash->Step( _T("网络") );
		if ( m_bInitialized ) m_pMainWnd->SendMessage( WM_COMMAND, ID_TAB_CONNECT );
	
	pCursor.Restore();
	
	dlgSplash->Hide();
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXgatewayApp termination

int CXgatewayApp::ExitInstance() 
{
	CWaitCursor pCursor;
	
	CloseDatabase();
	Settings.Save();
	WSACleanup();

	return CWinApp::ExitInstance();
}

BOOL CXgatewayApp::SetFirstInstance()
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

/////////////////////////////////////////////////////////////////////////////
// CXgatewayApp help (suppress F1)

void CXgatewayApp::WinHelp(DWORD dwData, UINT nCmd) 
{
}

#ifdef _DEBUG	// for debug break
BOOL CXgatewayApp::OnIdle(LONG lCount)
{
	return CWinApp::OnIdle(lCount);
}
#endif
/////////////////////////////////////////////////////////////////////////////
// CXgatewayApp version

void CXgatewayApp::GetVersionNumber()
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
// CXgatewayApp message

TCHAR CXgatewayApp::szMessageBuffer[16384];

void CXgatewayApp::Message(int nType, UINT nID, ...)
{
	if ( nType == MSG_DEBUG && ! Settings.General.Debug ) return;
	if ( nType == MSG_TEMP && ! Settings.General.DebugLog ) return;
	
	CSingleLock pLock( &m_csMessage, TRUE );
	CString strFormat;
	va_list pArgs;
	
	LoadString( strFormat, nID );
	va_start( pArgs, nID );
	
	if ( strFormat.Find( _T("%1") ) >= 0 )
	{
		LPTSTR lpszTemp;
		if ( ::FormatMessage( FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ALLOCATE_BUFFER,
			strFormat, 0, 0, (LPTSTR)&lpszTemp, 0, &pArgs ) != 0 && lpszTemp != NULL )
		{
			PrintMessage( nType, lpszTemp );
			if ( Settings.General.DebugLog ) LogMessage( lpszTemp );
			LocalFree( lpszTemp );
		}
	}
	else
	{
		_vsntprintf( szMessageBuffer, 16380, strFormat, pArgs );
		PrintMessage( nType, szMessageBuffer );
		if ( Settings.General.DebugLog || nType == MSG_SYSTEM ) LogMessage( szMessageBuffer );
	}

	va_end( pArgs );
}

void CXgatewayApp::Message(int nType, LPCTSTR pszFormat, ...)
{
	if ( nType == MSG_DEBUG && ! Settings.General.Debug ) return;
	if ( nType == MSG_TEMP && ! Settings.General.DebugLog ) return;
	
	CSingleLock pLock( &m_csMessage, TRUE );
	CString strFormat;
	va_list pArgs;
	
	va_start( pArgs, pszFormat );
	_vsntprintf( szMessageBuffer, 16380, pszFormat, pArgs );
	va_end( pArgs );
	
	PrintMessage( nType, szMessageBuffer );
	if ( Settings.General.DebugLog || nType == MSG_SYSTEM ) LogMessage( szMessageBuffer );
}

void CXgatewayApp::PrintMessage(int nType, LPCTSTR pszLog)
{
	if ( HWND hWnd = m_pMainWnd->GetSafeHwnd() )
	{
		PostMessage( hWnd, WM_LOG, nType, (LPARAM)_tcsdup( pszLog ) );
	}
}

void CXgatewayApp::LogMessage(LPCTSTR pszLog, BOOL bShowTime)
{
	CFile pFile;
	
#ifdef _BEST
	if ( pFile.Open( _T("XgatewayDebug.log"), CFile::modeReadWrite ) )
#else
	if ( pFile.Open( _T("Xgateway.log"), CFile::modeReadWrite ) )
#endif
	{
		pFile.Seek( 0, CFile::end );
	}
	else
	{
#ifdef _BEST
		if ( ! pFile.Open( _T("XgatewayDebug.log"), CFile::modeWrite|CFile::modeCreate ) ) return;
#else
		if ( ! pFile.Open( _T("Xgateway.log"), CFile::modeWrite|CFile::modeCreate ) ) return;
#endif
	}

	if ( bShowTime )
	{
		CTime pNow = CTime::GetCurrentTime();
		CString strLine;
		
		strLine.Format( _T("[%.2i:%.2i:%.2i] %s\r\n"),
			pNow.GetHour(), pNow.GetMinute(), pNow.GetSecond(), pszLog );
		
		pFile.Write( (LPCTSTR)strLine, sizeof(TCHAR) * strLine.GetLength() );
	}
	else
	{
		pFile.Write( pszLog, sizeof(TCHAR) * _tcslen(pszLog) );
		pFile.Write( _T("\r\n"), sizeof(TCHAR) * 2 );
	}
	
	pFile.Close();
}

/////////////////////////////////////////////////////////////////////////////
// CXgatewayApp database

BOOL CXgatewayApp::OpenDatabase()
{
	// 创建进行存取的字符串

	CString sConnect = _T("");
	sConnect += _T("Provider=SQLOLEDB.1");
	sConnect += _T(";Persist Security Info=False");
	sConnect += _T(";User ID=") + Settings.Database.UserID;
	sConnect += _T(";Password=") + Settings.Database.Password;
	sConnect += _T(";Initial Catalog=") + Settings.Database.Database;
	sConnect += _T(";Data Source=") + Settings.Database.Host;
	
	TRACE( sConnect + "\n");
	
	try
	{
		xConnect(m_pConnection, (_bstr_t)sConnect);
	}
	catch(_com_error e)//捕捉异常
	{
		AfxMessageBox(CString("连接数据库失败：") + e.ErrorMessage());
		return FALSE;
	} 
	
	return TRUE;
}

BOOL CXgatewayApp::CloseDatabase()
{
	if ( NULL == m_pConnection ) return TRUE;
	
	try
	{
		if ( m_pConnection->State ) xDisconnect( m_pConnection );
		m_pConnection.Release();
	}
	catch(_com_error e)//捕捉异常
	{
		AfxMessageBox(CString("断开数据库失败：") + e.ErrorMessage());
		return FALSE;
	} 
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXgatewayApp get error string

CString CXgatewayApp::GetErrorString()
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
