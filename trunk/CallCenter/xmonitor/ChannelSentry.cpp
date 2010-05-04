//
// ChannelSentry.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "ChannelSentry.h"
#include "Profiles.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_TRIES	1

//////////////////////////////////////////////////////////////////////
// CSentryChannel construction

CSentryChannel::CSentryChannel(CXMLElement* pXML) : m_pXML( pXML->Clone() )
{
	m_xJobId	= _tcsdup( pXML->GetAttributeValue( "Host" ) );
	ZeroMemory( &m_pHost, sizeof(m_pHost) );
	
	m_pHost.sin_family	= PF_INET;
	
	CString strValue = pXML->GetAttributeValue( "Host" );
	m_pHost.sin_addr.s_addr	= inet_addr( strValue );
	
	strValue = pXML->GetAttributeValue( "Port" );
	m_pHost.sin_port	= htons( _ttoi(strValue) );
	
	m_nTries	= 0;
}

CSentryChannel::~CSentryChannel()
{
	if ( m_pXML ) delete m_pXML;
}

//////////////////////////////////////////////////////////////////////
// CSentryChannel disconnect

void CSentryChannel::Disconnect(int nError)
{
	CTransfer::Disconnect(  );
	
//	if ( nError ) AlarmHotlink( nError );
	
	if ( m_nTries >= MAX_TRIES )
	{
		m_nTries = 0; AlarmMachine();
	}
	
//	if ( ! nError ) CChannel::Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CSentryChannel run function

BOOL CSentryChannel::OnRun()
{
	DWORD tNow = GetTickCount();
	
	if ( m_hSocket == INVALID_SOCKET )
	{
		if ( tNow - m_tConnected > 180 * 1000 ) 
		{
			return LinkRestart();
		}
	}
	
	return CChannel::OnRun();
}

//////////////////////////////////////////////////////////////////////
// CSentryChannel socket event handlers

BOOL CSentryChannel::OnConnected()
{
//	CChannel::Disconnect( 0 );
	
	return TRUE;
}

BOOL CSentryChannel::LinkRestart()
{
	m_nTries	= 0;
	
	return CChannel::LinkRestart();
}

BOOL CSentryChannel::AlarmMachine()
{
	CXMLElement* pActionXML;
	POSITION posNext = NULL;
	
	CXMLElement* pXML = Profiles.FindProcess( m_xJobId, posNext, &pActionXML );
	if ( pXML ) AlarmToShortMessage( pActionXML );
	
	CString strValue = m_pXML->GetAttributeValue( _T("Path") );
	return RestartMachine( (LPCTSTR)strValue );
}

BOOL CSentryChannel::RestartMachine(LPCTSTR lpszAction)
{
	CString strPath = lpszAction;
	strPath.TrimLeft(); strPath.TrimRight();
	
	CString sCommandLine;
	if ( strPath.Find( ' ' ) > 0 )
	{
		sCommandLine = strPath.Mid( strPath.Find( ' ' ) + 1 );
		strPath = strPath.Left( strPath.Find( ' ' ) );
	}
	
	ShellExecute( NULL, _T("open"), strPath, sCommandLine, NULL, SW_SHOWNORMAL );
	return TRUE;
}

BOOL CSentryChannel::AlarmToShortMessage(CXMLElement* pXML)
{
	CString strPhone, strContent;
	ASSERT( pXML->IsNamed( _T("ShortMessage") ) );
	
	if ( CXMLElement* pXMLSub = pXML->GetElementByName( _T("PhoneNum") ) )
	{
		strPhone = pXMLSub->GetValue();
	}
	if ( CXMLElement* pXMLSub = pXML->GetElementByName( _T("MsgContent") ) )
	{
		strContent = pXMLSub->GetValue();
		Replace( strContent, "<HOST>", m_xJobId );
	}
	
	CString	strQuery;
	
	strQuery.Format( "insert into [msgsend]..tempsm_mobile(user_mobile_number, msg_content, msg_service_id)"
					" values('%s', '%s', 'ZBK')", strPhone, strContent );
	TRACE( strQuery + "\n" );
	
	_variant_t vRowsAffected(0L);
//	theApp.m_pConnection->Execute( (_bstr_t)strQuery, &vRowsAffected, adExecuteNoRecords);
theApp.Message(MSG_TEMP, strQuery );
	
	return vRowsAffected.lVal > 0;
}
