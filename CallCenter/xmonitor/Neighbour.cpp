//
// Neighbour.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "Neighbour.h"
#include "Network.h"
#include "Channel.h"
#include "Settings.h"

#include "WndMain.h"
#include "WndHome.h"

CNeighbour Neighbour;

//////////////////////////////////////////////////////////////////////
// CNeighbour construction

CNeighbour::CNeighbour()
{
}

CNeighbour::~CNeighbour()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CNeighbour clear

void CNeighbour::Clear()
{
	for ( POSITION posNext = GetIterator(); posNext; )
	{
		delete GetNext( posNext );
	}
	
	m_pList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CNeighbour attributes

BOOL CNeighbour::Setup()
{
	CMainWnd* pMainWnd = (CMainWnd*)theApp.m_pMainWnd;
	
	m_pHomeWnd		   = (CHomeWnd*)pMainWnd->GetWindow( RUNTIME_CLASS(CHomeWnd) );
	ASSERT( m_pHomeWnd->GetSafeHwnd() != NULL );
	
	return TRUE;
}

int CNeighbour::GetChString(int nChType, CString& xChType) const
{
	for ( int nSuffix = 0; m_pszChSuffix[ nSuffix ]; nSuffix++ )
	{
		LPCTSTR xSuffix = m_pszChSuffix[ nSuffix ];
		
		CHAR xCh;
		CHAR xGroup[16], xImage[16];
		LPTSTR pChString = xChType.GetBuffer( 80 );
		ZeroMemory( xGroup, 16 ); ZeroMemory( xImage, 16 );
		
		sscanf( m_pszChSuffix[ nSuffix ], "%[^^]%c%[^^]%c%[^\0]",
				xGroup, &xCh, xImage, &xCh, pChString );
		xChType.ReleaseBuffer();
		
		if ( _ttoi(xGroup) == nChType ) return _ttoi(xImage);
	}

	xChType = _T("未知"); return -1;
}

//////////////////////////////////////////////////////////////////////
// CNeighbour connect

BOOL CNeighbour::Connect()
{
	CNeighbour::Setup();
	
	DWORD nAddress = inet_addr( (LPCTSTR)Settings.Center.Host );
	CConnection::ConnectTo( (IN_ADDR*)&nAddress, (WORD)Settings.Center.Port );
	
	WSAEventSelect( m_hSocket, Network.m_pWakeup, FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CNeighbour disconnect

void CNeighbour::Disconnect()
{
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	if ( IsConnected() )
	{
TRACE("%i -> nei to clear...\n",GetTickCount());
		m_pHomeWnd->ClearWindow();
TRACE("%i -> nei to clear ok...\n",GetTickCount());
	}
	
	Clear();
	
	CTransfer::Disconnect();
}

//////////////////////////////////////////////////////////////////////
// CNeighbour run function

BOOL CNeighbour::OnRun()
{
	if ( ! CTransfer::OnRun() ) return FALSE;

	DWORD tNow = GetTickCount();
	
	if ( ! IsConnected() && tNow - m_tConnected >= 60* 1000 )
	{
		OnDropped( FALSE ); return FALSE;
	}

	return TRUE;
}

BOOL CNeighbour::OnConnected()
{
	CTransfer::OnConnected();
#if 0	
	{
		IAchnPacket* pPacket = new IAchnPacket;
		pPacket->ChType = 2;
		pPacket->ChFirst = 4;
		pPacket->ChLength = 50;

		OnAchnPacket( pPacket); pPacket->Release();
	}
	{
		IAchnPacket* pPacket = new IAchnPacket;
		pPacket->ChType = 5;
		pPacket->ChFirst = 0;
		pPacket->ChLength = 3;

		OnAchnPacket( pPacket); pPacket->Release();
	}
	
	{
		IAchnPacket* pPacket = new IAchnPacket;
		pPacket->ChType = 2;
		pPacket->ChFirst = 180;
		pPacket->ChLength = 120;

		OnAchnPacket( pPacket); pPacket->Release();
	}
#endif
	return TRUE;
}

#if 0
IPacket* CNeighbour::ReadBuffer(CBuffer* pBuffer)
{
	CString strLine;
	ASSERT( pBuffer != NULL );
	if ( ! pBuffer->ReadString( strLine ) ) return NULL;
	
	LPBYTE pByte = (LPBYTE)strLine.GetBuffer(0);
	int nLength = strLine.GetLength();

	IPacket* pPacket = IPacket::ReadBuffer( pByte, nLength );

	if ( pPacket == NULL )
	{
		//SendPacket( new IErrPacket(ICP_ERROR) ); return NULL;
		IAchnPacket* pPacket2 = new IAchnPacket;
		pPacket2->ChType = GetTickCount() % 10;
		pPacket2->ChFirst = GetTickCount() % 100;
		pPacket2->ChLength = GetTickCount() % 256;
		return pPacket2;
	}

	return pPacket;
}
#endif
//////////////////////////////////////////////////////////////////////
// CNeighbour TCP Transfer func

void CNeighbour::OnPacket(IPacket* pPacket)
{
	CTransfer::OnPacket( pPacket );
	
	if ( pPacket->IsCommandId( ICP_ACHN ) )
	{
		OnAchnPacket( (IAchnPacket*)pPacket );
	}
	else
	if ( pPacket->IsCommandId( ICP_ECHN ) )
	{
		OnEchnPacket( (IEchnPacket*)pPacket );
	}
}

void CNeighbour::OnAchnPacket(IAchnPacket* pPacket)
{
	ASSERT( m_pHomeWnd != NULL );
	
	AddClient( pPacket->ChType, pPacket->ChFirst, pPacket->ChLength );
	//m_pHomeWnd->DrawChType( pPacket->ChType );
	m_pHomeWnd->SendMessage( WM_DRAWCHTYPE, pPacket->ChType );
}

void CNeighbour::OnEchnPacket(IEchnPacket* pPacket)
{
	for ( POSITION posNext = GetIterator(); posNext; )
	{
		CChannel* pChannel = GetNext( posNext );
		if ( pPacket->ChNum < pChannel->m_nChan ) continue;
		
		for ( ; pChannel; pChannel = pChannel->m_pChNext )
		{
			if ( pPacket->ChNum != pChannel->m_nChan ) continue;
			pChannel->SetChState( pPacket->ChState );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNeighbour Channel operations

void CNeighbour::AddClient(int nChType, int nChFirst, int nChLength)
{
	for ( POSITION posNext = GetIterator(), pos2 = posNext; posNext; pos2 = posNext )
	{
		CChannel* pChannel = GetNext( posNext );
		if ( nChFirst < pChannel->m_nChan ) break;
	}
	
	CChannel* pChannel = new CChannel( nChFirst, nChType );
	if ( pos2 ) m_pList.InsertBefore( pos2, pChannel );
	else m_pList.AddTail( pChannel );
	
	for ( int nChan = nChFirst + 1; nChan < nChFirst + nChLength; nChan++ )
	{
		pChannel->m_pChNext = new CChannel( nChan, nChType );
		pChannel->m_pChNext->m_pChPrev = pChannel;
		pChannel = pChannel->m_pChNext;
	}
}

//////////////////////////////////////////////////////////////////////
// CNeighbour channel suffixes

LPCTSTR CNeighbour::m_pszChSuffix[] =
{
	//_T("00^00^模拟外线"),
	//_T("01^01^内线电话"),
	_T("02^29^用户坐席"),
	//_T("03^02^录音通道"),
	_T("04^16^一号信令"),
	_T("05^31^传真通道"),
	_T("06^16^七号信令"),	// TUP
	_T("09^31^传真通道"),
	_T("11^16^七号信令"),	// ISUP
	NULL,
};
