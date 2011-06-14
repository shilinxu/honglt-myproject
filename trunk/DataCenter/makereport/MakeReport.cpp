//
// MakeReport.cpp
//

#include "stdafx.h"
#include "MakeReport.h"
#include "Settings.h"
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
const TCHAR	gcszWindowName[]	= _T("DataCenter.MakeReport");
const TCHAR	gcszWindowClass[]	= _T("CallCenter.MakeReportEx");

BEGIN_MESSAGE_MAP(CMakeReportApp, CWinApp)
	//{{AFX_MSG_MAP(CMakeReportApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMakeReportApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMakeReportApp construction

CMakeReportApp::CMakeReportApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMakeReportApp initialization

BOOL CMakeReportApp::InitInstance()
{
	CWaitCursor pCursor;
	
#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

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
		
// 	dlgSplash->Step( _T("数据库") );
// 		m_bInitialized = OpenDatabase();

	dlgSplash->Step( _T(" 图形用户界面") );

		m_pMainWnd = new CMainWnd();
		CoolMenu.EnableHook();

		CString strCaption;
		strCaption.LoadString( AFX_IDS_APP_TITLE );
		m_pMainWnd->SetWindowText( (LPCTSTR)strCaption );

		dlgSplash->Topmost();
		//m_pMainWnd->ShowWindow( SW_SHOWMAXIMIZED );
		m_pMainWnd->ShowWindow( SW_SHOW );
		m_pMainWnd->UpdateWindow();
	
	dlgSplash->Step( _T("网络") );
	if ( Settings.General.AutoUnicom )
		m_pMainWnd->SendMessage( WM_COMMAND, ID_TAB_UNICOM );

	pCursor.Restore();
	
	dlgSplash->Hide();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMakeReportApp termination

int CMakeReportApp::ExitInstance() 
{
	CWaitCursor pCursor;
	CloseDatabase();
	Settings.Save();
	
	WSACleanup();
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CMakeReportApp help (suppress F1)

void CMakeReportApp::WinHelp(DWORD dwData, UINT nCmd) 
{
}

/////////////////////////////////////////////////////////////////////////////
// CMakeReportApp version

void CMakeReportApp::GetVersionNumber()
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
// CMakeReportApp database

BOOL CMakeReportApp::OpenDatabase()
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

BOOL CMakeReportApp::CloseDatabase()
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
// CMakeReportApp get error string

CString CMakeReportApp::GetErrorString()
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
