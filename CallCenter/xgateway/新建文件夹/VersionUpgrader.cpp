//
// VersionUpgrader.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "VersionUpgrader.h"

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader construction

CVersionUpgrader::CVersionUpgrader(SOCKET hSocket, SOCKADDR_IN* pHost) : CTransfer( hSocket, pHost )
{
}

CVersionUpgrader::~CVersionUpgrader()
{
}

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader disconnect

void CVersionUpgrader::Disconnect()
{
	CTransfer::Disconnect();
	delete this;
}

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader socket event handlers

BOOL CVersionUpgrader::OnRead()
{
	if ( ! CConnection::OnRead() ) return FALSE;
	
	if ( m_pInput->m_nLength ) CVersionUpgrader::ReadBuffer( m_pInput );
	
	return TRUE;
}

BOOL CVersionUpgrader::OnWrite()
{
	if ( m_pOutput->m_nLength ) m_tConnected = GetTickCount();
	
	return CConnection::OnWrite();
}

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader packet unserialize

int CVersionUpgrader::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	if ( ! pBuffer->ReadString( strLine ) ) return 0;
	
	DWORD nRead = 0;
	DWORD nLength = strLine.GetLength();
	LPBYTE pBlock = (BYTE*)strLine.GetBuffer(0);
	for ( ; (*pBlock) == ' ' && nRead < nLength; pBlock++, nRead++ );
	
	CHAR xField[128];
	DWORD nField = IPacket::ReadField( pBlock, xField, 16 );
	
	if ( ! _tcscmp( xField, "DOWNLOAD" ) )
	{
		pBlock += ( nRead += nField );
		for ( ; (*pBlock) == ' ' && nRead < nLength; pBlock++, nRead++ );
		IPacket::ReadField( pBlock, xField, 128 );
		
		if ( ! OnDownloadFile(xField) ) CTransfer::SendPacket( new IErrPacket(ICP_ERROR) );
	}
	
	return nLength;
}

BOOL CVersionUpgrader::OnDownloadFile(LPCTSTR pszFile)
{
	HANDLE hFile = CreateFile( pszFile, GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL );

	if ( hFile == INVALID_HANDLE_VALUE ) return FALSE;
	
	DWORD nSizeHigh	= 0;
	DWORD nSizeLow	= GetFileSize( hFile, &nSizeHigh );
	QWORD nFileSize	= (QWORD)nSizeLow | ( (QWORD)nSizeHigh << 32 );
	
	for ( QWORD nLength = nFileSize ; nLength > 0 ; )
	{
		DWORD nBlock	= (DWORD)min( nLength, QWORD(20480) );
		LPBYTE pBuffer	= new BYTE [nBlock];
		
		ReadFile( hFile, pBuffer, nBlock, &nBlock, NULL );
		SendPacket( pBuffer, nBlock );
		
		nLength -= nBlock;
		delete [] pBuffer;
	}
	
	CloseHandle( hFile );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CVersionUpgrader send packet

BOOL CVersionUpgrader::SendPacket(const void * pData, DWORD nLength)
{
	m_pOutput->Add( pData, nLength );
	CConnection::OnWrite();

	return TRUE;
}
