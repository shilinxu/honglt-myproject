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
	CComboBox	m_wndHost;
	CComboBox	m_wndDatabase;
	CEdit		m_wndUserID;
	CEdit		m_wndPassword;
	CEdit		m_wndPathRec;
	CEdit		m_wndRecFile;
	CComboBox	m_wndTimeOut;
	//}}AFX_DATA

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();
private:
	CString			FormatTimeString(DWORD nMinutes) const;
	int				FormatTimeValue(LPCTSTR xTime) const;

// Overrides
protected:
	//{{AFX_VIRTUAL(CGeneralSettingsPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CGeneralSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPathRecBrowse();
	afx_msg void OnDefaultRecFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSGENERAL_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
