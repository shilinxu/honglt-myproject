//
// DlgSplash.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "DlgSplash.h"
#include "CoolInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSplashDlg, CDialog)

BEGIN_MESSAGE_MAP(CSplashDlg, CDialog)
	//{{AFX_MSG_MAP(CSplashDlg)
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED		0x80000
#define LWA_ALPHA			0x02
#endif
#define AW_BLEND			0x00080000
#define AW_HIDE				0x00010000

#define SPLASH_WIDTH		397
#define SPLASH_HEIGHT		63

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg construction

CSplashDlg::CSplashDlg() : CDialog( CSplashDlg::IDD, NULL )
{
	CString strVersion;
	strVersion.Format( _T("%i.%i.%i.%i"),
		theApp.m_nVersion[0], theApp.m_nVersion[1], theApp.m_nVersion[2], theApp.m_nVersion[3] );
	
	m_sState	= _T("°æ±¾ ") + strVersion + _T("...");
	
	m_hUser32			= NULL;
	m_pfnAnimateWindow	= NULL;
	
	Create( IDD );
}

CSplashDlg::~CSplashDlg()
{
	if ( m_hUser32 != NULL ) FreeLibrary( m_hUser32 );
}

void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg message handlers

BOOL CSplashDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CClientDC dcScreen( this );
	
	m_bmSplash.LoadBitmap( IDB_PRODUCTS );
	
	m_bmBuffer.CreateCompatibleBitmap( &dcScreen, SPLASH_WIDTH, SPLASH_HEIGHT );
	m_dcBuffer1.CreateCompatibleDC( &dcScreen );
	m_dcBuffer2.CreateCompatibleDC( &dcScreen );
	
	SetWindowPos( NULL, 0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, SWP_NOMOVE );
	CenterWindow();
	
	if ( m_hUser32 = LoadLibrary( _T("User32.dll") ) )
	{
		(FARPROC&)m_pfnAnimateWindow = GetProcAddress( m_hUser32, "AnimateWindow" );
		
		if ( m_pfnAnimateWindow != NULL )
		{
			(*m_pfnAnimateWindow)( GetSafeHwnd(), 250, AW_BLEND );
		}
	}
	
	SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
	UpdateWindow();
	
	return TRUE;
}

void CSplashDlg::Step(LPCTSTR pszText)
{
	m_sState.Format( _T("¼ÓÔØ %s..."), pszText );
	
	CClientDC dc( this );
	DoPaint( &dc );
}

void CSplashDlg::Topmost()
{
	if ( IsWindowVisible() )
	{
		SetWindowPos( &wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
	}
}

void CSplashDlg::Hide()
{
	m_sState = _T("¾ÍÐ÷");
	Invalidate();
	
	if ( m_pfnAnimateWindow != NULL )
	{
		(*m_pfnAnimateWindow)( GetSafeHwnd(), 250, AW_HIDE|AW_BLEND );
	}
	
	DestroyWindow();
	delete this;
}

LONG CSplashDlg::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	LONG lResult = Default();
	
	CDC* pDC = CDC::FromHandle( (HDC)wParam );
	DoPaint( pDC );
	
	return lResult;
}

void CSplashDlg::OnPaint() 
{
	CPaintDC dc( this );
	DoPaint( &dc );
}

void CSplashDlg::DoPaint(CDC* pDC)
{
	CBitmap* pOld1 = (CBitmap*)m_dcBuffer1.SelectObject( &m_bmSplash );
	CBitmap* pOld2 = (CBitmap*)m_dcBuffer2.SelectObject( &m_bmBuffer );
	
	m_dcBuffer2.BitBlt( 0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, &m_dcBuffer1, 0, 0, SRCCOPY );
	
	CFont* pOld3 = (CFont*)m_dcBuffer2.SelectObject( &CoolInterface.m_fntBold );

	m_dcBuffer2.SetBkMode( TRANSPARENT );
	m_dcBuffer2.SetTextColor( RGB( 0, 0, 0 ) );
	
	m_dcBuffer2.FillSolidRect( 0, 50, SPLASH_WIDTH, 16, RGB( 60, 60, 60 ) );
	
	CRect rc( 158, 50, SPLASH_WIDTH, SPLASH_HEIGHT );
	UINT nFormat = DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX;

	rc.OffsetRect( -1, 0 );
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	rc.OffsetRect( 2, 0 );
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	rc.OffsetRect( -1, -1 );
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	rc.OffsetRect( 0, 2 );
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	rc.OffsetRect( 0, -1 );
	
	m_dcBuffer2.SetTextColor( RGB( 255, 255, 255 ) );
	m_dcBuffer2.DrawText( m_sState, &rc, nFormat );
	
	m_dcBuffer2.SelectObject( pOld3 );
	
	pDC->BitBlt( 0, 0, SPLASH_WIDTH, SPLASH_HEIGHT + 16, &m_dcBuffer2, 0, 0, SRCCOPY );
	
	m_dcBuffer2.SelectObject( pOld2 );
	m_dcBuffer1.SelectObject( pOld1 );
}
