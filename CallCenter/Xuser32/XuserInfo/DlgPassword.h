//
// DlgPassword.h
//

#if !defined(AFX_DLGPASSWORD_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_)
#define AFX_DLGPASSWORD_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg

class CPasswordDlg : public CDialog
{
// Construction
public:
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
public:
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_USER_PASSWORD };
	CEdit	m_wndPass0;
	CEdit	m_wndPass1;
	CEdit	m_wndPass2;
	//}}AFX_DATA
	
	COLORREF	m_crWhite;
	CBrush		m_brWhite;
	HCURSOR		m_hCursor;
	BOOL		m_bInTimer;

// Operations
protected:
	void		OnSyncOrderRelationReq(LPCTSTR xOldPass, LPCTSTR xNewPass);

// Overrides
public:
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPasswordDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPASSWORD_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_)
