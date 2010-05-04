//
// FilterMediaReceiver.h:
//

#if !defined(AFX_FILTERMEDIARECEIVER_H__53576C13_346E_404C_B3D6_FEEB91EF4E26__INCLUDED_)
#define AFX_FILTERMEDIARECEIVER_H__53576C13_346E_404C_B3D6_FEEB91EF4E26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMediaReceiverFilter : public CBaseFilter
{
// Construction
public:
	CMediaReceiverFilter(LPUNKNOWN pUnk, HRESULT *phr);
	virtual ~CMediaReceiverFilter();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:
    CCritSec        m_Lock;                // Main renderer critical section
	CBasePin*		m_pOutput;

// Operations
protected:
	virtual int			GetPinCount();
	virtual CBasePin*	GetPin(int n);
};

class CMediaOutputPin : public CRenderedInputPin
{
// Construction
public:
	CMediaOutputPin(CMediaReceiverFilter* pFilter, LPUNKNOWN pUnk,
					CCritSec* pLock, CCritSec *pReceiveLock, HRESULT *phr);
	~CMediaOutputPin();
	
// Attributes
protected:
	CMediaOutputPin*	m_pMediaFilter;
    CCritSec* const		m_pReceiveLock;    // Sample critical section
	
// Operations
protected:
};

#endif // !defined(AFX_FILTERMEDIARECEIVER_H__53576C13_346E_404C_B3D6_FEEB91EF4E26__INCLUDED_)
