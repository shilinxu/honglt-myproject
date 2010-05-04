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
	DDX_Control(pDX, IDC_PLAY_NAME, m_wndName);
	DDX_Control(pDX, IDC_PLAY_INDEX, m_wndIndex);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPlayProfilePage message handlers

BOOL CPlayProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndList.InsertColumn( 0, _T("按键代码"), LVCFMT_LEFT, 80, -1 );
	m_wndList.InsertColumn( 1, _T("节目名称"), LVCFMT_LEFT, 220, 0 );
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	if ( m_pXML ) LoadXML();
	
	m_wndDelete.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPlayProfilePage::OnOK()
{
//	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CPlayProfilePage::LoadXML()
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
				CoolInterface.ImageForID(ID_REPORT_MUSIC), (LPARAM)pMap );
			
			if ( pAttri = pMap->GetAttribute( _T("Play") ) )
			{
				m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			}
		}
	}

	return TRUE;
}

void CPlayProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
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
					m_wndIndex.SetWindowText( (LPCTSTR)strValue );
					strValue = pMap->GetAttributeValue( _T("Play") );
					m_wndName.SetWindowText( (LPCTSTR)strValue );
					
					m_wndDelete.EnableWindow( TRUE );
				}
			}
		}
	}
}

void CPlayProfilePage::OnAdd() 
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	if ( CXMLElement* pMap = m_pXML->AddElement( _T("Map") ) )
	{
		m_wndIndex.GetWindowText( strValue );
		pMap->AddAttribute( _T("ID"), strValue );
		
		int nItem = m_wndList.InsertItem(
			LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), strValue, 0, 0,
			CoolInterface.ImageForID(ID_REPORT_MUSIC), (LPARAM)pMap );
		
		m_wndName.GetWindowText( strValue );
		pMap->AddAttribute( _T("Play"), strValue );
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
