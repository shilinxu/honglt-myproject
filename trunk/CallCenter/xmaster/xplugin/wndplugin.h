//
// WndPlugin.h
//

#if !defined(AFX_WNDPLUGIN_H__791A8CFB_7390_4C6F_B51F_4BE66CDAFCF7__INCLUDED_)
#define AFX_WNDPLUGIN_H__791A8CFB_7390_4C6F_B51F_4BE66CDAFCF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlPluginPanel.h"
class CTransfer;

/////////////////////////////////////////////////////////////////////////////
// CPluginWnd window

class CPluginWnd : public CChildWnd
{
// Construction
public:
	CPluginWnd();
	virtual ~CPluginWnd();
	
	DECLARE_SERIAL(CPluginWnd)

// Attributes
public:
	CPluginPanel	m_wndPanel;
	CListCtrl		m_wndList;
protected:
	DWORD			m_tUserSub;
	DWORD			m_tUserPay;
	DWORD			m_tUserSmSj;
protected:
	HANDLE				m_hThread;
	volatile BOOL		m_bThread;
	CEvent				m_pWakeup;
	CCriticalSection	m_pSection;

// Operations
public:
	BOOL			ReadUserSub();
	BOOL			ReadUserPay();
	BOOL			ReadUserSmSj();
protected:
	int				AddItem(LPCTSTR xPhone, LPCTSTR xFeeCode, LPCTSTR xContent, DWORD nState);
	void			SelectItem(int nItem);
protected:
	BOOL			ReadUserPay(DWORD tCurrChk);
	BOOL			ReadUserSmSj_add();
	BOOL			ReadUserSmSj_remove();
	BOOL			ReadUserSub(LPCTSTR xFeeCode, LPCTSTR xFeeType, BOOL bRefresh = FALSE);
	BOOL			ReadUserVox(LPCTSTR xFeeCode, double nExpense);
protected:
//	BOOL			InSubElapse(LPCTSTR xSubId, COleDateTime xSubDate, int nElapse);
	BOOL			WriteBalance(LPCTSTR xUserId, double nExpense);
//	BOOL			WriteBalance(_RecordsetPtr pRecordset, double nExpense);
//	BOOL			AddExpense(LPCTSTR xUserId, LPCTSTR xSubId);
	BOOL			GoPayRecord(LPCTSTR xUserId);
	BOOL			BuildPayRecord(LPCTSTR xPhone);
	BOOL			BuildPagerRec(LPCTSTR xUserId);
	BOOL			CheckInDate(COleDateTime dt, COleDateTime& et) const;
	BOOL			UnsubscibeUser(LPCTSTR xUserId, LPCTSTR xFeeCode);
private:
	BOOL			ReadFeeType(LPCTSTR xFeeType, int* pnElapse, double* pnExpense) const;
	BOOL			CheckInDate(COleDateTime dt, int nElapse) const;
//	BOOL			CheckInDate(COleDateTime dt) const;
	BOOL			IsLeapYear(int nYear) const;
	BOOL			IsInSubRule(int nSubId, double& nGiven) const;
private:
	BOOL			GetUserId_Of(LPCTSTR xPhone, CString& xUserId);
	BOOL			GetPhoneNum_Of(LPCTSTR xUserId, CString& xPhoneNum);
	BOOL			ChkAndUserCost(LPCTSTR xUserId, int nSubId, double nGiven);
	BOOL			ChkUserSubc(LPCTSTR xPhoneNum, int nSubId);

// Thread
protected:
	void			StartThread();
	void			StopThread();
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
protected:
	void			CloseThread(HANDLE* phThread, LPCTSTR pszName);

// Overrides
public:
	//{{AFX_VIRTUAL(CPluginWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPluginWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LONG OnNextTry(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define	IDC_PLUGIN_LIST		1001
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDPLUGIN_H__791A8CFB_7390_4C6F_B51F_4BE66CDAFCF7__INCLUDED_)
