//
// CtrlIconCombo.cpp
//

#include "StdAfx.h"
#include "Xcenter.h"
#include "CtrlIconCombo.h"
#include "CoolInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CIconComboCtrl, CComboBox)
	//{{AFX_MSG_MAP(CIconComboCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIconComboCtrl construction

CIconComboCtrl::CIconComboCtrl()
{
}

CIconComboCtrl::~CIconComboCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CIconComboCtrl operations

BOOL CIconComboCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_OWNERDRAWVARIABLE|CBS_HASSTRINGS;//|CBS_SORT;
	return CComboBox::Create( dwStyle, rect, pParentWnd, nID );
}

int CIconComboCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if ( ( GetStyle() & CBS_OWNERDRAWVARIABLE ) == 0 )
	{
		ModifyStyle( 0, CBS_OWNERDRAWVARIABLE|CBS_HASSTRINGS );
	}

	SetFont( &CoolInterface.m_fntNormal );
	
	return 0;
}

void CIconComboCtrl::OnDestroy() 
{
	CComboBox::OnDestroy();
	
	Clear();
}

int CIconComboCtrl::AddItem(UINT nID, int nImage, DWORD dwParam, BOOL bFocus)
{
	CString str;
	LoadString( str, nID );

	return AddItem( (LPCTSTR)str, nImage, dwParam, bFocus );
}

int CIconComboCtrl::AddItem(LPCTSTR pszTitle, int nImage, DWORD dwParam, BOOL bFocus)
{
	Item* pItem = new Item;
	pItem->Image= nImage;
	pItem->Param= dwParam;

	int nIndex = AddString( pszTitle );
	SetItemData( nIndex, (LPARAM)pItem );
			
	if ( bFocus ) nIndex = SetCurSel( nIndex );

	return nIndex;
}

void CIconComboCtrl::RemoveItem(int nItem)
{
	Item* pItem = (Item*)GetItemData( nItem );
	
	if ( pItem ) delete pItem;
	DeleteString( nItem );
	
//	nItem = GetNextItem( nItem - 1, LVNI_ALL  );
	if ( nItem == -1 ) nItem = GetCount() - 1;
	
	SetCurSel( nItem );
}

void CIconComboCtrl::Clear()
{
	for ( int nItem = GetCount() - 1 ; nItem >= 0 ; nItem-- )
	{
		RemoveItem( nItem );
	}
	
	GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(),CBN_SELCHANGE), (LPARAM)m_hWnd);
}

int CIconComboCtrl::SetCurSel(int nItem)
{
	CComboBox::SetCurSel( nItem );
	GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(),CBN_SELCHANGE), (LPARAM)m_hWnd);

	return nItem;
}

BOOL CIconComboCtrl::SetImageList(CImageList* pImageList)
{
	m_pImages = pImageList; return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CIconComboCtrl message handlers

void CIconComboCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemWidth	= 1024;
	lpMeasureItemStruct->itemHeight	= 18;
}

void CIconComboCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if ( lpDrawItemStruct->itemID == (UINT)-1 ) return;
	if ( ( lpDrawItemStruct->itemAction & ODA_SELECT ) == 0 &&
		 ( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE ) == 0 ) return;
	
	CRect rcItem( &lpDrawItemStruct->rcItem );
	CPoint pt( rcItem.left + 1, rcItem.top + 1 );
	CString str;
	CDC dc;
	
	dc.Attach( lpDrawItemStruct->hDC );
	
	CFont* pOldFont = (CFont*)dc.SelectObject( &CoolInterface.m_fntNormal );
	dc.SetTextColor( GetSysColor( ( lpDrawItemStruct->itemState & ODS_SELECTED )
		? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT ) );
	
	dc.FillSolidRect( &rcItem,
		GetSysColor( ( lpDrawItemStruct->itemState & ODS_SELECTED )
		? COLOR_HIGHLIGHT : COLOR_WINDOW ) );
	dc.SetBkMode( TRANSPARENT );
	
	Item* pItem = (Item*)lpDrawItemStruct->itemData;
	if ( pItem == NULL )
	CoolInterface.m_pImages.Draw( &dc, lpDrawItemStruct->itemID + 1, pt,
		( lpDrawItemStruct->itemState & ODS_SELECTED ) ? ILD_SELECTED : ILD_NORMAL );
	else
	CoolInterface.m_pImages.Draw( &dc, pItem->Image, pt,
		( lpDrawItemStruct->itemState & ODS_SELECTED ) ? ILD_SELECTED : ILD_NORMAL );
	
	GetLBText( lpDrawItemStruct->itemID, str );
	
	rcItem.left += 22; rcItem.right -= 2;
	dc.DrawText( str, &rcItem, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOPREFIX );
	
	dc.SelectObject( pOldFont );
	dc.Detach();
}
