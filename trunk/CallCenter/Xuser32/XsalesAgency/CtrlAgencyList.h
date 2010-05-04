//
// CtrlAgencyList.h
//

#if !defined(AFX_CTRLAGENCYLIST_H__5B2EB6F2_A461_470A_BF99_3E487E94425D__INCLUDED_)
#define AFX_CTRLAGENCYLIST_H__5B2EB6F2_A461_470A_BF99_3E487E94425D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAgencyList window
class CXMLElement;

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
	_ConnectionPtr	GetConnectionPtr() const;
	CString			GetKey(int nItem) const;
	void			Resync(int nItem);
public:
	BOOL			LoadRecordset(LPCTSTR lpszKey, int nMaxTop = 1024);
	BOOL			LoadRecordset(CXMLElement* pXML);
	BOOL			DeleteAllItems();
protected:
	void			SelectIndex(int nItem);
	int				SeekByPhone(LPCTSTR lpszPhone, int nNextItem = -1, BOOL bFocus = FALSE);
protected:
	BOOL			LoadRecordset(LVITEM* pItem, LPCTSTR lpszField);

// Overrides
public:
	//{{AFX_VIRTUAL(CAgencyList)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAgencyList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblClk(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	friend class CAgencyWnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLAGENCYLIST_H__5B2EB6F2_A461_470A_BF99_3E487E94425D__INCLUDED_)
