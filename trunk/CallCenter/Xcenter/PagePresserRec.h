//
// PagePresserRec.h
//

#if !defined(AFX_PAGEPRESSERREC_H__2FE13AE7_7C4F_40F9_8BC1_7097B4FBF4F8__INCLUDED_)
#define AFX_PAGEPRESSERREC_H__2FE13AE7_7C4F_40F9_8BC1_7097B4FBF4F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CRecPresserPage dialog

class CRecPresserPage : public CSettingsPage
{
// Construction
public:
	CRecPresserPage();
	virtual ~CRecPresserPage();

	DECLARE_DYNCREATE(CRecPresserPage)

// Dialog Data
public:
	//{{AFX_DATA(CRecPresserPage)
	enum { IDD = IDD_PRESSER_REC };
	CEdit	m_wndCopyTo;
	CComboBox	m_wndLength;
	CComboBox	m_wndPhone;
	CComboBox	m_wndUserId;
	CComboBox	m_wndExpire;
	//}}AFX_DATA
protected:
	CString*	m_pRecSrcFile;

// Operations
protected:
	BOOL		PressRecord(LPCTSTR xPath, LPCTSTR xUserId, int nExpire);
	int			PressRecord(LPCTSTR xPath, LPCTSTR xTarget, LPCTSTR xUserId, int nExpire);

// Overrides
public:
	//{{AFX_VIRTUAL(CRecPresserPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CRecPresserPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnUserPress();
	afx_msg void OnUserView();
	afx_msg void OnRecCopyBrowse();
	afx_msg void OnRecPress();
	afx_msg void OnRecView();
	afx_msg void OnCloseUpRecNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPRESSERREC_H__2FE13AE7_7C4F_40F9_8BC1_7097B4FBF4F8__INCLUDED_)
