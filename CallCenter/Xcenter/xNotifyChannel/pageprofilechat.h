//
// PageProfileChat.h
//

#if !defined(AFX_PAGEPROFILECHAT_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
#define AFX_PAGEPROFILECHAT_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsPage.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CChatProfilePage dialog

class CChatProfilePage : public CSettingsPage
{
// Construction
public:
	CChatProfilePage(CXMLElement* pXML = NULL);
	virtual ~CChatProfilePage();

	DECLARE_DYNCREATE(CChatProfilePage)

// Dialog Data
public:
	//{{AFX_DATA(CChatProfilePage)
	enum { IDD = IDD_PROFILE_CHAT };
	CComboBox	m_wndRoomId;
	CComboBox	m_wndMember;
	CComboBox	m_wndHearer;
	CComboBox	m_wndOwner;
	CButton		m_wndDelete;
	CEdit		m_wndDesc;
	CListCtrl	m_wndList;
	CButton		m_wndExtAdd;
	CButton		m_wndExtRemove;
	CEdit		m_wndMeetGrp;
	CButton		m_wndMeetOly;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML();
	void			FormatComboBox(LPCTSTR lpszText, CComboBox& pCombo);
	void			FormatString(CString& strText, CComboBox& pCombo);

// Overrides
public:
	//{{AFX_VIRTUAL(CChatProfilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CChatProfilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditChangeRoomOwner();
	afx_msg void OnSelChangeRoomOwner();
	afx_msg void OnOwnerAdd();
	afx_msg void OnOwnerRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEPROFILECHAT_H__B3A00FB3_4601_472E_8233_30F16E3CCC4D__INCLUDED_)
