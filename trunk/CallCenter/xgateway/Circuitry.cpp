//
// Circuitry.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Circuitry.h"
#include "Neighbour.h"
#include "Settings.h"

#include "Network.h"
#include "FilterGraph.h"
#include "Datagrams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TEMP_BUFFER		400

//////////////////////////////////////////////////////////////////////
// CCircuitry construction

CCircuitry::CCircuitry(LPCTSTR xUserId) : m_xUserId( _tcsdup(xUserId) )
{
	m_pChain		= NULL;
	m_nAlawCookie	= 0;
}

CCircuitry::CCircuitry(HANDLE hPipe) : CInterlink( hPipe )
{
	CString strTemp;
	srand( GetTickCount() );
	
	strTemp.Format( _T("#%i"), rand() );
	m_xUserId	 = _tcsdup( strTemp );
	
	m_pChain		= NULL;
	m_nAlawCookie	= 0;
	
	OnConnected();
}

CCircuitry::~CCircuitry()
{
	free( m_xUserId ); m_xUserId = NULL;
	
	if ( m_pChain ) m_pChain->Release();
}

//////////////////////////////////////////////////////////////////////
// CCircuitry attributes

void CCircuitry::Clone(CCircuitry* pParent)
{
	this->m_pChain	= pParent->m_pChain;
	pParent->m_pChain = NULL;
	
	if ( pParent->m_hPipe != INVALID_HANDLE_VALUE )
	{
		this->AttachTo( pParent );
	}
}

BOOL CCircuitry::TimerOut(DWORD nTimer)
{
	ASSERT( m_bConnected );
	
	if ( m_pChain && m_pChain->TimerOut(nTimer) )
	{
		Disconnect(); return TRUE;
	}
	
	return CInterlink::TimerOut(nTimer);
}

//////////////////////////////////////////////////////////////////////
// CCircuitry disconnect

void CCircuitry::Disconnect()
{
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	Neighbour.m_pList.RemoveKey( m_xUserId );
	
	m_pChain->Release(); m_pChain = NULL;
	
	CInterlink::Disconnect(); delete this;
}

void CCircuitry::ResetChain()
{
	m_pChain->Release();
	m_pChain = new CInterlink;
	
	// ´ý»ú
	
	CString strPipe = Settings.Centric.Local;
	strPipe += '.'; strPipe += m_xUserId;
	
	m_pChain->Connect( strPipe );
}

void CCircuitry::ResetNetwork()
{
	if ( ! m_pChain || m_pChain->IsConnected() ) return;
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CChannel* pChannel = Network.FindChannel( m_xUserId );
	if ( ! pChannel ) throw "Invalid channel";
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncHostFile );
	pXML->AddAttribute( "method", "bye" );
	
	CPacket* pPacket = new CSyncHostFile(pXML);
	pChannel->OnPacket(pPacket); pPacket->Release();
	
	delete pXML;
}

/////////////////////////////////////////////////////////////////////////////
// CCircuitry render buffer

BOOL CCircuitry::RenderBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE nType)
{
	if ( ! m_pChain || ! m_pChain->IsConnected() ) return FALSE;
	if ( nType != Payload_Alaw ) return TRUE;
	
#ifdef _DEBUG
//	TRACE( "Pipe[%i]->TX[%i]: [%i] {%s}\n", m_pChain->m_pOutput->m_nLength,
//		nType, nBuffer, ToHex( pBuffer, 100 ) );
#endif
	
	return m_pChain->SendPacket( pBuffer, nBuffer );
}

HRESULT CALLBACK CCircuitry::RenderBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE nType, DWORD, LPVOID lv)
{
	CCircuitry* pClass = (CCircuitry *)lv;
	
	pClass->RenderBuffer( pBuffer, nBuffer, nType );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CCircuitry run function

BOOL CCircuitry::OnRun()
{
	if ( ! CInterlink::OnRun() ) return FALSE;
	
	if ( m_pChain && ! m_pChain->OnRun() )
	{
		ResetChain(); ResetNetwork();
		return TRUE;
	}
	
	if ( ! m_pChain || ! m_pChain->IsConnected() ) return TRUE;
	
	CSingleLock pLock( &Datagrams.m_pSection, TRUE );
	
	CDatagrams* pDatagrams = Datagrams.SeekNext(Payload_Alaw);
	if ( pDatagrams )
	{
		CDatagramIn* pDG = pDatagrams->LookupKey( m_xUserId );
		if ( pDG ) pDG->RenderBuffer( m_nAlawCookie, RenderBuffer, this );
	}
	
	if ( pDatagrams && m_pChain->m_pInput )
	{
		CDatagramOut* pDG = pDatagrams->LookupKey( m_xUserId, pDG );
		if ( pDG ) RunDatagrams( m_pChain->m_pInput, pDG );
	}
	
	return TRUE;
}

BOOL CCircuitry::RunDatagrams(CBuffer* pBuffer, CDatagramOut* pDG)
{
	if ( pBuffer->m_nLength < TEMP_BUFFER) return FALSE;
	
	int nLength = min(TEMP_BUFFER, pBuffer->m_nLength);
	
	DWORD nTimestampinc = 1000 * nLength / 8000;
	
	CSingleLock pLock( &Network.m_pSection );
	if ( ! pLock.Lock(0) ) {Neighbour.m_pWakeup.SetEvent(); return FALSE;}
	
	CChannel* pChannel = Network.FindChannel( m_xUserId );
	
	pDG->RenderBuffer( pBuffer->m_pBuffer, nLength, Payload_Alaw,
		nTimestampinc, CChannel::RenderBuffer, pChannel );
	
	pBuffer->Remove( nLength );
	Neighbour.m_pWakeup.SetEvent();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CCircuitry pipe event handlers

BOOL CCircuitry::OnConnected()
{
	CInterlink::OnConnected();
	
	ResetChain(); return TRUE;
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
	if ( pPacket->IsCommandId( ICore_Ringing ) )
	{
		return OnRingPacket( (CRingPacket*)pPacket );
	}
	
	if ( pPacket->IsCommandId( ICore_DialRemote ) )
	{
		return OnDialPacket( (CDialPacket*)pPacket );
	}
	
	return CInterlink::OnPacket( pPacket );
}

BOOL CCircuitry::OnBindPacket(CBindPacket* pPacket)
{
	CHAR xUserId[MAX_PATH + 1];
	xUserId[ pPacket->GetUserId( xUserId ) ] = '\0';
	
	CCircuitry* pChannel = new CCircuitry( xUserId );
	pChannel->Clone( this ); Disconnect();
	
	Neighbour.SetChannel( pChannel );
	
//	CHAR xSrvCode[MAX_PATH + 1];
//	xSrvCode[ pPacket->GetSrvCode( xSrvCode ) ] = '\0';
	
//	pChannel->FormatRelation( xSrvCode );
	pChannel->SendPacket( pPacket, TRUE );
	pChannel->SendPacket( new COkPacket );
	
	pChannel->ResetChain();
	return FALSE;
}

BOOL CCircuitry::OnDialPacket(CDialPacket* pPacket)
{
	m_pChain->Release();
	m_pChain = new CInterlink;
	
	CString strPipe = Settings.Centric.Remote;
	strPipe += '.'; strPipe += m_xUserId;
	
	if ( ! m_pChain->ConnectTo( strPipe ) )
	{
		theApp.Message( MSG_ERROR, "Connect Error(%i)-> %s", GetLastError(), strPipe );
		ResetChain(); return FALSE;
	}
	
	return SendPacket( pPacket, TRUE );
}

BOOL CCircuitry::OnRingPacket(CRingPacket* pPacket)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CChannel* pChannel = Network.FindChannel( m_xUserId );
	if ( ! pChannel ) throw "Invalid channel";
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncHostFile );
	pXML->AddAttribute( "method", "ring" );
	
	CHAR szBuf[MAX_PHONE + 1];
	szBuf[ pPacket->GetLocalId(szBuf) ] = '\0';
	
	CXMLElement* pSync = pXML->AddElement( "LocalId" );
	pSync->SetValue( szBuf );
	
	szBuf[ pPacket->GetRemoteId(szBuf) ] = '\0';
	pSync = pXML->AddElement( "RemoteId" );
	pSync->SetValue( szBuf );
	
	_itot( pPacket->GetLineId(), szBuf, 10 );
	pSync = pXML->AddElement( "LineId" );
	pSync->SetValue( szBuf );
	
	pChannel->SendPacket( new CSyncHostFile(pXML) );
	delete pXML;
	
	pLock.Unlock();
	CSingleLock pLock2( &Datagrams.m_pSection, TRUE );
	
	CDatagrams* pDatagrams = Datagrams.SeekNext(Payload_Alaw);
	pDatagrams->BroadNext( m_xUserId, Payload_Alaw );
	
	return TRUE;
}
