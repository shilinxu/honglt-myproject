//
// WndPlayer.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndPlayer.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "skin.h"
#include "XML.h"

#include "CtrlNetworkPanel.h"
#include "Datagrams.h"
#include "Network.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CPlayerWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CPlayerWnd, CChildWnd)
	//{{AFX_MSG_MAP(CPlayerWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
//	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(AFX_IDW_TOOLBAR, OnToolbarReturn)
	ON_BN_DOUBLECLICKED(AFX_IDW_TOOLBAR, OnToolbarEscape)
//	ON_UPDATE_COMMAND_UI(ID_NETWORK_CONNECT, OnUpdateNetworkConnect)
	ON_COMMAND(ID_NETWORK_CONNECT, OnNetworkConnect)
	ON_COMMAND(ID_NETWORK_DISCONNECT, OnNetworkDisconnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28

#define	MAX_ITEM		1024

//////////////////////////////////////////////////////////////////////
// CPlayerWnd construction

CPlayerWnd::CPlayerWnd() : CChildWnd(FALSE)
{
	m_bPlaying	= FALSE;
	
	Create( IDR_PLAYERFRAME );
}

CPlayerWnd::~CPlayerWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPlayerWnd XML

CXMLElement* CPlayerWnd::LoadXML(LPCTSTR lpszName, BOOL bCreate)
{
	CXMLElement* pXML = CXMLElement::FromFile( _T("Xhistory.xml"), TRUE );
	
	CXMLElement* pOrder = NULL;
	if ( pXML ) pOrder = pXML->GetElementByName( lpszName, bCreate );
	else if ( bCreate ) pOrder = new CXMLElement( NULL, lpszName );
	
	if ( pOrder ) pOrder->Detach();
	pXML->Delete();
	
	return pOrder;
}

BOOL CPlayerWnd::SaveXML(CXMLElement* pXML)
{
	ASSERT( pXML != NULL );
	
	CFile pFile;
	if ( ! pFile.Open( _T("Xhistory.xml"), CFile::modeWrite|CFile::modeCreate ) ) return FALSE;
	
	CXMLElement* pParent = LoadXML( pXML->GetName() );
	if ( ! pParent ) pParent = CreateXML();
	
	pParent->GetElementByName( pXML->GetName() )->Delete();
	pParent->AddElement( pXML );
	
	CString strXML;
	strXML = pParent->ToString( TRUE, TRUE );
	
	pFile.Write( (LPCSTR)strXML, strXML.GetLength() );
	
	pFile.Close();
	pXML->Detach();
	pParent->Delete();
	
	return TRUE;
}

CXMLElement* CPlayerWnd::CreateXML()
{
	WORD nVersion[4];
	theApp.GetVersionNumber( nVersion );
	
	CString strVersion;
	strVersion.Format( _T("%i.%i.%i.%i"),
		nVersion[0], nVersion[1], nVersion[2], nVersion[3] );
	
	CXMLElement* pXML = new CXMLElement( NULL, _T("CallCenter") );
	pXML->AddAttribute( _T("Author"), _T("Honglt") );
	pXML->AddAttribute( _T("Version"), strVersion );
	
	return pXML;
}

/////////////////////////////////////////////////////////////////////////////
// CPlayerWnd message handlers

int CPlayerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	// Load Definitions
	
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFINITIONS );
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFAULT );
	
	// Panel
	
	//m_wndPanel.Create( WS_VISIBLE, rectDefault, this );
	
	// Header
	
	if ( ! m_wndHeaderBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndHeaderBar.SetBarStyle( m_wndHeaderBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	Skin.CreateToolBar( _T("CPlayerWnd.Header"), &m_wndHeaderBar );
	
	m_wndHistory.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|CBS_DROPDOWN|CBS_AUTOHSCROLL,
		rectDefault, &m_wndHeaderBar, ID_PLAYER_HISTORY );
	
	m_wndHistory.SetFont( &CoolInterface.m_fntNormal );
	CXMLElement* pXML = LoadXML( _T("XlivePlayer"), TRUE );
	if ( pXML ) LoadXML( pXML );
	pXML->Delete();
	
	// Bottom
	
	if ( ! m_wndBottom.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndBottom.SetBarStyle( m_wndBottom.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	Skin.CreateToolBar( _T("CPlayerWnd.Bottom"), &m_wndBottom );
	
	return 0;
}

void CPlayerWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CPlayerWnd::OnSize(UINT nType, int cx, int cy) 
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
	
	DeferWindowPos( hDWP, m_wndBottom.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.bottom - BAR_HEIGHT, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
	
	if ( ! m_bPlaying ) return;
	
	CRect rect(
		rc.left + PANEL_WIDTH, rc.top + BAR_HEIGHT,
		rc.right, rc.bottom - BAR_HEIGHT );
	
	m_wndPlayer.PutIntoWindow( m_hWnd, rect );
}

void CPlayerWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	if ( bActivate )
	{
		wndPanel.SetParent( this );
		OnSize( SIZE_INTERNAL, 0, 0 );
	}
	
	CChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
}

BOOL CPlayerWnd::OnEraseBkgnd(CDC* pDC)
{
	if ( m_bPlaying ) return TRUE;
	
	CRect rc;
	GetClientRect( &rc );
	
	pDC->FillSolidRect( &rc, GetSysColor( COLOR_APPWORKSPACE ) );
	
	return TRUE;
}

void CPlayerWnd::OnToolbarReturn()
{
}

void CPlayerWnd::OnToolbarEscape()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPlayerWnd status message functionality

void CPlayerWnd::GetMessageString(UINT nID, CString& rMessage) const
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	if ( m_hInstance ) AfxSetResourceHandle( m_hInstance );
	
	if ( LoadString( rMessage, nID ) )
	{
		int nPos = rMessage.Find( '\n' );
		if ( nPos >= 0 ) rMessage.SetAt( nPos, 0 );
	}
	
	AfxSetResourceHandle( hPrevInst );
}

/////////////////////////////////////////////////////////////////////////////
// CPlayerWnd custom message handlers

void CPlayerWnd::LoadXML(CXMLElement* pXML)
{
	ASSERT( pXML->IsNamed( _T("XlivePlayer") ) );
	
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = pXML->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Cookie") ) ) continue;
		
		int nItem = m_wndHistory.AddString( pMap->GetValue() );
		if ( nItem >= MAX_ITEM ) break;
	}
	
	m_wndHistory.SetCurSel( 0 );
}

BOOL CPlayerWnd::SaveXML()
{
	CXMLElement* pXML = new CXMLElement( NULL, _T("XlivePlayer") );
	
	for ( int nItem = 0; nItem < m_wndHistory.GetCount(); nItem++ )
	{
		CString strUrl;
		m_wndHistory.GetLBText( nItem, strUrl );
		
		CXMLElement* pMap = pXML->AddElement( _T("Cookie") );
		pMap->SetValue( strUrl );
	}
	
	BOOL bResult = SaveXML( pXML );
	pXML->Delete();
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CPlayerWnd menu GUI

void CPlayerWnd::OnUpdateNetworkConnect(CCmdUI* pCmdUI) 
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );

	CCoolBarItem* pItem = m_wndHeaderBar.GetID( ID_NETWORK_CONNECT );
	
	UINT nTextID = 0;
	UINT nTipID = 0;
	
	if ( FALSE )
	{
		if ( pItem ) pItem->SetCheck( TRUE );
		if ( pItem ) pItem->SetTextColour( CoolInterface.m_crCmdText == 0 ? RGB( 255, 0, 0 ) : CoolInterface.m_crCmdText );
		nTextID	= IDS_NETWORK_DISCONNECT;
		nTipID	= ID_NETWORK_DISCONNECT;
	}
	else
	{
		if ( pItem ) pItem->SetCheck( FALSE );
		if ( pItem ) pItem->SetTextColour( CoolInterface.m_crCmdText == 0 ? RGB( 0, 127, 0 ) : CoolInterface.m_crCmdText );
		nTextID	= IDS_NETWORK_CONNECT;
		nTipID	= ID_NETWORK_CONNECT;
	}

	CString strText;
	
	LoadString( strText, nTextID );
	if ( pItem ) pItem->SetText( strText );
	
	LoadString( strText, nTipID );
	if ( pItem ) pItem->SetTip( strText );
	
	if ( pItem ) pItem->SetImage( nTipID );
	
	AfxSetResourceHandle( hPrevInst );
}

void CPlayerWnd::OnNetworkConnect()
{
	CWaitCursor pCursor;
	
	CString strUrl;
	m_wndHistory.GetWindowText( strUrl );
	strUrl.TrimLeft(); strUrl.TrimRight();
	
	m_wndPlayer.Setup();
	m_wndPlayer.InstallCapture( (CComBSTR)Settings.Audio.Render, L"Audio Renderer" );
	
	IBaseFilter* pCapture;
	if ( FAILED(m_wndPlayer.RenderUrl( (CComBSTR)strUrl, &pCapture )) ) return;
	
	UnsyncStream( RenderPayload(PayloadMask_Audio | PayloadMask_Video) );
//	m_wndPlayer.ResizeConfig( pCapture );
	
	IBaseFilter* pInfTee[2];
	m_wndPlayer.BranchStream( L"Audio Renderer", &pInfTee[0] );
	m_wndPlayer.BranchStream( L"Video Renderer", &pInfTee[1] );
	
	m_bPlaying = SUCCEEDED(m_wndPlayer.StartPreview( rectDefault, m_hWnd ));
	OnSize( SIZE_INTERNAL, 0, 0 );
	
	int nItem = m_wndHistory.FindStringExact( -1, strUrl );
	if ( nItem != -1 ) m_wndHistory.DeleteString( nItem );
	
	if ( m_wndHistory.GetCount() >= MAX_ITEM ) m_wndHistory.DeleteString( MAX_ITEM - 1 );
	m_wndHistory.SetCurSel( m_wndHistory.InsertString( 0, strUrl ) );
	
	CCoolBarItem* pItem = m_wndHeaderBar.GetID( ID_NETWORK_CONNECT );
	if ( pItem ) pItem->Show( FALSE );

	pItem = m_wndHeaderBar.GetID( ID_NETWORK_DISCONNECT );
	if ( pItem ) pItem->Show( TRUE );
	
	SaveXML();
}

void CPlayerWnd::OnNetworkDisconnect()
{
	m_wndPlayer.Setup();
	m_bPlaying	= FALSE;
	
	CCoolBarItem* pItem = m_wndHeaderBar.GetID( ID_NETWORK_CONNECT );
	if ( pItem ) pItem->Show( TRUE );

	pItem = m_wndHeaderBar.GetID( ID_NETWORK_DISCONNECT );
	if ( pItem ) pItem->Show( FALSE );
	
	ResyncStream( RenderPayload(PayloadMask_Audio | PayloadMask_Video) );
}

/////////////////////////////////////////////////////////////////////////////
// CPlayerWnd operations

HRESULT CPlayerWnd::ResyncStream(WORD nPayloadMask)
{
	CTaskBox* pWindow = wndPanel.FindUser( Network.m_xUserId );
	if ( ! pWindow ) return S_FALSE;
	
	if ( (RenderPayload(PayloadMask_Audio) & nPayloadMask) == RenderPayload(PayloadMask_Audio) )
	{
		CDatagrams* pDatagram = Datagrams.SeekNext( Payload_Alaw );
		pWindow->SendMessage( WM_STARTPREVIEW, RenderPayload(PayloadMask_Audio), (DWORD)pDatagram );
	}
	if ( (RenderPayload(PayloadMask_Video) & nPayloadMask) == RenderPayload(PayloadMask_Video) )
	{
		CDatagrams* pDatagram = Datagrams.SeekNext( Payload_H263 );
		pWindow->SendMessage( WM_STARTPREVIEW, RenderPayload(PayloadMask_Video), (DWORD)pDatagram );
	}
	
	return S_OK;
}

HRESULT CPlayerWnd::UnsyncStream(WORD nPayloadMask)
{
	CTaskBox* pWindow = wndPanel.FindUser( Network.m_xUserId );
	if ( ! pWindow ) return S_FALSE;
	
	return pWindow->SendMessage( WM_STARTPREVIEW, RenderPayload(nPayloadMask), NULL );
}
