//
// XfaceTestDlg.cpp
//

#include "stdafx.h"
#include "XfaceTest.h"
#include "XfaceTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CXfaceTestDlg, CDialog)
	//{{AFX_MSG_MAP(CXfaceTestDlg)
	ON_WM_CLOSE()
	ON_COMMAND(IDC_CONNECT, OnOK)
	ON_COMMAND(IDC_DISCONNECT, OnCancel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SHEET, OnSelChangeSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define	WM_TIMERA	(WM_USER + 1)

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg construction

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CXfaceTestDlg construction

CXfaceTestDlg::CXfaceTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXfaceTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXfaceTestDlg)
	m_pPage			= NULL;
	//}}AFX_DATA_INIT
	
	CreateReal( IDD );
}

void CXfaceTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXfaceTestDlg)
	DDX_Control(pDX, IDC_PORT, m_wndPort);
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_USER_ID, m_wndUserId);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
	DDX_Control(pDX, IDC_SHEET, m_wndSheet);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CXfaceTestDlg operations

BOOL CXfaceTestDlg::CreateReal(UINT nID)
{
	LPCTSTR lpszTemplateName = MAKEINTRESOURCE( nID );

	HINSTANCE hInst		= AfxFindResourceHandle( lpszTemplateName, RT_DIALOG );
	HRSRC hResource		= ::FindResource( hInst, lpszTemplateName, RT_DIALOG );
	HGLOBAL hTemplate	= LoadResource( hInst, hResource );

	LPCDLGTEMPLATE lpDialogTemplate = (LPCDLGTEMPLATE)LockResource( hTemplate );

	BOOL bResult = CreateDlgIndirect( lpDialogTemplate, NULL, hInst );

	UnlockResource( hTemplate );

	FreeResource( hTemplate );

	return bResult;
}

BOOL CXfaceTestDlg::SetActivePage(CSettingsPage* pPage)
{
	if ( pPage == NULL || pPage == m_pPage ) return FALSE;

	ASSERT_KINDOF(CSettingsPage, pPage);

	if ( m_pPage != NULL )
	{
		if ( ! m_pPage->OnKillActive() ) return FALSE;
		m_pPage->ShowWindow( SW_HIDE );
	}
	
	if ( ! pPage->OnSetActive() ) return FALSE;
	
	m_pPage = pPage;
	m_pPage->ShowWindow( SW_SHOW );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXfaceTestDlg message handlers

BOOL CXfaceTestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_wndHost.SetWindowText( "192.168.0.18" );
	m_wndPort.SetWindowText( "9980" );
	
	m_wndUserId.SetWindowText( "057496031" );
	m_wndPassword.SetWindowText( "666666" );
	
	TC_ITEM pItem = {0};
	pItem.mask = TCIF_TEXT | TCIF_PARAM;
	
	pItem.pszText	= m_pGeneral.m_sCaption.GetBuffer(0);
	pItem.lParam	= (LPARAM)&m_pGeneral;
	m_wndSheet.InsertItem( 0, &pItem );
	
	pItem.pszText	= m_pChat.m_sCaption.GetBuffer(0);
	pItem.lParam	= (LPARAM)&m_pChat;
	m_wndSheet.InsertItem( 1, &pItem );
	
	CRect rect;
	m_wndSheet.GetWindowRect( &rect );
	
	rect.DeflateRect( 0, 0, 8, 31 );
	
	m_pGeneral.Create( rect, this );
	m_pChat.Create( rect, this );
	
	m_wndSheet.SetCurSel( 0 );
	OnSelChangeSheet( NULL, NULL );
	
	return TRUE;
}

void CXfaceTestDlg::OnClose() 
{
	XDisconnect();
	DestroyWindow();
}

void CXfaceTestDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

void CXfaceTestDlg::OnSelChangeSheet(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TCITEM pItem;
	pItem.mask = TCIF_PARAM;
	
	int nItem = m_wndSheet.GetCurSel();
	if ( ! m_wndSheet.GetItem(nItem, &pItem) ) return;
	
	CSettingsPage* pPage = (CSettingsPage*)pItem.lParam;
	SetActivePage( pPage );
}

void CXfaceTestDlg::OnOK()
{
	CString strHost;
	m_wndHost.GetWindowText( strHost );
	
	CString strValue;
	m_wndPort.GetWindowText( strValue );
	
	CString strUserId;
	m_wndUserId.GetWindowText( strUserId );
	
	int nPort = _ttoi( strValue );
	if ( int nResult = XConnect( strHost, nPort ) )
	{
		CString str;
		str.Format( "用户[%s]试图登录->[%s:%i], 返回代码:=[%i]", strUserId, strHost, nPort, nResult );
		
		AfxMessageBox( str ); return;
	}
	
	m_wndPassword.GetWindowText( strValue );
	
	int bTestor = ((CButton*)GetDlgItem(IDC_TESTOR))->GetCheck();
	int nResult = XLoginHost( strUserId, strValue, bTestor );
	
	CString str;
	str.Format( "用户[%s]试图登录->[%s:%i], 返回代码:=[%i]", strUserId, strHost, nPort, nResult );
	
	AfxMessageBox( str );
	
	ASSERT( m_pChat.GetSafeHwnd() );
	ASSERT_KINDOF( CChatTesterPage, &m_pChat );
	m_pChat.SetTimer( WM_TIMERA, 100, CChatTesterPage::TimerProcA );
}

void CXfaceTestDlg::OnCancel()
{
	ASSERT( m_pChat.GetSafeHwnd() );
	ASSERT_KINDOF( CChatTesterPage, &m_pChat );
	m_pChat.KillTimer( WM_TIMERA );
	
	XDisconnect();
	
	m_pPage->Invalidate();
}
