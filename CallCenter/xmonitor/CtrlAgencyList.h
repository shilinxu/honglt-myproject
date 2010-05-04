//
// CtrlAgencyList.h
//

#if !defined(AFX_CTRLAGENCYLIST_H__5B2EB6F2_A461_470A_BF99_3E487E94425D__INCLUDED_)
#define AFX_CTRLAGENCYLIST_H__5B2EB6F2_A461_470A_BF99_3E487E94425D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace Excel ;

/////////////////////////////////////////////////////////////////////////////
// CAgencyList window

class CAgencyList : public CListCtrl
{
// Construction
public:
	CAgencyList();
	virtual ~CAgencyList();
	
	DECLARE_DYNAMIC(CAgencyList)

// Attributes
protected:
	_RecordsetPtr	m_pRecordset;

// Operations
public:
	void			PrintToWorkbook(_WorkbookPtr pWorkbook);
	BOOL			LoadRecordset(LPCTSTR lpszFilter);
	BOOL			DeleteAllItems();
public:
	int				GetCurIndex(CString& str);
	void			ResyncIndex(int nIndex);
	int				AddNew(LPCTSTR xsaleinfo);
	void			ResyncItemWith(UINT nState, int nIndex);
protected:
	int				SeekByPhone(LPCTSTR lpszPhone);
	void			SelectIndex(int nItem);
	BOOL			RemoveItemOf(UINT nState);
	int				GetSalesIdIndex() const;
protected:
	BOOL			LoadRecordset(LVITEM* pItem, LPCTSTR lpszField);
	_WorksheetPtr	AttachWorkook(SheetsPtr pSheets, LPCTSTR lpszName);

// Overrides
public:
	//{{AFX_VIRTUAL(CAgencyList)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAgencyList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClk(NMHDR* pNotify, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	friend class CAgencyWnd;
	friend class CAgencyXpropertyDlg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLAGENCYLIST_H__5B2EB6F2_A461_470A_BF99_3E487E94425D__INCLUDED_)
