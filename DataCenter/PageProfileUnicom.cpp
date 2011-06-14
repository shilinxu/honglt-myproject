//
// PageProfileUnicom.cpp
//

#include "stdafx.h"
#include "MakeReport.h"
#include "PageProfileUnicom.h"
#include "CoolInterface.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CUnicomProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CUnicomProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CUnicomProfilePage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUnicomProfilePage property page

CUnicomProfilePage::CUnicomProfilePage(CXMLElement* pXML)
	: CSettingsPage( CUnicomProfilePage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CUnicomProfilePage)
	//}}AFX_DATA_INIT
}

CUnicomProfilePage::~CUnicomProfilePage()
{
}

void CUnicomProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnicomProfilePage)
	DDX_Control(pDX, IDC_TALK, m_wndTalk);
	DDX_Control(pDX, IDC_USERID, m_wndUserId);
	DDX_Control(pDX, IDC_NEWS, m_wndNews);
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_KEY, m_wndKey);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CUnicomProfilePage message handlers

BOOL CUnicomProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	m_wndList.InsertColumn( 0, _T("业务代码"), LVCFMT_LEFT, 80, -1 );
	m_wndList.InsertColumn( 1, _T("通话费"), LVCFMT_LEFT, 60, 0 );
	m_wndList.InsertColumn( 2, _T("信息费"), LVCFMT_LEFT, 60, 1 );
	m_wndList.InsertColumn( 3, _T("栏目费"), LVCFMT_LEFT, 180, 2 );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	if ( m_pXML ) LoadXML();
	
	m_wndDelete.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUnicomProfilePage::OnOK()
{
//	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CUnicomProfilePage::LoadXML()
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
			
			if ( pAttri = pMap->GetAttribute( _T("Talk") ) )
			{
				m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			}
			
			if ( pAttri = pMap->GetAttribute( _T("News") ) )
			{
				m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
			}
			
			if ( pAttri = pMap->GetAttribute( _T("Key") ) )
			{
				m_wndList.SetItemText( nItem, 3, pAttri->GetValue() );
			}
		}
	}

	return TRUE;
}

void CUnicomProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
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
					m_wndUserId.SetCurSel( m_wndUserId.FindStringExact( -1, (LPCTSTR)strValue ) );
					strValue = pMap->GetAttributeValue( _T("Talk") );
					m_wndTalk.SetWindowText( (LPCTSTR)strValue );
					strValue = pMap->GetAttributeValue( _T("News") );
					m_wndNews.SetWindowText( (LPCTSTR)strValue );
					strValue = pMap->GetAttributeValue( _T("Key") );
					m_wndKey.SetWindowText( (LPCTSTR)strValue );
					
					m_wndDelete.EnableWindow( TRUE );
				}
			}
		}
	}
}

void CUnicomProfilePage::OnAdd() 
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	if ( CXMLElement* pMap = m_pXML->AddElement( _T("Map") ) )
	{
		m_wndUserId.GetWindowText( strValue );
		pMap->AddAttribute( _T("ID"), strValue );
		int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), strValue, 0, 0, 28, (LPARAM)pMap );
		
		m_wndTalk.GetWindowText( strValue );
		pMap->AddAttribute( _T("Talk"), strValue );
		m_wndList.SetItemText( nItem, 1, strValue );
		
		m_wndNews.GetWindowText( strValue );
		pMap->AddAttribute( _T("News"), strValue );
		m_wndList.SetItemText( nItem, 2, strValue );
		
		m_wndKey.GetWindowText( strValue );
		pMap->AddAttribute( _T("Key"), strValue );
		m_wndList.SetItemText( nItem, 3, strValue );
	}
}

void CUnicomProfilePage::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndDelete.EnableWindow( FALSE );
}
