//
// PageProfilePlay.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "PageProfilePlay.h"
#include "CoolInterface.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPlayProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CPlayProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CPlayProfilePage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblClkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlayProfilePage property page

CPlayProfilePage::CPlayProfilePage(CXMLElement* pXML)
	: CSettingsPage( CPlayProfilePage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CPlayProfilePage)
	//}}AFX_DATA_INIT
}

CPlayProfilePage::~CPlayProfilePage()
{
}

void CPlayProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlayProfilePage)
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_NAME, m_wndName);
	DDX_Control(pDX, IDC_KEY, m_wndKey);
	DDX_Control(pDX, IDC_CLASS, m_wndClass);
	DDX_Control(pDX, IDC_ADMIN, m_wndAdmin);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_GROUP, m_wndGroup);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPlayProfilePage message handlers

BOOL CPlayProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndList.InsertColumn( 0, _T("按键代码"), LVCFMT_LEFT, 82, -1 );
	m_wndList.InsertColumn( 1, _T("节目名称"), LVCFMT_LEFT, 140, 0 );
	m_wndList.InsertColumn( 2, _T("宿主"), LVCFMT_LEFT, 162, 1 );
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	if ( m_pXML ) LoadXML( m_pXML );
	
	m_wndDelete.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPlayProfilePage::OnOK()
{
//	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CPlayProfilePage::LoadXML(CXMLElement* pParent)
{
	m_wndList.DeleteAllItems();
	
	if ( pParent->IsNamed( "Album" ) )
	{
		m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, 0,
			"..", 0, 0, 23, (LPARAM)pParent->GetParent() );
	}
	
	for ( POSITION pos = pParent->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pParent->GetNextElement( pos );
		
		if ( CXMLAttribute* pAttri = pXML->GetAttribute( _T("ID") ) )
		{
			CString strValue = pAttri->GetValue();
			
			int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, m_wndList.GetItemCount(),
				strValue, 0, 0, pXML->IsNamed( "Album" ) ? 23 : 24, (LPARAM)pXML );
			
			if ( pAttri = pXML->GetAttribute( _T("Play") ) )
			{
				m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			}
			
			if ( pAttri = pXML->GetAttribute( _T("Host") ) )
			{
				m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
			}
		}
	}
	
	m_pXML = pParent;
	return TRUE;
}

void CPlayProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	
	if ( pNMListView->iItem < 0 || ( pNMListView->uNewState & LVIS_SELECTED ) != LVIS_SELECTED ) return;
	
	if ( ( pNMListView->uOldState & LVIS_SELECTED ) == ( pNMListView->uNewState & LVIS_SELECTED ) ) return;
	
	if ( CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( pNMListView->iItem ) )
	{
		CString strValue;
		
		strValue = pMap->GetAttributeValue( _T("ID") );
		m_wndKey.SetWindowText( (LPCTSTR)strValue );
		
		strValue = pMap->GetAttributeValue( _T("Play") );
		m_wndName.SetWindowText( (LPCTSTR)strValue );
		
		strValue = pMap->GetAttributeValue( _T("Class") );
		m_wndClass.SetWindowText( (LPCTSTR)strValue );
		
		strValue = pMap->GetAttributeValue( _T("Admin") );
		m_wndAdmin.SetWindowText( (LPCTSTR)strValue );
		
		strValue = pMap->GetAttributeValue( _T("Host") );
		m_wndHost.SetWindowText( (LPCTSTR)strValue );
		
		strValue = pMap->GetAttributeValue( _T("Group") );
		m_wndGroup.SetWindowText( (LPCTSTR)strValue );
		
		m_wndDelete.EnableWindow( pMap != m_pXML->GetParent() );
	}
}

void CPlayProfilePage::OnDblClkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem < 0 ) return;
	
	if ( CXMLElement* pXML = (CXMLElement*)m_wndList.GetItemData( nItem ) )
	{
		if ( ! pXML->IsNamed( "Play" ) ) LoadXML( pXML );
	}
}

void CPlayProfilePage::OnAdd() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	CXMLElement* pXML = ( nItem != -1 ) ? (CXMLElement*)m_wndList.GetItemData(nItem) : NULL;
	
	if ( pXML ) pXML = pXML->Clone();
	else pXML = new CXMLElement( NULL, _T("Album") );
	
	if ( pXML = m_pXML->AddElement( pXML ) )
	{
		CString strValue;
		m_wndClass.GetWindowText( strValue );
		
		pXML->SetName( strValue.CompareNoCase("子类") == 0 ? _T("Album") : _T("Play") );
		
		m_wndKey.GetWindowText( strValue );
		pXML->AddAttribute( _T("ID"), strValue );
		
		int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, m_wndList.GetItemCount(),
			strValue, 0, 0, pXML->IsNamed( "Album" ) ? 23 : 24, (LPARAM)pXML );
		
		m_wndName.GetWindowText( strValue );
		pXML->AddAttribute( _T("Play"), strValue );
		m_wndList.SetItemText( nItem, 1, strValue );
		
		m_wndClass.GetWindowText( strValue );
		pXML->AddAttribute( _T("Class"), strValue );
		
		m_wndHost.GetWindowText( strValue );
		pXML->AddAttribute( _T("Host"), strValue );
		m_wndList.SetItemText( nItem, 2, strValue );
		
		m_wndAdmin.GetWindowText( strValue );
		pXML->AddAttribute( _T("Admin"), strValue );
		
		//m_wndGroup.GetWindowText( strValue );
		//pXML->AddAttribute( _T("Group"), strValue );
	}
}

void CPlayProfilePage::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndDelete.EnableWindow( FALSE );
}
