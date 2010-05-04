//
// CtrlNetworkTreeBar.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "CoolInterface.h"
#include "CtrlNetworkTreeBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CNetworkTreeBar, CCoolBarCtrl)

BEGIN_MESSAGE_MAP(CNetworkTreeBar, CCoolBarCtrl)
	//{{AFX_MSG_MAP(CNetworkTreeBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CNetworkTreeBar construction

CNetworkTreeBar::CNetworkTreeBar()
{
}

CNetworkTreeBar::~CNetworkTreeBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkTreeBar message handlers

void CNetworkTreeBar::PrepareRect(CRect* pRect) const
{
	CCoolBarCtrl::PrepareRect( pRect );
	pRect->left -= 10;
	if ( m_czLast.cx < pRect->Width() ) pRect->left = pRect->right - m_czLast.cx;
	pRect->left += 10;
	pRect->bottom --;
}

void CNetworkTreeBar::DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent)
{
	pDC->FillSolidRect( rcBar.left, rcBar.bottom - 1, rcBar.Width(), 1,
		GetSysColor( COLOR_3DSHADOW ) );
	rcBar.bottom --;

	if ( m_czLast.cx < rcBar.Width() - 22 )
	{
		CRect rcHeader( &rcBar );
		rcHeader.right = rcBar.left = rcBar.right - m_czLast.cx;
		PaintHeader( pDC, rcHeader, bTransparent );
	}

	CCoolBarCtrl::DoPaint( pDC, rcBar, bTransparent );
}

void CNetworkTreeBar::PaintHeader(CDC* pDC, CRect& rcBar, BOOL bTransparent)
{
	LPCTSTR xTitle = _T("当前在线用户群");
	CFont* pOldFont = (CFont*)pDC->SelectObject( &CoolInterface.m_fntBold );
	
	CSize szText = pDC->GetTextExtent( xTitle );
	
	pDC->SetTextColor( CoolInterface.m_crCmdText );
	pDC->SetBkColor( CoolInterface.m_crMidtone );

	int nMiddle = ( rcBar.top + rcBar.bottom ) / 2;

	if ( bTransparent )
	{
		ImageList_DrawEx( CoolInterface.m_pImages, 1, pDC->GetSafeHdc(),
			rcBar.left + 4, nMiddle - 8, 16, 16, CLR_NONE, CLR_NONE, ILD_NORMAL );
		
		pDC->SetBkMode( TRANSPARENT );
		pDC->ExtTextOut( rcBar.left + 22, nMiddle - szText.cy / 2,
			ETO_CLIPPED, &rcBar, xTitle, NULL );
	}
	else
	{
		pDC->SetBkMode( OPAQUE );
		
		ImageList_DrawEx( CoolInterface.m_pImages, 1, pDC->GetSafeHdc(),
			rcBar.left + 4, nMiddle - 8, 16, 16, CoolInterface.m_crMidtone, CLR_NONE, ILD_NORMAL );
		
		pDC->ExcludeClipRect( rcBar.left + 4, nMiddle - 8, rcBar.left + 20, nMiddle + 8 );
		
		pDC->FillSolidRect( rcBar.left, rcBar.top, 20, rcBar.Height(),
			CoolInterface.m_crMidtone );

		rcBar.left += 20;
		pDC->ExtTextOut( rcBar.left + 2, nMiddle - szText.cy / 2,
			ETO_CLIPPED|ETO_OPAQUE, &rcBar, xTitle, NULL );
		rcBar.left -= 20;
	}

	pDC->SelectObject( pOldFont );
}
