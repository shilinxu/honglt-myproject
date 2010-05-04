//
// PageProfileInter.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "PageProfileInter.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CInterProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CInterProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CInterProfilePage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInterProfilePage property page

CInterProfilePage::CInterProfilePage(CXMLElement* pXML)
	: CSettingsPage( CInterProfilePage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CInterProfilePage)
	//}}AFX_DATA_INIT
}

CInterProfilePage::~CInterProfilePage()
{
}

void CInterProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInterProfilePage)
	DDX_Control(pDX, IDC_LIMITED, m_wndLimited);
	DDX_Control(pDX, IDC_USER_VNET, m_wndVNet);
	DDX_Control(pDX, IDC_USER_ID, m_wndUserId);
	DDX_Control(pDX, IDC_USER_JOB, m_wndJob);
	DDX_Control(pDX, IDC_USER_HOST, m_wndHost);
	DDX_Control(pDX, IDC_LINK_ID, m_wndLinkId);
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CInterProfilePage message handlers

BOOL CInterProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	m_wndList.InsertColumn( 0, _T("坐席"), LVCFMT_LEFT, 52, -1 );
	m_wndList.InsertColumn( 1, _T("接入号码"), LVCFMT_LEFT, 92, 0 );
	m_wndList.InsertColumn( 2, _T("机主姓名"), LVCFMT_LEFT, 88, 1 );
	//m_wndList.InsertColumn( 3, _T("主叫显示"), LVCFMT_LEFT, 92, 2 );
	m_wndList.InsertColumn( 3, _T("虚拟短号"), LVCFMT_LEFT, 64, 3 );
	m_wndList.InsertColumn( 4, _T("限呼号段"), LVCFMT_LEFT, 88, 3 );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	if ( m_pXML ) LoadXML();
	
	m_wndDelete.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInterProfilePage::OnOK()
{
//	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CInterProfilePage::LoadXML()
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	for ( POSITION pos = m_pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = m_pXML->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		if ( pAttri = pMap->GetAttribute( _T("ID") ) )
		{
			strValue = pAttri->GetValue();
			int nItem = m_wndList.InsertItem( 
				LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
				m_wndList.GetItemCount(), strValue, 0, 0,
				CoolInterface.ImageForID(ID_TAB_USER), (LPARAM)pMap );
			
			if ( pAttri = pMap->GetAttribute( _T("Host") ) )
			{
				m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			}
			if ( pAttri = pMap->GetAttribute( _T("Job") ) )
			{
				m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
			}
			//if ( pAttri = pMap->GetAttribute( _T("Cap") ) )
			//{
			//	m_wndList.SetItemText( nItem, 3, pAttri->GetValue() );
			//}
			if ( pAttri = pMap->GetAttribute( _T("VNET") ) )
			{
				m_wndList.SetItemText( nItem, 3, pAttri->GetValue() );
			}
			if ( pAttri = pMap->GetAttribute( _T("Limited") ) )
			{
				m_wndList.SetItemText( nItem, 4, pAttri->GetValue() );
			}
		}
	}

	return TRUE;
}

void CInterProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
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
					m_wndUserId.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("Job") );
					m_wndJob.SetWindowText( (LPCTSTR)strValue );
					
					//strValue = pMap->GetAttributeValue( _T("Cap") );
					//m_wndCap.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("Host") );
					m_wndHost.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("VNET") );
					m_wndVNet.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("LinkId") );
					m_wndLinkId.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("Limited") );
					m_wndLimited.SetWindowText( (LPCTSTR)strValue );
					
					m_wndDelete.EnableWindow( TRUE );
				}
			}
		}
	}
}

void CInterProfilePage::OnAdd() 
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	if ( CXMLElement* pMap = m_pXML->AddElement( _T("Map") ) )
	{
		m_wndUserId.GetWindowText( strValue );
		pMap->AddAttribute( _T("ID"), strValue );
		
		int nItem = m_wndList.InsertItem(
			LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), strValue, 0, 0,
			CoolInterface.ImageForID(ID_TAB_USER), (LPARAM)pMap );
		
		m_wndHost.GetWindowText( strValue );
		pMap->AddAttribute( _T("Host"), strValue );
		m_wndList.SetItemText( nItem, 1, strValue );
		
		m_wndJob.GetWindowText( strValue );
		pMap->AddAttribute( _T("Job"), strValue );
		m_wndList.SetItemText( nItem, 2, strValue );
		
		//m_wndCap.GetWindowText( strValue );
		//pMap->AddAttribute( _T("Cap"), strValue );
		//m_wndList.SetItemText( nItem, 3, strValue );
		
		m_wndVNet.GetWindowText( strValue );
		pMap->AddAttribute( _T("VNET"), strValue );
		m_wndList.SetItemText( nItem, 3, strValue );
		
		m_wndLimited.GetWindowText( strValue );
		pMap->AddAttribute( _T("Limited"), strValue );
		
		m_wndLinkId.GetWindowText( strValue );
		pMap->AddAttribute( _T("LinkId"), strValue );
		
		m_wndList.SetItemText( nItem, 4, strValue );
	}
}

void CInterProfilePage::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndDelete.EnableWindow( FALSE );
}
