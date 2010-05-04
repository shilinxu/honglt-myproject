//
// VersionChecker.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "VersionChecker.h"
#include "VersionUpgrader.h"

#include "Network.h"
#include "DlgSplash.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CVersionChecker VersionChecker;

//////////////////////////////////////////////////////////////////////
// CVersionChecker construction

CVersionChecker::CVersionChecker()
{
#if 0
	m_hThread		= NULL;
	m_bThread		= FALSE;
#endif
	m_pWndNotify	= NULL;
	m_pUpgrader		= NULL;
}

CVersionChecker::~CVersionChecker()
{
	Release();
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker Setup

BOOL CVersionChecker::Setup(CSplashDlg* pWndNotify)
{
	Release();
	m_pWndNotify	= pWndNotify;
	
	m_pUpgrader = new CVersionUpgrader;
	m_pUpgrader->ConnectTo( "192.168.0.19", 2020 );
	if ( ! m_pUpgrader->IsConnected() ) return FALSE;
	
	m_pWndNotify->Step( _T("update.lst") );
	m_pUpgrader->DownloadFile( _T("update.lst") );
	
	BOOL bResult = OnNextFile( _T("update.lst") );
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	m_pUpgrader->Disconnect();
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker Release

void CVersionChecker::Release()
{
	m_pWndNotify	= NULL;
	
	if ( m_pUpgrader ) delete m_pUpgrader;
	m_pUpgrader		= NULL;
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker Restart

void CVersionChecker::Restart(CWinApp* pApp)
{
	TCHAR szPath[128];
	GetModuleFileName( pApp->m_hInstance, szPath, 128 );
	
	CString strPath = szPath;
	if ( strPath.ReverseFind( '\\' ) >= 0 )
		strPath = strPath.Left( strPath.ReverseFind( '\\' ) );
	
	CString strParam;
	DWORD nProcessId = GetCurrentProcessId();
	strParam.Format( "%s %i", szPath, nProcessId );
	
	ShellExecute( NULL, _T("open"), _T("upgrader"),
				strParam, strPath, SW_SHOWNORMAL );
}

#if 0
//////////////////////////////////////////////////////////////////////
// CVersionChecker thread run

void CVersionChecker::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CVersionChecker::StopThread()
{
	if ( m_hThread == NULL ) return;
	
	m_bThread = FALSE;
	m_pWakeup.SetEvent();
	
	for ( int nAttempt = 50 ; nAttempt > 0 ; nAttempt-- )
	{
		DWORD nCode;
		if ( ! GetExitCodeThread( m_hThread, &nCode ) ) break;
		if ( nCode != STILL_ACTIVE ) break;
		Sleep( 100 );
	}
	
	if ( nAttempt == 0 )
	{
		TerminateThread( m_hThread, 0 );
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CVersionChecker::ThreadStart(LPVOID pParam)
{
	CVersionChecker* pClass = (CVersionChecker*)pParam;
	pClass->OnRun();
	return 0;
}

void CVersionChecker::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
		WaitForSingleObject( m_pWakeup, 50 );
		
		pLock.Lock();
		if ( m_pUpgrader ) m_pUpgrader->OnRun();
		pLock.Unlock();
	}
}

BOOL CVersionChecker::HitUpgrader()
{
	return TRUE;
}
#endif

BOOL CVersionChecker::OnNextFile(LPCTSTR xFile)
{
return TRUE;
	HANDLE hFile = CreateFile( xFile, GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL );

	if ( hFile == INVALID_HANDLE_VALUE ) return FALSE;
	
	DWORD nSizeHigh	= 0;
	DWORD nSizeLow	= GetFileSize( hFile, &nSizeHigh );
	QWORD nFileSize	= (QWORD)nSizeLow | ( (QWORD)nSizeHigh << 32 );
	
	CBuffer* pBuffer = new CBuffer;
	BOOL bResult = TRUE;
	
	for ( QWORD nLength = nFileSize ; nLength > 0 ; )
	{
		DWORD nBlock	= (DWORD)min( nLength, QWORD(20480) );
		LPBYTE pBlock	= new BYTE [nBlock];
		
		ReadFile( hFile, pBlock, nBlock, &nBlock, NULL );
		pBuffer->Add( pBlock, nBlock );
		
		nLength -= nBlock;
		delete [] pBlock;
		
		CString strLine;
		if ( ! pBuffer->ReadString( strLine ) ) break;
		
		if ( strLine.FindOneOf("\r\n") != -1 )
			strLine = strLine.Left( strLine.FindOneOf("\r\n") );
		if ( bResult ) bResult = OnParseElement( (LPCTSTR)strLine );
	}
	
	delete pBuffer;
	CloseHandle( hFile );
	
	return bResult;
}

BOOL CVersionChecker::OnParseElement(LPCTSTR xElement)
{
	CString strValue = xElement;
	strValue		+= '|';
	
	CString xFile	= strValue.SpanExcluding( _T("|;,") );
	strValue		= strValue.Mid( xFile.GetLength() + 1 );
	xFile.TrimLeft(); xFile.TrimRight();
	
	CString xVer	= strValue.SpanExcluding( _T("|;,") );
	strValue		= strValue.Mid( xVer.GetLength() + 1 );
	xVer.TrimLeft(); xVer.TrimRight();
	
	WORD nVersion[4];
	GetVersionNumber( xFile, nVersion );
	
	CString strVersion;
	strVersion.Format( _T("ver%i.%i.%i.%i"),
		nVersion[0], nVersion[1], nVersion[2], nVersion[3] );
	
	if ( strVersion.CompareNoCase( xVer ) )
	m_pUpgrader->DownloadFile( xFile );
	
	return TRUE;
}

void CVersionChecker::GetVersionNumber(LPCTSTR xFile, WORD nVersion[])
{
	DWORD dwSize;

	nVersion[0] = nVersion[1] = nVersion[2] = nVersion[3] = 0;

	dwSize = GetFileVersionInfoSize( (LPTSTR)xFile, &dwSize );

	if ( dwSize )
	{
		BYTE* pBuffer = new BYTE[ dwSize ];

		if ( GetFileVersionInfo( (LPTSTR)xFile, NULL, dwSize, pBuffer ) )
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
