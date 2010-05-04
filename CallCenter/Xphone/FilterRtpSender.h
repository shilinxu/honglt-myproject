//
// FilterRtpSender.h
//

#if !defined(AFX_FILTERRTPSENDER_H__7B964277_A0A0_47D0_BEA5_427BDE2F5B23__INCLUDED_)
#define AFX_FILTERRTPSENDER_H__7B964277_A0A0_47D0_BEA5_427BDE2F5B23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRtpInputPin;

class CRtpSenderFilter : public CBaseFilter
{
// Construction
public:
	CRtpSenderFilter(LPUNKNOWN pUnk, HRESULT *phr);
	virtual ~CRtpSenderFilter();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:
    CCritSec        m_Lock;
    CCritSec        m_ReceiveLock;
protected:
	CRtpInputPin*	m_pInput;
	friend class	CRtpInputPin;
	
// Operations
public:
	virtual HRESULT		SetupMediaType(WAVEFORMATEX* pwfx);
	virtual HRESULT		SetupMediaType(VIDEOINFOHEADER* pvh);
protected:
	virtual int			GetPinCount();
	virtual CBasePin*	GetPin(int n);
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
	friend class	CRtpSenderFilter;
	
// Operations
public:
	void			DisplayMediaType(TCHAR *pDescription, const CMediaType *pmt) ;
	BYTE			GetPayloadType() const;
protected:
	virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType) ;
	virtual HRESULT	CheckMediaType(const CMediaType *pMediaType);
	virtual int		SendPacketSplitter(BYTE* pBuffer, DWORD nBuffer);
protected:
	STDMETHODIMP	Receive(IMediaSample *pMediaSample);
	STDMETHODIMP	EndOfStream(void);
	STDMETHODIMP	BeginFlush(void);
	STDMETHODIMP	EndFlush(void);
	STDMETHODIMP	ReceiveCanBlock(void);

};

#endif // !defined(AFX_FILTERRTPSENDER_H__7B964277_A0A0_47D0_BEA5_427BDE2F5B23__INCLUDED_)
