//
// DlgGrouping.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "DlgGrouping.h"
#include "CoolInterface.h"

#include "Network.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CGroupingDlg, CDialog)
	//{{AFX_MSG_MAP(CGroupingDlg)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_GROUP, OnSelChangeGroup)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST, OnBeginLabelEditList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST, OnEndLabelEditList)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnNMClickList)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST, OnCustomDrawList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CItemEdit, CEdit)
	//{{AFX_MSG_MAP(CSubEdit)
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define	METHOD_INSERT	_T("Insert")
#define	METHOD_DELETE	_T("Delete")
#define	METHOD_UPDATE	_T("Update")

/////////////////////////////////////////////////////////////////////////////
// CGroupingDlg construction

CGroupingDlg::CGroupingDlg(CWnd* pParent) : CDialog(CGroupingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGroupingDlg)
	//}}AFX_DATA_INIT
	
	m_pXML		= NULL;
	m_pXMLMethod= NULL;
}

CGroupingDlg::~CGroupingDlg()
{
	if ( m_pXML ) delete m_pXML;
	if ( m_pXMLMethod ) delete m_pXMLMethod;
}

void CGroupingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupingDlg)
	DDX_Control(pDX, IDC_GROUP, m_wndGroup);
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CGroupingDlg message handlers

BOOL CGroupingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_wndList.InsertColumn( 0, _T("ºÅÂë"), LVCFMT_LEFT, 108, -1 );
	m_wndList.InsertColumn( 1, _T("ÐÕÃû"), LVCFMT_LEFT, 144, 0 );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );
	
	UpdateWindow();
	
	CWaitCursor pCursor;
	OnSyncOrderRelation();
	
	return TRUE;
}

void CGroupingDlg::OnPaint() 
{
	CPaintDC dc( this );
	CRect rc;
	
	GetClientRect( &rc );
	rc.top += 51;
	
	dc.Draw3dRect( 0, 50, rc.right + 1, 0,
		RGB( 128, 128, 128 ), RGB( 128, 128, 128 ) );
	dc.FillSolidRect( &rc, m_crWhite );
}

HBRUSH CGroupingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
	
	pDC->SetBkColor( m_crWhite );
	
	return m_brWhite;
}

void CGroupingDlg::OnOK()
{
	if ( m_pXMLMethod )
	ProcessMethod( m_pXMLMethod );
	
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CGroupingDlg custom message handlers

void CGroupingDlg::OnSyncOrderRelation()
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "select" );
	
	CXMLElement* pSync = pXML->AddElement( "column_expression" );
	pSync->SetValue( "a.xphone, a.xname, a.xgroupnum, b.xgroupname" );
	
	pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xphonemeetinggroupmore a"
		" inner join xphonemeetinggroup b on a.xjsjbh=b.xjsjbh"
			" and a.xgroupnum=b.xgroupnum" );
	
	CString str;
	str.Format( "a.xjsjbh in (select xjsjbh from xuser_info "
				"where xphone = '%s')", Network.m_xUserId );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView(pXML) );
	delete pXML; pXML = NULL;
	
	if ( ! Network.CallWorker( &pXML, INFINITE ) )
	{
		OnSyncDataViewResp( pXML );
		//pXML->Delete();
	}
}

BOOL CGroupingDlg::OnSyncDataViewResp(CXMLElement* pXML)
{
	CWaitCursor pCursor;
	
	m_wndGroup.ResetContent();
	m_pXML->Delete(); m_pXML = pXML;
	
	for ( POSITION posNext = pXML->GetElementIterator(); posNext; )
	{
		CXMLElement* pSync = pXML->GetNextElement( posNext );
		if ( ! pSync->IsNamed( "SyncOrder" ) ) continue;
		
		static LPCTSTR pszGroup[] = { _T("xgroupname"), _T("xgroupnum") };
		
		CXMLAttribute* pAttri = pSync->GetAttribute( pszGroup[0] );
		if ( ! pAttri || pAttri->GetValue().IsEmpty() ) pAttri = pSync->GetAttribute( pszGroup[1] );
		
		if ( pAttri && m_wndGroup.FindStringExact( -1, pAttri->GetValue() ) == CB_ERR )
		m_wndGroup.SetItemDataPtr( m_wndGroup.AddString( pAttri->GetValue() ), pXML );
	}
	
	m_wndGroup.SetCurSel( 0 );
	SendMessage(WM_COMMAND, MAKELONG(m_wndGroup.GetDlgCtrlID(),CBN_SELCHANGE), (LPARAM)m_hWnd);
	
	return TRUE;
}

void CGroupingDlg::OnSelChangeGroup()
{
	m_wndList.DeleteAllItems();
	CWaitCursor pCursor;
	
	int nItem = m_wndGroup.GetCurSel();
	if ( nItem < 0 ) return;
	
	CXMLElement* pXML = (CXMLElement*)m_wndGroup.GetItemDataPtr( nItem );
	ASSERT( pXML != NULL );
	
	for ( POSITION posNext = pXML->GetElementIterator(); posNext; )
	{
		CXMLElement* pSync = pXML->GetNextElement( posNext );
		if ( ! pSync->IsNamed( "SyncOrder" ) ) continue;
		
		static LPCTSTR pszGroup[] = { _T("xgroupname"), _T("xgroupnum") };
		
		CXMLAttribute* pAttri = pSync->GetAttribute( pszGroup[0] );
		if ( ! pAttri || pAttri->GetValue().IsEmpty() ) pAttri = pSync->GetAttribute( pszGroup[1] );
		
		if ( pAttri && m_wndGroup.FindStringExact( -1, pAttri->GetValue() ) != nItem ) continue;
		
		static LPCTSTR pszSuffix[] = { _T("xphone"), _T("xname"), NULL };
		
		int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_PARAM,
				m_wndList.GetItemCount(), NULL, 0, 0, 0, (LPARAM)pSync );
		
		for ( int i = 0; pszSuffix[i] != NULL; i ++ )
		{
			CXMLAttribute* pAttri = pSync->GetAttribute( pszSuffix[i] );
			if ( pAttri ) m_wndList.SetItemText( nItem, i, pAttri->GetValue() );
		}
	}
}

void CGroupingDlg::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	
	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM) )
	{
		CDC* pDC = CDC::FromHandle( pDraw->nmcd.hdc );
		
		CRect rcItem;
		m_wndList.GetSubItemRect(pDraw->nmcd.dwItemSpec, pDraw->iSubItem, LVIR_LABEL, rcItem);
		
		CString strText = m_wndList.GetItemText(pDraw->nmcd.dwItemSpec, pDraw->iSubItem);
		pDC->DrawText(strText, &rcItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOCLIP);
		
		*pResult = CDRF_SKIPDEFAULT;
	}
}

void CGroupingDlg::OnBeginLabelEditList(LV_DISPINFO* pNotify, LRESULT* pResult) 
{
	#undef SubclassWindow
	ASSERT( m_pHitInfo.iItem == pNotify->item.iItem );
	
	CRect rect;
	m_wndList.GetSubItemRect( pNotify->item.iItem, m_pHitInfo.iSubItem, LVIR_BOUNDS, rect );
	m_pEdit.point	= rect.TopLeft();
	
	HWND hEdit = (HWND)m_wndList.SendMessage( LVM_GETEDITCONTROL );
	m_pEdit.SubclassWindow( hEdit );
	
	CString strText = m_wndList.GetItemText( pNotify->item.iItem, m_pHitInfo.iSubItem );
	m_pEdit.SetWindowText( (LPCTSTR)strText );
	
	m_wndList.GetSubItemRect( pNotify->item.iItem, m_pHitInfo.iSubItem, LVIR_LABEL, rect );
	
	*pResult = 0;
}

void CGroupingDlg::OnEndLabelEditList(LV_DISPINFO* pNotify, LRESULT* pResult)
{
	static LPCTSTR pszInfo[] = { _T("xphone"), _T("xname") };
	
	if ( pNotify->item.pszText && *pNotify->item.pszText )
	{
		m_wndList.SetItemText( pNotify->item.iItem, m_pHitInfo.iSubItem, pNotify->item.pszText );
		m_pEdit.UnsubclassWindow();
		
		CXMLElement* pXML = (CXMLElement*)pNotify->item.lParam; if ( ! pXML )
		{
			pXML = new CXMLElement( NULL, _T("SyncOrder") );
			pXML->AddAttribute( pszInfo[m_pHitInfo.iSubItem], pNotify->item.pszText );
			
			CString strValue;
			m_wndGroup.GetWindowText( strValue );
			
			strValue.TrimLeft(); strValue.TrimRight();
			ASSERT( ! strValue.IsEmpty() );
			
			pXML->AddAttribute( _T("xgroupname"), strValue );
			
			m_pXML->AddElement( pXML );
			AddMethod( pXML, METHOD_INSERT );
		}
		else
		{
			CXMLElement* pXMLSwap = new CXMLElement( NULL, _T("SyncOrder") );
			pXMLSwap->AddAttribute( pszInfo[m_pHitInfo.iSubItem], pNotify->item.pszText );
			
			AddMethod( pXML, METHOD_UPDATE, pXMLSwap );
			pXML->AddAttribute( pszInfo[m_pHitInfo.iSubItem], pNotify->item.pszText );
		}
		
		m_wndList.SetItemData( pNotify->item.iItem, (DWORD)pXML );
	}
	
	*pResult = 0;
}

void CGroupingDlg::OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint point;
	GetCursorPos( &point );
	
	m_wndList.ScreenToClient( &point );
	
	m_pHitInfo.pt = point;
	m_wndList.SubItemHitTest( &m_pHitInfo );
	
	m_wndList.EditLabel( m_pHitInfo.iItem );
	*pResult = 0;
}

void CGroupingDlg::OnAdd() 
{
	CString strValue;
	m_wndGroup.GetWindowText( strValue );
	
	strValue.TrimLeft(); strValue.TrimRight();
	if ( strValue.IsEmpty() ) return;
	
	if ( ! m_pXML ) m_pXML = new CXMLElement;
	
	if ( m_wndGroup.FindStringExact( -1, strValue ) == CB_ERR ) CreateGroup( strValue, m_pXML );
	
	int nItem = m_wndList.InsertItem( LVIF_TEXT|LVIF_PARAM,
		m_wndList.GetItemCount(), NULL, 0, 0, 0, NULL );
	
	for ( int nDeselect = m_wndList.GetNextItem( -1, LVNI_SELECTED ) ;
		nDeselect != -1 ; nDeselect = m_wndList.GetNextItem( nDeselect, LVNI_SELECTED ) )
	{
		m_wndList.SetItemState( nDeselect, 0, LVIS_SELECTED );
	}
	
	m_wndList.SetItemState( nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	m_wndList.EnsureVisible( nItem, TRUE );
	
	m_pHitInfo.iItem	= nItem;
	m_pHitInfo.iSubItem	= 0;
	
	m_wndList.SetFocus();
	m_wndList.EditLabel( nItem );
}

void CGroupingDlg::OnDelete() 
{
	for ( int nItem = m_wndList.GetNextItem(-1, LVIS_SELECTED); nItem != -1;
		nItem = m_wndList.GetNextItem(-1, LVIS_SELECTED) )
	{
		CXMLElement* pXML = (CXMLElement*)m_wndList.GetItemData( nItem );
		
		AddMethod( pXML, METHOD_DELETE );
		m_wndList.DeleteItem( nItem ); pXML->Delete();
	}
}

BOOL CGroupingDlg::CreateGroup(LPCTSTR lpszName, CXMLElement* pParent)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "insert into" );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "xphonemeetinggroup" );
	
	CString str;
	str.Format( "(xjsjbh, xgroupnum, xgroupname)"
				" select a.xjsjbh, isnull(max(b.xgroupnum),0)+1, '%s' from xuser_info a"
				" left join xphonemeetinggroup b on b.xjsjbh = a.xjsjbh"
				" where a.xphone='%s'"
#if 0
				" group by a.xjsjbh", lpszName, Network.m_xUserId );
#else
				" and '%s' not in (select xgroupname from xphonemeetinggroup"
					" where xjsjbh in (select xjsjbh from xuser_info where xphone='%s'))"
				" group by a.xjsjbh", lpszName, Network.m_xUserId, lpszName, Network.m_xUserId );
#endif
	
	pSync = pXML->AddElement( "column_name" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView( pXML ) ); delete pXML;
	
	m_wndGroup.SetItemDataPtr( m_wndGroup.AddString( lpszName ), pParent );
	m_wndList.DeleteAllItems();
	
	return TRUE;
}

void CGroupingDlg::AddMethod(CXMLElement* pXML, LPCTSTR pszMethod, CXMLElement* pXMLSwap)
{
	if ( ! m_pXMLMethod ) m_pXMLMethod = new CXMLElement;
	
	CXMLElement* pSync = m_pXMLMethod->AddElement( _T("SyncOrder") );
	pSync->AddAttribute( _T("method"), pszMethod );
	
	CXMLElement* pMap = pSync->AddElement( pXML->Clone() );
	if ( pXMLSwap ) pMap->AddElement( pXMLSwap );
}

BOOL CGroupingDlg::ProcessMethod(CXMLElement*& pXML)
{
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pSync = pXML->GetNextElement( pos );
		
		CString strValue = pSync->GetAttributeValue( _T("method") );
		
		if ( ! strValue.CompareNoCase( METHOD_INSERT ) )
		{
			ProcessInsert( pSync->GetFirstElement() );
		}
		else if ( ! strValue.CompareNoCase( METHOD_DELETE ) )
		{
			ProcessDelete( pSync->GetFirstElement() );
		}
		else if ( ! strValue.CompareNoCase( METHOD_UPDATE ) )
		{
			ProcessUpdate( pSync->GetFirstElement() );
		}
	}
	
	pXML->Delete();
	pXML	= NULL;
	
	return TRUE;
}

BOOL CGroupingDlg::ProcessInsert(CXMLElement* pParent, LPCTSTR lpszMethod)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", lpszMethod );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "xphonemeetinggroupmore" );
	
	CString str;
	str.Format( "(xjsjbh,xgroupnum,xphone,xname,xgrouprow)"
				" select a.xjsjbh,a.xgroupnum,'%s','%s',isnull(max(b.xgrouprow),0)+1"
				" from xphonemeetinggroup a"
				" left join xphonemeetinggroupmore b on b.xjsjbh=a.xjsjbh and a.xgroupnum=b.xgroupnum"
				" where a.xgroupname = '%s' and a.xjsjbh in"
					" (select xjsjbh from xuser_info where xphone='%s')"
				" group by a.xjsjbh,a.xgroupnum",
				pParent->GetAttributeValue( _T("xphone") ),
				pParent->GetAttributeValue( _T("xname") ),
				pParent->GetAttributeValue( _T("xgroupname") ),
				Network.m_xUserId );

	pSync = pXML->AddElement( "column_name" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView( pXML ) ); delete pXML;
	
	return TRUE;
}

BOOL CGroupingDlg::ProcessUpdate(CXMLElement* pParent, LPCTSTR lpszMethod)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", lpszMethod );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "xphonemeetinggroupmore" );
	
	CXMLElement* pMap = pParent->GetFirstElement();
	ASSERT( pMap != NULL );
	
	CString str;
	FormatUpdate( pMap, str );
	
	pSync = pXML->AddElement( "column_name" );
	pSync->SetValue( "set " + str );
	
	str.Format( "xjsjbh in (select xjsjbh from xuser_info"
				" where xphone='%s')", Network.m_xUserId );
	FormatMapping( pParent, str );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView( pXML ) ); delete pXML;
	
	return TRUE;
}

BOOL CGroupingDlg::ProcessDelete(CXMLElement* pParent, LPCTSTR lpszMethod)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", lpszMethod );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xphonemeetinggroupmore" );
	
	CString str;
	str.Format( "xjsjbh in (select xjsjbh from xuser_info"
				" where xphone='%s')", Network.m_xUserId );
	FormatMapping( pParent, str );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView( pXML ) ); delete pXML;
	
	return TRUE;
}

void CGroupingDlg::FormatMapping(CXMLElement* pXML, CString& str)
{
	static LPCTSTR pszNum[] = { _T("xgroupnum"), _T("xgrouprow") };
	static LPCTSTR pszExclude[] = { _T("xgroupname"), NULL };
	
	for ( POSITION pos = pXML->GetAttributeIterator() ; pos ; )
	{
		CXMLAttribute* pAttri = pXML->GetNextAttribute( pos );
		
		if ( IsNotField( pAttri->GetName() ) ) continue;
		
		if ( ! str.IsEmpty() ) str += _T(" and ");
		BOOL bNumber = IsNumField( pAttri->GetName() );
		
		str += pAttri->GetName(); str += '=';
		if ( ! bNumber ) str += '\'';
		str += pAttri->GetValue();
		if ( ! bNumber ) str += '\'';
	}
}

void CGroupingDlg::FormatUpdate(CXMLElement* pXML, CString& str)
{
	for ( POSITION pos = pXML->GetAttributeIterator() ; pos ; )
	{
		CXMLAttribute* pAttri = pXML->GetNextAttribute( pos );
		
		if ( ! str.IsEmpty() ) str += _T(", ");
		
		BOOL bNumber = IsNumField( pAttri->GetName() );
		
		str += pAttri->GetName(); str += '=';
		if ( ! bNumber ) str += '\'';
		str += pAttri->GetValue();
		if ( ! bNumber ) str += '\'';
	}
}

BOOL CGroupingDlg::IsNumField(LPCTSTR lpszField) const
{
	static LPCTSTR pszField[] = { _T("xgroupnum"), _T("xgrouprow") };
	
	for ( int i = 0; pszField[i] != NULL; i ++ )
	{
		if ( ! _tcsicmp( lpszField, pszField[i] ) ) return TRUE;
	}
	
	return FALSE;
}

BOOL CGroupingDlg::IsNotField(LPCTSTR lpszField) const
{
	static LPCTSTR pszField[] = { _T("xgroupname") };
	
	for ( int i = 0; pszField[i] != NULL; i ++ )
	{
		if ( ! _tcsicmp( lpszField, pszField[i] ) ) return TRUE;
	}
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CItemEdit construction

CItemEdit::CItemEdit()
{
}

/////////////////////////////////////////////////////////////////////////////
// CItemEdit message handlers

void CItemEdit::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	lpwndpos->x		= point.x;
	
	CEdit::OnWindowPosChanging(lpwndpos);
}
