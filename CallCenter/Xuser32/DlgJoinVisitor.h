//
// DlgJoinVisitor.h
//

#if !defined(AFX_DLGJOINVISITOR_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_)
#define AFX_DLGJOINVISITOR_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlNetworkPanel.h"
class CUserChannel;

class CJoinVisitorDlg	: public CDialog
{
// Construction
public:
	CJoinVisitorDlg(CUserChannel* pChannel);
	virtual ~CJoinVisitorDlg();
	
	DECLARE_DYNAMIC(CJoinVisitorDlg)
	
// Dialog Data
protected:
	//{{AFX_DATA(CJoinVisitorDlg)
	enum { IDD = IDD_JOINVISITOR };
	CStatic			m_wndPlaceHolder;
	CNetworkVideo	m_wndVideo;
	//}}AFX_DATA
	
	COLORREF		m_crWhite;
	CBrush			m_brWhite;
	
// Attributes
protected:
	CUserChannel*	m_pChannel;
	
// Operations
public:
	BOOL	CreateReal(UINT nID);

// Overrides
public:
	//{{AFX_VIRTUAL(CJoinVisitorDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CJoinVisitorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGJOINVISITOR_H__B99F0E55_6587_4EF8_88AD_66787BEE3240__INCLUDED_)
