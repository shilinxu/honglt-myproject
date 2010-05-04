//
// PageSettingsCentric.h
//

#if !defined(AFX_PAGESETTINGSCENTRIC_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSCENTRIC_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CCentricSettingsPage dialog

class CCentricSettingsPage : public CSettingsPage
{
// Construction
public:
	CCentricSettingsPage();
	virtual ~CCentricSettingsPage();

	DECLARE_DYNCREATE(CCentricSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CCentricSettingsPage)
	enum { IDD = IDD_SETTINGS_CENTRIC };
	CEdit		m_wndLocal;
	CEdit		m_wndRemote;
	CComboBox	m_wndUserId;
	CEdit		m_wndPassword;
	//}}AFX_DATA

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();

// Overrides
protected:
	//{{AFX_VIRTUAL(CCentricSettingsPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCentricSettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSCENTRIC_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
