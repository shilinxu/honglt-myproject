//
// PageSettingsRecord.h
//

#if !defined(AFX_PAGESETTINGSRECORD_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
#define AFX_PAGESETTINGSRECORD_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CRecordSettingsPage dialog

class CRecordSettingsPage : public CSettingsPage
{
// Construction
public:
	CRecordSettingsPage();
	virtual ~CRecordSettingsPage();

	DECLARE_DYNCREATE(CRecordSettingsPage)

// Dialog Data
public:
	//{{AFX_DATA(CRecordSettingsPage)
	enum { IDD = IDD_SETTINGS_RECORD };
	CButton	m_wndRecord;
	//}}AFX_DATA

// Operations
protected:
	BOOL		LoadSettings();
	BOOL		EditSettings();

// Overrides
public:
	//{{AFX_VIRTUAL(CRecordSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CRecordSettingsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETTINGSRECORD_H__4E0DA242_36FC_44AE_A98A_87446772C98C__INCLUDED_)
