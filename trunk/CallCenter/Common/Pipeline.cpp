//
// Pipeline.cpp
//

#include "stdafx.h"
#include "Pipeline.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TEMP_BUFFER			1024

//////////////////////////////////////////////////////////////////////
// CPipeline construction

CPipeline::CPipeline()
{
	m_hPipe			= INVALID_HANDLE_VALUE;
	m_pInput		= NULL;
	m_pOutput		= NULL;
	m_bConnected	= FALSE;
	m_tConnected	= 0;
	
	m_pReadIoOver	= NULL;
	m_pWriteIoOver	= NULL;
	
	m_pBuffer		= NULL;
	m_nBuffer		= 0;
}

CPipeline::CPipeline(HANDLE hPipe)
{
	m_hPipe			= INVALID_HANDLE_VALUE;
	m_pInput		= NULL;
	m_pOutput		= NULL;
	m_bConnected	= FALSE;
	m_tConnected	= 0;
	
	m_pReadIoOver	= NULL;
	m_pWriteIoOver	= NULL;
	
	m_pBuffer		= NULL;
	m_nBuffer		= 0;
	
	AcceptFrom( hPipe );
}

CPipeline::~CPipeline()
{
	CPipeline::Close();
}

//////////////////////////////////////////////////////////////////////
// CPipeline attributes

BOOL CPipeline::IsConnected() const
{
	return m_bConnected;
}

//////////////////////////////////////////////////////////////////////
// CPipeline connect

BOOL CPipeline::Connect(LPCTSTR lpPipeName, DWORD nMaxInstances, DWORD nTimeOut)
{
	if ( m_hPipe != INVALID_HANDLE_VALUE ) return FALSE;
	
	SECURITY_ATTRIBUTES sa;
	PSECURITY_DESCRIPTOR pSD;
	
	pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,SECURITY_DESCRIPTOR_MIN_LENGTH);
	if ( ! InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION) )
	{
		LocalFree( (HLOCAL)pSD ); return FALSE;
	}
	if ( ! SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE) )
	{
		LocalFree( (HLOCAL)pSD ); return FALSE;
	}
	
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = pSD; 
	sa.bInheritHandle = TRUE;
	
	m_hPipe = CreateNamedPipe( lpPipeName, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, nMaxInstances, TEMP_BUFFER, TEMP_BUFFER, nTimeOut, &sa );
	
	if ( m_hPipe == INVALID_HANDLE_VALUE ) return FALSE;
	
	ASSERT( ! m_pReadIoOver );
	m_pReadIoOver	= new OVERLAPPED;
	ZeroMemory( m_pReadIoOver, sizeof(OVERLAPPED) );
	
	if ( ! ConnectNamedPipe( m_hPipe, m_pReadIoOver ) && ERROR_IO_PENDING != GetLastError() ) return FALSE;
	
TRACE("CreateNamedPipe(0x%x): %s\n", m_hPipe,lpPipeName);
	m_bConnected	= FALSE;
	m_tConnected	= GetTickCount();
	
	return TRUE;
}

BOOL CPipeline::ConnectTo(LPCTSTR lpPipeName, DWORD nTimeOut)
{
	if ( m_hPipe != INVALID_HANDLE_VALUE ) return FALSE;
	
	if ( ! WaitNamedPipe( lpPipeName, nTimeOut ) ) return FALSE;
	
	m_hPipe = CreateFile( lpPipeName, GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL );
	
	if ( m_hPipe == INVALID_HANDLE_VALUE ) return FALSE;
	
	return OnConnected();
}

//////////////////////////////////////////////////////////////////////
// CPipeline accept an incoming pipeline

void CPipeline::AcceptFrom(HANDLE hPipe)
{
	ASSERT( m_hPipe == INVALID_HANDLE_VALUE );
	
	m_hPipe			= hPipe;
	
	OnConnected();
}

//////////////////////////////////////////////////////////////////////
// CPipeline attach to an existing connection

void CPipeline::AttachTo(CPipeline* pConnection)
{
	ASSERT( m_hPipe == INVALID_HANDLE_VALUE );
	ASSERT( pConnection != NULL );
	ASSERT( pConnection->m_hPipe != INVALID_HANDLE_VALUE );
	
	m_pOutput		= pConnection->m_pOutput;
	m_pInput		= pConnection->m_pInput;
	m_hPipe			= pConnection->m_hPipe;
	m_bConnected	= pConnection->m_bConnected;
	m_tConnected	= pConnection->m_tConnected;
	
	m_pReadIoOver	= pConnection->m_pReadIoOver;
	m_pWriteIoOver	= pConnection->m_pWriteIoOver;
	
	m_pBuffer		= pConnection->m_pBuffer;
	m_nBuffer		= pConnection->m_nBuffer;
	
	pConnection->m_hPipe	= INVALID_HANDLE_VALUE;
	pConnection->m_pOutput	= NULL;
	pConnection->m_pInput	= NULL;
	pConnection->m_bConnected = FALSE;
	
	pConnection->m_pReadIoOver	= NULL;
	pConnection->m_pWriteIoOver	=NULL;
	
	pConnection->m_pBuffer		= NULL;
	pConnection->m_nBuffer		= 0;
}

//////////////////////////////////////////////////////////////////////
// CPipeline close

void CPipeline::Close()
{
	ASSERT( this != NULL );
	ASSERT( AfxIsValidAddress( this, sizeof(*this) ) );
	
	if ( m_hPipe != INVALID_HANDLE_VALUE )
	{
		FlushFileBuffers( m_hPipe );
		CloseHandle( m_hPipe );
TRACE("disconnect 0x%x\n", m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}
	
	if ( m_pOutput ) delete m_pOutput;
	m_pOutput = NULL;
	
	if ( m_pInput ) delete m_pInput;
	m_pInput = NULL;
	
	if ( m_pReadIoOver ) delete m_pReadIoOver;
	m_pReadIoOver	= NULL;
	
	if ( m_pWriteIoOver ) delete m_pWriteIoOver;
	m_pWriteIoOver	= NULL;
	
	if ( m_pBuffer ) delete [] m_pBuffer;
	m_pBuffer	= NULL;
	m_nBuffer	= 0;
	
	m_bConnected = FALSE;
	m_tConnected = GetTickCount();
}

//////////////////////////////////////////////////////////////////////
// CPipeline run function

BOOL CPipeline::OnRun()
{
	if ( INVALID_HANDLE_VALUE == m_hPipe ) return FALSE;
	
	if ( HasOverlappedIoCompleted( m_pReadIoOver ) )
	{
		if ( ! IsConnected() ) return OnConnected();
	}
	
	if ( ! IsConnected() ) return TRUE;
	
	if ( HasOverlappedIoCompleted( m_pReadIoOver ) )
	{
		if ( ! OnRead() ) return FALSE;
	}
	
	if ( HasOverlappedIoCompleted( m_pWriteIoOver ) )
	{
		if ( ! OnWrite() ) return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPipeline pipe event handlers

BOOL CPipeline::OnConnected()
{
	if ( m_pInput ) delete m_pInput;	// Safe
	if ( m_pOutput ) delete m_pOutput;
	
	m_pInput		= new CBuffer;
	m_pOutput		= new CBuffer;
	
	if ( m_pReadIoOver ) delete m_pReadIoOver;
	if ( m_pWriteIoOver ) delete m_pWriteIoOver;
	
	m_pReadIoOver	= new OVERLAPPED;
	ZeroMemory( m_pReadIoOver, sizeof(OVERLAPPED) );
	m_pWriteIoOver	= new OVERLAPPED;
	ZeroMemory( m_pWriteIoOver, sizeof(OVERLAPPED) );
	
	if ( m_pBuffer ) delete [] m_pBuffer;
	m_pBuffer		= new BYTE [TEMP_BUFFER];
	m_nBuffer		= 0;
	
	m_bConnected	= TRUE;
	m_tConnected	= GetTickCount();
	
TRACE("connect 0x%x\n", m_hPipe);
	return TRUE;
}

BOOL CPipeline::OnDropped()
{
	m_bConnected	= FALSE;
	m_tConnected	= GetTickCount();
	
	CPipeline::Close();
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CPipeline read event handler

BOOL CPipeline::OnRead()
{
	if ( m_hPipe == INVALID_HANDLE_VALUE ) return FALSE;
	
	if ( ! HasOverlappedIoCompleted( m_pReadIoOver ) ) return TRUE;
	if ( ! OnReadCompleted() ) return FALSE;
	
	DWORD dwRead = 0;
	BOOL bResult = PeekNamedPipe( m_hPipe, NULL, 0, NULL, &dwRead, NULL );
	
	if ( bResult && dwRead ) bResult =
	ReadFile( m_hPipe, m_pBuffer, m_nBuffer = TEMP_BUFFER, NULL, m_pReadIoOver );
	
	if ( ! bResult )
	{
		if ( GetLastError() != ERROR_IO_PENDING && GetLastError() != ERROR_NO_DATA )
		{
			_RPTF1(_CRT_WARN,"CPipeline::Read(%i) - Unable to read the data\n", GetLastError());
			return OnDropped();
		}
	}
	
	return TRUE;
}

BOOL CPipeline::OnReadCompleted()
{
	ASSERT( HasOverlappedIoCompleted( m_pReadIoOver ) );
	if ( m_nBuffer == 0 ) return TRUE;
	
	if ( ! GetOverlappedResult( m_hPipe, m_pReadIoOver, &m_nBuffer, FALSE ) )
	{
		if ( GetLastError() == ERROR_BROKEN_PIPE )
		{
			_RPTF0(_CRT_WARN,"CPipeline::Read - Overlapped completed without result\n");
			return OnDropped();
		}
	}
	
	DWORD nLength = min( ( m_nBuffer & 0xFFFFF ), TEMP_BUFFER );
	
	if ( nLength > 0 && nLength <= TEMP_BUFFER )
	{
		m_pInput->Add( m_pBuffer, nLength );
		DoPrint( (BYTE*)m_pBuffer, nLength, _T("RX:") );
	}
	
	m_nBuffer	= 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPipeline write event handler

BOOL CPipeline::OnWrite()
{
	if ( m_hPipe == INVALID_HANDLE_VALUE ) return FALSE;
	if ( m_pOutput->m_nLength == 0 ) return TRUE;
	
	if ( ! HasOverlappedIoCompleted( m_pWriteIoOver ) ) return TRUE;
	if ( ! OnWriteCompleted() ) return FALSE;
	
	BYTE* pBuffer = m_pOutput->m_pBuffer;
	DWORD nBuffer = m_pOutput->m_nLength;
	
	DWORD nLength = min( ( nBuffer & 0xFFFFF ), TEMP_BUFFER );
	
	if ( ! WriteFile( m_hPipe, (void*)pBuffer, nLength, &nLength, m_pWriteIoOver ) )
	{
		if ( ::GetLastError() != ERROR_IO_PENDING && GetLastError() != ERROR_NO_DATA )
		{
			_RPTF1(_CRT_WARN,"CPipeline::Write(%i) - Unable to write the data\n", GetLastError());
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CPipeline::OnWriteCompleted()
{
	ASSERT( HasOverlappedIoCompleted( m_pWriteIoOver ) );
	if ( m_pOutput->m_nLength == 0 ) return TRUE;
	
	DWORD dwWrite = 0;
	GetOverlappedResult( m_hPipe, m_pWriteIoOver, &dwWrite, FALSE );
	
	DWORD nLength = min( ( dwWrite & 0xFFFFF ), TEMP_BUFFER );
	
	if ( nLength > 0 && nLength <= TEMP_BUFFER )
	{
		DoPrint( (BYTE*)m_pOutput->m_pBuffer, nLength, _T("TX:") );
		m_pOutput->Remove( dwWrite );
	}
	
	return TRUE;
}

void CPipeline::DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader)
{
#ifdef _DEBUG
	afxDump << lpszHeader << "[" << nLength << "]" << " (" << m_hPipe << ") "
			<< ToASCII( (BYTE*)pBuffer, nLength ) <<"\n";
#endif
}

//////////////////////////////////////////////////////////////////////
// String conversion

CString CPipeline::ToHex(BYTE* pBuffer, DWORD nLength)
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

CString CPipeline::ToASCII(BYTE* pBuffer, DWORD nLength)
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
