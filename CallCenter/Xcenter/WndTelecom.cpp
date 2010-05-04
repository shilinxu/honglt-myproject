//
// WndTelecom.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "Network.h"
#include "Channel.h"
#include "Skin.h"
#include "WndTelecom.h"
#include "CoolInterface.h"

#include "DlgExecute.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CTelecomWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CTelecomWnd, CChildWnd)
	//{{AFX_MSG_MAP(CTelecomWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TELECOM_LIST, OnCustomDrawList)
	ON_COMMAND(ID_P2JOB_START, OnP2jobStart)
	ON_COMMAND(ID_P2JOB_CLEAR, OnP2jobClear)
	ON_COMMAND(ID_P2JOB_SEARCH, OnSearchNumber)
	ON_COMMAND(ID_P2JOB_IMPORT, OnImportNumber)
	ON_COMMAND(ID_P2JOB_EXPORT, OnExportNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define SPLIT_SIZE		6
#define BAR_HEIGHT		28

#define DS_TEMP		(1<<13)
#define DS_ERROR	(1<<14)
#define DS_NORMAL	(1<<15)

#define MAX_MESSAGE_COUNT	1024

/////////////////////////////////////////////////////////////////////////////
// CTelecomWnd construction

CTelecomWnd::CTelecomWnd() : CChildWnd(FALSE)
{
	m_hThread		= NULL;
	m_bThread		= FALSE;
	m_pRecordset	= NULL;

	CString strCaption;
	strCaption.LoadString( IDR_TELECOMFRAME );
	
	Create( IDI_TASKPANEL, (LPCTSTR)strCaption );
}

CTelecomWnd::~CTelecomWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTelecomWnd operations

BOOL CTelecomWnd::Create(UINT nID, LPCTSTR xCaption) 
{
	m_nResID = nID;
	
	return CMDIChildWnd::Create( NULL, xCaption, WS_CHILD |
		 WS_OVERLAPPEDWINDOW | WS_VISIBLE );
}

/////////////////////////////////////////////////////////////////////////////
// CTelecomWnd message handlers

int CTelecomWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel

	m_wndPanel.Create( this );

	// ListCtrl
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOLABELWRAP|WS_VSCROLL|WS_TABSTOP|LVS_SHOWSELALWAYS,
		rectDefault, this, IDC_TELECOM_LIST );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );

	m_wndList.InsertColumn( 0, _T("发生时间"), LVCFMT_LEFT, 120 );
	m_wndList.InsertColumn( 1, _T("事件对象"), LVCFMT_LEFT,  94 );
	m_wndList.InsertColumn( 2, _T("事件代码"), LVCFMT_LEFT,  80 );
	m_wndList.InsertColumn( 3, _T("事件描述"), LVCFMT_LEFT, 250 );
	
	// Toolbar
	
	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	m_wndToolBar.SetOwner( this );
	
	if ( ! m_wndSearch.Create( WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rectDefault, &m_wndToolBar, IDC_SEARCH_NUMBER ) ) return -1;
	m_wndSearch.SetFont( &CoolInterface.m_fntNormal );
	
	Skin.CreateToolBar( _T("CTelecomWnd"), &m_wndToolBar );

	return 0;
}

void CTelecomWnd::OnDestroy() 
{
	StopThread();
	
	CChildWnd::OnDestroy();
}

void CTelecomWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );

	HDWP hDWP = BeginDeferWindowPos( 3 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );

	DeferWindowPos( hDWP, m_wndList.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH, rc.Height() - BAR_HEIGHT, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndToolBar.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.bottom - BAR_HEIGHT, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CTelecomWnd::OnP2jobStart() 
{
	StartThread();
}

void CTelecomWnd::OnP2jobClear() 
{
}

void CTelecomWnd::OnSearchNumber() 
{
//	CString str;
//	m_wndSearch.GetWindowText( str );
}

void CTelecomWnd::OnImportNumber() 
{
	CHAR xDirPath[MAX_PATH + 1];
	xDirPath[ GetCurrentDirectory(MAX_PATH,xDirPath) ] = '\0';
	
	LPCTSTR pszFilter = _T("Text Files|*.txt|All Files|*.*||");
	CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ENABLESIZING, pszFilter, this );
	
	if ( dlg.DoModal() != IDOK ) return;
	SetCurrentDirectory( xDirPath );
	
	CString str;
	str.Format( "您确定要导入\"%s\"的号码，并发布吗？", dlg.GetPathName() );
	if (AfxMessageBox(str, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDCANCEL)
	{
		return;
	}
	
	CImportNumber* dlgExecute = new CImportNumber( dlg.GetPathName() );
	dlgExecute->Start();
}

void CTelecomWnd::OnExportNumber() 
{
	CHAR xDirPath[MAX_PATH + 1];
	xDirPath[ GetCurrentDirectory(MAX_PATH,xDirPath) ] = '\0';
	
	LPCTSTR pszFilter = _T("Text Files|*.txt|All Files|*.*||");
	CFileDialog dlg( FALSE, _T(".txt"), NULL, OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_ENABLESIZING, pszFilter, this );
	
	if ( dlg.DoModal() != IDOK ) return;
	SetCurrentDirectory( xDirPath );
	
	CString str;
	str.Format( "您确定要把近24小时申请开通的号码导出到\"%s\"吗？", dlg.GetPathName() );
	if (AfxMessageBox(str, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDCANCEL)
	{
		return;
	}
	
	CExportNumber* dlgExecute = new CExportNumber( dlg.GetPathName() );
	dlgExecute->Start();
}

/////////////////////////////////////////////////////////////////////////////
// CTelecomWnd custom message handlers

void CTelecomWnd::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	
	if ( pDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
	{
		DWORD dwExisting	= m_wndList.GetItemState( pDraw->nmcd.dwItemSpec, 0xFFFF );
		BOOL bSelected		= ( dwExisting & LVIS_SELECTED ) ? TRUE : FALSE;
		
		if ( bSelected == TRUE )
		{
			pDraw->clrText   = GetSysColor(COLOR_HIGHLIGHTTEXT);
		}
		else if ( (dwExisting & DS_ERROR) == DS_ERROR )
		{
			pDraw->clrText = RGB( 169, 0, 0 );
		}
		else
		if ( (dwExisting & DS_NORMAL) == DS_NORMAL )
		{
			pDraw->clrText = GetSysColor( COLOR_WINDOWTEXT );
		}

		*pResult = CDRF_DODEFAULT;
	}
	else
	{
		*pResult = 0;
	}
}

//////////////////////////////////////////////////////////////////////
// CTelecomWnd thread run

void CTelecomWnd::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CTelecomWnd::StopThread()
{
	if ( m_hThread == NULL ) return;
	
	m_bThread = FALSE;
	m_pWakeup.SetEvent();
	
	for ( int nAttempt = 50 ; nAttempt > 0 ; nAttempt-- )
	{
		DWORD nCode;
		if ( ! GetExitCodeThread( m_hThread, &nCode ) ) break;
		if ( nCode != STILL_ACTIVE ) break;
		Sleep( 100 );

		MSG msg;
		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	
	if ( nAttempt == 0 )
	{
		TerminateThread( m_hThread, 0 );
		_RPTF0(_CRT_WARN,"WARNING: Terminating CTelecomWnd thread.\n");
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CTelecomWnd::ThreadStart(LPVOID pParam)
{
	CTelecomWnd* pClass = (CTelecomWnd*)pParam;
	pClass->OnRun();
	return 0;
}

BOOL CTelecomWnd::OnRun()
{
	CSingleLock pLock( &m_pSection );
	
	while ( m_bThread )
	{
		pLock.Lock();
		OnRunPager();
		pLock.Unlock();
		if ( ! m_bThread ) break;
		
		WaitForSingleObject( m_pWakeup, 6 * 1000 );
	}
	
	return TRUE;
}

BOOL CTelecomWnd::OnRunPager()
{
	COleDateTime tNow = COleDateTime::GetCurrentTime();

	COleDateTime tPagStart;
	tPagStart.ParseDateTime( Settings.P2job.PagStart, 0, VAR_TIMEVALUEONLY );
	if ( tPagStart.GetStatus() != COleDateTime::valid ) return FALSE;
	tPagStart.SetDateTime( tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(),
					tPagStart.GetHour(), tPagStart.GetMinute(), 0 );
	
	COleDateTime tPagClose;
	tPagClose.ParseDateTime( Settings.P2job.PagClose, 0, VAR_TIMEVALUEONLY );
	if ( tPagClose.GetStatus() != COleDateTime::valid ) return FALSE;
	tPagClose.SetDateTime( tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(),
					tPagClose.GetHour(), tPagClose.GetMinute(), 0 );
	
	if ( tNow < tPagStart || tNow > tPagClose ) return FALSE;
	
	try
	{
		DWORD nExpired = 7 * 24;
		return OnRunPager( nExpired );
	}
	catch(_com_error e) 
	{ 
		theApp.Message( MSG_ERROR, "error on CTelecomWnd::OnRunPager: %s", e.ErrorMessage() );
		return FALSE;
	}
}

BOOL CTelecomWnd::OnRunPager(DWORD nExpired)
{
	if ( NULL == m_pRecordset || ! m_pRecordset->State )
	{
		CString strQuery;
		
		strQuery.Format( "select * from xcorppager"
				" where (xusrstate is null or (xusrstate='UNDELIV'"
				" and datediff(Minute, xsendtime, getdate())>=%i"
				" and datediff(Minute, xRecDate, xsendtime)<%i))"
				" and (datediff(Minute, xRecDate, getdate()) & 0x7fffffff)<=%i",
				Settings.Pager.PageInterval / 60,
				Settings.Pager.PageInterval / 60 * Settings.Pager.PageRetry,
				Settings.Pager.PageExpire / 60 );
#ifdef _DEBUG
		afxDump << (LPCTSTR)strQuery << "\n";
#endif
		xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	}
	else if ( m_pRecordset->adoEOF )
	{
		m_pRecordset->Requery( adCmdUnspecified );
	}
	
	if ( m_pRecordset->adoEOF ) return TRUE;
	CSingleLock pLock( &Network.m_pSection );
	if ( ! pLock.Lock( 250 ) ) return FALSE;
	
	CChannel* pChannel = Network.LookOut( 2 );
	if ( pChannel == NULL ) return FALSE;
	
	IDialPacket* pPacket = new IDialPacket;
	pPacket->SetLocalId( "95950000" );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xphonenum" );
	if ( VT_NULL == TheValue.vt ) pPacket->SetRemoteId( "" );
	else pPacket->SetRemoteId( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xcorpid" );
	if ( VT_NULL == TheValue.vt ) pPacket->SetFeeNumber( "" );
	else pPacket->SetFeeNumber( (LPCTSTR)(_bstr_t)TheValue );
	
	CHAR xCallerId[MAX_PHONE + 1]; ZeroMemory(xCallerId, MAX_PHONE + 1);
	pPacket->GetFeeNumber(xCallerId); pPacket->SetCallerId(xCallerId);
	
	pChannel->OnDialPacket( pPacket ); pPacket->Release();
	
	TheValue = m_pRecordset->GetCollect( "xjsjbh" );
	pChannel->m_nTaskId = TheValue.lVal;
	
	m_pRecordset->MoveNext();
	if ( ! m_pRecordset->adoEOF ) m_pWakeup.SetEvent();
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CImportNumber construction

CImportNumber::CImportNumber(LPCTSTR xUsrFile)
{
	m_xUsrFile = xUsrFile;
	CStatic* pWndTitle = (CStatic*)GetDlgItem( IDC_EXECUTE_TITLE );
	pWndTitle->SetWindowText( _T("导入用户号码") );
}

/////////////////////////////////////////////////////////////////////////////
// CImportNumber operations

BOOL CImportNumber::OnRun()
{
	CHAR szBuff[80];
	CString strFormat;
	
	strFormat.Format( "insert into xcorppager(xphonenum,xrecdate,xcorpid)"
					" values('%%s',getdate(),'057495950000')" );
	
	FILE* fp = fopen( m_xUsrFile, "r" );
	while (fp && fgets(szBuff,80,fp) && (!feof(fp)))
	{
		int nLength = _tcslen( szBuff );
		if ( szBuff[nLength - 1] == '\n' ) szBuff[--nLength] = '\0';
		if ( nLength > 21 ) szBuff[nLength=21] = '\0';
		
		if ( nLength && ! Execute(szBuff, strFormat) ) break;
	}
	
	if (fp) fclose( fp );
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CExportNumber construction

CExportNumber::CExportNumber(LPCTSTR xUsrFile)
{
	m_xUsrFile = xUsrFile;
	CStatic* pWndTitle = (CStatic*)GetDlgItem( IDC_EXECUTE_TITLE );
	pWndTitle->SetWindowText( _T("导出用户号码") );
}

/////////////////////////////////////////////////////////////////////////////
// CExportNumber operations

BOOL CExportNumber::OnRun()
{
	CString strQuery;
	
	strQuery.Format( "select * from xcorppager"
					" where datediff(Hour, xsendtime, getdate()) <= 24"
					" and xusrback = '1' order by xsendtime" );
	TRACE( strQuery + "\n" );
	
	_RecordsetPtr xRecordPtr = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	if ( xRecordPtr->adoEOF ) return TRUE;
	
	CFile pFile;
	if ( ! pFile.Open( m_xUsrFile, CFile::modeWrite | CFile::modeCreate ) ) return FALSE;
	
	for ( ; ! xRecordPtr->adoEOF; xRecordPtr->MoveNext() )
	{
		_variant_t TheValue = xRecordPtr->GetCollect( "xphonenum" );
		
		if ( VT_NULL != TheValue.vt )
		{
			pFile.Write( (LPTSTR)(_bstr_t)TheValue, _tcslen((LPTSTR)(_bstr_t)TheValue) );
		}
		
		pFile.Write( _T("\r\n"), sizeof(TCHAR) * 2 );
	}
	
	pFile.Close();
	return TRUE;
}
