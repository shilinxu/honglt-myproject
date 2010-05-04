//
// FilterG729Compressor.h
//

#if !defined(AFX_FILTERG729COMPRESSOR_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_)
#define AFX_FILTERG729COMPRESSOR_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CG729OutputPin;

class CG729CompressorFilter : public CTransformFilter
{
// Construction
public:
	CG729CompressorFilter(LPUNKNOWN pUnk = NULL, HRESULT *phr = NULL);
	virtual ~CG729CompressorFilter();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:

// Operations
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

class CG729OutputPin : public CTransformOutputPin
{
// Construction
public:
	CG729OutputPin(CTransformFilter* pFilter, HRESULT* phr);
	virtual ~CG729OutputPin();
	
// Operations
protected:
	STDMETHODIMP	EnumMediaTypes( IEnumMediaTypes **ppEnum );
	virtual HRESULT	CheckMediaType(const CMediaType *pMediaType);
};

#endif // !defined(AFX_FILTERG729COMPRESSOR_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_)
