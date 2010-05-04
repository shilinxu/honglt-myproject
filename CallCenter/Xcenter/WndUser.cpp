//
// WndUser.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Profiles.h"
#include "Network.h"
#include "WndUser.h"
#include "CoolInterface.h"

#include "ChannelUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CUserWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CUserWnd, CChildWnd)
	//{{AFX_MSG_MAP(CUserWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_USER_CHILD, OnCustomDrawList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define TOOLBAR_HEIGHT	28
#define SPLIT_SIZE		6

/////////////////////////////////////////////////////////////////////////////
// CUserWnd

CUserWnd::CUserWnd() : CChildWnd(TRUE)
{
	m_nRetained	= -1;
	m_bInTimer	= FALSE;
	
	Create( IDR_USERFRAME );
}

CUserWnd::~CUserWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd message handlers

int CUserWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel

	m_wndPanel.Create( this );

	// Child

	m_wndList.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOLABELWRAP|WS_VSCROLL|WS_TABSTOP|LVS_SHOWSELALWAYS,
		rectDefault, this, IDC_USER_CHILD );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );

	m_wndList.InsertColumn( 0, _T("登录时间"), LVCFMT_LEFT, 120 );
	m_wndList.InsertColumn( 1, _T("坐席"),     LVCFMT_LEFT,  72 );
	m_wndList.InsertColumn( 2, _T("工号"),     LVCFMT_LEFT,  72 );
	m_wndList.InsertColumn( 3, _T("局端号码"), LVCFMT_LEFT, 102 );
	m_wndList.InsertColumn( 4, _T("远端号码"), LVCFMT_LEFT, 102 );
	m_wndList.InsertColumn( 5, _T("中继"),     LVCFMT_LEFT,  72 );
	m_wndList.InsertColumn( 6, _T("会话时长"), LVCFMT_LEFT,  76 );
	
	m_wndList.SetBkColor( RGB( 250, 250, 255 ) );
	SetTimer( 5, 100, NULL );
	
	return 0;
}

void CUserWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CUserWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );

	HDWP hDWP = BeginDeferWindowPos( 2 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );

	DeferWindowPos( hDWP, m_wndList.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd custom message handlers

void CUserWnd::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	
	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		int nChState		= m_wndList.GetItemState( pDraw->nmcd.dwItemSpec, LVIS_STATEIMAGEMASK ) >> 12;
		BOOL bSelected		= m_wndList.GetItemState( pDraw->nmcd.dwItemSpec, TVIS_SELECTED ) & TVIS_SELECTED ? TRUE : FALSE;
		pDraw->clrText		= bSelected ? GetSysColor( COLOR_HIGHLIGHTTEXT ) : GetChColour( nChState );
		pDraw->clrTextBk	= bSelected ? GetSysColor( COLOR_HIGHLIGHT ) : RGB( 250, 250, 255 );

		*pResult = CDRF_DODEFAULT;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd common timer

void CUserWnd::OnTimer(UINT nIDEvent) 
{
	if ( m_bInTimer ) return;
	m_bInTimer = TRUE;
	
	for ( int nItem = 0; nItem < m_wndList.GetItemCount(); nItem++ )
	{
		int nChState = m_wndList.GetItemState( nItem, LVIS_STATEIMAGEMASK ) >> 12;
		if ( nChState != S_CALL_TALKING ) continue;
		
		try
		{
		CChannel* pChannel = (CChannel*)m_wndList.GetItemData(nItem);
		if ( pChannel ) UpdateItem( nItem, pChannel );
		}
		catch(_com_error e) 
		{ 
			theApp.Message( MSG_ERROR, "error on CUserWnd::[%i], %s", nItem, e.ErrorMessage() );
		}
		catch(...)
		{
			theApp.Message( MSG_ERROR, "error on CUserWnd::[%i], %s", nItem, "a fatal error" );
		}
	}
	
	m_bInTimer = FALSE;
}

BOOL CUserWnd::UpdateItem(int nItem, CChannel* pChannel)
{
	DWORD tNow = GetTickCount();
	DWORD nLength = (tNow - pChannel->m_tConnected) / 1000;
	
	CString strText;
	if ( nLength >= 3600 )
	{
		strText.Format( _T("%i:%.2i:%.2i"), nLength / 3600, ( nLength / 60 ) % 60, nLength % 60 );
	}
	else
	{
		strText.Format( _T("%.2i:%.2i"), nLength / 60, nLength % 60 );
	}
	
	m_wndList.SetItemText( nItem, 6, (LPCTSTR)strText );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd operations

CUserFluxBox* CUserWnd::GetFluxBox()
{
	return &m_wndPanel.m_wndFlux;
}

BOOL CUserWnd::SetItem(CChannel* pChannel, int nChState)
{
	if ( nChState == S_CALL_RINGING || nChState == S_CALL_WAIT_REMOTE_PICKUP )
	{
		RingNow( pChannel, nChState );
	}
	if ( nChState == S_CALL_STANDBY || nChState == S_CALL_PENDING )
	{
		CompleteTask( pChannel );
	}
	if ( nChState == S_CALL_TALKING )
	{
		LPCTSTR xCallType = _tcscmp( pChannel->CallType, CA_TALK ) ? _T("Out") : _T("In");
		GetFluxBox()->Register( pChannel->m_xFeeCode, xCallType );
	}
	
	return SetColour( pChannel, nChState );
}

BOOL CUserWnd::SetColour(CChannel* pChannel, int nChState)
{
	int nItem = FindItem( pChannel );
	if ( nItem == -1 ) return FALSE;
	
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	pItem.mask	    = LVIF_STATE;
	pItem.iItem		= nItem;
	
	pItem.stateMask = LVIS_STATEIMAGEMASK;
	pItem.state		= INDEXTOSTATEIMAGEMASK(nChState);
	
	return m_wndList.SetItem( &pItem );
}

BOOL CUserWnd::RingNow(CChannel* pChannel, int nChState)
{
	int nItem = FindItem( pChannel );
	if ( nItem == -1 ) return FALSE;
	
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	pItem.mask		= LVIF_TEXT;
	pItem.iItem		= nItem;
	
	pItem.iSubItem	= 3;
	pItem.pszText	= pChannel->LocalId;
	m_wndList.SetItem( &pItem );
	
	pItem.iSubItem	= 4;
	pItem.pszText	= pChannel->RemoteId;
	m_wndList.SetItem( &pItem );
	
	CString str;
	str.Format( "%i", pChannel->m_nTaskId );
	
	pItem.iSubItem	= 5;
	pItem.pszText	= str.GetBuffer(0);
	m_wndList.SetItem( &pItem );
	
	return TRUE;
}

BOOL CUserWnd::CompleteTask(CChannel* pChannel)
{
	int nItem = FindItem( pChannel );
	if ( nItem == -1 ) return FALSE;
	
	int nChState = m_wndList.GetItemState( nItem, LVIS_STATEIMAGEMASK ) >> 12;
	if ( nChState == S_CALL_TALKING ) RecordItem( pChannel );
	
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	pItem.mask		= LVIF_TEXT;
	pItem.iItem		= nItem;
	
	pItem.iSubItem	= 3;
	pItem.pszText	= "";
	m_wndList.SetItem( &pItem );
	
	pItem.iSubItem	= 4;
	pItem.pszText	= "";
	m_wndList.SetItem( &pItem );
	
	pItem.iSubItem	= 5;
	pItem.pszText	= "";
	m_wndList.SetItem( &pItem );
	
	pItem.iSubItem	= 6;
	pItem.pszText	= "";
	m_wndList.SetItem( &pItem );
	
	return TRUE;
}

BOOL CUserWnd::RecordItem(CChannel* pChannel)
{
	ASSERT( SsmGetChType( pChannel->m_nChan ) == CHN_USER );
	
	DWORD tNow = GetTickCount();
	DWORD nLength = (tNow - pChannel->m_tConnected) / 1000;
	
	COleDateTime xStartTime = COleDateTime::GetCurrentTime();
	xStartTime -= COleDateTimeSpan( 0, 0, 0, nLength );
	
	CString strQuery;
	strQuery.Format( "insert into xphonehis"
					" (xdate, xcaller, xcalled, xcode, xlen, xrunk)"
					" values('%s', '%s', '%s', '%s', %i, %i)",
					xStartTime.Format("%Y-%m-%d %H:%M:%S"), pChannel->RemoteId,
					pChannel->LocalId, pChannel->m_xFeeCode, nLength, pChannel->m_nTaskId );
	
	TRACE( strQuery + "\n" );
	xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adExecuteNoRecords);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd Child's Operations

int CUserWnd::AddItem(CUserChannel* pChannel)
{
	LV_ITEM pItem;
	ZeroMemory( &pItem, sizeof(pItem) );
	
	RemoveItem( pChannel );
	
	CString str = COleDateTime::GetCurrentTime().Format("%m-%d %H:%M:%S");

	pItem.mask		= LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE|LVIF_STATE;
	pItem.iItem		= m_wndList.GetItemCount();
	pItem.pszText	= str.GetBuffer(0);
	pItem.iImage	= CoolInterface.ImageForID( ID_TAB_USER );;
	pItem.stateMask = LVIS_STATEIMAGEMASK;
	pItem.state		= INDEXTOSTATEIMAGEMASK(S_CALL_STANDBY);
	pItem.lParam	= (DWORD)pChannel;
	pItem.iItem		= m_wndList.InsertItem( &pItem );

	pItem.mask		= LVIF_TEXT;
	str.Format( "%i", pChannel->m_nChan );
	pItem.pszText	= str.GetBuffer(0);
	pItem.iSubItem  = 1;
	m_wndList.SetItem( &pItem );

	pItem.mask		= LVIF_TEXT;
	pItem.pszText	= pChannel->GetUserId();
	pItem.iSubItem  = 2;
	m_wndList.SetItem( &pItem );
	
	return pItem.iItem;
}

BOOL CUserWnd::RemoveItem(CUserChannel* pChannel)
{
	int nItem = FindItem( pChannel );
	if ( nItem == -1 ) return FALSE;

	return m_wndList.DeleteItem( nItem );
}

int CUserWnd::FindItem(CChannel* pChannel) const
{
	for ( int nItem = m_wndList.GetNextItem( -1, LVNI_ALL ) ; nItem >= 0 ;
		nItem = m_wndList.GetNextItem( nItem, LVNI_ALL ) )
	{
		DWORD nClient = m_wndList.GetItemData( nItem );
		if ( nClient == (DWORD)pChannel ) return nItem;
	}

	return -1;
}

CChannel* CUserWnd::ReadItem(LPCTSTR xJob, int nChState)
{
	int nCount = m_wndList.GetItemCount();

	for ( int i = 0; i < nCount; i++ )
	{
		int nItem = m_nRetained + 1;
		m_nRetained = nItem = nItem % nCount;
		
		CXMLElement* pXML;
		CChannel* pChannel = (CChannel*)m_wndList.GetItemData( nItem );
		
		if ( ! IsPermitCalled( &pChannel->m_pHost.sin_addr, &pXML ) ) continue;
		
		CXMLAttribute* pAttribute = pXML->GetAttribute( _T("Job") );
		if ( ! Profiles.HavingText( pAttribute->GetValue(), xJob ) ) continue;
		
//		if ( pChannel->m_nTaskId != -1 ) continue;
		
		if ( nChState == -1 || SsmGetChState(pChannel->m_nChan) == nChState ) return pChannel;
	}
	
	return NULL;
}

int CUserWnd::IsWellAlived(int* pnCount) const
{
	int nCount = 0;
	if ( pnCount ) *pnCount = m_wndList.GetItemCount();

	for ( int nItem = m_wndList.GetNextItem( -1, LVNI_ALL ) ; nItem >= 0 ;
		nItem = m_wndList.GetNextItem( nItem, LVNI_ALL ) )
	{
		DWORD dwExisting = m_wndList.GetItemState( nItem, LVIS_STATEIMAGEMASK );
		
		if ( dwExisting != INDEXTOSTATEIMAGEMASK(S_CALL_STANDBY) ) nCount++;
	}
	
	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd Helpers

BOOL CUserWnd::IsPermitCalled(IN_ADDR* pAddress, CXMLElement** ppXML) const
{
	CXMLElement* pXML;
	if ( ! ppXML ) ppXML = &pXML;

	CXMLElement* pParent = Profiles[ "User" ];
	
	if ( ( *ppXML ) = Profiles.LookupKey(pParent, inet_ntoa(*pAddress), "Host") )
	{
		CXMLAttribute* pAttribute =( *ppXML )->GetAttribute( _T("Action") );
		if ( pAttribute == NULL ) return FALSE;
		
		CString strValue = pAttribute->GetValue();
		return strValue.CompareNoCase( _T("允许所有呼入") ) == 0;
	}
	
	return FALSE;
}

COLORREF CUserWnd::GetChColour(DWORD nChState) const
{
	switch ( nChState )
	{
	case	S_CALL_STANDBY:
		return RGB( 192, 192, 192 );
		
	case	S_CALL_PICKUPED:
	case	S_CALL_RINGING:
		return RGB( 249, 59, 55 );

	case	S_CALL_TALKING:
		return RGB( 0, 168, 0 );
		
	case	S_CALL_PENDING:
	case	S_CALL_RemoteBlock:
	case	S_CALL_LocalBlock:
		return RGB( 128, 128, 128 );
		
	case	S_CALL_WAIT_REMOTE_PICKUP:
		return RGB( 0, 125, 251 );
		
	case	S_CALL_UNAVAILABLE:
		return RGB( 222, 81, 42 );

	case	S_TUP_WaitPcmReset:
	case	S_ISUP_WaitReset:
		return RGB( 255, 0, 128 );
	
	default:
		return RGB( 192, rand(), 192 );
	}
}
