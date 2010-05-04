#if !defined(AFX_CTRLICONCOMBO_H__D9310885_8AD2_45D5_AB52_C77C011A36DB__INCLUDED_)
#define AFX_CTRLICONCOMBO_H__D9310885_8AD2_45D5_AB52_C77C011A36DB__INCLUDED_

#pragma once

class CIconComboCtrl : public CComboBox
{
// Construction
public:
	CIconComboCtrl();
	virtual ~CIconComboCtrl();
	
// Attributes
public:
	struct Item
	{
		int		Image;
		DWORD	Param;
	};
protected:
	CImageList*	m_pImages;
	
// Operations
public:
	int		AddItem(UINT nID, int nImage, DWORD dwParam = NULL, BOOL bFocus = TRUE);
	int		AddItem(LPCTSTR pszTitle, int nImage, DWORD dwParam = NULL, BOOL bFocus = TRUE);
	void	RemoveItem(int nItem);
	void	Clear();
public:
	virtual int  SetCurSel(int nItem);
	virtual BOOL SetImageList(CImageList* pImageList);
	
// Overrides
public:
	//{{AFX_VIRTUAL(CIconComboCtrl)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CIconComboCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLICONCOMBO_H__D9310885_8AD2_45D5_AB52_C77C011A36DB__INCLUDED_)
