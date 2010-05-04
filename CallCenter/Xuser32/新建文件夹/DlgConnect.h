//
// DlgConnect.h
//

#if !defined(AFX_DLGCONNECT_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_)
#define AFX_DLGCONNECT_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChannel;
/////////////////////////////////////////////////////////////////////////////
// CConnectDlg dialog

class CConnectDlg : public CDialog
{
// Construction
public:
	CConnectDlg();
	virtual ~CConnectDlg();
	DECLARE_DYNAMIC(CConnectDlg)
	
// Dialog Data
protected:
	//{{AFX_DATA(CConnectDlg)
	enum { IDD = IDD_CONNECT };
	CComboBox	m_wndUserId;
	CEdit		m_wndPassword;
	CButton		m_wndSaveing;
	CButton		m_wndOK;
	//}}AFX_DATA
	
// Attributes
protected:
	CChannel*	m_pChannel;
	
// Operations
protected:
	int			LoadHistory();
	void		SaveHistory();
	void		EnableMemvar(BOOL bEnable = TRUE);
	int			InsertItem(int nIndex, LPCTSTR xUserId, DWORD nData);
	void		RemoveItem(int nItem);
protected:
	void		OnConnected();
	void		OnFailed(int nErrCode);
	
// Overrides
public:
	//{{AFX_VIRTUAL(CConnectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CConnectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeUserId();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LONG OnBindUser(BOOL bConnected, INT nError);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONNECT_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_)
