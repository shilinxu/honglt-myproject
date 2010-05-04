//
// DlgConnect.h
//

#if !defined(AFX_DLGCONNECT_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_)
#define AFX_DLGCONNECT_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXMLElement;

class CConnectDlg	: public CDialog
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
	CButton	m_wndSettings;
	CEdit	m_wndDatabase;
	CComboBox	m_wndHost;
	CButton	m_wndRemote;
	CComboBox	m_wndUserId;
	CEdit		m_wndPassword;
	CButton		m_wndSaveing;
	CButton		m_wndOK;
	//}}AFX_DATA
	
// Attributes
protected:
	CXMLElement*	m_pXML;
	
// Operations
public:
	BOOL		LoadXML();
	BOOL		SaveXML();
protected:
	CXMLElement*CreateXML();
	int			LoadXML(CXMLElement* pXML);
	void		SaveHistory();
	void		SaveSettings();
protected:
	void		RemoveItem(int nItem);
	void		EnableMemvar(BOOL bEnable = TRUE);
	int			InsertItem(int nIndex, CXMLElement* pXML);
	void		ShowSettings(BOOL bShow = TRUE);
	BOOL		ChkUserState(LPCTSTR xUsrNumber, LPCTSTR xPassword);

// Overrides
public:
	//{{AFX_VIRTUAL(CConnectDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual int  DoModal();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CConnectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSelChangeUserId();
	virtual void OnOK();
	afx_msg void OnSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONNECT_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_)
