//
// PageSettingsPager.h
//

#if !defined(AFX_PAGESETTINGSPAGER_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
#define AFX_PAGESETTINGSPAGER_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CPagerSettingsPage dialog

class CPagerSettingsPage : public CSettingsPage
{
// Construction
public:
	CPagerSettingsPage();
	virtual ~CPagerSettingsPage();

	DECLARE_DYNCREATE(CPagerSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CPagerSettingsPage)
	enum { IDD = IDD_SETTINGS_PAGER };
	CButton		m_wndNavPlay;
	CEdit		m_wndNavOffset;
	CComboBox	m_wndPageTry;
	CComboBox	m_wndPageVal;
	CComboBox	m_wndPageExp;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();
private:
	CString		FormatTimeString(DWORD nMinutes) const;
	int			FormatTimeValue(LPCTSTR xTime) const;
	CString		FormatRetryString(int nRetry) const;
	int			FormatRetryValue(LPCTSTR nRetry) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CPagerSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPagerSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnNavPlay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSPAGER_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
