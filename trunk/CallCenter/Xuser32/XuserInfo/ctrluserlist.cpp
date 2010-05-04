//
// CtrlUserList.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlUserList.h"
#include "Settings.h"
#include "WndUser.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CUserList, CListCtrl)
BEGIN_MESSAGE_MAP(CUserList, CListCtrl)
	//{{AFX_MSG_MAP(CUserList)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblClk)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserList construction

CUserList::CUserList()
{
}

CUserList::~CUserList()
{
}

/////////////////////////////////////////////////////////////////////////////
// CUserList message handlers

BOOL CUserList::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	dwStyle |= WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|LVS_REPORT|LVS_NOLABELWRAP|LVS_SHOWSELALWAYS;
	return CListCtrl::Create( dwStyle, rect, pParentWnd, nID );
}

int CUserList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CListCtrl::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	
	InsertColumn( 0, _T("语音文件"), LVCFMT_LEFT,  256 );
	InsertColumn( 1, _T("机主号码"), LVCFMT_LEFT,  94 );
	InsertColumn( 2, _T("接收时间"), LVCFMT_LEFT, 128 );
	InsertColumn( 3, _T("呼叫者"),   LVCFMT_LEFT, 94 );
	InsertColumn( 4, _T("状态"),   LVCFMT_LEFT, 80 );
	InsertColumn( 5, _T("流水号"),   LVCFMT_LEFT, 80 );
	
	return 0;
}

void CUserList::OnDblClk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	GetParent()->SendMessage( WM_COMMAND, ID_MEDIA_PLAY );
}

/////////////////////////////////////////////////////////////////////////////
// CUserList custom message handlers

void CUserList::OnCustomDraw(NMLVCUSTOMDRAW* pNotify, LRESULT* pResult)
{
	if ( pNotify->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pNotify->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		BOOL bSelected		= GetItemState( pNotify->nmcd.dwItemSpec, TVIS_SELECTED ) & TVIS_SELECTED ? TRUE : FALSE;
		
		pNotify->clrText	= bSelected ? GetSysColor( COLOR_HIGHLIGHTTEXT ) : RGB( 49, 98, 175 );
		pNotify->clrTextBk	= ((pNotify->nmcd.dwItemSpec+1) % 5) ? RGB( 255, 255, 255 ) : RGB( 235, 235, 255 );
		
		*pResult = CDRF_DODEFAULT;
	}
	else
	{
		*pResult = 0;
	}
}

BOOL CUserList::LoadRecordset(CXMLElement* pXML)
{
	DeleteAllItems();
	CWaitCursor pCursor;
	
	for ( POSITION posNext = pXML->GetElementIterator(); posNext; )
	{
		CXMLElement* pSync = pXML->GetNextElement( posNext );
		if ( ! pSync->IsNamed( "SyncOrder" ) ) continue;
		
		static LPCTSTR pszSuffix[] =
		{ _T("xPCMName"), _T("xPhoneNum"), _T("xRecDate1"), _T("xCaller"), _T("xTheState"),
		  _T("xjsjbh"), NULL };
		
		int nItem = InsertItem( GetItemCount(), "" );
		for ( int i = 0; pszSuffix[i] != NULL; i ++ )
		{
			CXMLAttribute* pAttri = pSync->GetAttribute( pszSuffix[i] );
#if 0
			if ( pAttri && ! pAttri->GetName().CompareNoCase("xPCMName") )
			{
				CString str = pAttri->GetValue();
				Replace( str, "\\\\192.168.0.2\\User\\", "" );
				pAttri->SetValue( str );
			}
#endif
			if ( pAttri ) SetItemText( nItem, i, pAttri->GetValue() );
		}
	}
	
	return TRUE;
}

CString CUserList::GetTempFile(int nItem) const
{
	CString strFile = GetItemText( nItem, 0 );
	CString strPath = Settings.General.Path + _T("\\Temp");
	
	if ( strFile.ReverseFind('\\') < 0 ) strPath += strFile;
	else strPath += strFile.Mid( strFile.ReverseFind('\\') );
	
	return strPath;
}

TRISTATE CUserList::ExistTempFile(LPCTSTR lpszFile) const
{
	HANDLE hFile = CreateFile( lpszFile, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	if ( hFile == INVALID_HANDLE_VALUE ) return TS_FALSE;
	
	CloseHandle( hFile );
	return TS_TRUE;
}
