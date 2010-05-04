//
// PageProfileUser.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "PageProfileUser.h"
#include "CoolInterface.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CUserProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CUserProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CUserProfilePage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUserProfilePage property page

CUserProfilePage::CUserProfilePage(CXMLElement* pXML)
	: CSettingsPage( CUserProfilePage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CUserProfilePage)
	//}}AFX_DATA_INIT
}

CUserProfilePage::~CUserProfilePage()
{
}

void CUserProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserProfilePage)
	DDX_Control(pDX, IDC_USER_ID, m_wndUserId);
	DDX_Control(pDX, IDC_USER_JOB, m_wndJob);
	DDX_Control(pDX, IDC_USER_HOST, m_wndHost);
	DDX_Control(pDX, IDC_USER_DESC, m_wndDesc);
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_USER_ACTION, m_wndAction);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CUserProfilePage message handlers

BOOL CUserProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	m_wndList.InsertColumn( 0, _T("坐席"), LVCFMT_LEFT, 48, -1 );
	m_wndList.InsertColumn( 1, _T("主机地址"), LVCFMT_LEFT, 92, 0 );
	m_wndList.InsertColumn( 2, _T("接入服务"), LVCFMT_LEFT, 200, 1 );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	if ( m_pXML ) LoadXML();
	
	m_wndDelete.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserProfilePage::OnOK()
{
//	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CUserProfilePage::LoadXML()
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
#if 0	
			if ( pAttri = pMap->GetAttribute( _T("Action") ) )
			{
				m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
			}
#endif
		}
	}

	return TRUE;
}

void CUserProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
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
					
					strValue = pMap->GetAttributeValue( _T("Host") );
					m_wndHost.SetWindowText( (LPCTSTR)strValue );
#if 1
					strValue = pMap->GetAttributeValue( _T("Action") );
					int nIndex = m_wndAction.SelectString( -1, strValue );
					if ( nIndex == CB_ERR ) m_wndAction.SetCurSel(-1);
#endif				
					strValue = pMap->GetAttributeValue( _T("Desc") );
					m_wndDesc.SetWindowText( (LPCTSTR)strValue );
					
					m_wndDelete.EnableWindow( TRUE );
				}
			}
		}
	}
}

void CUserProfilePage::OnAdd() 
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
		
		m_wndAction.GetWindowText( strValue );
		pMap->AddAttribute( _T("Action"), strValue );
//		m_wndList.SetItemText( nItem, 2, strValue );
		
		m_wndDesc.GetWindowText( strValue );
		pMap->AddAttribute( _T("Desc"), strValue );
//		m_wndList.SetItemText( nItem, 3, strValue );
	}
}

void CUserProfilePage::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndDelete.EnableWindow( FALSE );
}
