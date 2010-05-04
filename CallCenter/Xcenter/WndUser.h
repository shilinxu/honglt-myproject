//
// WndUser.h
//

#if !defined(AFX_WNDUSER_H__0FF9B39A_8AFA_4F90_AE21_FE3F821D11ED__INCLUDED_)
#define AFX_WNDUSER_H__0FF9B39A_8AFA_4F90_AE21_FE3F821D11ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlUserPanel.h"

class CUserChannel;
class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CUserWnd window

class CUserWnd : public CChildWnd
{
	friend class CUserChannel2;
	
// Construction
public:
	CUserWnd();
	virtual ~CUserWnd();
	
	DECLARE_SERIAL(CUserWnd)

// Attributes
public:
	CUserPanel		m_wndPanel;
	CListCtrl		m_wndList;
protected:
	int				m_nRetained;
	BOOL			m_bInTimer;

// Operations
public:
	CChannel*		ReadItem(LPCTSTR xJob, int nChState = -1);
	int				IsWellAlived(int* pnCount = NULL) const;
	CUserFluxBox*	GetFluxBox();
protected:
	int				AddItem(CUserChannel* pChannel);
	BOOL			RemoveItem(CUserChannel* pChannel);
	int				FindItem(CChannel* pChannel) const;
	BOOL			SetItem(CChannel* pChannel, int nChState);
	BOOL			SetColour(CChannel* pChannel, int nChState);
protected:
	BOOL			RingNow(CChannel* pChannel, int nChState = S_CALL_RINGING);
	BOOL			CompleteTask(CChannel* pChannel);
	virtual BOOL	UpdateItem(int nItem, CChannel* pChannel);
	virtual BOOL	RecordItem(CChannel* pChannel);
private:
	COLORREF		GetChColour(DWORD nChState) const;
	BOOL			IsPermitCalled(IN_ADDR* pAddress, CXMLElement** ppXML = NULL) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CUserWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUserWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define	IDC_USER_CHILD	1001
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDUSER_H__0FF9B39A_8AFA_4F90_AE21_FE3F821D11ED__INCLUDED_)
