//
// CtrlPluginList.h
//

#if !defined(AFX_CTRLPLUGINLIST_H__B851684C_A103_44F3_808E_15AE5EA58092__INCLUDED_)
#define AFX_CTRLPLUGINLIST_H__B851684C_A103_44F3_808E_15AE5EA58092__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPluginList window

class CPluginList : public CListCtrl
{
// Construction
public:
	CPluginList();
	virtual ~CPluginList();

	DECLARE_DYNAMIC(CPluginList)

// Attributes
protected:

// Operations
protected:
	int				Print(int nType, LPCTSTR xPhone, LPCTSTR xFeeCode, LPCTSTR xContent);
	void			SelectItem(int nItem);
private:
	COLORREF		GetChColour(DWORD nMaskState) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CPluginList)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPluginList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMLVCUSTOMDRAW* pNotify, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
	friend class CPluginWnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLPLUGINLIST_H__B851684C_A103_44F3_808E_15AE5EA58092__INCLUDED_)
