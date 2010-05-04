//
// DlgUserMeeting.cpp
//

#include "stdafx.h"
#include "XUser32.h"
#include "DlgUserMeeting.h"
#include "CoolInterface.h"

#include "Network.h"
#include "Datagrams.h"
#include "RenderGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CUserMeetingDlg, CDialog)
	//{{AFX_MSG_MAP(CUserMeetingDlg)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_MEMBER, OnMyGroup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define MIN_GROUP		4001
#define MAX_GROUP		4999

/////////////////////////////////////////////////////////////////////////////
// CUserMeetingDlg construction

CUserMeetingDlg::CUserMeetingDlg(CWnd* pParent) : CDialog(CUserMeetingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserMeetingDlg)
	//}}AFX_DATA_INIT
	
	m_pXML		= NULL;
	m_pRenderer	= NULL;
}

CUserMeetingDlg::~CUserMeetingDlg()
{
	if ( m_pXML ) delete m_pXML;
	if ( m_pRenderer ) delete m_pRenderer;
}

void CUserMeetingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserMeetingDlg)
	DDX_Control(pDX, IDC_GROUP, m_wndGroup);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_MEMBER, m_wndMember);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CUserMeetingDlg message handlers

BOOL CUserMeetingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_wndList.InsertColumn( 0, _T("号码"), LVCFMT_LEFT, 108, -1 );
	m_wndList.InsertColumn( 1, _T("姓名"), LVCFMT_LEFT, 94, 0 );
	m_wndList.InsertColumn( 2, _T("状态"), LVCFMT_LEFT, 52, 1 );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );
	
	HINSTANCE hInstance = LoadLibrary( _T("Xuserinfo.dll") );

	typedef DWORD (* USERGROUP) (LPCTSTR, CXMLElement**, DWORD);
	USERGROUP pfnUserInfo = (USERGROUP) GetProcAddress( hInstance, _T("GetUserGroupInfo") );
	
	if ( pfnUserInfo ) pfnUserInfo( Network.m_xUserId, &m_pXML, INFINITE );
	if ( m_pXML ) OnSyncDataViewResp( m_pXML );
	
	FreeLibrary( hInstance );
	PostMessage( WM_COMMAND, IDOK );
	
	return TRUE;
}

void CUserMeetingDlg::OnPaint() 
{
	CPaintDC dc( this );
	CRect rc;
	
	GetClientRect( &rc );
	rc.top += 51;
	
	dc.Draw3dRect( 0, 50, rc.right + 1, 0,
		RGB( 128, 128, 128 ), RGB( 128, 128, 128 ) );
	dc.FillSolidRect( &rc, m_crWhite );
}

HBRUSH CUserMeetingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( m_crWhite );
	
	return m_brWhite;
}

void CUserMeetingDlg::OnMyGroup()
{
	CMenu pMenu;
	pMenu.CreatePopupMenu();
	
	if ( m_pXML ) CreateMenuGroup( m_pXML, pMenu );
	
	HMENU hMenu = CreatePopupMenu();
	AppendMenu( pMenu, MF_STRING, MIN_GROUP - 1, _T("我的所有成员") );
	
	CRect rcButton;
	m_wndMember.GetWindowRect( &rcButton );
	
	DWORD nFlags = TPM_LEFTALIGN | TPM_VERTICAL | TPM_RIGHTBUTTON;
	
	if ( UINT nMenuID = pMenu.TrackPopupMenu(nFlags | TPM_RETURNCMD, rcButton.left, rcButton.bottom, this) )
	{
		if ( nMenuID == MIN_GROUP - 1 )
		{
			CString strText;
			m_wndGroup.GetWindowText( strText );
			
			CString str;
			str.Format( "是否发布\"%s\"内的所有成员?", strText );
			
			if (AfxMessageBox(str, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDOK)
			{
				CMenu* pSubMenu = pMenu.GetSubMenu( pMenu.GetMenuItemCount() - 2 );
				if ( pSubMenu ) PutIntoPhone( pSubMenu->GetSafeHmenu(), 0, pSubMenu->GetMenuItemCount() );
			}
		}
		else
		{
			PutIntoPhone( pMenu, nMenuID );
		}
	}
}

void CUserMeetingDlg::OnOK()
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "update" );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "xmeetingroomlist" );
	
	CString str;
	str.Format( "set xmeetingused = '1', xmeetingroomfee = '%s'", Network.m_xUserId );
	
	pSync = pXML->AddElement( "column_name" );
	pSync->SetValue( (LPCTSTR)str );
#if 0
	str.Format( "xmeetingroomcode in (select top 1 xmeetingroomcode"
		" from xmeetingroomlist where xmeetingused!='1' and xmeetingroomcode"
		" not in (select xmeetingroomcode from xmeetingroom))" );
#else
	str.Format( "xmeetingroomcode in (select top 1 xmeetingroomcode"
		" from xmeetingroomlist where xmeetingused!='1')" );
#endif
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView( pXML ) );
	delete pXML;
#if 0
	pXML = new CXMLElement( NULL, ISyncHostFile );
	pXML->AddAttribute( "method", "meeting" );
	
	Network.SendPacket( new CSyncHostFile( pXML ) );
	delete pXML;
#else
	pXML = new CXMLElement( NULL, ISyncHostFile );
	pXML->AddAttribute( "method", "talk" );
	
	Network.SendPacket( new CSyncHostFile(pXML) );
	delete pXML;
#endif
	GetDlgItem( IDOK )->EnableWindow( FALSE );
	
	CGraphRender*& pRenderer = m_pRenderer;
	if ( ! pRenderer ) pRenderer = new CGraphRender( Network.m_xUserId );
	
	CDatagrams* pDatagram = Datagrams.SeekNext(Payload_Alaw);
	pRenderer->RenderPreview( PreviewPayload(PayloadMask_Audio), NULL, (DWORD)pDatagram );
	
	SetTimer( 1, 6000, NULL );
	PostMessage( WM_TIMER, 1 );
}

void CUserMeetingDlg::OnCancel()
{
	if (AfxMessageBox(IDS_QUITPROMPT, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) != IDOK) return;
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "update" );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "xmeetingroomlist" );
	
	pSync = pXML->AddElement( "column_name" );
	pSync->SetValue( "set xmeetingused = '0'" );
	
	CString str;
	str.Format( "xmeetingroomfee = '%s'", Network.m_xUserId );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView( pXML ) );
	delete pXML;
#if 1
	pXML = new CXMLElement( NULL, ISyncHostFile );
	pXML->AddAttribute( "method", "bye" );
	
	Network.SendPacket( new CSyncHostFile(pXML) );
	delete pXML;
#endif
	
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CUserMeetingDlg common timer

void CUserMeetingDlg::OnTimer(UINT nIDEvent) 
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "select" );
	
	CXMLElement* pSync = pXML->AddElement( "column_expression" );
	pSync->SetValue( "xmeetingphone, xmeetingphonestate=case"
					" when xmeetingphonestate='0' then '等待'"
					" when xmeetingphonestate='1' then '呼叫'"
					" when xmeetingphonestate='2' then '在线'"
					" else '其他' end" );
	
	pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xmeetingroom" );
	
	CString str;
	str.Format( "xmeetingroomcode=(select xmeetingroomcode"
		" from xmeetingroomlist where xmeetingroomfee='%s' and xmeetingused='1')", Network.m_xUserId );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView(pXML) );
	delete pXML; pXML = NULL;
	
	if ( Network.CallWorker( &pXML, INFINITE ) ) return;
	
	int nItem = 0;
	POSITION posNext = pXML->GetElementIterator();
	for ( POSITION pos = posNext ; pos ; pos = posNext )
	{
		CXMLElement* pSync = pXML->GetNextElement( posNext );
		
		CString strDebug = pSync->GetAttributeValue( "xmeetingphone" );
		if ( m_wndList.GetItemText(nItem, 0).CompareNoCase( strDebug ) )
		{
			m_wndList.DeleteItem( nItem );
			posNext = pos; continue;
		}
		
		CString strValue = pSync->GetAttributeValue( "xmeetingphonestate" );
		m_wndList.SetItemText( nItem, 2, strValue );
		
		nItem++;
	}
	for ( ; nItem < m_wndList.GetItemCount(); nItem )
	{
		m_wndList.DeleteItem( nItem );
	}
	
	pXML->Delete();
}

/////////////////////////////////////////////////////////////////////////////
// CUserMeetingDlg custom message handlers

BOOL CUserMeetingDlg::OnSyncDataViewResp(CXMLElement* pXML)
{
	m_wndGroup.ResetContent();
	
	for ( POSITION posNext = pXML->GetElementIterator(); posNext; )
	{
		CXMLElement* pSync = pXML->GetNextElement( posNext );
		if ( ! pSync->IsNamed( "SyncOrder" ) ) continue;
		
		static LPCTSTR pszGroup[] = { _T("xgroupname"), _T("xgroupnum") };
		
		CXMLAttribute* pAttri = pSync->GetAttribute( pszGroup[0] );
		if ( ! pAttri || pAttri->GetValue().IsEmpty() ) pAttri = pSync->GetAttribute( pszGroup[1] );
		
		if ( pAttri && m_wndGroup.FindStringExact( -1, pAttri->GetValue() ) == CB_ERR )
		m_wndGroup.SetItemDataPtr( m_wndGroup.AddString( pAttri->GetValue() ), pXML );
	}
	
	m_wndGroup.SetCurSel( 0 );
	SendMessage(WM_COMMAND, MAKELONG(m_wndGroup.GetDlgCtrlID(),CBN_SELCHANGE), (LPARAM)m_hWnd);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUserMeetingDlg user group info

BOOL CUserMeetingDlg::CreateMenuGroup(CXMLElement* pXML, HMENU hParent)
{
	UINT nMenuId = MIN_GROUP;
	
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pSync = pXML->GetNextElement( pos );
		if ( ! pSync->IsNamed( "SyncOrder" ) ) continue;
		
		static LPCTSTR pszGroup[] = { _T("xgroupname"), _T("xgroupnum") };
		
		CXMLAttribute* pAttri = pSync->GetAttribute( pszGroup[0] );
		if ( ! pAttri || pAttri->GetValue().IsEmpty() ) pAttri = pSync->GetAttribute( pszGroup[1] );
		
		if ( pAttri && m_wndGroup.FindStringExact( -1, pAttri->GetValue() ) != m_wndGroup.GetCurSel() ) continue;
		
		HMENU hMenu = FindMenuGroup( hParent, pAttri->GetValue() );	if ( ! hMenu )
		{
			hMenu = CreatePopupMenu();
			AppendMenu( hParent, MF_STRING|MF_POPUP, (UINT)hMenu, pAttri->GetValue() );
		}
		
		CString strText = pSync->GetAttributeValue( _T("xname") );
		if ( ! strText.IsEmpty() ) strText += '\t';
		
		strText += pSync->GetAttributeValue( _T("xphone") );
		AppendMenu( hMenu, MF_STRING, nMenuId++, strText );
		
		if ( nMenuId > MAX_GROUP ) break;
	}
	
	return TRUE;
}

HMENU CUserMeetingDlg::FindMenuGroup(HMENU hParent, LPCTSTR lpszMenu)
{
	for ( int nItem = GetMenuItemCount( hParent ); nItem > 0; nItem-- )
	{
		TCHAR strItem[64];
		GetMenuString( hParent, nItem - 1, strItem, 64, MF_BYPOSITION );
		
		if ( ! _tcsicmp( strItem, lpszMenu ) ) return GetSubMenu( hParent, nItem - 1 );
	}
	
	return NULL;
}

void CUserMeetingDlg::PutIntoPhone(HMENU hMenu, UINT nMenuId)
{
	CString strValue;
	GetMenuString( hMenu, nMenuId, strValue.GetBuffer(32), 32, MF_BYCOMMAND );
	strValue.ReleaseBuffer();
	
	int nPos = strValue.Find( '\t' );
	PutIntoPhone( nPos == -1 ? strValue : strValue.Mid( nPos + 1 ),
		nPos == -1 ? "" : strValue.Left( nPos ) );
}

void CUserMeetingDlg::PutIntoPhone(HMENU hMenu, int nPos, int nCount)
{
	for ( int nItem = nPos ; nItem < nCount ; nItem ++ )
	{
		CString strValue;
		GetMenuString( hMenu, nItem, strValue.GetBuffer(32), 32, MF_BYPOSITION );
		strValue.ReleaseBuffer();
		
		int nPos = strValue.Find( '\t' );
		PutIntoPhone( nPos == -1 ? strValue : strValue.Mid( nPos + 1 ),
			nPos == -1 ? "" : strValue.Left( nPos ) );
	}
}

void CUserMeetingDlg::PutIntoPhone(LPCTSTR lpszPhone, LPCTSTR lpszName)
{
	int nItem = m_wndList.InsertItem( m_wndList.GetItemCount(), lpszPhone );
	m_wndList.SetItemText( nItem, 1, lpszName );
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "insert into" );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "xmeetingroom" );
	
	CString str;
	str.Format( "(xmeetingroomcode, xmeetingphone)"
				" select xmeetingroomcode, '%s' from xmeetingroomlist"
				" where xmeetingused='1' and xmeetingroomfee = '%s'", lpszPhone, Network.m_xUserId );
	
	pSync = pXML->AddElement( "column_name" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView( pXML ) );
	delete pXML;
}
