//
// Channel.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Channel.h"
#include "Network.h"

#include "WndMain.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CChannel construction

CChannel::CChannel(LPCTSTR xUserId)
{
	m_xUserId	 = _tcsdup( xUserId );
}

CChannel::CChannel(SOCKET hSocket, SOCKADDR_IN* pHost) : CTransfer( hSocket, pHost )
{
	CString strTemp;
	srand( GetTickCount() );
	
	strTemp.Format( _T("#%i"), rand() );
	m_xUserId	= _tcsdup( strTemp );
}

CChannel::~CChannel()
{
	Disconnect();
	
	free( m_xUserId ); m_xUserId = NULL;
}

void CChannel::Release()
{
	delete this;
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

void CChannel::SetUserId(LPCTSTR xUserId)
{
	free( m_xUserId );
	
	m_xUserId = _tcsdup( xUserId );
}

//////////////////////////////////////////////////////////////////////
// CChannel TCP Transfer func

void CChannel::OnHandshake()
{
	SendPacket( new IOkPacket );
	
	CTransfer::OnHandshake();
}

void CChannel::OnDropped(BOOL bError)
{
	CTransfer::OnDropped( bError );
	
	CMainWnd* pMainWnd = (CMainWnd*)theApp.m_pMainWnd;
	int nResult = pMainWnd->SendMessage( WM_BINDUSER, (WPARAM)this, NULL );
}

BOOL CChannel::OnPacket(IPacket* pPacket)
{
	if ( pPacket->IsCommandId( ICP_EXIT ) )
	{
		return OnExitPacket( (IExitPacket*)pPacket );
	}
	else
	if ( pPacket->IsCommandId( ICP_USER ) )
	{
		int nResult = OnUserPacket( (IUserPacket*)pPacket );
		if ( nResult ) SendPacket( new IErrPacket( nResult ) );
		
		//return nResult == ICP_SUCCESS;
		delete this; return FALSE;
	}
	
	return CTransfer::OnPacket( pPacket );
}

BOOL CChannel::OnExitPacket(IExitPacket* pPacket)
{
	CMainWnd* pMainWnd = (CMainWnd*)theApp.m_pMainWnd;
	int nResult = pMainWnd->SendMessage( WM_BINDUSER, (WPARAM)this, NULL );
	
	return FALSE;
}

BOOL CChannel::OnUserPacket(IUserPacket* pPacket)
{
	CHAR xUserId[MAX_BUFFER];
	xUserId[ pPacket->GetUserId(xUserId) ] = '\0';
	
	int nResult = ICP_SUCCESS;
	CHAR xUserPwd[MAX_BUFFER];
	ZeroMemory( xUserPwd, MAX_BUFFER );
	
	if ( nResult = OnReadUser( xUserId, xUserPwd ) )
	{
		SendPacket( new IErrPacket( nResult ) ); return FALSE;
	}
	
	if ( ! pPacket->IsHashUsrCompatible(xUserId, xUserPwd) )
	{
		SendPacket( new IErrPacket( ICP_NOUSER ) ); return FALSE;
	}
	
	CMainWnd* pMainWnd = (CMainWnd*)theApp.m_pMainWnd;
	pMainWnd->SendMessage( WM_BINDUSER, (WPARAM)this, (LPARAM)xUserId );
	
	return 0;
}

int CChannel::OnReadUser(LPCTSTR xUserId, LPTSTR xUserPwd)
{
	try
	{
	CString strQuery;
	strQuery.Format( "select * from xuser_info"
					" where xuserid = '%s'", xUserId );
	TRACE( strQuery + "\n" );
	theApp.Message( MSG_DEBUG, strQuery );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordset->adoEOF ) return ICP_NOUSER;
	
	_variant_t TheValue = xRecordset->GetCollect( "xuserpwd" );
	if ( VT_NULL == TheValue.vt ) return 0;
	
	CopyMemory( xUserPwd, (LPCTSTR)(_bstr_t)TheValue, MAX_BUFFER );
	}
	catch (_com_error e)
	{
		return ICP_ERROR;
	}
	return 0;
}
