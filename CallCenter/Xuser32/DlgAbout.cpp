//
// DlgAbout.cpp
//

#include "stdafx.h"
#include "XUser32.h"
#include "DlgAbout.h"
#include "CoolInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg construction

CAboutDlg::CAboutDlg(CWnd* pParent) : CDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_WEB, m_wndWeb);
	DDX_Control(pDX, IDC_MAIL, m_wndMail);
	DDX_Control(pDX, IDC_TITLE, m_wndTitle);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	WORD nVersion[4];
	theApp.GetVersionNumber( nVersion );
	
	CString strVersion;
	strVersion.Format( _T("%i.%i.%i.%i"),
		nVersion[0], nVersion[1], nVersion[2], nVersion[3] );

	CString strCaption;

	m_wndTitle.GetWindowText( strCaption );
	strCaption += _T(" © ") + strVersion;
	m_wndTitle.SetWindowText( strCaption );

	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );

	LoadRegistry();
	
	return TRUE;
}

void CAboutDlg::OnPaint() 
{
	CPaintDC dc( this );
	CRect rc;
	
	GetClientRect( &rc );
	rc.top += 51;
	
	dc.Draw3dRect( 0, 50, rc.right + 1, 0,
		RGB( 128, 128, 128 ), RGB( 128, 128, 128 ) );
	dc.FillSolidRect( &rc, m_crWhite );
}

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( m_crWhite );
	
	if ( pWnd == &m_wndTitle )
	{
		pDC->SelectObject( &CoolInterface.m_fntBold );
	}
	else if ( pWnd == &m_wndWeb )
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
		pDC->SelectObject( &CoolInterface.m_fntUnder );
	}
	else if ( pWnd == &m_wndMail )
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
		pDC->SelectObject( &CoolInterface.m_fntUnder );
	}
	
	return m_brWhite;
}

BOOL CAboutDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint point;
	CRect rc;

	GetCursorPos( &point );
	m_wndWeb.GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}
	
	m_wndMail.GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}
	
	return CDialog::OnSetCursor( pWnd, nHitTest, message );
}

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonUp( nFlags, point );
	
	CRect rc;
	m_wndWeb.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	if ( rc.PtInRect( point ) )
	{
		ShellExecute( GetSafeHwnd(), _T("open"),
			_T("http://www.96031.com.cn"), NULL, NULL, SW_SHOWNORMAL );
	}
	
	m_wndMail.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	if ( rc.PtInRect( point ) )
	{
		ShellExecute( GetSafeHwnd(), _T("open"),
			_T("mailto:hong_lt@163.com"),
			NULL, NULL, SW_SHOWNORMAL );
	}
}

void CAboutDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRect rc;

	m_wndWeb.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	if ( rc.PtInRect( point ) && ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) )
	{
		DWORD* pNullPtr = (DWORD*)NULL;
		*pNullPtr = 0xFFFFFFFF;
	}
}

void CAboutDlg::LoadRegistry()
{
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	::GetVersionEx ( &versionInfo ); 

	CString strKey, strOwner, strOrganiser;
	DWORD nErrorCode;
	HKEY hKey;

	if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		strKey.Format( _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\") );
	}
	else
	{
		strKey.Format( _T("Software\\Microsoft\\Windows\\CurrentVersion\\") );
	}
	
	nErrorCode = RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hKey );
	
	if ( nErrorCode == ERROR_SUCCESS )
	{
		DWORD nType = 0, nSize = 0;

		nErrorCode = RegQueryValueEx( hKey, _T("RegisteredOwner"), 0, &nType, NULL, &nSize ); 
		
		if ( nErrorCode == ERROR_SUCCESS && nType == REG_SZ && nSize >= sizeof(TCHAR) )
		{
			LPTSTR pszValue = strOwner.GetBuffer( nSize / sizeof(TCHAR) - 1 );
			nErrorCode = RegQueryValueEx( hKey, _T("RegisteredOwner"), 0, &nType, (PBYTE)pszValue, &nSize ); 
			strOwner.ReleaseBuffer( nSize / sizeof(TCHAR) - 1 );
		}

		nErrorCode = RegQueryValueEx( hKey, _T("RegisteredOrganization"), 0, &nType, NULL, &nSize ); 
		
		if ( nErrorCode == ERROR_SUCCESS && nType == REG_SZ && nSize >= sizeof(TCHAR) )
		{
			LPTSTR pszValue = strOrganiser.GetBuffer( nSize / sizeof(TCHAR) - 1 );
			nErrorCode = RegQueryValueEx( hKey, _T("RegisteredOrganization"), 0, &nType, (PBYTE)pszValue, &nSize ); 
			strOrganiser.ReleaseBuffer( nSize / sizeof(TCHAR) - 1 );
		}
		
		RegCloseKey( hKey );

		CString str = _T(' ') + strOwner + _T('\n');
		str += _T(' ') + strOrganiser + _T('\n');
		GetDlgItem(IDC_REGISTRY)->SetWindowText( (LPCTSTR)str );
	}
}
