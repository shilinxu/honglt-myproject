//
// PageProfileAnalog.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "PageProfileAnalog.h"
#include "CoolInterface.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CAnalogProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CAnalogProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CAnalogProfilePage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAnalogProfilePage property page

CAnalogProfilePage::CAnalogProfilePage(CXMLElement* pXML)
	: CSettingsPage( CAnalogProfilePage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CAnalogProfilePage)
	//}}AFX_DATA_INIT
}

CAnalogProfilePage::~CAnalogProfilePage()
{
}

void CAnalogProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalogProfilePage)
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_LINKID, m_wndLinkId);
	DDX_Control(pDX, IDC_LOCALID, m_wndLocalId);
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CAnalogProfilePage message handlers

BOOL CAnalogProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndList.InsertColumn( 0, _T("中继范围"), LVCFMT_LEFT, 180, -1 );
	m_wndList.InsertColumn( 1, _T("接入号码"), LVCFMT_LEFT, 102, 0 );
	m_wndList.InsertColumn( 2, _T("原始号码"), LVCFMT_LEFT, 80, 1 );
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	if ( m_pXML ) LoadXML();
	
	m_wndDelete.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnalogProfilePage::OnOK()
{
//	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CAnalogProfilePage::LoadXML()
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	for ( POSITION pos = m_pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = m_pXML->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		if ( pAttri = pMap->GetAttribute( _T("ID") ) )
		{
			int nItem = m_wndList.InsertItem( LVIF_IMAGE|LVIF_PARAM,
				m_wndList.GetItemCount(), NULL, 0, 0, 28, (LPARAM)pMap );
			
			strValue = pAttri->GetValue();
			m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			
			if ( pAttri = pMap->GetAttribute( _T("LinkId") ) )
			{
				m_wndList.SetItemText( nItem, 0, pAttri->GetValue() );
			}
			
			if ( pAttri = pMap->GetAttribute( _T("LocalId") ) )
			{
				m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
			}
		}
	}

	return TRUE;
}

void CAnalogProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	
	if ( pNMListView->iItem >= 0 )
	{
		if ( ( pNMListView->uOldState & LVIS_SELECTED ) != ( pNMListView->uNewState & LVIS_SELECTED ) )
		{
			if ( pNMListView->uNewState & LVIS_SELECTED )
			{
				CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( pNMListView->iItem );
				if ( pMap != NULL )
				{
					CString strValue;

					strValue = pMap->GetAttributeValue( _T("ID") );
					m_wndHost.SetWindowText( (LPCTSTR)strValue );
					strValue = pMap->GetAttributeValue( _T("LinkId") );
					m_wndLinkId.SetWindowText( (LPCTSTR)strValue );
					strValue = pMap->GetAttributeValue( _T("LocalId") );
					m_wndLocalId.SetWindowText( (LPCTSTR)strValue );
					
					m_wndDelete.EnableWindow( TRUE );
				}
			}
		}
	}
}

void CAnalogProfilePage::OnAdd() 
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	if ( CXMLElement* pMap = m_pXML->AddElement( _T("Map") ) )
	{
		m_wndLinkId.GetWindowText( strValue );
		pMap->AddAttribute( _T("LinkId"), strValue );
		
		int nItem = m_wndList.InsertItem(
			LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), strValue, 0, 0, 28, (LPARAM)pMap );
		
		m_wndHost.GetWindowText( strValue );
		pMap->AddAttribute( _T("ID"), strValue );
		m_wndList.SetItemText( nItem, 1, strValue );
		
		m_wndLocalId.GetWindowText( strValue );
		pMap->AddAttribute( _T("LocalId"), strValue );
		m_wndList.SetItemText( nItem, 2, strValue );
	}
}

void CAnalogProfilePage::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndDelete.EnableWindow( FALSE );
}
