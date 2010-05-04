//
// DlgExecute.h
//

#if !defined(AFX_DLGEXECUTE_H__7EA0C15C_E9AC_4463_AC52_14F35B73FD99__INCLUDED_)
#define AFX_DLGEXECUTE_H__7EA0C15C_E9AC_4463_AC52_14F35B73FD99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CExecuteDlg dialog

class CExecuteDlg : public CDialog
{
// Construction
protected:
	CExecuteDlg();
	virtual ~CExecuteDlg();
	
	//{{AFX_DATA(CExecuteDlg)
	enum { IDD = IDD_EXECUTE };
	//}}AFX_DATA
	DECLARE_DYNAMIC(CExecuteDlg)
	
// Attributes
protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;
	
// Operations
public:
	void	Release();
	void	Hide();
public:
	void	Start();
	void	Stop();
	BOOL	IsRunning();
	BOOL	Execute(LPCTSTR xObject, LPCTSTR xFormat);
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun() = 0;
	
// Overrides
public:
	//{{AFX_VIRTUAL(CExecuteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CExecuteDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXECUTE_H__7EA0C15C_E9AC_4463_AC52_14F35B73FD99__INCLUDED_)
