//
// H263Codec.cpp
//

#include "stdafx.h"

#include <initguid.h>
#include "H263Codec.h"

const AMOVIESETUP_MEDIATYPE sudPinTypes[] =
{
    { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB24 },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_H263 }
};

const AMOVIESETUP_PIN psudPins[] =
{
	{ L"Input", FALSE, FALSE, FALSE, FALSE, &CLSID_H263Codec, NULL, 2, sudPinTypes },
	{ L"Output", FALSE, TRUE, FALSE, FALSE, &CLSID_H263Codec, NULL, 2, sudPinTypes }
};

const AMOVIESETUP_FILTER sudCodec =
{
	&CLSID_H263Codec, L"H.263 Codec", 0x268800, 2, psudPins
};

int	clength = 0;
BYTE cbuffer[10000];
int cbuffer_size=10000;
long UpendBmp(unsigned char *lpdata,long width ,long height);

void OwnWriteFunction(int byte)
{
	if ( clength < cbuffer_size )
	{
		cbuffer[ clength ] = (unsigned char) byte;
	}
	
	clength ++;
}

//////////////////////////////////////////////////////////////////////
// CH263Codec construction

CH263Codec::CH263Codec(LPUNKNOWN pUnk, HRESULT *phr)
				: CTransformFilter(NAME("H263Codec"), pUnk, CLSID_H263Codec)
{
	ASSERT( phr != NULL );
	ASSERT( m_pOutput == NULL );
	
	CH263Output *pOut = new CH263Output(this, phr);
	
	if ( pOut && SUCCEEDED(*phr) ) m_pOutput = pOut;
	else
	if ( pOut && FAILED(*phr) ) delete pOut;
	else
		*phr = E_OUTOFMEMORY;
	
	CTransformInputPin *pIn = new CTransformInputPin(
							NAME("H263InputPin"), this, phr, L"Input" );
	if ( pIn && SUCCEEDED(*phr) ) m_pInput = pIn;
	else
	if ( pIn && FAILED(*phr) ) delete pIn;
	else
		*phr = E_OUTOFMEMORY;
}

CH263Codec::~CH263Codec()
{
}

CUnknown* WINAPI CH263Codec::CreateInstance(LPUNKNOWN pUnk, HRESULT* phr)
{
	ASSERT( phr != NULL );
	
	CH263Codec* pFilter = new CH263Codec(pUnk, phr);
	if ( pFilter == NULL ) (* phr) = E_OUTOFMEMORY;
	
	return pFilter;
}

//////////////////////////////////////////////////////////////////////
// CH263Codec operations

HRESULT CH263Codec::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
    CheckPointer( mtIn, E_POINTER );
    CheckPointer( mtOut, E_POINTER );
	
	HRESULT hr = CheckInputType( mtIn );
	if ( FAILED(hr) ) return hr;
	
	if ( mtIn->subtype == MEDIASUBTYPE_RGB24 &&
		 mtOut->subtype == MEDIASUBTYPE_H263 )
	{
		return NOERROR;
	}
	
	if ( mtIn->subtype == MEDIASUBTYPE_H263 &&
		 mtOut->subtype == MEDIASUBTYPE_RGB24 )
	{
		return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT CH263Codec::CheckInputType(const CMediaType* mtIn)
{
	VIDEOINFOHEADER *pvih = (VIDEOINFOHEADER *) mtIn->pbFormat;
	DisplayMediaType( "CH263Codec::CheckInputType", mtIn );
	
	if ( mtIn->majortype != MEDIATYPE_Video )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	
	if ( (mtIn->subtype != MEDIASUBTYPE_RGB24 || pvih->bmiHeader.biCompression != BI_RGB) &&
		 (mtIn->subtype != MEDIASUBTYPE_H263 || pvih->bmiHeader.biCompression != MAKEFOURCC('H','2','6','3')) )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	
	if ( pvih->bmiHeader.biWidth == QCIF_WIDTH && pvih->bmiHeader.biHeight == QCIF_HEIGHT &&
		 pvih->bmiHeader.biBitCount == 24 )
	{
		return NOERROR;
	}

	return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT CH263Codec::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	HRESULT hr = Copy( pIn, pOut );
	if ( FAILED(hr) ) return hr;
	
	CMediaType* pMediaType = &m_pInput->CurrentMediaType();
	
	if ( pMediaType->subtype == MEDIASUBTYPE_H263 )
	{
		DecompressFrame( pIn, pOut );
	}
	else // MEDIASUBTYPE_RGB24
	{
		CompressFrame( pIn, pOut );
	}
	
	return S_OK;
}

HRESULT CH263Codec::Copy(IMediaSample *pSource, IMediaSample *pDest) const
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

HRESULT CH263Codec::Receive(IMediaSample *pSample)
{
	return CTransformFilter::Receive(pSample);
}

HRESULT CH263Codec::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	if ( ! m_pInput->IsConnected() ) return E_UNEXPECTED;
	ASSERT( iPosition == 0 || iPosition == 1 );
	
	if ( iPosition < 0 ) return E_INVALIDARG;
	if ( iPosition > 0 ) return VFW_S_NO_MORE_ITEMS;
	
#if 1
	*pMediaType = m_pInput->CurrentMediaType();
#else
	pMediaType->majortype = MEDIATYPE_Video;
#endif
	VIDEOINFOHEADER *pvih = (VIDEOINFOHEADER *) pMediaType->pbFormat;
//	pvih->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	
	if ( pMediaType->subtype == MEDIASUBTYPE_H263 )
	{
		pMediaType->subtype = MEDIASUBTYPE_RGB24;
		pvih->bmiHeader.biCompression = BI_RGB;
//		pvih->bmiHeader.biBitCount = 24;
//		pvih->bmiHeader.biWidth=QCIF_WIDTH;
//		pvih->bmiHeader.biHeight=QCIF_HEIGHT;
//	pvih->bmiHeader.biSizeImage = QCIF_WIDTH*QCIF_HEIGHT*24>>3;
	}
	else // MEDIASUBTYPE_RGB24
	{
		pMediaType->subtype = MEDIASUBTYPE_H263;
		pvih->bmiHeader.biCompression = MAKEFOURCC('H','2','6','3');
	}

	return NOERROR;
}

HRESULT CH263Codec::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	HRESULT hr = NOERROR;
	if ( ! m_pInput->IsConnected() ) return E_UNEXPECTED;
	
	CheckPointer( pAlloc, E_POINTER );
	CheckPointer( pProperties, E_POINTER );
	
	pProperties->cBuffers = 1;
    pProperties->cbBuffer = m_pInput->CurrentMediaType().GetSampleSize();
	
	if ( ! m_pInput->CurrentMediaType().bFixedSizeSamples )
	{   
		pProperties->cbBuffer = QCIF_WIDTH * QCIF_HEIGHT * 24 >> 3;
	}
	
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

HRESULT CH263Codec::StartStreaming()
{
	CMediaType* pMediaType = &m_pInput->CurrentMediaType();
	
	InitLookupTable();
	
	if ( pMediaType->subtype == MEDIASUBTYPE_H263 )
	{
		InitH263Decoder();
	}
	else // MEDIASUBTYPE_RGB24
	{
		m_pParams.format = CPARAM_QCIF;
		InitH263Encoder( &m_pParams );  
		
		WriteByteFunction = OwnWriteFunction;
	}
	
	return CTransformFilter::StartStreaming();
	return S_OK;
}
#if 0
HRESULT CH263Codec::StopStreaming()
{
	CMediaType* pMediaType = &m_pInput->CurrentMediaType();
	
	if ( pMediaType->subtype == MEDIASUBTYPE_H263 )
	{
		ExitH263Decoder();
	}
	else // MEDIASUBTYPE_RGB24
	{
		ExitH263Encoder( &m_pParams );
	}
	
	return CTransformFilter::StopStreaming();
}
#endif
/*
STDMETHODIMP
CH263Codec::Run(REFERENCE_TIME tStart)
{
	return CTransformFilter::Run(tStart);
}
*/
//////////////////////////////////////////////////////////////////////
// CH263Codec encode/decode

HRESULT	CH263Codec::CompressFrame(IMediaSample *pSource, IMediaSample *pDest)
{
	// Copy the sample data
	
	BYTE *pSourceBuffer, *pDestBuffer;
	long lSourceSize = pSource->GetActualDataLength();
	
	pSource->GetPointer( &pSourceBuffer );
	pDest->GetPointer( &pDestBuffer );
	
//	BYTE *pSourceBuffer1 = new BYTE[ lSourceSize];
//	CopyMemory( pSourceBuffer1, pSourceBuffer, lSourceSize);
	ConvertRGB2YUV( QCIF_WIDTH, QCIF_HEIGHT, pSourceBuffer, (unsigned int*)yuvBuffer );
//	delete [] pSourceBuffer1;
//	UpendBmp( (BYTE*)yuvBuffer, QCIF_WIDTH, QCIF_HEIGHT );

	// Reset the counter
	clength = 0;
	
	//Compress the data...to h263
	m_pParams.format	= CPARAM_QCIF;
	m_pParams.inter	= CPARAM_INTRA;  
	m_pParams.Q_intra	= 8;
	m_pParams.data	= yuvBuffer;
	
	Bits bits;
	::CompressFrame( &m_pParams, &bits );
	
	CopyMemory( pDestBuffer, cbuffer, clength );
	
	// Copy the actual data length
	
	long nDestLength = clength;
	pDest->SetActualDataLength( nDestLength );
	
	return S_OK;
}

HRESULT	CH263Codec::DecompressFrame(IMediaSample *pSource, IMediaSample *pDest)
{
	// Copy the sample data
	
	BYTE *pSourceBuffer, *pDestBuffer;
	long lSourceSize = pSource->GetActualDataLength();
	long lDestSize = pDest->GetActualDataLength();
	
	pSource->GetPointer( &pSourceBuffer );
	pDest->GetPointer( &pDestBuffer );
	
	// Copy the actual data length
	
	::DecompressFrame( pSourceBuffer, lSourceSize, pDestBuffer, lDestSize );
	pDest->SetActualDataLength( lDestSize );
	
//	UpendBmp( pDestBuffer, QCIF_WIDTH, QCIF_HEIGHT );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CH263Output construction

CH263Output::CH263Output(CTransformFilter* pFilter, HRESULT* phr) :
	CTransformOutputPin(NAME("H263OutputPin"), pFilter, phr, L"Output")
{

}

CH263Output::~CH263Output()
{
}

//////////////////////////////////////////////////////////////////////
// CH263Output operations
/*
STDMETHODIMP CH263Output::EnumMediaTypes( IEnumMediaTypes **ppEnum )
{
    return CBaseOutputPin::EnumMediaTypes(ppEnum);
}
*/
HRESULT CH263Output::CheckMediaType(const CMediaType* pMediaType)
{
	CheckPointer( pMediaType, E_POINTER );
	
	if ( pMediaType->majortype != MEDIATYPE_Video )
	{
		return E_INVALIDARG;
	}
	
	if ( pMediaType->subtype == MEDIASUBTYPE_RGB24 ||
		 pMediaType->subtype == MEDIASUBTYPE_H263 )
	{
		return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
}

void CH263Codec::DisplayMediaType(TCHAR *pDescription,const CMediaType *pmt) 
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
//把图像倒立过来；
long UpendBmp(unsigned char *lpdata,long width ,long height)
{



long lBPL;//每行的字节数，因为要考虑dword对齐
long x,y,idx_src,idx_dest;
unsigned char *tmpdata;



if (0==((width*3)%4)) //nWidth * 3 是存储每行像素需要的字节数，如果是4的整数倍。
lBPL = (width*3); //那么返回 nWidth * 3 ，就是每行的字节数
else //如果不是4的整数倍，那么就一定要加上一个数，达到4的整数倍，才是每行的字节数。
lBPL = (width*3+(4-((width*3)%4)));

tmpdata= new unsigned char[lBPL * height];

x =0;
for (y=0 ; y<height ; y++)
{
idx_src =(height-1-y)*lBPL;//idx_src =(height-1-y)*lBPL+x*3;优化前
idx_dest=y*lBPL;//idx_dest=y*lBPL+x*3;优化前
memcpy(&tmpdata[idx_dest],&lpdata[idx_src],lBPL);//复制一行
} 



memcpy(lpdata,tmpdata,lBPL * height);
delete[] tmpdata;



return 0;
}

