//
// FilterG729Encoder.h
//

#if !defined(AFX_FILTERG729ENCODER_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_)
#define AFX_FILTERG729ENCODER_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CG729EncoderOutput;

class CG729EncoderFilter : public CTransformFilter
{
// Construction
public:
	CG729EncoderFilter(LPUNKNOWN pUnk, HRESULT* phr);
	virtual ~CG729EncoderFilter();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:

// Operations
public:
	static	
	CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT* phr);
protected:
	virtual HRESULT Transform(IMediaSample* pIn, IMediaSample* pOut);
	virtual HRESULT Receive(IMediaSample* pSample);
	
	virtual HRESULT CheckInputType(const CMediaType* mtIn);
	virtual HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut);
	
	virtual HRESULT GetMediaType(int iPosition, CMediaType* pMediaType) ;

	virtual HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* pProperties);
protected:
    virtual HRESULT StartStreaming();
    virtual HRESULT Copy(IMediaSample *pSource, IMediaSample *pDest) const;
protected:
	virtual HRESULT	EncodeMedia(IMediaSample *pSource, IMediaSample *pDest) const;
};

class CG729EncoderOutput : public CTransformOutputPin
{
// Construction
public:
	CG729EncoderOutput(CTransformFilter* pFilter, HRESULT* phr);
	virtual ~CG729EncoderOutput();
	
// Operations
protected:
	STDMETHODIMP	EnumMediaTypes( IEnumMediaTypes **ppEnum );
	virtual HRESULT	CheckMediaType(const CMediaType *pMediaType);
};

// {F7A72611-FF7B-4532-91F8-993DDEA3DDCF}
DEFINE_GUID(CLSID_G729Encoder, 
0xf7a72611, 0xff7b, 0x4532, 0x91, 0xf8, 0x99, 0x3d, 0xde, 0xa3, 0xdd, 0xcf);

extern const AMOVIESETUP_FILTER sudEncoder;

#endif // !defined(AFX_FILTERG729ENCODER_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_)
