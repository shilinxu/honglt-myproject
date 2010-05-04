//
// H263Codec.h
//

#if !defined(AFX_H263Codec_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_)
#define AFX_H263Codec_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Encoder/libr263.h"
#include "Decoder/tmndec.h"

#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144

class CH263Output;

class CH263Codec : public CTransformFilter
{
// Construction
public:
	CH263Codec(LPUNKNOWN pUnk, HRESULT* phr);
	virtual ~CH263Codec();
	
//	DECLARE_IUNKNOWN
	
// Attributes
protected:
	struct compression_parameters	m_pParams;
	unsigned int yuvBuffer[ QCIF_WIDTH*QCIF_HEIGHT  + (QCIF_WIDTH*QCIF_HEIGHT)/2 ];

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
//	virtual HRESULT StopStreaming();
    virtual HRESULT Copy(IMediaSample *pSource, IMediaSample *pDest) const;
protected:
	virtual HRESULT	CompressFrame(IMediaSample *pSource, IMediaSample *pDest);
	virtual	HRESULT	DecompressFrame(IMediaSample *pSource, IMediaSample *pDest);
	void			DisplayMediaType(TCHAR *pDescription, const CMediaType *pmt) ;
//STDMETHODIMP Run(REFERENCE_TIME tStart);
};

class CH263Output : public CTransformOutputPin
{
// Construction
public:
	CH263Output(CTransformFilter* pFilter, HRESULT* phr);
	virtual ~CH263Output();
	
// Operations
protected:
//	STDMETHODIMP	EnumMediaTypes(IEnumMediaTypes **ppEnum);
	virtual HRESULT	CheckMediaType(const CMediaType *pMediaType);
};

// {B7DE6897-0E3D-480b-97C3-4AC777FAA0D1}
DEFINE_GUID(CLSID_H263Codec, 
0xb7de6897, 0xe3d, 0x480b, 0x97, 0xc3, 0x4a, 0xc7, 0x77, 0xfa, 0xa0, 0xd1);

// {33363248-0000-0010-8000-00AA00389B71}
DEFINE_GUID(MEDIASUBTYPE_H263, 
0x33363248, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

extern const AMOVIESETUP_FILTER sudCodec;
extern const AMOVIESETUP_PIN psudPins[];

#endif // !defined(AFX_H263Codec_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_)
