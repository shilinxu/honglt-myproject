//
// DlgQueueProperties.h
//

#if !defined(AFX_DLGCHATPROPERTIES_H__930ED48C_23D4_4BA3_8F4C_66B115DE5083__INCLUDED_)
#define AFX_DLGCHATPROPERTIES_H__930ED48C_23D4_4BA3_8F4C_66B115DE5083__INCLUDED_

#pragma once
class CXMLElement;

class CChatPropertiesDlg : public CDialog
{
// Construction
public:
	CChatPropertiesDlg(CXMLElement* pXML, CWnd* pParent = NULL);

// Dialog Data
public:
	//{{AFX_DATA(CChatPropertiesDlg)
	enum { IDD = IDD_CHAT_PROPERTIES };
	CListCtrl	m_wndList;
	CComboBox	m_wndRoomId;
	CComboBox	m_wndMember;
	CComboBox	m_wndAuditor;
	CComboBox	m_wndOwner;
	CEdit		m_wndMeetGrp;
	CButton		m_wndMeetOly;
	CButton		m_wndDelete;
	CEdit		m_wndDesc;
	CButton		m_wndExtAdd;
	CButton		m_wndExtRemove;
	//}}AFX_DATA
protected:
	CXMLElement*	m_pXML;

// Operations
protected:
	BOOL			LoadXML(CXMLElement* pXML);
	void			FormatComboBox(LPCTSTR lpszText, CComboBox& pCombo);
	void			FormatString(CString& strText, CComboBox& pCombo);

// Overrides
public:
	//{{AFX_VIRTUAL(CChatPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CChatPropertiesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditChangeRoomOwner();
	afx_msg void OnSelChangeRoomOwner();
	afx_msg void OnOwnerAdd();
	afx_msg void OnOwnerRemove();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGCHATPROPERTIES_H__930ED48C_23D4_4BA3_8F4C_66B115DE5083__INCLUDED_)
