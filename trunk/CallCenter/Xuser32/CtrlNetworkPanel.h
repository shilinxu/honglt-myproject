//
// CtrlNetworkPanel.h
//

#if !defined(AFX_CTRLNETWORKPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
#define AFX_CTRLNETWORKPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "RenderGraph.h"

class AFX_EXT_CLASS CNetworkVideo : public CTaskBox, protected CGraphRender
{
// Construction
public:
	CNetworkVideo(LPCTSTR xUserId);
	virtual ~CNetworkVideo();
	
	DECLARE_DYNAMIC(CNetworkVideo)
	
// Attributes
protected:
	
// Operations
protected:

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkVideo)
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CNetworkVideo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LONG OnStartPreview(WPARAM nPayloadMask, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	
	friend class CNetworkPanel;
};

class AFX_EXT_CLASS CNetworkPanel : public CTaskPanel
{
// Construction
public:
	CNetworkPanel();
	virtual ~CNetworkPanel();
	
	DECLARE_DYNAMIC(CNetworkPanel)

// Attributes
protected:

// Operations
public:
	CTaskBox*		RenderUser(LPCTSTR lpzCaption, int nAtAfter = 0);
	CTaskBox*		RenderUser(LPCTSTR xUserId, LPCTSTR lpzAlias, int nAtAfter);
	CTaskBox*		FindUser(LPCTSTR xUserId, int nAtAfter = 0) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkPanel)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern AFX_EXT_CLASS CNetworkPanel wndPanel;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_NETWORK_PANEL		101
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
