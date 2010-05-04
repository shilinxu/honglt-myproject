//
// FilterG729Compressor.cpp
//

#include "stdafx.h"
#include "Xuser32.h"

#include "FilterG729Compressor.h"
#include "va_g729.h"
#include <initguid.h>

#pragma message("library is linking with \"va_g729.lib \"")
#pragma comment(lib,"va_g729.lib") 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// {41C375F2-BE80-4A66-AAE1-0516C05A8B0D}
DEFINE_GUID(CLSID_G729Compressor, 
0x41c375f2, 0xbe80, 0x4a66, 0xaa, 0xe1, 0x5, 0x16, 0xc0, 0x5a, 0x8b, 0xd);

//////////////////////////////////////////////////////////////////////
// CG729CompressorFilter construction

CG729CompressorFilter::CG729CompressorFilter(LPUNKNOWN pUnk, HRESULT *phr)
				: CTransformFilter(NAME("G729 Compressor"), pUnk, CLSID_G729Compressor)
{
	ASSERT( phr != NULL );
	ASSERT( m_pOutput == NULL );
	
	CG729OutputPin *pOut = new CG729OutputPin(this, phr);
	
	if ( pOut && SUCCEEDED(*phr) ) m_pOutput = pOut;
	else
	if ( pOut && FAILED(*phr) ) delete pOut;
	else
		*phr = E_OUTOFMEMORY;
	
	CTransformInputPin *pIn = new CTransformInputPin(
							NAME("G729 Input Pin"), this, phr, L"Input" );
	if ( pIn && SUCCEEDED(*phr) ) m_pInput = pIn;
	else
	if ( pIn && FAILED(*phr) ) delete pIn;
	else
		*phr = E_OUTOFMEMORY;
}

CG729CompressorFilter::~CG729CompressorFilter()
{
}

//////////////////////////////////////////////////////////////////////
// CG729CompressorFilter operations

HRESULT CG729CompressorFilter::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
    CheckPointer( mtIn, E_POINTER );
    CheckPointer( mtOut, E_POINTER );
	
	HRESULT hr = CheckInputType( mtIn );
	if ( FAILED(hr) ) return hr;
	
	return NOERROR;
}

HRESULT CG729CompressorFilter::CheckInputType(const CMediaType* mtIn)
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
		 pwfx->wBitsPerSample == 16 &&
		 pwfx->nSamplesPerSec == 8000 )
	{
		return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT CG729CompressorFilter::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	HRESULT hr = Copy( pIn, pOut );
	if ( FAILED(hr) ) return hr;
	
	return S_OK;
}

HRESULT CG729CompressorFilter::Copy(IMediaSample *pSource, IMediaSample *pDest) const
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
	
	// Copy the actual data length
	
	long lDataLength = pSource->GetActualDataLength();
	pDest->SetActualDataLength(lDataLength);
	
    return NOERROR;
}

HRESULT	CG729CompressorFilter::EncodeMedia(IMediaSample *pSource, IMediaSample *pDest) const
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

HRESULT CG729CompressorFilter::Receive(IMediaSample *pSample)
{
	return CTransformFilter::Receive(pSample);
}

HRESULT CG729CompressorFilter::GetMediaType(int iPosition, CMediaType *pMediaType)
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
	pwfx->wBitsPerSample = 16;
	pwfx->cbSize = 10;
	
	pMediaType->SetType( &MEDIATYPE_Audio );
	pMediaType->SetSubtype( &MEDIASUBTYPE_PCM );
	pMediaType->SetFormat( (BYTE *)pwfx, sizeof(WAVEFORMATEX) );
	pMediaType->SetFormatType( &FORMAT_WaveFormatEx );
	
	return S_OK;
}

HRESULT CG729CompressorFilter::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	HRESULT hr = NOERROR;
	if ( ! m_pInput->IsConnected() ) return E_UNEXPECTED;
	
	CheckPointer( pAlloc, E_POINTER );
	CheckPointer( pProperties, E_POINTER );
	
	pProperties->cBuffers = 1;
	pProperties->cbAlign  = 1;
	
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

HRESULT CG729CompressorFilter::StartStreaming()
{
	va_g729a_init_encoder();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CG729OutputPin construction

CG729OutputPin::CG729OutputPin(CTransformFilter* pFilter, HRESULT* phr) :
	CTransformOutputPin(NAME("G729 Output Pin"), pFilter, phr, L"Output")
{
}

CG729OutputPin::~CG729OutputPin()
{
}

//////////////////////////////////////////////////////////////////////
// CG729OutputPin operations

STDMETHODIMP CG729OutputPin::EnumMediaTypes( IEnumMediaTypes **ppEnum )
{
    return CBaseOutputPin::EnumMediaTypes(ppEnum);
}

HRESULT CG729OutputPin::CheckMediaType(const CMediaType* pMediaType)
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
//		 pwfx->wBitsPerSample == 16 &&
		 pwfx->nSamplesPerSec == 8000 )
	{
		return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
}
