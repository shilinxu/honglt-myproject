//
// WndUser.h
//

#if !defined(AFX_WNDUSER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
#define AFX_WNDUSER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlCoolHeaderBar.h"
#include "CtrlUserList.h"
#include "CtrlCoolBar.h"

/////////////////////////////////////////////////////////////////////////////
// CUserWnd window
class CFtpConnection;
class CInternetFile;

class CUserWnd : public CChildWnd  
{
// Construction
public:
	CUserWnd();
	virtual ~CUserWnd();
	
	DECLARE_SERIAL(CUserWnd)
		
// Attributes
protected:
	CCoolHeaderBarCtrl	m_wndHeaderBar;
	CListCtrl*		m_pListCtrl;
	CUserList		m_wndList;
	CCoolBarCtrl	m_wndBottom;
	HMENU			m_hSubMenu;
protected:
	CEdit			m_wndPhone;
	CEdit			m_wndFind;
	CStatic			m_wndTime;
	
// Operations
protected:
	BOOL			ApplyMenu(UINT nMenuId);
	void			DestroyMenu();
	void			UpdateReverse(CXMLElement* pXML, int nItem);
protected:
	virtual BOOL	OnSyncDataViewResp(CXMLElement* pXML);
	virtual void	OnSyncOrderRelation();	
protected:
	CInternetFile*	InternetOpenFile(LPCTSTR lpszFile, CFtpConnection*& pConnect, DWORD dwAccess = GENERIC_READ);
	BOOL			InternetCloseFile(CInternetFile* pFile, CFtpConnection* pConnect);
	BOOL			CreateMapFile(LPCTSTR lpszFile, CFile& pFile) const;
	CString			InternetMapFile(LPCTSTR lpszPath, LPCTSTR lpszFile);
public:
	static DWORD	GetUserGroupInfo(LPCTSTR xUserId, CXMLElement** ppXML, DWORD nTimeOut = INFINITE );

// Overrides
public:
	//{{AFX_VIRTUAL(CUserWnd)
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	//{{AFX_MSG(CUserWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnUpdateMediaPlay(CCmdUI* pCmdUI);
	afx_msg void OnToolbarReturn();
	afx_msg void OnToolbarEscape();
	afx_msg void OnMediaPlay();
	afx_msg void OnMediaRemove();
	afx_msg void OnMediaRefresh();
	afx_msg void OnMediaRenew();
	afx_msg void OnMediaUpload();
	afx_msg void OnTopicMailbox();
	afx_msg void OnUserPassword();
	afx_msg void OnUserGroup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_USER_NUMBER		1002
#define IDC_USER_PHONE		1003
#define IDC_USER_LIST		1006
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDUSER_H__6E66B11D_2F06_421D_AC8F_0F9DD9824D01__INCLUDED_)
