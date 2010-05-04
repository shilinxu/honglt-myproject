//
// CtrlNetworkTreeBar.h
//

#if !defined(AFX_CTRLNETWORKTREEBAR_H__6AA67DA1_818E_46A8_87BC_8688DBCA5925__INCLUDED_)
#define AFX_CTRLNETWORKTREEBAR_H__6AA67DA1_818E_46A8_87BC_8688DBCA5925__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlCoolBar.h"

class CNetworkTreeBar : public CCoolBarCtrl
{
// Construction
public:
	CNetworkTreeBar();
	virtual ~CNetworkTreeBar();

	DECLARE_DYNAMIC(CNetworkTreeBar)

// Attributes
protected:

// Operations
protected:
	virtual void	PrepareRect(CRect* pRect) const;
protected:
	void			PaintHeader(CDC* pDC, CRect& rcBar, BOOL bTransparent);

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkTreeBar)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkTreeBar)
	virtual void DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CTRLNETWORKTREEBAR_H__6AA67DA1_818E_46A8_87BC_8688DBCA5925__INCLUDED_)
