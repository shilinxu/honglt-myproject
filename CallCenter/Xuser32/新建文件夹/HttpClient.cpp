//
// HttpClient.cpp
//

#include "stdafx.h"
#include "HttpClient.h"
#include "PacketBase.h"
#include "Network.h"
#include "Buffer.h"

//////////////////////////////////////////////////////////////////////
// CHttpClient construction

CHttpClient::CHttpClient(SOCKET hSocket, SOCKADDR_IN* pHost) : CConnection( hSocket, pHost )
{
	m_bConnected= FALSE;
	httpState	= htsNull;
	
	Network.Add( this );
}

CHttpClient::CHttpClient()
{
	m_bConnected= FALSE;
	httpState	= htsNull;
	
	Network.Add( this );
}

CHttpClient::~CHttpClient()
{
	Disconnect();
	
	Network.Remove( this );
}

//////////////////////////////////////////////////////////////////////
// CHttpClient attributes

BOOL CHttpClient::IsConnected() const
{
	return m_bConnected;
}

void CHttpClient::SetState(HttpState htState)
{
	httpState	= htState;
	m_tConnected= GetTickCount();
}

//////////////////////////////////////////////////////////////////////
// CHttpClient connect

BOOL CHttpClient::ConnectTo(LPCTSTR pAddress, int nPort)
{
	DWORD nAddress = inet_addr( pAddress );
	CConnection::ConnectTo( (IN_ADDR*)&nAddress, nPort );
	
	WSAEventSelect( m_hSocket, Network.m_pWakeup, FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CHttpClient disconnect

void CHttpClient::Disconnect()
{
	CConnection::Close();
}

//////////////////////////////////////////////////////////////////////
// CHttpClient run function

BOOL CHttpClient::OnRun()
{
	if ( ! CConnection::OnRun() ) return FALSE;
	
	DWORD tNow = GetTickCount();
	
	if ( httpState < htsComplete && tNow - m_tConnected >= 15* 1000 )
	{
		OnDropped( FALSE ); return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CHttpClient socket event handlers

void CHttpClient::OnDropped(BOOL bError)
{
	CString strHost = inet_ntoa( m_pHost.sin_addr );
	_RPTF1(_CRT_WARN, _T("WARNING: Dropped Connection From %s\n"), strHost);
	
	Disconnect();
}

BOOL CHttpClient::OnHandshake()
{
	m_bConnected	= TRUE;
	m_tConnected	= GetTickCount();
	return TRUE;
}

BOOL CHttpClient::OnRead()
{
	if ( ! CConnection::OnRead() ) return FALSE;
	
	return ReadBuffer( httpState ) >= 0;
}

//////////////////////////////////////////////////////////////////////
// CHttpClient read buffer

int CHttpClient::ReadBuffer(HttpState& htState)
{
	if ( htsReady == htState )
	{
		CString strLine;
		if ( m_pInput->ReadString(strLine, FALSE) &&
			! _tcsnicmp(strLine, "POST ", 5) )
		{
			SetState( htsHeader );
		}
		if ( m_pInput->ReadString(strLine, FALSE) &&
			! _tcsnicmp(strLine, "HTTP/", 5) )
		{
			SetState( htsHeader );
		}
		
		return ReadBuffer( htState );
	}
	
	if ( htsHeader == htState )
	{
		CString strLine;
		if ( ! m_pInput->ReadString(strLine, FALSE) ) return 0;
		
		int nColon = strLine.Find( ':' );
		if ( nColon > 0 )
		{
			m_pResponseHeaders.SetAt( strLine.Left(nColon), strLine.Mid(nColon + 1) );
		}
		
//		if ( ! _tcsnicmp(strLine, "Content-Type: ", 14) ) OnContentType( (LPCTSTR)strLine + 14 );
//		if ( ! _tcsnicmp(strLine, "Content-Length: ", 16) ) OnContentLength( (LPCTSTR)strLine + 16 );
		
		if ( ! strLine.GetLength() ) SetState( htsContent );
		return ReadBuffer( htState );
	}
	
	if ( htsContent == htState )
	{
		CString strValue;
		m_pResponseHeaders.Lookup("Content-Length", strValue);
		DWORD nLength = (DWORD)_ttoi(strValue);
		
		if ( m_pInput->m_nLength >= nLength )
		{
			CPacketBase* pPacket = new CPacketBase;
			
			if ( pPacket = pPacket->LoadPacket(m_pInput->m_pBuffer, nLength) )
			{
				OnPacket( pPacket ); pPacket->Release();
			}
		}
	}
	
	return 0;
}

BOOL CHttpClient::OnPacket(CPacketBase* pPacket)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CHttpClient send packet

BOOL CHttpClient::SendPacket(CPacketBase* pPacket)
{
	CString str;
	m_pOutput->Print( "POST / HTTP/1.0\r\n" );
	
	str.Format( "Host: %s\r\n",  inet_ntoa(m_pHost.sin_addr) );
	m_pOutput->Print( (LPCTSTR)str );
	
	m_pOutput->Print( "Connection: Keep-Alive\r\n" );
	m_pOutput->Print( "Content-Type: text/xml; charset=utf-8\r\n" );
	
	CBuffer* pBuffer = new CBuffer;
	pPacket->EncodeBody( pBuffer );
	
	str.Format( "Content-Length: %d\r\n",  pBuffer->m_nLength );
	m_pOutput->Print( (LPCTSTR)str );
	
	m_pOutput->Print( "\r\n" );
	m_pOutput->AddBuffer( pBuffer );
	
	CConnection::OnWrite();
	pPacket->Release();
	
	delete pBuffer;
	return 0;
}
