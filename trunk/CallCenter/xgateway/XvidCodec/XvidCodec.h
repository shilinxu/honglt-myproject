//
// XvidCodec.h
//

#if !defined(AFX_XvidCodec_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_)
#define AFX_XvidCodec_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "xvid.h"

#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144

class CXvidCodec : public CVideoTransformFilter
{
// Construction
public:
	CXvidCodec(LPUNKNOWN pUnk, HRESULT* phr);
	virtual ~CXvidCodec();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:
	LPVOID		xvid_enc_handle;
	LPVOID		xvid_dec_handle;
private:
	xvid_enc_frame_t xvid_enc_frame;
	xvid_dec_frame_t xvid_dec_frame;
	DWORD		ar_x, ar_y;

// Operations
public:
	static	
	CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT* phr);
protected:
	virtual HRESULT Transform(IMediaSample* pIn, IMediaSample* pOut);
	HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt);
	
	virtual HRESULT CheckInputType(const CMediaType* mtIn);
	virtual HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut);
	
	virtual HRESULT GetMediaType(int iPosition, CMediaType* pMediaType) ;
	virtual HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* pProperties);
protected:
	virtual HRESULT	CompressFrame(IMediaSample *pSource, IMediaSample *pDest);
	virtual	HRESULT	DecompressFrame(IMediaSample *pSource, IMediaSample *pDest);
	void			DisplayMediaType(TCHAR *pDescription, const CMediaType *pmt) ;
protected:
	HRESULT			XvidInitEncode(void*& xvid_handle);
	HRESULT			XvidInitDecode(void*& xvid_handle);
	HRESULT			ChangeColorspace(GUID subtype, GUID formattype, void * format);
};

DEFINE_GUID(CLSID_XvidCodec, mmioFOURCC('X','V','I','D'), 
0xa800, 0x4d8f, 0xb8, 0x76, 0xc0, 0x37, 0xaa, 0x39, 0x64, 0x17);

// {34363248-0000-0010-8000-00AA00389B71}
DEFINE_GUID(MEDIASUBTYPE_XVID, mmioFOURCC('X','V','I','D'), 
0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

extern const AMOVIESETUP_FILTER sudCodec;
extern const AMOVIESETUP_PIN psudPins[];

#endif // !defined(AFX_XvidCodec_H__DE821325_6985_4FA3_8B64_005183A320DF__INCLUDED_)
