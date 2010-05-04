//
// PageSettingsMonitor.h
//

#if !defined(AFX_PAGESETTINGSMONITOR_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSMONITOR_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CMonitorSettingsPage dialog

class CMonitorSettingsPage : public CSettingsPage
{
// Construction
public:
	CMonitorSettingsPage();
	virtual ~CMonitorSettingsPage();

	DECLARE_DYNCREATE(CMonitorSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CMonitorSettingsPage)
	enum { IDD = IDD_SETTINGS_MONITOR };
	CEdit		m_wndHost;
	CEdit		m_wndPort;
	CButton		m_wndExtAdd;
	CButton		m_wndExtRemove;
	CComboBox	m_wndExtList;
	//}}AFX_DATA

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CMonitorSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMonitorSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditChangeExtList();
	afx_msg void OnSelChangeExtList();
	afx_msg void OnExtAdd();
	afx_msg void OnExtRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSMONITOR_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
