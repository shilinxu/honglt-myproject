//
// FilterRtpReceiver.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "FilterRtpReceiver.h"

#include "RenderGraph.h"
#include "Traffic.h"
#include "Buffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter construction

CRtpReceiverFilter::CRtpReceiverFilter(Payload nPayload, WAVEFORMATEX* pwfx, CFilterGraph* pFilterGraph)
: CBaseFilter(NAME("Rtp Receiver Filter"), NULL, &m_Lock, CLSID_NULL), m_nPayloadType(nPayload), m_pRenderer(RTP_Audio)
{
	HRESULT hr = S_OK;
	m_pCapture = new CRtpOutputPin( this, &m_Lock, &hr, L"Capture" );
	
	WAVEFORMATEX wfx = {0};
	if ( ! pwfx ) DeriveCompressFormat( *(pwfx = &wfx) );
	m_pCapture->SetMediaType( pwfx );
	
	WAVEFORMATEX wfxDestFormat = {0};
	DeriveCompressFormat( wfxDestFormat, pwfx );
	
	m_pPreview = new CRtpOutputPin( this, &m_Lock, &hr, L"Preview" );
	m_pPreview->SetMediaType( &wfxDestFormat );
	
	m_pFilterGraph	= pFilterGraph;
	m_nRefStart		= 0;
	m_nRefStream	= 0;
}

CRtpReceiverFilter::CRtpReceiverFilter(Payload nPayload, VIDEOINFOHEADER* pvih, CFilterGraph* pFilterGraph)
: CBaseFilter(NAME("Rtp Receiver Filter"), NULL, &m_Lock, CLSID_NULL), m_nPayloadType(nPayload), m_pRenderer(RTP_Video)
{
	HRESULT hr = S_OK;
	m_pCapture = new CRtpOutputPin( this, &m_Lock, &hr, L"Capture" );
	
	VIDEOINFOHEADER vih = {0};
	if ( ! pvih ) DeriveCompressFormat( *(pvih = &vih) );
	m_pCapture->SetMediaType( pvih );
	
	VIDEOINFOHEADER vihDestFormat = {0};
	DeriveCompressFormat( vihDestFormat, pvih );
	
	m_pPreview = new CRtpOutputPin( this, &m_Lock, &hr, L"Preview" );
	m_pPreview->SetMediaType( &vihDestFormat );
	
	m_pFilterGraph	= pFilterGraph;
	m_nRefStart		= 0;
	m_nRefStream	= 0;
}

CRtpReceiverFilter::~CRtpReceiverFilter()
{
	CAutoLock lock(&m_Lock);
	
	if ( m_pCapture ) delete m_pCapture;
	if ( m_pPreview ) delete m_pPreview;
}

//////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter attributes

int CRtpReceiverFilter::GetPinCount()
{
    return 2;
}

CBasePin* CRtpReceiverFilter::GetPin(int n)
{
	if ( n == 0 ) return m_pCapture;
	if ( n == 1 ) return m_pPreview;
	
	return NULL;
}

BYTE CRtpReceiverFilter::GetPayloadType() const
{
	return m_nPayloadType;
}

CFilterGraph* CRtpReceiverFilter::GetFilterGraph() const
{
	return m_pFilterGraph;
}

//////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter thread run

HRESULT CRtpReceiverFilter::StartThread()
{
	if ( ! ThreadExists() ) Create();
	
	CAutoLock lock(&m_Lock);
	
	LPCTSTR xUserId = GetFilterGraph()->GetNamed();
	
	RTPSessionLayer* pRTPSession = Traffic.FindChannel(m_pRenderer);
	if ( pRTPSession ) pRTPSession->PurgeBuffer( xUserId );
	
	m_nRefStart		= 0;
	return CallWorker( TM_Start );
}

STDMETHODIMP CRtpReceiverFilter::Stop(void)
{
	CallWorker( TM_Exit );
    Close();
	
	return CBaseFilter::Stop();
}

DWORD CRtpReceiverFilter::ThreadProc(void)
{
    DbgLog((LOG_TRACE, 0, TEXT("CRtpReceiverFilter[%s] worker thread initializing"), m_pRenderer));
	DWORD nRequest;
	
	do
	{
		nRequest = GetRequest();
		
		switch ( nRequest )
		{
		case	TM_Exit:
			Reply( NOERROR ); break;
			
		case	TM_Pause:
			Reply( NOERROR ); break;
			
		case	TM_Start:
			Reply( S_OK );
			OnRun(); break;
		}
	} while( nRequest != TM_Exit );
    DbgLog((LOG_TRACE, 0, TEXT("CRtpReceiverFilter[%s] worker thread exiting"), m_pRenderer));
	
	return 0;
}

HRESULT CRtpReceiverFilter::OnRun()
{
	DWORD nRequest;
	
	do {
		CSingleLock pLock( &Traffic.m_pSection );
		
		while ( ! CheckRequest( &nRequest ) )
		{
			WaitForSingleObject( Traffic.m_pWakeup, 50 );
			
			if ( ! pLock.Lock( 250 ) ) continue;
			LPCTSTR xUserId = GetFilterGraph()->GetNamed();
			
			RTPSessionLayer* pRTPSession = Traffic.FindChannel(m_pRenderer);
			if ( pRTPSession ) pRTPSession->RenderBuffer( xUserId, RenderBuffer, this );
			
			pLock.Unlock();
		}
		
		if ( nRequest != TM_Exit )
		{
			Reply( NOERROR );
		}
	} while( nRequest != TM_Exit );
	
	return S_FALSE;
}

//////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter operations

void CRtpReceiverFilter::DeriveCompressFormat(WAVEFORMATEX& wfx)
{
	wfx.wFormatTag		= WAVE_FORMAT_ALAW;
	wfx.nSamplesPerSec	= 8000; wfx.wBitsPerSample	= 8; wfx.nChannels	= 1;
	wfx.nBlockAlign		= wfx.nChannels * ( wfx.wBitsPerSample / 8 );
	wfx.nAvgBytesPerSec	= wfx.nBlockAlign * wfx.nSamplesPerSec;
}

void CRtpReceiverFilter::DeriveCompressFormat(WAVEFORMATEX& wfxDest, WAVEFORMATEX* pwfxSrc)
{
	wfxDest.wFormatTag = WAVE_FORMAT_PCM;
	wfxDest.wBitsPerSample = 16;
	wfxDest.cbSize = 0;
	
	wfxDest.nChannels = pwfxSrc->nChannels;
	wfxDest.nSamplesPerSec = pwfxSrc->nSamplesPerSec;
	
	wfxDest.nBlockAlign = wfxDest.nChannels * wfxDest.wBitsPerSample / 8;
	wfxDest.nAvgBytesPerSec = wfxDest.nBlockAlign * wfxDest.nSamplesPerSec;
}

void CRtpReceiverFilter::DeriveCompressFormat(VIDEOINFOHEADER& vih)
{
	vih.bmiHeader.biCompression	= MAKEFOURCC('M','2','6','3');
	vih.bmiHeader.biBitCount	= 24;
	vih.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	vih.bmiHeader.biWidth		= QCIF_WIDTH;
	vih.bmiHeader.biHeight		= QCIF_HEIGHT;
	vih.bmiHeader.biPlanes		= 1;
	vih.bmiHeader.biSizeImage	= 8192;//GetBitmapSize( &vih.bmiHeader );
	vih.bmiHeader.biClrImportant= 0;
	vih.AvgTimePerFrame			= (REFERENCE_TIME)UNITS / 30;
//	vih.dwBitRate				= 15206400;
}

void CRtpReceiverFilter::DeriveCompressFormat(VIDEOINFOHEADER& vihDest, VIDEOINFOHEADER* pvihSrc)
{
	vihDest.bmiHeader.biCompression	= BI_RGB;
	
	vihDest.bmiHeader.biBitCount	= pvihSrc->bmiHeader.biBitCount;
	vihDest.bmiHeader.biSize		= pvihSrc->bmiHeader.biSize;
	vihDest.bmiHeader.biWidth		= pvihSrc->bmiHeader.biWidth;
	vihDest.bmiHeader.biHeight		= pvihSrc->bmiHeader.biHeight;
	vihDest.bmiHeader.biPlanes		= pvihSrc->bmiHeader.biPlanes;
	vihDest.bmiHeader.biSizeImage	= GetBitmapSize( &vihDest.bmiHeader );
	vihDest.bmiHeader.biClrImportant= pvihSrc->bmiHeader.biClrImportant;
	
	vihDest.AvgTimePerFrame			= pvihSrc->AvgTimePerFrame;
	vihDest.rcSource				= pvihSrc->rcSource;
	vihDest.rcTarget				= pvihSrc->rcTarget;
	vihDest.dwBitRate				= pvihSrc->dwBitRate;
}

/////////////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter deliver buffer

HRESULT CRtpReceiverFilter::DeliverBuffer(BYTE* pBuffer, DWORD nLength, DWORD nTimestamp)
{
	return RenderBuffer( pBuffer, nLength, REFERENCE_TIME(nTimestamp) * 10000 );
}
/////////////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter render buffer

HRESULT CRtpReceiverFilter::RenderBuffer(BYTE* pByte, DWORD nLength, REFERENCE_TIME rtStart)
{
	LPBYTE pBuffer;
	IMediaSample *pMediaSample;
	
	for ( int nPin = 0; nPin < GetPinCount(); nPin++ )
	{
		CBaseOutputPin* pOutput = (CBaseOutputPin*)GetPin( nPin );
		if ( pOutput == NULL ) return S_FALSE;
		
		HRESULT hr = pOutput->GetDeliveryBuffer( &pMediaSample, NULL, NULL, 0 );
		if ( SUCCEEDED(hr) && SUCCEEDED(pMediaSample->GetPointer( &pBuffer )) )
		{
			CopyMemory( pBuffer, pByte, nLength );
			hr = pMediaSample->SetActualDataLength( nLength );
			
			pMediaSample->SetTime( &rtStart, NULL );
			pMediaSample->SetMediaTime( NULL, NULL );
			pMediaSample->SetSyncPoint( TRUE );
			
			hr = pOutput->Deliver( pMediaSample );
			pMediaSample->Release();
		}
	}
#ifdef _DEBUG
//	TRACE("%u-> (%i).\n", GetTickCount(), m_nPayloadType);
	TRACE( "RX[%2i][%4i]: [%I64i][%I64i], [%I64i][%I64i] {%s}\n", m_nPayloadType, nLength,
		rtStart, (REFERENCE_TIME)0, (REFERENCE_TIME)0, (REFERENCE_TIME)-1, ToHex( pByte, 100 ) );
#endif
	
	return S_OK;
}

HRESULT CALLBACK CRtpReceiverFilter::RenderBuffer(LPVOID lpHandle, RTPPayload* pPayload)
{
	CRtpReceiverFilter* pReceiver = (CRtpReceiverFilter*)lpHandle;
	
	if ( pReceiver->m_State == State_Stopped ) return S_OK;
	
	if ( pReceiver->m_State == State_Paused )
	{
		return pReceiver->DeliverBuffer( pPayload->PayloadBuffer.m_pBuffer, pPayload->PayloadBuffer.m_nLength, 0 );
	}
	
	volatile DWORD& nRefStream = pReceiver->m_nRefStream; if ( ! nRefStream )
	{
		REFERENCE_TIME rtTime;
		pReceiver->StreamTime( (CRefTime&)rtTime );
		
		nRefStream  = ntohl( pPayload->timestamp );
		nRefStream -= rtTime / 10000;
	}
	
	return pReceiver->DeliverBuffer( pPayload->PayloadBuffer.m_pBuffer, pPayload->PayloadBuffer.m_nLength,
		ntohl( pPayload->timestamp ) - nRefStream );
}

//////////////////////////////////////////////////////////////////////
// CRtpOutputPin construction

CRtpOutputPin::CRtpOutputPin(CRtpReceiverFilter* pFilter, CCritSec* pLock, HRESULT *phr, LPCWSTR lpszName)
: CBaseOutputPin(NAME("Output Pin"), pFilter, pLock, phr, lpszName)
{
	HACMStream	= NULL;
	m_hIC		= NULL;
	m_pBuffer	= NULL;
	
	ZeroMemory(&ACMStreamHeader, sizeof(ACMSTREAMHEADER));
}

CRtpOutputPin::~CRtpOutputPin()
{
	if ( m_pBuffer ) delete [] m_pBuffer;
}

//////////////////////////////////////////////////////////////////////
// CRtpOutputPin attributes

HRESULT CRtpOutputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
	if ( iPosition < 0 ) return E_INVALIDARG;
	if ( iPosition > 0 ) return VFW_S_NO_MORE_ITEMS;
	
	*pMediaType = CBaseOutputPin::m_mt;
	return S_OK;
}

HRESULT CRtpOutputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
	
	if ( *pMediaType == CBaseOutputPin::m_mt ) return NOERROR;
	
	return E_INVALIDARG;
}

HRESULT CRtpOutputPin::CompleteConnect(IPin *pReceivePin)
{
	CAutoLock lock(m_pLock);
	
	if ( m_mt.formattype == FORMAT_WaveFormatEx )
	{
		WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();
		if ( InitializeCodec( Name(), pwfx ) ) return E_FAIL;
		
		m_pBuffer	= new BYTE[pwfx->nAvgBytesPerSec]; 
		ACMStreamHeader.pbDst = m_pBuffer; 
		ACMStreamHeader.cbDstLength = pwfx->nAvgBytesPerSec;
	}
	else
	if ( m_mt.formattype == FORMAT_VideoInfo )
	{
		VIDEOINFOHEADER* pvih = (VIDEOINFOHEADER*)m_mt.Format();
		if ( InitializeCodec( Name(), pvih ) ) return E_FAIL;
		
		m_pBuffer	= new BYTE [pvih->bmiHeader.biSizeImage];
	}
	
	return CBaseOutputPin::CompleteConnect(pReceivePin);
}

HRESULT CRtpOutputPin::BreakConnect(void)
{
	CAutoLock lock(m_pLock);
	
	if ( m_mt.formattype == FORMAT_WaveFormatEx )
	{
		if ( HACMStream ) acmStreamClose( HACMStream, 0 );
		HACMStream	= NULL;
	}
	if ( m_mt.formattype == FORMAT_VideoInfo )
	{
		ICDecompressEnd( m_hIC );
		ICClose( m_hIC ); m_hIC = NULL;
	}
	
	if ( m_pBuffer ) delete [] m_pBuffer;
	m_pBuffer	= NULL;
	
	return CBaseOutputPin::BreakConnect();
}

MMRESULT CRtpOutputPin::InitializeCodec(LPCWSTR Id, WAVEFORMATEX* pwfx)
{
	if ( wcscmp( Id, L"Preview" ) ) return MMSYSERR_NOERROR;
	CRtpReceiverFilter* pTransformFilter = (CRtpReceiverFilter *)m_pFilter;
	
	WAVEFORMATEX wfxSrc;
	ZeroMemory( &wfxSrc, sizeof( wfxSrc ) );
	pTransformFilter->DeriveCompressFormat( wfxSrc );
	
	ZeroMemory(&ACMStreamHeader, sizeof(ACMSTREAMHEADER));
	ACMStreamHeader.cbStruct = sizeof(ACMStreamHeader);
	
	return acmStreamOpen( &HACMStream, NULL, &wfxSrc, pwfx,
		NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME );
}

DWORD CRtpOutputPin::InitializeCodec(LPCWSTR Id, VIDEOINFOHEADER* pvih)
{
	if ( wcscmp( Id, L"Preview" ) ) return ICERR_OK;
	CRtpReceiverFilter* pTransformFilter = (CRtpReceiverFilter *)m_pFilter;
	
	VIDEOINFOHEADER vihSrc;
	ZeroMemory( &vihSrc, sizeof( vihSrc ) );
	pTransformFilter->DeriveCompressFormat( vihSrc );
	
	CopyMemory( &m_bmiInput, &vihSrc.bmiHeader, sizeof(BITMAPINFOHEADER) );
	//CopyMemory( &m_bmiOutput, &pvih->bmiHeader, sizeof(BITMAPINFOHEADER) );
	
	m_hIC = ICOpen(ICTYPE_VIDEO, m_bmiInput.biCompression,ICMODE_DECOMPRESS);
	if ( m_hIC == NULL ) return ICERR_ERROR;
	
	ICSendMessage(m_hIC,0x60c9,0xf7329ace,0xacdeaea2);
	ICDecompressGetFormat( m_hIC, &m_bmiInput, &m_bmiOutput );

	return ICDecompressBegin( m_hIC, &m_bmiInput, &m_bmiOutput );
}

HRESULT CRtpOutputPin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	HRESULT hr = NOERROR;
	if ( ! IsConnected() ) return E_UNEXPECTED;
	
	CheckPointer( pAlloc, E_POINTER );
	CheckPointer( pProperties, E_POINTER );
	
	if (m_mt.formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * info = (VIDEOINFOHEADER *) m_mt.pbFormat;
		pProperties->cbBuffer = info->bmiHeader.biSizeImage;
	}
	else
	{
		WAVEFORMATEX * info = (WAVEFORMATEX *) m_mt.pbFormat;
		pProperties->cbBuffer = info->nAvgBytesPerSec;
	}
	
	pProperties->cBuffers = 1;
	pProperties->cbAlign  = 1;
	
	ASSERT( pProperties->cbBuffer );
	
	ALLOCATOR_PROPERTIES Actual;
	hr = pAlloc->SetProperties(pProperties,&Actual);
	if ( FAILED(hr) ) return hr;
	
//	ASSERT( Actual.cBuffers == 1 );
	
	if (pProperties->cBuffers > Actual.cBuffers ||
		pProperties->cbBuffer > Actual.cbBuffer)
	{
		return E_FAIL;
	}
	
	return NOERROR;
}

//////////////////////////////////////////////////////////////////////
// CRtpOutputPin operations

HRESULT CRtpOutputPin::SetMediaType(WAVEFORMATEX* pwfx)
{
	CheckPointer( pwfx, E_POINTER );
	
	CMediaType pMediaType;
	CreateAudioMediaType( pwfx, &pMediaType, TRUE );
	
	CBaseOutputPin::SetMediaType( &pMediaType );
	
	return S_OK;
}

HRESULT CRtpOutputPin::SetMediaType(VIDEOINFOHEADER* pvih)
{
	CheckPointer( pvih,E_POINTER );
	
	CMediaType pMediaType;
	
	SetRectEmpty( &pvih->rcSource );
	SetRectEmpty( &pvih->rcTarget );
	
	pMediaType.SetType( &MEDIATYPE_Video );
	pMediaType.SetFormatType( &FORMAT_VideoInfo );
//	pMediaType.SetTemporalCompression( FALSE );
	
	const GUID subType = GetBitmapSubtype(&pvih->bmiHeader);
	
	pMediaType.SetSubtype( &subType );
	pMediaType.SetSampleSize( pvih->bmiHeader.biSizeImage );
	
	pMediaType.SetFormat( (BYTE*)pvih, sizeof(VIDEOINFOHEADER) );
	CBaseOutputPin::SetMediaType( &pMediaType );
	
	return S_OK;
}

HRESULT CRtpOutputPin::Deliver(IMediaSample *pMediaSample)
{
	if ( wcscmp( Name(), L"Preview" ) )
	{
		return CBaseOutputPin::Deliver(pMediaSample);
	}
	
	LPBYTE pBuffer ;
	
	HRESULT hr = pMediaSample->GetPointer( &pBuffer );
	if ( FAILED(hr) ) return hr;
	
	int nLength = pMediaSample->GetActualDataLength();
	
	if ( m_mt.formattype == FORMAT_WaveFormatEx && HACMStream )
	{
		ACMStreamHeader.pbSrc = pBuffer;
		ACMStreamHeader.cbSrcLength = nLength;
		
		acmStreamPrepareHeader( HACMStream, &ACMStreamHeader, 0 );
		
		if ( acmStreamConvert( HACMStream, &ACMStreamHeader, 0 ) ) return E_FAIL;
		acmStreamUnprepareHeader( HACMStream, &ACMStreamHeader, 0 );
		
		nLength = ACMStreamHeader.cbDstLengthUsed;
		CopyMemory( pBuffer, ACMStreamHeader.pbDst, nLength );
		pMediaSample->SetActualDataLength( nLength );
	}
	else
	if ( m_mt.formattype == FORMAT_VideoInfo && m_hIC )
	{
		if ( ICDecompress(m_hIC, 0, &m_bmiInput, pBuffer, &m_bmiOutput, m_pBuffer) ) return E_FAIL;
		
		nLength = m_bmiOutput.biSizeImage;
		CopyMemory( pBuffer, m_pBuffer, nLength );
		pMediaSample->SetActualDataLength( nLength );
	}
	
//	pMediaSample->SetTime( NULL, NULL );
	return CBaseOutputPin::Deliver(pMediaSample);
}

//////////////////////////////////////////////////////////////////////
// CRtpOutputPin thread run

HRESULT CRtpOutputPin::Active(void)
{
	CAutoLock lock(m_pLock);
	
	if ( m_pFilter->IsActive() ) return S_FALSE;
	
	if ( ! IsConnected() ) return NOERROR;
	
	HRESULT hr = CBaseOutputPin::Active();
	if ( FAILED(hr) ) return hr;
	
	CRtpReceiverFilter* pTransformFilter = (CRtpReceiverFilter *)m_pFilter;
	hr = pTransformFilter->StartThread();
	
	return hr;
}

STDMETHODIMP CRtpOutputPin::Notify(IBaseFilter * pSelf, Quality q)
{
	return NOERROR;
}

