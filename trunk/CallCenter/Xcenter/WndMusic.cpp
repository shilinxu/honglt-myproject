//
// WndMusic.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "WndMusic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CMusicWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CMusicWnd, CChildWnd)
	//{{AFX_MSG_MAP(CMusicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_MUSIC_SEARCH, OnReportSearch)
	ON_COMMAND(ID_MUSIC_CLEAR, OnReportClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define SPLIT_SIZE		6

/////////////////////////////////////////////////////////////////////////////
// CMusicWnd construction

CMusicWnd::CMusicWnd() : CChildWnd(FALSE)
{
	Create( IDR_MUSICFRAME );
}

CMusicWnd::~CMusicWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMusicWnd message handlers

int CMusicWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel

	m_wndPanel.Create( this );

	// List
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOLABELWRAP|WS_VSCROLL|WS_TABSTOP|LVS_SHOWSELALWAYS,
		rectDefault, this, IDC_REPORT_LIST );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	
	m_wndList.InsertColumn( 0, _T("发生时间"), LVCFMT_LEFT, 120 );
	m_wndList.InsertColumn( 1, _T("发起人"), LVCFMT_LEFT,  94 );
	m_wndList.InsertColumn( 2, _T("接收人"), LVCFMT_LEFT,  94 );
	m_wndList.InsertColumn( 3, _T("歌名"),   LVCFMT_LEFT,  250 );
	m_wndList.InsertColumn( 4, _T("累计"),   LVCFMT_LEFT,  62 );
	m_wndList.InsertColumn( 5, _T("操作员"), LVCFMT_LEFT,  80 );

	return 0;
}

void CMusicWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
}

void CMusicWnd::OnSize(UINT nType, int cx, int cy) 
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

void CMusicWnd::OnReportSearch() 
{
	CString strGroup = _T("");
	CString strQuery = m_wndPanel.GetSearch( strGroup );
	
	m_wndList.DeleteAllItems();
	int nCount = strGroup.IsEmpty() ?
		LoadRecordset(strQuery) :
		LoadRecordset(strQuery, strGroup);
	
	m_wndPanel.SendMessage( WM_REPORTSEARCH, nCount );
}

void CMusicWnd::OnReportClear() 
{
	m_wndList.DeleteAllItems();
}

void CMusicWnd::OnReportPrint()
{
}

int CMusicWnd::LoadRecordset(LPCTSTR pszFilter, LPCTSTR pszGroup)
{
	CString strQuery;
	
	if ( pszGroup == NULL )
		strQuery.Format( "select * from xopermusichis" );
	else
		strQuery.Format( "select %s, count(*) as fcount"
						" from xopermusichis", pszGroup );
	
	strQuery += " where "; strQuery += pszFilter;
	
	if ( pszGroup != NULL )
	{
		strQuery += " group by "; strQuery += pszGroup;
		strQuery += " order by count(*) desc";
	}
	
	TRACE( strQuery + "\n" );
	
	try
	{
		xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);

		for ( int nItem = 0; ! m_pRecordset->adoEOF; m_pRecordset->MoveNext() )
		{
			nItem = m_wndList.InsertItem( nItem, "" );
			LoadRecord( nItem++, pszGroup );
		}

		return m_pRecordset->RecordCount;
	}
	catch(_com_error e) 
	{ 
		AfxMessageBox( CString("数据库操作失败：") + e.ErrorMessage() );
		return -1;
	}
}

void CMusicWnd::LoadRecord(int nItem, LPCTSTR pszGroup)
{
	_variant_t TheValue;
	
	if ( pszGroup == NULL )
	{
		TheValue = m_pRecordset->GetCollect( "xopertime" );
		CString str = COleDateTime(TheValue).Format("%m-%d %H:%M:%S");
		m_wndList.SetItemText( nItem, 0, (LPCTSTR)str );
	}
	
	if ( ! pszGroup || ! _tcscmp(pszGroup,"xcallphone") )
	{
		TheValue = m_pRecordset->GetCollect( "xcallphone" );
		m_wndList.SetItemText( nItem, 1, (LPCTSTR)(_bstr_t)TheValue );
	}
	
	if ( pszGroup == NULL )
	{
		TheValue = m_pRecordset->GetCollect( "xcalledphone" );
		m_wndList.SetItemText( nItem, 2, (LPCTSTR)(_bstr_t)TheValue );
	}
	
	if ( ! pszGroup || ! _tcscmp(pszGroup,"xmusicname") )
	{
		TheValue = m_pRecordset->GetCollect( "xmusicname" );
		m_wndList.SetItemText( nItem, 3, (LPCTSTR)(_bstr_t)TheValue );
	}
	
	if ( pszGroup != NULL )
	{
		TheValue = m_pRecordset->GetCollect( "fcount" );
		m_wndList.SetItemText( nItem, 4, (LPCTSTR)(_bstr_t)TheValue );
	}
	
	if ( pszGroup == NULL )
	{
		TheValue = m_pRecordset->GetCollect( "xcode" );
		m_wndList.SetItemText( nItem, 5, (LPCTSTR)(_bstr_t)TheValue );
	}
}
