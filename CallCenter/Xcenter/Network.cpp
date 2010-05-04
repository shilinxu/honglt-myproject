//
// Network.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "Settings.h"
#include "profiles.h"
#include "Handshakes.h"
#include "Channel.h"
#include "XML.h"

#include "ChannelClerk.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CNetwork Network;

//////////////////////////////////////////////////////////////////////
// CNetwork construction

CNetwork::CNetwork()
{
	m_bConnected	= FALSE;
	m_hThread		= NULL;
	m_bThread		= FALSE;
}

CNetwork::~CNetwork()
{
}

//////////////////////////////////////////////////////////////////////
// CNetwork attributes

BOOL CNetwork::IsConnected() const
{
	return m_bConnected;
}

int CNetwork::IsWellConnected() const
{
	if ( ! IsConnected() ) return 0;
	return m_pList.GetCount();
}

#ifdef NPICKUP
	CBuffer		m_pDebugBlock;
void CNetwork::PutBlock(EVTBLK* evtblk)
{
Network.m_nMaxChan=1000;
	m_pDebugBlock.Add( evtblk, sizeof(EVTBLK) );
}
BOOL CNetwork::ReadBlock(EVTBLK* evtblk)
{
	if ( m_pDebugBlock.m_nLength < sizeof(EVTBLK) ) return FALSE;
	CopyMemory( evtblk, m_pDebugBlock.m_pBuffer, sizeof(EVTBLK) );
	m_pDebugBlock.Remove( sizeof(EVTBLK) );

	return TRUE;
}
#endif
//////////////////////////////////////////////////////////////////////
// CNetwork setup

BOOL CNetwork::Setup()
{
	if ( SsmStartCti("ShConfig.ini", "ShIndex.ini") != 0 )
	{
		CHAR sError[1024];
		SsmGetLastErrMsg( sError );
		theApp.Message( MSG_ERROR, sError );
#ifdef NPICKUP
	MLOG( "debug :: CNetwork::Setup(), m_nMaxChan = 0" );
	m_nMaxChan = 0;
	return TRUE;
#endif
		return FALSE;
	}
	
	SsmFaxSetMaxSpeed(9600);
	m_nMaxChan = SsmGetMaxCh();
	
	return TRUE;
}

void CNetwork::Release()
{
	SsmCloseCti();
}

//////////////////////////////////////////////////////////////////////
// CNetwork connection

BOOL CNetwork::Connect()
{
	if ( IsConnected() ) return TRUE;
	
	EVENT_SET_INFO EventSet;
	EventSet.dwWorkMode = EVENT_POLLING;
	SsmSetEvent(-1, -1, TRUE, &EventSet);

	StartThread();
	
	m_bConnected = TRUE;
	
	return TRUE;
}

void CNetwork::Disconnect()
{
	if ( ! IsConnected() ) return;

	m_bConnected = FALSE;
	
	StopThread();
	
	for ( POSITION pos = m_pList.GetStartPosition(); pos; )
	{
		int nChan;
		CChannel* pChannel;
		
		m_pList.GetNextAssoc( pos, nChan, pChannel );
		
		HMODULE hModule = pChannel->Release();
		FreeLibrary( hModule );
	}
	
	m_pList.RemoveAll();
	
	Release();
}

//////////////////////////////////////////////////////////////////////
// CNetwork thread run

void CNetwork::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CNetwork::StopThread()
{
	if ( m_hThread == NULL ) return;
	
	m_bThread = FALSE;
	m_pWakeup.SetEvent();
	
	for ( int nAttempt = 50 ; nAttempt > 0 ; nAttempt-- )
	{
		DWORD nCode;
		if ( ! GetExitCodeThread( m_hThread, &nCode ) ) break;
		if ( nCode != STILL_ACTIVE ) break;
		Sleep( 100 );

		MSG msg;
		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	
	if ( nAttempt == 0 )
	{
		TerminateThread( m_hThread, 0 );
		theApp.Message( MSG_DEBUG, _T("WARNING: Terminating CNetwork thread.") );
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CNetwork::ThreadStart(LPVOID pParam)
{
	CNetwork* pNetwork = (CNetwork*)pParam;
	pNetwork->OnRun();
	return 0;
}

BOOL CNetwork::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
#ifdef NPICKUP
		WaitForSingleObject( m_pWakeup, 50 );
#endif	
		pLock.Lock();
		RunPolling();
		pLock.Unlock();
		if ( ! m_bThread ) break;
		
		pLock.Lock();
		RunChannel();
		pLock.Unlock();
		if ( ! m_bThread ) break;
	}
	
	return TRUE;
}

BOOL CNetwork::RunPolling()
{
	EVTBLK evtblk;
	
	if ( SsmWaitForEvent(50, &evtblk) == -1 ) return FALSE;
#ifdef NPICKUP
	if ( ! ReadBlock(&evtblk) ) return FALSE;
#endif

	int nChan = evtblk.nReference;

#if 1
	if ( nChan < 0 || nChan >= m_nMaxChan ) return FALSE;
#endif

#if 1
	SYSTEMTIME pTime;
	GetLocalTime( &pTime );

	theApp.Message( MSG_DEBUG, "Time: %02i:%02i:%02i.%03i Chan: %i Event: %04x Param: %08x(%i, %i)",
		pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds,
		evtblk.nReference, evtblk.wEventCode, evtblk.dwParam,
		evtblk.dwParam >> 16, evtblk.dwParam & 0xFFFF );
#endif
//#ifndef _DEBUG
	try
//#endif
	{
		CChannel* pChannel = FindChannel( nChan );
		if ( ! pChannel ) throw "Invalid channel";
		
#ifndef NPICKUP
		if ( evtblk.wEventCode == E_CHG_ChState )
		pChannel->PrintChState( &evtblk );
#endif
		
		// update status to monitor
		if ( evtblk.wEventCode == E_CHG_ChState )
		Handshakes.SendPacketTo( new CEchnPacket(evtblk.nReference, evtblk.dwParam & 0xFFFF) );

		struct state*& fcnp1 = pChannel->fcnp;
		/* process the event and save the next channel state */
		fcnp1 = (pChannel->*fcnp1->process)(&evtblk);

		struct state*& fcnp2 = pChannel->fcnp;
		/* begin the new state checking the error code returned */
		int nResult = (pChannel->*fcnp2->initiate)(&evtblk);
		
		if ( nResult != 0 )
		{
			CHAR sError[1024]; SsmGetLastErrMsg( sError ); throw sError;
		}
	}
	catch( CHAR* sError )
	{
		theApp.Message( MSG_ERROR, "error on channel %d: %s", nChan, sError );
		return FALSE;
	}
#ifndef _DEBUG
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on channel %d: %s", nChan, e.ErrorMessage() );
		return FALSE;
	}
	catch(...)
	{
		theApp.Message( MSG_ERROR, "RunPolling(): error on channel %d: %s", nChan, "a fatal error" );
		return FALSE;
	}
#endif

	return TRUE;
}

BOOL CNetwork::RunChannel()
{
	for ( POSITION posNext = m_pList.GetStartPosition(); posNext; )
	{
		int nChan;
		CChannel* pChannel;
		
		m_pList.GetNextAssoc( posNext, nChan, pChannel );
		ASSERT( pChannel != NULL );
//#ifndef _DEBUG
		try
//#endif
		{
			if ( pChannel ) pChannel->OnRun();
		}
		catch( CHAR* sError )
		{
			theApp.Message( MSG_ERROR, "error on channel %d: %s", nChan, sError );
			return FALSE;
		}
#ifndef _DEBUG
		catch(_com_error e) 
		{ 
			theApp.Message( MSG_ERROR, "error on channel %d: %s", nChan, e.ErrorMessage() );
			return FALSE;
		}
		catch(...)
		{
			theApp.Message( MSG_ERROR, "RunChannel(): error on channel %d: %s", nChan, "a fatal error" );
			return FALSE;
		}
#endif
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetwork Channel operations

CChannel* CNetwork::MakeChannel(int nChan, LPCTSTR xFeeCode)
{
	CString xModule = _T("");
	if ( Profiles.FindFinal(xFeeCode, &xModule) ) //return NULL;
	{
	CChannel* pChannel = FromHandle( nChan, xFeeCode, (LPCTSTR)xModule );
	if ( pChannel != NULL ) return pChannel;
	}
	
#if 1	// ¸¨ÖúÒµÎñ
	else
	if ( ! _tcsncmp( xFeeCode, _T("9119"), 4 ) )
	{
		return new CClerkChannel( nChan, xFeeCode );
	}
#endif

	return NULL;
}

CChannel* CNetwork::FromHandle(int nChan, LPCTSTR xFeeCode, LPCTSTR xInstance)
{
	HINSTANCE hInstance = LoadLibrary( xInstance );
	if ( hInstance == NULL ) return NULL;
	
	typedef CChannel* (* PCTOR) (int, LPCTSTR);
	PCTOR pCtor = (PCTOR) GetProcAddress( hInstance, _T("Create") );
	
	if ( NULL != pCtor )
	{
		CChannel* pChannel = pCtor( nChan, xFeeCode );
		pChannel->m_hInstance = hInstance;
//theApp.Message(MSG_ERROR,"debug: FromHandle-> [%i][%s] -->0x%x", nChan, xFeeCode, hInstance);
		
		return pChannel;
	}
	
	FreeLibrary( hInstance );
	return NULL;
}

CChannel* CNetwork::SetChannel(CChannel* pChannel)
{
	int nChan = pChannel->m_nChan;

	CChannel* pChannel2 = FindChannel( nChan );
	if ( pChannel2 ) pChannel2->Release();
	
	m_pList.SetAt( nChan, pChannel );
	return pChannel;
}

CChannel* CNetwork::FindChannel(int nChan) const
{
	CChannel* pChannel = NULL;
	
	m_pList.Lookup( nChan, pChannel );

	return pChannel;
}

CChannel* CNetwork::LookOut(LPCTSTR xLinkPcm)
{
	return LookOutPcm( xLinkPcm );
}

CChannel* CNetwork::LookOutCh(int nChFirst, int nChTotal)
{
	for ( int nChan = nChFirst; nChan < nChFirst + nChTotal; nChan++ )
	{
		if ( SsmGetChState( nChan ) != S_CALL_STANDBY ) continue;
		
		CChannel* pChannel = Network.FindChannel( nChan );
		if ( pChannel->m_hInstance || pChannel->m_hDllClosing ) continue;
		
		if ( ! pChannel || SsmPickup( nChan ) ) continue;
		CopyMemory( pChannel->CallType, CA_HOOK, MAX_CALLTYPE );
		return pChannel;
	}
	
	return NULL;
}

CChannel* CNetwork::LookOutPcm(WORD nLinkId, int nChCount)
{
	int nLinkCh = SsmPcmTsToCh( nLinkId, 1 );
	if ( nLinkCh == -1 ) return NULL;
	
	return LookOutCh( nLinkCh, nChCount );
}

CChannel* CNetwork::LookOutPcm(LPCTSTR xLinkPcm)
{
	CString strValue = xLinkPcm;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		if ( str.Find( '(' ) >= 0 ) str = strValue.Left( strValue.Find( ')' ) + 1 );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		int nLinkId = 0, nChFirst = 0, nChTotal = 0;
		_stscanf( str, "%d(%i,%i)", &nLinkId, &nChFirst, &nChTotal );
		
		CChannel* pChannel = ( nLinkId == 99 ) ? LookOutCh( nChFirst, nChTotal ) :
		LookOutPcm( nLinkId, 30 - DeflatePcm( nLinkId ) );
		
		if ( pChannel ) return pChannel;
	}
	
	return NULL;
}

CChannel* CNetwork::LookOutPhs(LPCTSTR xRemoteId)
{
	CString strValue = Settings.Trunk.OutPhone;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		if ( str.Find( '(' ) >= 0 ) str = strValue.Left( strValue.Find( ')' ) + 1 );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		CHAR Ch;
		CHAR xField1[32], xField2[32];
		
		if ( _stscanf( str, "%[^:]%c%[^^]", xField1, &Ch, xField2 ) != 3 ) continue;
		if ( ! HavingText( xField1, xRemoteId ) ) continue;
		
		if ( _stscanf( xField2, "(%[^)^]%c", xField1, &Ch ) == 2 )
		{
			return LookOutPcm( xField1 );
		}
		
		return LookOutPcm( xField2 );
	}
	
	return LookOutPcm( Settings.Trunk.LookOut );
}

CChannel* CNetwork::LookOutPhs(LPCTSTR xRemoteId, LPCTSTR xFeeNumber)
{
	CXMLElement* pXML = Profiles.FindCorpId( xFeeNumber );
	if ( ! pXML ) return LookOutPhs( Settings.Trunk.LookOut );
	
	if ( Profiles.IsDenied( pXML, xRemoteId ) ) return NULL;
	
	CString strValue = pXML->GetAttributeValue( "LinkId" );
	return LookOutPcm( (LPCTSTR)strValue );
}

int CNetwork::BlockRemote(LPCTSTR xRemotePcm)
{
	CString strValue = xRemotePcm;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		int nError = SsmBlockRemotePCM( _ttoi(str), 0 );
		if ( nError ) return nError;
	}
	
	return 0;
}

BOOL CNetwork::VerifyIncoming(int nLinkCh)
{
	if ( ! Settings.Trunk.InLimit ) return TRUE;
	CString strValue = Settings.Trunk.InLimited;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		if ( str.Find( '(' ) >= 0 ) str = strValue.Left( strValue.Find( ')' ) + 1 );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		int nLinkId = 0, nChFirst = 0, nChTotal = 0;
		_stscanf( str, "%d(%i,%i)", &nLinkId, &nChFirst, &nChTotal );
		
		if ( nLinkId == 99 )
		{
			if ( nLinkCh >= nChFirst && nLinkCh - nChFirst < nChTotal ) return FALSE;
		}
		
		nChFirst = SsmPcmTsToCh( nLinkId, 1 ); if ( nChFirst >= 0 &&
		nLinkCh >= nChFirst && nLinkCh < nChFirst + 30 ) return FALSE;
	}
	
	return FALSE;
}

int CNetwork::DeflatePcm(WORD nLinkId)
{
	CString strValue = Settings.Trunk.OutReserved;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		int nPcmId, nLinkCh;
		if ( _stscanf( str, "%i:%i", &nPcmId, &nLinkCh ) == 2 )
		{
			if ( nLinkId == nPcmId ) return nLinkCh;
		}
		else
		{
			if ( nLinkId == nPcmId ) return 30;
		}
	}
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CNetwork helper

BOOL CNetwork::HavingText(LPCTSTR lpszText, LPCTSTR lpszValue) const
{
	CHAR Ch;
	CHAR xField1[32], xField2[32];
	
	if ( _stscanf( lpszText, "(%[^)^]%c", xField1, &Ch ) == 2 )
	{
		return HavingText( xField1, lpszValue );
	}
	
	CString strValue = lpszText;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( _stscanf( str, "%[^-]%c%[^^]", xField1, &Ch, xField2 ) == 3 )
		{
			if ( _tcscmp( xField1, lpszValue ) <= 0 && _tcscmp( xField2, lpszValue ) >= 0 ) return TRUE;
		}
		else
		{
			if ( _tcsncmp( xField1, lpszValue, _tcslen( xField1 ) ) == 0 ) return TRUE;
		}
	}
	
	return FALSE;
}
