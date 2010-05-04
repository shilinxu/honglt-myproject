//
// FilterMediaReceiver.cpp
//

#include "stdafx.h"
#include "Xgateway.h"

#include "FilterMediaReceiver.h"
#include <initguid.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CMediaReceiverFilter construction

CMediaReceiverFilter::CMediaReceiverFilter(LPUNKNOWN pUnk, HRESULT *phr) :
	CBaseFilter(NAME("Media Receiver Filter"), pUnk, &m_Lock, CLSID_NULL)
{
}

CMediaReceiverFilter::~CMediaReceiverFilter()
{
}

//////////////////////////////////////////////////////////////////////
// CMediaReceiverFilter operations

int CMediaReceiverFilter::GetPinCount()
{
    return 1;
}

CBasePin* CMediaReceiverFilter::GetPin(int n)
{
	ASSERT( n == 0 );
	if ( n != 0 ) return NULL;
	
	return m_pOutput;
}

//////////////////////////////////////////////////////////////////////
// CMediaOutputPin construction

CMediaOutputPin::CMediaOutputPin(CMediaReceiverFilter* pFilter, LPUNKNOWN pUnk,
								 CCritSec* pLock, CCritSec *pReceiveLock, HRESULT *phr)
								:CRenderedInputPin(NAME("Media Output Pin"), pFilter, pLock, phr, L"Output")
{
}

CMediaOutputPin::~CMediaOutputPin()
{
}
