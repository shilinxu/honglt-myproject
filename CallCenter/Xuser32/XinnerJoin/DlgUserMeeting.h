//
// DlgUserMeeting.h
//

#if !defined(AFX_DLGUSERMEETING_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_)
#define AFX_DLGUSERMEETING_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CUserMeetingDlg
class CGraphRender;

class CUserMeetingDlg : public CDialog
{
// Construction
public:
	CUserMeetingDlg(CWnd* pParent = NULL);   // standard constructor
	~CUserMeetingDlg();

// Dialog Data
public:
	//{{AFX_DATA(CUserMeetingDlg)
	enum { IDD = IDD_USER_MEETING };
	CComboBox	m_wndGroup;
	CListCtrl	m_wndList;
	CButton		m_wndMember;
	//}}AFX_DATA
	
	COLORREF	m_crWhite;
	CBrush		m_brWhite;
private:
	CXMLElement*	m_pXML;
	CGraphRender*	m_pRenderer;

// Operations
protected:
	BOOL			OnSyncDataViewResp(CXMLElement* pXML);
protected:
	BOOL			CreateMenuGroup(CXMLElement* pXML, HMENU hParent);
	void			PutIntoPhone(HMENU hMenu, int nPos, int nCount);
	void			PutIntoPhone(HMENU hMenu, UINT nMenuId);
	void			PutIntoPhone(LPCTSTR lpszPhone, LPCTSTR lpszName);
private:
	HMENU			FindMenuGroup(HMENU hParent, LPCTSTR lpszMenu);

// Overrides
public:
	//{{AFX_VIRTUAL(CUserMeetingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUserMeetingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent) ;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMyGroup();
	afx_msg void OnCancel();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGUSERMEETING_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_)
