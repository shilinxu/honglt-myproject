//
// PageSettingsTelecom.h
//

#if !defined(AFX_PAGESETTINGSTELECOM_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSTELECOM_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CTelecomSettingsPage dialog

class CTelecomSettingsPage : public CSettingsPage
{
// Construction
public:
	CTelecomSettingsPage();
	virtual ~CTelecomSettingsPage();

	DECLARE_DYNCREATE(CTelecomSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CTelecomSettingsPage)
	enum { IDD = IDD_SETTINGS_TELECOM };
	CComboBox	m_wndHost;
	CComboBox	m_wndPort;
	CEdit		m_wndUserId;
	CEdit		m_wndPassword;
	CEdit		m_wndPathUsr;
	CButton		m_wndTest;
	CEdit		m_wndTestPhone;
	CEdit		m_wndBookmark;
	//}}AFX_DATA

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CTelecomSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTelecomSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnTestUser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSTELECOM_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
