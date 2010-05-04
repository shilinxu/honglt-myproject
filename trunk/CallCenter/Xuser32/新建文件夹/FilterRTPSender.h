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
	CRtpInputPin*	m_pInput[2];
	friend class	CRtpInputPin;
	
// Operations
protected:
	virtual int			GetPinCount();
	virtual CBasePin*	GetPin(int n);
protected:
};

class CRtpInputPin : public CRenderedInputPin
{
// Construction
public:
	CRtpInputPin(CRtpSenderFilter* pFilter, CCritSec* pLock,
				HRESULT* phr, LPCWSTR pName);
	~CRtpInputPin();
	
// Attributes
protected:
	CRtpSenderFilter*	m_pRtpFilter;
    CCritSec* const		m_pReceiveLock;
	
// Operations
protected:
	virtual HRESULT	CheckMediaType(const CMediaType *pMediaType);
	STDMETHODIMP	Receive(IMediaSample *pMediaSample);
protected:
	virtual int		SendPacket(byte* data, size_t len);

#ifdef _DEBUG
	void		DisplayMediaType(TCHAR *pDescription, const CMediaType *pmt) ;
#endif
};

#endif // !defined(AFX_FILTERRTPSENDER_H__7B964277_A0A0_47D0_BEA5_427BDE2F5B23__INCLUDED_)
