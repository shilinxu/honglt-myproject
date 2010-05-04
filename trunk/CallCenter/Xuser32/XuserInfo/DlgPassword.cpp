//
// DlgPassword.cpp
//

#include "stdafx.h"
#include "XUser32.h"
#include "DlgPassword.h"
#include "CoolInterface.h"

#include "Network.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
//	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg construction

CPasswordDlg::CPasswordDlg(CWnd* pParent) : CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	//}}AFX_DATA_INIT
	m_bInTimer=FALSE;
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Control(pDX, IDC_USER_PASS0, m_wndPass0);
	DDX_Control(pDX, IDC_USER_PASS1, m_wndPass1);
	DDX_Control(pDX, IDC_USER_PASS2, m_wndPass2);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

BOOL CPasswordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );
	
	m_wndPass0.SetLimitText( 6 );
	m_wndPass0.SetWindowText( "666666" );
	
	m_wndPass1.SetLimitText( 6 );
	m_wndPass2.SetLimitText( 6 );
	
	return TRUE;
}

void CPasswordDlg::OnPaint() 
{
	CPaintDC dc( this );
	CRect rc;
	
	GetClientRect( &rc );
	rc.top += 51;
	
	dc.Draw3dRect( 0, 50, rc.right + 1, 0,
		RGB( 128, 128, 128 ), RGB( 128, 128, 128 ) );
	dc.FillSolidRect( &rc, m_crWhite );
}

HBRUSH CPasswordDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( m_crWhite );
	
	return m_brWhite;
}
#if 0
void CPasswordDlg::OnOK()
{
	if ( m_bInTimer ) return;
	
	m_bInTimer = TRUE;
	SetTimer( 1, 10000, NULL );
	
	::SetCapture( m_hWnd );
	theApp.BeginWaitCursor();

//	CDialog::OnOK();
}
#else
void CPasswordDlg::OnOK()
{
	CString sOldPass;
	m_wndPass0.GetWindowText( sOldPass );
	
	if ( sOldPass.IsEmpty() ) return;
	
	CString sNewPass, strValue;
	m_wndPass1.GetWindowText( sNewPass );
	m_wndPass2.GetWindowText( strValue );
	
	if ( sNewPass.IsEmpty() || sNewPass != strValue ) return;
	
	OnSyncOrderRelationReq( sOldPass, sNewPass );
	
	strValue.Format( "已成功提交\"%s\"号码的密码修改任务, 请重新登录!", Network.m_xUserId );
	AfxMessageBox( strValue );
	
	CDialog::OnOK();
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg common timer

void CPasswordDlg::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent != 1 ) return;
	
	KillTimer( 1 );
	m_bInTimer = FALSE;
	
	theApp.EndWaitCursor();
	
	AfxMessageBox( IDS_ERROR_PASSWORD );
}

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg custom message handlers

void CPasswordDlg::OnSyncOrderRelationReq(LPCTSTR xOldPass, LPCTSTR xNewPass)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "update" );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "xuser_info" );
	
	CString strValue;
	strValue.Format( "set xuserpwd = '%s'", xNewPass );
	
	pSync = pXML->AddElement( "column_name" );
	pSync->SetValue( (LPCTSTR)strValue );
	
	strValue.Format( "xphone = '%s' and xuserpwd = '%s'", Network.m_xUserId, xOldPass );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)strValue );
	
	Network.SendPacket( new CSyncDataView(pXML) );
	delete pXML;
}
