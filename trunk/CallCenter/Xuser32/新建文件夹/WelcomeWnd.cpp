//
// WelcomeWnd.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CoolInterface.h"

#include "WelcomeWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CWelcomeWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CWelcomeWnd, CChildWnd)
	//{{AFX_MSG_MAP(CUserWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_COMMAND(IDC_USER_LOGIN, OnUsrLogin)
	ON_COMMAND(IDC_USER_LOGOUT, OnUsrLogout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define PANEL_WIDTH		260
#define PANEL_HEIGHT	480

//////////////////////////////////////////////////////////////////////
// CWelcomeWnd construction

CWelcomeWnd::CWelcomeWnd() : CChildWnd(TRUE)
{
	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );
	
	m_nLeftMargin	= 24;
	m_nTopMargin	= 80;
	
	Create( IDR_WELCOMEFRAME );
}

CWelcomeWnd::~CWelcomeWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWelcomeWnd message handlers

int CWelcomeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	CRect rc( 0, 0, 0, 0 );
	
	m_bmHeader.LoadBitmap( IDB_PRODUCTS );
	BITMAP pInfo; m_bmHeader.GetBitmap( &pInfo );
	m_nTopMargin += pInfo.bmHeight;
	
	m_wndUser.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWN|CBS_HASSTRINGS,
		rc, this, IDC_USER_NUMBER );
	m_wndUser.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndPass.Create( WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_PASSWORD,
		rc, this, IDC_USER_PASSWORD );
	m_wndPass.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndSave.Create( "自动保存密码", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX,
		rc, this, IDC_SAVE_PASSWORD );
	m_wndSave.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndLogin.Create( "登录", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON,
		rc, this, IDC_USER_LOGIN );
	m_wndLogin.SetFont( &CoolInterface.m_fntNormal );
	m_wndLogout.Create( "退出", WS_CHILD|WS_VISIBLE|WS_TABSTOP,
		rc, this, IDC_USER_LOGOUT );
	m_wndLogout.SetFont( &CoolInterface.m_fntNormal );

	LoadHistory();
	Layout();
	
	return 0;
}

void CWelcomeWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

int CWelcomeWnd::LoadHistory()
{
	CString strKey, strValue;
	DWORD nErrorCode;
	int nCount = 0;
	HKEY hKey;
	
	strKey.Format( _T("Software\\%s\\%s"), gcszCopyright, theApp.m_pszProfileName );
	nErrorCode = RegOpenKeyEx( HKEY_CURRENT_USER, strKey + _T("\\History"), 0, 0, &hKey );
	
	if ( nErrorCode != ERROR_SUCCESS ) return 0;
	
	TCHAR szName[128];
	for (	nErrorCode = RegEnumKey(hKey, nCount, szName, 128);
			nErrorCode == ERROR_SUCCESS; nCount++ )
	{
		DWORD nType = 0, nSize = 0;
		
		nErrorCode = RegQueryValueEx( hKey, szName, 0, &nType, NULL, &nSize ); 
		
		if ( nErrorCode == ERROR_SUCCESS && nType == REG_SZ && nSize >= sizeof(TCHAR) )
		{
			LPTSTR pszValue = strValue.GetBuffer( nSize / sizeof(TCHAR) - 1 );
			nErrorCode = RegQueryValueEx( hKey, szName, 0, &nType, (PBYTE)pszValue, &nSize ); 
			strValue.ReleaseBuffer( nSize / sizeof(TCHAR) - 1 );
		}
		
		RegCloseKey( hKey );
		
		int nItem = m_wndUser.FindString( -1, strValue );
		if ( nItem != -1 ) m_wndUser.DeleteString( nItem );
		m_wndUser.InsertString( 0, (LPCTSTR)strValue );
	}
	
	return nCount;
}

void CWelcomeWnd::Layout()
{
	TEXTMETRIC txtMetric;

	CDC* pDC = GetDC();
	pDC->SelectObject( &CoolInterface.m_fntNormal );
	pDC->GetTextMetrics( &txtMetric );
	ReleaseDC( pDC );
	
	int nEditHeight = ( txtMetric.tmHeight + txtMetric.tmExternalLeading ) + 6;
	int nButtonHeight = ( txtMetric.tmHeight + txtMetric.tmExternalLeading ) + 10;
	
	CMDIFrameWnd* pFrameWnd = GetMDIFrame();
	pFrameWnd->ModifyStyle( WS_MAXIMIZEBOX|WS_THICKFRAME, 0 );
	
	pFrameWnd->SetWindowPos( NULL, 
						(GetSystemMetrics(SM_CXSCREEN) - PANEL_WIDTH) / 2,
						(GetSystemMetrics(SM_CYSCREEN) - PANEL_HEIGHT) / 2,
						PANEL_WIDTH, PANEL_HEIGHT, 0 );
						
	CRect rc;
	GetClientRect( &rc );
	
	rc.left		+= m_nLeftMargin;
	rc.right	-= m_nLeftMargin;
	rc.top		+= m_nTopMargin + 18;
	rc.bottom	-= m_nTopMargin;
	m_wndUser.MoveWindow( &rc );
	
	rc.top		+= 46;
	rc.bottom	 = rc.top + nEditHeight;
	m_wndPass.MoveWindow( &rc );
	
	rc.top		+= 32;
	rc.right	 = rc.left + 93;
	rc.bottom	 = rc.top + nEditHeight;
	m_wndSave.MoveWindow( &rc );
	
	rc.top		+= 60;
	rc.left		+= 20;
	rc.right	 = rc.left + 76;
	rc.bottom	 = rc.top + nButtonHeight;
	m_wndLogin.MoveWindow( &rc );
	rc.OffsetRect( rc.Width() + 8, 0 );
	m_wndLogout.MoveWindow( &rc );
}

void CWelcomeWnd::OnPaint()
{
	CPaintDC dc( this );
						
	CRect rc;
	GetClientRect( &rc );
	
	PaintHeader( dc, rc );
	
	rc.left		+= m_nLeftMargin;
	rc.right	-= m_nLeftMargin;
	rc.top		+= m_nTopMargin;
	rc.bottom	-= m_nTopMargin;
	
	DoPaint( dc, rc );
	SetFocus();
}

void CWelcomeWnd::DoPaint(CDC& dc, CRect& rc)
{
	UINT nFlags = ETO_CLIPPED;
	CDC* pDC = &dc;
	
	CString strText;
	CSize szText;
	CRect rct;
	
	pDC->SetBkMode( OPAQUE );
	pDC->SetBkColor( m_crWhite );
	nFlags |= ETO_OPAQUE;
	
	CFont* pOldFont = (CFont*)pDC->SelectObject( &CoolInterface.m_fntBold );
	
	pDC->SetTextColor( 0 );
	
	strText	= _T("用户名");
	szText	= pDC->GetTextExtent( strText );
	
	rct.SetRect( rc.left, rc.top, rc.left + szText.cx + 8, rc.top + szText.cy );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, strText, NULL );
	pDC->ExcludeClipRect( &rct );
	
	rc.top  += 46;
	strText	= _T("密码");
	szText	= pDC->GetTextExtent( strText );
	
	rct.SetRect( rc.left, rc.top, rc.left + szText.cx + 8, rc.top + szText.cy );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, strText, NULL );
	pDC->ExcludeClipRect( &rct );
	
	pDC->SelectObject( pOldFont );
}

void CWelcomeWnd::PaintHeader(CDC& dc, CRect& rc)
{
	BITMAP pInfo;
	m_bmHeader.GetBitmap( &pInfo );

	CDC mdc;
	mdc.CreateCompatibleDC( &dc );
	CBitmap* pOldBitmap = (CBitmap*)mdc.SelectObject( &m_bmHeader );
	dc.BitBlt( rc.left, rc.top, pInfo.bmWidth, pInfo.bmHeight, &mdc, 0, 0, SRCCOPY );
	mdc.SelectObject( pOldBitmap );
	mdc.DeleteDC();
	
	dc.Draw3dRect( 0, pInfo.bmHeight, rc.right + 1, 0,
		RGB( 128, 128, 128 ), RGB( 128, 128, 128 ) );
}

HBRUSH CWelcomeWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CChildWnd::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( m_crWhite );
	
	return m_brWhite;
}

BOOL CWelcomeWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect( &rc );
	
	pDC->FillSolidRect( &rc, CCoolInterface::GetDialogBkColor() );
	
	return TRUE;
}

void CWelcomeWnd::OnUsrLogin()
{
}

void CWelcomeWnd::OnUsrLogout()
{
	GetMDIFrame()->PostMessage( WM_CLOSE );
}
