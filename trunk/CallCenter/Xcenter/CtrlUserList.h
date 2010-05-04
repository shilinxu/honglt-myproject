#if !defined(AFX_CTRLUSERLIST_H__C6B15CD6_5966_49E4_ABBD_1615ABCEFB9D__INCLUDED_)
#define AFX_CTRLUSERLIST_H__C6B15CD6_5966_49E4_ABBD_1615ABCEFB9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CtrlUserList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserList window

class CUserList : public CListCtrl
{
// Construction
public:
	CUserList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUserList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUserList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLUSERLIST_H__C6B15CD6_5966_49E4_ABBD_1615ABCEFB9D__INCLUDED_)
