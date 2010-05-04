//
// DlgSettingsManager.h
//

#if !defined(AFX_DLGSETTINGSMANAGER_H__E50A4019_05F6_4349_9A6C_8A0C31D30E5A__INCLUDED_)
#define AFX_DLGSETTINGSMANAGER_H__E50A4019_05F6_4349_9A6C_8A0C31D30E5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CSettingsManagerDlg dialog

class CSettingsManagerDlg : public CSettingsSheet
{
// Construction
public:
	CSettingsManagerDlg();
	virtual ~CSettingsManagerDlg();

	DECLARE_DYNAMIC(CSettingsManagerDlg)

// Attributes
protected:
	CBitmap			m_bmHeader;

// Operations
public:
	static BOOL		Run(LPCTSTR pszWindow = NULL);
	int				DoModal(LPCTSTR pszWindow = NULL);
protected:
	void			AddPage(CSettingsPage* pPage);
	void			AddGroup(CSettingsPage* pPage);
	virtual void	DoPaint(CDC& dc);

// Overrides
public:
	//{{AFX_VIRTUAL(CSettingsManagerDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSettingsManagerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETTINGSMANAGER_H__E50A4019_05F6_4349_9A6C_8A0C31D30E5A__INCLUDED_)
