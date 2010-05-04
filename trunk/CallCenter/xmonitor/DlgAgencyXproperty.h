//
// DlgAgencyXproperty.h
//

#if !defined(AFX_DLGAGENCYXPROPERTY_H__2CEC35DC_9836_4FA5_B187_38C654DD2328__INCLUDED_)
#define AFX_DLGAGENCYXPROPERTY_H__2CEC35DC_9836_4FA5_B187_38C654DD2328__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAgencyXpropertyDlg dialog

class CAgencyXpropertyDlg : public CDialog
{
// Construction
public:
	CAgencyXpropertyDlg(LPCTSTR xsale);

// Dialog Data
protected:
	//{{AFX_DATA(CAgencyXpropertyDlg)
	enum { IDD = IDD_AGENCY_XPROPERTY };
	CComboBox	m_wndUserId;
	CDateTimeCtrl	m_wndTime;
	CEdit	m_wndPhone;
	CEdit	m_wndDesc;
	CComboBox	m_wndState;
	//}}AFX_DATA

// Attributes
protected:
	LPCTSTR			m_xsale;
	_RecordsetPtr	m_pRecordset;

// Operations
protected:
	BOOL		LoadRecordset();
	BOOL		SaveRecordset();

// Overrides
protected:
	//{{AFX_VIRTUAL(CAgencyXpropertyDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAgencyXpropertyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAGENCYXPROPERTY_H__2CEC35DC_9836_4FA5_B187_38C654DD2328__INCLUDED_)
