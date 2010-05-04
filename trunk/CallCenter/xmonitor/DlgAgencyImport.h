//
// DlgAgencyImport.h
//

#if !defined(AFX_DLGAGENCYIMPORT_H__3DDF5DE7_3CE5_4F07_B6BD_FD1E0F42F0BB__INCLUDED_)
#define AFX_DLGAGENCYIMPORT_H__3DDF5DE7_3CE5_4F07_B6BD_FD1E0F42F0BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace Excel ;
/////////////////////////////////////////////////////////////////////////////
// CAgencyImportDlg dialog

class CAgencyImportDlg : public CDialog
{
// Construction
public:
	CAgencyImportDlg(LPCTSTR);
	virtual ~CAgencyImportDlg();

// Dialog Data
protected:
	//{{AFX_DATA(CAgencyImportDlg)
	enum { IDD = IDD_AGENCY_IMPORT };
	CEdit	m_wndPath;
	//}}AFX_DATA
	
// Attributes
protected:
	LPTSTR			m_xsaleinfo;

// Operations
protected:
	virtual BOOL	ImportFromExcel(LPCTSTR lpszFile);
	void			ImportFromWorksheet(_RecordsetPtr xRecordset, _WorksheetPtr pWorksheet);
protected:
	int				GetSalesIdIndex() const;

// Overrides
	//{{AFX_VIRTUAL(CAgencyImportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAgencyImportDlg)
	afx_msg void OnPathBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAGENCYIMPORT_H__3DDF5DE7_3CE5_4F07_B6BD_FD1E0F42F0BB__INCLUDED_)
