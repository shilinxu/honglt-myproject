//
// FilterMediaSender.h
//

#if !defined(AFX_FILTERMEDIASENDER_H__05D2C758_9C37_4825_8FA8_74FEE1A6BD31__INCLUDED_)
#define AFX_FILTERMEDIASENDER_H__05D2C758_9C37_4825_8FA8_74FEE1A6BD31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMediaInputPin;

class CMediaSenderFilter : public CBaseFilter
{
// Construction
public:
	CMediaSenderFilter(LPUNKNOWN pUnk, HRESULT *phr);
	virtual ~CMediaSenderFilter();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:
    CCritSec        m_Lock;                // Main renderer critical section
    CCritSec        m_ReceiveLock;         // Sublock for received samples
private:
    friend class CMediaInputPin;
	CMediaInputPin*	m_pPinInput;
	
// Operations
protected:
	virtual int			GetPinCount();
	virtual CBasePin*	GetPin(int n);
};

class CMediaInputPin : public CRenderedInputPin
{
// Construction
public:
	CMediaInputPin(  CMediaSenderFilter* pG7xxFilter, LPUNKNOWN pUnk,
					CCritSec* pLock, CCritSec *pReceiveLock, HRESULT *phr);
	~CMediaInputPin();
	
// Attributes
protected:
	CMediaSenderFilter*	m_pMediaFilter;
    CCritSec* const		m_pReceiveLock;    // Sample critical section
	
// Operations
protected:
	STDMETHODIMP	EndOfStream(void);
	STDMETHODIMP	Receive(IMediaSample *pMediaSample);
	HRESULT			CheckMediaType(const CMediaType *pMediaType);
protected:
	virtual int		SendPacket(byte* data, size_t len);
};

#endif // !defined(AFX_FILTERMEDIASENDER_H__05D2C758_9C37_4825_8FA8_74FEE1A6BD31__INCLUDED_)
