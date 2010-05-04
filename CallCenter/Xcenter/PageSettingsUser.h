//
// PageSettingsUser.h
//

#if !defined(AFX_PAGESETTINGSUSER_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSUSER_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CUserSettingsPage dialog

class CUserSettingsPage : public CSettingsPage
{
// Construction
public:
	CUserSettingsPage();
	virtual ~CUserSettingsPage();

	DECLARE_DYNCREATE(CUserSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CUserSettingsPage)
	enum { IDD = IDD_SETTINGS_USER };
	CComboBox	m_wndRecTrunk;
	CComboBox	m_wndRecType;
	CEdit		m_wndRecFile;
	CComboBox	m_wndMaxRing;
	CButton		m_wndNoUser;
	CButton		m_wndUsrBusy;
	CEdit		m_wndUsrTime;
	CEdit		m_wndBrgPort;
	CComboBox	m_wndBridge;
	//}}AFX_DATA

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();
private:
	CString		FormatTimeString(DWORD nMinutes) const;
	int			FormatTimeValue(LPCTSTR xTime) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CUserSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUserSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnNoUser();
	afx_msg void OnRecFileDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSUSER_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
