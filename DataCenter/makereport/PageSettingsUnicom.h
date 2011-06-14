//
// PageSettingsUnicom.h
//

#if !defined(AFX_PAGESETTINGSUNICOM_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSUNICOM_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CUnicomSettingsPage dialog

class CUnicomSettingsPage : public CSettingsPage
{
// Construction
public:
	CUnicomSettingsPage();
	virtual ~CUnicomSettingsPage();

	DECLARE_DYNCREATE(CUnicomSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CUnicomSettingsPage)
	enum { IDD = IDD_SETTINGS_UNICOM };
	CComboBox	m_wndHost;
	CComboBox	m_wndPort;
	CEdit		m_wndUserId;
	CEdit		m_wndPassword;
	CEdit		m_wndPathUsr;
	CComboBox	m_wndInterval;
	CEdit		m_wndBookmark;
	CButton		m_wndTest;
	CEdit		m_wndTestPhone;
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
	//{{AFX_VIRTUAL(CUnicomSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUnicomSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnTestUser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSUNICOM_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
