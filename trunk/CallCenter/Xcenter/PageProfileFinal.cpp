//
// PageProfileCorp.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "PageProfileFinal.h"
#include "CoolInterface.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CFinalProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CFinalProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CFinalProfilePage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFinalProfilePage property page

CFinalProfilePage::CFinalProfilePage(CXMLElement* pXML)
	: CSettingsPage( CFinalProfilePage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CFinalProfilePage)
	//}}AFX_DATA_INIT
}

CFinalProfilePage::~CFinalProfilePage()
{
}

void CFinalProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFinalProfilePage)
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_MODEL, m_wndModel);
	DDX_Control(pDX, IDC_NAME, m_wndName);
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CFinalProfilePage message handlers

BOOL CFinalProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	m_wndList.InsertColumn( 0, _T("业务"), LVCFMT_LEFT, 92, -1 );
	m_wndList.InsertColumn( 1, _T("模型"), LVCFMT_LEFT, 100, 0 );
	m_wndList.InsertColumn( 2, _T("描述"), LVCFMT_LEFT, 128, 1 );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	if ( m_pXML ) LoadXML();
	
	m_wndDelete.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFinalProfilePage::OnOK()
{
//	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CFinalProfilePage::LoadXML()
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	for ( POSITION pos = m_pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pMap = m_pXML->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), _T(""), 0, 0, 28, (LPARAM)pMap );
		
		if ( pAttri = pMap->GetAttribute( _T("Host") ) )
		{
			m_wndList.SetItemText( nItem, 0, pAttri->GetValue() );
		}
		
		if ( pAttri = pMap->GetAttribute( _T("Module") ) )
		{
			m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
		}
		
		if ( pAttri = pMap->GetAttribute( _T("Name") ) )
		{
			m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
		}
	}

	return TRUE;
}

void CFinalProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
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
					//m_wndHost.SetWindowText( (LPCTSTR)strValue );
					MutiHostToCombo( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("Module") );
					m_wndModel.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("Name") );
					m_wndName.SetWindowText( (LPCTSTR)strValue );
					
					m_wndDelete.EnableWindow( TRUE );
				}
			}
		}
	}
}

void CFinalProfilePage::OnAdd() 
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	if ( CXMLElement* pMap = m_pXML->AddElement( _T("Map") ) )
	{
		int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), _T(""), 0, 0, 28, (LPARAM)pMap );
		
		//m_wndHost.GetWindowText( strValue );
		MutiHostFromCombo( strValue );
		pMap->AddAttribute( _T("Host"), strValue );
		m_wndList.SetItemText( nItem, 0, strValue );
		
		m_wndModel.GetWindowText( strValue );
		pMap->AddAttribute( _T("Module"), strValue );
		m_wndList.SetItemText( nItem, 1, strValue );
		
		m_wndName.GetWindowText( strValue );
		pMap->AddAttribute( _T("Name"), strValue );
		m_wndList.SetItemText( nItem, 2, strValue );
	}
}

void CFinalProfilePage::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndDelete.EnableWindow( FALSE );
}

void CFinalProfilePage::MutiHostToCombo(LPCTSTR lpszHost)
{
	m_wndHost.ResetContent();
	CString strValue = lpszHost;
	
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		m_wndHost.AddString( (LPCTSTR)str );
	}
	
	m_wndHost.SetCurSel( 0 );
}

void CFinalProfilePage::MutiHostFromCombo(CString& strHost)
{
	strHost.Empty();
	
	CString strValue;
	m_wndHost.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	
	if ( m_wndHost.FindStringExact(-1, strValue) == CB_ERR )
	{
		MutiHostToCombo( strValue );
	}
	
	for ( int i = 0; i < m_wndHost.GetCount(); i++ )
	{
		m_wndHost.GetLBText( i, strValue );
		
		if ( i > 0 ) strHost += '|';
		strHost += strValue;
	}
}
