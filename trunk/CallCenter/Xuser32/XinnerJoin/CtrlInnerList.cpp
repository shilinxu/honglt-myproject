//
// CtrlInnerList.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlInnerList.h"
#include "WndInner.h"

#include <math.h>
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CInnerList, CListCtrl)
BEGIN_MESSAGE_MAP(CInnerList, CListCtrl)
	//{{AFX_MSG_MAP(CInnerList)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblClk)
//	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInnerList construction

CInnerList::CInnerList()
{
}

CInnerList::~CInnerList()
{
}
#if 0
_ConnectionPtr CInnerList::GetConnectionPtr() const
{
	CInnerWnd* pInnerWnd = (CInnerWnd*)GetParent();
	if ( pInnerWnd->IsKindOf( RUNTIME_CLASS(CInnerWnd) ) ) return pInnerWnd->m_pConnection;
	
	return NULL;
}
#endif
/////////////////////////////////////////////////////////////////////////////
// CInnerList message handlers

BOOL CInnerList::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|LVS_REPORT|LVS_NOLABELWRAP|LVS_SHOWSELALWAYS;
//	dwStyle |= WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|LVS_REPORT|LVS_NOLABELWRAP|LVS_SHOWSELALWAYS|LVS_OWNERDATA;
	return CListCtrl::Create( dwStyle, rect, pParentWnd, nID );
}

int CInnerList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CListCtrl::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	
	InsertColumn( 0, _T("流水号"), LVCFMT_LEFT,  84 );
	InsertColumn( 1, _T("局端号码"),   LVCFMT_LEFT,  94 );
	InsertColumn( 2, _T("资费名称"),   LVCFMT_LEFT, 128 );
	InsertColumn( 3, _T("呼叫类型"),   LVCFMT_LEFT, 64 );
	InsertColumn( 4, _T("通话时间"),   LVCFMT_LEFT, 128 );
	InsertColumn( 5, _T("时长"),   LVCFMT_LEFT, 80 );
	InsertColumn( 6, _T("费用"),   LVCFMT_LEFT, 60 );
	InsertColumn( 7, _T("远端号码"),   LVCFMT_LEFT, 94 );
	
	return 0;
}

void CInnerList::OnDblClk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	GetParent()->SendMessage( WM_COMMAND, ID_INNER_REPLY );
}

/////////////////////////////////////////////////////////////////////////////
// CInnerList custom message handlers

void CInnerList::OnCustomDraw(NMLVCUSTOMDRAW* pNotify, LRESULT* pResult)
{
	if ( pNotify->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pNotify->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		TRISTATE bEnabled	= GetItemState( pNotify->nmcd.dwItemSpec, LVIS_STATEIMAGEMASK ) >> 12;
		pNotify->clrText	= ( bEnabled ) ? ( bEnabled == TS_FALSE ) ? RGB( 222, 81, 42 ) : RGB( 0, 168, 0 ) : RGB( 49, 98, 175 );
		pNotify->clrTextBk	= ((pNotify->nmcd.dwItemSpec+1) % 5) ? RGB( 255, 255, 255 ) : RGB( 235, 235, 255 );
		
		*pResult = CDRF_DODEFAULT;
	}
	else
	{
		*pResult = 0;
	}
}

void CInnerList::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;
	
	static LPCTSTR pszSuffix[] =
	{ _T("jsjbh"), _T("xphone"), _T("xfeename"), _T("xtalktype"),
	 _T("xtimestart"), _T("xtimelength"), _T("xexpense"), _T("xthirdid") };
	
	if ( pDispInfo->item.mask & LVIF_TEXT )
	{
		if ( pDispInfo->item.iSubItem < 8 )
			LoadRecordset( &pDispInfo->item, pszSuffix[pDispInfo->item.iSubItem] );
	}
}
#if 0
BOOL CInnerList::LoadRecordset(LPCTSTR lpszKey, int nMaxTop)
{
	DeleteAllItems();
	CWaitCursor pCursor;
	
	try
	{
		CString strQuery;
		strQuery.Format( "select top %i a.jsjbh, b.xphone, c.xfeename, c.xtalktype,"
			" a.xtimestart, a.xtimelength, a.xexpense, a.xthirdid from xuserdetail a"
			" inner join xuser b on a.xuserbindid = b.xuserbindid"
			" inner join xfeetable c on a.xfeetableid = c.xjsjbh"
			" where a.xjsjbh = '%s' order by a.jsjbh desc", nMaxTop, lpszKey );
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
#else
/*BOOL CInnerList::LoadRecordset(LPCTSTR lpszKey, int nMaxTop)
{
	DeleteAllItems();
	CWaitCursor pCursor;
	
	try
	{
		CString strQuery;
		strQuery.Format( "select top %i a.jsjbh, b.xphone, c.xfeename, c.xtalktype,"
			" a.xtimestart, a.xtimelength, a.xexpense, a.xthirdid from xuserdetail a"
			" inner join xuser b on a.xuserbindid = b.xuserbindid"
			" inner join xfeetable c on a.xfeetableid = c.xjsjbh"
			" where c.xphone = '%s' order by a.jsjbh desc", nMaxTop, lpszKey );
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
*/
#endif
BOOL CInnerList::LoadRecordset(LVITEM* pItem, LPCTSTR lpszField)
{
	try
	{
		m_pRecordset->AbsolutePosition = (PositionEnum)(pItem->iItem + 1);
		
		_variant_t TheValue = m_pRecordset->GetCollect( lpszField );
#if 1
		TCHAR szValue[MAX_PATH];
		
		switch ( TheValue.vt )
		{
		case	VT_I2:
		case	VT_I4:
			wsprintf( szValue, "%d", TheValue.iVal);
			lstrcpyn( pItem->pszText, szValue, pItem->cchTextMax );
			break;
			
		case	VT_DECIMAL:
#if 1
			{
				double val = TheValue.decVal.Lo32;
				
				val *= (TheValue.decVal.sign == 128)? -1 : 1;
				val /= pow(10, TheValue.decVal.scale); 
				    
				CString strFormat;
				strFormat.Format("%d", TheValue.decVal.scale);
				strFormat = "%." + strFormat + "f";
				
				sprintf( szValue, strFormat, val );
				lstrcpyn( pItem->pszText, szValue, pItem->cchTextMax );
				break;
			}
#else
		TheValue.ChangeType( VT_BSTR );
		lstrcpyn( pItem->pszText, (LPCTSTR)(_bstr_t)TheValue, pItem->cchTextMax );
			break;
#endif
			
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
#else
		if ( VT_NULL == TheValue.vt ) return TRUE;
		TheValue.ChangeType( VT_BSTR );
		lstrcpyn( pItem->pszText, (LPCTSTR)(_bstr_t)TheValue, pItem->cchTextMax );
#endif
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE );
		return FALSE;
	}
	
	return TRUE;
}

BOOL CInnerList::LoadRecordset(CXMLElement* pXML)
{
	DeleteAllItems();
	CWaitCursor pCursor;
	
	for ( POSITION posNext = pXML->GetElementIterator(); posNext; )
	{
		CXMLElement* pSync = pXML->GetNextElement( posNext );
		if ( ! pSync->IsNamed( "SyncOrder" ) ) continue;
		
		static LPCTSTR pszSuffix[8] =
		{ _T("jsjbh"), _T("xphone"), _T("xfeename"), _T("xtalktype"),
		_T("xtimestart"), _T("xtimelength"), _T("xexpense"), _T("xthirdid") };
		
		int nItem = InsertItem( GetItemCount(), "" );
		for ( int i = 0; i < 8; i ++ )
		{
			CXMLAttribute* pAttri = pSync->GetAttribute( pszSuffix[i] );
			if ( pAttri ) SetItemText( nItem, i, pAttri->GetValue() );
		}
	}
	
	return TRUE;
}

BOOL CInnerList::DeleteAllItems()
{
	CListCtrl::DeleteAllItems();
	
	xCloseRecordset( m_pRecordset );
	return TRUE;
}

void CInnerList::SelectIndex(int nItem)
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
// CInnerList Recordset handlers

int CInnerList::SeekByPhone(LPCTSTR lpszPhone, int nNextItem, BOOL bFocus)
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
#if 0
CString CInnerList::GetKey(int nItem) const
{
	m_pRecordset->AbsolutePosition = (PositionEnum)(nItem + 1);
	
	// 取重拨号码
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xthirdid" );
	if ( TheValue.vt != VT_NULL ) return (LPCTSTR)(_bstr_t)TheValue;
	
	// 取回电号码
	
	TheValue = m_pRecordset->GetCollect( "xphone" );
	return (LPCTSTR)(_bstr_t)TheValue;
}
#else
CString CInnerList::GetKey(int nItem) const
{
	// 取重拨号码
	
	CString strValue;
	strValue = GetItemText( nItem, 7 );
	strValue.TrimLeft(); strValue.TrimRight();
	
	if ( ! strValue.IsEmpty() ) return strValue;
	
	// 取回电号码
	
	strValue = GetItemText( nItem, 1 );
	strValue.TrimLeft(); strValue.TrimRight();
	return strValue;
}
#endif
