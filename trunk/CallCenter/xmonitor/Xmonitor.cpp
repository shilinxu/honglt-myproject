//
// Xmonitor.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "Settings.h"
#include "Profiles.h"
#include "Network.h"
#include "Skin.h"

#include "WndMain.h"
#include "DlgSplash.h"
#include "DlgConnect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Registry
const TCHAR	gcszCopyright[]		= _T("未来科技");
const TCHAR	gcszWindowName[]	= _T("CallCenter.Xmonitor");
const TCHAR	gcszWindowClass[]	= _T("CallCenter.XmonitorEx");

BEGIN_MESSAGE_MAP(CXmonitorApp, CWinApp)
	//{{AFX_MSG_MAP(CXmonitorApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXmonitorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXmonitorApp construction

CXmonitorApp::CXmonitorApp() : m_pMutex( FALSE, gcszWindowName )
{
	m_bInitialized	= FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CXmonitorApp initialization

BOOL CXmonitorApp::InitInstance()
{
	CWaitCursor pCursor;
	
	Enable3dControls();
	SetRegistryKey( gcszCopyright );
	
	AfxOleInit();
	AfxEnableControlContainer();
	
	WSADATA wsaData;
	WSAStartup( 0x0101, &wsaData );
	
	CConnectDlg pConnect;
	if ( pConnect.DoModal() != IDOK ) return FALSE;
	
	CSplashDlg* dlgSplash = new CSplashDlg;
	dlgSplash->Topmost();
	
	dlgSplash->Step( _T("注册表") );
		Settings.Load();
		Profiles.Setup();
		Skin.Apply();
	
	dlgSplash->Step( _T("GUI") );
		m_pMainWnd = new CMainWnd();
		CoolMenu.EnableHook();
#if 1
		CString strCaption;
		strCaption.LoadString( AFX_IDS_APP_TITLE );
		m_pMainWnd->SetWindowText( (LPCTSTR)strCaption );
#else
		m_pMainWnd->SetWindowText( _T("96031城市总机") );
#endif
		dlgSplash->Topmost();
		m_pMainWnd->ShowWindow( SW_SHOWMAXIMIZED );
		m_pMainWnd->UpdateWindow();
		
	dlgSplash->Step( _T("连接") );
		m_pMainWnd->SendMessage( WM_COMMAND, ID_TAB_CONNECT );
		
	dlgSplash->Hide();
	
	pCursor.Restore();
	
	return m_bInitialized = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXmonitorApp termination

int CXmonitorApp::ExitInstance() 
{
	CWaitCursor pCursor;
	
	CloseDatabase();
	if ( m_bInitialized ) Settings.Save();
	WSACleanup();
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CXmonitorApp help (suppress F1)

void CXmonitorApp::WinHelp(DWORD dwData, UINT nCmd) 
{
}

/////////////////////////////////////////////////////////////////////////////
// CXmonitorApp message

TCHAR CXmonitorApp::szMessageBuffer[16384];

void CXmonitorApp::Message(int nType, UINT nID, ...)
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

void CXmonitorApp::Message(int nType, LPCTSTR pszFormat, ...)
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

void CXmonitorApp::PrintMessage(int nType, LPCTSTR pszLog)
{
	if ( HWND hWnd = m_pMainWnd->GetSafeHwnd() )
	{
		PostMessage( hWnd, WM_LOG, nType, (LPARAM)_tcsdup( pszLog ) );
	}
}

void CXmonitorApp::LogMessage(LPCTSTR pszLog, BOOL bShowTime)
{
	CFile pFile;
	
	if ( pFile.Open( _T("Xmonitor.log"), CFile::modeReadWrite ) )
	{
		pFile.Seek( 0, CFile::end );
	}
	else
	{
		if ( ! pFile.Open( _T("Xmonitor.log"), CFile::modeWrite|CFile::modeCreate ) ) return;
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
// CXmonitorApp database

BOOL CXmonitorApp::OpenDatabase()
{
	// 创建进行存取的字符串
	
	CString sConnect = _T("");
	sConnect += _T("Provider=SQLOLEDB.1");
	sConnect += _T(";Persist Security Info=False");
	sConnect += _T(";User ID=sa");
	sConnect += _T(";Password=");
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

BOOL CXmonitorApp::CloseDatabase()
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
// CXmonitorApp version

void CXmonitorApp::GetVersionNumber(WORD nVersion[])
{
	TCHAR szPath[128];
	DWORD dwSize;
	
	nVersion[0] = nVersion[1] = nVersion[2] = nVersion[3] = 0;
	
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
				nVersion[0] = (WORD)( pTable->dwFileVersionMS >> 16 );
				nVersion[1] = (WORD)( pTable->dwFileVersionMS & 0xFFFF );
				nVersion[2] = (WORD)( pTable->dwFileVersionLS >> 16 );
				nVersion[3] = (WORD)( pTable->dwFileVersionLS & 0xFFFF );
			}
		}
		
		delete [] pBuffer;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXmonitorApp get error string

CString CXmonitorApp::GetErrorString()
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
