//
// FilterG729Decoder.h
//

#if !defined(AFX_FILTERG729DECODER_H__0F8094EB_1F7E_4C9D_A753_4DB3C40BB082__INCLUDED_)
#define AFX_FILTERG729DECODER_H__0F8094EB_1F7E_4C9D_A753_4DB3C40BB082__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CG729DecoderOutput;

class CG729DecoderFilter : public CTransformFilter
{
// Construction
public:
	CG729DecoderFilter(LPUNKNOWN pUnk, HRESULT* phr);
	virtual ~CG729DecoderFilter();
	
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
	virtual HRESULT	DecodeMedia(IMediaSample *pSource, IMediaSample *pDest) const;
};

class CG729DecoderOutput : public CTransformOutputPin
{
// Construction
public:
	CG729DecoderOutput(CTransformFilter* pFilter, HRESULT* phr);
	virtual ~CG729DecoderOutput();
	
// Operations
protected:
	STDMETHODIMP	EnumMediaTypes( IEnumMediaTypes **ppEnum );
	virtual HRESULT	CheckMediaType(const CMediaType *pMediaType);
};

// {3451CA02-B740-4d50-A2EB-24FDEB6351F4}
DEFINE_GUID(CLSID_G729Decoder, 
0x3451ca02, 0xb740, 0x4d50, 0xa2, 0xeb, 0x24, 0xfd, 0xeb, 0x63, 0x51, 0xf4);

extern const AMOVIESETUP_FILTER sudDecoder;

#endif // !defined(AFX_FILTERG729DECODER_H__0F8094EB_1F7E_4C9D_A753_4DB3C40BB082__INCLUDED_)
