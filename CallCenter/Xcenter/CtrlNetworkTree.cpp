//
// CtrlNetworkTree.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Network.h"
#include "CoolInterface.h"

#include "WndNetwork.h"
#include "CtrlNetworkTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkTree, CTreeCtrl)

BEGIN_MESSAGE_MAP(CNetworkTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CNetworkTree)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TVN_SELCHANGEDW, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(TVN_SELCHANGEDA, OnSelchanged)
	//}}AFX_MSG_MAP
//	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
//	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982

/////////////////////////////////////////////////////////////////////////////
// CNetworkTree construction

CNetworkTree::CNetworkTree()
{
}

CNetworkTree::~CNetworkTree()
{
}

void CNetworkTree::Clear()
{
	HTREEITEM hItem = GetNextItem( NULL, TVGN_CHILD );

	for ( ; hItem != NULL ; hItem = GetNextItem( NULL, TVGN_CHILD ) )
	{
		RemoveItem( hItem );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkTree message handlers

BOOL CNetworkTree::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|/*TVS_PRIVATEIMAGELISTS|*/TVS_HASLINES|TVS_LINESATROOT;
	dwStyle |= TVS_HASBUTTONS|TVS_SHOWSELALWAYS;
	return CTreeCtrl::Create( dwStyle, rect, pParentWnd, nID );
//	return CTreeCtrl::Create(
//		dwStyle | TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_NOTOOLTIPS ,
//		rect, pParentWnd, nID );
}

int CNetworkTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTreeCtrl::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	SetImageList( &CoolInterface.m_pImages, TVSIL_NORMAL );
	SetBkColor( RGB( 250, 250, 255 ) );
//	EnableToolTips( TRUE );
	
	return 0;
}

void CNetworkTree::OnDestroy() 
{
	Clear();

	CTreeCtrl::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkTree custom message handlers

void CNetworkTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = reinterpret_cast<NM_TREEVIEW*>(pNMHDR);
	
	if ( ( pNMTreeView->itemNew.state & TVIS_SELECTED ) == 0 ) return;
	CNetworkChild* pCtrlChild = (CNetworkChild*)GetItemData( GetSelectedItem() );
	((CNetworkWnd*)GetParent()) ->SelectChild( pCtrlChild->GetSafeHwnd() );
	
	*pResult = 0;
}

void CNetworkTree::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVCUSTOMDRAW* pDraw = (NMTVCUSTOMDRAW*)pNMHDR;

	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == (CDDS_ITEM|CDDS_PREPAINT) )
	{
		BOOL bSelected		= GetItemState( (HTREEITEM)pDraw->nmcd.dwItemSpec, TVIS_SELECTED ) & TVIS_SELECTED ? TRUE : FALSE;
		pDraw->clrText		= GetSysColor( bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT );
		pDraw->clrTextBk	= bSelected ? GetSysColor(COLOR_HIGHLIGHT) : RGB( 250, 250, 255 );
			//GetSysColor( bSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW );
		*pResult = CDRF_DODEFAULT;
	}
}
#if 0
int CNetworkTree::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	RECT rect;
	UINT nFlags;

	HTREEITEM hItem = HitTest( point, &nFlags );
	
	if ( nFlags & TVHT_ONITEMLABEL )
	{
		GetItemRect( hItem, &rect, TRUE );
		pTI->hwnd		= m_hWnd;
		pTI->uId		= (UINT)hItem;
		pTI->lpszText	= LPSTR_TEXTCALLBACK;
		pTI->rect		= rect;
		return pTI->uId;
	}
	
	return -1;
}

BOOL CNetworkTree::OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	
	if (( pNMHDR->idFrom == (UINT)m_hWnd ) &&
		((pNMHDR->code   == TTN_NEEDTEXTA && pTTTA->uFlags & TTF_IDISHWND ) ||
		 (pNMHDR->code   == TTN_NEEDTEXTW && pTTTW->uFlags & TTF_IDISHWND )))
	{
		return FALSE;
	}

	CNetworkChild* pChild = (CNetworkChild*)GetItemData( (HTREEITEM)pNMHDR->idFrom );

	CString strTip = GetItemText( (HTREEITEM)pNMHDR->idFrom );

	if ( pChild != NULL )
	{
		CString str;
		str.Format( _T("%i - %i"), pChild->m_nChFirst, pChild->m_nChFirst + pChild->GetItemCount() - 1 );
		strTip += _T(": ") + str;
	}
	
#ifndef _UNICODE
	if ( pNMHDR->code == TTN_NEEDTEXTA )
		lstrcpyn( pTTTA->szText, strTip, 80 );
	else
		_mbstowcsz( pTTTW->szText, strTip, 80 );
#else
	if ( pNMHDR->code == TTN_NEEDTEXTA )
		_wcstombsz( pTTTA->szText, strTip, 80 );
	else
		lstrcpyn( pTTTW->szText, strTip, 80 );
#endif
	*pResult = 0;

	return TRUE;    // message was handled
}
#endif
/////////////////////////////////////////////////////////////////////////////
// CNetworkTree operations

CNetworkChild* CNetworkTree::AddItem(int nChType)
{
	CNetworkChild* pChild = new CNetworkChild( nChType );
	pChild->Create( WS_CHILD, CRect(0,0,0,0), GetParent(), IDC_NETWORK_CHILD );
	
	CString xChType = _T("");
	int nImage = GetChString(nChType, xChType);

	InsertItem(
		TVIF_PARAM|TVIF_TEXT|TVIF_STATE|TVIF_IMAGE|TVIF_SELECTEDIMAGE,
		(LPCTSTR)xChType, nImage, nImage, TVIS_EXPANDED,
		TVIS_EXPANDED, (LPARAM)pChild, TVI_ROOT, TVI_LAST );
	
	return pChild;
}

BOOL CNetworkTree::RemoveItem(HTREEITEM hItem)
{
	HTREEITEM hItem2 = GetNextItem( hItem, TVGN_NEXT );
	if ( hItem2 == NULL ) hItem = GetNextItem( NULL, TVGN_CHILD );

	CNetworkChild* pChild = (CNetworkChild*)GetItemData(hItem);
	DeleteItem( hItem );
	
	if ( hItem2 == NULL )
	{
		((CNetworkWnd*)GetParent()) ->SelectChild( NULL );
	}
	else
	{
		SelectItem( hItem2 );
	}
		
	if ( pChild )
	{
		pChild->DestroyWindow();
		delete pChild;
	}

	return TRUE;
}

CNetworkChild* CNetworkTree::FindNext(int nChType, HTREEITEM* pItem) const
{
	HTREEITEM hItem = GetNextItem( pItem ? *pItem : NULL,
				( pItem && *pItem ) ? TVGN_NEXT : TVGN_CHILD );
	if ( pItem != NULL ) *pItem = hItem;
	if ( hItem == NULL ) return NULL;

	CNetworkChild* pChild = (CNetworkChild*)GetItemData( hItem );
	if ( ! pChild || ! pChild->IsKindOf( RUNTIME_CLASS(CNetworkChild) ) ) return NULL;

	if ( nChType == -1 || pChild->m_nChType == nChType ) return pChild;

	return FindNext( nChType, pItem );
}

HTREEITEM CNetworkTree::FindNext(CNetworkChild* pChild, HTREEITEM* pItem) const
{
	HTREEITEM hItem = GetNextItem( pItem ? *pItem : NULL,
				( pItem && *pItem ) ? TVGN_NEXT : TVGN_CHILD );
	if ( pItem != NULL ) *pItem = hItem;
	if ( hItem == NULL ) return NULL;

	if ( (CNetworkChild*)GetItemData( hItem ) == pChild ) return ( *pItem );

	return FindNext( pChild, pItem );
}

void CNetworkTree::SelectFirst()
{
	HTREEITEM hItem = GetNextItem( NULL, TVGN_CHILD );
	if ( hItem != NULL ) SelectItem( hItem );
}

int CNetworkTree::GetChString(int nChType, CString& xChType) const
{
	static struct {INT nChType; LPCTSTR pszName; INT nImage;} pChannel[] = {
	{ CHN_ANALOG,	_T("模拟中继"),		27, },
	{ NULL },
	{ CHN_USER,		_T("坐席通道"),		29, },
	{ NULL },
	{ CHN_SS1,		_T("一号信令"),		16, },
	{ CHN_FAX,		_T("传真资源"),		31, },
	{ CHN_TUP,		_T("七号信令[TUP]"),16, },
	{ CHN_ISDN_USER,_T("ISDN信令[USER]"),16, },
	{ CHN_ISDN_NET,	_T("ISDN信令[NET]"),16, },
	{ CHN_SOFTFAX,	_T("传真资源"),		31, },
	{ NULL },
	{ CHN_ISUP,		_T("七号信令[ISUP]"),16, },
	{ CHN_UNKNOWN,	_T("未知类型"),		18, } };
	
	if ( nChType < 0 || nChType > CHN_UNKNOWN ) nChType = CHN_UNKNOWN;
	
	xChType	= pChannel[ nChType ].pszName;
	return pChannel[ nChType ].nImage;
}
