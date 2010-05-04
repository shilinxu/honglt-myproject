//
// CtrlUserPanel.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "CoolInterface.h"
#include "CtrlUserPanel.h"
#include "Settings.h"
#include "XML.h"

#include "WndUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CUserPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CUserPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CUserPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CUserTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CUserTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CUserTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CUserFluxBox, CTaskBox)
BEGIN_MESSAGE_MAP(CUserFluxBox, CTaskBox)
	//{{AFX_MSG_MAP(CUserFluxBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_FLUX_LIST, OnCustomDrawList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	10

/////////////////////////////////////////////////////////////////////////////
// CUserPanel construction

CUserPanel::CUserPanel()
{
}

CUserPanel::~CUserPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CUserPanel message handlers

BOOL CUserPanel::Create(CWnd* pParentWnd)
{
	return CTaskPanel::Create( WS_VISIBLE, CRect(0,0,0,0), pParentWnd, IDC_USER_PANEL );
}

int CUserPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndTask.Create( this, 92, _T("任务"), IDI_TASKPANEL );
	m_wndFlux.Create( this, 92, _T("当月排行榜"), IDI_TASKPANEL );
	
	SetStretchBox( &m_wndFlux );
	
	AddBox( &m_wndTask );
	AddBox( &m_wndFlux );
	
	return 0;
}

void CUserPanel::ExecuteReport()
{
	CUserWnd* pWnd = (CUserWnd*)GetParent();
	
	int nCount = 0;
	int nAlived = pWnd->IsWellAlived( &nCount );

	m_wndTask.UpdateReport( nCount, nAlived );
}

/////////////////////////////////////////////////////////////////////////////
// CUserTaskBox construction

CUserTaskBox::CUserTaskBox()
{
	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );
}

CUserTaskBox::~CUserTaskBox()
{
	if ( m_brWhite.m_hObject )
	{
		m_brWhite.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CUserTaskBox message handlers

int CUserTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;

	CRect rc( 0, 0, 0, 0 );
	
	m_wndReady.Create( "0", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndReady.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndNavigate.Create( "0", WS_CHILD|WS_VISIBLE, rc, this );
	m_wndNavigate.SetFont( &CoolInterface.m_fntNormal );
	
	SetPrimary( TRUE );
	SetTimer( 5, 1000, NULL );
	PostMessage( WM_TIMER, 5 );
	
	return 0;
}

void CUserTaskBox::OnDestroy()
{
	KillTimer( 5 );
}

void CUserTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 2 );
	
	DeferWindowPos( hDWP, m_wndReady, NULL, BOX_MARGIN + 106, BOX_MARGIN,
		cx - BOX_MARGIN * 2 - 106, 24, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndNavigate, NULL, BOX_MARGIN + 106, BOX_MARGIN + 22,
		cx - BOX_MARGIN * 2 - 106, 24, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CUserTaskBox::OnPaint() 
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
	
	rct.SetRect( BOX_MARGIN, BOX_MARGIN + 1, rc.right - BOX_MARGIN, BOX_MARGIN + 16 );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "当前在线坐席数:", NULL );
	pDC->ExcludeClipRect( &rct );

	rct.OffsetRect( 0, 31 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "当前活跃坐席数:", NULL );
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

HBRUSH CUserTaskBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CTaskBox::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( m_crWhite );
	
	if ( pWnd == &m_wndReady || pWnd == &m_wndNavigate )
	{
		pDC->SetTextColor( RGB( 0, 0, 255 ) );
		pDC->SelectObject( &CoolInterface.m_fntBold );
	}
	
	return m_brWhite;
}

void CUserTaskBox::OnTimer(UINT nIDEvent) 
{
	CUserPanel* pPanel = (CUserPanel*)GetPanel();
	pPanel->ExecuteReport();

	CTaskBox::OnTimer(nIDEvent);
}

void CUserTaskBox::UpdateReport(int nReady, int nNavigate)
{
	CString strValue;

	m_wndReady.GetWindowText( strValue );
	if ( _ttoi( strValue ) != nReady )
	{
		strValue.Format( "%i", nReady );
		m_wndReady.SetWindowText( strValue );
	}

	m_wndNavigate.GetWindowText( strValue );
	if ( _ttoi( strValue ) != nNavigate )
	{
		strValue.Format( "%i", nNavigate );
		m_wndNavigate.SetWindowText( strValue );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CUserFluxBox construction

CUserFluxBox::CUserFluxBox()
{
	CHisReport::m_strPath  = Settings.General.Path;
	CHisReport::m_strPath += _T("\\DATA");
	
	m_pXMLMonth	= NULL;
	m_pXMLToday	= NULL;
}

CUserFluxBox::~CUserFluxBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CUserFluxBox message handlers

BOOL CUserFluxBox::Create(CTaskPanel* pPanel, int nHeight, LPCTSTR pszCaption, UINT nIDIcon)
{
	return CTaskBox::Create(pPanel, nHeight, pszCaption, nIDIcon);
}

int CUserFluxBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOLABELWRAP|WS_VSCROLL|WS_TABSTOP|LVS_SHOWSELALWAYS,
		rc, this, IDC_FLUX_LIST );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT/*|LVS_EX_HEADERDRAGDROP*/ );
	
	m_wndList.InsertColumn( 0, _T("工号"), LVCFMT_LEFT, 72 );
	m_wndList.InsertColumn( 1, _T("呼入"), LVCFMT_LEFT, 40 );
	m_wndList.InsertColumn( 2, _T("呼出"), LVCFMT_LEFT, 40 );
	
	SetTimer( 5, 1000, NULL );
	PostMessage( WM_TIMER, 5 );
	
	return 0;
}

void CUserFluxBox::OnDestroy()
{
	KillTimer( 5 );
}

void CUserFluxBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );

	CRect rc; GetClientRect( &rc );
	rc.top += 1;
	
	HDWP hDWP = BeginDeferWindowPos( 1 );
	
	DeferWindowPos( hDWP, m_wndList.GetSafeHwnd(), NULL,
		rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CUserFluxBox::OnPaint() 
{
	CPaintDC dc( this );
	CRect rc;
	
	GetClientRect( &rc );
	
	dc.Draw3dRect( 0, 0, rc.right, 1,
		RGB( 128, 128, 128 ), RGB( 128, 128, 128 ) );
}

void CUserFluxBox::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	
	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		DWORD dwExisting	= m_wndList.GetItemState( pDraw->nmcd.dwItemSpec, 0xFFFF );
		BOOL bSelected		= ( dwExisting & LVIS_SELECTED ) ? TRUE : FALSE;
		
		if ( ! bSelected == TRUE ) pDraw->clrText = RGB( 34, 93, 217 );

		*pResult = CDRF_DODEFAULT;
	}
	else
	{
		*pResult = 0;
	}
}

void CUserFluxBox::OnTimer(UINT nIDEvent) 
{
	if ( CheckOut() ) InitReport();
	
//Register( "hong","In" );
	
	CTaskBox::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// CUserFluxBox History Report

BOOL CUserFluxBox::LoadReport()
{
	if ( ! CHisReport::LoadReport() ) return FALSE;
	m_wndList.DeleteAllItems();
	
	ASSERT( m_pXML != NULL );
	ASSERT( m_pXML->IsNamed( _T("History") ) );
	
	for ( POSITION pos = m_pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = m_pXML->GetNextElement( pos );
		if ( ! pXML->IsNamed( _T("History") ) ) continue;
		
		Register( pXML, _T("In") );
		Register( pXML, _T("Out") );
	}
	
	return TRUE;
}

void CUserFluxBox::Register(CXMLElement* pParent, LPCTSTR pszPart)
{
	for ( POSITION pos = pParent->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pParent->GetNextElement( pos );
		if ( ! pXML->IsNamed( _T("Map") ) ) continue;
		
		CString xUserId = pXML->GetAttributeValue( _T("ID") );
		CString strValue= pXML->GetAttributeValue( pszPart );
		
		Register( xUserId, pszPart, _ttoi(strValue) );
	}
}

void CUserFluxBox::Register(LPCTSTR xUser, LPCTSTR xPart, int nValue)
{
	CString strValue;
	
	for ( int nItem = m_wndList.GetItemCount() - 1; nItem >= 0; nItem -- )
	{
		strValue = m_wndList.GetItemText( nItem, 0 );
		if ( ! strValue.CompareNoCase( xUser ) ) break;
	}
	
	if ( nItem == -1 )
	{
		nItem = m_wndList.InsertItem( m_wndList.GetItemCount(), xUser );
	}
	
	int nSubItem = ( ! _tcsicmp( xPart, _T("In") ) ) ? 1 : 2;
	
	CString xOldValue = m_wndList.GetItemText( nItem, nSubItem );
	
	strValue.Format( "%i", nValue + _ttoi(xOldValue) );
	m_wndList.SetItemText( nItem, nSubItem, strValue );
}

void CUserFluxBox::Register(LPCTSTR xUser, LPCTSTR xPart)
{
	CString strValue;
	WORD nYear, nMonth, nDay, nHour;
	nHour	= WORD( m_tLastChk % 24 );
	nDay	= WORD( m_tLastChk / 24 % 31 + 1 );
	nMonth	= WORD( m_tLastChk / ( 31 * 24 ) ) % 12 + 1;
	nYear	= WORD( m_tLastChk / ( 12 * 31 * 24 ) + 1 );
	
	// Register date
	CHisReport::Register( 1 );
	
	// Find date
	for ( POSITION pos = m_pXML->GetElementIterator(), posNext = pos; pos; pos = posNext )
	{
		CXMLElement* pXML = m_pXML->GetNextElement( posNext );
		if ( ! pXML->IsNamed( _T("History") ) ) continue;
		
		WORD nYear2, nMonth2, nDay2;
		nYear2 = nMonth2 = nDay2 = 0;
		
		strValue = pXML->GetAttributeValue( _T("Date") );
		_stscanf( strValue, _T("%d年%d月%d日"), &nYear2, &nMonth2, &nDay2 );

		if ( nYear == nYear2 && nMonth == nMonth2 && nDay == nDay2 ) break;
	}
	
	ASSERT( pos != NULL );
	CXMLElement* pXML = m_pXML->GetNextElement( pos );
	
	// Find user's map
	for ( pos = pXML->GetElementIterator(), posNext = pos; pos; pos = posNext )
	{
		CXMLElement* pMap = pXML->GetNextElement( posNext );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		strValue = pMap->GetAttributeValue( _T("ID") );
		if ( ! strValue.CompareNoCase( xUser ) ) break;
	}
	
	if ( pos )
	{
		pXML = pXML->GetNextElement( pos );
	}
	else
	{
		pXML = pXML->AddElement( _T("Map") );
		pXML->AddAttribute( _T("ID"), xUser );
	}
	
	CXMLAttribute* pAttri = pXML->GetAttribute( xPart );
	if ( ! pAttri ) pAttri = pXML->AddAttribute( xPart );
	strValue = pAttri->GetValue(); LONG nValue = _ttol( strValue );
	strValue.Format( _T("%lu"), nValue + 1 ); pAttri->SetValue( strValue );
	
	Register( xUser, xPart, 1 );
}
