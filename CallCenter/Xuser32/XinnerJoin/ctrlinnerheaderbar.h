//
// CtrlInnerHeaderBar.h
//

#if !defined(AFX_CTRLINNERHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_)
#define AFX_CTRLINNERHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrlcoolbar.h"

class CInnerHeaderBar : public CCoolBarCtrl  
{
// Construction
public:
	CInnerHeaderBar();
	virtual ~CInnerHeaderBar();

	DECLARE_DYNAMIC(CInnerHeaderBar)

// Attributes
protected:
	CString			m_strTitle;
	COLORREF		m_crCmdText;

// Operations
public:
	void			SetCaption(LPCTSTR pszCaption, COLORREF crCmdText = GetSysColor( COLOR_MENUTEXT ));
	void			SetIcon(HICON hIcon);
protected:
	virtual void	PrepareRect(CRect* pRect) const;
protected:
	void			PaintHeader(CDC* pDC, CRect& rcBar, BOOL bTransparent);

// Overrides
public:
	//{{AFX_VIRTUAL(CInnerHeaderBar)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CInnerHeaderBar)
	virtual void DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CTRLINNERHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_)
