//
// WndPlayer.h
//

#if !defined(AFX_WNDPLAYER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
#define AFX_WNDPLAYER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlCoolHeaderBar.h"
#include "CtrlCoolBar.h"
#include "CtrlMediaPlayer.h"
class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CPlayerWnd window

class CPlayerWnd : public CChildWnd  
{
// Construction
public:
	CPlayerWnd();
	virtual ~CPlayerWnd();
	
	DECLARE_SERIAL(CPlayerWnd)
		
// Attributes
protected:
	CCoolHeaderBar	m_wndHeaderBar;
	CCoolBarCtrl	m_wndBottom;
protected:
	CMediaPlayer	m_wndPlayer;
	CComboBox		m_wndHistory;
private:
	BOOL			m_bPlaying;
	
// Operations
protected:
	virtual void	LoadXML(CXMLElement* pXML);
	virtual BOOL	SaveXML();
protected:
	HRESULT			ResyncStream(WORD nPayloadMask);
	HRESULT			UnsyncStream(WORD nPayloadMask);
protected:
	CXMLElement*	CreateXML();
	BOOL			SaveXML(CXMLElement* pXML);
	CXMLElement*	LoadXML(LPCTSTR lpszName, BOOL bCreate = FALSE);
	
// Overrides
public:
	//{{AFX_VIRTUAL(CPlayerWnd)
	virtual void	GetMessageString(UINT nID, CString& rMessage) const;
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	//{{AFX_MSG(CPlayerWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnToolbarReturn();
	afx_msg void OnToolbarEscape();
	afx_msg void OnUpdateNetworkConnect(CCmdUI* pCmdUI);
	afx_msg void OnNetworkConnect();
	afx_msg void OnNetworkDisconnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_USER_PHONE		1002
#define IDC_USER_SEARCH		1003
#define IDC_USER_BALANCE	1004
#define IDC_INNER_LIST		1006
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDPLAYER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
