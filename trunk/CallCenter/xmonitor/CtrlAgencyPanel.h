//
// CtrlAgencyPanel.h
//

#if !defined(AFX_CTRLAGENCYPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
#define AFX_CTRLAGENCYPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"

class CAgencyTaskBox : public CTaskBox
{
// Construction
public:
	CAgencyTaskBox();
	virtual ~CAgencyTaskBox();
	DECLARE_DYNAMIC(CAgencyTaskBox)

// Attributes
protected:
	CComboBox		m_wndFamily;
	CComboBox		m_wndFilter;
	CIconButtonCtrl	m_wndStart;
	CIconButtonCtrl	m_wndClear;
protected:
	_RecordsetPtr	m_pRecordset;
	
// Operations
public:
	BOOL		GetSearch(CString& strFilter) const;
protected:
	BOOL		LoadRecordset(CComboBox* pCombo);
	int			SeekByJobId(LPCTSTR xJobId);

// Overrides
public:
	//{{AFX_VIRTUAL(CAgencyTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAgencyTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnDropdownFamily();
	afx_msg void OnCloseupFamily();
	afx_msg void OnSearch();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	friend class CAgencyPanel;
};

class CAgencyPanel : public CTaskPanel
{
// Construction
public:
	CAgencyPanel();
	virtual ~CAgencyPanel();
	DECLARE_DYNAMIC(CAgencyPanel)

// Attributes
protected:
	CAgencyTaskBox		m_wndTask;

// Operations
public:
	void		ExecuteDefaultSearch(LPCTSTR xsaleinfo = NULL);
	void		ExecuteSearch();
	void		ExecuteClear();
public:
	BOOL		GetSearch(CString& strFilter) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CAgencyPanel)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAgencyPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_AGENCY_PANEL	101
#define IDC_AGENCY_FAMILY	1001
#define IDC_AGENCY_FILTER	1002
#define IDC_AGENCY_SEARCH	1004
#define IDC_AGENCY_CLEAR	1005
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLAGENCYPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
