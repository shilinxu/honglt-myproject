//
// CtrlReportList.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "CoolInterface.h"
#include "CtrlReportList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CReportListCtrl, CListCtrl)
BEGIN_MESSAGE_MAP(CReportListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CReportListCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportListCtrl construction

CReportListCtrl::CReportListCtrl()
{
}

CReportListCtrl::~CReportListCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CReportListCtrl message handlers

BOOL CReportListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|LVS_REPORT|LVS_NOLABELWRAP;
	return CListCtrl::Create( dwStyle, rect, pParentWnd, nID );
}

int CReportListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CListCtrl::OnCreate( lpCreateStruct ) == -1 ) return -1;
	SetOwner( GetParent() );
	
//	SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );

	InsertColumn( 0, _T("发生时间"), LVCFMT_LEFT, 120 );
	InsertColumn( 1, _T("事件对象"), LVCFMT_LEFT,  94 );
	InsertColumn( 2, _T("局端号码"), LVCFMT_LEFT,  94 );
	InsertColumn( 3, _T("时长"),     LVCFMT_LEFT,  76 );
	InsertColumn( 4, _T("费用"),     LVCFMT_LEFT,  62 );
	InsertColumn( 5, _T("第三方号码"),LVCFMT_LEFT, 94 );
	InsertColumn( 6, _T("流水号"),   LVCFMT_LEFT,  70 );
	InsertColumn( 7, _T("服务名称"), LVCFMT_LEFT, 100 );
	InsertColumn( 8, _T("通话类型"), LVCFMT_LEFT,  70 );
//	InsertColumn( 9, _T("对方所在地"), LVCFMT_LEFT,120);
	
	return 0;
}

void CReportListCtrl::OnDestroy() 
{
	CListCtrl::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CReportListCtrl custom message handlers

void CReportListCtrl::OnCustomDraw(NMLVCUSTOMDRAW* pNotify, LRESULT* pResult)
{
	if ( pNotify->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pNotify->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		TRISTATE bEnabled	= GetItemState( pNotify->nmcd.dwItemSpec, LVIS_STATEIMAGEMASK ) >> 12;
		pNotify->clrText	= (bEnabled == TS_TRUE) ? GetSysColor( COLOR_WINDOWTEXT ) : RGB( 192, 192, 192 );

		*pResult = CDRF_DODEFAULT;
	}
	else
	{
		*pResult = 0;
	}
}

void CReportListCtrl::ClearWindow()
{
	DeleteAllItems();
}

int CReportListCtrl::LoadRecordset(LPCTSTR pszFilter, int* pnIgn)
{
	ClearWindow();

	CString strQuery;
	
	strQuery.Format( "select a.*, b.xphone as UsrNumber, c.xtalktype as TalkType, c.xfeename as FeeName"
					" from XuserDetail a inner join"
					" xuser b on a.xuserbindid = b.xuserbindid inner join"
					" Xfeetable c on a.xfeetableId = c.xjsjbh" );
	strQuery += " where "; strQuery += pszFilter;
	strQuery += " order by a.jsjbh desc";
	TRACE( strQuery + "\n" );

	try
	{
		if ( pnIgn ) *pnIgn = 0;
		xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);

		for ( int nItem = 0; ! m_pRecordset->adoEOF; m_pRecordset->MoveNext() )
		{
			nItem = InsertItem( nItem, "" );
			TRISTATE bEnabled = LoadRecord( nItem );
			
			if ( bEnabled != TS_TRUE && pnIgn ) (*pnIgn) ++;
			SetItemState( nItem, bEnabled << 12, LVIS_STATEIMAGEMASK );
			
			nItem += 1;
		}
		
		return m_pRecordset->RecordCount;
	}
	catch(_com_error e) 
	{ 
		AfxMessageBox( CString("数据库操作失败：") + e.ErrorMessage() );
		return -1;
	}
}

int CReportListCtrl::LoadRecord(int nItem)
{
	_variant_t TheValue = m_pRecordset->GetCollect( "jsjbh" );
	SetItemText( nItem, 6, (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xTimeStart" );
	CString str = COleDateTime(TheValue).Format("%m-%d %H:%M:%S");
	SetItemText( nItem, 0, (LPCTSTR)str );
	
	TheValue = m_pRecordset->GetCollect( "UsrNumber" );
	SetItemText( nItem, 1, (LPCTSTR)(_bstr_t)TheValue );
	TheValue = m_pRecordset->GetCollect( "xFwdPhone" );
	SetItemText( nItem, 2, (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xTimeLength" );
	if ( TheValue.vt != VT_NULL && TheValue.lVal >= 3600 )
	{
		str.Format( "%d:%02d:%02d", TheValue.lVal / 3600, TheValue.lVal % 24 / 60,
			TheValue.lVal % 24 % 60 );
		SetItemText( nItem, 3, (LPCTSTR)str );
	}
	else if ( TheValue.vt != VT_NULL )
	{
		str.Format( "%02d:%02d", TheValue.lVal / 60, TheValue.lVal % 60 );
		SetItemText( nItem, 3, (LPCTSTR)str );
	}
	
	_variant_t xTimeLength = TheValue;
	
	TheValue = m_pRecordset->GetCollect( "xExpense" );
	if ( TheValue.vt != VT_NULL )
	{
		str.Format( "%.2lf", (double)TheValue );
		SetItemText( nItem, 4, (LPCTSTR)str );
	}
	
	TheValue = m_pRecordset->GetCollect( "FeeName" );
	SetItemText( nItem, 7, (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "TalkType" );
	SetItemText( nItem, 8, (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xthirdid" );
	if ( TheValue.vt != VT_NULL ) SetItemText( nItem, 5, (LPCTSTR)(_bstr_t)TheValue );
	
	return (xTimeLength.vt != VT_NULL && xTimeLength.lVal) ? TS_TRUE : TS_FALSE;
}
