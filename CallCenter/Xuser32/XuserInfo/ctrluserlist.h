//
// CtrlUserList.h
//

#if !defined(AFX_CTRLUSERLIST_H__5B2EB6F2_A461_470A_BF99_3E487E94425D__INCLUDED_)
#define AFX_CTRLUSERLIST_H__5B2EB6F2_A461_470A_BF99_3E487E94425D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CUserList window
class CXMLElement;

class CUserList : public CListCtrl
{
// Construction
public:
	CUserList();
	virtual ~CUserList();
	
	DECLARE_DYNAMIC(CUserList)

// Attributes
protected:

// Operations
public:
	CString			GetTempFile(int nItem) const;
	TRISTATE		ExistTempFile(LPCTSTR lpszFile) const;
protected:
	BOOL			LoadRecordset(CXMLElement* pXML);

// Overrides
public:
	//{{AFX_VIRTUAL(CUserList)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUserList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblClk(NMHDR* pNotify, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMLVCUSTOMDRAW* pNotify, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	friend class CUserWnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLUSERLIST_H__5B2EB6F2_A461_470A_BF99_3E487E94425D__INCLUDED_)
