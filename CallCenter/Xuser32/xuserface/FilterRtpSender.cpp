//
// FilterRtpSender.cpp
//

#include "stdafx.h"
#include "FilterRtpSender.h"

#include "RenderGraph.h"
#include "Datagrams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CRtpSenderFilter construction

CRtpSenderFilter::CRtpSenderFilter(WAVEFORMATEX* pwfx, CFilterGraph* pFilterGraph, DWORD nReserved)
: CBaseFilter(NAME("Rtp Sender Filter"), NULL, &m_Lock, CLSID_NULL), m_nRefRender((float)1000.0 / pwfx->nAvgBytesPerSec)
{
	HRESULT hr = S_OK;
	
	m_pCapture = new CRtpInputPin(this, &m_Lock, &hr);
	m_pCapture->SetMediaType( pwfx );
	
	m_pDatagrams	= (CDatagrams*)nReserved;
	VERIFY( m_pDatagrams != NULL );
	
	m_pFilterGraph	= pFilterGraph;
	m_nRefStream	= 0;
	m_pBuffer	= NULL;
}

CRtpSenderFilter::CRtpSenderFilter(VIDEOINFOHEADER* pvih, CFilterGraph* pFilterGraph, DWORD nReserved)
: CBaseFilter(NAME("Rtp Sender Filter"), NULL, &m_Lock, CLSID_NULL), m_nRefRender(float(pvih->AvgTimePerFrame / 10000))
{
	HRESULT hr = S_OK;
	
	m_pCapture = new CRtpInputPin(this, &m_Lock, &hr);
	m_pCapture->SetMediaType( pvih );
	
	ZeroMemory( &m_cv, sizeof(m_cv) );
	m_cv.cbSize		= sizeof(m_cv);
	m_cv.dwFlags	= ICMF_COMPVARS_VALID ;
	m_cv.hic		= NULL;
	m_cv.fccType	= ICTYPE_VIDEO ;
	m_cv.fccHandler	= MAKEFOURCC('M','2','6','3');
	m_cv.lpbiOut	= 0;
	m_cv.lKey		= 10;
	m_cv.lDataRate	= 8;
	m_cv.lQ			= 10000;
	
	ZeroMemory( &m_bmiInput, sizeof(BITMAPINFO) );
	CopyMemory( &m_bmiInput.bmiHeader, &pvih->bmiHeader, sizeof(BITMAPINFO) );
	
	m_pDatagrams	= (CDatagrams*)nReserved;
	VERIFY( m_pDatagrams != NULL );
	
	m_pFilterGraph	= pFilterGraph;
	m_nRefStream	= 0;
	m_pBuffer		= NULL;
}

CRtpSenderFilter::~CRtpSenderFilter()
{
	if ( m_pCapture ) delete m_pCapture;
}

//////////////////////////////////////////////////////////////////////
// CRtpSenderFilter attributes

int CRtpSenderFilter::GetPinCount()
{
    return 1;
}

CBasePin* CRtpSenderFilter::GetPin(int n)
{
	ASSERT( n == 0 );
	if ( n != 0 ) return NULL;
	
	return m_pCapture;
}

BYTE CRtpSenderFilter::GetPayloadType() const
{
	if ( ! m_pDatagrams ) return Payload_Unknown;
	return m_pDatagrams->m_nType;
}

CFilterGraph* CRtpSenderFilter::GetFilterGraph() const
{
	return m_pFilterGraph;
}

STDMETHODIMP CRtpSenderFilter::Run(REFERENCE_TIME tStart)
{
	CAutoLock lock(&m_Lock);
	
	if ( ! m_nRefStream ) m_nRefStream = GetTickCount();
	
	return CBaseFilter::Run( tStart );
}

MMRESULT CRtpSenderFilter::InitializeCodec(WAVEFORMATEX* pwfx)
{
	WAVEFORMATEX wfxDestFormat;
	DeriveCompressFormat( wfxDestFormat, pwfx );
	
	ZeroMemory(&ACMStreamHeader, sizeof(ACMSTREAMHEADER));
	ACMStreamHeader.cbStruct = sizeof(ACMStreamHeader);
	
	MMRESULT mmr = acmStreamOpen( &HACMStream, NULL, pwfx, &wfxDestFormat,
		NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME );
	if ( mmr ) return mmr;
	
	ACMStreamHeader.pbDst = new BYTE[wfxDestFormat.nAvgBytesPerSec]; 
	ACMStreamHeader.cbDstLength = wfxDestFormat.nAvgBytesPerSec;
	
	m_pBuffer = ACMStreamHeader.pbDst;
	
	return mmr;
}

BOOL CRtpSenderFilter::InitializeCodec(VIDEOINFOHEADER* pvih)
{
	m_cv.hic = ICOpen(ICTYPE_VIDEO,m_cv.fccHandler,ICMODE_COMPRESS|ICMODE_DECOMPRESS);
	if ( m_cv.hic == NULL ) return FALSE;
	
	ICSendMessage(m_cv.hic,0x60c9,0xf7329ace,0xacdeaea2);
	
	if ( ! ICSeqCompressFrameStart( &m_cv, &m_bmiInput ) ) return FALSE;
	
	m_pBuffer = new BYTE [pvih->bmiHeader.biSizeImage];
	return TRUE;
}

void CRtpSenderFilter::DestroyCodec(WAVEFORMATEX* pwfx)
{
	if ( HACMStream ) acmStreamClose( HACMStream, 0 );
	HACMStream	= NULL;
	
	if ( m_pBuffer ) delete [] m_pBuffer;
	m_pBuffer	= NULL;
}

void CRtpSenderFilter::DestroyCodec(VIDEOINFOHEADER* pvih)
{
	ICSeqCompressFrameEnd( &m_cv );
	ICCompressorFree( &m_cv );
	
	ICClose( m_cv.hic );
	m_cv.hic = NULL;
	
	if ( m_pBuffer ) delete [] m_pBuffer;
	m_pBuffer	= NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CRtpSenderFilter deliver buffer

HRESULT CRtpSenderFilter::Deliver(IMediaSample *pMediaSample)
{
	LPBYTE pBuffer ;
	HRESULT hr = pMediaSample->GetPointer( &pBuffer );
	
	if ( FAILED(hr) ) return hr;
	int nLength = pMediaSample->GetActualDataLength();
	
	if ( GetPayloadType() == Payload_Alaw )
	{
		hr = Deliver( pBuffer, nLength );
	}
	else
	if ( GetPayloadType() == Payload_H263 )
	{
		hr = Deliver( pBuffer, nLength, 0 );
	}
	
	return hr;
}

HRESULT CRtpSenderFilter::Deliver(BYTE* pBuffer, DWORD nLength)	
{
	ACMStreamHeader.pbSrc = pBuffer;
	ACMStreamHeader.cbSrcLength = nLength;
	
	acmStreamPrepareHeader( HACMStream, &ACMStreamHeader, 0 );
	
	if ( acmStreamConvert( HACMStream, &ACMStreamHeader, 0 ) ) return E_FAIL;
	acmStreamUnprepareHeader( HACMStream, &ACMStreamHeader, 0 );
	
	DWORD nTimestampinc = DWORD(m_nRefRender * nLength);
	SendPayload( ACMStreamHeader.pbDst, ACMStreamHeader.cbDstLengthUsed, GetPayloadType(), nTimestampinc );
	
	return S_OK;
}

HRESULT CRtpSenderFilter::Deliver(BYTE* pBuffer, DWORD nLength, DWORD nReserved)	
{
	BOOL bKey;
	pBuffer = (BYTE*)ICSeqCompressFrame( &m_cv, 0, pBuffer, &bKey, (LONG*)&nLength );
	
	if ( ! pBuffer ) return E_FAIL;
	
	SendPayload( pBuffer, nLength, GetPayloadType(), (DWORD)m_nRefRender );
	
	return S_OK;
}

int CRtpSenderFilter::SendPayload(BYTE* pBuffer, DWORD nBuffer, BYTE nType, DWORD nTimestampinc)
{
	DWORD tDiff = GetTickCount() - m_nRefStream;
	m_nRefStream = GetTickCount();
	
	LPCTSTR xUserId = GetFilterGraph()->GetNamed();
	
	CSingleLock pLock( &m_pDatagrams->m_pSection, TRUE );
	
	if ( CDatagramOut* pDG = m_pDatagrams->LookupKey(xUserId, pDG) )
	{
		pDG->IncrementTimestamp( tDiff -= nTimestampinc );
		pDG->RenderBuffer( pBuffer, nBuffer, nType, nTimestampinc, NULL, NULL );
	}
	
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CRtpInputPin construction

CRtpInputPin::CRtpInputPin(CRtpSenderFilter* pFilter, CCritSec* pLock, HRESULT *phr)
: CRenderedInputPin(NAME("Input Pin"), pFilter, pLock, phr, L"Transport"), m_pReceiveLock(&pFilter->m_ReceiveLock), m_pRtpFilter(pFilter)
{
}

CRtpInputPin::~CRtpInputPin()
{
}

//////////////////////////////////////////////////////////////////////
// CRtpInputPin attributes

HRESULT CRtpInputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
	if ( iPosition < 0 ) return E_INVALIDARG;
	if ( iPosition > 0 ) return VFW_S_NO_MORE_ITEMS;
	
	*pMediaType = CRenderedInputPin::m_mt;
	return S_OK;
}

HRESULT CRtpInputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
	
	DisplayMediaType( "CRtpInputPin::CheckMediaType", pMediaType );
	
	if ( *pMediaType == CRenderedInputPin::m_mt ) return NOERROR;
	
	return E_INVALIDARG;
}

HRESULT CRtpInputPin::CompleteConnect(IPin *pReceivePin)
{
	CAutoLock lock(m_pLock);
	
	if ( m_mt.formattype == FORMAT_WaveFormatEx )
	{
		WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();
		if ( m_pRtpFilter->InitializeCodec( pwfx ) ) return E_FAIL;
	}
	else
	if ( m_mt.formattype == FORMAT_VideoInfo )
	{
		VIDEOINFOHEADER* pvih = (VIDEOINFOHEADER*)m_mt.Format();
		if ( ! m_pRtpFilter->InitializeCodec( pvih ) ) return E_FAIL;
	}
	
	return CBaseInputPin::CompleteConnect(pReceivePin);
}

HRESULT CRtpInputPin::BreakConnect(void)
{
	CAutoLock lock(m_pLock);
	
	if ( m_mt.formattype == FORMAT_WaveFormatEx )
	{
		WAVEFORMATEX* pwfx = (WAVEFORMATEX*)m_mt.Format();
		m_pRtpFilter->DestroyCodec( pwfx );
	}
	else
	if ( m_mt.formattype == FORMAT_VideoInfo )
	{
		VIDEOINFOHEADER* pvih = (VIDEOINFOHEADER*)m_mt.Format();
		m_pRtpFilter->DestroyCodec( pvih );
	}
	
	return CBaseInputPin::BreakConnect();
}

//////////////////////////////////////////////////////////////////////
// CRtpInputPin operations

HRESULT CRtpInputPin::SetMediaType(WAVEFORMATEX* pwfx)
{
	CheckPointer( pwfx, E_POINTER );
	
	CMediaType pMediaType;
	CreateAudioMediaType( pwfx, &pMediaType, TRUE );
	
	CBaseInputPin::SetMediaType( &pMediaType );
	
	return S_OK;
}

HRESULT CRtpInputPin::SetMediaType(VIDEOINFOHEADER* pvih)
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
	CBaseInputPin::SetMediaType( &pMediaType );
	
	return S_OK;
}

HRESULT CRtpInputPin::Receive(IMediaSample* pMediaSample)
{
	CAutoLock lock(m_pReceiveLock);
	if ( m_pRtpFilter->IsStopped() ) return E_FAIL;
	
	HRESULT hr = CBaseInputPin::Receive( pMediaSample );
	if (FAILED(hr) || m_SampleProps.dwStreamId != AM_STREAM_MEDIA) return hr;
	
#ifdef _DEBUG
	LPBYTE pBuffer ;
	pMediaSample->GetPointer( &pBuffer );
	
	int nLength = pMediaSample->GetActualDataLength();
#endif

	CRtpSenderFilter* pTransformFilter = (CRtpSenderFilter *)m_pFilter;
#ifdef _DEBUG
	BYTE nPayload = pTransformFilter->GetPayloadType();
	
	REFERENCE_TIME rcStart, rcEnd;
	pMediaSample->GetTime( &rcStart, &rcEnd);
	REFERENCE_TIME rcStartMedia, rcEndMedia;
	pMediaSample->GetMediaTime( &rcStartMedia, &rcEndMedia);
	
//	TRACE( "TX[%2i][%4i]: [%I64i][%I64i], [%I64i][%I64i] {%s}\n", 
//		nPayload, nLength, rcStart, rcEnd, rcStartMedia, rcEndMedia, ToHex( pBuffer, 100 ) );
#endif
	hr = pTransformFilter->Deliver( pMediaSample );
	
	return hr;
}

STDMETHODIMP CRtpInputPin::EndOfStream(void)
{
	CAutoLock lock(m_pReceiveLock);
	
	return CRenderedInputPin::EndOfStream();
}

STDMETHODIMP CRtpInputPin::BeginFlush(void)
{
	CAutoLock lock(m_pLock);
	
	return CRenderedInputPin::BeginFlush();
}

STDMETHODIMP CRtpInputPin::EndFlush(void)
{
	CAutoLock lock(m_pLock);
	
	return CRenderedInputPin::EndFlush();
}

STDMETHODIMP CRtpInputPin::ReceiveCanBlock(void)
{
	return S_OK;
}

void CRtpInputPin::DisplayMediaType(TCHAR *pDescription,const CMediaType *pmt) 
{ 
#ifdef _DEBUG
	DbgLog((LOG_TRACE, 0, TEXT("")));
	DbgLog((LOG_TRACE, 0, TEXT("%s"),pDescription));
	DbgLog((LOG_TRACE, 0, TEXT("Media Type Description")));
	DbgLog((LOG_TRACE, 0, TEXT("Major type: %s"),GuidNames[*pmt->Type()]));

	wchar_t szGUID[39];
	szGUID[ StringFromGUID2( *(GUID*)&pmt->majortype, szGUID, 39 ) - 2 ] = 0;
	DbgLog((LOG_TRACE, 0, TEXT("Major id: %s"),(CString)&szGUID[1]));
	
	DbgLog((LOG_TRACE, 0, TEXT("Subtype: %s"),GuidNames[*pmt->Subtype()]));
	szGUID[ StringFromGUID2( *(GUID*)pmt->Subtype(), szGUID, 39 ) - 2 ] = 0;
	DbgLog((LOG_TRACE, 0, TEXT("Subtype id: %s"),(CString)&szGUID[1]));

	DbgLog((LOG_TRACE, 0, TEXT("Subtype description: %s"),GetSubtypeName(pmt->Subtype())));
	DbgLog((LOG_TRACE, 0, TEXT("Format size: %d"),pmt->cbFormat)); 
	
	// Dump the generic media types 
	
	DbgLog((LOG_TRACE, 0, TEXT("Fixed size sample %d"),pmt->IsFixedSize())); 
	DbgLog((LOG_TRACE, 0, TEXT("Temporal compression %d"),pmt->IsTemporalCompressed())); 
	DbgLog((LOG_TRACE, 0, TEXT("Sample size %d"),pmt->GetSampleSize())); 
#endif
}
