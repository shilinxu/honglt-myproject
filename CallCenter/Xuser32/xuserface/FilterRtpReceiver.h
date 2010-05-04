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

class CRtpOutputPin;
struct RTPPayload;
class CDatagrams;

class CRtpReceiverFilter : public CBaseFilter, protected CAMThread
{
// Construction
public:
	CRtpReceiverFilter(WAVEFORMATEX* pwfx, CFilterGraph* pFilterGraph, DWORD nReserved);
	CRtpReceiverFilter(VIDEOINFOHEADER* pvih, CFilterGraph* pFilterGraph, DWORD nReserved);
	virtual ~CRtpReceiverFilter();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:
    CCritSec        m_Lock;
	CFilterGraph*	m_pFilterGraph;
	CDatagrams*		m_pDatagrams;
protected:
	CRtpOutputPin*	m_pCapture;
	CRtpOutputPin*	m_pPreview;
	volatile DWORD	m_nRefStart;
	volatile DWORD	m_nRefStream;
private:
	int				m_nCookie;
	
// Operations
protected:
	virtual HRESULT DeliverBuffer(BYTE* pBuffer, DWORD nLength, DWORD nTimestamp);
	virtual HRESULT RenderBuffer(BYTE* pByte, DWORD nLength, REFERENCE_TIME rtStart);
	static	HRESULT CALLBACK RenderBuffer(BYTE* pBuffer, DWORD nBuffer, BYTE /*nType*/, DWORD nTimestamp, LPVOID lv);
protected:
	virtual int		GetPinCount();
	virtual CBasePin*	GetPin(int n);
	virtual BYTE	GetPayloadType() const;
	CFilterGraph*	GetFilterGraph() const;
protected:
	enum ThreadMsg { TM_Pause, TM_Start, TM_Exit };
	
	virtual DWORD	ThreadProc(void);
	virtual HRESULT	StartThread();
	virtual HRESULT	OnRun();
	STDMETHODIMP	Stop();
	
	friend class CRtpOutputPin;
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
	virtual HRESULT Active(void);
	virtual HRESULT BreakConnect(void);
	virtual HRESULT CompleteConnect(IPin *pReceivePin);
	virtual HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* pProperties);
protected:
	virtual MMRESULT InitializeCodec(LPCWSTR Id, WAVEFORMATEX* pwfx);
	virtual DWORD	InitializeCodec(LPCWSTR Id, VIDEOINFOHEADER* pvih);
};

#endif // !defined(AFX_FILTERRTPRECEIVER_H__FDA52734_4919_4108_9ABD_B653EDAC118F__INCLUDED_)
