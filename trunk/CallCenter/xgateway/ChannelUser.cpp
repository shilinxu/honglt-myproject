//
// ChannelUser.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "ChannelUser.h"
#include "Settings.h"
#include "Network.h"
#include "XML.h"

#include "Neighbour.h"
#include "Circuitry.h"
#include "Datagrams.h"
#include "FilterGraph.h"

//////////////////////////////////////////////////////////////////////
// CUserChannel construction

CUserChannel::CUserChannel(LPCTSTR xUserId, LPCTSTR xGroupId) : CChannel( xUserId )
{
	m_xGroupId		= _tcsdup( xGroupId );
	m_xLinkId		= NULL;
	
	m_nAlawCookie	= m_nH263Cookie = 0;
	
	CUserChannel::Setup();
}

CUserChannel::~CUserChannel()
{
	free( m_xGroupId ); m_xGroupId = NULL;
	free( m_xLinkId ); m_xLinkId = NULL;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel attributes

void CUserChannel::Setup()
{
	CChannel::Setup();
}

void CUserChannel::SetLinkId(LPCTSTR xLinkId)
{
	ASSERT( m_xLinkId == NULL );
	m_xLinkId	= _tcsdup( xLinkId );
}

//////////////////////////////////////////////////////////////////////
// CUserChannel disconnect

void CUserChannel::Disconnect(DWORD)
{
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	CCircuitry* pChannel = Neighbour.FindChannel( m_xUserId );
	if ( pChannel ) pChannel->Disconnect();
	
	CChannel::Disconnect(0);
}

//////////////////////////////////////////////////////////////////////
// CUserChannel socket event handlers

void CUserChannel::OnHandshake()
{
	CChannel::OnHandshake();
	
	Neighbour.ConnectTo( m_xUserId, 0 );
}

//////////////////////////////////////////////////////////////////////
// CUserChannel run function

BOOL CUserChannel::OnRun()
{
	CSingleLock pLock( &Datagrams.m_pSection, TRUE );
	
	if ( CDatagrams* pDatagram = Datagrams.SeekNext(Payload_Alaw) )
	{
		CDatagramIn* pDG = pDatagram->LookupKey( m_xUserId );
		if ( pDG ) pDG->RenderBuffer( m_nAlawCookie, CChannel::DeliverBuffer, (CChannel*)this );
	}
	
	if ( CDatagrams* pDatagram = Datagrams.SeekNext(Payload_H263) )
	{
		CDatagramIn* pDG = pDatagram->LookupKey( m_xUserId );
		if ( pDG ) pDG->RenderBuffer( m_nH263Cookie, CChannel::DeliverBuffer, (CChannel*)this );
	}
	
	pLock.Unlock();
	return CChannel::OnRun();
}

//////////////////////////////////////////////////////////////////////
// CUserChannel TCP Transfer func

BOOL CUserChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( IBase_SyncDataView ) )
	{
		return OnSyncDataView( (CSyncDataView*)pPacket );
	}
	else if ( pPacket->IsCommandId( IBase_SyncHostFile ) )
	{
		return OnSyncHostFile( (CSyncHostFile*)pPacket );
	}
	
	return CChannel::OnPacket( pPacket );
}

BOOL CUserChannel::OnSyncOrderRelation(CXMLElement* pXML)
{
	CString strMethod = pXML->GetAttributeValue("method");
	
	if ( ! strMethod.CompareNoCase( "home" ) )
	{
		return OnSyncOrderRelation( pXML, 0 );
	}
#if 0
	else if ( ! strMethod.CompareNoCase("mirror") )
	{
		return OnSyncMirrorView( pXML );
	}
#endif
	else if ( ! strMethod.CompareNoCase("monitor") )
	{
		return OnSyncMonitorView( pXML );
	}
	
	return CChannel::OnSyncOrderRelation(pXML);
}

BOOL CUserChannel::OnSyncDataView(CSyncDataView* pPacket)
{
	CString strMethod = pPacket->GetMethod();
	
	if ( ! strMethod.CompareNoCase("file") )
	{
//		return OnSyncDataFileReq( pPacket->m_pXML );
	}
	
	return OnSyncDataView( pPacket->m_pXML );
}

//////////////////////////////////////////////////////////////////////
// CUserChannel OrderRelation sync func

BOOL CUserChannel::OnSyncOrderRelation(CXMLElement*, DWORD)
{
	CString strQuery;
	
	strQuery.Format( "select * from xenterprise"
					" where xentercode = '%s'", m_xGroupId );
	TRACE( strQuery + "\n" );
	theApp.Message( MSG_DEBUG, strQuery );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return TRUE;
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncOrderRelation );
	pXML->AddAttribute( "method", "home" );
	
	_variant_t TheValue = xRecordset->GetCollect( "xentername" );
	pXML->AddAttribute( "text", (LPCTSTR)(_bstr_t)TheValue );
	
	strQuery.Format( "select * from xenterpriseinfo where xentercode = '%s'"
		" and (patindex(',', xprivate) > 0 or patindex('%%%s,%%', xprivate) > 0)", m_xGroupId, m_xUserId );
	TRACE( strQuery + "\n" );
	theApp.Message( MSG_DEBUG, strQuery );
	
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
	{
		TheValue = xRecordset->GetCollect( "xalias" );
		
		CXMLElement* pSync = pXML->AddElement( "SyncOrder" );
		pSync->SetValue( (LPCTSTR)(_bstr_t)TheValue );
		
		TheValue = xRecordset->GetCollect( "xjob" );
		if ( VT_NULL != TheValue.vt ) pSync->AddAttribute( "text", (LPCTSTR)(_bstr_t)TheValue );
		
		TheValue = xRecordset->GetCollect( "xenterinfo" );
		if ( VT_NULL != TheValue.vt ) pSync->AddAttribute( "id", (LPCTSTR)(_bstr_t)TheValue );
	}
	
	SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML;
	
	return TRUE;
}

BOOL CUserChannel::OnSyncMirrorView(CXMLElement* pXML)
{
	CSingleLock pLock( &Datagrams.m_pSection, TRUE );
	
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pSync = pXML->GetNextElement( pos );
		
		CString strValue = pSync->GetAttributeValue( "Type" );
		
		CDatagrams* pDatagram = Datagrams.SeekNext( _ttoi(strValue) );
		if ( ! pDatagram ) continue;
		
		strValue = pSync->GetAttributeValue( "ID" );
		pDatagram->JoinedMirror( m_xUserId, _ttoi(strValue) );
	}
	
	return TRUE;
}

void CUserChannel::OnSyncMirrorView(DWORD)
{
	for ( CDatagrams* pDatagram = &Datagrams; pDatagram; )
	{
		pDatagram->LeavedMirror( m_xUserId, 0 );
		pDatagram->LeavedMirror( m_xUserId );
		pDatagram = pDatagram->m_pNext;
	}
}

//////////////////////////////////////////////////////////////////////
// CUserChannel DataView sync func

BOOL CUserChannel::OnSyncDataView(CXMLElement* pXML)
{
	CString strExecute = pXML->GetAttributeValue( "method" );
	CString strQuery = strExecute;
	
	for ( POSITION posNext = pXML->GetElementIterator(); posNext; )
	{
		CXMLElement* pSync = pXML->GetNextElement( posNext );
		CString strValue = pSync->GetName();

		if ( ! strValue.CompareNoCase( "search_condition" ) ) strQuery += " where";
		if ( ! strValue.CompareNoCase( "order_by_expression" ) ) strQuery += " order by";
		
		strQuery += " " + pSync->GetValue();
	}
	
	TRACE( strQuery + "\n" );
	theApp.Message( MSG_DEBUG, strQuery );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	if ( _tcsistr(strExecute, "select") != (LPCTSTR)strExecute ) return TRUE;
//	if ( strExecute.CompareNoCase( "select" ) ) return TRUE;
	
#if 1
	pXML = new CXMLElement( NULL, ISyncDataView );
#else
	CString hKey = pXML->GetAttributeValue( "hKey" );
	pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "hKey", (LPCTSTR)hKey );
#endif
	
	for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
	{
		CXMLElement* pSync = pXML->AddElement( "SyncOrder" );
		FieldsPtr pFields = xRecordset->GetFields();
		
		for ( int nItem = 0; nItem < pFields->GetCount(); nItem ++ )
		{
			_variant_t TheValue = pFields->GetItem((long)nItem)->Value;
			if ( TheValue.vt != VT_NULL )
			{
				pSync->AddAttribute( (LPCTSTR)pFields->GetItem((long)nItem)->GetName(), (LPCTSTR)(_bstr_t)TheValue );
			}
		}
	}
	
	SendPacket( new CSyncDataView(pXML) );
	delete pXML;
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel file Transfer func
/*
BOOL CUserChannel::OnSyncDataFileReq(CXMLElement* pXML)
{
	CString strFile = pXML->GetAttributeValue("file");
	
	pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "file" );
	pXML->AddAttribute( "file", strFile );
	
	HANDLE hFile = CreateFile( (LPCTSTR)strFile, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	if ( hFile == INVALID_HANDLE_VALUE ) return OnSyncDataFileResp( pXML, IDS_TRANSFER_NOTFILE );
	
	OnSyncDataFileResp( pXML, hFile );
	CloseHandle( hFile );
	
	return TRUE;
}

BOOL CUserChannel::OnSyncDataFileResp(CXMLElement* pXML, HANDLE hFile)
{
	FILETIME pFileTime;
	GetFileTime( hFile, NULL, NULL, &pFileTime );
	
	CString strDate, strTime;
	SYSTEMTIME pSystemTime;
	
	FileTimeToSystemTime( &pFileTime, &pSystemTime );
	SystemTimeToTzSpecificLocalTime( NULL, &pSystemTime, &pSystemTime );
	
	GetDateFormat( LOCALE_USER_DEFAULT, 0, &pSystemTime, _T("yyyy-MM-dd"), strDate.GetBuffer( 64 ), 64 );
	GetTimeFormat( LOCALE_USER_DEFAULT, 0, &pSystemTime, _T("HH:mm"), strTime.GetBuffer( 64 ), 64 );
	strDate.ReleaseBuffer(); strTime.ReleaseBuffer();
	
	strDate += ' ';
	strDate += strTime;
	
	CXMLElement* pSync = pXML->AddElement("filetime");
	pSync->SetValue( (LPCTSTR)strDate );
	
	DWORD nSizeHigh	= 0;
	DWORD nSizeLow	= GetFileSize( hFile, &nSizeHigh );
	QWORD nSize		= (QWORD)nSizeLow + ( (QWORD)nSizeHigh << 32 );
	
	CString strValue;
	strValue.Format( "%I64i", nSize );
	
	pSync = pXML->AddElement("filesize");
	pSync->SetValue( (LPCTSTR)strValue );
	
	pSync = pXML->AddElement("RTPAddress");
	pSync->SetValue( Settings.Network.Host );
	
	SendPacket( new CSyncDataView(pXML) );
	delete pXML;
	
	return TRUE;
}

BOOL CUserChannel::OnSyncDataFileResp(CXMLElement* pXML, UINT nErrId)
{
	CString strValue;
	strValue.Format( "%i", GetLastError() );
	
	pXML->AddAttribute( "faultcode", strValue );
	
	LoadString( strValue, nErrId );
	
	CXMLElement* pSync = pXML->AddElement( "faultstring" );
	pSync->SetValue( (LPCTSTR)strValue );
	
	SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML;
	
	return FALSE;
}
*/
//////////////////////////////////////////////////////////////////////
// CUserChannel HostFile sync func

BOOL CUserChannel::OnSyncHostFile(CSyncHostFile* pPacket)
{
	CString strMethod = pPacket->GetMethod();
	
	if ( ! strMethod.CompareNoCase("dialup") )
	{
		return OnSyncHostshaking( pPacket->m_pXML );
	}
	else if ( ! strMethod.CompareNoCase("bye") )
	{
		return OnSyncHostBreaking( pPacket->m_pXML );
	}
	else if ( ! strMethod.CompareNoCase("rec") )
	{
		return OnSyncHostRecording( pPacket->m_pXML );
	}
	else if ( ! strMethod.CompareNoCase("talk") )
	{
		return OnSyncHostTalking( pPacket->m_pXML );
	}
	
	return TRUE;
}

BOOL CUserChannel::OnSyncHostshaking(CXMLElement* pXML)
{
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	CCircuitry* pChannel = Neighbour.ConnectTo( m_xUserId );
	if ( ! pChannel ) throw "Invalid channel";
	
	CDialPacket* pPacket = new CDialPacket;
	
	if ( m_xLinkId ) pPacket->SetLocalId( m_xLinkId );
	else pPacket->SetLocalId( "96031" );
	
	CXMLElement* pSync = pXML->GetElementByName( "RemoteId" );
	pPacket->SetRemoteId( pSync->GetValue() );
	
	free( m_xRemoteId );
	m_xRemoteId	= _tcsdup( pSync->GetValue() );
	
	pSync = pXML->GetElementByName( "CallerId" );
	
	if ( pSync ) pPacket->SetCallerId( pSync->GetValue() );
	else pPacket->SetCallerId( m_xLinkId ? m_xLinkId : m_xUserId );
	
	pPacket->SetFeeNumber( m_xLinkId ? m_xLinkId : m_xUserId );
	
	pChannel->OnPacket( pPacket ); pPacket->Release();
	
	pLock.Unlock();
	Datagrams.BroadNext( m_xUserId, Payload_Alaw );
	
	//return OnSyncHostTalking( pXML );
	return TRUE;
}

BOOL CUserChannel::OnSyncHostBreaking(CXMLElement* pXML)
{
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	CCircuitry* pChannel = Neighbour.ConnectTo( m_xUserId );
	if ( pChannel ) pChannel->ResetChain();
	
	pLock.Unlock();
	Datagrams.UnbroadNext( m_xUserId, Payload_Alaw );
	Datagrams.UnbroadNext( m_xUserId, Payload_H263 );
	
	SendPacket( new CSyncHostFile(pXML) );
	return TRUE;
}

BOOL CUserChannel::OnSyncHostTalking(CXMLElement* pXML)
{
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	CCircuitry* pChannel = Neighbour.ConnectTo( m_xUserId );
	if ( ! pChannel ) throw "Invalid channel";
	
	pLock.Unlock();
	Datagrams.BroadNext( m_xUserId, Payload_Alaw );
	Datagrams.BroadNext( m_xUserId, Payload_H263 );
	
	return pChannel->SendPacket( new CTalkPacket );
}
