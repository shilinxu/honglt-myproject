//
// Channel.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Channel.h"

#include "Network.h"
#include "Settings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CChannel construction

CChannel::CChannel(LPCTSTR xUserId)
{
	m_xUserId	= _tcsdup( xUserId );
	
	m_xRemoteId	= NULL;
	
	CChannel::Setup();
}

CChannel::CChannel(SOCKET hSocket, SOCKADDR_IN* pHost) : CTransfer( hSocket, pHost )
{
	CString strTemp;
	srand( GetTickCount() );
	
	strTemp.Format( _T("#%i"), rand() );
	m_xUserId	= _tcsdup( strTemp );
	
	m_xRemoteId	= NULL;
	
	CChannel::Setup();
}

CChannel::~CChannel()
{
	free( m_xUserId ); m_xUserId = NULL;
	free( m_xRemoteId );
}

//////////////////////////////////////////////////////////////////////
// CChannel attributes

void CChannel::Setup()
{
}

void CChannel::Clone(CChannel* pParent)
{
	if ( pParent->m_hSocket != INVALID_SOCKET )
	{
		this->AttachTo( pParent );
	}
}

CString CChannel::GetRecordFile(LPCTSTR fMask) const
{
	CString xNewFile = CString( fMask );
	
	if ( xNewFile.Find( "<USER>" ) >= 0 )
	{
		Replace( xNewFile, "<USER>", m_xUserId );
	}
	
	if ( xNewFile.Find( "<DATE>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%04i年%02i月%02i日",
			pTime.wYear, pTime.wMonth, pTime.wDay );
		Replace( xNewFile, "<DATE>", strValue );
	}
	
	if ( xNewFile.Find( "<TIME>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%02i时%02i分%02i秒%03i",
			pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds );
		Replace( xNewFile, "<TIME>", strValue );
	}
	
	if ( xNewFile.Find( "<REMOTE>" ) >= 0 )
	{
		CString strValue;
		Replace( xNewFile, "<REMOTE>", m_xRemoteId );
	}
	
	for ( LPCTSTR xPath = xNewFile, xPathExt = NULL;
		  xPathExt = _tcsistr(xPath, "\\"); xPath = xPathExt + 1 )
	{
		CString strValue = xNewFile.Left(xPathExt - (LPCTSTR)xNewFile);
		CreateDirectory( Settings.Record.PathRec + "\\" + strValue, NULL );
	}
	
	return xNewFile;
}

//////////////////////////////////////////////////////////////////////
// CChannel disconnect

void CChannel::Disconnect()
{
	Disconnect(0); delete this;
}

void CChannel::Disconnect(DWORD)
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	Network.m_pList.RemoveKey( m_xUserId );
	
	CTransfer::Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CChannel deliver buffer

HRESULT CChannel::DeliverBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType, DWORD)
{
	return S_OK;
}

HRESULT CALLBACK CChannel::DeliverBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE nType, DWORD nTimestamp, LPVOID lv)
{
	CChannel* pClass = (CChannel *)lv;
	
	return pClass->DeliverBuffer( pBuffer, nBuffer, nType, nTimestamp );
}

/////////////////////////////////////////////////////////////////////////////
// CCircuitry render buffer

HRESULT CChannel::RenderBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType)
{
	return S_OK;
}

HRESULT CALLBACK CChannel::RenderBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE nType, DWORD, LPVOID lv)
{
	CChannel* pClass = (CChannel *)lv;
	
	return pClass->RenderBuffer( pBuffer, nBuffer, nType );
}

//////////////////////////////////////////////////////////////////////
// CChannel socket event handlers

void CChannel::OnHandshake()
{
	CTransfer::OnHandshake();
}

//////////////////////////////////////////////////////////////////////
// CChannel TCP Transfer func

BOOL CChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId(IBase_SyncOrderRelation) )
	{
		return OnSyncOrderRelation( ((CPacketBase*)pPacket)->m_pXML );
	}
	
	return CTransfer::OnPacket( pPacket );
}

BOOL CChannel::OnSyncOrderRelation(CXMLElement* pXML)
{
	CString strMethod = pXML->GetAttributeValue("method");
	
	if ( ! strMethod.CompareNoCase( "login" ) )
	{
		return OnSyncHostView( pXML );
	}
	
	return TRUE;
}

BOOL CChannel::OnSyncHostView(CXMLElement* pXML)
{
	CXMLElement* pSync = pXML->GetElementByName( "UserId" );
	CString sUserId = ( pSync ) ? pSync->GetValue() : "Guest";
	
	CString sUserPwd;
	if ( int nResult = pXML->GetAttribute( "operator" ) == NULL ?
		OnReadUser( sUserId, sUserPwd ) : OnReadGroup( sUserId, sUserPwd ) )
	{
		return OnFaultResp( nResult );
	}
	
	CString strValue;
	int nLength	= CalcHashBytes( strValue.GetBuffer(32), sUserPwd );
	strValue.ReleaseBuffer( nLength );
	
	pSync = pXML->GetElementByName( "Hashing" );
	if ( ! pSync || pSync->GetValue().CompareNoCase(strValue) )
	{
		return OnFaultResp( EBase_NoUser );
	}
	
	LPVOID lParam = (LPVOID)(LPCTSTR)sUserId;
	
	if ( int nResult = SendMessage( theApp.m_pMainWnd->GetSafeHwnd(),
		WM_USERCHANGED, 0, (LPARAM)&lParam ) )
	{
		OnFaultResp( nResult );
		OnDropped( nResult ); return FALSE;
	}
	
	CChannel* pChannel = (CChannel*)lParam;
	pChannel->Clone( this ); Disconnect();
	
	pChannel->OnSyncHostView( EBase_Success );
	Network.SetChannel( pChannel );
	pChannel->OnHandshake();
	
	return FALSE;
}

BOOL CChannel::OnSyncHostView(int nResult)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncOrderRelation );
	pXML->AddAttribute( "method", "welcome" );
	
	SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML;
	
	return TRUE;
}

BOOL CChannel::OnFaultResp(int nFaultCode)
{
	return SendPacket( new CFaultResp(nFaultCode) );
}

//////////////////////////////////////////////////////////////////////
// CChannel packet unserialize

CPacket* CChannel::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	
	if ( ! pBuffer->ReadLine( strLine, TRUE ) ) return NULL;
	
	CPacket* pPacket = CPacketBase::ReadBuffer( pBuffer->m_pBuffer, pBuffer->m_nLength );
	
	if ( pPacket && pPacket->GetBufferSize() )
	{
		int nLength = pPacket->GetBufferSize();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
		
		return pPacket;
	}
	else if ( ! pPacket )
	{
		int nLength = strLine.GetLength();
		pBuffer->Remove( nLength + 1 + (pBuffer->m_pBuffer[nLength] == '\r') ); // + [\r] \n"
	}
	
	pPacket->Release();
	return NULL;
}

int CChannel::OnReadUser(LPCTSTR sUserId, CString& sUserPwd)
{
	CString strQuery;
	strQuery.Format( "select a.*, b.xuserbindid"
					" from xuser_info a inner join"
					" xuser b on a.xjsjbh = b.xjsjbh"
					" where b.xphone = '%s'", sUserId );
	TRACE( strQuery + "\n" );
	theApp.Message( MSG_DEBUG, strQuery );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return EBase_NoUser;
	
	_variant_t TheValue = xRecordset->GetCollect( "xuserpwd" );
	if ( VT_NULL == TheValue.vt ) return 0;
	
	sUserPwd = (LPCTSTR)(_bstr_t)TheValue;
	return 0;
}

int CChannel::OnReadGroup(LPCTSTR sUserId, CString& sUserPwd)
{
	CString strQuery;
	strQuery.Format( "select * from xoperator"
					" where xcode = '%s'", sUserId );
	TRACE( strQuery + "\n" );
	theApp.Message( MSG_DEBUG, strQuery );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return EBase_NoUser;
	
	_variant_t TheValue = xRecordset->GetCollect( "xpwd" );
	if ( VT_NULL == TheValue.vt ) return 0;
	
	sUserPwd = (LPCTSTR)(_bstr_t)TheValue;
	return 0;
}
