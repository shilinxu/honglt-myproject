//
// WndClerk.h
//

#if !defined(AFX_WNDCLERK_H__791A8CFB_7390_4C6F_B51F_4BE66CDAFCF7__INCLUDED_)
#define AFX_WNDCLERK_H__791A8CFB_7390_4C6F_B51F_4BE66CDAFCF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlClerkPanel.h"
#include "CtrlCoolBar.h"

class CUserWnd;

/////////////////////////////////////////////////////////////////////////////
// CClerkWnd window

class CClerkWnd : public CChildWnd
{
// Construction
public:
	CClerkWnd();
	virtual ~CClerkWnd();
	
	DECLARE_SERIAL(CClerkWnd)

// Attributes
public:
	CClerkPanel		m_wndPanel;
	CListCtrl		m_wndList;
	CCoolBarCtrl	m_wndToolBar;
	CEdit			m_wndSearch;
	CEvent			m_pWakeup;
protected:
	HANDLE				m_hThread;
	volatile BOOL		m_bThread;
	CCriticalSection	m_pSection;
	_RecordsetPtr		m_pRecordset;
	DWORD				m_tNavigated;
	CUserWnd*			m_pUserWnd;
	BOOL				m_bIsOpen;

// Operations
public:
	BOOL			AddItem(LPCTSTR xClerkId, LPCTSTR xChState = NULL);
	void			StartThread();
	void			StopThread();
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
protected:
	void			Setup();
	BOOL			OnRunClerk();
	BOOL			VerifyRecordset();
	BOOL			OnRunClerk(COleDateTime* pNow, COleDateTime* xTime);
	BOOL			OnClerkOfWalk(COleDateTime* pNow, COleDateTime* xTime);
private:
	BOOL			WeekNeedClerk(int nDay);

// Overrides
public:
	//{{AFX_VIRTUAL(CClerkWnd)
	virtual BOOL Create(UINT nID, LPCTSTR xCaption);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CClerkWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSearchNumber();
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_SEARCH_NUMBER		1002
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDCLERK_H__791A8CFB_7390_4C6F_B51F_4BE66CDAFCF7__INCLUDED_)
