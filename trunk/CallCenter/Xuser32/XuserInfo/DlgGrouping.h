//
// DlgGrouping.h
//

#if !defined(AFX_DLGGROUPING_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_)
#define AFX_DLGGROUPING_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_

#pragma once
class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CGroupingDlg

class CItemEdit : public CEdit
{
// Construction
public:
	CItemEdit();

// Attributes
public:
	CPoint		point;
	
// Implementation
protected:
	//{{AFX_MSG(CItemEdit)
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

class CGroupingDlg : public CDialog
{
// Construction
public:
	CGroupingDlg(CWnd* pParent = NULL);   // standard constructor
	~CGroupingDlg();

// Dialog Data
public:
	//{{AFX_DATA(CGroupingDlg)
	enum { IDD = IDD_USER_GROUP };
	CComboBox	m_wndGroup;
	CListCtrl	m_wndList;
	//}}AFX_DATA
	
	COLORREF	m_crWhite;
	CBrush		m_brWhite;
	CItemEdit	m_pEdit;
private:
	CXMLElement*	m_pXML;
	CXMLElement*	m_pXMLMethod;
	LVHITTESTINFO	m_pHitInfo;

// Operations
protected:
	BOOL			CreateGroup(LPCTSTR lpszName, CXMLElement* pXML);
	void			AddMethod(CXMLElement* pXML, LPCTSTR pszMethod, CXMLElement* pXMLSwap = NULL);
protected:
	BOOL			ProcessMethod(CXMLElement*& pXML);
	BOOL			ProcessInsert(CXMLElement* pXML, LPCTSTR lpszMethod = "insert into");
	BOOL			ProcessUpdate(CXMLElement* pParent, LPCTSTR lpszMethod = "update");
	BOOL			ProcessDelete(CXMLElement* pParent, LPCTSTR lpszMethod = "delete");
protected:
	virtual void	OnSyncOrderRelation();	
	virtual BOOL	OnSyncDataViewResp(CXMLElement* pXML);
protected:
	void			FormatMapping(CXMLElement* pXML, CString& str);
	void			FormatUpdate(CXMLElement* pXML, CString& str);
private:
	BOOL			IsNotField(LPCTSTR lpszField) const;
	BOOL			IsNumField(LPCTSTR lpszField) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CGroupingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CGroupingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelChangeGroup();
	afx_msg void OnBeginLabelEditList(LV_DISPINFO* pNotify, LRESULT* pResult);
	afx_msg void OnEndLabelEditList(LV_DISPINFO* pNotify, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGROUPING_H__068D89FF_4098_4092_B2E4_818D4867F112__INCLUDED_)
