//
// DlgBlacklst.cpp
//

#include "stdafx.h"
#include "XUser32.h"
#include "DlgBlacklst.h"
#include "CoolInterface.h"

#include "Network.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CBlacklstDlg, CDialog)
	//{{AFX_MSG_MAP(CBlacklstDlg)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlacklstDlg construction

CBlacklstDlg::CBlacklstDlg(CWnd* pParent) : CDialog(CBlacklstDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBlacklstDlg)
	//}}AFX_DATA_INIT
	
	m_pXML		= NULL;
}

CBlacklstDlg::~CBlacklstDlg()
{
	if ( m_pXML ) delete m_pXML;
}

void CBlacklstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBlacklstDlg)
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_REMOVE, m_wndRemove);
	DDX_Control(pDX, IDC_PHONE, m_wndPhone);
	DDX_Control(pDX, IDC_LOCKED_DATE, m_wndLockedDate);
	DDX_Control(pDX, IDC_LOCKED_TIME, m_wndLockedTime);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CBlacklstDlg message handlers

BOOL CBlacklstDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );
	
	CRect rc;
	m_wndList.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL ) + 1;
	
	m_wndList.InsertColumn( 0, _T("编号"), LVCFMT_LEFT, 68, -1 );
	m_wndList.InsertColumn( 1, _T("用户号"), LVCFMT_LEFT, 120, 0 );
	m_wndList.InsertColumn( 2, _T("有效期至"), LVCFMT_LEFT, 160, 1 );
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	m_wndRemove.EnableWindow( FALSE );
	
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	tNow += COleDateTimeSpan( 7, 0, 0, 0 );
	m_wndLockedDate.SetTime( tNow );
	
	tNow.SetTime( 23, 59, 59 );
	m_wndLockedTime.SetFormat( "HH:mm:ss");
	m_wndLockedTime.SetTime( tNow );
	
	UpdateWindow();
	OnSyncOrderRelation();
	
	return TRUE;
}

HBRUSH CBlacklstDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( m_crWhite );
	
	return m_brWhite;
}

void CBlacklstDlg::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	
	if ( pNMListView->iItem < 0 || ! ( pNMListView->uNewState & LVIS_SELECTED ) ) return;
	
	if ( ( pNMListView->uOldState & LVIS_SELECTED ) == ( pNMListView->uNewState & LVIS_SELECTED ) ) return;
	
	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( pNMListView->iItem );
	CString strValue;
	
	strValue = pMap->GetAttributeValue( _T("xphone") );
	m_wndPhone.SetWindowText( (LPCTSTR)strValue );
	
	strValue = pMap->GetAttributeValue( _T("xlocked") );
	COleDateTime tt; tt.ParseDateTime( strValue );
	
	if ( tt.GetStatus() == COleDateTime::valid )
	{
		m_wndLockedDate.SetTime( tt );
		m_wndLockedTime.SetTime( tt );
	}
	
	m_wndRemove.EnableWindow( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CBlacklstDlg custom message handlers

void CBlacklstDlg::OnSyncOrderRelation()
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "select" );
	
	CXMLElement* pSync = pXML->AddElement( "column_expression" );
	pSync->SetValue( "xbh, xphone, xlocked" );
	
	pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xenterblacklst" );
	
	CString str;
	str.Format( "xjsjbh in (select xjsjbh from xuser_info "
				"where xphone = '%s')", Network.m_xUserId );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView(pXML) );
	delete pXML; pXML = NULL;
	
	m_pXML->Delete();
	if ( ! Network.CallWorker( &m_pXML ) )
	{
		OnSyncDataViewResp( m_pXML );
	}
}

BOOL CBlacklstDlg::OnSyncDataViewResp(CXMLElement* pXML)
{
	m_wndList.DeleteAllItems();
	CWaitCursor pCursor;
	
	for ( POSITION posNext = pXML->GetElementIterator(); posNext; )
	{
		CXMLElement* pSync = pXML->GetNextElement( posNext );
		if ( ! pSync->IsNamed( "SyncOrder" ) ) continue;
		
		static LPCTSTR pszSuffix[] = { _T("xbh"), _T("xphone"), _T("xlocked"), NULL };
		
		int nItem = m_wndList.InsertItem(
				LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
				m_wndList.GetItemCount(), NULL, 0, 0,
				CoolInterface.ImageForID(ID_USER_BLACKLST), (LPARAM)pSync );
		
		for ( int i = 0; pszSuffix[i] != NULL; i ++ )
		{
			CXMLAttribute* pAttri = pSync->GetAttribute( pszSuffix[i] );
			if ( pAttri ) m_wndList.SetItemText( nItem, i, pAttri->GetValue() );
		}
	}
	
	return TRUE;
}

void CBlacklstDlg::OnAdd()
{
	CXMLElement* pMap = m_pXML->AddElement( _T("SyncOrder") );
	
	int nItem = m_wndList.InsertItem(
				LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
				m_wndList.GetItemCount(), NULL, 0, 0,
				CoolInterface.ImageForID(ID_USER_BLACKLST), (LPARAM)pMap );
	
	CString strPhone;
	m_wndPhone.GetWindowText( strPhone );
	pMap->AddAttribute( _T("xphone"), strPhone );
	m_wndList.SetItemText( nItem, 1, strPhone );
	
	COleDateTime tDate; m_wndLockedDate.GetTime( tDate );
	COleDateTime tTime; m_wndLockedTime.GetTime( tTime );
	
	tDate.SetDateTime( tDate.GetYear(), tDate.GetMonth(), tDate.GetDay(),
				tTime.GetHour(), tTime.GetMinute(), tTime.GetSecond() );
	
	CString strDate = tDate.Format( "%Y-%m-%d %H:%M:%S" );
	pMap->AddAttribute( "xlocked", strDate );
	m_wndList.SetItemText( nItem, 2, strDate );
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "insert into" );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "xenterblacklst" );
	
	CString str;
	str.Format( "(xjsjbh,xphone,xlocked) select xjsjbh, '%s', '%s' from xuser_info"
				" where xphone = '%s'", strPhone, strDate, Network.m_xUserId );
	
	pSync = pXML->AddElement( "column_name" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView( pXML ) ); delete pXML;
}

void CBlacklstDlg::OnRemove() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;
	
	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "delete" );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xenterblacklst" );
	
	CString str;
	str.Format( "xbh = %s", pMap->GetAttributeValue("xbh") );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView( pXML ) ); delete pXML;
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndRemove.EnableWindow( FALSE );
}
