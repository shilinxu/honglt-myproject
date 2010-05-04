//
// CtrlHomePanel.h
//

#if !defined(AFX_CTRLHOMEPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
#define AFX_CTRLHOMEPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"

class IPacket;

class CHomeTaskBox : public CTaskBox
{
// Construction
public:
	CHomeTaskBox();
	virtual ~CHomeTaskBox();
	DECLARE_DYNAMIC(CHomeTaskBox)

// Attributes
protected:
	CTreeCtrl		m_wndTree;
	
// Operations
public:
	void			DrawChType(int nChType);
	void			Empty();
protected:
	void			SelectFirst();

// Overrides
public:
	//{{AFX_VIRTUAL(CHomeTaskBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CHomeTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelectPage(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CHomePanel;
};

class CHomeListenBox : public CTaskBox
{
// Construction
public:
	CHomeListenBox();
	virtual ~CHomeListenBox();
	DECLARE_DYNAMIC(CHomeListenBox)

// Attributes
protected:
	CComboBox		m_wndChan;
	CComboBox		m_wndRecord;
	CComboBox		m_wndPlay;
	CIconButtonCtrl	m_wndStart;
protected:
	BOOL			m_bPending;
	
// Operations
public:
	IPacket*		GetTask(BOOL bRestart = TRUE);
	void			SetChan(int nChan);

// Overrides
public:
	//{{AFX_VIRTUAL(CHomeListenBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CHomeListenBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CHomePanel;
};

class CHomePanel : public CTaskPanel
{
// Construction
public:
	CHomePanel();
	virtual ~CHomePanel();
	DECLARE_DYNAMIC(CHomePanel)

// Attributes
public:
	CHomeTaskBox	m_wndTask;
	CHomeListenBox	m_wndListen;

// Operations
public:
	void			ExecuteFirst();
	void			ExecuteRedraw(int nChType);
	void			ExecuteClient(int nChType);
//	DWORD			GetNextClient(int nChType, int& nAtFrom) const;

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
#define IDC_HOME_TREE		1001
#define IDC_HOME_CHAN		1003
#define IDC_HOME_RECORD		1004
#define IDC_HOME_PLAY		1005
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLHOMEPANEL_H__089F2565_529B_4BC3_BA2A_7B8BDA135D4B__INCLUDED_)
