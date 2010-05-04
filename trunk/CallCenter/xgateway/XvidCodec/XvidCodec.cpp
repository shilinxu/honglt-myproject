//
// XvidCodec.cpp
//

#include "stdafx.h"
#include <initguid.h>
#include "XvidCodec.h"

#include "xvid.h"
#include <dvdmedia.h>	// VIDEOINFOHEADER2
#pragma comment(lib,"libxvidcore.lib")

const float	ARG_FRAMERATE = 25.00f;

static const int PARS[][2] =
{
	{1, 1},
	{12, 11},
	{10, 11},
	{16, 11},
	{40, 33},
	{0, 0},
};

const AMOVIESETUP_MEDIATYPE sudPinTypes[] =
{
    { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB24 },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_XVID }
};

const AMOVIESETUP_PIN psudPins[] =
{
	{ L"Input11", FALSE, FALSE, FALSE, FALSE, &CLSID_XvidCodec, NULL, 2, sudPinTypes },
	{ L"Output", FALSE, TRUE, FALSE, FALSE, &CLSID_XvidCodec, NULL, 2, sudPinTypes }
};

const AMOVIESETUP_FILTER sudCodec =
{
	&CLSID_XvidCodec, L"Xvid MPEG-4 Codec", 0x268800, 2, psudPins
};

//////////////////////////////////////////////////////////////////////
// CXvidCodec create instance

CUnknown* WINAPI CXvidCodec::CreateInstance(LPUNKNOWN pUnk, HRESULT* phr)
{
	ASSERT( phr != NULL );
	
	CXvidCodec* pNewObject = new CXvidCodec(pUnk, phr);
	if ( pNewObject == NULL ) (* phr) = E_OUTOFMEMORY;
	
	return pNewObject;
}

//////////////////////////////////////////////////////////////////////
// CXvidCodec construction

CXvidCodec::CXvidCodec(LPUNKNOWN pUnk, HRESULT *phr)
	: CVideoTransformFilter(NAME("XvidCodec"), pUnk, CLSID_XvidCodec)
{
	xvid_gbl_init_t init;
	ZeroMemory( &init, sizeof(init) );
	
	init.version = XVID_VERSION;
	
	xvid_global( NULL, XVID_GBL_INIT, &init, NULL );
	
	xvid_enc_handle = NULL;
	xvid_dec_handle = NULL;
}

CXvidCodec::~CXvidCodec()
{
	if ( xvid_enc_handle )
	{
		xvid_encore( xvid_enc_handle, XVID_ENC_DESTROY, NULL, NULL );
	}
	if ( xvid_dec_handle )
	{
		xvid_decore( xvid_dec_handle, XVID_DEC_DESTROY, NULL, NULL );
	}
}

HRESULT CXvidCodec::XvidInitEncode(void*& xvid_handle)
{
	xvid_enc_create_t xvid_enc_create;
	ZeroMemory( &xvid_enc_create, sizeof(xvid_enc_create) );
	
	/* Version again */
	xvid_enc_create.version = XVID_VERSION;
	
	/* Global encoder options */
	xvid_enc_create.global = 0;
	
	/* Width and Height of input frames */
	xvid_enc_create.width = QCIF_WIDTH;
	xvid_enc_create.height = QCIF_HEIGHT;
	xvid_enc_create.profile = XVID_PROFILE_AS_L4;
	
	/* Frame rate - Do some quick float fps = fincr/fbase hack */
	xvid_enc_create.fbase = (int) ARG_FRAMERATE;
	xvid_enc_create.fincr = 1;

	/* Maximum key frame interval */
    xvid_enc_create.max_key_interval = (int) ARG_FRAMERATE *10;
	
	/* Dropping ratio frame -- we don't need that */
	xvid_enc_create.frame_drop_ratio = 0;

	/* Bframes settings */
	xvid_enc_create.max_bframes = 0;

	if ( xvid_encore(NULL, XVID_ENC_CREATE, &xvid_enc_create, NULL) < 0 ) return S_FALSE;
	xvid_handle = xvid_enc_create.handle;
	
	ZeroMemory( &xvid_enc_frame, sizeof(xvid_enc_frame) );
	
	/* Version for the frame and the stats */
	xvid_enc_frame.version = XVID_VERSION;
	
	/* Set up core's general features */
	xvid_enc_frame.vol_flags  = 0;
	//xvid_enc_frame.vol_flags |= XVID_VOL_MPEGQUANT; //允许MPEG量化
	//xvid_enc_frame.vol_flags |= XVID_VOL_GMC; //允许GMC
	//xvid_enc_frame.vol_flags |= XVID_VOL_QUARTERPEL; //采用1/4像素运算
	
	/* Set up core's general features */
	xvid_enc_frame.vop_flags  = 0;
	xvid_enc_frame.vop_flags |= XVID_VOP_CHROMAOPT; //允许色彩优化
	xvid_enc_frame.vop_flags |= XVID_VOP_HALFPEL; //允许半像素
	
	/* Set up motion estimation flags */
	xvid_enc_frame.motion = 0; //运动标识
	xvid_enc_frame.quant = 0; //质量控制=量化参数，1~31，数值越小质量越高
	xvid_enc_frame.bframe_threshold =0;
	xvid_enc_frame.quant_intra_matrix = NULL;
	xvid_enc_frame.quant_inter_matrix = NULL;
	
	/* Frame type -- let core decide for us */
	xvid_enc_frame.type = XVID_TYPE_AUTO;
	
	return S_OK;
}

HRESULT CXvidCodec::XvidInitDecode(void*& xvid_handle)
{
	xvid_dec_create_t xvid_dec_create;
	ZeroMemory( &xvid_dec_create, sizeof(xvid_dec_create) );
	
	xvid_dec_create.version = XVID_VERSION;
	
	if ( xvid_decore(NULL, XVID_DEC_CREATE, &xvid_dec_create, NULL) < 0 ) return S_FALSE;
	xvid_handle = xvid_dec_create.handle;
	
	ZeroMemory( &xvid_dec_frame, sizeof(xvid_dec_frame) );
	
	/* Version for the frame and the stats */
	xvid_dec_frame.version = XVID_VERSION;
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CXvidCodec operations

HRESULT CXvidCodec::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
#if 0
    CheckPointer( mtIn, E_POINTER );
    CheckPointer( mtOut, E_POINTER );
	
	HRESULT hr = CheckInputType( mtIn );
	if ( FAILED(hr) ) return hr;
	
	if ( mtIn->subtype == MEDIASUBTYPE_RGB24 &&
		 mtOut->subtype == MEDIASUBTYPE_XVID )
	{
		return NOERROR;
	}
	
	if ( mtIn->subtype == MEDIASUBTYPE_XVID &&
		 mtOut->subtype == MEDIASUBTYPE_YUY2 )
	{
		return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
#else
		return S_OK;
#endif
}

HRESULT CXvidCodec::CheckInputType(const CMediaType* mtIn)
{
	DisplayMediaType( "CXvidCodec::CheckInputType", mtIn );
	ar_x = ar_y = 0;
	
	if ( mtIn->majortype != MEDIATYPE_Video )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	
	if ( mtIn->subtype == MEDIASUBTYPE_RGB24 )
	{
		VIDEOINFOHEADER *pvih = (VIDEOINFOHEADER *) mtIn->pbFormat;
		
		if ( pvih->bmiHeader.biCompression != BI_RGB ) return VFW_E_TYPE_NOT_ACCEPTED;
		if ( pvih->bmiHeader.biWidth != QCIF_WIDTH || pvih->bmiHeader.biHeight != QCIF_HEIGHT ) return VFW_E_TYPE_NOT_ACCEPTED;
		
		if ( xvid_enc_handle || SUCCEEDED(XvidInitEncode(xvid_enc_handle)) ) return NOERROR;
	}
	
	if ( mtIn->subtype == MEDIASUBTYPE_XVID )
	{
		VIDEOINFOHEADER *pvih = (VIDEOINFOHEADER *) mtIn->pbFormat;
		if ( pvih->bmiHeader.biCompression != MAKEFOURCC('X','V','I','D') ) return VFW_E_TYPE_NOT_ACCEPTED;
		
		if ( xvid_dec_handle || SUCCEEDED(XvidInitDecode(xvid_dec_handle)) ) return NOERROR;
	}
	
	return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT CXvidCodec::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	if ( ! m_pInput->IsConnected() ) return E_UNEXPECTED;
	
	if ( iPosition < 0 ) return E_INVALIDARG;
	if ( iPosition > 0 ) return VFW_S_NO_MORE_ITEMS;

	BITMAPINFOHEADER * bmih, *bmih2;
	CMediaType& mt = m_pInput->CurrentMediaType();
	bmih2 = &((VIDEOINFOHEADER *) mt.pbFormat)->bmiHeader;
	
	if ( xvid_enc_handle )
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER *) pMediaType->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER));
		ZeroMemory(vih, sizeof (VIDEOINFOHEADER));
		bmih = &(vih->bmiHeader);
		
		vih->bmiHeader.biCompression = MEDIASUBTYPE_XVID.Data1;
		vih->bmiHeader.biBitCount = 24;
		
		pMediaType->SetSubtype( &MEDIASUBTYPE_XVID );
		pMediaType->SetFormatType( &FORMAT_VideoInfo );
	}
	else
	if ( xvid_dec_handle )
	{
		VIDEOINFOHEADER2 * vih = (VIDEOINFOHEADER2 *) pMediaType->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER2));
		ZeroMemory(vih, sizeof (VIDEOINFOHEADER2));
		bmih = &(vih->bmiHeader);
		
		vih->dwPictAspectRatioX = bmih2->biWidth;
		vih->dwPictAspectRatioY = abs( bmih2->biHeight );
		
		vih->bmiHeader.biCompression = MEDIASUBTYPE_YUY2.Data1;
		vih->bmiHeader.biBitCount = 16;
		
		pMediaType->SetSubtype( &MEDIASUBTYPE_YUY2 );
		pMediaType->SetFormatType( &FORMAT_VideoInfo2 );
	}
	
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth  = bmih2->biWidth;
	bmih->biHeight = bmih2->biHeight;
	bmih->biPlanes = 1;
	
	bmih->biSizeImage = GetBitmapSize( bmih );
	
	pMediaType->SetType( &MEDIATYPE_Video );
	pMediaType->SetTemporalCompression( FALSE );
	pMediaType->SetSampleSize( bmih->biSizeImage );

	return NOERROR;
}

#define CALC_BI_STRIDE(width,bitcount)  ((((width * bitcount) + 31) & ~31) >> 3)
HRESULT CXvidCodec::ChangeColorspace(GUID subtype, GUID formattype, void * format)
{
	DWORD biWidth;

	if (formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER * )format;
		biWidth = vih->bmiHeader.biWidth;
		xvid_dec_frame.output.stride[0] = CALC_BI_STRIDE(vih->bmiHeader.biWidth, vih->bmiHeader.biBitCount);
		if (vih->bmiHeader.biHeight >= 0)  xvid_dec_frame.output.csp |= XVID_CSP_VFLIP;
	}
	else if (formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 * vih2 = (VIDEOINFOHEADER2 * )format;
		biWidth = vih2->bmiHeader.biWidth;
		xvid_dec_frame.output.stride[0] = CALC_BI_STRIDE(vih2->bmiHeader.biWidth, vih2->bmiHeader.biBitCount);
		if (vih2->bmiHeader.biHeight >= 0)  xvid_dec_frame.output.csp |= XVID_CSP_VFLIP;
	}
	else
	{
		return S_FALSE;
	}

	if (subtype == MEDIASUBTYPE_YUY2)
	{
		xvid_dec_frame.output.csp = XVID_CSP_YUY2;
	}
	
	return S_OK;
}

HRESULT CXvidCodec::SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt)
{
	if (direction == PINDIR_OUTPUT)
	{
		return ChangeColorspace(*pmt->Subtype(), *pmt->FormatType(), pmt->Format());
	}
	
	return S_OK;
}

HRESULT CXvidCodec::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
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

HRESULT CXvidCodec::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	AM_MEDIA_TYPE * mtOut;
	pOut->GetMediaType(&mtOut);
	
	if ( mtOut && xvid_dec_handle )
	{
		HRESULT hr = ChangeColorspace(mtOut->subtype, mtOut->formattype, mtOut->pbFormat);
		DeleteMediaType(mtOut);

		if ( hr != S_OK ) return hr;
	}
	
	if ( xvid_dec_handle )
	{
		xvid_dec_frame.general = XVID_LOWDELAY;
		
		if (pIn->IsDiscontinuity() == S_OK)
			xvid_dec_frame.general |= XVID_DISCONTINUITY;
	}
	
	if ( xvid_enc_handle )
	{
		CompressFrame( pIn, pOut );
	}
	else
	if ( xvid_dec_handle && pIn->IsPreroll() )
	{
		DecompressFrame( pIn, pOut );
	}
	
	return S_OK;
}

HRESULT	CXvidCodec::CompressFrame(IMediaSample *pSource, IMediaSample *pDest)
{
	/* Bind output buffer */
	xvid_enc_frame.length = pDest->GetActualDataLength();
	pDest->GetPointer( (BYTE**)&xvid_enc_frame.bitstream );
	
	/* Bind input buffer */
	pSource->GetPointer( (BYTE**)&xvid_enc_frame.input.plane[0] );
	xvid_enc_frame.input.csp = XVID_CSP_BGR | XVID_CSP_VFLIP;
	xvid_enc_frame.input.stride[0] = QCIF_WIDTH * 3;
	
	/* Encode the frame */
	int nLength = xvid_encore(xvid_enc_handle, XVID_ENC_ENCODE, &xvid_enc_frame, NULL);
	
	pDest->SetActualDataLength( nLength );
	return S_OK;
}

HRESULT	CXvidCodec::DecompressFrame(IMediaSample *pSource, IMediaSample *pDest)
{
	xvid_dec_stats_t stats;

	ZeroMemory( &stats, sizeof(stats) );
	stats.version = XVID_VERSION;
	
	/* Input stream */
	xvid_dec_frame.length = pSource->GetActualDataLength();
	pSource->GetPointer( (BYTE**)&xvid_dec_frame.bitstream );

	/* Output frame structure */
	pDest->GetPointer( (BYTE**)&xvid_dec_frame.output.plane[0] );

repeat :
	int length = xvid_decore(xvid_dec_handle, XVID_DEC_DECODE, &xvid_dec_frame, &stats);
	
	if (stats.type != XVID_TYPE_NOTHING)
	{  /* dont attempt to set vmr aspect ratio if no frame was returned by decoder */
		// inspired by minolta! works for VMR 7 + 9
		IMediaSample2 *pOut2 = NULL;
		AM_SAMPLE2_PROPERTIES outProp2;
		if (SUCCEEDED(pDest->QueryInterface(IID_IMediaSample2, (void **)&pOut2)) &&
			SUCCEEDED(pOut2->GetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, tStart), (PBYTE)&outProp2)))
		{
			CMediaType mtOut2 = m_pOutput->CurrentMediaType();
			VIDEOINFOHEADER2* vihOut2 = (VIDEOINFOHEADER2*)mtOut2.Format();
			
			if (*mtOut2.FormatType() == FORMAT_VideoInfo2 && 
				vihOut2->dwPictAspectRatioX != ar_x && vihOut2->dwPictAspectRatioY != ar_y)
			{
				vihOut2->dwPictAspectRatioX = ar_x;
				vihOut2->dwPictAspectRatioY = ar_y;
				pOut2->SetMediaType(&mtOut2);
				m_pOutput->SetMediaType(&mtOut2);
			}
			pOut2->Release();
		}
	}
	
	if (stats.type == XVID_TYPE_VOL)
	{
		pDest->SetSyncPoint(TRUE);

		{ /* auto */
			int par_x, par_y;
			if (stats.data.vol.par == XVID_PAR_EXT) {
				par_x = stats.data.vol.par_width;
				par_y = stats.data.vol.par_height;
			} else {
				par_x = PARS[stats.data.vol.par-1][0];
				par_y = PARS[stats.data.vol.par-1][1];
			}

			ar_x = par_x * stats.data.vol.width;
			ar_y = par_y * stats.data.vol.height;
		}

		xvid_dec_frame.bitstream = (BYTE*)xvid_dec_frame.bitstream + length;
		xvid_dec_frame.length -= length;
		goto repeat;
	}

	if (pSource->IsPreroll() == S_OK) {
		return S_FALSE;
	}
	
	return S_OK;
}

void CXvidCodec::DisplayMediaType(TCHAR *pDescription,const CMediaType *pmt) 
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
