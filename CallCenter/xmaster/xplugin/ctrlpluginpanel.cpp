//
// CtrlPluginPanel.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Settings.h"
#include "CoolInterface.h"
#include "CtrlPluginPanel.h"
#include "DlgSettingsManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CPluginPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CPluginPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CPluginPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CPluginTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CPluginTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CPluginTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	10

/////////////////////////////////////////////////////////////////////////////
// CPluginPanel construction

CPluginPanel::CPluginPanel()
{
}

CPluginPanel::~CPluginPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPluginPanel message handlers

BOOL CPluginPanel::Create(CWnd* pParentWnd)
{
	return CTaskPanel::Create( WS_VISIBLE, CRect(0,0,0,0), pParentWnd, IDC_PLUGIN_PANEL );
}

int CPluginPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndTask.Create( this, 96, _T("任务"), IDI_TASKPANEL );
	
	AddBox( &m_wndTask );
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPluginTaskBox construction

CPluginTaskBox::CPluginTaskBox()
{
	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );
}

CPluginTaskBox::~CPluginTaskBox()
{
	if ( m_brWhite.m_hObject )
	{
		m_brWhite.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPluginTaskBox message handlers

int CPluginTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndUsrTime.Create( "", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndUsrTime.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndPayTime.Create( "", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndPayTime.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndSetting.Create( "编辑扣费参数", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndSetting.SetFont( &CoolInterface.m_fntNormal );
	
	UpdateSettings();
	
	SetPrimary( TRUE );
	
	return 0;
}

void CPluginTaskBox::OnDestroy()
{
}

void CPluginTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 3 );
	
	DeferWindowPos( hDWP, m_wndUsrTime, NULL, BOX_MARGIN + 80, BOX_MARGIN,
		cx - BOX_MARGIN * 2 - 80, 16, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndPayTime, NULL, BOX_MARGIN + 80, BOX_MARGIN + 22,
		cx - BOX_MARGIN * 2 - 80, 16, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndSetting, NULL, BOX_MARGIN + 30, BOX_MARGIN + 46,
		cx - BOX_MARGIN * 2 - 8, 16, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CPluginTaskBox::OnPaint() 
{
	CPaintDC dc( this );
	CRect rc, rct;
	CString str;
	
	UINT nFlags = ETO_CLIPPED;
	CDC* pDC = &dc;
	
	GetClientRect( &rc );
	
	if ( m_bmWatermark.m_hObject != NULL )
	{
		pDC = CoolInterface.GetBuffer( dc, rc.Size() );
		CoolInterface.DrawWatermark( pDC, &rc, &m_bmWatermark );
		pDC->SetBkMode( TRANSPARENT );
	}
	else
	{
		pDC->SetBkMode( OPAQUE );
		pDC->SetBkColor( m_crWhite );
		nFlags |= ETO_OPAQUE;
	}
	
	CFont* pOldFont = (CFont*)pDC->SelectObject( &CoolInterface.m_fntNormal );
	
	pDC->SetTextColor( 0 );
	
	rct.SetRect( BOX_MARGIN, BOX_MARGIN, rc.right - BOX_MARGIN, BOX_MARGIN + 16 );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "月费下发规则:", NULL );
	pDC->ExcludeClipRect( &rct );

	rct.OffsetRect( 0, 31 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "余额提醒规则:", NULL );
	pDC->ExcludeClipRect( &rct );
	
	pDC->SelectObject( pOldFont );
	
	if ( pDC != &dc )
	{
		dc.BitBlt( 0, 0, rc.Width(), rc.Height(), pDC, 0, 0, SRCCOPY );
		pDC->SelectClipRgn( NULL );
	}
	else
	{
		pDC->FillSolidRect( &rc, m_crWhite );
	}
}

HBRUSH CPluginTaskBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CTaskBox::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( m_crWhite );
	
	if ( pWnd == &m_wndUsrTime || pWnd == &m_wndPayTime )
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
		//pDC->SelectObject( &CoolInterface.m_fntBold );
	}
	else
	if ( pWnd == &m_wndSetting )
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
		pDC->SelectObject( &CoolInterface.m_fntUnder );
	}

	return m_brWhite;
}

BOOL CPluginTaskBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint point;
	CRect rc;

	GetCursorPos( &point );
	m_wndSetting.GetWindowRect( &rc );

	if ( rc.PtInRect( point ) )
	{
		SetCursor( theApp.LoadCursor( IDC_HAND ) );
		return TRUE;
	}
	
	return CTaskBox::OnSetCursor( pWnd, nHitTest, message );
}

void CPluginTaskBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTaskBox::OnLButtonUp( nFlags, point );
	
	CRect rc;
	m_wndSetting.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	if ( rc.PtInRect( point ) )
	{
		if ( CSettingsManagerDlg::Run( _T("CBlockSettingsPage") ) )
		{
			UpdateSettings();
		}
	}
}

void CPluginTaskBox::UpdateSettings()
{
	CString strValue;

	strValue.Format( "%i日 && %s",
					Settings.Block.UsrDate,
					Settings.Block.UsrTime );
	
	m_wndUsrTime.SetWindowText( (LPCTSTR)strValue );
	
	strValue.Format( "%s && %s",
					Settings.Payer.PayStart,
					Settings.Payer.PayClose );
	
	m_wndPayTime.SetWindowText( (LPCTSTR)strValue );
}
