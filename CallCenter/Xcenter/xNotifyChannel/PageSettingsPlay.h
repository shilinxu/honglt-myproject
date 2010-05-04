//
// PageSettingsPlay.h
//

#if !defined(AFX_PAGESETTINGSPLAY_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSPLAY_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

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
	CComboBox	m_wndLock;
	CEdit		m_wndNavOffset;
	//}}AFX_DATA

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();
protected:
	virtual BOOL	LockJob(LPCTSTR pszText);
	virtual BOOL	LockWith(int nChan);

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

#endif // !defined(AFX_PAGESETTINGSPLAY_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
