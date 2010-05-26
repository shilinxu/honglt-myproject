//
// Buffer.cpp
//

#include "stdafx.h"
#include "Buffer.h"

#define BLOCK_SIZE			1024
#define BLOCK_MASK			0xFFFFFC00

///////////////////////////////////////////////////////////////////////////////
// CBuffer construction

CBuffer::CBuffer()
{
	m_pBuffer	= NULL;
	m_nBuffer	= 0;
	m_nLength	= 0;
}

CBuffer::~CBuffer()
{
	if ( m_pBuffer ) free( m_pBuffer );
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer add

void CBuffer::Add(const void * pData, DWORD nLength)
{
	if ( m_nLength + nLength > m_nBuffer )
	{
		m_nBuffer = m_nLength + nLength;
		m_nBuffer = ( m_nBuffer + BLOCK_SIZE - 1 ) & BLOCK_MASK;
		m_pBuffer = (BYTE*)realloc( m_pBuffer, m_nBuffer );
	}
	else if ( m_nBuffer > 0x80000 && m_nLength + nLength < 0x40000 )
	{
		m_nBuffer = 0x40000;
		m_pBuffer = (BYTE*)realloc( m_pBuffer, m_nBuffer );
	}

	CopyMemory( m_pBuffer + m_nLength, pData, nLength );
	m_nLength += nLength;
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer insert

void CBuffer::Insert(DWORD nOffset, const void * pData, DWORD nLength)
{
	if ( m_nLength + nLength > m_nBuffer )
	{
		m_nBuffer = m_nLength + nLength;
		m_nBuffer = ( m_nBuffer + BLOCK_SIZE - 1 ) & BLOCK_MASK;
		m_pBuffer = (BYTE*)realloc( m_pBuffer, m_nBuffer );
	}
	else if ( m_nBuffer > 0x80000 && m_nLength + nLength < 0x40000 )
	{
		m_nBuffer = 0x40000;
		m_pBuffer = (BYTE*)realloc( m_pBuffer, m_nBuffer );
	}
	
	MoveMemory( m_pBuffer + nOffset + nLength, m_pBuffer + nOffset, m_nLength - nOffset );
	CopyMemory( m_pBuffer + nOffset, pData, nLength );
	m_nLength += nLength;
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer remove

void CBuffer::Remove(DWORD nLength)
{
	if ( nLength > m_nLength || nLength == 0 ) return;
	
	m_nLength -= nLength;
	MoveMemory( m_pBuffer, m_pBuffer + nLength, m_nLength );
}

void CBuffer::Clear()
{
	m_nLength = 0;
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer add utilities

void CBuffer::Print(LPCSTR pszText)
{
	if ( pszText == NULL ) return;
	
	Add( (void*)pszText, strlen( pszText ) );
}

void CBuffer::Print(LPCWSTR pszText, UINT nCodePage)
{
	if ( pszText == NULL ) return;
	
	int nLength = wcslen( pszText );
	int nBytes = WideCharToMultiByte( nCodePage, 0, pszText, nLength, NULL, 0, NULL, NULL );
	
	EnsureBuffer( (DWORD)nBytes );
	
	WideCharToMultiByte( nCodePage, 0, pszText, nLength, (LPSTR)( m_pBuffer + m_nLength ), nBytes, NULL, NULL );
	m_nLength += nBytes;
}

DWORD CBuffer::AddBuffer(CBuffer* pBuffer, DWORD nLength, BOOL bPeek)
{
	nLength = nLength < 0xFFFFFFFF ? ( min( pBuffer->m_nLength, nLength ) ) : pBuffer->m_nLength;
	Add( pBuffer->m_pBuffer, nLength );
	
	if ( ! bPeek ) pBuffer->Remove( nLength );
	return nLength;
}

void CBuffer::AddReversed(const void * pData, DWORD nLength)
{
	EnsureBuffer( nLength );
	ReverseBuffer( pData, m_pBuffer + m_nLength, nLength );
	
	m_nLength += nLength;
}

void CBuffer::Prefix(LPCSTR pszText)
{
	if ( NULL == pszText ) return;
	
	Insert( 0, (void*)pszText, strlen( pszText ) );
}

void CBuffer::EnsureBuffer(DWORD nLength)
{
	if ( m_nBuffer - m_nLength >= nLength ) return;
	
	m_nBuffer = m_nLength + nLength;
	m_nBuffer = ( m_nBuffer + BLOCK_SIZE - 1 ) & BLOCK_MASK;
	m_pBuffer = (BYTE*)realloc( m_pBuffer, m_nBuffer );
}

//////////////////////////////////////////////////////////////////////
// CBuffer reverse buffer

void CBuffer::ReverseBuffer(const void* pInput, void* pOutput, DWORD nLength)
{
	const DWORD* pInputWords = (const DWORD*)( (const BYTE*)pInput + nLength );

	register DWORD nTemp;
	DWORD* pOutputWords      = (DWORD*)( pOutput );

	while ( nLength > 4 )
	{
		nTemp = *--pInputWords;

		*pOutputWords++ = SWAP_LONG( nTemp );

		nLength -= 4;
	}

	if ( nLength )
	{
		const BYTE* pInputBytes	= (const BYTE*)pInputWords;
		BYTE* pOutputBytes		= (BYTE*)pOutputWords;

		while ( nLength-- )
		{
			*pOutputBytes++ = *--pInputBytes;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer read helper

BOOL CBuffer::Read(void* pData, DWORD nLength)
{
	if ( nLength > m_nLength ) return FALSE;
	
	CopyMemory( pData, m_pBuffer, nLength );
	Remove( nLength );
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer read string helper

CString CBuffer::ReadString(DWORD nBytes, UINT nCodePage)
{
	int nSource = (int)min( nBytes, m_nLength );
	int nLength = MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pBuffer, nSource, NULL, 0 );

	LPWSTR pszWide = new WCHAR[ nLength ];
	MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pBuffer, nSource, pszWide, nLength );

	nBytes = WideCharToMultiByte( nCodePage, 0, (LPCWSTR)pszWide, nLength, NULL, 0, NULL, NULL );

	LPSTR pByte = new CHAR[ nBytes ];
	WideCharToMultiByte( nCodePage, 0, (LPCWSTR)pszWide, nLength, pByte, nBytes, NULL, NULL );

	delete [] pszWide;
	CString str( pByte, 0, nBytes );
	delete [] pByte;
	
	Remove( nBytes );
	return str;
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer read line helper

BOOL CBuffer::ReadLine(CString& strLine, UINT nCodePage, BOOL bPeek)
{
	DWORD nLength; for ( nLength = 0 ; nLength < m_nLength ; nLength++ )
	{
		if ( m_pBuffer[ nLength ] == '\n' ) break;
	}

	if ( nLength >= m_nLength ) return FALSE;

	int nWide = MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pBuffer, nLength, NULL, 0 );

	LPWSTR pszWide = new WCHAR[ nWide ];
	MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pBuffer, nLength, pszWide, nWide );
	
	int nBytes = WideCharToMultiByte( nCodePage, 0, (LPCWSTR)pszWide, nWide, NULL, 0, NULL, NULL );

	LPSTR pByte = new CHAR[ nBytes ];
	WideCharToMultiByte( nCodePage, 0, (LPCWSTR)pszWide, nWide, pByte, nBytes, NULL, NULL );

	delete [] pszWide;
	strLine.append( pByte, nBytes);
	delete [] pByte;

	int nCR = strLine.find_last_of( '\r' );
	if ( nCR >= 0 ) strLine = strLine.erase( nCR );

	if ( ! bPeek ) Remove( nLength + 1 );

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer starts with helper

BOOL CBuffer::StartsWith(LPCSTR pszString, BOOL bRemove)
{
	if ( m_nLength < (int)strlen( pszString ) ) return FALSE;

	if ( strncmp( (LPCSTR)m_pBuffer, pszString, strlen( pszString ) ) ) return FALSE;

	if ( bRemove ) Remove( strlen( pszString ) );

	return TRUE;
}
