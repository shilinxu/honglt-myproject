//
// FilterRtpSender.cpp
//

#include "stdafx.h"
#include "FilterRtpSender.h"
#include "RTPSession2.h"

#include <wmsdkidl.h>
#include <G711uids.h>
#include <initguid.h>
#include <mmreg.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// {04EBD0AE-97A5-4917-B081-1DDA180E6891}
DEFINE_GUID(CLSID_RtpSender, 
0x4ebd0ae, 0x97a5, 0x4917, 0xb0, 0x81, 0x1d, 0xda, 0x18, 0xe, 0x68, 0x91);

#define TEMP_BUFFER			1024

//////////////////////////////////////////////////////////////////////
// CRtpSenderFilter construction

CRtpSenderFilter::CRtpSenderFilter(LPUNKNOWN pUnk, HRESULT *phr)
: CBaseFilter(NAME("Sender Filter"), pUnk, &m_Lock, CLSID_RtpSender)
{
	ASSERT( phr != NULL );
	CRtpInputPin* pInput =
	new CRtpInputPin(this, &m_Lock, phr);
	
	if ( pInput && SUCCEEDED(*phr) ) m_pInput = pInput;
	else
	if ( pInput && FAILED(*phr) ) delete pInput;
	else
		*phr = E_OUTOFMEMORY;
}

CRtpSenderFilter::~CRtpSenderFilter()
{
	if ( m_pInput ) delete m_pInput;
}

//////////////////////////////////////////////////////////////////////
// CRtpSenderFilter operations

int CRtpSenderFilter::GetPinCount()
{
    return 1;
}

CBasePin* CRtpSenderFilter::GetPin(int n)
{
	ASSERT( n == 0 );
	if ( n != 0 ) return NULL;
	
	return m_pInput;
}

HRESULT CRtpSenderFilter::SetupMediaType(WAVEFORMATEX* pwfx)
{
	CheckPointer(pwfx,E_POINTER);
	CheckPointer(m_pInput, E_POINTER);
	
	if ( pwfx->wFormatTag == WAVE_FORMAT_ALAW )
	{
		pwfx->nSamplesPerSec	= 8000;
		pwfx->wBitsPerSample	= 8;
		pwfx->nChannels			= 1;
		pwfx->nBlockAlign		= pwfx->nChannels * ( pwfx->wBitsPerSample / 8 );
		pwfx->nAvgBytesPerSec	= pwfx->nBlockAlign * pwfx->nSamplesPerSec;
		
		CMediaType& pMediaType = m_pInput->m_mt;
		pMediaType.SetSubtype( &MEDIASUBTYPE_ALAWAudio );
	}
	
	CMediaType& pMediaType = m_pInput->m_mt;
	
	pMediaType.SetType( &MEDIATYPE_Audio );
	pMediaType.SetFormatType( &FORMAT_WaveFormatEx );
	pMediaType.SetTemporalCompression( TRUE );
	
	pMediaType.SetFormat( (BYTE*)pwfx, sizeof(WAVEFORMATEX) );
	return S_OK;
}

HRESULT CRtpSenderFilter::SetupMediaType(VIDEOINFOHEADER* pvh)
{
	CheckPointer(pvh,E_POINTER);
	CheckPointer(m_pInput, E_POINTER);
	
	if ( pvh->bmiHeader.biCompression == MAKEFOURCC('H','2','6','3') )
	{
		pvh->bmiHeader.biBitCount	= 24;
		pvh->bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
		pvh->bmiHeader.biWidth		= 176;
		pvh->bmiHeader.biHeight		= 144;
		pvh->bmiHeader.biPlanes		= 1;
		pvh->bmiHeader.biSizeImage	= GetBitmapSize( &pvh->bmiHeader );
		pvh->bmiHeader.biClrImportant= 0;
		pvh->AvgTimePerFrame		= UNITS / 30;
	}
	if ( pvh->bmiHeader.biCompression == MAKEFOURCC('M','P','4','3') )
	{
		pvh->bmiHeader.biBitCount	= 24;
		pvh->bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
		pvh->bmiHeader.biWidth		= 320;
		pvh->bmiHeader.biHeight		= 240;
		pvh->bmiHeader.biPlanes		= 1;
		pvh->bmiHeader.biSizeImage	= GetBitmapSize( &pvh->bmiHeader );
		pvh->bmiHeader.biClrImportant= 0;
		pvh->AvgTimePerFrame		= UNITS / 30;
	}
	
	CMediaType& pMediaType = m_pInput->m_mt;
	
	SetRectEmpty( &pvh->rcSource );
	SetRectEmpty( &pvh->rcTarget );
	
	pMediaType.SetType( &MEDIATYPE_Video );
	pMediaType.SetFormatType( &FORMAT_VideoInfo );
	pMediaType.SetTemporalCompression( TRUE );
	
	const GUID SubTypeGUID = GetBitmapSubtype(&pvh->bmiHeader);
	pMediaType.SetSubtype( &SubTypeGUID );
	pMediaType.SetSampleSize( pvh->bmiHeader.biSizeImage );
	
	pMediaType.SetFormat( (BYTE*)pvh, sizeof(VIDEOINFOHEADER) );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CRtpInputPin construction

CRtpInputPin::CRtpInputPin(CRtpSenderFilter* pFilter, CCritSec* pLock, HRESULT *phr)
: CRenderedInputPin(NAME("Input Pin"), pFilter, pLock, phr, L"Transport")
, m_pReceiveLock(&pFilter->m_ReceiveLock)
, m_pRtpFilter( pFilter )
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
	
	*pMediaType = CBasePin::m_mt;
	return S_OK;
}

HRESULT CRtpInputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
	
	DisplayMediaType( "CRtpInputPin::CheckMediaType", pMediaType );
	
	if ( *pMediaType == CBasePin::m_mt ) return NOERROR;
	
	return E_INVALIDARG;
}

HRESULT CRtpInputPin::Receive(IMediaSample* pMediaSample)
{
	CAutoLock lock(m_pReceiveLock);
	if ( m_pRtpFilter->m_State == State_Stopped ) return E_FAIL;
	
	HRESULT hr = CBaseInputPin::Receive( pMediaSample );
	if (FAILED(hr) || m_SampleProps.dwStreamId != AM_STREAM_MEDIA) return hr;
	
	LPBYTE pBuffer ;
	hr = pMediaSample->GetPointer( &pBuffer );
	if ( FAILED(hr) ) return hr;
	
	int nLength = pMediaSample->GetActualDataLength();
	SendPacketSplitter( pBuffer, nLength );
	
	return S_OK;
}

int CRtpInputPin::SendPacketSplitter(BYTE* pBuffer, DWORD nBuffer)
{
	BYTE nPayload = GetPayloadType();
	
	for ( int nLength = min( ( nBuffer & 0xFFFFF ), TEMP_BUFFER );
		nLength > 0; nLength = min( ( nBuffer & 0xFFFFF ), TEMP_BUFFER ) )
	{
		DWORD nTimestampinc = (nBuffer - nLength) <= 0 ? 10 : 0;
		int nError = RTPSession.SendPacket( pBuffer, nLength, nPayload, false, nTimestampinc );
			
		if ( nError < 0 ) return nError;
		
		pBuffer += nLength; nBuffer -= nLength;
		nTimestampinc = 10;
	}
	
	return 0;
}

BYTE CRtpInputPin::GetPayloadType() const
{
	if ( m_mt.majortype == MEDIATYPE_Audio ) return 'A';
	if ( m_mt.majortype == MEDIATYPE_Video ) return 'V';
	
	return '?';
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
