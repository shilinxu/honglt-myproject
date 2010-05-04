//
// PageSettingsSecure.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "Settings.h"
#include "CoolInterface.h"
#include "XML.h"

#include "PageSettingsSecure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSecureSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CSecureSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CSecureSettingsPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSecureSettingsPage property page

CSecureSettingsPage::CSecureSettingsPage(CXMLElement* pXML)
	: CSettingsPage( CSecureSettingsPage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CSecureSettingsPage)
	//}}AFX_DATA_INIT
}

CSecureSettingsPage::~CSecureSettingsPage()
{
}

void CSecureSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSecureSettingsPage)
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_DESC, m_wndDesc);
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_ACTION, m_wndAction);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSecureSettingsPage message handlers

BOOL CSecureSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	CRect rc;
	m_wndList.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL ) + 1;

	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	m_wndList.InsertColumn( 0, _T("Host"), LVCFMT_LEFT, rc.right / 2, -1 );
	m_wndList.InsertColumn( 1, _T("Date"), LVCFMT_LEFT, rc.right / 2, 0 );

	m_wndList.SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE,
		LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );

	if ( m_pXML ) LoadXML();
	
	m_wndDelete.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSecureSettingsPage::OnOK()
{
	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CSecureSettingsPage::LoadXML()
{
	CString strValue;
	CXMLElement* pXML = NULL;
	CXMLAttribute* pAttri = NULL;
	
	if ( pXML = m_pXML->GetElementByName( _T("SecureRule") ) )
	{
		if ( pAttri = pXML->GetAttribute( _T("Action") ) )
		{
			strValue = pAttri->GetValue();
			m_wndAction.SelectString( -1, strValue );
		}
		
		for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
		{
			CXMLElement* pMap = pXML->GetNextElement( pos );
			if ( ! pMap->IsNamed( _T("Map") ) ) continue;

			if ( pAttri = pMap->GetAttribute( _T("Host") ) )
			{
				strValue = pAttri->GetValue();
				int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
					m_wndList.GetItemCount(), strValue, 0, 0, 4, (LPARAM)pMap );
				
				if ( pAttri = pMap->GetAttribute( _T("Date") ) )
				{
					m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
				}
			}
		}
	}

	return TRUE;
}

BOOL CSecureSettingsPage::EditXML() 
{
	CString strValue;
	CXMLElement* pXML = NULL;
	CXMLAttribute* pAttri = NULL;
	
	if ( pXML = m_pXML->GetElementByName( _T("SecureRule"), TRUE ) )
	{
		pAttri = pXML->GetAttribute( _T("Action") );
		if ( ! pAttri ) pAttri = pXML->AddAttribute( _T("Action") );
		int nIndex = m_wndAction.GetCurSel();
		if ( nIndex == CB_ERR )
			pAttri->Delete();
		else
		{
			m_wndAction.GetLBText( nIndex, strValue );
			pAttri->SetValue( strValue );
		}
	}

	return TRUE;
}

void CSecureSettingsPage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
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

					strValue = pMap->GetAttributeValue( _T("Host") );
					m_wndHost.SetWindowText( (LPCTSTR)strValue );
					strValue = pMap->GetAttributeValue( _T("Desc") );
					m_wndDesc.SetWindowText( (LPCTSTR)strValue );
					m_wndDelete.EnableWindow( TRUE );
				}
			}
		}
	}
}

void CSecureSettingsPage::OnAdd() 
{
	CString strValue;
	CXMLElement* pXML = NULL;
	CXMLAttribute* pAttri = NULL;
	
	if ( pXML = m_pXML->GetElementByName( _T("SecureRule"), TRUE ) )
	{
		if ( CXMLElement* pMap = pXML->AddElement( _T("Map") ) )
		{
			m_wndHost.GetWindowText( strValue );
			pMap->AddAttribute( _T("Host"), strValue );
			int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
					m_wndList.GetItemCount(), strValue, 0, 0, 4, (LPARAM)pMap );
			
			CTime tt = CTime::GetCurrentTime();
			strValue = tt.Format( "%YÄê%mÔÂ%dÈÕ" );
			pMap->AddAttribute( _T("Date"), strValue );
			m_wndList.SetItemText( nItem, 1, strValue );

			m_wndDesc.GetWindowText( strValue );
			pMap->AddAttribute( _T("Desc"), strValue );
		}
	}
}

void CSecureSettingsPage::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	if ( CXMLElement* pXML = m_pXML->GetElementByName( _T("SecureRule") ) )
	{
		CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
		m_wndList.DeleteItem( nItem );
		if ( pMap ) pMap->Delete();
	}
	
	m_wndDelete.EnableWindow( FALSE );
}
