//
// PageSettingsGeneral.h
//

#if !defined(AFX_PAGESETTINGSGENERAL_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSGENERAL_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CGeneralSettingsPage dialog

class CGeneralSettingsPage : public CSettingsPage
{
// Construction
public:
	CGeneralSettingsPage();
	virtual ~CGeneralSettingsPage();

	DECLARE_DYNCREATE(CGeneralSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CGeneralSettingsPage)
	enum { IDD = IDD_SETTINGS_GENERAL };
	CEdit	m_wndShtFree;
	CEdit	m_wndUsrLength;
	CEdit	m_wndUsrHeader;
	CEdit	m_wndGivenValue;
	CComboBox	m_wndGiven;
	CComboBox	m_wndHost;
	CComboBox	m_wndDatabase;
	CEdit	m_wndUserId;
	CEdit	m_wndPassword;
	CEdit		m_wndGateway;
	CEdit		m_wndPort;
	CComboBox	m_wndMaxLength;
	//}}AFX_DATA

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();
private:
	CString			FormatTimeString(DWORD nMinutes) const;
	int				FormatTimeValue(LPCTSTR xTime) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CGeneralSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CGeneralSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeGiven();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSGENERAL_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
