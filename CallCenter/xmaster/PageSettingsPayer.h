//
// PageSettingsPayer.h
//

#if !defined(AFX_PAGESETTINGSPAYER_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
#define AFX_PAGESETTINGSPAYER_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CPayerSettingsPage dialog

class CPayerSettingsPage : public CSettingsPage
{
// Construction
public:
	CPayerSettingsPage();
	virtual ~CPayerSettingsPage();

	DECLARE_DYNCREATE(CPayerSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CPayerSettingsPage)
	enum { IDD = IDD_SETTINGS_PAYER };
	CEdit	m_wndPayStart;
	CEdit	m_wndPayClose;
	CEdit	m_wndPayBalan;
	CButton	m_wndRedEnable;
	CEdit	m_wndRedBalan;
	CEdit	m_wndPath;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CPayerSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPayerSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPathBrowse();
	afx_msg void OnRedEnable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSPAYER_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
