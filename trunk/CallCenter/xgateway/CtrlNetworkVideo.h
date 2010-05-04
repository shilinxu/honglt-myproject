//
// CtrlNetworkVideo.h
//

#if !defined(AFX_CTRLNETWORKVIDEO_H__31BD374E_A689_4334_A2A2_ED3A66B29AC3__INCLUDED_)
#define AFX_CTRLNETWORKVIDEO_H__31BD374E_A689_4334_A2A2_ED3A66B29AC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "ChannelUser.h"
#include "RenderGraph.h"

#include <mmreg.h>
#include <msacm.h>

class CNetworkChild;

class CNetworkVideo : public CTaskBox, public CUserChannel, public CGraphRender
{
// Construction
public:
	CNetworkVideo(CNetworkChild* pPanel, LPCTSTR xUserId);
	virtual ~CNetworkVideo();
	
	DECLARE_DYNAMIC(CNetworkVideo)

// Attributes
protected:
	CNetworkChild*	m_pPanel;
	BOOL			m_bRecording;	// full record
protected:
	CBuffer*		m_pBuffer;		// mix buffer
	BYTE*			m_pMixedPtr;
	DWORD			m_nDeliverId;
	HACMSTREAM		HACMPcm;
	HACMSTREAM		HACMStream;

// Operations
protected:
	virtual void	Disconnect();
	CNetworkChild*	GetPanel() const;
protected:
	virtual BOOL	JoinedMirror(CNetworkVideo* pMirror);
	virtual BOOL	LeavedMirror(CNetworkVideo* pMirror);
protected:
	virtual BOOL	OnSyncMonitorView(CXMLElement* pXML);
	virtual BOOL	OnSyncHostRecording(CXMLElement* pXML);
//	virtual BOOL	OnSyncHostTalking(CXMLElement* pXML);
	virtual HRESULT	OnRecordFile(WORD nPayloadMask, LPCTSTR fMask);
protected:
	virtual HRESULT DeliverBuffer(CBuffer* pBuffer, BYTE*& pMixedPtr);
	virtual HRESULT	DeliverBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType, DWORD nTimestamp);
	virtual HRESULT	RenderBuffer(BYTE* pBuffer, DWORD nLength, BYTE nPayload);
protected:
	HRESULT			MixedBuffer(BYTE* pBuffer, DWORD nLength, DWORD nDeliverId = 0);
	HRESULT			InitializeMixer(WAVEFORMATEX* pwfx);
	MMRESULT		MixACMStreamWith(BYTE* pBuffer, BYTE* pByte, DWORD nLength);
	MMRESULT		ConvertACMStream(HACMSTREAM has, BYTE* pSource, BYTE* pDest, DWORD nLength, float nPercent);

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkVideo)
	virtual void PostNcDestroy();
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkVideo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent) ;
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg LONG OnStartPreview(WPARAM nPayloadMask, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	
	static void CALLBACK SyncMirrorClock(CNetworkVideo* pView);
	static void CALLBACK SyncMirrorView(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
	friend class CNetworkChild;
};

//
// Window Config
//

#define WINDOW_HEIGHT	99
#define WINDOW_WIDTH	120
//#define CAPTION_HEIGHT	20

#endif // !defined(AFX_CTRLNETWORKVIDEO_H__31BD374E_A689_4334_A2A2_ED3A66B29AC3__INCLUDED_)
