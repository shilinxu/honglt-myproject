//
// CtrlIconCombo.h
//

#if !defined(AFX_CTRLICONCOMBO_H__D9310885_8AD2_45D5_AB52_C77C011A36DB__INCLUDED_)
#define AFX_CTRLICONCOMBO_H__D9310885_8AD2_45D5_AB52_C77C011A36DB__INCLUDED_

#pragma once
class CXMLElement;

class CIconComboCtrl : public CComboBox
{
// Construction
public:
	CIconComboCtrl();
	virtual ~CIconComboCtrl();
	
// Attributes
protected:
	
// Operations
public:
	BOOL		LoadXML(CXMLElement* pXML);
	void		ResetContent();
	int			AddString(CXMLElement* pXML);
protected:
	int			GetItemImage(int nItem) const;
	
// Overrides
public:
	//{{AFX_VIRTUAL(CIconComboCtrl)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CIconComboCtrl)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLICONCOMBO_H__D9310885_8AD2_45D5_AB52_C77C011A36DB__INCLUDED_)
