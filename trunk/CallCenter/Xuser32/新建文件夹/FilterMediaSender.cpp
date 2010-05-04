//
// FilterMediaSender.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "Settings.h"

#include "FilterMediaSender.h"
#include "RTPSession2.h"

#include <G711uids.h>
#include <initguid.h>
#include <AMRTPUID.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// {04EBD0AE-97A5-4917-B081-1DDA180E6891}
DEFINE_GUID(CLSID_MediaSender, 
0x4ebd0ae, 0x97a5, 0x4917, 0xb0, 0x81, 0x1d, 0xda, 0x18, 0xe, 0x68, 0x91);

//////////////////////////////////////////////////////////////////////
// CMediaSenderFilter construction

CMediaSenderFilter::CMediaSenderFilter(LPUNKNOWN pUnk, HRESULT *phr)
				: CBaseFilter(NAME("CMediaSenderFilter"), pUnk, &m_Lock, CLSID_MediaSender)
{
	ASSERT( phr != NULL );
	CMediaInputPin* pInput = new CMediaInputPin(
		this, GetOwner(), &m_Lock, &m_ReceiveLock, phr);
	
	if ( pInput && SUCCEEDED(*phr) ) m_pPinInput = pInput;
	else
	if ( pInput && FAILED(*phr) ) delete pInput;
	else
		*phr = E_OUTOFMEMORY;
}

CMediaSenderFilter::~CMediaSenderFilter()
{
	if ( m_pPinInput ) delete m_pPinInput;
}

//////////////////////////////////////////////////////////////////////
// CMediaSenderFilter operations

int CMediaSenderFilter::GetPinCount()
{
    return 1;
}

CBasePin* CMediaSenderFilter::GetPin(int n)
{
	ASSERT( n == 0 );
	if ( n != 0 ) return NULL;
	
	return m_pPinInput;
}

//////////////////////////////////////////////////////////////////////
// CMediaInputPin construction

CMediaInputPin::CMediaInputPin(CMediaSenderFilter* pMpegFilter, LPUNKNOWN pUnk,
							 CCritSec* pLock, CCritSec *pReceiveLock, HRESULT *phr)
: CRenderedInputPin(NAME("CMediaInputPin"), pMpegFilter, pLock, phr, L"Input")
, m_pMediaFilter(pMpegFilter)
, m_pReceiveLock(pReceiveLock)
{
}

CMediaInputPin::~CMediaInputPin()
{
}

//////////////////////////////////////////////////////////////////////
// CMediaInputPin attributes

HRESULT CMediaInputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
	
	if ( pMediaType->majortype == MEDIATYPE_Audio &&
		pMediaType->subtype == MEDIASUBTYPE_ALAWAudio )
	{
		return NOERROR;
	}
	if ( pMediaType->majortype == MEDIATYPE_Video &&
		 pMediaType->subtype == MEDIASUBTYPE_H263 )
	{
		return NOERROR;
	}
	
	return E_INVALIDARG;
}

HRESULT CMediaInputPin::Receive(IMediaSample* pMediaSample)
{
	CAutoLock lock(m_pReceiveLock);
	
	if ( m_pMediaFilter->m_State == State_Stopped ) return E_FAIL;
	
	HRESULT hr = CBaseInputPin::Receive( pMediaSample );
	if ( FAILED(hr) ) return hr;
	
	BYTE*  pbBuffer ;
	hr = pMediaSample->GetPointer( &pbBuffer );
    
	if (SUCCEEDED (hr) )
	{
		int nError = SendPacket( pbBuffer, pMediaSample->GetActualDataLength() );
		if ( nError < 0 )
		{
#ifdef _DEBUG
			CString str;
			std::string stdErrStr = RTPGetErrorString(nError);
			
			str.Format( "Error[%i]: %s", nError, stdErrStr.c_str() );
			TRACE( str + '\n' );
#endif
	//		return E_FAIL;
		}
	}
	
	return hr;
}

STDMETHODIMP CMediaInputPin::EndOfStream(void)
{
    CAutoLock lock(m_pReceiveLock);
    return CRenderedInputPin::EndOfStream();
}

int CMediaInputPin::SendPacket(byte* data, size_t len)
{
	long timestampinc = GetTickCount();
	const int nMaxPacket = Settings.Network.MaxPacket;
	
	for ( byte* ptr = data; ptr < data + len; )
	{
		int nLength = min( int(data + len - ptr), nMaxPacket - 100 );
#ifdef _DEBUG
//		afxDump << "Tx[" << nLength << "]: " << (LPCTSTR)ToHex(ptr, nLength) << "\n";
#endif
		int nError = RTPSession.SendPacket( ptr, nLength, 0, false, timestampinc );
		
		if ( nError < 0 ) return nError;
		ptr += nLength;
	}
	
	return len;
}
