//
// CtrlHomeForm.h
//

#if !defined(AFX_CTRLHOMEFORM_H__D2EFCE64_6CC6_4F0A_868E_9262412F4891__INCLUDED_)
#define AFX_CTRLHOMEFORM_H__D2EFCE64_6CC6_4F0A_868E_9262412F4891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHomeForm : public CDialog
{
// Construction
public:
	CHomeForm();
	virtual ~CHomeForm();
	DECLARE_DYNAMIC(CHomeForm)

// Form Data
public:
	//{{AFX_DATA(CHomeForm)
	enum { IDD = IDD_HOME_FORM };
	CComboBox	m_wndUserId;
	CEdit		m_wndPassword;
	CButton		m_wndSaveing;
	//}}AFX_DATA
	
// Attributes
protected:
	CString		m_strCaption;
	CBrush		m_brWhite;
	
// Operations
public:
	BOOL		LoadDefaultCaption();
	BOOL		Create(const RECT& rect, CWnd* pParentWnd);
protected:
	int			LoadHistory();
	void		SaveHistory();
protected:
	int			InsertItem(int nIndex, LPCTSTR xUserId, DWORD nData);
	void		RemoveItem(int nItem);
public:
	inline LPCTSTR GetTemplateName() const
	{
		return m_lpszTemplateName;
	}
	
// Overrides
public:
	//{{AFX_VIRTUAL(CHomeForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CHomeForm)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelChangeUserId();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	friend class CHomeWnd;
};

#endif // !defined(AFX_CTRLHOMEFORM_H__D2EFCE64_6CC6_4F0A_868E_9262412F4891__INCLUDED_)
