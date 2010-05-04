//
// PageSettingsMaster.h
//

#if !defined(AFX_PAGESETTINGSMASTER_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSMASTER_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CMasterSettingsPage dialog

class CMasterSettingsPage : public CSettingsPage
{
// Construction
public:
	CMasterSettingsPage();
	virtual ~CMasterSettingsPage();

	DECLARE_DYNCREATE(CMasterSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CMasterSettingsPage)
	enum { IDD = IDD_SETTINGS_MASTER };
	CEdit		m_wndHost;
	CEdit		m_wndPort;
	CEdit		m_wndTimeOut;
	//}}AFX_DATA

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CMasterSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CMasterSettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSMASTER_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
