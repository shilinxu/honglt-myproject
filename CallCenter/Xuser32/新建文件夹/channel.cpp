//
// Channel.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Channel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CChannel construction

CChannel::CChannel()
{
}

CChannel::~CChannel()
{
	Disconnect();
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

//////////////////////////////////////////////////////////////////////
// CChannel socket event handlers

void CChannel::OnHandshake()
{
	CTransfer::OnHandshake();
}

void CChannel::OnDropped(int nError)
{
	CTransfer::OnDropped( nError );
	
	CErrPacket* pPacket = new CErrPacket( nError );
	SendMessage( theApp.m_pMainWnd->GetSafeHwnd(), WM_PACKET123, (WPARAM)pPacket, 0L );
	
	pPacket->Release();
}

//////////////////////////////////////////////////////////////////////
// CChannel TCP Transfer func

BOOL CChannel::OnPacket(CPacket* pPacket)
{
	return CTransfer::OnPacket(pPacket);
}

BOOL CChannel::OnOkPacket(COkPacket* pPacket)
{
	if ( theApp.m_pMainWnd )
	SendMessage( theApp.m_pMainWnd->GetSafeHwnd(), WM_PACKET123, (WPARAM)pPacket, 0L );
	else
	if ( theApp.m_pActiveWnd )
	SendMessage( theApp.m_pActiveWnd->GetSafeHwnd(), WM_PACKET123, (WPARAM)pPacket, 0L );
	
	return TRUE;
}

BOOL CChannel::OnErrPacket(CErrPacket* pPacket)
{
	SendMessage( theApp.m_pMainWnd->GetSafeHwnd(), WM_PACKET123, (WPARAM)pPacket, 0L );
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CChannel packet unserialize

CPacket* CChannel::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	if ( ! pBuffer->ReadLine( strLine ) ) return NULL;
	
	LPBYTE pByte = (LPBYTE)strLine.GetBuffer(0);
	int nLength = strLine.GetLength();
	
	CPacket* pPacket = CPacketBase::ReadBuffer( pByte, nLength );
	
	if ( pPacket == NULL )
	{
#ifdef _DEBUG
		afxDump << _T("Invalid: ") << (LPCTSTR)strLine << "\n";
#endif
		SendPacket( new CErrPacket(EBase_Unknown) ); return NULL;
	}
	
	return pPacket;
}
