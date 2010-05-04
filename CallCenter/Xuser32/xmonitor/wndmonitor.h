//
// WndMonitor.h
//

#if !defined(AFX_WNDMONITOR_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
#define AFX_WNDMONITOR_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlCoolHeaderBar.h"
#include "CtrlNetworkChild.h"
#include "CtrlCoolBar.h"

/////////////////////////////////////////////////////////////////////////////
// CMonitorWnd window
class CPacket;
class CXMLElement;
class RTPAddress;
class RTPSessionLayer;
class CDatagrams;

class CMonitorWnd : public CChildWnd  
{
// Construction
public:
	CMonitorWnd();
	virtual ~CMonitorWnd();
	
	DECLARE_SERIAL(CMonitorWnd)
		
// Attributes
protected:
	CCoolHeaderBarCtrl	m_wndHeaderBar;
	CNetworkChild	m_wndChild;
	CCoolBarCtrl	m_wndBottom;
	HMENU			m_hSubMenu;
	CCriticalSection m_pSection;
protected:
	CEdit			m_wndPhone;
	CEdit			m_wndFind;
	CStatic			m_wndTime;
protected:
	CDatagrams*		m_pDatagrams;
	
// Operations
protected:
	void			Apply();
	BOOL			ApplyMenu(UINT nMenuId);
	void			DestroyMenu();
	void			SyncMirrorView();
	void			BreakMirrorView();
	BOOL			ParseMatch(CXMLElement* pXML, CDatagrams* pParent);
	CDatagrams*		JoinedMirror(LPCTSTR xUserId, CXMLElement* pXML, WORD& nPayloadMask);
protected:
	virtual void	JoinMonitor(BOOL bJoined = TRUE);
protected:
	virtual BOOL	OnSyncOrderRelation(CXMLElement* pXML);
	virtual BOOL	OnSyncMirrorView(CXMLElement* pXML);
	virtual BOOL	OnSyncMirrorTalk(CXMLElement* pXML);
	
// Overrides
public:
	//{{AFX_VIRTUAL(CMonitorWnd)
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	//{{AFX_MSG(CMonitorWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnUpdateMediaRecord(CCmdUI* pCmdUI);
	afx_msg void OnMediaRecord();
	afx_msg void OnUpdateMediaRecMask(CCmdUI* pCmdUI);
	afx_msg void OnMediaRecMask();
	afx_msg void OnUserBlacklst();
	//}}AFX_MSG
	afx_msg BOOL OnSyncNotify(WPARAM, CXMLElement* pXML);
	DECLARE_MESSAGE_MAP()
		
	friend class CNetworkThumb;
};

#define WM_SYNCRECORD		(WM_USER + 2)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_USER_NUMBER		1002
#define IDC_USER_PHONE		1003
#define IDC_INNER_LIST		1006
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDMONITOR_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
