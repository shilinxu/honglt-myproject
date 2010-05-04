//
// WndInner.h
//

#if !defined(AFX_WNDINNER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
#define AFX_WNDINNER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlInnerHeaderBar.h"
#include "CtrlInnerList.h"
#include "CtrlCoolBar.h"

/////////////////////////////////////////////////////////////////////////////
// CInnerWnd window

class CInnerWnd : public CChildWnd  
{
// Construction
public:
	CInnerWnd();
	virtual ~CInnerWnd();
	
	DECLARE_SERIAL(CInnerWnd)
		
// Attributes
protected:
	CInnerHeaderBar	m_wndHeaderBar;
	CInnerList		m_wndList;
	CCoolBarCtrl	m_wndBottom;
	CEdit			m_wndPhone;
	CEdit			m_wndFind;
	CStatic			m_wndBalan;
protected:
	CTaskBox*		m_pWindow;
	CToolBar		m_wndDtmfBar;
	
// Operations
public:
	void			PutReady(LPCTSTR xRemoteId = "");
	void			PutRinging(LPCTSTR xRemoteId);
	void			PutConnecting(LPCTSTR xRemoteId);
protected:
	BOOL			PlayRinging(HINSTANCE hInstance, UINT nResourceID);
	BOOL			PutDialup(LPCTSTR xRemoteId, LPCTSTR xCallerId = NULL);
	void			PutIntoPhone(CXMLElement* pXML, int nPos);
	void			StopRinging();
protected:
	LPBYTE			LoadDtmfKey(HINSTANCE hInstance, UINT nResourceID);
	LPBYTE			LoadDtmfKey(LPBYTE pBuffer, int nIndex);
protected:
	virtual BOOL	OnSyncHostFile(CXMLElement* pXML);
	virtual BOOL	OnSyncDataViewResp(CXMLElement* pXML);
	virtual BOOL	OnCalcUserBlanceResp(CXMLElement* pXML);
	virtual BOOL	OnUserGroupInfoResp(CXMLElement* pXML, HMENU hMenu);
protected:
	virtual void	OnSyncOrderRelation();	
	virtual void	CalcUserBlance(LPCTSTR xUserId);
	virtual void	GetUserGroupInfo(LPCTSTR xUserId);
private:
	HMENU			FindMenuGroup(HMENU hParent, LPCTSTR lpszMenu);
	void			PutIntoPhone(HMENU hMenu, UINT nMenuId);
	
// Overrides
public:
	//{{AFX_VIRTUAL(CInnerWnd)
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	//{{AFX_MSG(CInnerWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnToolbarReturn();
	afx_msg void OnToolbarEscape();
	afx_msg void OnUpdateNetworkConnect(CCmdUI* pCmdUI);
	afx_msg void OnNetworkConnect();
	afx_msg void OnInnerRefresh();
	afx_msg void OnInnerReply();
	afx_msg void OnDtmfKeydown(UINT nID);
	afx_msg void OnUpdateDtmfKeydown(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUserGroup(CCmdUI* pCmdUI) ;
	afx_msg void OnSearchGroup();
	afx_msg void OnUserMeeting();
	//}}AFX_MSG
	afx_msg BOOL OnSyncNotify(WPARAM, CXMLElement* pXML);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_USER_PHONE		1002
#define IDC_USER_SEARCH		1003
#define IDC_USER_BALANCE	1004
#define IDC_INNER_LIST		1006
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDINNER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
