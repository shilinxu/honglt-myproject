//
// FilterRtpSender.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "FilterRtpSender.h"
#include <initguid.h>
#include "Settings.h"
#include "RTPSession2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// {04EBD0AE-97A5-4917-B081-1DDA180E6891}
DEFINE_GUID(CLSID_RtpSender, 
0x4ebd0ae, 0x97a5, 0x4917, 0xb0, 0x81, 0x1d, 0xda, 0x18, 0xe, 0x68, 0x91);

//////////////////////////////////////////////////////////////////////
// CRtpSenderFilter construction

CRtpSenderFilter::CRtpSenderFilter(LPUNKNOWN pUnk, HRESULT *phr)
: CBaseFilter(NAME("Sender Filter"), pUnk, &m_Lock, CLSID_RtpSender)
{
	m_pInput[0] = new CRtpInputPin(this, &m_Lock, phr, L"Transport");
	m_pInput[1] = new CRtpInputPin(this, &m_Lock, phr, L"Transport");
}

CRtpSenderFilter::~CRtpSenderFilter()
{
	if ( m_pInput[1] ) delete m_pInput[1];
	if ( m_pInput[0] ) delete m_pInput[0];
}

//////////////////////////////////////////////////////////////////////
// CRtpSenderFilter operations

int CRtpSenderFilter::GetPinCount()
{
    return 2;
}

CBasePin* CRtpSenderFilter::GetPin(int n)
{
	if ( n >=0 && n < GetPinCount() )
	{
		return m_pInput[ n ];
	}
	
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CRtpInputPin construction

CRtpInputPin::CRtpInputPin(CRtpSenderFilter* pFilter, CCritSec* pLock, 
						   HRESULT *phr, LPCWSTR pName)
: CRenderedInputPin(NAME("Input Pin"), pFilter, pLock, phr, pName)
, m_pReceiveLock(&pFilter->m_ReceiveLock)
, m_pRtpFilter( pFilter )
{
}

CRtpInputPin::~CRtpInputPin()
{
}

//////////////////////////////////////////////////////////////////////
// CRtpInputPin attributes

HRESULT CRtpInputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
#ifdef _DEBUG
	DisplayMediaType(TEXT("Input Pin Checking"), pMediaType);
#endif
	
	if ( pMediaType->majortype == MEDIATYPE_Stream &&
		pMediaType->subtype == MEDIASUBTYPE_Avi )
	{
		return NOERROR;
	}
	
	return E_INVALIDARG;
}

HRESULT CRtpInputPin::Receive(IMediaSample* pMediaSample)
{
	CAutoLock lock(m_pReceiveLock);
	if ( m_pRtpFilter->m_State == State_Stopped ) return E_FAIL;
	
	HRESULT hr = CBaseInputPin::Receive( pMediaSample );
	if ( FAILED(hr) ) return hr;
	
	LPBYTE pBuffer ;
	hr = pMediaSample->GetPointer( &pBuffer );
	if ( FAILED(hr) ) return hr;
	
	int nError = SendPacket( pBuffer, pMediaSample->GetActualDataLength() );
	if ( nError < 0 )
	{
#ifdef _DEBUG
		CString str;
		std::string stdErrStr = RTPGetErrorString(nError);
		
		str.Format( "Error[%i]: %s", nError, stdErrStr.c_str() );
		TRACE( str + '\n' );
#endif
	}
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CRtpInputPin send packets

int CRtpInputPin::SendPacket(byte* data, size_t len)
{
	long timestampinc = GetTickCount();
	const int nMaxPacket = Settings.Network.MaxPacket;
	
	for ( byte* ptr = data; ptr < data + len; )
	{
		int nLength = min( int(data + len - ptr), nMaxPacket - 12 );
		int nError = RTPSession.SendPacket( ptr, nLength, 0, false, timestampinc );
		
		if ( nError < 0 ) return nError;
		ptr += nLength;
	}
	
	return len;
}

#ifdef _DEBUG
void CRtpInputPin::DisplayMediaType(TCHAR *pDescription,const CMediaType *pmt) 
{ 
	DbgLog((LOG_TRACE, 0, TEXT("")));
	DbgLog((LOG_TRACE, 0, TEXT("%s"),pDescription));
	DbgLog((LOG_TRACE, 0, TEXT("")));
	DbgLog((LOG_TRACE, 0, TEXT("Media Type Description")));
	DbgLog((LOG_TRACE, 0, TEXT("Major type: %s"),GuidNames[*pmt->Type()]));
	DbgLog((LOG_TRACE, 0, TEXT("Subtype: %s"),GuidNames[*pmt->Subtype()]));
	DbgLog((LOG_TRACE, 0, TEXT("Subtype description: %s"),GetSubtypeName(pmt->Subtype())));
	DbgLog((LOG_TRACE, 0, TEXT("Format size: %d"),pmt->cbFormat)); 
	
	// Dump the generic media types 
	
	DbgLog((LOG_TRACE, 0, TEXT("Fixed size sample %d"),pmt->IsFixedSize())); 
	DbgLog((LOG_TRACE, 0, TEXT("Temporal compression %d"),pmt->IsTemporalCompressed())); 
	DbgLog((LOG_TRACE, 0, TEXT("Sample size %d"),pmt->GetSampleSize())); 
}
#endif
