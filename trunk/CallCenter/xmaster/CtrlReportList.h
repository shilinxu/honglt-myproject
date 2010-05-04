//
// CtrlReportList.h
//

#if !defined(AFX_CTRLREPORTLIST_H__BC9C68BE_8BA6_4DDB_8FC1_6D72F85BCB4D__INCLUDED_)
#define AFX_CTRLREPORTLIST_H__BC9C68BE_8BA6_4DDB_8FC1_6D72F85BCB4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CReportListCtrl window

class CReportListCtrl : public CListCtrl
{
// Construction
public:
	CReportListCtrl();
	virtual ~CReportListCtrl();
	
	DECLARE_DYNAMIC(CReportListCtrl)

// Attributes
protected:
	_RecordsetPtr	m_pRecordset;

// Operations
public:
	int		LoadRecordset(LPCTSTR pszFilter, int* pnIgn = NULL);
	void	ClearWindow();
protected:
	int		LoadRecord(int nItem);

// Overrides
public:
	//{{AFX_VIRTUAL(CReportListCtrl)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CReportListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnCustomDraw(NMLVCUSTOMDRAW* pNotify, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLREPORTLIST_H__BC9C68BE_8BA6_4DDB_8FC1_6D72F85BCB4D__INCLUDED_)
