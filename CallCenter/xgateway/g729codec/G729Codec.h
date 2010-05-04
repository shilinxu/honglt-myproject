//
// G729Codec.h
//

#if !defined(AFX_G729CODEC_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_)
#define AFX_G729CODEC_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CG729Output;

class CG729Codec : public CTransformFilter
{
// Construction
public:
	CG729Codec(LPUNKNOWN pUnk, HRESULT* phr);
	virtual ~CG729Codec();
	
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
	virtual	HRESULT	DecodeMedia(IMediaSample *pSource, IMediaSample *pDest) const;
	void			DisplayMediaType(TCHAR *pDescription, const CMediaType *pmt) ;
};

class CG729Output : public CTransformOutputPin
{
// Construction
public:
	CG729Output(CTransformFilter* pFilter, HRESULT* phr);
	virtual ~CG729Output();
	
// Operations
protected:
//	STDMETHODIMP	EnumMediaTypes(IEnumMediaTypes **ppEnum);
	virtual HRESULT	CheckMediaType(const CMediaType *pMediaType);
};

// {F7A72611-FF7B-4532-91F8-993DDEA3DDCF}
DEFINE_GUID(CLSID_G729ACodec, 
0xf7a72611, 0xff7b, 0x4532, 0x91, 0xf8, 0x99, 0x3d, 0xde, 0xa3, 0xdd, 0xcf);

// {00000083-0000-0010-8000-00AA00389B71}
DEFINE_GUID(MEDIASUBTYPE_G729AAudio, 
0x00000083, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

#define	WAVE_FORMAT_G729A	0x0083	/* AT&T */

extern const AMOVIESETUP_FILTER sudCodec;
extern const AMOVIESETUP_PIN psudPins[];

#endif // !defined(AFX_G729CODEC_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_)
