//
// WndSettingsSheet.h
//

#if !defined(AFX_WNDSETTINGSSHEET_H__F770E40C_17F2_41E1_B8CD_7FA606D3990A__INCLUDED_)
#define AFX_WNDSETTINGSSHEET_H__F770E40C_17F2_41E1_B8CD_7FA606D3990A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "CtrlIconButton.h"

class CSettingsPage;

/////////////////////////////////////////////////////////////////////////////
// CSettingsSheet dialog

class CSettingsSheet : public CDialog
{
// Construction
public:
	CSettingsSheet(CWnd* pParent = NULL, UINT nCaptionID = 0);
	virtual ~CSettingsSheet();

	DECLARE_DYNAMIC(CSettingsSheet)

// Attributes
protected:
	CObArray		m_pPages;
	CSettingsPage*	m_pPage;
	CSettingsPage*	m_pFirst;
	CSize			m_szPages;
protected:
	CTreeCtrl		m_wndTree;
	CButton			m_wndOK;
	CButton			m_wndCancel;
	CButton			m_wndApply;
	BOOL			m_bModified;
protected:
	DLGTEMPLATE*	m_pTemplate;
	CString			m_sCaption;
	int				m_nLeftMargin;
	int				m_nTopMargin;
	int				m_nListWidth;
	int				m_nListMargin;
	int				m_nButtonHeight;

// Operations
public:
	void			AddPage(CSettingsPage* pPage, LPCTSTR pszCaption = NULL);
	void			AddGroup(CSettingsPage* pPage, LPCTSTR pszCaption = NULL);
	CSettingsPage*	GetPage(int nPage) const;
	CSettingsPage*	GetPage(CRuntimeClass* pClass) const;
	CSettingsPage*	GetPage(LPCTSTR pszClass) const;
	int				GetPageIndex(CSettingsPage* pPage) const;
	int				GetPageCount() const;
	CSettingsPage*	GetActivePage() const;
	BOOL			SetActivePage(CSettingsPage* pPage);
	BOOL			SetActivePage(int nPage);
	BOOL			IsModified() const;
	void			SetModified(BOOL bChanged = TRUE);
	int				DoModal();
protected:
	void			BuildTree();
	void			Layout();
	BOOL			CreatePage(CSettingsPage* pPage);
	virtual void	DoPaint(CDC& dc);

// Overrides
protected:
	//{{AFX_VIRTUAL(CSettingsSheet)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CSettingsSheet)
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnApply();
	afx_msg void OnTreeExpanding(NM_TREEVIEW* pNotify, LRESULT *pResult);
	afx_msg void OnSelectPage(NM_TREEVIEW* pNotify, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
#define IDC_SETTINGS_TREE	100
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDSETTINGSSHEET_H__F770E40C_17F2_41E1_B8CD_7FA606D3990A__INCLUDED_)
