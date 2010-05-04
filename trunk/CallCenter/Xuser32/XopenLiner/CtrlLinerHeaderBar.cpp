//
// CtrlLinerHeaderBar.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CoolInterface.h"
#include "CtrlLinerHeaderBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CLinerHeaderBar, CCoolBarCtrl)
BEGIN_MESSAGE_MAP(CLinerHeaderBar, CCoolBarCtrl)
	//{{AFX_MSG_MAP(CLinerHeaderBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CLinerHeaderBar construction

CLinerHeaderBar::CLinerHeaderBar()
{
}

CLinerHeaderBar::~CLinerHeaderBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLinerHeaderBar operations

void CLinerHeaderBar::SetTitle(LPCTSTR lpszTitle)
{
	if ( m_strTitle.CompareNoCase(lpszTitle) )
	{
		m_strTitle = lpszTitle;
		if (m_hWnd) Invalidate();
	}
	HICON	m_hIcon = (HICON)LoadImage( AfxGetResourceHandle(),
			MAKEINTRESOURCE( IDI_CONNECT ), IMAGE_ICON, 16, 16, 0 );
		CWnd::SetIcon( m_hIcon, FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CLinerHeaderBar message handlers

void CLinerHeaderBar::PrepareRect(CRect* pRect) const
{
	CCoolBarCtrl::PrepareRect( pRect );
	pRect->left -= 10;
	if ( m_czLast.cx < pRect->Width() ) pRect->left = pRect->right - m_czLast.cx;
	pRect->left += 10;
	pRect->bottom --;
}

void CLinerHeaderBar::DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent)
{
	pDC->FillSolidRect( rcBar.left, rcBar.bottom - 1, rcBar.Width(), 1,
		GetSysColor( COLOR_3DSHADOW ) );
	rcBar.bottom --;

	if ( m_czLast.cx < rcBar.Width() - 22 )
	{
		CRect rcHeader( &rcBar );
		rcHeader.right = rcBar.left = rcBar.right - m_czLast.cx;
//		PaintHeader( pDC, rcHeader, bTransparent );
	}

	CCoolBarCtrl::DoPaint( pDC, rcBar, bTransparent );
}

void CLinerHeaderBar::PaintHeader(CDC* pDC, CRect& rcBar, BOOL bTransparent)
{
	LPCTSTR xTitle = m_strTitle;
	CFont* pOldFont = (CFont*)pDC->SelectObject( &CoolInterface.m_fntBold );
	
	CSize szText = pDC->GetTextExtent( xTitle );
	
	pDC->SetTextColor( CoolInterface.m_crCmdText );
	pDC->SetBkColor( CoolInterface.m_crMidtone );

	int nMiddle = ( rcBar.top + rcBar.bottom ) / 2;

	if ( bTransparent )
	{
		ImageList_DrawEx( CoolInterface.m_pImages, 22, pDC->GetSafeHdc(),
			rcBar.left + 4, nMiddle - 8, 16, 16, CLR_NONE, CLR_NONE, ILD_NORMAL );
		
		pDC->SetBkMode( TRANSPARENT );
		pDC->ExtTextOut( rcBar.left + 22, nMiddle - szText.cy / 2,
			ETO_CLIPPED, &rcBar, xTitle, NULL );
	}
	else
	{
		pDC->SetBkMode( OPAQUE );
		
		ImageList_DrawEx( CoolInterface.m_pImages, 22, pDC->GetSafeHdc(),
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
