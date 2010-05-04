//
// Buffer.cpp
//

#include "stdafx.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define BLOCK_SIZE			2048
#define BLOCK_MASK			0xFFFFFC00

///////////////////////////////////////////////////////////////////////////////
// CBuffer construction

CBuffer::CBuffer(DWORD* pLimit)
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

// This method is static, which means you can call it like CBuffer::ReverseBuffer() without having a CBuffer object at all
// Takes pointers to input memory and an output buffer, and a length, which is both the memory in input and the space in output
// Copies the bytes from input to output, but in reverse order
void CBuffer::ReverseBuffer(const void* pInput, void* pOutput, DWORD nLength)
{
	// Point pInputWords at the end of the input memory block
	const DWORD* pInputWords = (const DWORD*)( (const BYTE*)pInput + nLength ); // This is a DWORD pointer, so it will move in steps of 4

	// Point pOutputWords at the start of the output buffer
	DWORD* pOutputWords      = (DWORD*)( pOutput );

	// Make a new local DWORD called nTemp, and request that Visual Studio place it in a machine register
	register DWORD nTemp; // The register keyword asks that nTemp be a machine register, making it really fast

	// Loop while nLength is bigger than 4, grabbing bytes 4 at a time and reversing them
	while ( nLength > 4 )
	{
		// Move pInputWords back 4 bytes, then copy the 4 bytes there into nTemp, the fast machine register DWORD
		nTemp = *--pInputWords;

		// Have SWAP_LONG reverse the order of the 4 bytes, copy them under pOutputWords, and then move that pointer 4 bytes forward
		*pOutputWords++ = SWAP_LONG( nTemp ); // If nTemp is "ABCD", SWAP_LONG( nTemp ) will be "DCBA", bit order is not changed

		// We've just reverse 4 bytes, subtract the length to reflect this
		nLength -= 4;
	}

	// If there are still some input bytes to add reversed
	if ( nLength )
	{
		// Point pInputBytes and pOutputBytes at the same places
		const BYTE* pInputBytes	= (const BYTE*)pInputWords; // This is a byte pointer, so it will move in steps of 1
		BYTE* pOutputBytes		= (BYTE*)pOutputWords;

		// Loop until there are no more bytes to copy over
		while ( nLength-- )
		{
			// Move pInputBytes back to grab a byte, copy it under pOutputBytes, then move pOutputBytes forward
			*pOutputBytes++ = *--pInputBytes;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer read string helper

CString CBuffer::ReadString(DWORD nBytes, UINT nCodePage)
{
	CString str;
	
	int nSource = (int)min( nBytes, m_nLength );
	int nLength = MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pBuffer, nSource, NULL, 0 );
#ifdef _UNICODE
	MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pBuffer, nSource, str.GetBuffer( nLength ), nLength );
	str.ReleaseBuffer( nLength );
#else
	LPWSTR pszWide = new WCHAR[ nLength + 1 ];
	MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pBuffer, nSource, pszWide, nLength );
	pszWide[ nLength ] = 0;
	str = pszWide;
	delete [] pszWide;
#endif
	
	return str;
}

BOOL CBuffer::ReadString(CString& strLine, BOOL bPeek)
{
	strLine.Empty();
	if ( ! m_nLength ) return FALSE;
	
	for ( DWORD nLength = 0 ; nLength < m_nLength ; nLength++ )
	{
		if ( m_pBuffer[ nLength ] == '\n' ) break;
	}
	
	if ( nLength >= m_nLength ) return FALSE;

	CopyMemory( strLine.GetBuffer( nLength ), m_pBuffer, nLength );
	strLine.ReleaseBuffer( nLength );
	
	if ( ! bPeek ) Remove( nLength + 1 );
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer read line helper
// Takes access to a string, default peek false to move a line from the buffer to the string, and default CP_ACP to read ASCII text
// Looks for bytes like "line\r\n" in the buffer, and moves them from the buffer to the string, throwing away the "\r\n" part
// Returns true if a line was found and moved from the buffer to the string, false if there isn't a '\n' in the buffer right now
BOOL CBuffer::ReadLine(CString& strLine, BOOL bPeek, UINT nCodePage)
{
	// Empty the string, making it blank
	strLine.Empty();

	// If this buffer is empty, tell the caller we didn't find a complete line
	if ( ! m_nLength ) return FALSE;

	// Scan down each byte in the buffer
	DWORD nLength;
	for ( nLength = 0 ; nLength < m_nLength ; nLength++ )
	{
		// If the byte at this length is the newline character '\n', exit the loop
		if ( m_pBuffer[ nLength ] == '\n' ) break;
	}

	// If the loop didn't find a '\n' and instead stopped because nLength grew to equal m_nLength
	if ( nLength >= m_nLength ) return FALSE; // There isn't an '\n' in the buffer, tell the caller we didn't find a complete line

	// Convert the nLength ASCII characters in the buffer into wide characters in strLine
	int nWide = MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pBuffer, nLength, NULL, 0 );
#ifdef _UNICODE
	MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pBuffer, nLength, strLine.GetBuffer( nWide ), nWide );
	strLine.ReleaseBuffer( nWide );
#else
	LPWSTR pszWide = new WCHAR[ nLength + 1 ];
	MultiByteToWideChar( nCodePage, 0, (LPCSTR)m_pBuffer, nLength, pszWide, nWide );
	pszWide[ nLength ] = 0;
	strLine = pszWide;
	delete [] pszWide;
#endif

	// Find the last carriage return '\r' character in the string
	int nCR = strLine.ReverseFind( '\r' );   // Find the distance to the last \r, "hello\r" would be 5
	if ( nCR >= 0 ) strLine = strLine.Left( nCR ); // Cut the string to that length, like "hello"

	// Now that the line has been copied into the string, remove it and the '\n' from the buffer
	if ( ! bPeek ) Remove( nLength + 1 ); // Unless we're peeking, then leave it in the buffer

	// Report that we found a line and moved it from the buffer to the string
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer starts with helper

// Takes a pointer to ASCII text, and the option to remove these characters from the start of the buffer if they are found there
// Looks at the bytes at the start of the buffer, and determines if they are the same as the given ASCII text
// Returns true if the text matches, false if it doesn't
BOOL CBuffer::StartsWith(LPCSTR pszString, BOOL bRemove)
{
	// If the buffer isn't long enough to contain the given string, report the buffer doesn't start with it
	if ( m_nLength < (int)strlen( pszString ) ) return FALSE;

	// If the first characters in the buffer don't match those in the ASCII string, return false
	if ( strncmp(               // Returns 0 if all the characters are the same
		(LPCSTR)m_pBuffer,      // Look at the start of the buffer as ASCII text
		(LPCSTR)pszString,      // The given text
		strlen( pszString ) ) ) // Don't look too far into the buffer, we know it's long enough to hold the string
		return FALSE;           // If one string would sort above another, the result is positive or negative

	// If we got the option to remove the string if it matched, do it
	if ( bRemove ) Remove( strlen( pszString ) );

	// Report that the buffer does start with the given ASCII text
	return TRUE;
}
