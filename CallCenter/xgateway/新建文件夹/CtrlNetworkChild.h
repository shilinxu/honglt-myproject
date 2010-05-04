//
// CtrlNetworkChild.h
//

#if !defined(AFX_CTRLNETWORKCHILD_H__49120D56_4119_4101_9BDA_4A6E04AEF5E5__INCLUDED_)
#define AFX_CTRLNETWORKCHILD_H__49120D56_4119_4101_9BDA_4A6E04AEF5E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNetworkTree;
class CNetworkVideo;

class CNetworkChild : public CWnd
{
// Construction
public:
	CNetworkChild(LPCTSTR xGroupId = NULL);
	virtual ~CNetworkChild();
	DECLARE_DYNAMIC(CNetworkChild)

// Attributes
public:
	LPTSTR			m_xGroupId;
	CNetworkVideo*	m_pFocus;
	CNetworkTree*	m_pPanel;
protected:
	CPtrList		m_pWindows;
	BOOL			m_bLayout;
	int				m_nColumns;
	CSize			m_szBlock;

// Operations
public:
	CNetworkVideo*	AddChild(CNetworkVideo* pWindow, POSITION posBefore = NULL);
	POSITION		GetChildIterator() const;
	CNetworkVideo*	GetNextChild(POSITION& pos) const;
	int				GetChildCount() const;
	void			RemoveChild(CNetworkVideo* pWindow);
	void			ClearChilds(BOOL bDelete);
public:
	BOOL			IsGroupCompatible(LPCTSTR xGroupId);
	void			SelectTo(CNetworkVideo* pWindow);
	CNetworkTree*	GetPanel() const;
	BOOL			GetHeaderContent(CString& sContent);
protected:
	void			UpdateScroll();
	void			ScrollBy(int nDelta);
	void			ScrollTo(int nDelta);
//	CNetworkVideo*	HitTest(const CPoint& point) const;
protected:
	void		OnChanged();
	void		DoPaint(CDC* pDC, CRect& rcClient, int nScroll);

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkChild)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkChild)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg BOOL OnSelected(CNetworkVideo* pWindow);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_NETWORK_CHILD		101
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKCHILD_H__49120D56_4119_4101_9BDA_4A6E04AEF5E5__INCLUDED_)
