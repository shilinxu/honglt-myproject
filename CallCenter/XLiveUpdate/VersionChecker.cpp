//
// VersionChecker.cpp
//

#include "stdafx.h"
#include "LiveUpdate.h"
#include "VersionChecker.h"

#include "..\Common\Buffer.h"

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
	m_hThread		= NULL;
	m_bThread		= FALSE;
	m_hInternet		= NULL;
	m_hSession		= NULL;
	m_hNotify		= NULL;
}

CVersionChecker::~CVersionChecker()
{
	Release();
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker Setup

BOOL CVersionChecker::Setup()
{
	Release();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker Release

void CVersionChecker::Release()
{
	Disconnect();
	
	m_hNotify = NULL;
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker set owner

void CVersionChecker::SetOwner(HWND hNotify)
{
	CSingleLock pLock( &m_pSection, TRUE );
	m_hNotify	= hNotify;
}

//////////////////////////////////////////////////////////////////////
// CVersionChecker connect

BOOL CVersionChecker::Connect()
{
	Disconnect();
	
	m_hInternet = InternetOpen( NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
	
	return TRUE;
}

void CVersionChecker::Disconnect()
{
	if ( m_hInternet ) InternetCloseHandle( m_hInternet );
	m_hInternet = NULL;
	
	if ( m_hThread == NULL ) return;
	m_bThread = FALSE;
	ResumeThread( m_hThread );
	
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
	CVersionChecker* pChecker = (CVersionChecker*)pParam;
	pChecker->OnRun();
	return 0;
}

void CVersionChecker::OnRun()
{
	BOOL bSuccess = TRUE;
	
	if ( bSuccess && !( bSuccess = ConnectTo("ftp.96031.com.cn") ) )
		SendMessageNotify( WM_NEXTPAGE, IDD_WIZARD_FAILED );
	
	if ( bSuccess && !( bSuccess = GetFile("update.lst") ) )
		SendMessageNotify( WM_NEXTPAGE, IDD_WIZARD_FAILED );
	
	int nNumFile = 0;
	if ( bSuccess && !( bSuccess = OnNextFile("update.lst", &nNumFile) ) )
		SendMessageNotify( WM_NEXTPAGE, IDD_WIZARD_NOTHING );
	
	if ( bSuccess ) SendMessageNotify( WM_NEXTPAGE, -1 );
	if ( bSuccess ) SuspendThread( m_hThread );
	
	if ( ! m_bThread ) return;
	
	if ( bSuccess && !( bSuccess = OnNextFile("update.lst") ) )	// begin to download
		SendMessageNotify( WM_NEXTPAGE, IDD_WIZARD_FAILED );
	else
		SendMessageNotify( WM_NEXTPAGE, IDD_WIZARD_FINISHED );
	
	if ( m_hSession ) InternetCloseHandle( m_hSession );
	m_hSession = NULL;
}

LRESULT CVersionChecker::SendMessageNotify(UINT nMsgId, WPARAM wParam, LPARAM lParam)
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	return SendMessage( m_hNotify, nMsgId, wParam, lParam );
}

BOOL CVersionChecker::ConnectTo(LPCTSTR xHost)
{
	if ( m_hSession ) InternetCloseHandle( m_hSession );
	m_hSession = NULL;
	
	SendMessageNotify( WM_CHECKOUT, (WPARAM)_T("正在连接服务器") );
	
#ifdef _DEBUG
	m_hSession = InternetConnect( m_hInternet, _T("ftp.96031.com.cn"),
		INTERNET_DEFAULT_FTP_PORT, "honglt", "542881", INTERNET_SERVICE_FTP , 0, 0 );
#else
	m_hSession = InternetConnect( m_hInternet, _T("ftp.96031.com.cn"),
		INTERNET_DEFAULT_FTP_PORT, NULL, NULL, INTERNET_SERVICE_FTP , 0, 0 );
#endif
	if ( ! m_hSession ) return FALSE;
	
	return TRUE;
}

BOOL CVersionChecker::GetFile(LPCTSTR xFile)
{
	if ( ! m_hSession ) return FALSE;
	
	CString strText;
	strText.Format( _T("正在下载 %s"), xFile );
	
	SendMessageNotify( WM_CHECKOUT, (WPARAM)(LPCTSTR)strText );
	return FtpGetFile( m_hSession, xFile, xFile, FALSE, 0, FTP_TRANSFER_TYPE_UNKNOWN, 0 );
}

BOOL CVersionChecker::OnNextFile(LPCTSTR xFile, int* pNumFile)
{
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
		while ( pBuffer->ReadString( strLine ) )
		{
			if ( strLine.FindOneOf("\r\n") != -1 )
				strLine = strLine.Left( strLine.FindOneOf("\r\n") );
			
			if ( pNumFile )
				*pNumFile += OnParseElement( (LPCTSTR)strLine, TRUE );
			else
			if ( bResult )
				bResult = OnParseElement( (LPCTSTR)strLine, FALSE );
		}
	}
	
	delete pBuffer;
	CloseHandle( hFile );
	
	if ( pNumFile ) return (*pNumFile) > 0;
	return bResult;
}

BOOL CVersionChecker::OnParseElement(LPCTSTR xElement, BOOL bOnlyCheck)
{
	CString strValue = xElement;
	strValue		+= '|';
	
	CString xFile	= strValue.SpanExcluding( _T("|;,") );
	strValue		= strValue.Mid( xFile.GetLength() + 1 );
	xFile.TrimLeft(); xFile.TrimRight();
	
	if ( strValue.IsEmpty() ) return FALSE;
	
	CString xVer	= strValue.SpanExcluding( _T("|;,") );
	strValue		= strValue.Mid( xVer.GetLength() + 1 );
	xVer.TrimLeft(); xVer.TrimRight();
	
	WORD nVersion[4];
	GetVersionNumber( xFile, nVersion );
	
	CString strVersion;
	strVersion.Format( _T("ver%i.%i.%i.%i"),
		nVersion[0], nVersion[1], nVersion[2], nVersion[3] );
	
	if ( strVersion.CompareNoCase( xVer ) )
		return bOnlyCheck ? TRUE : GetFile( xFile );
	
	return bOnlyCheck ? FALSE : TRUE;
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
