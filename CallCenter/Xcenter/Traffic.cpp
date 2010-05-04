//
// Traffic.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Traffic.h"
#include "Network.h"
#include "Channel.h"

#include "Settings.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CTraffic Traffic;

//////////////////////////////////////////////////////////////////////
// CTraffic construction

CTraffic::CTraffic()
{
	m_hThread		= NULL;
	m_bThread		= FALSE;
}

CTraffic::~CTraffic()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CTraffic attributes

BOOL CTraffic::IsMusic(LPCTSTR xPath) const
{
	LPCTSTR pptr, sptr;

	for (	pptr  = Settings.General.PathMuc;
			pptr != NULL; pptr = sptr + 1 )
	{
		sptr = _tcsnistr( pptr, ";", 1 );

		if ( sptr == NULL ) return _tcsistr(xPath, pptr) == xPath;

		if ( _tcsnistr(xPath, pptr, int(sptr - pptr)) == xPath ) return TRUE;
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CTraffic setup

BOOL CTraffic::Setup()
{
	StartThread(); return TRUE;
}

void CTraffic::Release()
{
	StopThread();
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CTraffic clear

void CTraffic::Clear()
{
	for ( POSITION posNext = m_pUsrBacks.GetStartPosition(); posNext; )
	{
		COleDateTime tTimer;
		CPacket* pPacket;
		
		m_pUsrBacks.GetNextAssoc( posNext, tTimer, pPacket );

		pPacket->Release();
	}
	
	m_pUsrBacks.RemoveAll();
}

CPacket* CTraffic::AddUsrBack(CPacket* pPacket, DWORD nReserve)
{
	CRegistry pRegistry;
	DWORD nInterval = pRegistry.GetDword( "Trunk", "CallBackOut", 3 );
	
	COleDateTime tt = COleDateTime::GetCurrentTime();
	tt += COleDateTimeSpan( 0, 0, 0, nInterval + nReserve );
	
	CPacket* pPacket2;
	if ( m_pUsrBacks.Lookup( tt, pPacket2 ) )
	{
		return AddUsrBack( pPacket, nReserve + 1 );
	}
	
	m_pUsrBacks.SetAt( tt, pPacket );
	m_pWakeup.SetEvent();

	return pPacket;
}

CPacket* CTraffic::AddUsrBack(LPCTSTR xLocalId, LPCTSTR xRemoteId)
{
	CDialPacket* pPacket = new CDialPacket;
	
	pPacket->SetLocalId( xLocalId );
	pPacket->SetRemoteId( xRemoteId );
	
	return AddUsrBack( pPacket );
}

//////////////////////////////////////////////////////////////////////
// CTraffic thread run

void CTraffic::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CTraffic::StopThread()
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
		theApp.Message( MSG_DEBUG, _T("WARNING: Terminating CTraffic thread.") );
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CTraffic::ThreadStart(LPVOID pParam)
{
	CTraffic* pTraffic = (CTraffic*)pParam;
	pTraffic->OnRun();
	return 0;
}

BOOL CTraffic::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
		WaitForSingleObject( m_pWakeup, 6000 );
		
		try
		{
			if ( ! m_bThread ) break;
			pLock.Lock();
			OnRunUsrBack( NULL );
			pLock.Unlock();
			
			if ( ! m_bThread ) break;
			pLock.Lock();
			OnRunMeeting();
			pLock.Unlock();
		}
		catch(_com_error e) 
		{ 
			theApp.Message( MSG_ERROR, "error on CTraffic: %s", e.ErrorMessage() );
			pLock.Unlock();
		}
		catch(...)
		{
			theApp.Message( MSG_ERROR, "error on CTraffic: %s", "a fatal error" );
			pLock.Unlock();
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTraffic run function

BOOL CTraffic::OnRunUsrBack(POSITION posNext)
{
	if ( ! posNext ) posNext = m_pUsrBacks.GetStartPosition();
	if ( ! posNext ) return FALSE;
	
	COleDateTime tTimer;
	CPacket* pPacket;
	
	m_pUsrBacks.GetNextAssoc( posNext, tTimer, pPacket );
	if ( tTimer > COleDateTime::GetCurrentTime() )
	{
		return posNext ? OnRunUsrBack( posNext ) : FALSE;
	}
	
	CSingleLock pLock( &Network.m_pSection );
	if ( ! pLock.Lock( 250 ) ) return FALSE;
	
	CHAR xRemoteId[MAX_PHONE + 1];
	xRemoteId[ ((CDialPacket*)pPacket)->GetRemoteId(xRemoteId) ] = '\0';
	
	CHAR xFeeNumber[MAX_PHONE + 1];
	xFeeNumber[ ((CDialPacket*)pPacket)->GetLocalId(xFeeNumber) ] = '\0';
	
	CChannel* pChannel = Network.LookOutPhs( xRemoteId, xFeeNumber );
	if ( pChannel == NULL ) return FALSE;
	
	pChannel->OnDialPacket((CDialPacket*)pPacket); pPacket->Release();
	CopyMemory( pChannel->CallType, CA_BACK, MAX_CALLTYPE );
	
	m_pUsrBacks.RemoveKey( tTimer );
	m_pWakeup.SetEvent();
	
	return TRUE;
}

BOOL CTraffic::OnRunMeeting()
{
	static _RecordsetPtr pRecordset;
	
	if ( pRecordset == NULL || ! pRecordset->State )
	{
		CString strQuery;
		
		strQuery.Format( "select a.xmeetingroomcode, a.xmeetingphone, a.xmeetingphonestate, b.xmeetingroomfee"
						" from xmeetingroom a left join xmeetingroomlist b"
						" on a.xmeetingroomcode=b.xmeetingroomcode"
						" where a.xmeetingphonestate = '0'" );
		TRACE( strQuery + "\n" );
		
		xOpenRecordset(pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	}
	else
	if ( pRecordset != NULL && pRecordset->adoEOF )
	{
		pRecordset->Requery( adCmdUnspecified );
	}
	
	for ( ; ! pRecordset->adoEOF ; pRecordset->MoveNext() )
	{
		_variant_t FeeNumber= pRecordset->GetCollect( "xmeetingroomfee" );
		_variant_t TheValue = pRecordset->GetCollect( "xmeetingphone" );
		
		CSingleLock pLock( &Network.m_pSection, TRUE );
		
		if ( CChannel* pChannel = Network.LookOutPhs( (_bstr_t)TheValue ) )
		{
			pChannel = PagerLibrary( pChannel, "xmeetchannel.dll", (_bstr_t)TheValue,
				(_bstr_t)FeeNumber, _ttoi( (_bstr_t)pRecordset->GetCollect( "xmeetingroomcode" ) ) );
			
			pRecordset->PutCollect( "xmeetingphonestate", _variant_t("1") );
			pRecordset->Update();
		}
	}
	
	return TRUE;
}

CChannel* CTraffic::PagerLibrary(CChannel* pParent, LPCTSTR pszName, LPCTSTR xRemoteId, LPCTSTR xFeeNumber, DWORD nReserved)
{
	HINSTANCE hInstance = LoadLibrary( pszName );
	if ( hInstance == NULL ) return NULL;
	
	typedef CChannel* (* PCTOR) (CChannel*, LPCTSTR, LPCTSTR, DWORD);
	PCTOR pCtor = (PCTOR) GetProcAddress( hInstance, _T("PagerFrom") );
	
	if ( NULL != pCtor )
	{
		CChannel* pChannel = pCtor( pParent, xRemoteId, xFeeNumber, nReserved );
		pChannel->m_hInstance = hInstance;
		
		return pChannel;
	}
	
	FreeLibrary( hInstance );
	return NULL;
}
