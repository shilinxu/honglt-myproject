//
// CtrlNetworkVideo.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "CtrlNetworkVideo.h"
#include "CtrlNetworkChild.h"
#include "Settings.h"

#include "Neighbour.h"
#include "Circuitry.h"
#include "Datagrams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CNetworkVideo, CTaskBox)
BEGIN_MESSAGE_MAP(CNetworkVideo, CTaskBox)
	//{{AFX_MSG_MAP(CNetworkVideo)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_STARTPREVIEW, OnStartPreview)
END_MESSAGE_MAP()

#define TEMP_BUFFER		400
#define MASK_BUFFER		4000

//////////////////////////////////////////////////////////////////////
// CNetworkVideo construction

CNetworkVideo::CNetworkVideo(CNetworkChild* pPanel, LPCTSTR xUserId)
: CUserChannel(xUserId, pPanel->m_xGroupId), CGraphRender(xUserId)
{
	m_pPanel		= pPanel;
	m_bRecording	= FALSE;
	
	HACMPcm			= HACMStream = NULL;
	m_pMixedPtr		= NULL;
	m_pBuffer		= NULL;
	
	Create( pPanel, WINDOW_HEIGHT, xUserId, IDI_TASKPANEL );
}

CNetworkVideo::~CNetworkVideo()
{
	GetPanel()->RemoveBox( this );
	
	if ( m_pBuffer ) delete m_pBuffer;
}

void CNetworkVideo::Disconnect()
{
	CUserChannel::Disconnect( 0 );
	
	SendMessage( WM_CLOSE );
}

void CNetworkVideo::PostNcDestroy()
{
	if ( HACMPcm ) acmStreamClose( HACMPcm, 0 );
	if ( HACMStream ) acmStreamClose( HACMStream, 0 );
	
	HACMPcm = HACMStream = NULL;
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo operations

CNetworkChild* CNetworkVideo::GetPanel() const
{
	ASSERT( m_pPanel != NULL );
	ASSERT_KINDOF( CNetworkChild, m_pPanel );
	return m_pPanel;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo message handlers

int CNetworkVideo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	WAVEFORMATEX wfx = {0};
	DeriveCompressFormat( wfx );
	
	InitializeMixer( &wfx );
	
	SetTimer( 5, 5000, SyncMirrorView );
	SetTimer( 1, 1000, NULL );
	EnableToolTips( TRUE );
	
	CDatagrams* pDatagram = Datagrams.SeekNext( Payload_Alaw );
	SendMessage( WM_STARTPREVIEW, PreviewPayload(PayloadMask_Audio), (LPARAM)pDatagram );
	
	pDatagram = Datagrams.SeekNext( Payload_H263 );
	SendMessage( WM_STARTPREVIEW, PreviewPayload(PayloadMask_Video), (LPARAM)pDatagram );
	
	return 0;
}

void CNetworkVideo::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetPanel()->SelectTo( this );
}

void CNetworkVideo::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnLButtonDown( nFlags, point );
}

void CNetworkVideo::OnClose()
{
	DestroyWindow();
}

int CNetworkVideo::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	if ( ! pTI ) return 1;
	
	pTI->uFlags		= TTF_IDISHWND;
	pTI->hwnd		= GetSafeHwnd();
	pTI->uId		= (UINT)GetSafeHwnd();
	
	CString strText;
	strText.Format( "IP: %s:%i", inet_ntoa(m_pHost.sin_addr), ntohs(m_pHost.sin_port) );
	
	pTI->lpszText	= _tcsdup( strText );

	return pTI->uId;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo common timer

void CNetworkVideo::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent != 1 ) return;
	
	if ( _tcsicmp( m_xGroupId, GUEST_CODE ) ) return;
	CSingleLock pLock( &Neighbour.m_pSection, TRUE );
	
	CCircuitry* pChannel = Neighbour.FindChannel( m_xUserId );
	if ( pChannel ) pChannel->TimerOut( Settings.General.TimeOut* 1000 );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo custom message handlers

LONG CNetworkVideo::OnStartPreview(WPARAM nPayloadMask, LPARAM lParam)
{
	if ( (PreviewPayload(0) & nPayloadMask) == PreviewPayload(0) )
	{
		RenderPreview( PreviewPayload(nPayloadMask), NULL, (LPVOID)lParam );
		CRect rc(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		
		HRESULT hr = CGraphRender::StartPreview( rc, GetSafeHwnd() );
		Expand( SUCCEEDED(hr) );
	}
	else
	if ( (RecordPayload(0) & nPayloadMask) == RecordPayload(0) )
	{
		OnRecordFile( RecordPayload(nPayloadMask), Settings.Record.RecFile );
	}
	else
	{
		ASSERT( FALSE );
	}
	
	return 0;
}

HRESULT CNetworkVideo::OnRecordFile(WORD nPayloadMask, LPCTSTR fMask)
{
	if ( ! (m_bRecording ^= TRUE) || nPayloadMask == RecordPayload(0) ) return StopRecord();
	
	CString strFile = GetRecordFile( fMask );
	
	HRESULT hr = RecordFile( CComBSTR(Settings.Record.PathRec+'\\'+strFile), nPayloadMask );
	
	if ( FAILED(hr) ) return hr;
	theApp.Message( MSG_TEMP, "Record[%s]: %s", m_xUserId, strFile );
	
	try
	{
		CString	strQuery;
		
		strQuery.Format( "insert into xuservoice"
						" (xPCMName,xPhoneNum,xRecDate,xCaller,xTheState)"
						" values('%s','%s',getdate(),'%s','DELIVRD')",
						Settings.Record.PathRec+'\\'+strFile, m_xUserId, m_xRemoteId );
		TRACE( strQuery + "\n" );
		xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	}
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on channel '%s': %s", m_xUserId, e.ErrorMessage() );
		return E_FAIL;
	}
	
	return hr;
}

//////////////////////////////////////////////////////////////////////
// CNetworkVideo TCP Transfer func

BOOL CNetworkVideo::OnSyncMonitorView(CXMLElement* pXML)
{
	if ( pXML && ! pXML->GetAttribute( _T("leave") ) )
		return GetPanel()->JoinedMirror( this, SyncMirrorClock );
	else
		return GetPanel()->LeavedMirror( this );
}

void CALLBACK CNetworkVideo::SyncMirrorClock(CNetworkVideo* pView)
{
	if ( ! pView->GetSafeHwnd() ) return;
	
	pView->SetTimer( 5, 5000, SyncMirrorView );
}

void CALLBACK CNetworkVideo::SyncMirrorView(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	CNetworkVideo *pView = (CNetworkVideo *) CWnd::FromHandle( hWnd );
	
	pView->GetPanel()->JoinedMirror( pView );
	
	pView->KillTimer( 5 );
}

BOOL CNetworkVideo::OnSyncHostRecording(CXMLElement* pXML)
{
	DWORD nPayloadMask = Settings.Record.RecMask;
	
	return PostMessage( WM_STARTPREVIEW, nPayloadMask );
}
/*
BOOL CNetworkVideo::OnSyncHostTalking(CXMLElement* pXML)
{
	GetPanel()->OnSyncHostTalking(pXML, this);
	
	return CUserChannel::OnSyncHostTalking(pXML);
}
*/
/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo mirror message handlers

BOOL CNetworkVideo::JoinedMirror(CNetworkVideo* pMirror)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncMirrorView );
	pXML->AddAttribute( "method", "mirror" );
	
	pXML->AddAttribute( "UserId", m_xUserId );
	
	Datagrams.FormatXML( pXML, m_xUserId );
	pMirror->SendPacket( new CSyncOrderRelation(pXML) );
	
	delete pXML;
	return TRUE;
}

BOOL CNetworkVideo::LeavedMirror(CNetworkVideo* pMirror)
{
	if ( ! pMirror->IsConnected() ) return FALSE;
	CXMLElement* pXML = new CXMLElement( NULL, ISyncMirrorView );
	
	pXML->AddAttribute( "method", "mirror" );
	pXML->AddAttribute( "leave" );
	
	pXML->AddAttribute( "UserId", m_xUserId );
	
	pMirror->SendPacket( new CSyncOrderRelation(pXML) );
	
	delete pXML;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo deliver buffer

HRESULT CNetworkVideo::DeliverBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType, DWORD nTimestamp)
{
	if ( nType == Payload_Alaw )
	{
		MixedBuffer( pBuffer, nLength, 0 );
		return DeliverBuffer( m_pBuffer, m_pMixedPtr );
	}
	
	GetPanel()->DeliverBuffer( pBuffer, nLength, nType, nTimestamp, m_xUserId );
	return CGraphRender::DeliverBuffer( pBuffer, nLength, nType, nTimestamp );
}


HRESULT CNetworkVideo::DeliverBuffer(CBuffer* pBuffer, BYTE*& pMixedPtr)
{
	DWORD nMixedLen = DWORD( pMixedPtr - pBuffer->m_pBuffer );
	if ( pBuffer->m_nLength >= MASK_BUFFER ) nMixedLen = max(nMixedLen, TEMP_BUFFER);
	
	if ( nMixedLen < TEMP_BUFFER ) return S_OK;
	
	DWORD nTimestamp = GetTickCount();
	GetPanel()->DeliverBuffer( pBuffer->m_pBuffer, nMixedLen, Payload_Alaw, nTimestamp, m_xUserId );
	
	CGraphRender::RenderBuffer( pBuffer->m_pBuffer, nMixedLen, Payload_Alaw );
	pBuffer->Remove( nMixedLen );
	
	pMixedPtr = pBuffer->m_pBuffer;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo render buffer

HRESULT CNetworkVideo::RenderBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType)
{
	ASSERT ( nType == Payload_Alaw );
	
	MixedBuffer( pBuffer, nLength, 1 );
	return DeliverBuffer( m_pBuffer, m_pMixedPtr );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo mixed buffer

HRESULT CNetworkVideo::MixedBuffer(BYTE* pBuffer, DWORD nLength, DWORD nDeliverId)
{
	if ( ! m_pBuffer ) m_pBuffer = new CBuffer;
	
	DWORD nMixedLen = DWORD( m_pMixedPtr - m_pBuffer->m_pBuffer );
	if ( ! m_pMixedPtr || ! m_pBuffer->m_nLength ) m_nDeliverId = nDeliverId;
	
	if ( m_nDeliverId != nDeliverId )
	{
		DWORD nNeedMix = min( m_pBuffer->m_nLength - nMixedLen, nLength );
		if ( nNeedMix ) MixACMStreamWith( m_pMixedPtr, pBuffer, nNeedMix );
		
		nMixedLen += nNeedMix; pBuffer += nNeedMix;
		if ( (nLength -= nNeedMix) > 0 ) m_nDeliverId = nDeliverId;
	}
	
	m_pBuffer->Add( pBuffer, nLength );
	m_pMixedPtr = m_pBuffer->m_pBuffer + nMixedLen;

	return S_OK;
}

MMRESULT CNetworkVideo::ConvertACMStream(HACMSTREAM has, BYTE* pSource, BYTE* pDest, DWORD nLength, float nPercent)
{
	ACMSTREAMHEADER ACMStreamHeader;
	ZeroMemory(&ACMStreamHeader, sizeof(ACMSTREAMHEADER));
	
	ACMStreamHeader.cbStruct = sizeof(ACMStreamHeader);
	ACMStreamHeader.pbSrc = pSource;
	ACMStreamHeader.cbSrcLength = nLength;
	
	ACMStreamHeader.pbDst = (BYTE*)pDest; 
	ACMStreamHeader.cbDstLength = nLength * nPercent; 
	
	MMRESULT mmr = acmStreamPrepareHeader( has, &ACMStreamHeader, 0 );
	if ( ! mmr ) mmr = acmStreamConvert( has, &ACMStreamHeader, 0 );
	
	acmStreamUnprepareHeader( has, &ACMStreamHeader, 0 );
	
	return mmr;
}

MMRESULT CNetworkVideo::MixACMStreamWith(BYTE* pBuffer, BYTE* pByte, DWORD nLength)
{
	MMRESULT mmr;
	
	// 还原成16位PCM
	
	WORD* pPCMBuff1 = new WORD[ nLength ];
	WORD* pPCMBuff2 = new WORD[ nLength ];
	
	mmr = ConvertACMStream( HACMStream, pBuffer, (BYTE*)pPCMBuff1, nLength, 2 );
	if ( ! mmr ) mmr = ConvertACMStream( HACMStream, pByte, (BYTE*)pPCMBuff2, nLength, 2 );
	
	if ( mmr && pPCMBuff1 ) delete [] pPCMBuff1;
	if ( mmr && pPCMBuff2 ) delete [] pPCMBuff2;
	
	if ( mmr ) return mmr;
	
	// 混音PCM
	
	for ( int i = 0 ; i < nLength ; i++ )   
	{
#if 0
		pPCMBuff1[i] = ( pPCMBuff1[i] + pPCMBuff2[i] );
#else
		LONG nMixed = (SHORT)pPCMBuff1[i] + (SHORT)pPCMBuff2[i];
		
		pPCMBuff1[i] = min( nMixed, 32767 );
		pPCMBuff1[i] = max( nMixed, -32768 );
#endif
	}
	
	// 压缩
	
	mmr = ConvertACMStream( HACMPcm, (BYTE*)pPCMBuff1, pBuffer, nLength * 2, 0.5 );
	
	delete [] pPCMBuff1;
	delete [] pPCMBuff2;
	
	return mmr;
}

HRESULT CNetworkVideo::InitializeMixer(WAVEFORMATEX* pwfx)
{
	WAVEFORMATEX wfxRgn;
	DeriveCompressFormat( wfxRgn, pwfx );
	
	if ( acmStreamOpen( &HACMPcm, NULL, pwfx, &wfxRgn,
		NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME ) ) return E_FAIL;
	
	if ( acmStreamOpen( &HACMStream, NULL, &wfxRgn, pwfx,
		NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME ) ) return E_FAIL;
	
	return S_OK;
}
