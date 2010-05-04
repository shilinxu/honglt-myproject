//
// CtrlNetworkPanel.h
//

#if !defined(AFX_CTRLNETWORKPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
#define AFX_CTRLNETWORKPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlTaskPanel.h"
#include "CtrlIconButton.h"
#include "EDClock.h"

class CNetworkTaskBox : public CTaskBox
{
// Construction
public:
	CNetworkTaskBox();
	virtual ~CNetworkTaskBox();
	DECLARE_DYNAMIC(CNetworkTaskBox)
		
// Attributes
protected:
	CComboBox		m_wndChan;
	CComboBox		m_wndPlay;
	CIconButtonCtrl	m_wndStart;
protected:
	BOOL			m_bPending;
	
// Operations
public:
	
// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkTaskBox)
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	//{{AFX_MSG(CNetworkTaskBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CChannel;
class CXMLElement;
class CNetworkMonitorBox : public CTaskBox
{
// Construction
public:
	CNetworkMonitorBox();
	virtual ~CNetworkMonitorBox();
	DECLARE_DYNAMIC(CNetworkMonitorBox)

// Attributes
protected:
	CEDClock		m_wndClock;
	CIconButtonCtrl	m_wndStart;
protected:
	CMap<CXMLElement*, CXMLElement*, CChannel*, CChannel*>	m_pList;
	
// Operations
protected:
	BOOL			ProcessXML(HANDLE hThread, POSITION posNext);
	BOOL			ProcessXML(POSITION posNext);
	BOOL			ClearProcess(POSITION posNext);
protected:
	BOOL			AlarmMachine();
	BOOL			RestartMachine(LPCTSTR lpszAction);
	BOOL			AlarmToShortMessage(LPCTSTR lpszHost);
	BOOL			AlarmToShortMessage(LPCTSTR lpszHost, CXMLElement* pXML);

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkMonitorBox)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkMonitorBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg LONG OnTimerOut(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CNetworkPanel : public CTaskPanel
{
// Construction
public:
	CNetworkPanel();
	virtual ~CNetworkPanel();
	DECLARE_DYNAMIC(CNetworkPanel)

// Attributes
protected:
	CNetworkTaskBox	m_wndTask;
	CNetworkMonitorBox	m_wndMonitor;

// Operations
public:
	void		ExecuteMonitor();

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkPanel)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_NETWORK_PANEL		101
#define IDC_NETWORK_CHAN		1003
#define IDC_NETWORK_PLAY		1005
#define IDC_NETWORK_CLOCK		1007
#define IDC_NETWORK_ACTION		1008
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKPANEL_H__C7FD016F_7EEA_4D8F_80F3_73BB808AA1C3__INCLUDED_)
