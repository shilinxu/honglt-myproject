//
// CtrlHomePanel.h
//

#if !defined(AFX_CTRLHOMEPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
#define AFX_CTRLHOMEPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
class CVideoCapture;

class CHomeVideoBox : public CTaskBox
{
// Construction
public:
	CHomeVideoBox();
	virtual ~CHomeVideoBox();
	DECLARE_DYNAMIC(CHomeVideoBox)

// Attributes
protected:
	
// Operations
public:

// Overrides
public:
	//{{AFX_VIRTUAL(CHomeVideoBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CHomeVideoBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CHomePanel : public CTaskPanel
{
// Construction
public:
	CHomePanel();
	virtual ~CHomePanel();
	DECLARE_DYNAMIC(CHomePanel)

// Attributes
protected:
	CHomeVideoBox	m_wndVideo;

// Operations
public:
	void		StartPreview(CVideoCapture* pCapture);

// Overrides
public:
	//{{AFX_VIRTUAL(CHomePanel)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CHomePanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_HOME_PANEL		101
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLHOMEPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
