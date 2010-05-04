//
// VersionUpgrader.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "VersionUpgrader.h"
#include "VersionChecker.h"

#include "..\COMMON\md5.h"

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader construction

CVersionUpgrader::CVersionUpgrader()
{
/*	MD5 ctx;
	UCHAR sss[32];
		ctx.update( (UCHAR*)"Xuser32.exe|ver3.0\r\n", 20 );
		ctx.update( (UCHAR*)"xProfile.xml\r\n", 14 );
	ctx.finalize();
	ctx.raw_digest(	(UCHAR *)sss );
	CString str;
	str.Format( 
		_T("md5:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"),
		sss[0], sss[1], sss[2], sss[3],
		sss[4], sss[5], sss[6], sss[7],
		sss[8], sss[9], sss[10], sss[11],
		sss[12], sss[13], sss[14], sss[15] );
	TRACE("%s\n", str);
*/
	m_fnProgress	= NULL;
	m_hFile			= INVALID_HANDLE_VALUE;
}

CVersionUpgrader::~CVersionUpgrader()
{
	m_fnProgress	= NULL;
	ASSERT( m_hFile == INVALID_HANDLE_VALUE );
}

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader connect

#define MAX_CONNECT_TIMEOUT	(3* 1000)

BOOL CVersionUpgrader::ConnectTo(LPCTSTR pAddress, int nPort)
{
	CTransfer::ConnectTo( pAddress, nPort );
	
	return WaitForSingleObject( m_pWakeup, MAX_CONNECT_TIMEOUT ) == WAIT_OBJECT_0;
}

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader downalod

#define MAX_DOWNLOAD_TIMEOUT	(60* 1000)

BOOL CVersionUpgrader::DownloadFile(LPCTSTR xFile)
{
	if ( ! IsConnected() ) return FALSE;
	
	CString strInput;
	strInput.Format( "DOWNLOAD %s\r\n", xFile );
	
	SendPacket( (LPCTSTR)strInput, strInput.GetLength() );
	
	if ( WaitForSingleObject( m_pWakeup, MAX_DOWNLOAD_TIMEOUT ) == WAIT_OBJECT_0 )
		return OnFileCompletion( (LPCTSTR)m_strFile );
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader socket event handlers

BOOL CVersionUpgrader::OnConnected()
{
	CConnection::OnConnected();
	
	m_pWakeup.SetEvent();
	return TRUE;
}

void CVersionUpgrader::OnDropped(BOOL bError)
{
	CTransfer::OnDropped( bError );
	m_pWakeup.SetEvent();
}

BOOL CVersionUpgrader::OnRead()
{
	if ( ! CConnection::OnRead() ) return FALSE;
	
	if ( m_pInput->m_nLength ) ReadBuffer( m_pInput );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTransfer packet unserialize

int CVersionUpgrader::ReadBuffer(CBuffer* pBuffer)
{
	if ( m_hFile == INVALID_HANDLE_VALUE )
	{
		CString strLine;
		if ( ! m_pInput->ReadString( strLine ) ) return 0;
		
		OnFileHeader( (LPCTSTR)strLine );
		return strLine.GetLength();
	}
	else
	{
		DWORD nRead = OnFileContent( pBuffer );
		pBuffer->Remove( nRead );
		
		if ( (m_nCookie += nRead) >= m_nLength )
		{
			CloseHandle( m_hFile ); m_hFile = INVALID_HANDLE_VALUE;
			m_pWakeup.SetEvent();
		}
		
		return nRead;
	}
	
	return 0;
}

BOOL CVersionUpgrader::OnFileHeader(LPCTSTR lpszPacket)
{
	CHAR xField[MAX_PATH];
	DWORD nLength = _tcslen( lpszPacket );
	
	DWORD nRead = ReadField( (LPBYTE)lpszPacket, xField, MAX_PATH );
	BYTE* ptr = (LPBYTE)lpszPacket + nRead;
	
	m_strFile.Format( "%s.upd", xField );
	m_hFile = CreateFile( (LPCTSTR)m_strFile, GENERIC_WRITE,
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL );
	
	if ( m_hFile == INVALID_HANDLE_VALUE ) return FALSE;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, xField, 10 );
	nRead += nField; ptr += nField;
	
	m_nLength = _ttoi( xField ); m_nCookie = 0;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, m_strMD5hash.GetBuffer(36), 36 );
	m_strMD5hash.ReleaseBuffer( nField );
	nRead += nField; ptr += nField;
	
	if ( *( ptr ) == '\r' ) return TRUE;
	CloseHandle( m_hFile ); m_hFile = INVALID_HANDLE_VALUE;
	return FALSE;
}

int CVersionUpgrader::OnFileContent(CBuffer* pBuffer)
{
	ASSERT( m_hFile != INVALID_HANDLE_VALUE );
	DWORD nBuffer = m_nLength - m_nCookie;
	
	nBuffer = min( pBuffer->m_nLength, nBuffer );
	WriteFile( m_hFile, pBuffer->m_pBuffer, nBuffer, &nBuffer, NULL );
	
	return nBuffer;
}

BOOL CVersionUpgrader::OnFileCompletion(LPCTSTR xFile)
{
	HANDLE hFile = CreateFile( xFile, GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL );

	if ( hFile == INVALID_HANDLE_VALUE ) return FALSE;
	
	BOOL bResult = HashWithFile( hFile );
	if ( bResult && m_fnProgress ) (this->*m_fnProgress)( hFile );
	
	CloseHandle( hFile );
	
	return bResult;
}

BOOL CVersionUpgrader::HashWithFile(HANDLE hFile)
{
	DWORD nSizeHigh	= 0;
	DWORD nSizeLow	= GetFileSize( hFile, &nSizeHigh );
	QWORD nFileSize	= (QWORD)nSizeLow | ( (QWORD)nSizeHigh << 32 );
	
	MD5 ctx;
	BYTE hash[32];
	
	for ( QWORD nLength = nFileSize ; nLength > 0 ; )
	{
		DWORD nBlock	= (DWORD)min( nLength, QWORD(20480) );
		LPBYTE pBuffer	= new BYTE [nBlock];
		
		ReadFile( hFile, pBuffer, nBlock, &nBlock, NULL );
		ctx.update( pBuffer, nBlock );
		
		nLength -= nBlock;
		delete [] pBuffer;
	}
	
	ctx.finalize();
	ctx.raw_digest(	hash );
	
	CString strHash;
	strHash.Format( 
		_T("md5:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"),
		hash[0], hash[1], hash[2], hash[3],
		hash[4], hash[5], hash[6], hash[7],
		hash[8], hash[9], hash[10], hash[11],
		hash[12], hash[13], hash[14], hash[15] );
	
	return strHash.CompareNoCase( (LPCTSTR)m_strMD5hash )== 0;
}

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader progress

void CVersionUpgrader::OnUpdateList(HANDLE hFile)
{
	SetFilePointer( hFile, 0, 0, FILE_BEGIN );
}

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader parsing

int CVersionUpgrader::ReadField(BYTE* pBuffer, CHAR* sField, int nField, CHAR cPart)
{
	for ( int nLength = 0; nLength < nField && *( pBuffer + nLength ) != '\r'; nLength++ )
	{
		if ( *( pBuffer + nLength ) == cPart ) break;
		sField[ nLength ] = *( pBuffer + nLength );
	}
	
	if ( nLength < nField ) sField[ nLength ] = '\0';

	return nLength;
}
