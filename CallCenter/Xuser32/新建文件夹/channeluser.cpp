//
// ChannelUser.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "ChannelUser.h"
#include "Network.h"
#include "XML.h"

//////////////////////////////////////////////////////////////////////
// CUserChannel construction

CUserChannel::CUserChannel()
{
}

CUserChannel::~CUserChannel()
{
//	CVideoCapture::Release();
}

//////////////////////////////////////////////////////////////////////
// CUserChannel attributes

void CUserChannel::Setup()
{
	CChannel::Setup();
//	CVideoBuilder::Setup();
}

void CUserChannel::Release()
{
//	CVideoCapture::Release();
	CChannel::Release();
}

//////////////////////////////////////////////////////////////////////
// CUserChannel TCP Transfer func

BOOL CUserChannel::OnPacket(CPacket* pPacket)
{
	if ( pPacket->IsCommandId( IBase_SyncOrderRelation ) )
	{
		return OnSyncOrderRelationReq( (CSyncOrderRelation*)pPacket );
	}
	
	return CChannel::OnPacket(pPacket);
}

BOOL CUserChannel::OnSyncOrderRelationReq(CSyncOrderRelation* pPacket)
{
	WaitForSingleObject( Network.m_pWakeup, 500 );
	CConnection::OnRead();
	
	CString strLine;
	CMapStringToString pResponseHeaders;
	
	while ( m_pInput->ReadLine(strLine, FALSE) && strLine.GetLength() )
	{
		int nColon = strLine.Find( ':' );
		if ( nColon > 0 )
		{
			pResponseHeaders.SetAt( strLine.Left(nColon), strLine.Mid(nColon + 1) );
		}
	}
	
	CString strValue;
	pResponseHeaders.Lookup("Content-Length", strValue);
	DWORD nLength = (DWORD)_ttoi(strValue);
	
	if ( m_pInput->m_nLength < nLength ) return FALSE;
	
	
	if ( CXMLElement* pXML = CXMLElement::FromBytes(m_pInput->m_pBuffer, nLength) )
	{
		SendMessage( theApp.m_pMainWnd->GetSafeHwnd(), WM_UPDATESERVICE, (WPARAM)pXML, 0L );
		delete pXML;
	}
	
	pResponseHeaders.RemoveAll();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUserChannel operations
