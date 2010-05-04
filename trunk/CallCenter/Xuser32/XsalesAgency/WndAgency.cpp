//
// WndAgency.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndAgency.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "Network.h"
#include "skin.h"
#include "XML.h"

#include "WndMain.h"
#include "WndNetwork.h"
#include "DlgAgencyProperties.h"
#include "DlgAgencyLayout.h"

#include "VideoCapture.h"
#include "PacketCore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CAgencyWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CAgencyWnd, CChildWnd)
	//{{AFX_MSG_MAP(CAgencyWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	ON_CBN_SELCHANGE(IDC_AGENCY_FAMILY, OnSelChangeFamily)
	ON_UPDATE_COMMAND_UI(ID_AGENCY_PROPERTIES, OnUpdateAgencyProperties)
	ON_COMMAND(ID_AGENCY_PROPERTIES, OnAgencyProperties)
	ON_COMMAND(ID_AGENCY_REFRESH, OnAgencyRefresh)
	ON_UPDATE_COMMAND_UI(ID_AGENCY_LAYOUT, OnUpdateAgencyLayout)
	ON_COMMAND(ID_AGENCY_LAYOUT, OnAgencyLayout)
	ON_COMMAND(ID_AGENCY_XQUICK, OnAgencyXquick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define SPLIT_SIZE		6
#define BAR_HEIGHT		26
#define MARGIN_WIDTH	5

/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd construction

CAgencyWnd::CAgencyWnd() : CChildWnd(FALSE)
{
	Create( IDR_AGENCYFRAME );
}

CAgencyWnd::~CAgencyWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd message handlers

int CAgencyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	// Load Definitions
	
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFINITIONS );
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFAULT );
	
	// Panel
	
	m_wndPanel.Create( WS_VISIBLE, rectDefault, this );
	
	// Header
	
	m_wndFamily.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|CBS_DROPDOWNLIST|CBS_HASSTRINGS,
		rectDefault, this, IDC_AGENCY_FAMILY );
	
	if ( ! m_wndHeaderBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndHeaderBar.SetBarStyle( m_wndHeaderBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	
	Skin.CreateToolBar( _T("CAgencyWnd.Header"), &m_wndHeaderBar );
	
	// List
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE, rectDefault, this, IDC_AGENCY_LIST );
	
	// Bottom
	
	if ( ! m_wndBottom.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndBottom.SetBarStyle( m_wndBottom.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	
	if ( ! m_wndPhone.Create( WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rectDefault, &m_wndBottom, IDC_USER_NUMBER ) ) return -1;
	m_wndPhone.SetFont( &CoolInterface.m_fntNormal );
	
	Skin.CreateToolBar( _T("CAgencyWnd.Bottom"), &m_wndBottom );
	
	Network.SendPacket( new CBindPacket(Network.m_xUserId, "+2;") );
	m_wndPanel.OnChanged();
	
	return 0;
}

void CAgencyWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
	
	m_wndList.DeleteAllItems();
	
//	if ( m_wndPanel.GetSafeHwnd() )
//	m_wndPanel.UnsubclassWindow();
	Network.SendPacket( new CBindPacket(Network.m_xUserId, "-2;") );
	
	CloseDatabase();
}

void CAgencyWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );
	
	if ( rc.Width() < 32 || rc.Height() < 32 ) return;
	
	for ( int nWidth = 0, nIndex = 0; nIndex < m_wndHeaderBar.GetCount(); nIndex++ )
	{
		CRect rcClient;
		CCoolBarItem* pItem = m_wndHeaderBar.GetIndex( nIndex );
		
		if ( m_wndHeaderBar.GetItemRect( pItem, &rcClient ) ) nWidth += rcClient.Width();
	}

	nWidth += MARGIN_WIDTH * 2;
	HDWP hDWP = BeginDeferWindowPos( 5 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndFamily.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH - nWidth, rc.Height(), SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndHeaderBar.GetSafeHwnd(), NULL,
		rc.right - nWidth, rc.top, nWidth, BAR_HEIGHT, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndList.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top + BAR_HEIGHT, rc.Width() - PANEL_WIDTH, rc.Height() - BAR_HEIGHT * 2, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndBottom.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.bottom - BAR_HEIGHT, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CAgencyWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	if ( bActivate ) wndVideo.SetParent( &m_wndPanel);
	
	CChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd custom message handlers

void CAgencyWnd::OnSelChangeFamily() 
{
	int nItem = m_wndFamily.GetCurSel();
	if ( nItem == CB_ERR ) return;
	
	CXMLElement* pXML = (CXMLElement*)m_wndFamily.GetItemData( nItem );
	ASSERT( pXML != NULL );
#if 0
	CXMLElement* pSync = new CXMLElement( NULL, "SyncOrderRelation" );
	pSync->AddAttribute( "hKey", pXML->GetAttributeValue( "id" ) );
	pSync->AddAttribute( "top", "1024" );
	
	CXMLElement* pSub = pSync->AddElement( "MsgType" );
	pSub->SetValue( "XsalesAgency" );
	
	Network.SendPacket( new CSyncOrderRelation( pSync ) ); delete pSync;
#else
	CString strValue = pXML->GetAttributeValue( "id" );
	m_wndList.LoadRecordset( (LPCTSTR)strValue );
#endif
}
/*
BOOL CAgencyWnd::OnSyncOrderRelation(LPCTSTR lpszKey, int nMaxTop)
{
	try
	{
		CString strQuery;
		strQuery.Format( "select top %i * from xsale"
			" where xsaleinfo = '%s'", nMaxTop, lpszKey );
		TRACE( strQuery + "\n" );
		theApp.Message( MSG_DEBUG, strQuery );
		
		_RecordsetPtr xRecordset;
		xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		
		for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
		{
			_variant_t TheValue = xRecordset->GetCollect( "xsale" );
			pMap->AddAttribute( "xsale", (LPCTSTR)(_bstr_t)TheValue );
			
			TheValue = xRecordset->GetCollect( "xphone" );
			pMap->AddAttribute( "xphone", (LPCTSTR)(_bstr_t)TheValue );
			
			TheValue = xRecordset->GetCollect( "xreason" );
			if ( VT_NULL != TheValue.vt ) pMap->AddElement( "xreason" )->SetValue( (LPCTSTR)(_bstr_t)TheValue );
		}
	}
	catch (_com_error e)
	{
	}
	
	SendPacket( new CSyncOrderRelation( pXML ) );
	delete pXML; pXML = NULL;
	
	return TRUE;
}
*/
/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd database

BOOL CAgencyWnd::OpenDatabase(LPCTSTR xHost, LPCTSTR xCatalog)
{
	// 创建进行存取的字符串

	CString sConnect = _T("");
	sConnect += _T("Provider=SQLOLEDB.1");
	sConnect += _T(";Persist Security Info=False");
	sConnect += _T(";User ID=sa");
	sConnect += _T(";Password=");
	sConnect += _T(";Initial Catalog="); sConnect += xCatalog;
	sConnect += _T(";Data Source="); sConnect += xHost;
	
	TRACE( sConnect + "\n");
	
	try
	{
		xConnect(m_pConnection, (_bstr_t)sConnect);
	}
	catch(_com_error e)//捕捉异常
	{
		AfxMessageBox(CString("连接数据库失败：") + e.ErrorMessage());
		return FALSE;
	} 
	
	return TRUE;
}

BOOL CAgencyWnd::CloseDatabase()
{
	if ( NULL == m_pConnection ) return TRUE;
	
	try
	{
		if ( m_pConnection->State ) xDisconnect( m_pConnection );
		m_pConnection.Release();
	}
	catch(_com_error e)//捕捉异常
	{
		AfxMessageBox(CString("断开数据库失败：") + e.ErrorMessage());
		return FALSE;
	} 
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyWnd menu GUI

void CAgencyWnd::OnUpdateAgencyProperties(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndFamily.GetCurSel() != CB_ERR );
}

void CAgencyWnd::OnAgencyProperties()
{
	int nCurIndex = m_wndFamily.GetCurSel();
	CXMLElement* pXML = (CXMLElement*)m_wndFamily.GetItemData( nCurIndex );
	
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );
	
	CString strValue = pXML->GetAttributeValue( "id" );
	CAgencyPropertiesDlg dlg( (LPCTSTR)strValue, this );
	dlg.DoModal();
	
	AfxSetResourceHandle( hPrevInst );
}

void CAgencyWnd::OnUpdateAgencyLayout(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndList.GetSelectedCount() > 0 );
}
#if 0
void CAgencyWnd::OnAgencyLayout()
{
	int nItem = m_wndList.GetNextItem( -1, LVIS_SELECTED );
	if ( nItem < 0 ) return;
	
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );
	
	CXMLElement* pXML = (CXMLElement*)m_wndList.GetItemData( nItem );
	CXMLElement* pJobs= (CXMLElement*)m_wndFamily.GetItemData( m_wndFamily.GetCurSel() );
	
	CAgencyLayoutDlg dlg( pXML, pJobs );
	if ( dlg.DoModal() == IDOK )
	{
		CXMLElement* pSync = new CXMLElement( NULL, "SyncOrderRelation" );
		
		pSync->AddElement( "MsgType" )->SetValue( "XsalesAgency" );
		
		pSync->AddElement( pXML->Clone() );
		
		Network.SendPacket( new CSyncOrderRelation( pSync ) ); delete pSync;
		
		m_wndList.SetItemText( nItem, 3, pXML->GetElementByName("xstate")->GetValue() );
		m_wndList.SetItemText( nItem, 4, pXML->GetElementByName("xreason")->GetValue() );
	}
	
	AfxSetResourceHandle( hPrevInst );
}
#else
void CAgencyWnd::OnAgencyLayout()
{
	int nItem = m_wndList.GetNextItem( -1, LVIS_SELECTED );
	if ( nItem < 0 ) return;
	
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );
	
//	CXMLElement* pXML = (CXMLElement*)m_wndFamily.GetItemData( m_wndFamily.GetCurSel() );
//	CString strValue = pXML->GetAttributeValue( "id" );
	CString strKey = m_wndList.GetKey( nItem );
	
	CAgencyLayoutDlg dlg( strKey, this );
	if ( dlg.DoModal() == IDOK ) m_wndList.Resync( nItem );
	
	AfxSetResourceHandle( hPrevInst );
}
#endif
void CAgencyWnd::OnAgencyRefresh()
{
	SendMessage(WM_COMMAND, MAKELONG(IDC_AGENCY_FAMILY,CBN_SELCHANGE), (LPARAM)m_hWnd);
}

void CAgencyWnd::OnAgencyXquick()
{
	CString strValue;
	m_wndPhone.GetWindowText( strValue );
	
	strValue.TrimLeft(); strValue.TrimRight();
	if ( strValue.IsEmpty() ) return;
	
	POSITION posNext = m_wndList.GetFirstSelectedItemPosition();
	int nItem = posNext ? m_wndList.GetNextSelectedItem( posNext ) : -1;
	
	m_wndList.SeekByPhone( strValue, nItem, TRUE );
}
#if 0
BOOL CAgencyWnd::LayoutJobs(CXMLElement* pXML)
{
	ASSERT( pXML && pXML->IsNamed( "SyncOrderRelation" ) );
	ASSERT( pXML->GetElementByName( "MsgType", TRUE )->GetValue() == "XsalesAgency" );
	
	CXMLAttribute* hKey = pXML->GetAttribute( _T("hKey") );
	if ( hKey ) return LayoutJobs( pXML, hKey->GetValue() );
	
	int nCurIndex = m_wndFamily.GetCurSel();
	m_wndFamily.ResetContent();
	
	for ( POSITION posNext = pXML->GetElementIterator(); posNext; )
	{
		CXMLElement* pMap = pXML->GetNextElement( posNext );
		if ( ! pMap->IsNamed( "SyncOrder" ) ) continue;
		
		CXMLElement* pItem = pMap->Clone();
		pItem->AddAttribute( "image", "1" );
		pItem->AddAttribute( "text", pMap->GetAttributeValue( "job" ) );
		
		m_wndFamily.AddString( (CXMLElement*)pItem );
		delete pItem;
	}
	
	m_wndFamily.SetCurSel( max(nCurIndex, 0) );
	SendMessage(WM_COMMAND, MAKELONG(IDC_AGENCY_FAMILY,CBN_SELCHANGE), (LPARAM)m_hWnd);
	
	return TRUE;
}
#else
BOOL CAgencyWnd::LayoutJobs(CXMLElement* pXML)
{
	if ( m_pConnection != NULL && m_pConnection->State ) return TRUE;
	if ( ! OpenDatabase( Settings.Database.Host, "xcallcenter") ) return FALSE;
	
	if ( ! OnSyncOrderRelation() ) return FALSE;
	
	m_wndFamily.SetCurSel( 0 );
	SendMessage(WM_COMMAND, MAKELONG(IDC_AGENCY_FAMILY,CBN_SELCHANGE), (LPARAM)m_hWnd);
	return TRUE;
}

BOOL CAgencyWnd::OnSyncOrderRelation()
{
	try
	{
		CString strQuery;
		strQuery.Format( "select * from xsaleinfo"
			" where xtimeexpired >= getdate() or xtimeexpired is null" );
		TRACE( strQuery + "\n" );
		
		_RecordsetPtr xRecordset;
		xRecordset = xExecute(m_pConnection, (_bstr_t)strQuery, adCmdText);
		
		for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
		{
			CXMLElement* pXML = new CXMLElement( NULL, "SyncOrder" );
			
			_variant_t TheValue = xRecordset->GetCollect( "xsi" );
			pXML->AddAttribute( "id", (LPCTSTR)(_bstr_t)TheValue );
			
			TheValue = xRecordset->GetCollect( "xjob" );
			pXML->AddAttribute( "text", (LPCTSTR)(_bstr_t)TheValue );
			
			pXML->AddAttribute( "image", "1" );
			
			m_wndFamily.AddString( (CXMLElement*)pXML );
			delete pXML;
		}
	}
	catch (_com_error e)
	{
		return FALSE;
	}
	
	return TRUE;
}
#endif
BOOL CAgencyWnd::LayoutJobs(CXMLElement* pXML, LPCTSTR lpszKey)
{
#ifdef _DEBUG
	int nItem = m_wndFamily.GetCurSel();
	CXMLElement* pMap = nItem != CB_ERR ? (CXMLElement*)m_wndFamily.GetItemData( nItem ) : NULL;
	
	ASSERT( pMap && pMap->GetAttributeValue("id").CompareNoCase( lpszKey ) == 0 );
#endif
	
	m_wndList.LoadRecordset( pXML );
	return TRUE;
}
