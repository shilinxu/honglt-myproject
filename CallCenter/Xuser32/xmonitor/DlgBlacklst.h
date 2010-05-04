//
// DlgBlacklst.h
//

#if !defined(AFX_DLGBLACKLST_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_)
#define AFX_DLGBLACKLST_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CBlacklstDlg

class CBlacklstDlg : public CDialog
{
// Construction
public:
	CBlacklstDlg(CWnd* pParent = NULL);
	~CBlacklstDlg();

// Dialog Data
public:
	//{{AFX_DATA(CBlacklstDlg)
	enum { IDD = IDD_USER_BLACKLST };
	CListCtrl	m_wndList;
	CButton		m_wndRemove;
	CEdit		m_wndPhone;
	CDateTimeCtrl	m_wndLockedDate;
	CDateTimeCtrl	m_wndLockedTime;
	//}}AFX_DATA
	
	COLORREF	m_crWhite;
	CBrush		m_brWhite;
	CXMLElement*	m_pXML;

// Operations
protected:
	virtual void	OnSyncOrderRelation();	
	virtual BOOL	OnSyncDataViewResp(CXMLElement* pXML);

// Overrides
public:
	//{{AFX_VIRTUAL(CBlacklstDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CBlacklstDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBALCKLST_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_)
