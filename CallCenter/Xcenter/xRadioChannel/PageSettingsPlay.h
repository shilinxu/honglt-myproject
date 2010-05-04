//
// PageSettingsPlay.h
//

#if !defined(AFX_PAGESETTINGSPLAY_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
#define AFX_PAGESETTINGSPLAY_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CPlaySettingsPage dialog

class CPlaySettingsPage : public CSettingsPage
{
// Construction
public:
	CPlaySettingsPage();
	virtual ~CPlaySettingsPage();

	DECLARE_DYNCREATE(CPlaySettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CPlaySettingsPage)
	enum { IDD = IDD_SETTINGS_PLAY };
	CEdit		m_wndNew0;
	CEdit		m_wndMarkup0;
	CEdit		m_wndCode1;
	CEdit		m_wndNew1;
	CEdit		m_wndMarkup1;
	CComboBox	m_wndExpired;
	CComboBox	m_wndRecLeng;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();
private:
	CString		FormatTimeString(DWORD nMinutes) const;
	int			FormatTimeValue(LPCTSTR xTime) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CPlaySettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPlaySettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSPLAY_H__96BE0AAC_2556_4254_A09F_D013DB0F1B21__INCLUDED_)
