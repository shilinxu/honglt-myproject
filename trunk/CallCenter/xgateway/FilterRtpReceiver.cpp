//
// FilterRtpReceiver.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "FilterRtpReceiver.h"

#include "RenderGraph.h"
#include "Datagrams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter construction

CRtpReceiverFilter::CRtpReceiverFilter(WAVEFORMATEX* pwfx, CFilterGraph* pFilterGraph, LPVOID lv)
: CBaseFilter(NAME("Rtp Receiver Filter"), NULL, &m_Lock, CLSID_NULL)
{
	HRESULT hr = S_OK;
	m_pPreview = new CRtpOutputPin( this, &m_Lock, &hr, L"Preview" );
	
	WAVEFORMATEX wfx = {0};
	::DeriveCompressFormat( wfx );
	
	m_pPreview->SetMediaType( &wfx );
	m_pCapture = new CRtpOutputPin( this, &m_Lock, &hr, L"Capture" );
	
	WAVEFORMATEX wfxDestFormat = {0};
	if ( ! pwfx ) ::DeriveCompressFormat( *(pwfx = &wfxDestFormat), &wfx );
	m_pCapture->SetMediaType( pwfx );
	
	m_pDatagram	= (CDatagrams *)lv;
	VERIFY( m_pDatagram != NULL );
	
	m_pFilterGraph	= pFilterGraph;
	m_nRefStream	= 0;
}

CRtpReceiverFilter::CRtpReceiverFilter(VIDEOINFOHEADER* pvih, CFilterGraph* pFilterGraph, LPVOID lv)
: CBaseFilter(NAME("Rtp Receiver Filter"), NULL, &m_Lock, CLSID_NULL)
{
	HRESULT hr = S_OK;
	m_pPreview = new CRtpOutputPin( this, &m_Lock, &hr, L"Preview" );
	
	VIDEOINFOHEADER vih = {0};
	DeriveCompressFormat( vih );
	
	m_pPreview->SetMediaType( &vih );
	m_pCapture = new CRtpOutputPin( this, &m_Lock, &hr, L"Capture" );
	
	VIDEOINFOHEADER vihDestFormat = {0};
	if ( ! pvih ) DeriveCompressFormat( *(pvih = &vihDestFormat), &vih );
	m_pCapture->SetMediaType( pvih );
	
	m_pDatagram	= (CDatagrams *)lv;
	VERIFY( m_pDatagram != NULL );
	
	m_pFilterGraph	= pFilterGraph;
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

Payload CRtpReceiverFilter::GetPayloadType() const
{
	if ( ! m_pDatagram ) return Payload_Unknown;
	return (Payload)m_pDatagram->m_nType;
}

CFilterGraph* CRtpReceiverFilter::GetFilterGraph() const
{
	return m_pFilterGraph;
}

STDMETHODIMP CRtpReceiverFilter::Stop(void)
{
	m_nRefStream	= 0;
	
	return CBaseFilter::Stop();
}

/////////////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter deliver buffer

HRESULT CRtpReceiverFilter::DeliverBuffer(BYTE* pBuffer, DWORD nLength, LONG nTimestamp)
{
	return RenderBuffer( pBuffer, nLength, REFERENCE_TIME(nTimestamp) * 10000 );
}

/////////////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter render buffer

HRESULT CRtpReceiverFilter::RenderBuffer(BYTE* pByte, DWORD nLength)
{
//	ASSERT( m_nPayloadType == Payload_Alaw );

	REFERENCE_TIME rtStart = 0;
	if ( m_State >= State_Paused ) StreamTime( (CRefTime&)rtStart );
	
	return RenderBuffer( pByte, nLength, rtStart );
}

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
//	TRACE( "RX[%2i][%4i]: [%I64i][%I64i], [%I64i][%I64i] {%s}\n", GetPayloadType(), nLength,
//		rtStart, (REFERENCE_TIME)0, (REFERENCE_TIME)0, (REFERENCE_TIME)-1, ToHex( pByte, min(100,nLength) ) );
#endif
	
	return S_OK;
}

HRESULT CALLBACK CRtpReceiverFilter::RenderBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE /*nType*/, DWORD nTimestamp, LPVOID lv)
{
	CRtpReceiverFilter* pReceiver = (CRtpReceiverFilter*)lv;
	
	if ( pReceiver->m_State == State_Stopped ) return S_OK;
	
	if ( pReceiver->m_State == State_Paused )
	{
		return pReceiver->DeliverBuffer( pBuffer, nBuffer, 0 );
	}
	
	volatile DWORD& nRefStream = pReceiver->m_nRefStream; if ( ! nRefStream )
	{
		REFERENCE_TIME rtTime;
		pReceiver->StreamTime( (CRefTime&)rtTime );
		
		nRefStream  = nTimestamp;
		nRefStream -= LONG(rtTime / 10000);
	}
	
	return pReceiver->DeliverBuffer( pBuffer, nBuffer, nTimestamp - nRefStream );
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
	
	WAVEFORMATEX wfxSrc = {0};
	DeriveCompressFormat( wfxSrc, pwfx );
	
	ZeroMemory(&ACMStreamHeader, sizeof(ACMSTREAMHEADER));
	ACMStreamHeader.cbStruct = sizeof(ACMStreamHeader);
	
	return acmStreamOpen( &HACMStream, NULL, &wfxSrc, pwfx,
		NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME );
}

DWORD CRtpOutputPin::InitializeCodec(LPCWSTR Id, VIDEOINFOHEADER* pvih)
{
	if ( wcscmp( Id, L"Preview" ) ) return ICERR_OK;
	CRtpReceiverFilter* pTransformFilter = (CRtpReceiverFilter *)m_pFilter;
	
	VIDEOINFOHEADER vihSrc = {0};
	DeriveCompressFormat( vihSrc, pvih );
	
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
//	pProperties->cbAlign  = 1;
	
	ASSERT( pProperties->cbBuffer );
	
	ALLOCATOR_PROPERTIES Actual;
	hr = pAlloc->SetProperties(pProperties,&Actual);
	if ( FAILED(hr) ) return hr;
	
	ASSERT( Actual.cBuffers == 1 );
	
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
	if ( m_mt.formattype == FORMAT_VideoInfo && m_hIC && nLength )
	{
		if ( ICDecompress(m_hIC, 0, &m_bmiInput, pBuffer, &m_bmiOutput, m_pBuffer) ) return E_FAIL;
		
		nLength = m_bmiOutput.biSizeImage;
		CopyMemory( pBuffer, m_pBuffer, nLength );
		pMediaSample->SetActualDataLength( nLength );
	}
	
	pMediaSample->SetTime( NULL, NULL );
	return CBaseOutputPin::Deliver(pMediaSample);
}
/*
HRESULT CRtpOutputPin::Active(void)
{
	CAutoLock lock(m_pLock);
	
	if ( m_pFilter->IsActive() ) return S_FALSE;
	
	if ( ! IsConnected() ) return NOERROR;
	
	HRESULT hr = CBaseOutputPin::Active();
	if ( FAILED(hr) ) return hr;
	
	IMediaSample *pMediaSample;
	
	if ( SUCCEEDED(GetDeliveryBuffer( &pMediaSample, NULL, NULL, 0 )) )
	{
		pMediaSample->SetActualDataLength( 0 );
		
		REFERENCE_TIME rtStart = 0;
		pMediaSample->SetTime( &rtStart, NULL );
		pMediaSample->SetMediaTime( NULL, NULL );
		pMediaSample->SetSyncPoint( TRUE );
			
		Deliver( pMediaSample );
		pMediaSample->Release();
	}
	
	return hr;
}
*/