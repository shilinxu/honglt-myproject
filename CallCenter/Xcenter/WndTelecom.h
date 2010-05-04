//
// WndTelecom.h
//

#if !defined(AFX_WNDTELECOM_H__791A8CFB_7390_4C6F_B51F_4BE66CDAFCF7__INCLUDED_)
#define AFX_WNDTELECOM_H__791A8CFB_7390_4C6F_B51F_4BE66CDAFCF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlP2jobPanel.h"
#include "CtrlCoolBar.h"

/////////////////////////////////////////////////////////////////////////////
// CTelecomWnd window

class CTelecomWnd : public CChildWnd
{
// Construction
public:
	CTelecomWnd();
	virtual ~CTelecomWnd();
	
	DECLARE_SERIAL(CTelecomWnd)

// Attributes
public:
	CP2jobPanel		m_wndPanel;
	CListCtrl		m_wndList;
	CCoolBarCtrl	m_wndToolBar;
	CEdit			m_wndSearch;
protected:
	HANDLE				m_hThread;
	volatile BOOL		m_bThread;
	CEvent				m_pWakeup;
	CCriticalSection	m_pSection;
protected:
	_RecordsetPtr		m_pRecordset;

// Operations
public:

// Thread
protected:
	void			StartThread();
	void			StopThread();
	BOOL			OnRunPager();
	BOOL			OnRunPager(DWORD nExpired);
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();

// Overrides
public:
	//{{AFX_VIRTUAL(CTelecomWnd)
	virtual BOOL Create(UINT nID, LPCTSTR xCaption);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTelecomWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnP2jobStart();
	afx_msg void OnP2jobClear();
	afx_msg void OnSearchNumber();
	afx_msg void OnImportNumber();
	afx_msg void OnExportNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#include "DlgExecute.h"
class CImportNumber : public CExecuteDlg  
{
// Construction
public:
	CImportNumber(LPCTSTR xUsrFile);

// Attributes
protected:
	CString		m_xUsrFile;

// Operations
protected:
	virtual	BOOL	OnRun();
};

class CExportNumber : public CExecuteDlg  
{
// Construction
public:
	CExportNumber(LPCTSTR xUsrFile);

// Attributes
protected:
	CString		m_xUsrFile;

// Operations
protected:
	virtual	BOOL	OnRun();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define	IDC_TELECOM_LIST		1001
#define IDC_SEARCH_NUMBER		1002
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDTELECOM_H__791A8CFB_7390_4C6F_B51F_4BE66CDAFCF7__INCLUDED_)
