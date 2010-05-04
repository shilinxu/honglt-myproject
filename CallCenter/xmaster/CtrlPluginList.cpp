//
// CtrlPluginList.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "CtrlPluginList.h"
#include "CoolInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CPluginList, CListCtrl)

BEGIN_MESSAGE_MAP(CPluginList, CListCtrl)
	//{{AFX_MSG_MAP(CPluginList)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define MAX_MESSAGE_COUNT	1024

/////////////////////////////////////////////////////////////////////////////
// CPluginList construction

CPluginList::CPluginList()
{
}

CPluginList::~CPluginList()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPluginList message handlers

BOOL CPluginList::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|WS_VSCROLL|WS_TABSTOP|LVS_REPORT|LVS_NOLABELWRAP;
	return CListCtrl::Create( dwStyle, rect, pParentWnd, nID );
}

int CPluginList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CListCtrl::OnCreate( lpCreateStruct ) == -1 ) return -1;
	SetOwner( GetParent() );
	
	SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	InsertColumn( 0, _T("发生时间"), LVCFMT_LEFT, 120 );
	InsertColumn( 1, _T("事件对象"), LVCFMT_LEFT, 126 );
	InsertColumn( 2, _T("事件代码"), LVCFMT_LEFT,  80 );
	InsertColumn( 3, _T("事件描述"), LVCFMT_LEFT, 250 );

	return 0;
}

void CPluginList::OnDestroy() 
{
	CListCtrl::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CPluginList custom message handlers

void CPluginList::OnCustomDraw(NMLVCUSTOMDRAW* pNotify, LRESULT* pResult)
{
	if ( pNotify->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pNotify->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		BOOL bSelected		= GetItemState( pNotify->nmcd.dwItemSpec, TVIS_SELECTED ) & TVIS_SELECTED ? TRUE : FALSE;
		DWORD nMaskState	= GetItemState( pNotify->nmcd.dwItemSpec, LVIS_STATEIMAGEMASK );
		pNotify->clrText	= bSelected ? GetSysColor( COLOR_HIGHLIGHTTEXT ) : GetChColour( nMaskState );
		pNotify->clrTextBk	= GetSysColor( bSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW );
		
		*pResult = CDRF_DODEFAULT;
	}
	else
	{
		*pResult = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPluginList Text operations

int CPluginList::Print(int nType, LPCTSTR xPhone, LPCTSTR xFeeCode, LPCTSTR xContent)
{
	if ( GetItemCount() >= MAX_MESSAGE_COUNT )
	{
		CListCtrl::DeleteAllItems();
	}
	
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	pItem.mask		= LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	
	pItem.stateMask = LVIS_STATEIMAGEMASK;
	pItem.iImage = ( nType == MSG_DEFAULT ) ? 30 : ( nType == MSG_ERROR ) ? 7 : 0;
	
	CString str = COleDateTime::GetCurrentTime().Format("%m-%d %H:%M:%S");
	pItem.pszText	= str.GetBuffer( 0 );
	
	pItem.iItem		= GetItemCount();
	pItem.iItem		= InsertItem( &pItem );
	
	pItem.mask		= LVIF_TEXT;
	pItem.pszText	= (LPSTR)xPhone;
	pItem.iSubItem	= 1;
	CListCtrl::SetItem( &pItem );
	
	pItem.mask		= LVIF_TEXT;
	pItem.pszText	= (LPSTR)xFeeCode;
	pItem.iSubItem	= 2;
	CListCtrl::SetItem( &pItem );
	
	pItem.mask		= LVIF_TEXT;
	pItem.pszText	= (LPSTR)xContent;
	pItem.iSubItem	= 3;
	CListCtrl::SetItem( &pItem );
	
	SelectItem( pItem.iItem );
	
	return pItem.iItem;
}

void CPluginList::SelectItem(int nItem)
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
// CPluginList operations

/////////////////////////////////////////////////////////////////////////////
// CPluginList Helpers

COLORREF CPluginList::GetChColour(DWORD nMaskState) const
{
	switch ( nMaskState )
	{
	case	INDEXTOSTATEIMAGEMASK( MSG_DEFAULT ):
		return RGB( 0, 0, 0 );

	case	INDEXTOSTATEIMAGEMASK( MSG_SYSTEM ):
		return RGB( 0, 0, 127 );
		
	case	INDEXTOSTATEIMAGEMASK( MSG_ERROR ):
		return RGB( 255, 0, 0 );
		
	case	INDEXTOSTATEIMAGEMASK( MSG_TEMP ):
		return RGB( 0, 0, 255 );
	
	default:
		return RGB( 192, 192, 192 );
	}
}
