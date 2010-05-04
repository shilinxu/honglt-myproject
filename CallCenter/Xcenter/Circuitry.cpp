//
// Circuitry.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Circuitry.h"

#include "Neighbour.h"
#include "Network.h"
#include "Settings.h"
#include "Channel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TEMP_BUFFER			192
#define MASK_BUFFER			4000

//////////////////////////////////////////////////////////////////////
// CCircuitry construction

CCircuitry::CCircuitry(LPCTSTR xUserId) : m_xUserId( _tcsdup(xUserId) )
{
	m_nTaskId		= -1;
	m_pChain		= NULL;
	m_pBuffer		= NULL;
	
	m_pBufferIn		= NULL;
	m_nPlyCookie	= 0;
	
	m_pRecIoOver	= NULL;
	m_pPlayIoOver	= NULL;
}

CCircuitry::CCircuitry(HANDLE hPipe) : CInterlink( hPipe )
{
	CString strTemp;
	srand( GetTickCount() );
	
	strTemp.Format( _T("#%i"), rand() );
	m_xUserId		= _tcsdup( strTemp );
	
	m_pChain		= NULL;
	m_pBuffer		= NULL;
	
	m_pBufferIn		= NULL;
	m_nPlyCookie	= 0;
	m_pRecIoOver	= NULL;
	m_pPlayIoOver	= NULL;
}

CCircuitry::~CCircuitry()
{
	FreeBuffer();
	
	if ( m_pChain ) delete m_pChain;
	free( m_xUserId ); m_xUserId = NULL;
}

//////////////////////////////////////////////////////////////////////
// CCircuitry attributes

void CCircuitry::FreeBuffer()
{
	if ( m_pBuffer ) delete[] m_pBuffer;
	m_pBuffer	= NULL;
	
	if ( m_pBufferIn ) delete[] m_pBufferIn;
	m_pBufferIn	= NULL;
	
	m_nPlyCookie= 0;
	
	if ( m_pRecIoOver ) CloseHandle( m_pRecIoOver->hEvent );
	if ( m_pRecIoOver ) delete m_pRecIoOver;
	m_pRecIoOver = NULL;
	
	if ( m_pPlayIoOver ) CloseHandle( m_pPlayIoOver->hEvent );
	if ( m_pPlayIoOver ) delete m_pPlayIoOver;
	m_pPlayIoOver = NULL;
}

void CCircuitry::NewBuffer(int nChan)
{
	FreeBuffer();
	
	m_pBuffer	= new BYTE [TEMP_BUFFER];
	m_pBufferIn	= new BYTE[TEMP_BUFFER][MAX_PLAYLST];
	
	m_pRecIoOver= new OVERLAPPED;
	ZeroMemory( m_pRecIoOver, sizeof(OVERLAPPED) );
	
	m_pRecIoOver->hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_pRecIoOver->InternalHigh	= (DWORD)this;
	m_pRecIoOver->OffsetHigh	= nChan;
	
	SetEvent( m_pRecIoOver->hEvent );
	
	m_pPlayIoOver	= new OVERLAPPED;
	ZeroMemory( m_pPlayIoOver, sizeof(OVERLAPPED) );
	
	m_pPlayIoOver->hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_pPlayIoOver->InternalHigh	= (DWORD)this;
	m_pPlayIoOver->OffsetHigh	= nChan;
	
	SetEvent( m_pPlayIoOver->hEvent );
}

void CCircuitry::Clone(CCircuitry* pParent)
{
	this->m_pChain	= pParent->m_pChain;
	pParent->m_pChain = NULL;
	
	if ( pParent->m_hPipe != INVALID_HANDLE_VALUE )
	{
		this->AttachTo( pParent );
	}
}

BOOL CCircuitry::Start(int nChan, BOOL bRebind)
{
	NewBuffer( nChan );
	RecordWith( nChan, m_pBuffer );
	ReplayWith( nChan, m_pChain );
	
	m_nTaskId = nChan;
	if ( ! bRebind ) return TRUE;
	
	NETRESOURCE ns;
	
	ns.dwScope = RESOURCE_CONNECTED;   
	ns.dwType = RESOURCETYPE_ANY; 
	ns.dwDisplayType = RESOURCEDISPLAYTYPE_GENERIC;
	ns.dwUsage = RESOURCEUSAGE_CONNECTABLE;
	
	ns.lpRemoteName	= Settings.Centric.Remote.GetBuffer(0);
	ns.lpLocalName = NULL;
	ns.lpProvider = NULL;
	
	WNetAddConnection2( &ns, Settings.Centric.Password, Settings.Centric.UserId, 0 );
	
	m_pChain->Release();
	m_pChain = new CInterlink;
	
	CString strPipe = Settings.Centric.Remote;
	strPipe += '.'; strPipe += m_xUserId;
	
	if ( ! m_pChain->ConnectTo( strPipe, 0 ) ) return Reset();
	
	return TRUE;
}

BOOL CCircuitry::Reset()
{
	if ( m_pRecIoOver ) ResetRecord( m_pRecIoOver );
	if ( m_pPlayIoOver ) ResetReplay( m_pPlayIoOver );
	
	const HANDLE hEvent[] =
	{ m_pRecIoOver ? m_pRecIoOver->hEvent : NULL, m_pPlayIoOver? m_pPlayIoOver->hEvent: NULL };
	
	WaitForMultipleObjects( 2, hEvent, TRUE, INFINITE );
	SsmHangup( m_nTaskId ); m_nTaskId = -1;
	
	m_pChain->Release();
	m_pChain = new CInterlink;
	
	// ´ý»ú
	
	CString strPipe = Settings.Centric.Local;
	strPipe += '.'; strPipe += m_xUserId;
	
	m_pChain->Connect( strPipe );
	
	FreeBuffer();
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CCircuitry disconnect

void CCircuitry::Disconnect()
{
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	Neighbour.m_pList.RemoveKey( m_xUserId );
	if ( m_pChain )	this->Reset();
	
	CInterlink::Disconnect(); delete this;
}

int CCircuitry::ResetRecord(OVERLAPPED*	pRecIoOver)
{
	pRecIoOver->Internal = STATUS_PENDING;
	if ( pRecIoOver->OffsetHigh == -1 ) return 0;
	
	return SsmStopRecordMemBlock( pRecIoOver->OffsetHigh );
}

int CCircuitry::ResetReplay(OVERLAPPED* pPlayIoOver)
{
	pPlayIoOver->Internal = STATUS_PENDING;
	if ( pPlayIoOver->OffsetHigh == -1 ) return 0;
	
	return SsmStopPlayMemBlock( pPlayIoOver->OffsetHigh );
}

//////////////////////////////////////////////////////////////////////
// CCircuitry run function

BOOL CCircuitry::OnRun()
{
	if ( ! CInterlink::OnRun() ) return FALSE;
	
	if ( m_pChain && ! m_pChain->OnRun() ) Reset();
	
	if ( ! m_pChain || ! m_pChain->IsConnected() ) return TRUE;
	
	if ( m_pRecIoOver && m_pRecIoOver->OffsetHigh != -1 )
	RecordWith( m_pRecIoOver->OffsetHigh, m_pBuffer );
	
	if ( m_pPlayIoOver && m_pPlayIoOver->OffsetHigh != -1 )
	ReplayWith( m_pPlayIoOver->OffsetHigh, m_pChain );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CCircuitry pipe event handlers

BOOL CCircuitry::OnConnected()
{
	CInterlink::OnConnected();
	
	Reset(); return TRUE;
}

BOOL CCircuitry::OnDropped()
{
	Disconnect(); return FALSE;
}

#ifdef _DEBUG
void CCircuitry::DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader)
{
	CPipeline::DoPrint( pBuffer, nLength, lpszHeader );
}
#else
void CCircuitry::DoPrint(BYTE* pBuffer, DWORD nLength, LPCTSTR lpszHeader)
{
	CString strLine = lpszHeader;
	strLine += ToASCII( (BYTE*)pBuffer, nLength );
	
	theApp.Message( MSG_DEBUG, (LPCTSTR)strLine );
}
#endif
//////////////////////////////////////////////////////////////////////
// CCircuitry packet unserialize

CPacket* CCircuitry::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	
	if ( ! pBuffer->ReadLine( strLine, TRUE ) ) return NULL;
	
	CPacket* pPacket = CPacketCore::ReadBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	int nLength = strLine.GetLength();
	
	if ( pPacket == NULL )
	{
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		SendPacket( new CErrPacket(ECore_Unknown) ); return NULL;
	}
	else
	if ( pPacket->CommandId != NULL )
	{
		nLength = pPacket->GetBufferSize();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		return pPacket;
	}
	
	return pPacket;
}

BOOL CCircuitry::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( ICore_BindLink ) )
	{
		return OnBindPacket( (CBindPacket*)pPacket );
	}
	if ( pPacket->IsCommandId( ICore_DialRemote ) )
	{
		return OnDialPacket( (CDialPacket*)pPacket );
	}
	
	if ( pPacket->IsCommandId( ICore_Ringing ) )
	{
		return OnRingPacket( (CRingPacket*)pPacket );
	}
	if ( pPacket->IsCommandId( ICore_TalkBegin ) )
	{
		return OnTalkPacket( (CTalkPacket*)pPacket );
	}
	
	return CInterlink::OnPacket( pPacket );
}

BOOL CCircuitry::OnBindPacket(CBindPacket* pPacket)
{
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	CHAR xUserId[MAX_PATH + 1];
	xUserId[ pPacket->GetUserId( xUserId ) ] = '\0';
	
	CCircuitry* pChannel = new CCircuitry( xUserId );
	pChannel->Clone( this ); Disconnect();
	
	Neighbour.SetChannel( pChannel );
	
	CHAR xSrvCode[MAX_PATH + 1];
	xSrvCode[ pPacket->GetSrvCode( xSrvCode ) ] = '\0';
	
//	pChannel->FormatRelation( xSrvCode );
	pChannel->SendPacket( new COkPacket );
/*	
	if ( pChannel->m_pList.GetCount() == 0 )
	{
		pChannel->Disconnect(); return FALSE;
	}
*/	
	pChannel->Reset();
	return FALSE;
}

BOOL CCircuitry::OnDialPacket(CDialPacket* pPacket)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CHAR xRemoteId[MAX_PHONE + 1];
	xRemoteId[ pPacket->GetRemoteId( xRemoteId ) ] = '\0';
	
	CChannel* pChannel = Network.LookOutPhs( xRemoteId );
	if ( ! pChannel ) return SendPacket( new CErrPacket( ECore_Busy ) );
	
	int nChan = pChannel->m_nChan;
	SsmSetRecMixer( nChan, FALSE, 0 );
	SsmSetDTMFStopRecCharSet( nChan, "" );
	
	SsmSetDtmfStopPlay( nChan, FALSE );
	
	pChannel->OnDialPacket( pPacket );
	
	NewBuffer( nChan );
	Start( nChan );
	
	return TRUE;
}

BOOL CCircuitry::OnRingPacket(CRingPacket* pPacket)
{
	int nChan = pPacket->GetLineId();
	
	SsmSetRecMixer( nChan, FALSE, 0 );
	SsmSetDTMFStopRecCharSet( nChan, "" );
	
	SsmSetDtmfStopPlay( nChan, FALSE );
	
	NewBuffer( nChan );
	Start( nChan, TRUE );
	
	return SendPacket( pPacket, TRUE );
}

BOOL CCircuitry::OnTalkPacket(CTalkPacket* pPacket)
{
	SsmPutUserEvent( E_CHG_ChState, m_nTaskId,
		MAKEVENT(m_nTaskId & 0xFFFF, S_CALL_TALKING) );
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCircuitry CChannel operations

int CCircuitry::RecordWith(int nChan, BYTE* pBuffer, DWORD dwStopOffset)
{
	if ( ! m_pChain || ! m_pChain->IsConnected() ) return -1;
	
	if ( m_pRecIoOver->Offset >= MAX_RECLST ) return -1;
	if ( ! HasOverlappedIoCompleted(m_pRecIoOver) ) return -1;
	
	CBuffer* pOutput = m_pChain->m_pOutput;
	if ( dwStopOffset ) pOutput->Add( pBuffer, dwStopOffset + 1 );
	
	Neighbour.m_pWakeup.SetEvent();
	
	if ( int nResult = SsmRecordMemBlock(nChan, 6, pBuffer, TEMP_BUFFER, OnRecMemBlockDone, m_pRecIoOver) )
	{
#ifdef _DEBUG
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, sError );
#endif
		return nResult;
	}
	
	m_pRecIoOver->Offset++;
	ResetEvent( m_pRecIoOver->hEvent );
	
//TRACE("%i-> RecordWith in [%i]\n", GetTickCount(), m_pRecIoOver->Offset);
	return 0;
}

BOOL CALLBACK CCircuitry::OnRecMemBlockDone(int ch, int nEndReason, PUCHAR pucBuf, DWORD dwStopOffset, PVOID pV)
{
	enum {
		RecordBy_BlockApp	= 1,
		RecordBy_RcvDtmf	= 2,
		RecordBy_Hangup		= 3,
		RecordBy_Done		= 4,
	};
	
	if ( SsmGetChState(ch) == S_CALL_STANDBY ) nEndReason = RecordBy_Hangup;
	
	OVERLAPPED*	pRecIoOver = (OVERLAPPED*)pV;
//TRACE("%i-> OnRecMemBlockDone in [%i]\n", GetTickCount(), pRecIoOver->Offset);
	
	CSingleLock pLock( &Neighbour.m_pSection );
	for ( ; HasOverlappedIoCompleted(pRecIoOver) && ! pLock.Lock(50); );
	
	if ( ! --pRecIoOver->Offset ) SetEvent( pRecIoOver->hEvent );
	
	CCircuitry* pChannel = (CCircuitry*) pRecIoOver->InternalHigh;
	
	if ( nEndReason == RecordBy_Done && pLock.IsLocked() )
	{
		pChannel->RecordWith( ch, pucBuf, min(dwStopOffset,TEMP_BUFFER) );
	}
	else if ( nEndReason != RecordBy_Done )
	{
		pRecIoOver->OffsetHigh = -1;
		if ( nEndReason == RecordBy_Hangup ) pChannel->m_nTaskId = -1;
	}
	
	return TRUE;
}

int CCircuitry::ReplayWith(int nChan, CInterlink* pChain)
{
	if ( ! pChain || ! pChain->IsConnected() ) return -1;
	
	if ( m_pPlayIoOver->Offset >= MAX_PLAYLST ) return -1;
	if ( ! HasOverlappedIoCompleted(m_pPlayIoOver) ) return -1;
	
	CBuffer* pInput = pChain->m_pInput;
	if ( pInput->m_nLength > MASK_BUFFER ) pInput->Remove( pInput->m_nLength - MASK_BUFFER );
	if ( pInput->m_nLength < TEMP_BUFFER ) return -1;
	
	Neighbour.m_pWakeup.SetEvent();
	m_pPlayIoOver->Internal = STATUS_PENDING;
//TRACE("%i->buffer in[%i], [%i]\n", GetTickCount(), pInput->m_nBuffer,pInput->m_nLength);
	
	m_nPlyCookie %= MAX_PLAYLST;
	BYTE* pBuffer = m_pBufferIn[0] + ( m_nPlyCookie++ * TEMP_BUFFER );
	
	CopyMemory( pBuffer, pInput->m_pBuffer, TEMP_BUFFER );
	pInput->Remove( TEMP_BUFFER );
	
	if ( int nResult = SsmPlayMemBlock(nChan, 6, pBuffer, TEMP_BUFFER, OnPlayMemBlockDone, m_pPlayIoOver) )
	{
#ifdef _DEBUG
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, sError );
#endif
		m_pPlayIoOver->Internal = 0;
		return nResult;
	}
	
	m_pPlayIoOver->Offset++;
	m_pPlayIoOver->Internal = 0;
	ResetEvent( m_pPlayIoOver->hEvent );
	
//TRACE("%i-> ReplayWith in [%i]\n", GetTickCount(), m_pPlayIoOver->Offset);
	return 0;
}

BOOL CALLBACK CCircuitry::OnPlayMemBlockDone(int ch, int nEndReason, PUCHAR pucBuf, DWORD dwStopOffset, PVOID pV)
{
	enum {
		PlayBy_Done		= 1,
		PlayBy_RcvDtmf	= 2,
		PlayBy_RcvBargein=3,
		PlayBy_Hangup	= 4,
		PlayBy_BlockApp	= 5,
	};
	
	if ( SsmGetChState(ch) == S_CALL_STANDBY ) nEndReason = PlayBy_Hangup;
	
	OVERLAPPED*	pPlayIoOver = (OVERLAPPED*)pV;
//TRACE("%i-> OnPlayMemBlockDone[%i] in [%i]\n", GetTickCount(), nEndReason,pPlayIoOver->Offset);
	
	CSingleLock pLock( &Neighbour.m_pSection );
	for ( ; HasOverlappedIoCompleted(pPlayIoOver) && ! pLock.Lock(50); );
	
	if ( ! --pPlayIoOver->Offset ) SetEvent( pPlayIoOver->hEvent );
	
	CCircuitry* pChannel = (CCircuitry*) pPlayIoOver->InternalHigh;
	
	if ( nEndReason == PlayBy_Done && pLock.IsLocked() )
	{
		pChannel->ReplayWith( ch, pChannel->m_pChain );
	}
	else if ( nEndReason != PlayBy_Done )
	{
		pPlayIoOver->OffsetHigh = -1;
		if ( nEndReason == PlayBy_Hangup ) pChannel->m_nTaskId = -1;
	}
	
	return TRUE;
}
