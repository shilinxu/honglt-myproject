//
// DlgSplash.h
//

#if !defined(AFX_DLGSPLASH_H__7EA0C15C_E9AC_4463_AC52_14F35B73FD99__INCLUDED_)
#define AFX_DLGSPLASH_H__7EA0C15C_E9AC_4463_AC52_14F35B73FD99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg dialog

class CSplashDlg : public CDialog
{
// Construction
public:
	CSplashDlg();
	virtual ~CSplashDlg();
	
	//{{AFX_DATA(CSplashDlg)
	enum { IDD = IDD_SPLASH };
	//}}AFX_DATA
	DECLARE_DYNAMIC(CSplashDlg)
	
// Attributes
protected:
	CString		m_sState;
protected:
	CBitmap		m_bmSplash;
	CBitmap		m_bmBuffer;
	CDC			m_dcBuffer1;
	CDC			m_dcBuffer2;
protected:
	HINSTANCE	m_hUser32;
	BOOL		(WINAPI *m_pfnAnimateWindow)(HWND, DWORD, DWORD);
	
// Operations
public:
	void	Step(LPCTSTR pszText);
	void	Topmost();
	void	Hide();
protected:
	void	DoPaint(CDC* pDC);
	
// Overrides
public:
	//{{AFX_VIRTUAL(CSplashDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//	CSplashDlg(CWnd* pParent = NULL);   // standard constructor
	//{{AFX_MSG(CSplashDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg LONG OnPrintClient(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSPLASH_H__7EA0C15C_E9AC_4463_AC52_14F35B73FD99__INCLUDED_)
