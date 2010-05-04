//
// CtrlCoolHeaderBar.h
//

#if !defined(AFX_CTRLCOOLHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_)
#define AFX_CTRLCOOLHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrlcoolbar.h"

class CCoolHeaderBar : public CCoolBarCtrl  
{
// Construction
public:
	CCoolHeaderBar();
	virtual ~CCoolHeaderBar();

	DECLARE_DYNAMIC(CCoolHeaderBar)

// Attributes
protected:

// Operations
protected:

// Overrides
public:
	//{{AFX_VIRTUAL(CCoolHeaderBar)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCoolHeaderBar)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CTRLCOOLHEADERBAR_H__22ACBAAD_EEAC_4B17_9D11_8B32C40960E3__INCLUDED_)
