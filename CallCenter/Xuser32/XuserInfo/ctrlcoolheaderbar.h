//
// CtrlCoolHeaderBar.h
//

#if !defined(AFX_CTRLCOOLHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_)
#define AFX_CTRLCOOLHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlCoolBar.h"
class CCoolMenu;

class CCoolHeaderBarCtrl : public CCoolBarCtrl  
{
// Construction
public:
	CCoolHeaderBarCtrl();
	virtual ~CCoolHeaderBarCtrl();

	DECLARE_DYNAMIC(CCoolHeaderBarCtrl)

// Attributes
protected:
	CCoolMenu*		m_pCoolMenu;

// Operations
public:
	void			SetCaption(LPCTSTR pszCaption);
	void			SetIcon(HICON hIcon);
protected:
	virtual void	PrepareRect(CRect* pRect) const;
protected:
	void			PaintHeader(CDC* pDC, CRect& rcBar, BOOL bTransparent);

// Overrides
public:
	//{{AFX_VIRTUAL(CCoolHeaderBarCtrl)
	virtual void DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCoolHeaderBarCtrl)
	afx_msg void OnUserinfoType();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CTRLCOOLHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_)
