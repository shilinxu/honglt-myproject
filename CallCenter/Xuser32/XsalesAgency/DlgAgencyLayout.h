//
// DlgAgencyLayout.h
//

#if !defined(AFX_DLGAGENCYLAYOUT_H__D77CAFA7_2BF8_4059_8A30_90D85AC68565__INCLUDED_)
#define AFX_DLGAGENCYLAYOUT_H__D77CAFA7_2BF8_4059_8A30_90D85AC68565__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAgencyLayoutDlg dialog
class CXMLElement;

class CAgencyLayoutDlg : public CDialog
{
// Construction
public:
	CAgencyLayoutDlg(CXMLElement* pXML, CXMLElement* pJobs);
	CAgencyLayoutDlg::CAgencyLayoutDlg(LPCTSTR hKey, CWnd* pParentWnd = NULL);

// Dialog Data
protected:
	//{{AFX_DATA(CAgencyLayoutDlg)
	enum { IDD = IDD_AGENCY_LAYOUT };
	CEdit		m_wndDesc;
	CEdit		m_wndPhone;
	CComboBox	m_wndState;
	CEdit		m_wndReason;
	CButton		m_wndStartup;
	CButton		m_wndOK;
	//}}AFX_DATA

// Attributes
protected:
	CXMLElement*	m_pXML;
	CXMLElement*	m_pJobs;
	LPCTSTR			m_hKey;
protected:
	_RecordsetPtr	m_pRecordset;
	CWnd*			m_pMainWnd;
	
// Operations
public:
	_ConnectionPtr	GetConnectionPtr() const;
protected:
	BOOL		LoadRecordset();
	void		EnableMemvar(BOOL bEnable = TRUE);

// Overrides
protected:
	//{{AFX_VIRTUAL(CAgencyLayoutDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAgencyLayoutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnStartup();
	//}}AFX_MSG
	afx_msg LONG OnPacketReq(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAGENCYLAYOUT_H__D77CAFA7_2BF8_4059_8A30_90D85AC68565__INCLUDED_)
