//
// PageProfileMonitor.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "CoolInterface.h"
#include "XML.h"

#include "PageProfileMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMonitorProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CMonitorProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CMonitorProfilePage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_PATH_BROWSE, OnPathBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonitorProfilePage property page

CMonitorProfilePage::CMonitorProfilePage(CXMLElement* pXML)
	: CSettingsPage( CMonitorProfilePage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CMonitorProfilePage)
	//}}AFX_DATA_INIT
}

CMonitorProfilePage::~CMonitorProfilePage()
{
}

void CMonitorProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonitorProfilePage)
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_PHONE, m_wndPhone);
	DDX_Control(pDX, IDC_CONTENT, m_wndContent);
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_PATH, m_wndPath);
	DDX_Control(pDX, IDC_HOST, m_wndHost);
	DDX_Control(pDX, IDC_PORT, m_wndPort);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorProfilePage message handlers

BOOL CMonitorProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	CRect rc;
	m_wndList.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL ) + 1;

	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	m_wndList.InsertColumn( 0, _T("主机"), LVCFMT_LEFT, 80, -1 );
	m_wndList.InsertColumn( 1, _T("端口"), LVCFMT_LEFT, 42, 0 );
	m_wndList.InsertColumn( 2, _T("程序"), LVCFMT_LEFT, rc.right - 122, 1 );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );

	if ( m_pXML ) LoadXML();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMonitorProfilePage::OnOK()
{
	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CMonitorProfilePage::LoadXML()
{
	if ( CXMLElement* pXML = m_pXML->GetElementByName( _T("Process") ) )
	{
		for ( POSITION posNext = pXML->GetElementIterator() ; posNext ; )
		{
			CXMLElement* pMap = pXML->GetNextElement( posNext );
			if ( ! pMap->IsNamed( _T("Map") ) ) continue;
			
			int nItem = m_wndList.GetItemCount();
			
			if ( CXMLAttribute* pAttri = pMap->GetAttribute( "Host" ) )
			{
				m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
					nItem, pAttri->GetValue(), 0, 0, 3, (LPARAM)pMap );
			}
			
			if ( CXMLAttribute* pAttri = pMap->GetAttribute( "Port" ) )
			{
				m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			}
			
			if ( CXMLAttribute* pAttri = pMap->GetAttribute( "Path" ) )
			{
				m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
			}
		}
	}
	
	if ( CXMLElement* pXML = m_pXML->GetElementByName( _T("ShortMessage") ) )
	{
		if ( CXMLElement* pXMLSub = pXML->GetElementByName( _T("PhoneNum") ) )
		{
			m_wndPhone.SetWindowText( pXMLSub->GetValue() );
		}
		if ( CXMLElement* pXMLSub = pXML->GetElementByName( _T("MsgContent") ) )
		{
			m_wndContent.SetWindowText( pXMLSub->GetValue() );
		}
	}
	
	return TRUE;
}

BOOL CMonitorProfilePage::EditXML() 
{
	if ( CXMLElement* pXML = m_pXML->GetElementByName( _T("Process"), TRUE ) )
	{
	}
	
	if ( CXMLElement* pXML = m_pXML->GetElementByName( _T("ShortMessage"), TRUE ) )
	{
		CString strText;
		m_wndPhone.GetWindowText( strText );
		
		if ( CXMLElement* pXMLSub = pXML->GetElementByName( _T("PhoneNum"), TRUE ) )
		{
			pXMLSub->SetValue( (LPCTSTR)strText );
		}
		
		m_wndContent.GetWindowText( strText );
		
		if ( CXMLElement* pXMLSub = pXML->GetElementByName( _T("MsgContent"), TRUE ) )
		{
			pXMLSub->SetValue( (LPCTSTR)strText );
		}
	}
	
	return TRUE;
}

void CMonitorProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	
	if ( pNMListView->iItem >= 0 )
	{
		if ( ( pNMListView->uOldState & LVIS_SELECTED ) != ( pNMListView->uNewState & LVIS_SELECTED ) )
		{
			if ( pNMListView->uNewState & LVIS_SELECTED )
			{
				if ( CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( pNMListView->iItem ) )
				{
					m_wndHost.SetWindowText( pMap->GetAttributeValue( _T("Host") ) );
					m_wndPort.SetWindowText( pMap->GetAttributeValue( _T("Port") ) );
					m_wndPath.SetWindowText( pMap->GetAttributeValue( _T("Path") ) );
				}
			}
		}
	}
}

void CMonitorProfilePage::OnAdd() 
{
	CString strValue;
	m_wndPath.GetWindowText( strValue );
	
	strValue.TrimLeft(); strValue.TrimRight();
	if ( strValue.IsEmpty() ) return;
	
	if ( CXMLElement* pXML = m_pXML->GetElementByName( _T("Process"), TRUE ) )
	{
		if ( CXMLElement* pMap = pXML->AddElement( _T("Map") ) )
		{
			CString strValue;
			m_wndHost.GetWindowText( strValue );
			
			CXMLAttribute* pAttri = pMap->AddAttribute( _T("Host"), strValue );
			int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
				m_wndList.GetItemCount(), pAttri->GetValue(), 0, 0, 3, (LPARAM)pMap );
			
			m_wndPort.GetWindowText( strValue );
			int nPort = _ttoi( strValue ); strValue.Format( "%i", nPort );
			pAttri = pMap->AddAttribute( _T("Port"), strValue );
			m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			
			m_wndPath.GetWindowText( strValue );
			pAttri = pMap->AddAttribute( _T("Path"), strValue );
			m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
		}
	}
}

void CMonitorProfilePage::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndDelete.EnableWindow( FALSE );
}

void CMonitorProfilePage::OnPathBrowse() 
{
	CString strValue;
	m_wndPath.GetWindowText( strValue );
	
	strValue.TrimLeft(); strValue.TrimRight();
	
	CFileDialog dlg( TRUE, _T("exe"), strValue , OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
		_T("Executable Files|*.exe;*.com|All Files|*.*||"), this );
	
	if ( dlg.DoModal() != IDOK ) return;
	
	m_wndPath.SetWindowText( dlg.GetPathName() );
}
