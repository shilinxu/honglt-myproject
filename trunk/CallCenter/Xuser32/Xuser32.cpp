//
// Xuser32.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Settings.h"
#include "Network.h"
#include "Skin.h"

#include "WndMain.h"

#include "DlgConnect.h"
#include "DlgSplash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Registry
const TCHAR	gcszCopyright[]		= _T("未来科技");
const TCHAR	gcszWindowName[]	= _T("CallCenter.Xuser32");
const TCHAR	gcszWindowClass[]	= _T("CallCenter.Xuser32Ex");

BEGIN_MESSAGE_MAP(CXuser32App, CWinApp)
	//{{AFX_MSG_MAP(CXuser32App)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXuser32App theApp;

/////////////////////////////////////////////////////////////////////////////
// CXuser32App construction

CXuser32App::CXuser32App() : m_pMutex( FALSE, gcszWindowName )
{
	m_bInitialized	= FALSE;
	m_bTestor		= FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CXuser32App initialization

BOOL CXuser32App::InitInstance()
{
	CWaitCursor pCursor;
	
#ifndef _DEBUG
	if ( SetFirstInstance() == FALSE ) return FALSE;
#endif
	
	Enable3dControls();
	SetRegistryKey( gcszCopyright );
	
	AfxOleInit();
	AfxEnableControlContainer();
	
	WSADATA wsaData;
	WSAStartup( 0x0101, &wsaData );
	
	CString xUserId, xPassword;
	CheckUsage( xUserId, xPassword, m_bTestor );
	
	CConnectDlg pConnect;
	if ( pConnect.DoModal(xUserId, xPassword) != IDOK ) return FALSE;
	//if ( pConnect.DoModal() != IDOK ) return FALSE;
	
	CSplashDlg* dlgSplash = new CSplashDlg;
	dlgSplash->Topmost();
	
	dlgSplash->Step( _T("注册表") );
		Settings.Load();
		Skin.Apply();
		
	dlgSplash->Step( _T("GUI") );
		m_pMainWnd = new CMainWnd();
		CoolMenu.EnableHook();
#if 0
		CString strCaption;
		strCaption.LoadString( AFX_IDS_APP_TITLE );
		m_pMainWnd->SetWindowText( (LPCTSTR)strCaption );
#else
		m_pMainWnd->SetWindowText( _T("96031城市总机") );
#endif
		dlgSplash->Topmost();
		m_pMainWnd->ShowWindow( SW_SHOW );
		m_pMainWnd->UpdateWindow();
		
	dlgSplash->Step( _T("连接") );
		m_pMainWnd->SendMessage( WM_COMMAND, ID_TAB_CONNECT );
		
	dlgSplash->Hide();
	
	pCursor.Restore();
	
	return m_bInitialized = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXuser32App termination

int CXuser32App::ExitInstance() 
{
	CWaitCursor pCursor;
	
	if ( m_bInitialized ) Settings.Save();
	
	Network.Disconnect();
	WSACleanup();

	return CWinApp::ExitInstance();
}

BOOL CXuser32App::SetFirstInstance()
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

BOOL CXuser32App::CheckUsage(CString& xUserId, CString& xPassword, BOOL& bTestor)
{
	CString xCmdLine = m_lpCmdLine;
	xCmdLine.MakeLower();
	
	if ( m_lpCmdLine && xCmdLine.Find( "-t") >= 0 )
	{
		bTestor = TRUE;
	}
	
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
	
	return ! (xUserId.IsEmpty() ^ xPassword.IsEmpty());
}

/////////////////////////////////////////////////////////////////////////////
// CXuser32App help (suppress F1)

void CXuser32App::WinHelp(DWORD dwData, UINT nCmd) 
{
}

/////////////////////////////////////////////////////////////////////////////
// CXuser32App message

TCHAR CXuser32App::szMessageBuffer[16384];

void CXuser32App::Message(int nType, UINT nID, ...)
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

void CXuser32App::Message(int nType, LPCTSTR pszFormat, ...)
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

void CXuser32App::PrintMessage(int nType, LPCTSTR pszLog)
{
	if ( HWND hWnd = m_pMainWnd->GetSafeHwnd() )
	{
		PostMessage( hWnd, WM_LOG, nType, (LPARAM)_tcsdup( pszLog ) );
	}
}

void CXuser32App::LogMessage(LPCTSTR pszLog, BOOL bShowTime)
{
	CFile pFile;
	
	if ( pFile.Open( _T("Xuser32.log"), CFile::modeReadWrite ) )
	{
		pFile.Seek( 0, CFile::end );
	}
	else
	{
		if ( ! pFile.Open( _T("Xuser32.log"), CFile::modeWrite|CFile::modeCreate ) ) return;
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
// CXuser32App version

void CXuser32App::GetVersionNumber(WORD nVersion[])
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
// CXuser32App get error string

CString CXuser32App::GetErrorString()
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
