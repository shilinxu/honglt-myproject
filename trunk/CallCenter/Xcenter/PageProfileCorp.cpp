//
// PageProfileCorp.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "PageProfileCorp.h"
#include "CoolInterface.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCorpProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CCorpProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CCorpProfilePage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	ON_BN_CLICKED(IDC_PATH_BLACK_BROWSE, OnPathBlkBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCorpProfilePage property page

CCorpProfilePage::CCorpProfilePage(CXMLElement* pXML)
	: CSettingsPage( CCorpProfilePage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CCorpProfilePage)
	//}}AFX_DATA_INIT
}

CCorpProfilePage::~CCorpProfilePage()
{
}

void CCorpProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCorpProfilePage)
	DDX_Control(pDX, IDC_LIMITED, m_wndLimited);
	DDX_Control(pDX, IDC_LINKID, m_wndLinkId);
	DDX_Control(pDX, IDC_CORPID, m_wndCorpId);
	DDX_Control(pDX, IDC_USERID, m_wndUserId);
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_PATH_BLACK, m_wndPathBlk);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCorpProfilePage message handlers

BOOL CCorpProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	m_wndList.InsertColumn( 0, _T("企业ID"), LVCFMT_LEFT, 52, -1 );
	m_wndList.InsertColumn( 1, _T("接入号码"), LVCFMT_LEFT, 92, 0 );
	m_wndList.InsertColumn( 2, _T("本地PCM"), LVCFMT_LEFT, 60, 1 );
	m_wndList.InsertColumn( 3, _T("外呼坐席"), LVCFMT_LEFT, 80, 2 );
	m_wndList.InsertColumn( 4, _T("限呼号段"), LVCFMT_LEFT, 88, 3 );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	if ( m_pXML ) LoadXML();
	
	m_wndDelete.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCorpProfilePage::OnOK()
{
//	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CCorpProfilePage::LoadXML()
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
			int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
				m_wndList.GetItemCount(), strValue, 0, 0, 28, (LPARAM)pMap );
			
			if ( pAttri = pMap->GetAttribute( _T("Host") ) )
			{
				m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			}
			
			if ( pAttri = pMap->GetAttribute( _T("LinkId") ) )
			{
				m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
			}
			
			if ( pAttri = pMap->GetAttribute( _T("UserId") ) )
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

void CCorpProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
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
					m_wndCorpId.SetWindowText( (LPCTSTR)strValue );
					strValue = pMap->GetAttributeValue( _T("Host") );
					m_wndHost.SetWindowText( (LPCTSTR)strValue );
					strValue = pMap->GetAttributeValue( _T("LinkId") );
					m_wndLinkId.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("UserId") );
					m_wndUserId.SetWindowText( (LPCTSTR)strValue );
					strValue = pMap->GetAttributeValue( _T("Limited") );
					m_wndLimited.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("BlackList") );
					m_wndPathBlk.SetWindowText( (LPCTSTR)strValue );
					
					m_wndDelete.EnableWindow( TRUE );
				}
			}
		}
	}
}

void CCorpProfilePage::OnAdd() 
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	if ( CXMLElement* pMap = m_pXML->AddElement( _T("Map") ) )
	{
		m_wndCorpId.GetWindowText( strValue );
		pMap->AddAttribute( _T("ID"), strValue );
		int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), strValue, 0, 0, 28, (LPARAM)pMap );
		
		m_wndHost.GetWindowText( strValue );
		pMap->AddAttribute( _T("Host"), strValue );
		m_wndList.SetItemText( nItem, 1, strValue );
		
		m_wndLinkId.GetWindowText( strValue );
		pMap->AddAttribute( _T("LinkId"), strValue );
		m_wndList.SetItemText( nItem, 2, strValue );
		
		m_wndUserId.GetWindowText( strValue );
		pMap->AddAttribute( _T("UserId"), strValue );
		m_wndList.SetItemText( nItem, 3, strValue );
		
		m_wndLimited.GetWindowText( strValue );
		pMap->AddAttribute( _T("Limited"), strValue );
		m_wndList.SetItemText( nItem, 4, strValue );
		
		m_wndPathBlk.GetWindowText( strValue );
		pMap->AddAttribute( _T("BlackList"), strValue );
	}
}

void CCorpProfilePage::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndDelete.EnableWindow( FALSE );
}

void CCorpProfilePage::OnPathBlkBrowse() 
{
	CFileDialog dlg( TRUE, _T("txt"), NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
		_T("Text Files|*.txt|HTML Files|*.htm;*.html|All Files|*.*||"), this );
	
	if ( dlg.DoModal() != IDOK ) return;
	
	m_wndPathBlk.SetWindowText( dlg.GetPathName() );
}
