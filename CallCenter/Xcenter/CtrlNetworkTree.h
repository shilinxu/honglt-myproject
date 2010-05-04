//
// CtrlNetworkTree.h
//

#if !defined(AFX_CTRLNETWORKTREE_H__68A8078F_4474_4157_A00E_B05E8C62B825__INCLUDED_)
#define AFX_CTRLNETWORKTREE_H__68A8078F_4474_4157_A00E_B05E8C62B825__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNetworkChild;

/////////////////////////////////////////////////////////////////////////////
// CNetworkTree window

class CNetworkTree : public CTreeCtrl
{
	friend class CNetworkWnd;

// Construction
public:
	CNetworkTree();
	virtual ~CNetworkTree();

	DECLARE_DYNAMIC(CNetworkTree)

// Attributes
protected:

// Operations
public:
	void			SelectFirst();
protected:
	CNetworkChild*	AddItem(int nChType);
	BOOL			RemoveItem(HTREEITEM hItem);
	CNetworkChild*	FindNext(int nChType, HTREEITEM* pItem = NULL) const;
	HTREEITEM		FindNext(CNetworkChild* pChild, HTREEITEM* pItem = NULL) const;
	void			Clear();
private:
	int				GetChString(int nChType, CString& xChType) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkTree)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
//	virtual BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
//	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKTREE_H__68A8078F_4474_4157_A00E_B05E8C62B825__INCLUDED_)
