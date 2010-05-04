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
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_KEY, m_wndKey);
	DDX_Control(pDX, IDC_FORWARD, m_wndForward);
	DDX_Control(pDX, IDC_PLAYER, m_wndPlayer);
	DDX_Control(pDX, IDC_ADMIN, m_wndAdmin);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPlayProfilePage message handlers

BOOL CPlayProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndList.InsertColumn( 0, _T("企业号码"), LVCFMT_LEFT, 82, -1 );
	m_wndList.InsertColumn( 1, _T("呼叫转移"), LVCFMT_LEFT, 220, 0 );
	m_wndList.InsertColumn( 2, _T("开启"), LVCFMT_LEFT, 60, 1 );
	
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
	
	for ( POSITION pos = pParent->GetElementIterator(); pos; )
	{
		CXMLElement* pXML = pParent->GetNextElement( pos );
		
		if ( CXMLAttribute* pAttri = pXML->GetAttribute( _T("ID") ) )
		{
			CString strValue = pAttri->GetValue();
			
			int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), strValue, 0, 0, 24, (LPARAM)pXML );
			
			if ( pAttri = pXML->GetAttribute( _T("Forward") ) )
			{
				m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			}
			
			if ( pAttri = pXML->GetAttribute( _T("Enable") ) )
			{
				m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
			}
		}
	}
	
	return TRUE;
}

void CPlayProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	
	if ( pNMListView->iItem < 0 || (pNMListView->uNewState & LVIS_SELECTED) != LVIS_SELECTED ) return;
	if ( ( pNMListView->uOldState & LVIS_SELECTED ) == ( pNMListView->uNewState & LVIS_SELECTED ) ) return;
	
	if ( CXMLElement* pXML = (CXMLElement*)m_wndList.GetItemData( pNMListView->iItem ) )
	{
		CString strValue;
		
		strValue = pXML->GetAttributeValue( "ID" );
		m_wndKey.SetWindowText( (LPCTSTR)strValue );
		
		strValue = pXML->GetAttributeValue( "Enable" );
		m_wndForward.SelectString( -1, (LPCTSTR)strValue );
		
		strValue = pXML->GetAttributeValue( "Forward" );
		FormatComboBox( strValue, m_wndPlayer );
		m_wndPlayer.SetCurSel( 0 );
		
		m_wndDelete.EnableWindow( TRUE );
	}
}

void CPlayProfilePage::OnAdd() 
{
	CXMLAttribute* pAttri = NULL;
	
	if ( CXMLElement* pXML = m_pXML->AddElement( _T("Map") ) )
	{
		CString strValue;
		
		m_wndKey.GetWindowText( strValue );
		pXML->AddAttribute( _T("ID"), (LPCTSTR)strValue );
			
		int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), strValue, 0, 0, 24, (LPARAM)pXML );
		
		m_wndForward.GetWindowText( strValue );
		pXML->AddAttribute( _T("Enable"), strValue );
		m_wndList.SetItemText( nItem, 2, strValue );
		
		m_wndPlayer.GetWindowText( strValue );
//		strValue += '|';
		
//		FormatString( strValue, m_wndPlayer );
		pXML->AddAttribute( _T("Forward"), strValue );
		m_wndList.SetItemText( nItem, 1, strValue );
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

void CPlayProfilePage::FormatComboBox(LPCTSTR lpszText, CComboBox& pCombo)
{
	pCombo.ResetContent();
	CString strValue = lpszText;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		str.TrimLeft(); str.TrimRight();
		if ( ! str.IsEmpty() ) pCombo.AddString( str );
	}
}

void CPlayProfilePage::FormatString(CString& strText, CComboBox& pCombo)
{
	for ( int nItem = 0 ; nItem < pCombo.GetCount() ; nItem++ )
	{
		CString str;
		pCombo.GetLBText( nItem, str );
		
		str.TrimLeft(); str.TrimRight();
		if ( ! str.IsEmpty() ) strText += str + '|';
	}
}
