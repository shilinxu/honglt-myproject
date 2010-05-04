//
// DlgChatProperties.cpp
//

#include "StdAfx.h"
#include "Xcenter.h"
#include "DlgChatProperties.h"
#include "CoolInterface.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CChatPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CChatPropertiesDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChangedList)
	ON_CBN_EDITCHANGE(IDC_ROOM_OWNER, OnEditChangeRoomOwner)
	ON_CBN_SELCHANGE(IDC_ROOM_OWNER, OnSelChangeRoomOwner)
	ON_BN_CLICKED(IDC_OWNER_ADD, OnOwnerAdd)
	ON_BN_CLICKED(IDC_OWNER_REMOVE, OnOwnerRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatPropertiesDlg dialog

CChatPropertiesDlg::CChatPropertiesDlg(CXMLElement* pXML, CWnd* pParent) : CDialog(CChatPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChatPropertiesDlg)
	//}}AFX_DATA_INIT
	
	m_pXML	= pXML;
}

void CChatPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChatPropertiesDlg)
	DDX_Control(pDX, IDC_LIST, m_wndList);
	DDX_Control(pDX, IDC_ROOM_ID, m_wndRoomId);
	DDX_Control(pDX, IDC_MEMBER, m_wndMember);
	DDX_Control(pDX, IDC_AUDITOR, m_wndAuditor);
	DDX_Control(pDX, IDC_ROOM_OWNER, m_wndOwner);
	DDX_Control(pDX, IDC_MEET_GROUP, m_wndMeetGrp);
	DDX_Control(pDX, IDC_MEET_ONLY, m_wndMeetOly);
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_OWNER_ADD, m_wndExtAdd);
	DDX_Control(pDX, IDC_OWNER_REMOVE, m_wndExtRemove);
	DDX_Control(pDX, IDC_ROOM_DESC, m_wndDesc);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CChatPropertiesDlg message handlers

BOOL CChatPropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	m_wndList.InsertColumn( 0, _T("房间"), LVCFMT_LEFT, 60, -1 );
	m_wndList.InsertColumn( 1, _T("成员数"), LVCFMT_LEFT, 68, 0 );
	m_wndList.InsertColumn( 2, _T("听众数"), LVCFMT_LEFT, 68, 1 );
	m_wndList.InsertColumn( 3, _T("房主"), LVCFMT_LEFT, 180, 2 );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	if ( m_pXML ) LoadXML( m_pXML );
	
	OnEditChangeRoomOwner();
	OnSelChangeRoomOwner();
	
	m_wndDelete.EnableWindow( FALSE );

	return TRUE;
}

BOOL CChatPropertiesDlg::LoadXML(CXMLElement* pParent)
{
	for ( POSITION pos = pParent->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pParent->GetNextElement( pos );
		if ( ! pXML->IsNamed( _T("Chat") ) ) continue;
		
		if ( CXMLAttribute* pAttri = pXML->GetAttribute( _T("ID") ) )
		{
			CString strValue = pAttri->GetValue();
			
			int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
				m_wndList.GetItemCount(), strValue, 0, 0, 0, (LPARAM)pXML );
			
			if ( pAttri = pXML->GetAttribute( _T("MmbrNum") ) )
			{
				m_wndList.SetItemText( nItem, 1, pAttri->GetValue() );
			}
			if ( pAttri = pXML->GetAttribute( _T("AuditNum") ) )
			{
				m_wndList.SetItemText( nItem, 2, pAttri->GetValue() );
			}
			if ( pAttri = pXML->GetAttribute( _T("RoomOwner") ) )
			{
				m_wndList.SetItemText( nItem, 3, pAttri->GetValue() );
			}
		}
	}
	
	return TRUE;
}

void CChatPropertiesDlg::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	
	if ( pNMListView->iItem < 0 || ( pNMListView->uNewState & LVIS_SELECTED ) != LVIS_SELECTED ) return;
	
	if ( ( pNMListView->uOldState & LVIS_SELECTED ) == ( pNMListView->uNewState & LVIS_SELECTED ) ) return;
	
	if ( CXMLElement* pXML = (CXMLElement*)m_wndList.GetItemData( pNMListView->iItem ) )
	{
		CString strValue;
		
		strValue = pXML->GetAttributeValue( _T("ID") );
		m_wndRoomId.SetWindowText( (LPCTSTR)strValue );
		
		strValue = pXML->GetAttributeValue( _T("MmbrNum") );
		m_wndMember.SetWindowText( (LPCTSTR)strValue );
		
		strValue = pXML->GetAttributeValue( _T("AuditNum") );
		m_wndAuditor.SetWindowText( (LPCTSTR)strValue );
		
		strValue = pXML->GetAttributeValue( _T("RoomOwner") );
		FormatComboBox( strValue, m_wndOwner );
		m_wndOwner.SetCurSel( 0 );
#if 1
		strValue = pXML->GetAttributeValue( _T("MeetGroup") );
		m_wndMeetGrp.SetWindowText( (LPCTSTR)strValue );
		
		strValue = pXML->GetAttributeValue( _T("MeetOnly") );
		m_wndMeetOly.SetCheck( _ttoi( strValue ) );
#endif
		strValue = pXML->GetAttributeValue( _T("Desc") );
		m_wndDesc.SetWindowText( (LPCTSTR)strValue );
		
		OnEditChangeRoomOwner();
		m_wndDelete.EnableWindow( TRUE );
	}
}

void CChatPropertiesDlg::OnAdd() 
{
	CString strValue;
	CXMLAttribute* pAttri = NULL;
	
	if ( CXMLElement* pXML = m_pXML->AddElement( _T("Chat") ) )
	{
		m_wndRoomId.GetWindowText( strValue );
		pXML->AddAttribute( _T("ID"), strValue );
		
		int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM,
			m_wndList.GetItemCount(), strValue, 0, 0, 0, (LPARAM)pXML );
		
		m_wndMember.GetWindowText( strValue );
		pXML->AddAttribute( _T("MmbrNum"), strValue );
		m_wndList.SetItemText( nItem, 1, strValue );
		
		m_wndAuditor.GetWindowText( strValue );
		pXML->AddAttribute( _T("AuditNum"), strValue );
		m_wndList.SetItemText( nItem, 2, strValue );
		
		m_wndOwner.GetWindowText( strValue );
		CString strText; FormatString( strText, m_wndOwner );
		pXML->AddAttribute( _T("RoomOwner"), strText );
		m_wndList.SetItemText( nItem, 3, strText );
		
		m_wndDesc.GetWindowText( strValue );
		pXML->AddAttribute( _T("Desc"), strValue );
//		m_wndList.SetItemText( nItem, 4, strValue );
		
		m_wndMeetGrp.GetWindowText( strValue );
		pXML->AddAttribute( _T("MeetGroup"), strValue );
		
		strValue.Format( "%i", m_wndMeetOly.GetCheck() );
		pXML->AddAttribute( _T("MeetOnly"), strValue );
	}
}

void CChatPropertiesDlg::OnDelete() 
{
	int nItem = m_wndList.GetNextItem( -1, LVNI_SELECTED );
	if ( nItem == -1 ) return;

	CXMLElement* pMap = (CXMLElement*)m_wndList.GetItemData( nItem );
	m_wndList.DeleteItem( nItem );
	if ( pMap ) pMap->Delete();
	
	m_wndDelete.EnableWindow( FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CChatPropertiesDlg owner's message

void CChatPropertiesDlg::OnEditChangeRoomOwner() 
{
	m_wndExtAdd.EnableWindow( m_wndOwner.GetWindowTextLength() > 0 );
}

void CChatPropertiesDlg::OnSelChangeRoomOwner() 
{
	m_wndExtRemove.EnableWindow( m_wndOwner.GetCurSel() >= 0 );
}

void CChatPropertiesDlg::OnOwnerAdd() 
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

void CChatPropertiesDlg::OnOwnerRemove() 
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

void CChatPropertiesDlg::FormatComboBox(LPCTSTR lpszText, CComboBox& pCombo)
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

void CChatPropertiesDlg::FormatString(CString& strText, CComboBox& pCombo)
{
	for ( int nItem = 0 ; nItem < pCombo.GetCount() ; nItem++ )
	{
		CString str;
		pCombo.GetLBText( nItem, str );
		
		str.TrimLeft(); str.TrimRight();
		if ( ! str.IsEmpty() ) strText += str + '|';
	}
}
