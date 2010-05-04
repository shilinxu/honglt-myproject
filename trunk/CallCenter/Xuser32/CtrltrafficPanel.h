//
// CtrlTrafficPanel.h
//

#if !defined(AFX_CTRLTRAFFICPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
#define AFX_CTRLTRAFFICPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"

class CTrafficPanel : public CTaskPanel
{
// Construction
public:
	CTrafficPanel();
	virtual ~CTrafficPanel();
	DECLARE_DYNAMIC(CTrafficPanel)

// Attributes
protected:

// Operations
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CTrafficPanel)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTrafficPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_TRAFFIC_PANEL	101
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLUSERPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
