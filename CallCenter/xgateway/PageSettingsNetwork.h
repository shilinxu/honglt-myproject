//
// PageSettingsNetwork.h
//

#if !defined(AFX_PAGESETTINGSNETWORK_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
#define AFX_PAGESETTINGSNETWORK_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CNetworkSettingsPage dialog

class CNetworkSettingsPage : public CSettingsPage
{
// Construction
public:
	CNetworkSettingsPage();
	virtual ~CNetworkSettingsPage();

	DECLARE_DYNCREATE(CNetworkSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CNetworkSettingsPage)
	enum { IDD = IDD_SETTINGS_NETWORK };
	CComboBox	m_wndHost;
	CEdit		m_wndPort;
	//}}AFX_DATA

// Operations
protected:
	BOOL			LoadSettings();
	BOOL			EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkSettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSNETWORK_H__A8D0DAF8_D2D4_4866_8228_080BF0C97393__INCLUDED_)
