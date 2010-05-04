//
// FilterRtpReceiver.h
//

#if !defined(AFX_FILTERRTPRECEIVER_H__FDA52734_4919_4108_9ABD_B653EDAC118F__INCLUDED_)
#define AFX_FILTERRTPRECEIVER_H__FDA52734_4919_4108_9ABD_B653EDAC118F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRtpOutputPin;

class CRtpReceiverFilter : public CBaseFilter
{
// Construction
public:
	CRtpReceiverFilter(LPUNKNOWN pUnk, HRESULT *phr);
	virtual ~CRtpReceiverFilter();
	
	DECLARE_IUNKNOWN
	
// Attributes
protected:
    CCritSec        m_Lock;
protected:
	CRtpOutputPin*	m_pOutput;
	
// Operations
protected:
	virtual int			GetPinCount();
	virtual CBasePin*	GetPin(int n);
};

class CRtpOutputPin : public CBaseOutputPin
{
// Construction
public:
	CRtpOutputPin(CRtpReceiverFilter* pFilter, CCritSec* pLock, HRESULT *phr);
	~CRtpOutputPin();
	
// Operations
protected:
	virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType) ;
	virtual HRESULT	CheckMediaType(const CMediaType *pMediaType);
	virtual HRESULT DecideBufferSize(
					IMemAllocator* pMemAllocator,
					ALLOCATOR_PROPERTIES* ppropInputRequest);
        virtual
        HRESULT
        InitAllocator (
            OUT IMemAllocator ** ppAlloc
            ) ;
		virtual HRESULT Active();
};

#endif // !defined(AFX_FILTERRTPRECEIVER_H__FDA52734_4919_4108_9ABD_B653EDAC118F__INCLUDED_)
