//
// CtrlNetworkThumb.h
//

#if !defined(AFX_CTRLNETWORKTHUMB_H__F22FC277_731A_425E_9130_DAEC1C360010__INCLUDED_)
#define AFX_CTRLNETWORKTHUMB_H__F22FC277_731A_425E_9130_DAEC1C360010__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNetworkThumb window
class CXMLElement;
class CChildWnd;

class CNetworkThumb : public CListBox
{
// Construction
public:
	CNetworkThumb();
	virtual ~CNetworkThumb();
	DECLARE_DYNAMIC(CNetworkThumb)

// Attributes
protected:
	CBrush			m_brWhite;
	CCriticalSection m_pSection;

// Operations
public:
	BOOL			Layout(CXMLElement* pXML);
	CXMLElement*	FindJob(LPCTSTR lpszJob) const;
protected:
	BOOL			CreateJob(CXMLElement* pXML);
	CChildWnd*		FromHandle(CXMLElement* pXML);
protected:
	virtual void	ResetContent();

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkThumb)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkThumb)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnDblClick();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_NETWORK_THUMB		109
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKTHUMB_H__F22FC277_731A_425E_9130_DAEC1C360010__INCLUDED_)
