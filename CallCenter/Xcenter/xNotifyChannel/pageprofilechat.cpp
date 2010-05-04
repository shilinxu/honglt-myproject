//
// PageProfileChat.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "PageProfileChat.h"
#include "CoolInterface.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CChatProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CChatProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CChatProfilePage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	ON_CBN_EDITCHANGE(IDC_ROOM_OWNER, OnEditChangeRoomOwner)
	ON_CBN_SELCHANGE(IDC_ROOM_OWNER, OnSelChangeRoomOwner)
	ON_BN_CLICKED(IDC_OWNER_ADD, OnOwnerAdd)
	ON_BN_CLICKED(IDC_OWNER_REMOVE, OnOwnerRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChatProfilePage property page

CChatProfilePage::CChatProfilePage(CXMLElement* pXML)
	: CSettingsPage( CChatProfilePage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CChatProfilePage)
	//}}AFX_DATA_INIT
}

CChatProfilePage::~CChatProfilePage()
{
}

void CChatProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChatProfilePage)
	DDX_Control(pDX, IDC_ROOM_ID, m_wndRoomId);
	DDX_Control(pDX, IDC_MEMBER, m_wndMember);
	DDX_Control(pDX, IDC_HEARER, m_wndHearer);
	DDX_Control(pDX, IDC_ROOM_DESC, m_wndDesc);
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_ROOM_OWNER, m_wndOwner);
	DDX_Control(pDX, IDC_OWNER_ADD, m_wndExtAdd);
	DDX_Control(pDX, IDC_OWNER_REMOVE, m_wndExtRemove);
	DDX_Control(pDX, IDC_MEET_GROUP, m_wndMeetGrp);
	DDX_Control(pDX, IDC_MEET_ONLY, m_wndMeetOly);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CChatProfilePage message handlers

BOOL CChatProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	m_wndList.InsertColumn( 0, _T("房间"), LVCFMT_LEFT, 60, -1 );
	m_wndList.InsertColumn( 1, _T("成员数"), LVCFMT_LEFT, 68, 0 );
	m_wndList.InsertColumn( 2, _T("听众数"), LVCFMT_LEFT, 68, 1 );
	m_wndList.InsertColumn( 3, _T("房主"), LVCFMT_LEFT, 180, 2 );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	if ( m_pXML ) LoadXML();
	OnEditChangeRoomOwner();
	OnSelChangeRoomOwner();
	
	m_wndDelete.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChatProfilePage::OnOK()
{
//	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CChatProfilePage::LoadXML()
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
				CoolInterface.ImageForID(ID_TAB_HOME), (LPARAM)pMap );
			
			if ( pAttri = pMap->GetAttribute( _T("MmbrNum") ) )
			{
				m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			}
			if ( pAttri = pMap->GetAttribute( _T("AuditNum") ) )
			{
				m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
			}
			if ( pAttri = pMap->GetAttribute( _T("RoomOwner") ) )
			{
				m_wndList.SetItemText( nItem, 3, pAttri->GetValue() );
			}
		}
	}

	return TRUE;
}

void CChatProfilePage::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
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
					m_wndRoomId.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("MmbrNum") );
					m_wndMember.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("AuditNum") );
					m_wndHearer.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("RoomOwner") );
					FormatComboBox( strValue, m_wndOwner );
					m_wndOwner.SetCurSel( 0 );
#if 1				
					strValue = pMap->GetAttributeValue( _T("Desc") );
					m_wndDesc.SetWindowText( (LPCTSTR)strValue );
#endif
					strValue = pMap->GetAttributeValue( _T("MeetGroup") );
					m_wndMeetGrp.SetWindowText( (LPCTSTR)strValue );
					
					strValue = pMap->GetAttributeValue( _T("MeetOnly") );
					m_wndMeetOly.SetCheck( _ttoi( strValue ) );
					
					OnEditChangeRoomOwner();
					m_wndDelete.EnableWindow( TRUE );
				}
			}
		}
	}
}

void CChatProfilePage::OnAdd() 
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	if ( CXMLElement* pMap = m_pXML->AddElement( _T("Map") ) )
	{
		m_wndRoomId.GetWindowText( strValue );
		pMap->AddAttribute( _T("ID"), strValue );
		
		int nItem = m_wndList.InsertItem(
			LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), strValue, 0, 0,
			CoolInterface.ImageForID(ID_TAB_HOME), (LPARAM)pMap );
		
		m_wndMember.GetWindowText( strValue );
		pMap->AddAttribute( _T("MmbrNum"), strValue );
		m_wndList.SetItemText( nItem, 1, strValue );
		
		m_wndHearer.GetWindowText( strValue );
		pMap->AddAttribute( _T("AuditNum"), strValue );
		m_wndList.SetItemText( nItem, 2, strValue );
		
		m_wndOwner.GetWindowText( strValue );
		CString strText; FormatString( strText, m_wndOwner );
		pMap->AddAttribute( _T("RoomOwner"), strText );
		m_wndList.SetItemText( nItem, 3, strText );
		
		m_wndDesc.GetWindowText( strValue );
		pMap->AddAttribute( _T("Desc"), strValue );
//		m_wndList.SetItemText( nItem, 4, strValue );
		
		m_wndMeetGrp.GetWindowText( strValue );
		pMap->AddAttribute( _T("MeetGroup"), strValue );
		
		strValue.Format( "%i", m_wndMeetOly.GetCheck() );
		pMap->AddAttribute( _T("MeetOnly"), strValue );
	}
}

void CChatProfilePage::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndDelete.EnableWindow( FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CChatProfilePage owner's message

void CChatProfilePage::OnEditChangeRoomOwner() 
{
	m_wndExtAdd.EnableWindow( m_wndOwner.GetWindowTextLength() > 0 );
}

void CChatProfilePage::OnSelChangeRoomOwner() 
{
	m_wndExtRemove.EnableWindow( m_wndOwner.GetCurSel() >= 0 );
}

void CChatProfilePage::OnOwnerAdd() 
{
	CString strExist;
	m_wndOwner.GetWindowText( strExist );

	strExist.TrimLeft(); strExist.TrimRight();
	if ( strExist.IsEmpty() ) return;

	if ( m_wndOwner.FindString( -1, strExist ) >= 0 ) return;

	m_wndOwner.AddString( strExist );
	m_wndOwner.SetWindowText( _T("") );
	
	CString strText;
	FormatString( strText, m_wndOwner );
	
	int nItem = m_wndList.GetNextItem( -1, LVNI_FOCUSED );
	CXMLElement* pXML = ( nItem >= 0 ) ? (CXMLElement*)m_wndList.GetItemData( nItem ) : NULL;
	
	if ( pXML ) pXML->AddAttribute( _T("RoomOwner"), strText );
}

void CChatProfilePage::OnOwnerRemove() 
{
	int nItem = m_wndOwner.GetCurSel();
	if ( nItem >= 0 ) m_wndOwner.DeleteString( nItem );
	m_wndExtRemove.EnableWindow( FALSE );
	
	CString strText;
	FormatString( strText, m_wndOwner );
	
	nItem = m_wndList.GetNextItem( -1, LVNI_FOCUSED );
	CXMLElement* pXML = ( nItem >= 0 ) ? (CXMLElement*)m_wndList.GetItemData( nItem ) : NULL;
	
	if ( pXML ) pXML->AddAttribute( _T("RoomOwner"), strText );
}

void CChatProfilePage::FormatComboBox(LPCTSTR lpszText, CComboBox& pCombo)
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

void CChatProfilePage::FormatString(CString& strText, CComboBox& pCombo)
{
	for ( int nItem = 0 ; nItem < pCombo.GetCount() ; nItem++ )
	{
		CString str;
		pCombo.GetLBText( nItem, str );
		
		str.TrimLeft(); str.TrimRight();
		if ( ! str.IsEmpty() ) strText += str + '|';
	}
}
