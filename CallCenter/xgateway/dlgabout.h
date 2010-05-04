//
// DlgAbout.h
//

#if !defined(AFX_DLGABOUT_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_)
#define AFX_DLGABOUT_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg

class CAboutDlg : public CDialog
{
// Construction
public:
	CAboutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
public:
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUT };
	CStatic	m_wndWeb;
	CStatic	m_wndMail;
	CStatic	m_wndTitle;
	//}}AFX_DATA
	
	COLORREF	m_crWhite;
	CBrush		m_brWhite;

protected:
	void		LoadRegistry();

// Overrides
public:
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGABOUT_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_)
