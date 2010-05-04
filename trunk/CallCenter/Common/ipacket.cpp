//
// IPacket.cpp
//

#include "stdafx.h"
#include "Buffer.h"
#include "IPacket.h"
#include "md5.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// IPacket construction

IPacket::IPacket(LPCTSTR xCommandId)
{
	CommandId	= xCommandId;
}

IPacket::~IPacket()
{
}

//////////////////////////////////////////////////////////////////////
// IPacket parsing

int IPacket::ReadField(BYTE* pBuffer, CHAR* sField, int nField, CHAR cPart)
{
	for ( int nLength = 0; nLength < nField && *( pBuffer + nLength ) != '\r'; nLength++ )
	{
		if ( *( pBuffer + nLength ) == cPart ) break;
#if 0
		if ( *( pBuffer + nLength ) == '#' ) 
		{
			sField[ nLength++ ] = '\0'; break;
		}
#endif
		sField[ nLength ] = *( pBuffer + nLength );
	}
	
	if ( nLength < nField ) sField[ nLength ] = '\0';

	return nLength;
}

//////////////////////////////////////////////////////////////////////
// IPacket new packet from buffer

IPacket* IPacket::ReadBuffer(BYTE* pBuffer, DWORD nLength)
{
	IPacket* pPacket = NULL;

	//
	
	pPacket->Release(); pPacket = new IOkPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new IErrPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new IExitPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	//

	pPacket->Release(); pPacket = new IBindPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new ITalkPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); pPacket = new ISyncPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new IRedPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	//

	pPacket->Release(); pPacket = new IHandPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new IRingPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new IDialPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new IRoutPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new ICtocPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new IRecdPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new ILisnPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new IHaltPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new IWaitPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	//

	pPacket->Release(); pPacket = new IAchnPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;

	pPacket->Release(); pPacket = new IEchnPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	//
	
	pPacket->Release(); pPacket = new IUserPacket;
	if ( pPacket->LoadPacket( pBuffer, nLength ) != -1 ) return pPacket;
	
	pPacket->Release(); return NULL;
}

//////////////////////////////////////////////////////////////////////
// IOkPacket construction

IOkPacket::IOkPacket() : IPacket( ICP_OK )
{
}

IOkPacket::~IOkPacket()
{
}

int IOkPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR xCommandId[10];
	
	DWORD nRead = ReadField( pBuffer, xCommandId, 10 );
	if ( ! IsCommandId( xCommandId ) ) return -1;

	if ( *( pBuffer + nRead ) != '\r' ) return -1;

	return nRead + 1;
}

int IOkPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_OK ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IErrPacket construction

IErrPacket::IErrPacket(int nErrCode) : IPacket( ICP_ERR )
{
	ErrCode	= nErrCode;
}

IErrPacket::~IErrPacket()
{
}

int IErrPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &ErrCode );
	nRead += nField; ptr += nField;

	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int IErrPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_ERR ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", ErrCode );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IBindPacket construction

IBindPacket::IBindPacket() : IPacket( ICP_BIND )
{
	ZeroMemory( CallType, 4 );

	ZeroMemory( LocalId, 21 );
	ZeroMemory( RemoteId, 21 );
	ZeroMemory( FeeNumber, 21 );
}

IBindPacket::~IBindPacket()
{
}

//////////////////////////////////////////////////////////////////////
// IBindPacket load and encode

int IBindPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR xCommandId[10];
	
	DWORD nRead = ReadField( pBuffer, xCommandId, 10 );
	if ( ! IsCommandId( xCommandId ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, CallType, 4 );
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

	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int IBindPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_BIND ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	if ( _tcslen(CallType) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( CallType, min( _tcslen(CallType), 4 ) );
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
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IExitPacket construction

IExitPacket::IExitPacket() : IPacket( ICP_EXIT )
{
}

IExitPacket::~IExitPacket()
{
}

int IExitPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR xCommandId[10];
	
	DWORD nRead = ReadField( pBuffer, xCommandId, 10 );
	if ( ! IsCommandId( xCommandId ) ) return -1;

	if ( *( pBuffer + nRead ) != '\r' ) return -1;

	return nRead + 1;
}

int IExitPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_EXIT ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// ITalkPacket construction

ITalkPacket::ITalkPacket() : IPacket( ICP_TALK )
{
}

ITalkPacket::~ITalkPacket()
{
}

int ITalkPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR xCommandId[10];
	
	DWORD nRead = ReadField( pBuffer, xCommandId, 10 );
	if ( ! IsCommandId( xCommandId ) ) return -1;

	if ( *( pBuffer + nRead ) != '\r' ) return -1;

	return nRead + 1;
}

int ITalkPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_TALK ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IRedPacket construction

IRedPacket::IRedPacket(int nMinutes) : IPacket( ICP_RED )
{
	SetMinutes( nMinutes );
}

IRedPacket::~IRedPacket()
{
}

int IRedPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10, ',' );
	VERIFY( _tcscmp( sField, "MIN" ) == 0 );
	nRead += nField; ptr += nField + 1;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &Minutes );
	nRead += nField; ptr += nField;
	
	if ( *( ptr ) != '\r' ) return -1;
	
	return nRead + 1;
}

int IRedPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_RED ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( (void*)"MIN", 3 );
	
	pBuffer->Add( (void*)",", 1 );
	_stprintf( sField, "%i", Minutes );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IHaltPacket construction

IHaltPacket::IHaltPacket(int nChannel) : IPacket( ICP_HALT )
{
	Channel	= nChannel;
}

IHaltPacket::~IHaltPacket()
{
}

int IHaltPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &Channel );
	nRead += nField; ptr += nField;
	
	if ( *( ptr ) != '\r' ) return -1;
	
	return nRead + 1;
}

int IHaltPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_HALT ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	if ( Channel != -1 )
	{
		CHAR sField[10];
		
		pBuffer->Add( (void*)" ", 1 );
		_stprintf( sField, "%i", Channel );
		pBuffer->Add( sField, _tcslen(sField) );
	}
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// ISyncPacket construction

ISyncPacket::ISyncPacket(double nMoney, int nRepeat) : IPacket( ICP_SYNC )
{
	SyncMoney	= nMoney;
	SyncRepeat	= nRepeat;
}

ISyncPacket::~ISyncPacket()
{
}

int ISyncPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%lf", &SyncMoney );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &SyncRepeat );
	nRead += nField; ptr += nField;
	
	if ( *( ptr ) != '\r' ) return -1;
	
	return nRead + 1;
}

int ISyncPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_SYNC ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%0.2lf", SyncMoney );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", SyncRepeat );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IHandPacket construction

IHandPacket::IHandPacket() : IPacket( ICP_HAND )
{
	ZeroMemory( ClientId, 8 );
}

IHandPacket::~IHandPacket()
{
}

//////////////////////////////////////////////////////////////////////
// IHandPacket load and encode

int IHandPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR xCommandId[10];
	
	DWORD nRead = ReadField( pBuffer, xCommandId, 10 );
	if ( ! IsCommandId( xCommandId ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, ClientId, 8 );
	nRead += nField; ptr += nField;
	
	if ( *( ptr ) != '\r' ) return -1;
	
	return nRead + 1;
}

int IHandPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_HAND ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( ClientId, min( _tcslen(ClientId), 8 ) );
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IRingPacket construction

IRingPacket::IRingPacket() : IPacket( ICP_RING )
{
	ZeroMemory( CallerId, 21 );
	ZeroMemory( PhonNumber, 21 );
	ChNumber	= -1;
}

IRingPacket::~IRingPacket()
{
}

//////////////////////////////////////////////////////////////////////
// IRingPacket load and encode

int IRingPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR xCommandId[10];
	
	DWORD nRead = ReadField( pBuffer, xCommandId, 10 );
	if ( ! IsCommandId( xCommandId ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, CallerId, 21 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, PhonNumber, 21 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, xCommandId, 10 );
	_stscanf( xCommandId, "%i", &ChNumber );
	nRead += nField; ptr += nField;
	
	if ( *( ptr ) != '\r' ) return -1;
	
	return nRead + 1;
}

int IRingPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_RING ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( CallerId, min( _tcslen(CallerId), 21 ) );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( PhonNumber, min( _tcslen(PhonNumber), 21 ) );
	
	if ( ChNumber != -1 )
	{
		CHAR sField[10];
		pBuffer->Add( (void*)" ", 1 );
		_stprintf( sField, "%i", ChNumber );
		pBuffer->Add( sField, _tcslen(sField) );
	}
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IDialPacket construction

IDialPacket::IDialPacket() : IPacket( ICP_DIAL )
{
	ZeroMemory( LocalId, 21 );
	ZeroMemory( RemoteId, 21 );
	ZeroMemory( CallerId, 21 );
	ZeroMemory( FeeNumber, 21 );
}

IDialPacket::~IDialPacket()
{
}

//////////////////////////////////////////////////////////////////////
// IDialPacket load and encode

int IDialPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR xCommandId[10];
	
	DWORD nRead = ReadField( pBuffer, xCommandId, 10 );
	if ( ! IsCommandId( xCommandId ) ) return -1;
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

	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int IDialPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_DIAL ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( RemoteId, min( _tcslen(RemoteId), 21 ) );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( LocalId, min( _tcslen(LocalId), 21 ) );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( CallerId, min( _tcslen(CallerId), 21 ) );
	
	if ( _tcslen(FeeNumber) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( FeeNumber, min( _tcslen(FeeNumber), 21 ) );
	}

	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IRoutPacket construction

IRoutPacket::IRoutPacket() : IPacket( ICP_ROUT )
{
	ZeroMemory( RemoteId, 21 );
	ZeroMemory( PakParam, 800 );
}

IRoutPacket::~IRoutPacket()
{
}

//////////////////////////////////////////////////////////////////////
// IRoutPacket load and encode

int IRoutPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR xCommandId[10];
#ifdef _DEBUG
//	pBuffer=(BYTE*)"SVR 96031222 V13867877813\r\n";
#endif
	
	DWORD nRead = ReadField( pBuffer, xCommandId, 10 );
	if ( ! IsCommandId( xCommandId ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, RemoteId, 21 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, PakParam, 800, '\0' );
	nRead += nField; ptr += nField;

	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int IRoutPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_ROUT ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( RemoteId, min( _tcslen(RemoteId), 21 ) );
	
	if ( _tcslen(PakParam) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( PakParam, min( _tcslen(PakParam), 800 ) );
	}

	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// ICtocPacket construction

ICtocPacket::ICtocPacket(int nClientId) : IPacket( ICP_CTOC )
{
	ClientId	= nClientId;
}

ICtocPacket::~ICtocPacket()
{
}

int ICtocPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &ClientId );
	nRead += nField; ptr += nField;

	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int ICtocPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_CTOC ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", ClientId );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IRecdPacket construction

IRecdPacket::IRecdPacket(int nChannel) : IPacket( ICP_RECD )
{
	Channel	= nChannel;
	ZeroMemory( PakParam, 16 );
}

IRecdPacket::~IRecdPacket()
{
}

int IRecdPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &Channel );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, PakParam, 16 );
	nRead += nField; ptr += nField;

	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int IRecdPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_RECD ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", Channel );
	pBuffer->Add( sField, _tcslen(sField) );
	
	if ( _tcslen(PakParam) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( PakParam, min( _tcslen(PakParam), 16 ) );
	}
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// ILisnPacket construction

ILisnPacket::ILisnPacket(int nChannel) : IPacket( ICP_LISN )
{
	Channel	= nChannel;
	ZeroMemory( PakParam, 16 );
}

ILisnPacket::~ILisnPacket()
{
}

int ILisnPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10 );
	_stscanf( sField, "%i", &Channel );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, PakParam, 16 );
	nRead += nField; ptr += nField;

	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int ILisnPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_LISN ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", Channel );
	pBuffer->Add( sField, _tcslen(sField) );
	
	if ( _tcslen(PakParam) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( PakParam, min( _tcslen(PakParam), 16 ) );
	}
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IWalkPacket construction

IWalkPacket::IWalkPacket(LPCTSTR xWalkId) : IPacket( ICP_WALK )
{
	int nLength = _tcslen( xWalkId ); nLength = min( nLength, 21 );
	CopyMemory( WalkId, xWalkId, nLength );
	ZeroMemory( WalkId + nLength, 21 - nLength );
}

IWalkPacket::~IWalkPacket()
{
}

int IWalkPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, WalkId, 21 );
	nRead += nField; ptr += nField;

	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int IWalkPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_WALK ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( WalkId, min( _tcslen(WalkId), 21 ) );
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IWaitPacket construction

IWaitPacket::IWaitPacket(LPCTSTR xParam) : IPacket( ICP_WAIT )
{
	ZeroMemory( PakParam, 16 );
}

IWaitPacket::~IWaitPacket()
{
}

int IWaitPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, PakParam, 16 );
	nRead += nField; ptr += nField;

	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int IWaitPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_WAIT ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	if ( _tcslen( PakParam ) )
	{
		pBuffer->Add( (void*)" ", 1 );
		pBuffer->Add( PakParam, min( _tcslen(PakParam), 16 ) );
	}
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IAchnPacket construction

IAchnPacket::IAchnPacket() : IPacket( ICP_ACHN )
{
}

IAchnPacket::IAchnPacket(int nChType, int nChFirst, int nChLength) : IPacket( ICP_ACHN )
{
	ChType		= nChType;
	ChFirst		= nChFirst;
	ChLength	= nChLength;
}

IAchnPacket::~IAchnPacket()
{
}

int IAchnPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &ChType );
	nRead += nField; ptr += nField + 1;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &ChFirst );
	nRead += nField; ptr += nField + 1;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &ChLength );
	nRead += nField; ptr += nField;

	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int IAchnPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_ACHN ) );
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
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IEchnPacket construction

IEchnPacket::IEchnPacket() : IPacket( ICP_ECHN )
{
}

IEchnPacket::IEchnPacket(int nChan, int nChState) : IPacket( ICP_ECHN )
{
	ChNum	= nChan;
	ChState	= nChState;
}

IEchnPacket::~IEchnPacket()
{
}

int IEchnPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR sField[10];
	
	DWORD nRead = ReadField( pBuffer, sField, 10 );
	if ( ! IsCommandId( sField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &ChNum );
	nRead += nField; ptr += nField + 1;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, sField, 10, ',' );
	_stscanf( sField, "%i", &ChState );
	nRead += nField; ptr += nField;
	
	if ( *( ptr ) != '\r' ) return -1;
	
	return nRead + 1;
}

int IEchnPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_ECHN ) );
	DWORD nLength = pBuffer->m_nLength;

	pBuffer->Print( CommandId );
	
	CHAR sField[10];
	
	pBuffer->Add( (void*)" ", 1 );
	_stprintf( sField, "%i", ChNum );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Add( (void*)",", 1 );
	_stprintf( sField, "%i", ChState );
	pBuffer->Add( sField, _tcslen(sField) );
	
	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}

//////////////////////////////////////////////////////////////////////
// IUserPacket construction

IUserPacket::IUserPacket(LPCTSTR xUserId, LPCTSTR xPass) : IPacket( ICP_USER )
{
	MD5 ctx; BYTE xMD5Hash[16];
	ctx.update( (LPBYTE)xPass, _tcslen(xPass) );
	ctx.finalize(); ctx.raw_digest( xMD5Hash );
	
	CopyMemory( UserId, xUserId, 21 );
	ctx.tostring( (LPBYTE)MD5Hashing );
}

IUserPacket::IUserPacket() : IPacket( ICP_USER )
{
	ZeroMemory( UserId, 21 );
	ZeroMemory( MD5Hashing, 32 );
}

IUserPacket::~IUserPacket()
{
}

//////////////////////////////////////////////////////////////////////
// IUserPacket load and encode

int IUserPacket::LoadPacket(BYTE* pBuffer, DWORD nLength)
{
	CHAR xField[32];
	
	DWORD nRead = ReadField( pBuffer, xField, 10 );
	if ( ! IsCommandId( xField ) ) return -1;
	BYTE* ptr = pBuffer + nRead;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	int nField = ReadField( ptr, UserId, 21 );
	nRead += nField; ptr += nField;
	
	for ( ; (*ptr) == ' ' && nRead < nLength; ptr++, nRead++ );
	nField = ReadField( ptr, MD5Hashing, 32 ); 
	nRead += nField; ptr += nField;
	
	if ( *( ptr ) != '\r' ) return -1;

	return nRead + 1;
}

int IUserPacket::EncodeBody(CBuffer *pBuffer)
{
	ASSERT( IsCommandId( ICP_USER ) );
	DWORD nLength = pBuffer->m_nLength;
	
	pBuffer->Print( CommandId );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( UserId, min( _tcslen(UserId), 21 ) );
	
	pBuffer->Add( (void*)" ", 1 );
	pBuffer->Add( MD5Hashing, 32 );

	pBuffer->Print( "\r\n" );
	
	return pBuffer->m_nLength - nLength;
}
