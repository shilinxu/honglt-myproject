//
// FilterRtpSender.h
//

#if !defined(AFX_FILTERRTPSENDER_H__7B964277_A0A0_47D0_BEA5_427BDE2F5B23__INCLUDED_)
#define AFX_FILTERRTPSENDER_H__7B964277_A0A0_47D0_BEA5_427BDE2F5B23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmreg.h>
#include <msacm.h>
class CRtpInputPin;
class CDatagrams;

class CRtpSenderFilter : public CBaseFilter
{
// Construction
public:
	CRtpSenderFilter(WAVEFORMATEX* pwfx, CFilterGraph* pFilterGraph, DWORD nReserved);
	CRtpSenderFilter(VIDEOINFOHEADER* pvih, CFilterGraph* pFilterGraph, DWORD nReserved);
	virtual ~CRtpSenderFilter();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:
    CCritSec        m_Lock;
    CCritSec        m_ReceiveLock;
	CFilterGraph*	m_pFilterGraph;
	CDatagrams*		m_pDatagrams;
protected:
	CRtpInputPin*	m_pCapture;
	const	 float	m_nRefRender;
	volatile DWORD	m_nRefStream;
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
			COMPVARS	m_cv;
			BITMAPINFO	m_bmiInput;
		};
	};
	
	BYTE*	m_pBuffer;
	
// Operations
protected:
	virtual HRESULT Deliver(IMediaSample *pMediaSample);
	virtual HRESULT	Deliver(BYTE* pBuffer, DWORD nLength);
	virtual HRESULT	Deliver(BYTE* pBuffer, DWORD nLength, DWORD nReserved);
protected:
	virtual int		GetPinCount();
	virtual CBasePin*	GetPin(int n);
	virtual BYTE	GetPayloadType() const;
	CFilterGraph*	GetFilterGraph() const;
	STDMETHODIMP	Run(REFERENCE_TIME tStart);
protected:
	virtual	MMRESULT InitializeCodec(WAVEFORMATEX* pwfx);
	virtual BOOL	InitializeCodec(VIDEOINFOHEADER* pvih);
	virtual void	DestroyCodec(WAVEFORMATEX* pwfx);
	virtual void	DestroyCodec(VIDEOINFOHEADER* pvih);
private:
	int				SendPayload(BYTE* pBuffer, DWORD nBuffer, BYTE nType, DWORD nTimestampinc);
	
	friend class	CRtpInputPin;
};

class CRtpInputPin : public CRenderedInputPin
{
// Construction
public:
	CRtpInputPin(CRtpSenderFilter* pFilter, CCritSec* pLock, HRESULT* phr);
	~CRtpInputPin();
	
// Attributes
protected:
	CRtpSenderFilter*	m_pRtpFilter;
    CCritSec* const		m_pReceiveLock;
	
// Operations
public:
	void			DisplayMediaType(TCHAR *pDescription, const CMediaType *pmt) ;
	virtual HRESULT	SetMediaType(VIDEOINFOHEADER* pvih);
	virtual HRESULT	SetMediaType(WAVEFORMATEX* pwfx);
protected:
	virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType) ;
	virtual HRESULT	CheckMediaType(const CMediaType *pMediaType);
protected:
	virtual HRESULT CompleteConnect(IPin *pReceivePin);
	virtual HRESULT BreakConnect(void);
protected:
	STDMETHODIMP	Receive(IMediaSample *pMediaSample);
	STDMETHODIMP	EndOfStream(void);
	STDMETHODIMP	BeginFlush(void);
	STDMETHODIMP	EndFlush(void);
	STDMETHODIMP	ReceiveCanBlock(void);
};

#endif // !defined(AFX_FILTERRTPSENDER_H__7B964277_A0A0_47D0_BEA5_427BDE2F5B23__INCLUDED_)
