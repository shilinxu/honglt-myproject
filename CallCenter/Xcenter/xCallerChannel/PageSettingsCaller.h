//
// PageSettingsCaller.h
//

#if !defined(AFX_PAGESETTINGSCALLER_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSCALLER_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CCallerSettingsPage dialog

class CCallerSettingsPage : public CSettingsPage
{
// Construction
public:
	CCallerSettingsPage();
	virtual ~CCallerSettingsPage();

	DECLARE_DYNCREATE(CCallerSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CCallerSettingsPage)
	enum { IDD = IDD_SETTINGS_CALLER };
	CComboBox	m_wndMaxRing;
	CButton		m_wndNoUser;
	CButton		m_wndUsrBusy;
	CEdit		m_wndUsrTime;
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
	//{{AFX_VIRTUAL(CCallerSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCallerSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnNoUser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSCALLER_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
