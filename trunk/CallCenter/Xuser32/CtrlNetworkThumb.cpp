//
// CtrlNetworkThumb.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CoolInterface.h"
#include "CtrlNetworkThumb.h"
#include "XML.h"

#include "WndMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkThumb, CListBox)
BEGIN_MESSAGE_MAP(CNetworkThumb, CListBox)
	//{{AFX_MSG_MAP(CNetworkThumb)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkThumb construction

CNetworkThumb::CNetworkThumb()
{
	m_brWhite.CreateSolidBrush( CoolInterface.m_crBackNormal );
}

CNetworkThumb::~CNetworkThumb()
{
}

void CNetworkThumb::ResetContent()
{
	for( int nItem = GetCount(); nItem > 0; nItem -- )
	{
		CXMLElement* pXML = (CXMLElement*)GetItemData(nItem - 1);
		delete pXML; DeleteString( nItem - 1 );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkThumb message handlers

BOOL CNetworkThumb::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd)
{
	dwStyle |= LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | WS_VSCROLL | LBS_HASSTRINGS | LBS_OWNERDRAWVARIABLE;
	return CListBox::Create( dwStyle, rect, pParentWnd, IDC_NETWORK_THUMB );
}

int CNetworkThumb::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CListBox::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	return 0;
}

void CNetworkThumb::OnDestroy() 
{
	CListBox::OnDestroy();
	
	ResetContent();
}

void CNetworkThumb::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	CDC* pDC = GetDC();
	
	CFont* pOld = (CFont*)pDC->SelectObject( &CoolInterface.m_fntNormal );
	CSize sz = pDC->GetTextExtent( (LPCTSTR) lpMeasureItemStruct->itemData );
	pDC->SelectObject( pOld );
	
	lpMeasureItemStruct->itemWidth	= sz.cx + 32;
	lpMeasureItemStruct->itemHeight	= 23;
}

void CNetworkThumb::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rcItem, rcText;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	if ( lpDrawItemStruct->itemID == (UINT)-1 ) return;
	BOOL	bSelected	= lpDrawItemStruct->itemState & ODS_SELECTED;
	
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);
	//LPCTSTR lpszText = (LPCTSTR) lpDrawItemStruct->itemData;
	
	rcItem.CopyRect( &lpDrawItemStruct->rcItem );
//	rcItem.OffsetRect( -rcItem.left, -rcItem.top );
	
	rcText.CopyRect( &rcItem );
	rcText.left += 24;
	rcText.right -= 2;
	
	if ( bSelected )
	{
		pDC->Draw3dRect( rcItem.left + 1, rcItem.top + 1,
			rcItem.Width() - 2, rcItem.Height() - 1,
			CoolInterface.m_crBorder, CoolInterface.m_crBorder );
		pDC->FillSolidRect( rcItem.left + 2, rcItem.top + 2,
			rcItem.Width() - 4, rcItem.Height() - 3,
			CoolInterface.m_crBackSel );
		
		pDC->SetBkColor( CoolInterface.m_crBackSel );
	}
	else
	{
		pDC->FillSolidRect( rcItem.left + 1, rcItem.top + 1,
			rcItem.Width() - 2, rcItem.Height() - 1,
			CoolInterface.m_crBackNormal );
		pDC->SetBkColor( CoolInterface.m_crBackNormal );
	}
	
	CPoint pt( rcItem.left + 4, rcItem.top + 4 );
	int nIcon = CoolInterface.ImageForID( ID_TAB_CONNECT );
	
	if ( bSelected )
	{
		pt.Offset( 1, 1 );
		pDC->SetTextColor( CoolInterface.m_crShadow );
		CoolInterface.m_pImages.Draw( pDC, nIcon, pt, ILD_MASK );
		pt.Offset( -2, -2 );
		CoolInterface.m_pImages.Draw( pDC, nIcon, pt, ILD_TRANSPARENT );
	}
	else
	{
		ImageList_DrawEx( CoolInterface.m_pImages.m_hImageList, nIcon, pDC->GetSafeHdc(),
			pt.x, pt.y, 0, 0, CLR_NONE, CoolInterface.m_crMargin, ILD_BLEND25 );
	}
	
	CString sText;
	CListBox::GetText(lpDrawItemStruct->itemID, sText);
	
	CFont* pOld = (CFont*)pDC->SelectObject(
		( lpDrawItemStruct->itemState & ODS_DEFAULT ) ?
		&CoolInterface.m_fntBold : &CoolInterface.m_fntNormal );
	
	pDC->SetBkMode( TRANSPARENT );
	pDC->SetTextColor( bSelected ? CoolInterface.m_crCmdTextSel : CoolInterface.m_crCmdText );
	pDC->DrawText( sText, &rcText, DT_SINGLELINE|DT_VCENTER|DT_LEFT );
	
	pDC->SelectObject( pOld );
}

BOOL CNetworkThumb::Layout(CXMLElement* pXML)
{
	ResetContent();
	
	for ( POSITION posNext = pXML->GetElementIterator(); posNext; )
	{
		CXMLElement* pMap = pXML->GetNextElement( posNext );
		if ( ! pMap->IsNamed( "SyncOrder" ) ) continue;
		
		CreateJob( pMap );
	}
	
	return TRUE;
}

BOOL CNetworkThumb::CreateJob(CXMLElement* pXML)
{
	CString strValue;
	
	if ( pXML->GetAttribute( "text" ) )
		strValue = pXML->GetAttributeValue( "text" );
	else
		strValue = pXML->GetValue();
	
	int nItem = AddString( (LPCTSTR)strValue );
	SetItemData( nItem, (DWORD)pXML->Clone() );
#if 0
	CDC* pDC = GetDC();
	CSize sz = pDC->GetTextExtent( strValue );

	int nScroll = GetHorizontalExtent();
	nScroll = max( nScroll, sz.cx );
	
	SetHorizontalExtent( nScroll );
	ReleaseDC( pDC );
#endif
	return TRUE;
}

HBRUSH CNetworkThumb::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetBkColor( CoolInterface.m_crBackNormal );
	
	return m_brWhite;
}

void CNetworkThumb::OnDblClick() 
{
	int nItem = GetCurSel();
	if ( nItem < 0 ) return;
	
	CXMLElement* pXML = (CXMLElement*)GetItemData( nItem );
	ASSERT( pXML != NULL );
	
	CChildWnd* pChild = FromHandle( pXML );
}

BOOL CNetworkThumb::PreTranslateMessage(MSG* pMsg) 
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_RETURN )
		{
			PostMessage( WM_COMMAND, MAKELONG( GetDlgCtrlID(), LBN_DBLCLK ), (LPARAM)GetSafeHwnd() );
		}
	}
	
	return CListBox::PreTranslateMessage(pMsg);
}

CChildWnd* CNetworkThumb::FromHandle(CXMLElement* pXML)
{
	CString strValue = pXML->GetValue();
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	
	HINSTANCE hInstance = LoadLibrary( strValue + ".dll" );
	if ( hInstance == NULL ) return NULL;
	
	AfxSetResourceHandle(hInstance);
	typedef CChildWnd* (* PCTOR) (CXMLElement*, CWindowManager*);
	PCTOR pCtor = (PCTOR) GetProcAddress( hInstance, _T("Create") );
	
	if ( NULL != pCtor )
	{
		CMainWnd* pMainWnd = (CMainWnd*)AfxGetMainWnd();
		
		CChildWnd* pChild = pCtor( pXML, &pMainWnd->m_pWindows );
		pChild->m_hInstance = hInstance;
		
		AfxSetResourceHandle( hPrevInst );
		return pChild;
	}
	
	FreeLibrary( hInstance );
	return NULL;
}

CXMLElement* CNetworkThumb::FindJob(LPCTSTR lpszJob) const
{
	for ( int nItem = 0; nItem < GetCount(); nItem++ )
	{
		if ( CXMLElement* pXML = (CXMLElement*)GetItemData( nItem ) )
		{
			if ( ! pXML->GetValue().CompareNoCase( lpszJob ) ) return pXML;
		}
	}
	
	return NULL;
}
