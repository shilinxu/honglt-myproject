//
// WndNetwork.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndNetwork.h"
#include "CtrlNetworkPanel.h"
#include "CoolInterface.h"
#include "Skin.h"
#include "XML.h"

#include "Network.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CNetworkWnd, CChildWnd)

BEGIN_MESSAGE_MAP(CNetworkWnd, CChildWnd)
	//{{AFX_MSG_MAP(CNetworkWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28

//////////////////////////////////////////////////////////////////////
// CNetworkWnd construction

CNetworkWnd::CNetworkWnd() : CChildWnd(TRUE)
{
	Create( IDR_HOMEFRAME );
}

CNetworkWnd::~CNetworkWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd message handlers

int CNetworkWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel

	wndPanel.Create( WS_VISIBLE, rectDefault, this );
	
	// Header
	
	if ( ! m_wndHeaderBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndHeaderBar.SetBarStyle( m_wndHeaderBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	Skin.CreateToolBar( _T("CNetworkWnd.Header"), &m_wndHeaderBar );
	
	m_wndHeaderBar.SetOwner( GetOwner() );
	m_wndHeaderBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	
	// Thumb
	
	m_wndThumb.Create( WS_VISIBLE, rectDefault, this );
	
	OnSyncOrderRelation();
	
	return 0;
}

void CNetworkWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CNetworkWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );
	
	if ( rc.Width() < 32 || rc.Height() < 32 ) return;
	
	HDWP hDWP = BeginDeferWindowPos( 3 );
	
	DeferWindowPos( hDWP, wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndHeaderBar.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndThumb.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top + BAR_HEIGHT, rc.Width() - PANEL_WIDTH,
		rc.Height() - BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CNetworkWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	if ( bActivate )
	{
		wndPanel.SetParent( this );
		OnSize( SIZE_INTERNAL, 0, 0 );
	}
	
	CChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd common timer

void CNetworkWnd::OnSyncOrderRelation()
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncOrderRelation );
	pXML->AddAttribute( "method", "home" );
	
	Network.SendPacket( new CSyncOrderRelation(pXML) );
	delete pXML; pXML = NULL;
	
	if ( ! Network.CallWorker( &pXML ) )
	{
		OnSyncOrderRelation( pXML );
		pXML->Delete();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkWnd custom message handlers

BOOL CNetworkWnd::OnSyncOrderRelation(CXMLElement* pXML)
{
	CString strMethod = pXML->GetAttributeValue("method");
	
	if ( ! strMethod.CompareNoCase("home") )
	{
		return OnSyncOrderRelation( pXML, 0 );
	}
	
	return FALSE;
}

BOOL CNetworkWnd::OnSyncOrderRelation(CXMLElement* pXML, DWORD)
{
	CString strValue = pXML->GetAttributeValue( "text" );
	ASSERT( strValue.IsEmpty() == FALSE );
/*	
	if ( CXMLAttribute* pAttri = pXML->GetAttribute( "xphone") )
	{
		m_wndHeaderBar.SetCaption( strValue + " - [" + pAttri->GetValue() + "]" );
		Network.SetUserId( pAttri->GetValue() );
	}
	else
	*/
	{
		m_wndHeaderBar.SetCaption( strValue + " - [" + Network.m_xUserId + "]" );
	}
	
	if ( ! m_wndThumb.GetSafeHwnd() ) return FALSE;
	return m_wndThumb.Layout( pXML );
}
