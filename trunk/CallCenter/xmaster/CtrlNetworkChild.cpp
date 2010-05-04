//
// CtrlNetworkChild.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "CoolInterface.h"
#include "Channel.h"

#include "CtrlNetworkChild.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkChild, CListCtrl)

BEGIN_MESSAGE_MAP(CNetworkChild, CListCtrl)
	//{{AFX_MSG_MAP(CNetworkChild)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild construction

CNetworkChild::CNetworkChild()
{
}

CNetworkChild::~CNetworkChild()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild message handlers

BOOL CNetworkChild::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|WS_VSCROLL|WS_TABSTOP|LVS_REPORT|LVS_NOLABELWRAP;
	return CListCtrl::Create( dwStyle, rect, pParentWnd, nID );
}

int CNetworkChild::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CListCtrl::OnCreate( lpCreateStruct ) == -1 ) return -1;
	SetOwner( GetParent() );
	
	SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	SetBkColor( RGB( 250, 250, 255 ) );

	InsertColumn( 0, _T("发生时间"), LVCFMT_LEFT, 120 );
	InsertColumn( 1, _T("事件对象"), LVCFMT_LEFT,  94 );
	InsertColumn( 2, _T("局端号码"), LVCFMT_LEFT,  94 );
	InsertColumn( 3, _T("时长"),     LVCFMT_LEFT,  76 );
	InsertColumn( 4, _T("费用"),     LVCFMT_LEFT,  62 );
	InsertColumn( 5, _T("计费号码"), LVCFMT_LEFT,  94 );
	InsertColumn( 6, _T("业务代码"), LVCFMT_LEFT,  80 );
	InsertColumn( 7, _T("主机"),     LVCFMT_LEFT,  100 );
	InsertColumn( 8, _T("流水号"),   LVCFMT_LEFT,  68 );

	return 0;
}

void CNetworkChild::OnDestroy() 
{
	Clear();
	
	CListCtrl::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild custom message handlers

void CNetworkChild::OnCustomDraw(NMLVCUSTOMDRAW* pNotify, LRESULT* pResult)
{
	if ( pNotify->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pNotify->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		int nChState		= GetItemState( pNotify->nmcd.dwItemSpec, LVIS_STATEIMAGEMASK ) >> 12;
		BOOL bSelected		= GetItemState( pNotify->nmcd.dwItemSpec, TVIS_SELECTED ) & TVIS_SELECTED ? TRUE : FALSE;
		pNotify->clrText	= bSelected ? GetSysColor( COLOR_HIGHLIGHTTEXT ) : GetChColour( nChState );
		pNotify->clrTextBk	= bSelected ? GetSysColor(COLOR_HIGHLIGHT) : RGB( 250, 250, 255 );
		
		*pResult = CDRF_DODEFAULT;
	}
	else
	{
		*pResult = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild operations

int CNetworkChild::AddItem(CChannel* pChannel)
{
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	CString str = COleDateTime::GetCurrentTime().Format("%m-%d %H:%M:%S");

	pItem.mask		= LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	pItem.iItem		= GetItemCount();
	pItem.pszText	= str.GetBuffer(0);
	pItem.lParam	= (LPARAM)pChannel;
	pItem.iItem		= InsertItem( &pItem );
	
	pItem.mask		= LVIF_TEXT;
	pItem.iSubItem	= 7;
	CString strHost	= inet_ntoa( pChannel->m_pHost.sin_addr );
	pItem.pszText	= strHost.GetBuffer(0);
	SetItem( &pItem );
	
	return pItem.iItem;
}

BOOL CNetworkChild::RemoveItem(CChannel* pChannel)
{
	int nItem = FindItem( pChannel );
	if ( nItem == -1 ) return FALSE;
	
	return DeleteItem( nItem );
}

int CNetworkChild::FindItem(CChannel* pChannel) const
{
	for ( int nItem = GetNextItem( -1, LVNI_ALL ) ; nItem >= 0 ;
		nItem = GetNextItem( nItem, LVNI_ALL ) )
	{
		if ( (DWORD)pChannel == GetItemData( nItem ) ) return nItem;
	}

	return -1;
}

CChannel* CNetworkChild::FindChannel(DWORD xSeqNumber) const
{
	for ( int nItem = GetNextItem( -1, LVNI_ALL ) ; nItem >= 0 ;
		nItem = GetNextItem( nItem, LVNI_ALL ) )
	{
		CChannel* pChannel = (CChannel*)GetItemData( nItem );
		
		DWORD nSeqId;
		if ( SUCCEEDED(pChannel->GetSeqNumber( nSeqId )) )
		{
			if ( nSeqId == xSeqNumber ) return pChannel;
		}
	}
	
	return NULL;
}

int CNetworkChild::PrintOwner(int nItem, LPCTSTR xLocalId, LPCTSTR xUsrNumber, LPCTSTR xFeeNumber)
{
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	pItem.mask		= LVIF_TEXT;
	pItem.iItem		= nItem;
	
	pItem.iSubItem	= 2;
	pItem.pszText	= (LPSTR)xLocalId;
	CListCtrl::SetItem( &pItem );
	
	pItem.iSubItem	= 1;
	pItem.pszText	= (LPSTR)xUsrNumber;
	CListCtrl::SetItem( &pItem );
	
	if ( xFeeNumber )
	{
		pItem.iSubItem	= 5;
		pItem.pszText	= (LPSTR)xFeeNumber;
		CListCtrl::SetItem( &pItem );
	}
	
	return nItem;
}

int CNetworkChild::PrintChState(CChannel* pChannel, int nChState)
{
	int nItem = FindItem( pChannel );
	if ( nItem == -1 ) return -1;
	
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	pItem.mask		= LVIF_IMAGE;
	pItem.iItem		= nItem;
	pItem.iImage	= nChState;
	
	pItem.mask	   |= LVIF_STATE;
	pItem.stateMask = LVIS_STATEIMAGEMASK;
	pItem.state		= INDEXTOSTATEIMAGEMASK(nChState);
	
	CListCtrl::SetItem( &pItem );
	return pItem.iItem;
}

int CNetworkChild::PrintChState(CChannel* pChannel, int nChState, DWORD nReason)
{
	int nItem = PrintChState( pChannel, nChState );
	if ( nItem == -1 ) return -1;
	
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	CString strText;
	strText.Format( "%X", nReason );
	
	pItem.mask	   |= LVIF_TEXT;
	pItem.iItem		= nItem;
	pItem.iSubItem	= 6;
	pItem.pszText	= strText.GetBuffer( 0 );
	
	CListCtrl::SetItem( &pItem );
	return pItem.iItem;
}

int CNetworkChild::PrintSeqNumber(int nItem, DWORD xSeqNumber)
{
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	pItem.mask		= LVIF_TEXT;
	pItem.iItem		= nItem;
	pItem.iSubItem	= 8;
	
	CString strText;
	strText.Format( "%i", xSeqNumber );
	pItem.pszText	= (LPSTR)strText.GetBuffer(0);
	CListCtrl::SetItem( &pItem );
	
	return nItem;
}

int CNetworkChild::PrintLength(CChannel* pChannel, DWORD nLength)
{
	int nItem = FindItem( pChannel );
	if ( nItem == -1 ) return -1;
	
	CString strText;
	if ( nLength >= 3600 )
	{
		strText.Format( _T("%i:%.2i:%.2i"), nLength / 3600, ( nLength / 60 ) % 60, nLength % 60 );
	}
	else
	{
		strText.Format( _T("%.2i:%.2i"), nLength / 60, nLength % 60 );
	}
	
	SetItemText( nItem, 3, (LPCTSTR)strText );
	return nItem;
}

int CNetworkChild::PrintExpense(CChannel* pChannel, double nExpense)
{
	int nItem = FindItem( pChannel );
	if ( nItem == -1 ) return -1;
	
	CString strText;
	strText.Format( _T("%.2lf"), nExpense );
	
	SetItemText( nItem, 4, (LPCTSTR)strText );
	return nItem;
}

void CNetworkChild::Clear()
{
	for ( int nItem = GetItemCount() - 1 ; nItem >= 0 ; nItem-- )
	{
		CChannel* pChannel = (CChannel*)GetItemData( nItem );
		
		RemoveItem( pChannel );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild Helpers

COLORREF CNetworkChild::GetChColour(DWORD nChState) const
{
	switch ( INDEXTOSTATEIMAGEMASK(nChState) )
	{
	case	INDEXTOSTATEIMAGEMASK(Ch_Ready) & LVIS_STATEIMAGEMASK:
		return RGB( 192, 192, 192 );

	case	INDEXTOSTATEIMAGEMASK(Ch_Talking) & LVIS_STATEIMAGEMASK:
		return RGB( 0, 168, 0 );
		
	case	INDEXTOSTATEIMAGEMASK(Ch_Rejected) & LVIS_STATEIMAGEMASK:
		return RGB( 222, 81, 42 );
	
	default:
		return RGB( 192, rand(), 192 );
	}
}
