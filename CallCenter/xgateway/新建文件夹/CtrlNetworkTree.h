//
// CtrlNetworkTree.h
//

#if !defined(AFX_CTRLNETWORKTREE_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
#define AFX_CTRLNETWORKTREE_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChannel;
class CNetworkVideo;
#include "CtrlNetworkChild.h"

class CNetworkTree : public CTreeCtrl
{
// Construction
public:
	CNetworkTree();
	virtual ~CNetworkTree();
	DECLARE_DYNAMIC(CNetworkTree)

// Attributes
protected:
	
// Operations
public:
	void			Clear();
public:
	int				AddUser(LPCTSTR xUserId);
	int				AddUser(LPCTSTR xUserId, CNetworkVideo*& pxVideo);
	BOOL			GetChildContent(CNetworkChild* pChild, CString& xUserGrp);
protected:
	CNetworkChild*	AddItem(LPCTSTR xUserGrp, LPCTSTR xGroupId);
	int				OnReadGroup(LPCTSTR xUserId, CString& xUserGrp, CString& xGroupId);
	CNetworkChild*	FindItem(LPCTSTR xGroupId) const;
	HTREEITEM		FindItem(CNetworkChild* pChild) const;
	
	virtual BOOL	RemoveItem(HTREEITEM hItem);
	virtual BOOL	RemoveItem(CNetworkChild* pChild);

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkTree)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CNetworkChild;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_NETWORK_TREE	101
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKTREE_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
