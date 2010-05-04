//
// DlgAgencyProperties.h
//

#if !defined(AFX_DLGAGENCYPROPERTIES_H__2CEC35DC_9836_4FA5_B187_38C654DD2328__INCLUDED_)
#define AFX_DLGAGENCYPROPERTIES_H__2CEC35DC_9836_4FA5_B187_38C654DD2328__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAgencyPropertiesDlg dialog

class CAgencyPropertiesDlg : public CDialog
{
// Construction
public:
	CAgencyPropertiesDlg(LPCTSTR xsaleinfo);
	virtual ~CAgencyPropertiesDlg();

// Dialog Data
protected:
	//{{AFX_DATA(CAgencyPropertiesDlg)
	enum { IDD = IDD_AGENCY_PROPERTIES };
	CDateTimeCtrl	m_wndExpired;
	CEdit	m_wndOpening;
	CEdit	m_wndDesc;
	CEdit	m_wndJobs;
	CEdit	m_wndFeeNumber;
	//}}AFX_DATA

// Attributes
protected:
	_RecordsetPtr	m_pRecordset;
	LPTSTR			m_xsaleinfo;

// Operations
protected:
	BOOL		LoadRecordset();
	BOOL		SaveRecordset();
	BOOL		GetJobId(CString& strJobId);

// Overrides
protected:
	//{{AFX_VIRTUAL(CAgencyPropertiesDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAgencyPropertiesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CAgencyWnd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAGENCYPROPERTIES_H__2CEC35DC_9836_4FA5_B187_38C654DD2328__INCLUDED_)
