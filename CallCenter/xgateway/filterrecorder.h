//
// FilterRecorder.h
//

#if !defined(AFX_FILTERRECORDER_H__3F2354F5_34C5_4B67_8350_7E179EE29848__INCLUDED_)
#define AFX_FILTERRECORDER_H__3F2354F5_34C5_4B67_8350_7E179EE29848__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum Payload
{
	Payload_Audio	= 'A',
	Payload_Video	= 'V',
};

#include "FilterGraph.h"
class CAudioRenderer;
class CVideoRenderer;
class CBuffer;

class CFilterRecorder : public CFilterGraph  
{
// Construction
public:
	CFilterRecorder();
	virtual ~CFilterRecorder();
	
// Attributes
public:
	CAudioRenderer*	m_pAudioRender;
	CVideoRenderer*	m_pVideoRender;
protected:
	CBuffer*		m_pBuffer;
	DWORD			m_nTimestamp;

// Operations
public:
	virtual HRESULT	RecordFile(LPCWSTR lpszFile);
	virtual	HRESULT	ConnectTo(const RECT& rect, HWND hWnd);
public:
	virtual HRESULT	DeliverBuffer(BYTE nPayload, CBuffer* pBuffer);
protected:
	HRESULT			RenderAudio(IBaseFilter** ppCapture);
	HRESULT			RenderVideo(IBaseFilter** ppCapture);
};

#endif // !defined(AFX_FILTERRECORDER_H__3F2354F5_34C5_4B67_8350_7E179EE29848__INCLUDED_)
