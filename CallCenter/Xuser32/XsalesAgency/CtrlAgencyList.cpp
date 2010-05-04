//
// CtrlAgencyList.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlAgencyList.h"
#include "WndAgency.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CAgencyList, CListCtrl)
BEGIN_MESSAGE_MAP(CAgencyList, CListCtrl)
	//{{AFX_MSG_MAP(CAgencyList)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblClk)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgencyList construction

CAgencyList::CAgencyList()
{
}

CAgencyList::~CAgencyList()
{
}

_ConnectionPtr CAgencyList::GetConnectionPtr() const
{
	CAgencyWnd* pAgencyWnd = (CAgencyWnd*)GetParent();
	if ( pAgencyWnd->IsKindOf( RUNTIME_CLASS(CAgencyWnd) ) ) return pAgencyWnd->m_pConnection;
	
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyList message handlers

BOOL CAgencyList::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|LVS_REPORT|LVS_NOLABELWRAP|LVS_SHOWSELALWAYS|LVS_OWNERDATA;
	return CListCtrl::Create( dwStyle, rect, pParentWnd, nID );
}

int CAgencyList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CListCtrl::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	
	InsertColumn( 0, _T("发起人"), LVCFMT_LEFT,  94 );
	InsertColumn( 1, _T("接收人"), LVCFMT_LEFT,  94 );
	InsertColumn( 2, _T("时间"),   LVCFMT_LEFT, 128 );
	InsertColumn( 3, _T("状态"),   LVCFMT_LEFT, 100 );
	InsertColumn( 4, _T("原因"),   LVCFMT_LEFT, 250 );
	
	return 0;
}

void CAgencyList::OnDblClk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	GetParent()->SendMessage( WM_COMMAND, ID_AGENCY_LAYOUT );
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyList custom message handlers

void CAgencyList::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;
	
	static LPCTSTR pszSuffix[] =
	{ _T("xuserid"), _T("xphone"), _T("xdatetime"), _T("xstate"), _T("xreason") };
	
	if ( pDispInfo->item.mask & LVIF_TEXT )
	{
		if ( pDispInfo->item.iSubItem < 5 )
			LoadRecordset( &pDispInfo->item, pszSuffix[pDispInfo->item.iSubItem] );
	}
}

BOOL CAgencyList::LoadRecordset(LPCTSTR lpszKey, int nMaxTop)
{
	DeleteAllItems();
	
	try
	{
		CString strQuery;
		strQuery.Format( "select top %i * from xsale"
			" where xsaleinfo = '%s' order by xstate", nMaxTop, lpszKey );
		TRACE( strQuery + "\n" );
		
		xOpenRecordset(m_pRecordset, GetConnectionPtr(), (_bstr_t)strQuery);
		
		SetItemCount( m_pRecordset->RecordCount );
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE ); return FALSE;
		return FALSE;
	}
	
	return TRUE;
}

BOOL CAgencyList::LoadRecordset(CXMLElement* pXML)
{
	DeleteAllItems();
	
	for ( POSITION posNext = pXML->GetElementIterator(); posNext; )
	{
		CXMLElement* pMap = pXML->GetNextElement( posNext );
		if ( ! pMap->IsNamed( "SyncOrder" ) ) continue;
		
		int nItem = InsertItem( GetItemCount(), "" );
		SetItemText( nItem, 1, pMap->GetAttributeValue( "xphone" ) );
		
		CXMLElement* pSub = pMap->GetElementByName( "xreason" );
		if ( pSub ) SetItemText( nItem, 4, pSub->GetValue() );
		
		SetItemData( nItem, (DWORD)pMap->Clone() );
	}
	
	return TRUE;
}

BOOL CAgencyList::LoadRecordset(LVITEM* pItem, LPCTSTR lpszField)
{
	try
	{
		m_pRecordset->AbsolutePosition = (PositionEnum)(pItem->iItem + 1);
		
		_variant_t TheValue = m_pRecordset->GetCollect( lpszField );
		TCHAR szValue[MAX_PATH];
		
		switch ( TheValue.vt )
		{
		case	VT_I2:
		case	VT_I4:
			wsprintf( szValue, "%d", TheValue.iVal);
			lstrcpyn( pItem->pszText, szValue, pItem->cchTextMax );
			break;
			
		case	VT_R4:
		case	VT_R8:
			wsprintf( szValue, "%f", TheValue.fltVal );
			lstrcpyn( pItem->pszText, szValue, pItem->cchTextMax );
			break;
			
		case	VT_DATE:
			{
				COleDateTime tt( TheValue.date );
				lstrcpyn( pItem->pszText, tt.Format("%Y-%m-%d %H:%M:%S"), pItem->cchTextMax );
			}
			break;
			
		case	VT_BSTR:
			lstrcpyn( pItem->pszText, (LPCTSTR)(_bstr_t)TheValue, pItem->cchTextMax );
			break;
		}
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE );
		return FALSE;
	}
	
	return TRUE;
}
#if 0
BOOL CAgencyList::DeleteAllItems()
{
	for ( int nItem = GetItemCount() - 1; nItem >= 0; nItem-- )
	{
		CXMLElement* pMap = (CXMLElement*)GetItemData( nItem );
		if ( pMap ) delete pMap;
		
		CListCtrl::DeleteItem( nItem );
	}
	
	return TRUE;
}
#else
BOOL CAgencyList::DeleteAllItems()
{
	CListCtrl::DeleteAllItems();
	
	xCloseRecordset( m_pRecordset );
	return TRUE;
}
#endif
void CAgencyList::SelectIndex(int nItem)
{
	for ( int i = GetItemCount() - 1 ; i >= 0 ; i-- )
	{
		if ( GetItemState( i, LVIS_SELECTED ) )
		{
			if ( i != nItem ) SetItemState( i, 0, LVIS_SELECTED );
		}
		else
		{
			if ( i == nItem ) SetItemState( i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		}
	}
	
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	pItem.iItem = nItem; pItem.mask = LVIF_STATE;
	pItem.state = 0xFFFFFFFF; pItem.stateMask = LVIS_SELECTED|LVIS_FOCUSED;
	SendMessage( LVM_SETITEMSTATE, pItem.iItem, (LPARAM)&pItem );
	PostMessage( LVM_ENSUREVISIBLE, pItem.iItem, FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyList Recordset handlers

int CAgencyList::SeekByPhone(LPCTSTR lpszPhone, int nNextItem, BOOL bFocus)
{
	for ( int nItem = GetNextItem( nNextItem, LVNI_ALL ) ; nItem >= 0 ;
		nItem = GetNextItem( nItem, LVNI_ALL ) )
	{
		CString strValue = GetItemText( nItem, 1 );
		if ( strValue.CompareNoCase( lpszPhone ) == 0 ) break;
	}
	
	if ( nItem != -1 && bFocus ) SelectIndex( nItem );
	
	return nItem;
}

CString CAgencyList::GetKey(int nItem) const
{
	m_pRecordset->AbsolutePosition = (PositionEnum)(nItem + 1);
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xsale" );
	return (LPCTSTR)(_bstr_t)TheValue;
}

void CAgencyList::Resync(int nItem)
{
	m_pRecordset->AbsolutePosition = (PositionEnum)(nItem + 1);
	
	m_pRecordset->Resync( adAffectCurrent, adResyncAllValues );
	this->RedrawItems( nItem, nItem );
}
