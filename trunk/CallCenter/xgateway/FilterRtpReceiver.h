//
// FilterRtpReceiver.h
//

#if !defined(AFX_FILTERRTPRECEIVER_H__FDA52734_4919_4108_9ABD_B653EDAC118F__INCLUDED_)
#define AFX_FILTERRTPRECEIVER_H__FDA52734_4919_4108_9ABD_B653EDAC118F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmreg.h>
#include <msacm.h>
#include "FilterGraph.h"

class CBuffer;
class CRtpOutputPin;
class CDatagrams;

class CRtpReceiverFilter : public CBaseFilter
{
// Construction
public:
	CRtpReceiverFilter(WAVEFORMATEX* pwfx, CFilterGraph* pFilterGraph, LPVOID lv);
	CRtpReceiverFilter(VIDEOINFOHEADER* pvih, CFilterGraph* pFilterGraph, LPVOID lv);
	virtual ~CRtpReceiverFilter();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:
    CCritSec        m_Lock;
	CFilterGraph*	m_pFilterGraph;
	CDatagrams*		m_pDatagram;
protected:
	CRtpOutputPin*	m_pCapture;
	CRtpOutputPin*	m_pPreview;
	volatile DWORD	m_nRefStream;
	
// Operations
protected:
	virtual HRESULT	RenderBuffer(BYTE* pByte, DWORD nLength);
	virtual HRESULT RenderBuffer(BYTE* pByte, DWORD nLength, REFERENCE_TIME rtStart);
	virtual HRESULT DeliverBuffer(BYTE* pBuffer, DWORD nLength, LONG nTimestamp);
	static	HRESULT CALLBACK RenderBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE /*nType*/, DWORD nTimestamp, LPVOID lv);
protected:
	virtual int		GetPinCount();
	virtual CBasePin*	GetPin(int n);
	CFilterGraph*	GetFilterGraph() const;
	virtual Payload	GetPayloadType() const;
protected:
	STDMETHODIMP	Stop(void);
	
	friend class CRtpOutputPin;
	friend class CGraphRender;
};

class CRtpOutputPin : public CBaseOutputPin
{
// Construction
public:
	CRtpOutputPin(CRtpReceiverFilter* pFilter, CCritSec* pLock, HRESULT *phr, LPCWSTR lpszName);
	~CRtpOutputPin();
	
// Attributes
protected:
	union
	{
		struct // ACM
		{
			HACMSTREAM	HACMStream;
			ACMSTREAMHEADER ACMStreamHeader;
		};
		struct // H263
		{
			HIC	m_hIC;
			BITMAPINFOHEADER m_bmiInput;
			BITMAPINFOHEADER m_bmiOutput;
		};
	};
	
	BYTE*	m_pBuffer;
	
// Operations
public:
	virtual HRESULT	SetMediaType(WAVEFORMATEX* pwfx);
	virtual HRESULT	SetMediaType(VIDEOINFOHEADER* pvih);
protected:
	virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType) ;
	virtual HRESULT	CheckMediaType(const CMediaType *pMediaType);
	virtual HRESULT Deliver(IMediaSample *pMediaSample);
protected:
	virtual HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* pProperties);
	virtual HRESULT CompleteConnect(IPin *pReceivePin);
	virtual HRESULT BreakConnect(void);
//	virtual HRESULT Active(void);
protected:
	virtual MMRESULT InitializeCodec(LPCWSTR Id, WAVEFORMATEX* pwfx);
	virtual DWORD	InitializeCodec(LPCWSTR Id, VIDEOINFOHEADER* pvih);
//	STDMETHODIMP	Notify(IBaseFilter * pSelf, Quality q);
};

#endif // !defined(AFX_FILTERRTPRECEIVER_H__FDA52734_4919_4108_9ABD_B653EDAC118F__INCLUDED_)
