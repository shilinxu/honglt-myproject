//
// PacketCore.cpp
//

#include "stdafx.h"
#include "PacketCore.h"
#include "Buffer.h"
//#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CPacketCore construction

CPacketCore::CPacketCore(LPCTSTR sCommandId) : CPacket(sCommandId)
{
}

CPacketCore::~CPacketCore()
{
}

//////////////////////////////////////////////////////////////////////
// CPacketCore new packet from buffer

CPacket* CPacketCore::ReadBuffer(BYTE* pBuffer, DWORD nLength)
{
	CPacket* pPacket = NULL;
	
	pPacket->Release(); pPacket = new CBindPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
#if 1
	pPacket->CommandId = "HND";
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
#endif
	
	pPacket->Release(); pPacket = new CDialPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CRingPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
#if 1
	pPacket->CommandId = "CTC";
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
#endif
	
	pPacket->Release(); pPacket = new CTalkPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CRedPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CDeliverPacket( "" );
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CRecordPacket( -1 );
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CListenPacket( -1 );
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CWaitPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CWalkPacket( "" );
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CHaltPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CAchnPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CEchnPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CHandskPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

#if 0
	pPacket->Release(); pPacket = new CSyncTrunkView;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new CSyncLineState;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
#endif
	
	pPacket->Release();
	return CPacket::ReadBuffer(pBuffer, nLength);
}

//////////////////////////////////////////////////////////////////////
// CBindPacket construction

CBindPacket::CBindPacket(LPCTSTR xUserId, LPCTSTR xPassword) : CPacketCore( ICore_BindLink )
{
	ZeroMemory( LocalId, 21 ); ZeroMemory( RemoteId, 21 );
	ZeroMemory( CallType, 16 ); ZeroMemory( FeeNumber, 21 );
	
	if ( xUserId ) CopyMemory( UserId, xUserId, 16 );
	if ( xPassword ) CopyMemory( Password, xPassword, 16 );
}

CBindPacket::CBindPacket(LPCTSTR xFeeCode) : CPacketCore( ICore_BindLink )
{
	ZeroMemory( LocalId, 21 ); ZeroMemory( RemoteId, 21 );
	ZeroMemory( CallType, 16 ); ZeroMemory( FeeNumber, 21 );
	
	if ( xFeeCode ) SetFeeCode( xFeeCode );
}

CBindPacket::~CBindPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CBindPacket operations

int CBindPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, CallType, 16 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, LocalId, 21 );
	nRead += nField; ptr += nField;

	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, RemoteId, 21 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, FeeNumber, 21 );
	nRead += nField; ptr += nField;
	
	return BufferSize = nRead;
}

int CBindPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_BindLink ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	if ( _tcslen(CallType) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( CallType, min( _tcslen(CallType), 16 ) );
	}
	
	if ( _tcslen(LocalId) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( LocalId, min( _tcslen(LocalId), 21 ) );
	}
	
	if ( _tcslen(RemoteId) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( RemoteId, min( _tcslen(RemoteId), 21 ) );
	}
	
	if ( _tcslen(FeeNumber) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( FeeNumber, min( _tcslen(FeeNumber), 21 ) );
	}
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CDialPacket construction

CDialPacket::CDialPacket() : CPacketCore( ICore_DialRemote )
{
	ZeroMemory( LocalId, 21 ); ZeroMemory( RemoteId, 21 );
	ZeroMemory( FeeNumber, 21 ); ZeroMemory( CallerId, 21 );
}

CDialPacket::~CDialPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CDialPacket operations

int CDialPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, RemoteId, 21 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, LocalId, 21 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, CallerId, 21 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, FeeNumber, 21 );
	nRead += nField; ptr += nField;
	
	return BufferSize = nRead;
}

int CDialPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_DialRemote ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	pBuffer->Add( (void*)" ", 1 );
	if ( ! _tcslen(RemoteId) ) RemoteId[0] = 'P';
	pBuffer->Add( RemoteId, min( _tcslen(RemoteId), 21 ) );
	
	pBuffer->Add( (void*)" ", 1 );
	if ( ! _tcslen(LocalId) ) LocalId[0] = 'P';
	pBuffer->Add( LocalId, min( _tcslen(LocalId), 21 ) );
	
	pBuffer->Add( (void*)" ", 1 );
	if ( ! _tcslen(CallerId) ) CallerId[0] = 'P';
	pBuffer->Add( CallerId, min( _tcslen(CallerId), 21 ) );
	
	if ( _tcslen(FeeNumber) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( FeeNumber, min( _tcslen(FeeNumber), 21 ) );
	}
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CRingPacket construction

CRingPacket::CRingPacket(LPCTSTR xLocalId, LPCTSTR xRemoteId, int nLineId) : CPacketCore( ICore_Ringing )
{
	int nLength = xLocalId ? _tcslen( xLocalId ) : 0; nLength = min( nLength, 21 );
	CopyMemory( LocalId, xLocalId, nLength );
	ZeroMemory( LocalId + nLength, 21 - nLength );
	
	nLength = xRemoteId ? _tcslen( xRemoteId ) : 0; nLength = min( nLength, 21 );
	CopyMemory( RemoteId, xRemoteId, nLength );
	ZeroMemory( RemoteId + nLength, 21 - nLength );
	
	LineId	= nLineId;
}

CRingPacket::CRingPacket() : CPacketCore( ICore_Ringing )
{
	ZeroMemory( LocalId, 21 ); ZeroMemory( RemoteId, 21 );
	LineId	= -1;
}

CRingPacket::~CRingPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CRingPacket operations

int CRingPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	CHAR sField[10];
	
#if 1
	if ( IsCommandId( "CTC" ) )
	{
		LocalId[0] = 'P'; LocalId[1] = '\0';
		RemoteId[0] = 'P'; RemoteId[1] = '\0';
		
		for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
		int nField = ReadField( ptr, sField, 10 );
		_stscanf( sField, "%i", &LineId );
		nRead += nField; ptr += nField;
		
		return BufferSize = nRead;
	}
#endif
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, LocalId, 21 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, RemoteId, 21 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &LineId );
	nRead += nField; ptr += nField;
	
	return BufferSize = nRead;
}

int CRingPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_Ringing ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	pBuffer->Add( (void*)" ", 1 );
	if ( ! _tcslen(LocalId) ) LocalId[0] = 'P';
	pBuffer->Add( LocalId, min( _tcslen(LocalId), 21 ) );

#if 1
	if ( IsCommandId( "CTC" ) )
	{
		pBuffer->Print( "\r\n" );
		
		BufferSize = pBuffer->m_nLength - nLength;
		return BufferSize;
	}
#endif
	
	pBuffer->Add( (void*)" ", 1 );
	if ( ! _tcslen(RemoteId) ) RemoteId[0] = 'P';
	pBuffer->Add( RemoteId, min( _tcslen(RemoteId), 21 ) );
	
	if ( LineId != -1 )
	{
		CHAR sField[10];
		pBuffer->Add( (void*)" ", 1 );
		_stprintf( sField, "%i", LineId );
		pBuffer->Add( sField, _tcslen(sField) );
	}
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CTalkPacket construction

CTalkPacket::CTalkPacket() : CPacketCore( ICore_TalkBegin )
{
}

CTalkPacket::~CTalkPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CRedPacket construction

CRedPacket::CRedPacket(LPCTSTR xMsgType, int nMinutes) : CPacketCore( ICore_RedLink )
{
	int nLength = xMsgType ? _tcslen( xMsgType ) : 0; nLength = min( nLength, 16 );
	CopyMemory( MsgType, xMsgType, nLength );
	ZeroMemory( MsgType + nLength, 16 - nLength );
	
	Minutes		= nMinutes;
}

CRedPacket::~CRedPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CRedPacket operations

int CRedPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	CHAR sField[10];
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, MsgType, 10, ',' );
	nRead += nField; ptr += nField;
	
	if ( (*ptr) == ',' ) { nRead++; ptr++; }
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &Minutes );
	nRead += nField; ptr += nField;
	
	//if ( (*ptr) == ',' ) { nRead++; ptr++; }
	
	return BufferSize = nRead;
}

int CRedPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_RedLink ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( MsgType, min( _tcslen(MsgType), 16 ) );
	
	pBuffer->Add( (void*)",", 1 );
	_stprintf( sField, "%i", Minutes );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CDeliverPacket construction

CDeliverPacket::CDeliverPacket(LPCTSTR xMsgType, LPCTSTR xContent) : CPacketCore( ICore_Deliver )
{
	int nLength = xMsgType ? _tcslen( xMsgType ) : 0; nLength = min( nLength, 16 );
	CopyMemory( MsgType, xMsgType, nLength );
	ZeroMemory( MsgType + nLength, 16 - nLength );
	
	nLength = xContent ? _tcslen( xContent ) : 0; nLength = min( nLength, 1024 );
	CopyMemory( MsgContent, xContent, nLength );
	ZeroMemory( MsgContent + nLength, 1024 - nLength );
}

CDeliverPacket::~CDeliverPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CDeliverPacket operations

int CDeliverPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, MsgType, 16 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, MsgContent, 1024, '\0' );
	nRead += nField; ptr += nField;
	
	return BufferSize = nRead;
}

int CDeliverPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_Deliver ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( MsgType, min( _tcslen(MsgType), 16 ) );
	
	if ( _tcslen(MsgContent) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( MsgContent, min( _tcslen(MsgContent), 1024 ) );
	}
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CRecordPacket construction

CRecordPacket::CRecordPacket(int nChan, LPCTSTR xMsgType) : CPacketCore( ICore_RecordFile )
{
	int nLength = xMsgType ? _tcslen( xMsgType ) : 0; nLength = min( nLength, 16 );
	CopyMemory( MsgType, xMsgType, nLength );
	ZeroMemory( MsgType + nLength, 16 - nLength );
	
	this->Channel	= nChan;
}

CRecordPacket::~CRecordPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CRecordPacket operations

int CRecordPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	CHAR sField[10];
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &Channel );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, MsgType, 16 );
	nRead += nField; ptr += nField;
	
	return BufferSize = nRead;
}

int CRecordPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_RecordFile ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", Channel );
	pBuffer->Add( sField, _tcslen(sField) );
	
	if ( _tcslen(MsgType) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( MsgType, min( _tcslen(MsgType), 16 ) );
	}
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CListenPacket construction

CListenPacket::CListenPacket(int nChan, LPCTSTR xMsgType) : CPacketCore( ICore_Listen )
{
	int nLength = xMsgType ? _tcslen( xMsgType ) : 0; nLength = min( nLength, 16 );
	CopyMemory( MsgType, xMsgType, nLength );
	ZeroMemory( MsgType + nLength, 16 - nLength );
	
	this->Channel	= nChan;
}

CListenPacket::~CListenPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CListenPacket operations

int CListenPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	CHAR sField[10];
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &Channel );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, MsgType, 16 );
	nRead += nField; ptr += nField;
	
	return BufferSize = nRead;
}

int CListenPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_Listen ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", Channel );
	pBuffer->Add( sField, _tcslen(sField) );
	
	if ( _tcslen(MsgType) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( MsgType, min( _tcslen(MsgType), 16 ) );
	}
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CWaitPacket construction

CWaitPacket::CWaitPacket(LPCTSTR xMsgType) : CPacketCore( ICore_Waiting )
{
	int nLength = xMsgType ? _tcslen( xMsgType ) : 0; nLength = min( nLength, 16 );
	CopyMemory( MsgType, xMsgType, nLength );
	ZeroMemory( MsgType + nLength, 16 - nLength );
}

CWaitPacket::~CWaitPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CWaitPacket operations

int CWaitPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, MsgType, 16 );
	nRead += nField; ptr += nField;
	
	return BufferSize = nRead;
}

int CWaitPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_Waiting ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	if ( _tcslen(MsgType) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( MsgType, min( _tcslen(MsgType), 16 ) );
	}
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CWalkPacket construction

CWalkPacket::CWalkPacket(LPCTSTR xWalkId) : CPacketCore( ICore_Walker )
{
	int nLength = xWalkId ? _tcslen( xWalkId ) : 0; nLength = min( nLength, 21 );
	CopyMemory( WalkId, xWalkId, nLength );
	ZeroMemory( WalkId + nLength, 21 - nLength );
}

CWalkPacket::~CWalkPacket()
{
}

int CWalkPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, WalkId, 21 );
	nRead += nField; ptr += nField;
	
	return BufferSize = nRead;
}

int CWalkPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_Walker ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( WalkId, min( _tcslen(WalkId), 21 ) );
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CHaltPacket construction

CHaltPacket::CHaltPacket(int nLinkId) : CPacketCore( ICore_HaltLink ), LinkId( nLinkId )
{
}

CHaltPacket::~CHaltPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CHaltPacket operations

int CHaltPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	CHAR sField[10];
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &LinkId );
	nRead += nField; ptr += nField;
	
	return BufferSize = nRead;
}

int CHaltPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_HaltLink ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", LinkId );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CAchnPacket construction

CAchnPacket::CAchnPacket(int nChType, int nChFirst, int nChLength) : CPacketCore( ICore_AddLink )
{
	ChType		= nChType;
	ChFirst		= nChFirst;
	ChLength	= nChLength;
}

CAchnPacket::CAchnPacket() : CPacketCore( ICore_AddLink )
{
}

CAchnPacket::~CAchnPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CAchnPacket operations

int CAchnPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	CHAR sField[10];
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &ChType );
	nRead += nField + 1; ptr += nField + 1;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &ChFirst );
	nRead += nField + 1; ptr += nField + 1;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &ChLength );
	nRead += nField + 1; ptr += nField + 1;
	
	return BufferSize = nRead;
}

int CAchnPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_AddLink ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", ChType );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Add( (void*)",", 1 );
	_stprintf( sField, "%i", ChFirst );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Add( (void*)",", 1 );
	_stprintf( sField, "%i", ChLength );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CEchnPacket construction

CEchnPacket::CEchnPacket(int nChan, int nChState) : CPacketCore( ICore_SyncLink )
{
	Channel		= nChan;
	ChState		= nChState;
}

CEchnPacket::~CEchnPacket()
{
}

//////////////////////////////////////////////////////////////////////
// CEchnPacket operations

int CEchnPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	BYTE* ptr = pBuffer + nRead;
	CHAR sField[10];
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &Channel );
	nRead += nField + 1; ptr += nField + 1;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &ChState );
	nRead += nField + 1; ptr += nField + 1;
	
	return BufferSize = nRead;
}

int CEchnPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_SyncLink ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", Channel );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Add( (void*)",", 1 );
	_stprintf( sField, "%i", ChState );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CHandskPacket construction

CHandskPacket::CHandskPacket() : CPacketCore( ICore_Handshake )
{
}

CHandskPacket::~CHandskPacket()
{
}

#if 0
//////////////////////////////////////////////////////////////////////
// CSyncTrunkView construction

CSyncTrunkView::CSyncTrunkView(CXMLElement* pXML) : CPacketCore( ICore_SyncTrunkView )
{
	m_pXML = pXML ? pXML->Clone() : NULL;
}

CSyncTrunkView::~CSyncTrunkView()
{
	if ( m_pXML ) delete m_pXML;
}

//////////////////////////////////////////////////////////////////////
// CSyncTrunkView operations

int CSyncTrunkView::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	DWORD nRead = CPacketCore::LoadPacket( pBuffer, nLength );
	if ( nRead == -1 ) return -1;
	
	CHAR szTemp[16];
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, szTemp, 10 );
	nRead += nField; ptr += nField;
	
	ASSERT( m_pXML == NULL );
	DWORD nXMLLength = _ttoi( szTemp );
	m_pXML = CXMLElement::FromBytes( pBuffer + nRead + 2, nXMLLength, FALSE );
	
	if ( nXMLLength && m_pXML == NULL ) CommandId = NULL;
	BufferSize = nRead + 2 + (m_pXML ? nXMLLength : 0);
	return BufferSize;
}

int CSyncTrunkView::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_SyncTrunkView ) );
	DWORD nLength = pBuffer->m_nLength;
	
	if ( m_pXML )
	{
		CString strXML =
		m_pXML->ToString( FALSE, TRUE );
		
		CString strValue;
		strValue.Format( "%s %i\r\n", CommandId, strXML.GetLength() );
		pBuffer->Print( (LPCTSTR)strValue );
		
		pBuffer->Print( (LPCTSTR)strXML );
		pBuffer->Print( "\r\n" );
	}
	else
	{
		pBuffer->Print( CommandId );
		pBuffer->Print( "\r\n" );
	}
	
	BufferSize = pBuffer->m_nLength - nLength;
	return BufferSize;
}

//////////////////////////////////////////////////////////////////////
// CSyncLineState construction

CSyncLineState::CSyncLineState(int nChan, int nChState) : CPacketCore( ICore_SyncLineState )
{
	m_nChannel	= nChan;
	m_nChState	= nChState;
}

CSyncLineState::~CSyncLineState()
{
}

//////////////////////////////////////////////////////////////////////
// CSyncLineState operations

int CSyncLineState::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &m_nChannel );
	nRead += nField; ptr += nField + 1;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &m_nChState );
	nRead += nField; ptr += nField;
	
	if ( *( ptr ) != '\r' ) return -1;
	
	return nRead + 1;
}

int CSyncLineState::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICore_SyncLineState ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", m_nChannel );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Add( (void*)",", 1 );
	_stprintf( sField, "%i", m_nChState );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}
#endif
