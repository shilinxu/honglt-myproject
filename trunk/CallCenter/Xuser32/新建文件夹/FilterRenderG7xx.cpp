//
// FilterRenderG7xx.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "FilterRenderG7xx.h"
#include <initguid.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// { 36a5f770-fe4c-11ce-a8ed-00aa002feab5 }
DEFINE_GUID(CLSID_Dump,
0x36a5f770, 0xfe4c, 0x11cd, 0xa8, 0xed, 0x00, 0xaa, 0x00, 0x2f, 0xea, 0xb5);


//////////////////////////////////////////////////////////////////////
// CG7xxRenderFilter construction

CG7xxRenderFilter::CG7xxRenderFilter(LPUNKNOWN pUnk, HRESULT *phr)
				: CBaseFilter(NAME("G7xxRenderFilter"), pUnk, &m_Lock, CLSID_Dump)
{
	m_pPinInput = new CG7xxInputPin(this, GetOwner(), 
									&m_Lock, &m_ReceiveLock, phr);
	if ( ! m_pPinInput && phr ) *phr = E_OUTOFMEMORY;
}

CG7xxRenderFilter::~CG7xxRenderFilter()
{
	if ( m_pPinInput ) delete m_pPinInput;
}

//////////////////////////////////////////////////////////////////////
// CG7xxRenderFilter operations

int CG7xxRenderFilter::GetPinCount()
{
    return 1;
}

CBasePin* CG7xxRenderFilter::GetPin(int n)
{
	ASSERT( n == 0 );
	if ( n != 0 ) return NULL;
	
	return m_pPinInput;
}

//////////////////////////////////////////////////////////////////////
// CG7xxInputPin construction

CG7xxInputPin::CG7xxInputPin(CG7xxRenderFilter* pG7xxFilter, LPUNKNOWN pUnk,
							 CCritSec* pLock, CCritSec *pReceiveLock, HRESULT *phr)
: CRenderedInputPin(NAME("CG7xxInputPin"), pG7xxFilter, pLock, phr, L"Input")
, m_pG7xxFilter(pG7xxFilter)
, m_pReceiveLock(pReceiveLock)
{
}

CG7xxInputPin::~CG7xxInputPin()
{
}

//////////////////////////////////////////////////////////////////////
// CG7xxInputPin attributes

HRESULT CG7xxInputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
	
	if( *(pMediaType->Type()) != MEDIATYPE_Audio )
	{
		return E_INVALIDARG;
	}
	
	return NOERROR;
}

HRESULT CG7xxInputPin::Receive(IMediaSample* pSample)
{
	CAutoLock lock(m_pReceiveLock);
	
	if ( m_pG7xxFilter->m_State == State_Stopped ) return E_FAIL;
	
	return CBaseInputPin::Receive(pSample);
}

STDMETHODIMP
CG7xxInputPin::EndOfStream()
{
    CAutoLock lock(m_pReceiveLock);
    return CRenderedInputPin::EndOfStream();
}

STDMETHODIMP
CG7xxInputPin::EndFlush(void)
{
    return S_OK;
}

