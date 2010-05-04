//
// FilterRtpReceiver.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include <initguid.h>
#include "FilterRtpReceiver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// {7B2C2A04-495F-4ff7-B93B-6BBD710299AD}
DEFINE_GUID(CLSID_RtpReceiver, 
0x7b2c2a04, 0x495f, 0x4ff7, 0xb9, 0x3b, 0x6b, 0xbd, 0x71, 0x2, 0x99, 0xad);

void DisplayMediaType(TCHAR *pDescription,const CMediaType *pmt) ;

//////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter construction

CRtpReceiverFilter::CRtpReceiverFilter(LPUNKNOWN pUnk, HRESULT *phr)
: CBaseFilter(NAME("Rtp Receiver Filter"), pUnk, &m_Lock, CLSID_RtpReceiver)
{
	ASSERT( phr != NULL );
	CRtpOutputPin* pOutput =
	new CRtpOutputPin(this, &m_Lock, phr);
	
	if ( pOutput && SUCCEEDED(*phr) ) m_pOutput = pOutput;
	else
	if ( pOutput && FAILED(*phr) ) delete pOutput;
	else
		*phr = E_OUTOFMEMORY;
}

CRtpReceiverFilter::~CRtpReceiverFilter()
{
}

//////////////////////////////////////////////////////////////////////
// CRtpReceiverFilter operations

int CRtpReceiverFilter::GetPinCount()
{
    return 1;
}

CBasePin* CRtpReceiverFilter::GetPin(int n)
{
	ASSERT( n == 0 );
	if ( n != 0 ) return NULL;
	
	return m_pOutput;
}

//////////////////////////////////////////////////////////////////////
// CRtpOutputPin construction

CRtpOutputPin::CRtpOutputPin(CRtpReceiverFilter* pFilter, CCritSec* pLock, HRESULT *phr)
: CBaseOutputPin(NAME("Rtp Output Pin"), pFilter, pLock, phr, L"Output")
{
}

CRtpOutputPin::~CRtpOutputPin()
{
}

//////////////////////////////////////////////////////////////////////
// CRtpOutputPin attributes

HRESULT CRtpOutputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
#if 0
    if (iPosition<0) {
        return E_INVALIDARG;
    }
    if (iPosition>0) {
        return VFW_S_NO_MORE_ITEMS;
    }
    //return GetMediaType(pMediaType);
	return TRUE;
#else	
	CheckPointer(pMediaType,E_POINTER);
if ( iPosition==0)	
{
	pMediaType->InitMediaType();
	
	pMediaType->SetType(& MEDIATYPE_Stream) ;
	pMediaType->SetSubtype(& MEDIASUBTYPE_Avi) ;
//	pMediaType->SetFormatType(&CLSID_NULL);
	pMediaType->SetSampleSize(1); 
//         pMediaType->SetVariableSize(); 
         pMediaType->SetTemporalCompression(FALSE); 
	//	 pMediaType->SetFixedSizeSample()

	return NOERROR;
}
else
{
		return VFW_S_NO_MORE_ITEMS;
}
#endif
}

HRESULT CRtpOutputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
#if 1	
	if ( pMediaType->majortype == MEDIATYPE_Stream &&
		pMediaType->subtype == MEDIASUBTYPE_Avi )
	{
	DisplayMediaType( "test:", pMediaType);
		return NOERROR;
	}
	
	return E_INVALIDARG;
#else
    CMediaType mt;
    GetMediaType(&mt);

    if (mt == *pMediaType) {
        return NOERROR;
    }

    return E_FAIL;
#endif
}
HRESULT
CRtpOutputPin::InitAllocator (
    OUT IMemAllocator ** ppAlloc
    )
{
    return S_OK ;
}

HRESULT CRtpOutputPin::DecideBufferSize(IMemAllocator *pMemAllocator,
                                        ALLOCATOR_PROPERTIES * ppropInputRequest)
{
	return NOERROR;
}
HRESULT CRtpOutputPin::Active()
{
        CBaseOutputPin::Active();
        return S_OK;

}
void DisplayMediaType(TCHAR *pDescription,const CMediaType *pmt) 
{ 
// Dump the GUID types and a short description 
DbgLog((LOG_TRACE,0,TEXT(""))); 
DbgLog((LOG_TRACE,0,TEXT("%s"),pDescription)); 
DbgLog((LOG_TRACE,0,TEXT(""))); 
DbgLog((LOG_TRACE,0,TEXT("Media Type Description"))); 
DbgLog((LOG_TRACE,0,TEXT("Major type: %s"),GuidNames[*pmt->Type()])); 
DbgLog((LOG_TRACE,0,TEXT("Subtype: %s"),GuidNames[*pmt->Subtype()])); 
DbgLog((LOG_TRACE,0,TEXT("Subtype description: %s"),GetSubtypeName(pmt->Subtype()))); 
DbgLog((LOG_TRACE,0,TEXT("Format size: %d"),pmt->cbFormat)); 
// Dump the generic media types 
DbgLog((LOG_TRACE,0,TEXT("Fixed size sample %d"),pmt->IsFixedSize())); 
DbgLog((LOG_TRACE,0,TEXT("Temporal compression %d"),pmt->IsTemporalCompressed())); 
DbgLog((LOG_TRACE,0,TEXT("Sample size %d"),pmt->GetSampleSize())); 
} 
