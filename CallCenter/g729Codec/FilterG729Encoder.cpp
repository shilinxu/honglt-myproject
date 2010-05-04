//
// FilterG729Encoder.cpp
//

#include "stdafx.h"
#include "va_g729.h"

#include <initguid.h>
#include "FilterG729Encoder.h"

const AMOVIESETUP_FILTER sudEncoder =
{
	&CLSID_G729Encoder, L"G.729 Encoder", MERIT_DO_NOT_USE, 0, 0
};

//////////////////////////////////////////////////////////////////////
// CG729EncoderFilter construction

CG729EncoderFilter::CG729EncoderFilter(LPUNKNOWN pUnk, HRESULT *phr)
				: CTransformFilter(NAME("G729Encoder"), pUnk, CLSID_G729Encoder)
{
	ASSERT( phr != NULL );
	ASSERT( m_pOutput == NULL );
	
	CG729EncoderOutput *pOut = new CG729EncoderOutput(this, phr);
	
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

CG729EncoderFilter::~CG729EncoderFilter()
{
}

CUnknown* WINAPI CG729EncoderFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT* phr)
{
	ASSERT( phr != NULL );
	
	CG729EncoderFilter* pFilter = new CG729EncoderFilter(pUnk, phr);
	if ( pFilter == NULL ) (* phr) = E_OUTOFMEMORY;
	
	return pFilter;
}

//////////////////////////////////////////////////////////////////////
// CG729EncoderFilter operations

HRESULT CG729EncoderFilter::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
    CheckPointer( mtIn, E_POINTER );
    CheckPointer( mtOut, E_POINTER );
	
	HRESULT hr = CheckInputType( mtIn );
	if ( FAILED(hr) ) return hr;
	
	return NOERROR;
}

HRESULT CG729EncoderFilter::CheckInputType(const CMediaType* mtIn)
{
	WAVEFORMATEX *pwfx = (WAVEFORMATEX *) mtIn->pbFormat;
	
	if ( mtIn->majortype != MEDIATYPE_Audio )
	{
		return E_INVALIDARG;
	}
	
	if ( mtIn->formattype != FORMAT_WaveFormatEx )
	{
		return E_INVALIDARG;
	}
	
	if ( pwfx->wFormatTag != WAVE_FORMAT_PCM )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	
	if ( pwfx->nChannels == 1 &&
//		 pwfx->wBitsPerSample == 16 &&
		 pwfx->nSamplesPerSec == 8000 )
	{
		return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT CG729EncoderFilter::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	HRESULT hr = Copy( pIn, pOut );
	if ( FAILED(hr) ) return hr;
	
	return S_OK;
}

HRESULT CG729EncoderFilter::Copy(IMediaSample *pSource, IMediaSample *pDest) const
{
	CheckPointer( pSource, E_POINTER );
	CheckPointer( pDest, E_POINTER );
	
	// Encode the sample data
	
	EncodeMedia( pSource, pDest );
	
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

HRESULT	CG729EncoderFilter::EncodeMedia(IMediaSample *pSource, IMediaSample *pDest) const
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

HRESULT CG729EncoderFilter::Receive(IMediaSample *pSample)
{
	return CTransformFilter::Receive(pSample);
}

HRESULT CG729EncoderFilter::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	ASSERT( iPosition == 0 || iPosition == 1 );
	
	if ( iPosition < 0 ) return E_INVALIDARG;
	if ( iPosition > 0 ) return VFW_S_NO_MORE_ITEMS;
	
	HRESULT hr = S_OK;
	CheckPointer( pMediaType, E_POINTER );
	
	WAVEFORMATEX* pwfx = (WAVEFORMATEX *)pMediaType->AllocFormatBuffer( sizeof(WAVEFORMATEX) );
	if ( ! pwfx ) return E_OUTOFMEMORY;
	
	ZeroMemory( pwfx, sizeof(WAVEFORMATEX) );
	
	pwfx->wFormatTag = WAVE_FORMAT_PCM;
	pwfx->nChannels = 1;
	pwfx->nSamplesPerSec = 8000;
	pwfx->wBitsPerSample = 8;
	pwfx->cbSize = 10;
	
	pMediaType->SetType( &MEDIATYPE_Audio );
	pMediaType->SetSubtype( &MEDIASUBTYPE_PCM );
	pMediaType->SetFormat( (BYTE *)pwfx, sizeof(WAVEFORMATEX) );
	pMediaType->SetFormatType( &FORMAT_WaveFormatEx );
	
	return S_OK;
}

HRESULT CG729EncoderFilter::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	HRESULT hr = NOERROR;
	if ( ! m_pInput->IsConnected() ) return E_UNEXPECTED;
	
	CheckPointer( pAlloc, E_POINTER );
	CheckPointer( pProperties, E_POINTER );
	
	pProperties->cBuffers = 1;
//	pProperties->cbAlign  = 1;
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

HRESULT CG729EncoderFilter::StartStreaming()
{
	va_g729a_init_encoder();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CG729EncoderOutput construction

CG729EncoderOutput::CG729EncoderOutput(CTransformFilter* pFilter, HRESULT* phr) :
	CTransformOutputPin(NAME("G729OutputPin"), pFilter, phr, L"Output")
{
}

CG729EncoderOutput::~CG729EncoderOutput()
{
}

//////////////////////////////////////////////////////////////////////
// CG729EncoderOutput operations

STDMETHODIMP CG729EncoderOutput::EnumMediaTypes( IEnumMediaTypes **ppEnum )
{
    return CBaseOutputPin::EnumMediaTypes(ppEnum);
}

HRESULT CG729EncoderOutput::CheckMediaType(const CMediaType* pMediaType)
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
	
	if ( pwfx->wFormatTag != WAVE_FORMAT_PCM )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	
	if ( pwfx->nChannels == 1 &&
//		 pwfx->wBitsPerSample == 8 &&
		 pwfx->nSamplesPerSec == 8000 )
	{
		return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
}
