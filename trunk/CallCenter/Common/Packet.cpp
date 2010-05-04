//
// Packet.cpp
//

#include "stdafx.h"
#include "Packet.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CPacket construction

CPacket::CPacket(LPCTSTR sCommandId)
{
	CommandId	= sCommandId;
	BufferSize	= 0;
}

CPacket::~CPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CPacket parsing

int CPacket::ReadField(BYTE* pBuffer, CHAR* sField, int nField, CHAR cPart)
{
	CHAR cRight = '\0';
	BYTE* pLeft = pBuffer;
	
	for ( int nLength = 0; nLength < nField && ! _istcntrl(*pLeft); pLeft++ )
	{
		if ( *( pLeft ) == '[') {cRight = ']'; continue;}
		if ( *( pLeft ) == cRight ) {cRight = '\0'; continue;}
		
		if ( *( pLeft ) == cPart && cRight == '\0' ) break;
		if ( cRight != '\0' && nLength >= nField ) break;
		
		sField[ nLength++ ] = *( pLeft );
	}
	
	if ( nLength < nField ) sField[ nLength ] = '\0';
	
	return nLength;
}

//////////////////////////////////////////////////////////////////////
// CPacket new packet from buffer

CPacket* CPacket::ReadBuffer(BYTE* pBuffer, DWORD nLength)
{
	CPacket* pPacket = NULL;
	
	pPacket->Release(); pPacket = new COkPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CErrPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CByePacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); return NULL;
}

//////////////////////////////////////////////////////////////////////
// CPacket operations

int CPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sCommandId[10];
	
	DWORD nRead = ReadField( pBuffer, sCommandId, 10 );
	if ( ! IsCommandId( sCommandId ) ) return -1;
	
	return BufferSize = nRead;
}

int CPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( CommandId ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// COkPacket construction

COkPacket::COkPacket() : CPacket( IOK )
{
}

COkPacket::~COkPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CErrPacket construction

CErrPacket::CErrPacket(int nErrCode) : CPacket( IERROR )
{
	m_nErrCode	= nErrCode;
}

CErrPacket::~CErrPacket()
{
}

int CErrPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacket::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	CHAR sField[10];
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &m_nErrCode );
	nRead += nField; ptr += nField;
	
	return BufferSize = nRead;
}

int CErrPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( "ERR" ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	pBuffer->Add( (void*)" ", 1 );
	
	_stprintf( sField, "%i", m_nErrCode );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// CByePacket construction

CByePacket::CByePacket() : CPacket( IBYE )
{
}

CByePacket::~CByePacket()
{
}
