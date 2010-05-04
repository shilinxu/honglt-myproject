//
// CtrlNetworkHeaderBar.h
//

#if !defined(AFX_CTRLNETWORKHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_)
#define AFX_CTRLNETWORKHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlCoolBar.h"
class CCoolMenu;

class CNetworkHeaderBar : public CCoolBarCtrl  
{
// Construction
public:
	CNetworkHeaderBar();
	virtual ~CNetworkHeaderBar();

	DECLARE_DYNAMIC(CNetworkHeaderBar)

// Attributes
public:
	int			m_nChType;
protected:
	CCoolMenu*	m_pCoolMenu;

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
	//{{AFX_VIRTUAL(CNetworkHeaderBar)
	virtual void DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkHeaderBar)
	afx_msg void OnNetworkChType();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);

	DECLARE_MESSAGE_MAP()
		
// Channel Suffixes
//protected:
//	static LPCTSTR m_pszChSuffix[][3];
};

#endif // !defined(AFX_CTRLNETWORKHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_)
