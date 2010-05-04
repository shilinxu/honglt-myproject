//
// CtrlAgencyList.cpp
//

#include "stdafx.h"
#include "xmonitor.h"
#include "CtrlAgencyList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CAgencyList, CListCtrl)
BEGIN_MESSAGE_MAP(CAgencyList, CListCtrl)
	//{{AFX_MSG_MAP(CAgencyList)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
//	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblClk)
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

void CAgencyList::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	int nItem = GetNextItem( -1, LVNI_SELECTED );
	if ( nItem >= 0 ) 
		m_pRecordset->AbsolutePosition = (PositionEnum)(nItem + 1);
	
	*pResult = 0;
}

void CAgencyList::OnDblClk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	GetParent()->SendMessage( WM_COMMAND, ID_AGENCY_XPROPERTY );
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

BOOL CAgencyList::LoadRecordset(LPCTSTR lpszFilter)
{
	try
	{
	CString strQuery;
	strQuery.Format( "select * from xsale" );
	
	if ( lpszFilter && _tcslen(lpszFilter) )
	strQuery += CString(" where ") + lpszFilter;
	
	strQuery += " order by xstate";
	TRACE( strQuery + "\n" );
	
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	
	SetItemCount( m_pRecordset->RecordCount );
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE ); return FALSE;
		return FALSE;
	}
	
	return TRUE;
}

BOOL CAgencyList::LoadRecordset(LVITEM* pItem, LPCTSTR lpszField)
{
//TRACE("%i-> get...(%i)..(%s)\n", GetTickCount(), pItem->iItem + 1, lpszField);
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

BOOL CAgencyList::DeleteAllItems()
{
	CListCtrl::DeleteAllItems();
	
	xCloseRecordset( m_pRecordset );
	return TRUE;
}

void CAgencyList::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu pMenu;
	pMenu.LoadMenu( IDR_AGENCY );
	
	CMenu* pPopup = pMenu.GetSubMenu( 0 );
	
	MENUITEMINFO pInfo;
	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= MIIM_STATE;
	GetMenuItemInfo( pPopup->GetSafeHmenu(), ID_AGENCY_XPROPERTY, FALSE, &pInfo );
	pInfo.fState	|= MFS_DEFAULT;
	SetMenuItemInfo( pPopup->GetSafeHmenu(), ID_AGENCY_XPROPERTY, FALSE, &pInfo );
	
	pPopup->TrackPopupMenu( TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd() );
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyList import/export handlers

void CAgencyList::PrintToWorkbook(_WorkbookPtr pWorkbook)
{
	_WorksheetPtr pWorksheet = NULL;
	m_pRecordset->Requery( adCmdUnknown );
	
	for ( int nItem = 1; ! m_pRecordset->adoEOF; m_pRecordset->MoveNext() ) 
	{
		_variant_t TheValue = m_pRecordset->GetCollect( "xstate" );
		if ( TheValue.vt == VT_NULL ) TheValue.SetString( _T("未知") );
		if ( ! _tcslen( (LPCTSTR)(_bstr_t)TheValue ) ) TheValue.SetString( _T("未知") );
		
		_WorksheetPtr pWorksheet2 =
		AttachWorkook( pWorkbook->Sheets, (LPCTSTR)(_bstr_t)TheValue );
		
		if ( pWorksheet2 != pWorksheet ) nItem = 1;
		pWorksheet = pWorksheet2;
		
		RangePtr pRange = pWorksheet->GetCells();
		
		TheValue = m_pRecordset->GetCollect( "xphone" );
		pRange->PutItem( (short)nItem, (short)1, TheValue );
		
		TheValue = m_pRecordset->GetCollect( "xuserid" );
		pRange->PutItem( (short)nItem, (short)2, TheValue );
		
		TheValue = m_pRecordset->GetCollect( "xdatetime" );
		pRange->PutItem( (short)nItem, (short)3, TheValue );
		
		TheValue = m_pRecordset->GetCollect( "xstate" );
		pRange->PutItem( (short)nItem, (short)4, TheValue );
		
		TheValue = m_pRecordset->GetCollect( "xreason" );
		pRange->PutItem( (short)nItem, (short)5, TheValue );
		
		nItem += 1;
	}
}

_WorksheetPtr CAgencyList::AttachWorkook(SheetsPtr pSheets, LPCTSTR lpszName)
{
	_WorksheetPtr pWorksheet = NULL;
	
	for ( int nSheet = 1; nSheet <= pSheets->GetCount(); nSheet++ )
	{
		pWorksheet = pSheets->GetItem( (short)nSheet );
		
		if ( _tcsicmp( lpszName, pWorksheet->GetName() ) ) continue;
		
		RangePtr pRange = pWorksheet->GetUsedRange();
		if ( pRange->GetRows()->Count < 65536 ) return pWorksheet;
		
		if ( LPCTSTR lpszPart = _tcsstr( lpszName, "." ) )
		{
			CString strName( lpszName );
			strName.Delete( strName.Find('.'), _tcslen(lpszPart) );
			
			CString str; str.Format( "%d", _ttoi(lpszPart + 1) + 1 );
			return AttachWorkook( pSheets, strName + '.' + str );
		}
		else
		{
			CString strName( lpszName ); strName += ".2";
			return AttachWorkook( pSheets, (LPCTSTR)strName );
		}
	}
	
	_variant_t vAfter( pWorksheet.GetInterfacePtr() );
	
	pWorksheet = pSheets->Add( vtMissing, vAfter );
	pWorksheet->PutName( lpszName );
	
	RangePtr pRange = pWorksheet->GetRange( "A1", "A65536" );
	pRange->PutHorizontalAlignment( (long)-4152 );
	pRange->PutNumberFormatLocal( "@");
	pRange->PutColumnWidth( 14.8 );
	
	pRange = pWorksheet->GetRange( "B1", "B65536" );
	pRange->PutHorizontalAlignment( (long)-4152 );
	pRange->PutNumberFormatLocal( "@");
	pRange->PutColumnWidth( 12.6 );
	
	pRange = pWorksheet->GetRange( "C1", "C65536" );
//	pRange->PutHorizontalAlignment( (long)-4152 );
	pRange->PutNumberFormatLocal( "yyyy-MM-d HH:mm:ss");
	pRange->PutColumnWidth( 20.3 );
	
	pRange = pWorksheet->GetRange( "D1", "D65536" );
	pRange->PutNumberFormatLocal( "@");
	pRange->PutColumnWidth( 12.0 );
	
	pRange = pWorksheet->GetRange( "E1", "E65536" );
	pRange->PutNumberFormatLocal( "@");
	pRange->PutColumnWidth( 36.0 );
	
	return pWorksheet;
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyList Recordset handlers

int CAgencyList::SeekByPhone(LPCTSTR lpszPhone)
{
	CString strQuery;
	
	strQuery.Format( "xphone = '%s'", lpszPhone );
	TRACE( strQuery + "\n" );
	
	try
	{
		if ( m_pRecordset->adoEOF ) m_pRecordset->MoveFirst();
		
		m_pRecordset->Find( (_bstr_t)strQuery, 1, adSearchForward, m_pRecordset->Bookmark );
		
		if ( m_pRecordset->adoEOF ) return -1;
		int nItem = m_pRecordset->AbsolutePosition - 1;
		
		SelectIndex( nItem );
		return nItem;
	}
	catch (_com_error e)
	{
	}
	
	return -1;
}

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

BOOL CAgencyList::RemoveItemOf(UINT nState)
{
	try
	{

	for ( int nItem = GetNextItem( -1, nState ) ; nItem != -1 ;
		nItem = GetNextItem( nItem, nState ) )
	{
		m_pRecordset->AbsolutePosition = (PositionEnum)(nItem + 1);
		
		m_pRecordset->Delete( adAffectCurrent );
		
		DeleteItem( nItem );
	}
	
	m_pRecordset->UpdateBatch( adAffectAll );
	return TRUE;
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE );
		return FALSE;
	}
}

int CAgencyList::GetCurIndex(CString& str)
{
	int nItem = GetNextItem( -1, LVIS_SELECTED );
	if ( nItem < 0 ) return -1;
	
	m_pRecordset->AbsolutePosition = (PositionEnum)(nItem + 1);
	_variant_t TheValue = m_pRecordset->GetCollect( "xsale" );
	
	ASSERT( TheValue.vt != VT_NULL );
	str = (LPCTSTR)(_bstr_t)TheValue;
	
	return nItem;
}

void CAgencyList::ResyncIndex(int nIndex)
{
	m_pRecordset->AbsolutePosition = (PositionEnum)(nIndex + 1);
	m_pRecordset->Resync( adAffectCurrent, adResyncAllValues );
	
	this->RedrawItems( nIndex, nIndex );
}

void CAgencyList::ResyncItemWith(UINT nState, int nIndex)
{
	m_pRecordset->AbsolutePosition = (PositionEnum)(nIndex + 1);
	
	_variant_t xdatetime = m_pRecordset->GetCollect( "xdatetime" );
	_variant_t xstate	 = m_pRecordset->GetCollect( "xstate" );
	_variant_t xreason	 = m_pRecordset->GetCollect( "xreason" );
	
	for ( int nItem = GetNextItem( -1, nState ) ; nItem != -1 ;
		nItem = GetNextItem( nItem, nState ) )
	{
		m_pRecordset->AbsolutePosition = (PositionEnum)(nItem + 1);
		
		m_pRecordset->PutCollect( "xdatetime", xdatetime );
		m_pRecordset->PutCollect( "xstate", xstate );
		m_pRecordset->PutCollect( "xreason", xreason );
		
		this->RedrawItems( nItem, nItem );
	}
	
	m_pRecordset->UpdateBatch( adAffectAll );
}

int CAgencyList::AddNew(LPCTSTR xsaleinfo)
{
	int nSalIdIndex = GetSalesIdIndex();
	CTime pNow = CTime::GetCurrentTime();
	
	m_pRecordset->AddNew();
	m_pRecordset->PutCollect( "xsaleinfo", _variant_t(xsaleinfo) );
	
	CString strValue;
	strValue.Format( "SAL%.4i%.2i%.2i%.6i", 
				pNow.GetYear(), pNow.GetMonth(), pNow.GetDay(), nSalIdIndex++ );
	m_pRecordset->PutCollect( "xsale", _variant_t(strValue) );
	
	SetItemCount( GetItemCount() + 1 );
	return m_pRecordset->AbsolutePosition - 1;
}

int CAgencyList::GetSalesIdIndex() const
{
	CString strQuery;
	CTime pNow = CTime::GetCurrentTime();
	
	strQuery.Format( "select max(cast(right(xsale,6) as int))"
					" from xsale where len(xsale)=17 and substring(xsale,4,8)='%.4i%.2i%.2i'",
					pNow.GetYear(), pNow.GetMonth(), pNow.GetDay() );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	_variant_t TheValue = xRecordset->Fields->GetItem( _variant_t(long(0)) )->Value;
	
	return ( TheValue.vt == VT_NULL ) ? 1 : TheValue.lVal + 1;
}
