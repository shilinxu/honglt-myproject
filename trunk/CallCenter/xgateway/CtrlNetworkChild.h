//
// CtrlNetworkChild.h
//

#if !defined(AFX_CTRLNETWORKCHILD_H__49120D56_4119_4101_9BDA_4A6E04AEF5E5__INCLUDED_)
#define AFX_CTRLNETWORKCHILD_H__49120D56_4119_4101_9BDA_4A6E04AEF5E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
class CNetworkTree;
class CNetworkVideo;
class CXMLElement;

class CNetworkChild : public CTaskPanel
{
// Construction
public:
	CNetworkChild(CNetworkTree*	pPanel = NULL, LPCTSTR xGroupId = NULL);
	virtual ~CNetworkChild();
	DECLARE_DYNAMIC(CNetworkChild)

// Attributes
public:
	LPTSTR			m_xGroupId;
	CNetworkVideo*	m_pSelected;
	CNetworkTree*	m_pPanel;
protected:
	int				m_nColumns;
	CSize			m_szBlock;
protected:
	CPtrList		m_pList;
	CNetworkVideo*	m_pDefualtMirror;

// Operations
public:
	void			RemoveBox(CTaskBox* pBox);
//	CTaskBox*		AddBox(CTaskBox* pBox, POSITION posBefore = NULL);
public:
	BOOL			IsGroupCompatible(LPCTSTR xGroupId);
	BOOL			GetHeaderContent(CString& sContent);
	CNetworkTree*	GetPanel() const;
protected:
	void			UpdateScroll();
	virtual void	Layout(CRect& rcClient);
protected:
	void			SelectTo(CNetworkVideo* pWindow);
	virtual BOOL	JoinedMirror(CNetworkVideo* pMirror, void (CALLBACK* lpfnTimer)(CNetworkVideo*));
	virtual void	JoinedMirror(CNetworkVideo* pVideo);
	virtual BOOL	LeavedMirror(CNetworkVideo* pVideo);
	virtual HRESULT	DeliverBuffer(BYTE* pBuffer, DWORD nLength, BYTE nType, DWORD nTimestamp, LPVOID lv);
protected:
	virtual BOOL	OnSyncHostTalking(CXMLElement* pXML, CNetworkVideo* pWindow);


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
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CNetworkVideo;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_NETWORK_CHILD		101
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKCHILD_H__49120D56_4119_4101_9BDA_4A6E04AEF5E5__INCLUDED_)
