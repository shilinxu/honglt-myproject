//
// CtrlNetworkHeaderBar.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "CoolInterface.h"
#include "CtrlNetworkHeaderBar.h"
#include "WndNetwork.h"
#include "CoolMenu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CNetworkHeaderBar, CCoolBarCtrl)
BEGIN_MESSAGE_MAP(CNetworkHeaderBar, CCoolBarCtrl)
	//{{AFX_MSG_MAP(CNetworkHeaderBar)
	ON_COMMAND(ID_NETWORK_CHTYPE, OnNetworkChType)
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_MENUSELECT()
	ON_WM_ENTERIDLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CNetworkHeaderBar construction

CNetworkHeaderBar::CNetworkHeaderBar()
{
	m_pCoolMenu	= NULL;
}

CNetworkHeaderBar::~CNetworkHeaderBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkHeaderBar operations

void CNetworkHeaderBar::SetCaption(LPCTSTR pszCaption)
{
	SetWindowText( pszCaption );
	Invalidate();
}

void CNetworkHeaderBar::SetIcon(HICON hIcon)
{
	CWnd::SetIcon( hIcon, FALSE );
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkHeaderBar message handlers

void CNetworkHeaderBar::PrepareRect(CRect* pRect) const
{
	CCoolBarCtrl::PrepareRect( pRect );
	pRect->left -= 10;
	if ( m_czLast.cx < pRect->Width() ) pRect->left = pRect->right - m_czLast.cx;
	pRect->left += 10;
	pRect->bottom --;
}

void CNetworkHeaderBar::DoPaint(CDC* pDC, CRect& rcBar, BOOL bTransparent)
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

void CNetworkHeaderBar::PaintHeader(CDC* pDC, CRect& rcBar, BOOL bTransparent)
{
	CString strCaption;
	GetWindowText( strCaption );
	
	CFont* pOldFont = (CFont*)pDC->SelectObject( &CoolInterface.m_fntBold );
	CSize szText = pDC->GetTextExtent( strCaption );
	
	pDC->SetTextColor( CoolInterface.m_crCmdText );
	pDC->SetBkColor( CoolInterface.m_crMidtone );

	int nMiddle = ( rcBar.top + rcBar.bottom ) / 2;

	if ( bTransparent )
	{
		DrawIconEx( pDC->GetSafeHdc(), rcBar.left + 4, nMiddle - 8,
			GetIcon( FALSE ), 16, 16, 0, CBrush(CoolInterface.m_crMidtone), DI_NORMAL );
		
		pDC->SetBkMode( TRANSPARENT );
		pDC->ExtTextOut( rcBar.left + 22, nMiddle - szText.cy / 2,
			ETO_CLIPPED, &rcBar, strCaption, NULL );
	}
	else
	{
		pDC->SetBkMode( OPAQUE );
		
		DrawIconEx( pDC->GetSafeHdc(), rcBar.left + 4, nMiddle - 8,
			GetIcon( FALSE ), 16, 16, 0, CBrush(CoolInterface.m_crMidtone), DI_NORMAL );
		
		pDC->ExcludeClipRect( rcBar.left + 4, nMiddle - 8, rcBar.left + 20, nMiddle + 8 );
		
		pDC->FillSolidRect( rcBar.left, rcBar.top, 20, rcBar.Height(),
			CoolInterface.m_crMidtone );

		rcBar.left += 20;
		pDC->ExtTextOut( rcBar.left + 2, nMiddle - szText.cy / 2,
			ETO_CLIPPED|ETO_OPAQUE, &rcBar, strCaption, NULL );
		rcBar.left -= 20;
	}

	pDC->SelectObject( pOldFont );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkHeaderBar view menu

void CNetworkHeaderBar::OnNetworkChType()
{
	CMenu pMenu;
	pMenu.LoadMenu( IDR_CHTYPE );
	
	CMenu* pSubMenu = pMenu.GetSubMenu( 0 );
	CNetworkWnd* pNetworkWnd = (CNetworkWnd*)GetParent();
	
	for ( int nPos = 0; nPos < (int)pSubMenu->GetMenuItemCount(); nPos ++ )
	{
		UINT nMenuID = pSubMenu->GetMenuItemID( nPos );
		if ( nMenuID == ID_SEPARATOR) continue;
		
		CString strValue;
		LoadString( strValue, nMenuID );
		LPCTSTR pszName = _tcschr( strValue, '\n' );
		
		strValue = pszName + 1;
		CNetworkChild* pChildCtrl = &pNetworkWnd->m_wndChild;
			
		CXMLElement* pXML = pChildCtrl->FindXMLFirstBy( strValue );
		pSubMenu->EnableMenuItem( nMenuID, pXML ? MF_ENABLED : MF_DISABLED | MF_GRAYED );
		
		BOOL bEmbeded = pChildCtrl->IsEmbededChType( strValue );
		pSubMenu->CheckMenuItem( nMenuID, bEmbeded ? MF_CHECKED : MF_UNCHECKED );
	}
	
	m_pCoolMenu = new CCoolMenu();
	m_pCoolMenu->AddMenu( &pMenu, TRUE );
	
	if ( UINT nMenuID = ThrowMenu( ID_NETWORK_CHTYPE, pSubMenu, this, TRUE, TRUE ) )
	{
		CString strValue;
		LoadString( strValue, nMenuID );
		LPCTSTR pszName = _tcschr( strValue, '\n' );
		
		strValue = pszName + 1;
		CNetworkChild* pChildCtrl = &pNetworkWnd->m_wndChild;
		pChildCtrl->SetFilter( (LPCTSTR)strValue );
		
		if ( CCoolBarItem* pItem = GetID( ID_NETWORK_CHTYPE ) )
		{
			CString strName;
			pSubMenu->GetMenuString( nMenuID, strName, MF_BYCOMMAND );
			
			pItem->SetImage( nMenuID );
			pItem->SetText( strName );
			pItem->SetCheck( TRUE);
		}
	}
	
	delete m_pCoolMenu;
	m_pCoolMenu	= NULL;
}

void CNetworkHeaderBar::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if ( m_pCoolMenu ) m_pCoolMenu->OnMeasureItem( lpMeasureItemStruct );
}

void CNetworkHeaderBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ( m_pCoolMenu ) m_pCoolMenu->OnDrawItem( lpDrawItemStruct );
}

void CNetworkHeaderBar::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	AfxGetMainWnd()->SendMessage( WM_MENUSELECT, MAKELONG( nItemID, nFlags ), (LPARAM)hSysMenu );
}

void CNetworkHeaderBar::OnEnterIdle(UINT nWhy, CWnd* pWho)
{
	AfxGetMainWnd()->SendMessage( WM_ENTERIDLE, (WPARAM)nWhy, (LPARAM)pWho->GetSafeHwnd() );
}

#if 0
//////////////////////////////////////////////////////////////////////
// CNetworkHeaderBar channel suffixes

LPCTSTR CNetworkHeaderBar::m_pszChSuffix[][3] =
{
	{ _T("00"), _T("00"), _T("模拟外线") },
	{ _T("01"), _T("01"), _T("内线电话") },
	{ _T("02"), _T("29"), _T("用户坐席") },
	{ _T("03"), _T("02"), _T("录音通道") },
	{ _T("04"), _T("16"), _T("一号信令") },
	{ _T("05"), _T("31"), _T("传真通道") },
	{ _T("06"), _T("16"), _T("七号信令") },	// TUP
	{ _T("09"), _T("31"), _T("传真通道") },
	{ _T("11"), _T("16"), _T("七号信令") },	// ISUP
};
#endif
