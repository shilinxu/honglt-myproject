//
// CtrlLinerHeaderBar.h
//

#if !defined(AFX_CTRLLINERHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_)
#define AFX_CTRLLINERHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrlcoolbar.h"

class CLinerHeaderBar : public CCoolBarCtrl  
{
// Construction
public:
	CLinerHeaderBar();
	virtual ~CLinerHeaderBar();

	DECLARE_DYNAMIC(CLinerHeaderBar)

// Attributes
protected:
	CString			m_strTitle;

// Operations
public:
	virtual void	SetTitle(LPCTSTR lpszTitle);
protected:
	virtual void	PrepareRect(CRect* pRect) const;
protected:
	void			PaintHeader(CDC* pDC, CRect& rcBar, BOOL bTransparent);

// Overrides
public:
	//{{AFX_VIRTUAL(CLinerHeaderBar)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CLinerHeaderBar)
	virtual void DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CTRLLINERHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_)
