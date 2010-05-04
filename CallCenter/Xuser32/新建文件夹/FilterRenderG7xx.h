//
// FilterRenderG7xx.h
//

#if !defined(AFX_FILTERRENDERG7XX_H__05D2C758_9C37_4825_8FA8_74FEE1A6BD31__INCLUDED_)
#define AFX_FILTERRENDERG7XX_H__05D2C758_9C37_4825_8FA8_74FEE1A6BD31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CG7xxInputPin;

class CG7xxRenderFilter : public CBaseFilter
{
// Construction
public:
	CG7xxRenderFilter(LPUNKNOWN pUnk, HRESULT *phr);
	virtual ~CG7xxRenderFilter();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:
    CCritSec        m_Lock;                // Main renderer critical section
    CCritSec        m_ReceiveLock;         // Sublock for received samples
private:
    friend class CG7xxInputPin;
	CG7xxInputPin*	m_pPinInput;
	
// Operations
protected:
	virtual int		GetPinCount();
	virtual CBasePin* GetPin(int n);
};

class CG7xxInputPin : public CRenderedInputPin
{
// Construction
public:
	CG7xxInputPin(  CG7xxRenderFilter* pG7xxFilter, LPUNKNOWN pUnk,
					CCritSec* pLock, CCritSec *pReceiveLock, HRESULT *phr);
    ~CG7xxInputPin();
	
// Attributes
protected:
	CG7xxRenderFilter*	m_pG7xxFilter;
    CCritSec* const		m_pReceiveLock;    // Sample critical section
	
// Operations
protected:
    STDMETHODIMP Receive(IMediaSample *pSample);
    STDMETHODIMP EndOfStream(void);
//	virtual HRESULT BeginFlush(void);
	STDMETHODIMP	EndFlush(void);
	virtual HRESULT CheckMediaType(const CMediaType *pmt);
};

#endif // !defined(AFX_FILTERRENDERG7XX_H__05D2C758_9C37_4825_8FA8_74FEE1A6BD31__INCLUDED_)
