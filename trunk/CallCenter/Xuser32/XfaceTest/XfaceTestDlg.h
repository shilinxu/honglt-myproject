//
// XfaceTestDlg.h
//

#if !defined(AFX_XFACETESTDLG_H__66632DE7_44B0_4131_BD0B_8441CFB0DCB7__INCLUDED_)
#define AFX_XFACETESTDLG_H__66632DE7_44B0_4131_BD0B_8441CFB0DCB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

class CAboutDlg : public CDialog
{
// Construction
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#include "PageTesterGeneral.h"
#include "PageTesterChat.h"

/////////////////////////////////////////////////////////////////////////////
// CXfaceTestDlg dialog

class CXfaceTestDlg : public CDialog
{
// Construction
public:
	CXfaceTestDlg(CWnd* pParent = NULL);

// Dialog Data
protected:
	//{{AFX_DATA(CXfaceTestDlg)
	enum { IDD = IDD_XFACETEST_DIALOG };
	CEdit		m_wndPort;
	CComboBox	m_wndHost;
	CComboBox	m_wndUserId;
	CEdit		m_wndPassword;
	CTabCtrl	m_wndSheet;
	//}}AFX_DATA

// Dialog Pages
protected:
	CGeneralTesterPage	m_pGeneral;
	CChatTesterPage		m_pChat;
	CSettingsPage*		m_pPage;

// Overrides
	//{{AFX_VIRTUAL(CXfaceTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL	CreateReal(UINT nID);
	virtual BOOL	SetActivePage(CSettingsPage* pPage);
protected:
	//{{AFX_MSG(CXfaceTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnSelChangeSheet(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XFACETESTDLG_H__66632DE7_44B0_4131_BD0B_8441CFB0DCB7__INCLUDED_)
