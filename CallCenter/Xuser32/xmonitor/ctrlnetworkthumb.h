//
// CtrlNetworkThumb.h
//

#if !defined(AFX_CTRLNETWORKTHUMB_H__31BD374E_A689_4334_A2A2_ED3A66B29AC3__INCLUDED_)
#define AFX_CTRLNETWORKTHUMB_H__31BD374E_A689_4334_A2A2_ED3A66B29AC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlNetworkPanel.h"
class CNetworkChild;

class CNetworkThumb : public CNetworkVideo
{
// Construction
public:
	CNetworkThumb(CNetworkChild* pPanel, LPCTSTR xUserId);
	virtual ~CNetworkThumb();
	DECLARE_DYNAMIC(CNetworkThumb)

// Attributes
protected:
	CNetworkChild*	m_pPanel;
protected:
	SOCKADDR_IN		m_pHost;

// Operations
protected:
	CNetworkChild*	GetPanel() const;
	CString			GetRecordFile(LPCTSTR fMask, LPCTSTR lpszPath) const;
protected:
	virtual HRESULT	OnRecordFile(DWORD nPayloadMask, LPCTSTR fMask);

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkThumb)
	virtual void PostNcDestroy();
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkThumb)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	friend class CNetworkChild;
	friend class CMonitorWnd;
};

//
// Window Config
//

#define WINDOW_HEIGHT	99
#define WINDOW_WIDTH	120
//#define CAPTION_HEIGHT	20

#endif // !defined(AFX_CTRLNETWORKTHUMB_H__31BD374E_A689_4334_A2A2_ED3A66B29AC3__INCLUDED_)
