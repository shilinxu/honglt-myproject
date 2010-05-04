//
// CtrlCoolHeaderBar.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CoolInterface.h"
#include "CtrlCoolHeaderBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CCoolHeaderBar, CCoolBarCtrl)
BEGIN_MESSAGE_MAP(CCoolHeaderBar, CCoolBarCtrl)
	//{{AFX_MSG_MAP(CCoolHeaderBar)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CCoolHeaderBar construction

CCoolHeaderBar::CCoolHeaderBar()
{
}

CCoolHeaderBar::~CCoolHeaderBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CCoolHeaderBar operations

/////////////////////////////////////////////////////////////////////////////
// CCoolHeaderBar message handlers

void CCoolHeaderBar::DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent)
{
	if ( CCoolBarItem* pItem = GetID(ID_PLAYER_HISTORY) )
	{
		pItem->InflateRect( rcBar.Width() - m_czLast.cx, 0 );
		CalcFixedLayout( FALSE, TRUE );
	}
	
	CCoolBarCtrl::DoPaint( pDC, rcBar, bTransparent );
}

void CCoolHeaderBar::OnTimer(UINT nIDEvent) 
{
	if ( m_pHot && m_pHot->m_nCtrlID == ID_PLAYER_HISTORY ) return;
	
	CCoolBarCtrl::OnTimer( nIDEvent) ;
}
