//
// CtrlHomePanel.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "Neighbour.h"
#include "CtrlHomePanel.h"
#include "CoolInterface.h"
#include "WndHome.h"
#include "IPacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CHomePanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CHomePanel, CTaskPanel)
	//{{AFX_MSG_MAP(CHomePanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CHomeTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CHomeTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CHomeTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGEDW, IDC_HOME_TREE, OnSelectPage)
	ON_NOTIFY(TVN_SELCHANGEDA, IDC_HOME_TREE, OnSelectPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CHomeListenBox, CTaskBox)
BEGIN_MESSAGE_MAP(CHomeListenBox, CTaskBox)
	//{{AFX_MSG_MAP(CHomeListenBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	10

/////////////////////////////////////////////////////////////////////////////
// CHomePanel construction

CHomePanel::CHomePanel()
{
}

CHomePanel::~CHomePanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHomePanel message handlers

BOOL CHomePanel::Create(CWnd* pParentWnd)
{
	return CTaskPanel::Create( WS_VISIBLE, CRect(0,0,0,0), pParentWnd, IDC_HOME_PANEL );
}

int CHomePanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	m_wndTask.Create( this, 140, _T("资源"), IDI_TASKPANEL );
	m_wndListen.Create( this, 147, _T("操作"), IDI_SEARCH_FOLDER );
	
	AddBox( &m_wndTask );
	AddBox( &m_wndListen );
	
	return 0;
}

void CHomePanel::ExecuteFirst()
{
	m_wndTask.SelectFirst();
	m_wndTask.SetFocus();
}

void CHomePanel::ExecuteClient(int nChType)
{
//	m_wndTask.AddClient( nChType );
}

void CHomePanel::ExecuteRedraw(int nChType)
{
//TRACE("%i -> Redraw\n", GetTickCount());
	CHomeWnd* pHomeWnd = (CHomeWnd*)GetParent();
	pHomeWnd->RedrawChild( nChType );
}

/////////////////////////////////////////////////////////////////////////////
// CHomeTaskBox construction

CHomeTaskBox::CHomeTaskBox()
{
}

CHomeTaskBox::~CHomeTaskBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHomeTaskBox message handlers

int CHomeTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndTree.Create( WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_TRACKSELECT,
		rc, this, IDC_HOME_TREE );
	m_wndTree.SetBkColor( CoolInterface.m_crTaskBoxClient );
	m_wndTree.SetImageList( &CoolInterface.m_pImages, TVSIL_NORMAL );
	
	SetPrimary( TRUE );
	/*
AddClient(2,MAKELONG(9999,100));
AddClient(5,MAKELONG(339,64));
AddClient(2,MAKELONG(10,120));
AddClient(5,MAKELONG(100,4));
AddClient(5,MAKELONG(200,120));
	*/
	return 0;
}

void CHomeTaskBox::OnDestroy()
{
}

void CHomeTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 0 );
	
	DeferWindowPos( hDWP, m_wndTree, NULL, BOX_MARGIN, BOX_MARGIN,
		cx - BOX_MARGIN * 2, cy - BOX_MARGIN * 2, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CHomeTaskBox::OnSelectPage(NMHDR* pNMHDR, LRESULT* pResult)
{
//	TRACE("%i -> OnSelect\n",GetTickCount());
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	if ( ( pNMTreeView->itemNew.state & TVIS_SELECTED ) == 0 ) return;
	DWORD nChData = m_wndTree.GetItemData( m_wndTree.GetSelectedItem() );

	CHomePanel* pPanel = (CHomePanel*)GetPanel();
	pPanel->ExecuteRedraw( nChData );
}
#if 0
void CHomeTaskBox::AddClient(WORD nChType, DWORD nChMask)
{
	DWORD dwClient = 0;
	PGROUP_INFO pClient = PGROUP_INFO(&dwClient);
	
	pClient->nChType	= nChType;
	pClient->nChFirst	= LOWORD( nChMask );
	pClient->nChLength	= HIWORD( nChMask );
	m_pChList.Add( dwClient );
	
	for (	HTREEITEM hItem = m_wndTree.GetNextItem( NULL, TVGN_CHILD );
		hItem ; hItem = m_wndTree.GetNextItem( hItem, TVGN_NEXT ) )
	{
		DWORD nGroupId = m_wndTree.GetItemData( hItem );
		if ( nGroupId == nChType ) break;
	}
	
	if ( hItem == NULL )
	{
		CString xChType = _T("");
		int nImage = Network.GetChString( nChType, xChType );
		
		m_wndTree.InsertItem( TVIF_PARAM|TVIF_TEXT|TVIF_STATE|TVIF_IMAGE|TVIF_SELECTEDIMAGE,
			(LPCTSTR)xChType, nImage, nImage, TVIS_EXPANDED,
			TVIS_EXPANDED, (LPARAM)nChType, TVI_ROOT, TVI_LAST );
	}
}
#else
void CHomeTaskBox::DrawChType(int nChType)
{
	CString xChType = _T("");
	int nImage = Neighbour.GetChString( nChType, xChType );
	
	for ( HTREEITEM hItem = m_wndTree.GetNextItem( NULL, TVGN_CHILD );
		hItem ; hItem = m_wndTree.GetNextItem( hItem, TVGN_NEXT ) )
	{
		DWORD nGroupId = m_wndTree.GetItemData( hItem );
		if ( (int)nGroupId == nChType ) break;
#if 0
		CString strText = m_wndTree.GetItemText( hItem );
		if ( ! strText.CompareNoCase( xChType ) ) break;
#endif
	}
	
	if ( hItem == NULL )
	{
		hItem = m_wndTree.InsertItem( TVIF_PARAM|TVIF_TEXT|TVIF_STATE|TVIF_IMAGE|TVIF_SELECTEDIMAGE,
			(LPCTSTR)xChType, nImage, nImage, TVIS_EXPANDED,
			TVIS_EXPANDED, (LPARAM)nChType, TVI_ROOT, TVI_LAST );
	}
	
#if 0
	if ( ! m_wndTree.GetSelectedItem() ) SelectFirst();
#else
	if ( ! m_wndTree.GetSelectedItem() )
	{
//TRACE("%i -> Select new item\n",GetTickCount());
		m_wndTree.SelectItem( hItem );
	NMHDR pNM = { m_wndTree.GetSafeHwnd(), m_wndTree.GetDlgCtrlID(), TVN_SELCHANGED };
	SendMessage( WM_NOTIFY, pNM.idFrom, (LPARAM)&pNM );
	}
	else
	{
//TRACE("%i -> Select old item\n",GetTickCount());
		m_wndTree.SelectItem( m_wndTree.GetSelectedItem() );
	NMHDR pNM = { m_wndTree.GetSafeHwnd(), m_wndTree.GetDlgCtrlID(), TVN_SELCHANGED };
	SendMessage( WM_NOTIFY, pNM.idFrom, (LPARAM)&pNM );
	}
#endif
}
#endif
void CHomeTaskBox::SelectFirst()
{
	HTREEITEM hItem = m_wndTree.GetNextItem( NULL, TVGN_CHILD );
	if ( hItem != NULL ) m_wndTree.SelectItem( hItem );
}

void CHomeTaskBox::Empty()
{
	m_wndTree.DeleteAllItems();
//	CHomePanel* pPanel = (CHomePanel*)GetPanel();
//	pPanel->ExecuteRedraw( -1 );
}

/////////////////////////////////////////////////////////////////////////////
// CHomeListenBox construction

CHomeListenBox::CHomeListenBox()
{
	m_bPending	= FALSE;
}

CHomeListenBox::~CHomeListenBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHomeListenBox message handlers

int CHomeListenBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndChan.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWN|CBS_SORT,
		rc, this, IDC_HOME_CHAN );
	m_wndRecord.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWN|CBS_AUTOHSCROLL,
		rc, this, IDC_HOME_RECORD );
	m_wndPlay.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWN|CBS_AUTOHSCROLL,
		rc, this, IDC_HOME_PLAY );
	
	m_wndChan.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndRecord.SetFont( &CoolInterface.m_fntNormal );
	m_wndRecord.ModifyStyle( 0, WS_DISABLED );
//	m_wndRecord.SetDroppedWidth( 165 );
//	m_wndRecord.AddString( "无" );
//	m_wndRecord.AddString( "<CHAN>-<DATE><TIME>.pcm" );
//	m_wndRecord.SetCurSel( 0 );
	
	m_wndPlay.SetFont( &CoolInterface.m_fntNormal );
	m_wndPlay.AddString( "监听" );
//	m_wndPlay.AddString( "录音" );
	m_wndPlay.SetCurSel( 0 );
	
	m_wndStart.Create( rc, this, IDOK );
	m_wndStart.SetWindowText( "确定" );
	m_wndStart.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndStart.SetHandCursor( TRUE );
	
	return 0;
}

void CHomeListenBox::OnDestroy()
{
}

void CHomeListenBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 2 );
	
	DeferWindowPos( hDWP, m_wndChan, NULL, BOX_MARGIN, 27,
		cx - BOX_MARGIN * 2, 256, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndRecord, NULL, BOX_MARGIN, 71,
		cx - BOX_MARGIN * 2, 256, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndPlay, NULL, BOX_MARGIN, 118,
		cx - BOX_MARGIN * 2 - 80, 256, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndStart, NULL, cx - 80, 117, 72, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CHomeListenBox::OnPaint() 
{
	CPaintDC dc( this );
	CRect rc, rct;
	CString str;
	
	UINT nFlags = ETO_CLIPPED;
	CDC* pDC = &dc;
	
	GetClientRect( &rc );
	
	if ( m_bmWatermark.m_hObject != NULL )
	{
		pDC = CoolInterface.GetBuffer( dc, rc.Size() );
		CoolInterface.DrawWatermark( pDC, &rc, &m_bmWatermark );
		pDC->SetBkMode( TRANSPARENT );
	}
	else
	{
		pDC->SetBkMode( OPAQUE );
		pDC->SetBkColor( CoolInterface.m_crTaskBoxClient );
		nFlags |= ETO_OPAQUE;
	}
	
	CFont* pOldFont = (CFont*)pDC->SelectObject( &CoolInterface.m_fntNormal );
	
	pDC->SetTextColor( 0 );
	
	rct.SetRect( BOX_MARGIN + 1, BOX_MARGIN, rc.right - BOX_MARGIN, BOX_MARGIN + 16 );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "通道号:", NULL );
	pDC->ExcludeClipRect( &rct );

	rct.OffsetRect( 0, 54 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "保存录音的文件名:", NULL );
	pDC->ExcludeClipRect( &rct );

	rct.OffsetRect( 0, 102 - rct.top );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "命令:", NULL );
	pDC->ExcludeClipRect( &rct );
	
	pDC->SelectObject( pOldFont );
	
	if ( pDC != &dc )
	{
		dc.BitBlt( 0, 0, rc.Width(), rc.Height(), pDC, 0, 0, SRCCOPY );
		pDC->SelectClipRgn( NULL );
	}
	else
	{
		pDC->FillSolidRect( &rc, CoolInterface.m_crTaskBoxClient );
	}
}

void CHomeListenBox::OnOK()
{
	if ( ! Neighbour.IsConnected() ) return;
	
	IPacket* pPacket = GetTask( m_bPending == FALSE );
	if ( pPacket ) Neighbour.SendPacket( pPacket );
	
	if ( m_bPending = ! m_bPending )
	{
		SetCaption( "正在执行...");
		m_wndStart.SetWindowText( "停止" );
	}
	else
	{
		SetCaption( "操作");
		m_wndStart.SetWindowText( "确定" );
	}
	
/*	CString strValue;
	
	m_wndChan.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	if ( strValue.IsEmpty() ) return;
	int nChan = _ttoi( strValue );
*/	
}

IPacket* CHomeListenBox::GetTask(BOOL bRestart)
{
	CString strValue;
	
	m_wndChan.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	if ( strValue.IsEmpty() ) return NULL;
	int nChan = _ttoi( strValue );
	
	int nItem = m_wndPlay.GetCurSel();
	if ( nItem == -1 )
	{
		m_wndPlay.GetWindowText( strValue );
		strValue += "\r\n";
		return IPacket::ReadBuffer( (BYTE*)strValue.GetBuffer(0), strValue.GetLength() );
	}
	else
	if ( nItem == 0 )	// LIS
	{
		ILisnPacket* pPacket = new ILisnPacket( nChan );
		if ( ! bRestart ) CopyMemory( pPacket->PakParam, "STOP", 4 );
		return pPacket;
	}
	
	return NULL;
}

void CHomeListenBox::SetChan(int nChan)
{
	CString strValue;
	strValue.Format( "%i", nChan );
	
	m_wndChan.SetWindowText( strValue );
}

/*	
void CHomePanel::ExecuteStart()
{
	CString strCaption;
	m_wndResult.GetWindowText(strCaption);
	m_wndResult.SetCaption( "正在查询...");
	
	CWaitCursor pCursor;
	GetParent()->SendMessage( WM_COMMAND, ID_REPORT_SEARCH );
	m_wndResult.SetCaption( (LPCTSTR)strCaption );
}
	CHomePanel* pPanel = (CHomePanel*)GetPanel();
//	pPanel->ExecuteStart();
}
*/