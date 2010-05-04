//
// RecordGraph.h
//

#if !defined(AFX_RECORDGRAPH_H__4B4FA47D_9351_4E97_95E5_AD75B6B37085__INCLUDED_)
#define AFX_RECORDGRAPH_H__4B4FA47D_9351_4E97_95E5_AD75B6B37085__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterGraph.h"
class CBuffer;

class CGraphRecord : public CFilterGraph  
{
// Construction
public:
	CGraphRecord(BOOL bInit = FALSE);
	virtual ~CGraphRecord();
	
// Attributes
protected:
	
// Operations
public:
	virtual HRESULT	RecordFile(LPCWSTR lpszFile);
public:
	virtual HRESULT	DeliverBuffer(BYTE nPayload, CBuffer* pBuffer);
	virtual HRESULT DeliverBuffer(BYTE nPayload, BYTE* pByte, DWORD nLength, DWORD nTimestamp);
protected:
	HRESULT			RenderAudio(IBaseFilter** ppCapture);
	HRESULT			RenderVideo(IBaseFilter** ppCapture);
};

#endif // !defined(AFX_RECORDGRAPH_H__4B4FA47D_9351_4E97_95E5_AD75B6B37085__INCLUDED_)
