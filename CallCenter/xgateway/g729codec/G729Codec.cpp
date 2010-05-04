//
// G729Codec.cpp
//

#include "stdafx.h"
#include "va_g729.h"

#include <initguid.h>
#include "G729Codec.h"

const AMOVIESETUP_MEDIATYPE sudPinTypes[] =
{
    { &MEDIATYPE_Audio, &MEDIASUBTYPE_PCM },
	{ &MEDIATYPE_Audio, &MEDIASUBTYPE_G729AAudio }
};

const AMOVIESETUP_PIN psudPins[] =
{
	{ L"Input", FALSE, FALSE, FALSE, FALSE, &CLSID_G729ACodec, NULL, 2, sudPinTypes },
	{ L"Output", FALSE, TRUE, FALSE, FALSE, &CLSID_G729ACodec, NULL, 2, sudPinTypes }
};

const AMOVIESETUP_FILTER sudCodec =
{
	&CLSID_G729ACodec, L"G.729A Codec", MERIT_NORMAL, 2, psudPins
};

//////////////////////////////////////////////////////////////////////
// CG729Codec construction

CG729Codec::CG729Codec(LPUNKNOWN pUnk, HRESULT *phr)
				: CTransformFilter(NAME("G729Codec"), pUnk, CLSID_G729ACodec)
{
	ASSERT( phr != NULL );
	ASSERT( m_pOutput == NULL );
	
	CG729Output *pOut = new CG729Output(this, phr);
	
	if ( pOut && SUCCEEDED(*phr) ) m_pOutput = pOut;
	else
	if ( pOut && FAILED(*phr) ) delete pOut;
	else
		*phr = E_OUTOFMEMORY;
	
	CTransformInputPin *pIn = new CTransformInputPin(
							NAME("G729InputPin"), this, phr, L"Input" );
	if ( pIn && SUCCEEDED(*phr) ) m_pInput = pIn;
	else
	if ( pIn && FAILED(*phr) ) delete pIn;
	else
		*phr = E_OUTOFMEMORY;
}

CG729Codec::~CG729Codec()
{
}

CUnknown* WINAPI CG729Codec::CreateInstance(LPUNKNOWN pUnk, HRESULT* phr)
{
	ASSERT( phr != NULL );
	
	CG729Codec* pFilter = new CG729Codec(pUnk, phr);
	if ( pFilter == NULL ) (* phr) = E_OUTOFMEMORY;
	
	return pFilter;
}

//////////////////////////////////////////////////////////////////////
// CG729Codec operations

HRESULT CG729Codec::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
    CheckPointer( mtIn, E_POINTER );
    CheckPointer( mtOut, E_POINTER );
	
	HRESULT hr = CheckInputType( mtIn );
	if ( FAILED(hr) ) return hr;
	
	if ( mtIn->subtype == MEDIASUBTYPE_PCM &&
		 mtOut->subtype == MEDIASUBTYPE_G729AAudio )
	{
		return NOERROR;
	}
	
	if ( mtIn->subtype == MEDIASUBTYPE_G729AAudio &&
		 mtOut->subtype == MEDIASUBTYPE_PCM )
	{
		return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT CG729Codec::CheckInputType(const CMediaType* mtIn)
{
	WAVEFORMATEX *pwfx = (WAVEFORMATEX *) mtIn->pbFormat;
//	DisplayMediaType( "CG729Codec::CheckInputType", mtIn );
	
	if ( mtIn->majortype != MEDIATYPE_Audio )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
/*	
	if ( mtIn->subtype == MEDIASUBTYPE_PCM &&
		 mtIn->subtype != MEDIASUBTYPE_G729AAudio )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
*/	
	if ( mtIn->formattype != FORMAT_WaveFormatEx )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	
	if ( pwfx->wFormatTag != WAVE_FORMAT_PCM &&
		 pwfx->wFormatTag != WAVE_FORMAT_G729A )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	
	if ( pwfx->nChannels == 1 &&
		 pwfx->wBitsPerSample == 16 &&
		 pwfx->nSamplesPerSec == 8000 )
	{
		return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT CG729Codec::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	HRESULT hr = Copy( pIn, pOut );
	if ( FAILED(hr) ) return hr;
	
	CMediaType* pMediaType = &m_pInput->CurrentMediaType();
	WAVEFORMATEX *pwfx = (WAVEFORMATEX *) pMediaType->pbFormat;
	
	if ( pMediaType->subtype == MEDIASUBTYPE_G729AAudio )
	//if ( pwfx->wFormatTag == WAVE_FORMAT_G729A)
	{
		DecodeMedia( pIn, pOut );
	}
	else // MEDIASUBTYPE_PCM
	{
		EncodeMedia( pIn, pOut );
	}
	
	return S_OK;
}

HRESULT CG729Codec::Copy(IMediaSample *pSource, IMediaSample *pDest) const
{
	CheckPointer( pSource, E_POINTER );
	CheckPointer( pDest, E_POINTER );
	
	// Copy the sample times
	
	REFERENCE_TIME TimeStart, TimeEnd;
	if(NOERROR == pSource->GetTime(&TimeStart, &TimeEnd))
	{
		pDest->SetTime(&TimeStart, &TimeEnd);
	}
	
	LONGLONG MediaStart, MediaEnd;
	if(pSource->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR)
	{
		pDest->SetMediaTime(&MediaStart,&MediaEnd);
	}

	// Copy the Sync point property

	HRESULT hr = pSource->IsSyncPoint();
	if(hr == S_OK)
	{
		pDest->SetSyncPoint(TRUE);
	}
	else if(hr == S_FALSE)
	{
		pDest->SetSyncPoint(FALSE);
	}
	else
    {
		return E_UNEXPECTED;
	}
	
	// Copy the media type
	
	AM_MEDIA_TYPE *pMediaType;
	pSource->GetMediaType(&pMediaType);
	pDest->SetMediaType(pMediaType);
	DeleteMediaType(pMediaType);

	// Copy the preroll property
	
	hr = pSource->IsPreroll();
	if(hr == S_OK)
	{
		pDest->SetPreroll(TRUE);
	}
	else if(hr == S_FALSE)
	{
		pDest->SetPreroll(FALSE);
	}
	else
	{
		return E_UNEXPECTED;
	}
	
	// Copy the discontinuity property
	
	hr = pSource->IsDiscontinuity();
	if(hr == S_OK)
	{
		pDest->SetDiscontinuity(TRUE);
	}
	else if(hr == S_FALSE)
	{
		pDest->SetDiscontinuity(FALSE);
	}
	else
	{
        return E_UNEXPECTED;
    }
	
    return NOERROR;
}

HRESULT CG729Codec::Receive(IMediaSample *pSample)
{
	return CTransformFilter::Receive(pSample);
}

HRESULT CG729Codec::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	if ( ! m_pInput->IsConnected() ) return E_UNEXPECTED;
	ASSERT( iPosition == 0 || iPosition == 1 );
	
	if ( iPosition < 0 ) return E_INVALIDARG;
	if ( iPosition > 0 ) return VFW_S_NO_MORE_ITEMS;
	
	*pMediaType = m_pInput->CurrentMediaType();
	WAVEFORMATEX *pwfx = (WAVEFORMATEX *) pMediaType->pbFormat;
	
	if ( pMediaType->subtype == MEDIASUBTYPE_G729AAudio )
	//if ( pwfx->wFormatTag == WAVE_FORMAT_G729A)
	{
		pMediaType->subtype = MEDIASUBTYPE_PCM;
		pwfx->wFormatTag = WAVE_FORMAT_PCM;
	}
	else // MEDIASUBTYPE_PCM
	{
		pMediaType->subtype = MEDIASUBTYPE_G729AAudio;
		pwfx->wFormatTag = WAVE_FORMAT_G729A;
	}

	return NOERROR;
}

HRESULT CG729Codec::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	HRESULT hr = NOERROR;
	if ( ! m_pInput->IsConnected() ) return E_UNEXPECTED;
	
	CheckPointer( pAlloc, E_POINTER );
	CheckPointer( pProperties, E_POINTER );
	
	pProperties->cBuffers = 1;
    pProperties->cbBuffer = m_pInput->CurrentMediaType().GetSampleSize();
	
	ALLOCATOR_PROPERTIES InProps;
	IMemAllocator * pInAlloc = NULL;
	
	hr = m_pInput->GetAllocator(&pInAlloc);
	if(SUCCEEDED(hr))
	{
		hr = pInAlloc->GetProperties(&InProps);
		if ( SUCCEEDED(hr) )
		{
			pProperties->cbBuffer = InProps.cbBuffer;
		}
		pInAlloc->Release();
    }
	
	if ( FAILED(hr) ) return hr;
	
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

HRESULT CG729Codec::StartStreaming()
{
	CMediaType* pMediaType = &m_pInput->CurrentMediaType();
	
	if ( pMediaType->subtype == MEDIASUBTYPE_G729AAudio )
	{
		va_g729a_init_decoder();
	}
	else // MEDIASUBTYPE_PCM
	{
		va_g729a_init_encoder();
	}
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CG729Codec encode/decode

HRESULT	CG729Codec::EncodeMedia(IMediaSample *pSource, IMediaSample *pDest) const
{
	// Copy the sample data
	
	BYTE *pSourceBuffer, *pDestBuffer;
	long lSourceSize = pSource->GetActualDataLength();
	
	pSource->GetPointer( &pSourceBuffer );
	pDest->GetPointer( &pDestBuffer );
	
	for ( BYTE *pSrc = pSourceBuffer, *pDst = pDestBuffer;
		pSrc - pSourceBuffer < lSourceSize; pSrc += L_FRAME * sizeof(WORD) )
	{
		va_g729a_encoder( (SHORT *)pSrc, pDst );
		pDst += L_FRAME_COMPRESSED * sizeof(BYTE);
	}
	
	// Copy the actual data length
	
	long nDestLength = long( pDst - pDestBuffer );
	pDest->SetActualDataLength( nDestLength );
	
	return S_OK;
}

HRESULT	CG729Codec::DecodeMedia(IMediaSample *pSource, IMediaSample *pDest) const
{
	// Copy the sample data
	
	BYTE *pSourceBuffer, *pDestBuffer;
	long lSourceSize = pSource->GetActualDataLength();
	
	pSource->GetPointer( &pSourceBuffer );
	pDest->GetPointer( &pDestBuffer );
long ldddSize = pDest->GetActualDataLength();
	
	for ( BYTE *pSrc = pSourceBuffer, *pDst = pDestBuffer;
		pSrc - pSourceBuffer < lSourceSize; pSrc += L_FRAME_COMPRESSED * sizeof(BYTE) )
	{
		va_g729a_decoder( pSrc, (SHORT *)pDst, 0 );
		pDst += L_FRAME * sizeof(WORD);
	}
	
	// Copy the actual data length
	
	long nDestLength = long( pDst - pDestBuffer );
	pDest->SetActualDataLength( nDestLength );
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CG729Output construction

CG729Output::CG729Output(CTransformFilter* pFilter, HRESULT* phr) :
	CTransformOutputPin(NAME("G729OutputPin"), pFilter, phr, L"Output")
{

}

CG729Output::~CG729Output()
{
}

//////////////////////////////////////////////////////////////////////
// CG729Output operations
/*
STDMETHODIMP CG729Output::EnumMediaTypes( IEnumMediaTypes **ppEnum )
{
    return CBaseOutputPin::EnumMediaTypes(ppEnum);
}
*/
HRESULT CG729Output::CheckMediaType(const CMediaType* pMediaType)
{
	CheckPointer( pMediaType, E_POINTER );
	WAVEFORMATEX *pwfx = (WAVEFORMATEX *) pMediaType->pbFormat;
	
	if ( pMediaType->majortype != MEDIATYPE_Audio )
	{
		return E_INVALIDARG;
	}
	
	if ( pMediaType->formattype != FORMAT_WaveFormatEx )
	{
		return E_INVALIDARG;
	}
	
	if ( pwfx->wFormatTag != WAVE_FORMAT_PCM &&
		 pwfx->wFormatTag != WAVE_FORMAT_G729A )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	if ( pwfx->nChannels == 1 &&
		 pwfx->wBitsPerSample == 16 &&
		 pwfx->nSamplesPerSec == 8000 )
	{
		return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
}

void CG729Codec::DisplayMediaType(TCHAR *pDescription,const CMediaType *pmt) 
{ 
#ifdef _DEBUG
	DbgLog((LOG_TRACE, 0, TEXT("")));
	DbgLog((LOG_TRACE, 0, TEXT("%s"),pDescription));
	DbgLog((LOG_TRACE, 0, TEXT("Media Type Description")));
	DbgLog((LOG_TRACE, 0, TEXT("Major type: %s"),GuidNames[*pmt->Type()]));
	DbgLog((LOG_TRACE, 0, TEXT("Subtype: %s"),GuidNames[*pmt->Subtype()]));
	DbgLog((LOG_TRACE, 0, TEXT("Subtype description: %s"),GetSubtypeName(pmt->Subtype())));
	DbgLog((LOG_TRACE, 0, TEXT("Format size: %d"),pmt->cbFormat)); 
	
	// Dump the generic media types 
	
	DbgLog((LOG_TRACE, 0, TEXT("Fixed size sample %d"),pmt->IsFixedSize())); 
	DbgLog((LOG_TRACE, 0, TEXT("Temporal compression %d"),pmt->IsTemporalCompressed())); 
	DbgLog((LOG_TRACE, 0, TEXT("Sample size %d"),pmt->GetSampleSize())); 
#endif
}
