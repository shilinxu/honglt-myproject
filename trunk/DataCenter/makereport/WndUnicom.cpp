//
// WndUnicom.cpp
//

#include "stdafx.h"
#include "MakeReport.h"
#include "WndUnicom.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "Skin.h"
#include "XML.h"

#include "Connection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CUnicomWnd, CChildWnd, 0)

BEGIN_MESSAGE_MAP(CUnicomWnd, CChildWnd)
	//{{AFX_MSG_MAP(CUnicomWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_TAB_CONNECT, OnTabConnect)
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28
#define SPLIT_SIZE		6

#define MAX_MESSAGE_COUNT	1024

/////////////////////////////////////////////////////////////////////////////
// CUnicomWnd construction

CUnicomWnd::CUnicomWnd() : CChildWnd(FALSE)
{
	m_nText		= 102;
	m_pXML		= NULL;

	CString strCaption;
	strCaption.LoadString( IDR_UNICOMFRAME );
	
	Create( IDI_TASKPANEL, (LPCTSTR)strCaption );
	
	Setup();
}

CUnicomWnd::~CUnicomWnd()
{
	Release();
}

/////////////////////////////////////////////////////////////////////////////
// CUnicomWnd operations

BOOL CUnicomWnd::Create(UINT nID, LPCTSTR xCaption) 
{
	m_nResID = nID;
	
	return CMDIChildWnd::Create( NULL, xCaption, WS_CHILD |
		 WS_OVERLAPPEDWINDOW | WS_VISIBLE );
}

BOOL CUnicomWnd::IsTestPhone(LPCTSTR xPhone) const
{
	if ( ! Settings.Unicom.Test ) return FALSE;
	
	CString strValue;
	
	strValue = Settings.Unicom.TestPhone;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( str.GetLength() > 0 && str == xPhone ) return TRUE;
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CUnicomWnd setup

BOOL CUnicomWnd::Setup()
{
	Release();
	
	CSingleLock pLock( &m_pSection, TRUE );
	
	CString strXML;
	
	strXML = Settings.General.Path;
	int nLength = strXML.GetLength();

	if ( nLength > 0 && strXML.GetAt(nLength-1) != '\\' ) strXML += '\\';
	strXML += _T("xProfile.xml");
	
	if ( CXMLElement* pXML = CXMLElement::FromFile( strXML, TRUE ) )
	{
		CXMLElement* pXMLMap;
		
		if ( pXMLMap = pXML->GetElementByName( _T("Unicom") ) )
		{
			m_pXML = pXMLMap->Clone();
		}
		
		delete pXML; return TRUE;
	}

	return FALSE;
}

void CUnicomWnd::Release()
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_pXML != NULL )
	{
		delete m_pXML; m_pXML = NULL;
	}
}

CXMLElement* CUnicomWnd::FindProfile(CXMLElement* pXML, LPCTSTR xID)
{
	for ( POSITION pos = pXML->GetElementIterator(); pos; )
	{
		CXMLElement* pMap = pXML->GetNextElement( pos );
		if ( ! pMap->IsNamed( _T("Map") ) ) continue;
		
		CString strValue;
		strValue = pMap->GetAttributeValue( _T("ID") );
		if ( ! strValue.CompareNoCase( xID ) ) return pMap;
	}
	
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CUnicomWnd message handlers
	
int CUnicomWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel
	
	m_wndPanel.Create( this );
	
	// ListCtrl
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOLABELWRAP|WS_VSCROLL|WS_TABSTOP,
		rectDefault, this, IDC_UNICOM_LIST );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	m_wndList.InsertColumn( 0, _T("计费号码"), LVCFMT_LEFT, 94, -1 );
	m_wndList.InsertColumn( 1, _T("对端号码"), LVCFMT_LEFT, 94, 0 );
	m_wndList.InsertColumn( 2, _T("通话日期"), LVCFMT_LEFT, 100, 1 );
	m_wndList.InsertColumn( 3, _T("通话时间"), LVCFMT_LEFT, 100, 2 );
	m_wndList.InsertColumn( 4, _T("通话时长"), LVCFMT_RIGHT, 80, 3 );
	m_wndList.InsertColumn( 5, _T("通话费用"), LVCFMT_RIGHT, 80, 4 );
	m_wndList.InsertColumn( 6, _T("呼叫类型"), LVCFMT_LEFT, 64, 5 );

	// CtrlText

	m_wndText.Create( WS_VISIBLE, rectDefault, this, IDC_UNICOM_TEXT );
	
	// Toolbar
	
	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	m_wndToolBar.SetOwner( this );
	
	Skin.CreateToolBar( _T("CUnicomWnd"), &m_wndToolBar );
	
	m_hThread	= NULL;
	m_bThread	= FALSE;
	
	if ( Settings.General.AutoUnicom )
	m_wndPanel.ExecuteStart();

	return 0;
}

void CUnicomWnd::OnDestroy() 
{
	CChildWnd::OnDestroy();
	
	m_wndPanel.m_wndTask.SendMessage( WM_COMMAND, IDC_UNICOM_CLEAR );
}

void CUnicomWnd::OnSize(UINT nType, int cx, int cy) 
{
	if ( nType != SIZE_INTERNAL ) CChildWnd::OnSize( nType, cx, cy );

	CRect rc;
	GetClientRect( &rc );

	HDWP hDWP = BeginDeferWindowPos( 4 );
	
	DeferWindowPos( hDWP, m_wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	rc.left += PANEL_WIDTH;
	
	DeferWindowPos( hDWP, m_wndList.GetSafeHwnd(), NULL,
		rc.left, rc.top, rc.Width(), rc.Height() - BAR_HEIGHT - m_nText - SPLIT_SIZE, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndText.GetSafeHwnd(), NULL,
		rc.left, rc.bottom - BAR_HEIGHT - m_nText, rc.Width(), m_nText, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndToolBar.GetSafeHwnd(), NULL,
		rc.left, rc.bottom - BAR_HEIGHT, rc.Width(), BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CUnicomWnd::OnPaint() 
{
	CPaintDC dc( this );
	CRect rcClient, rc;

	GetClientRect( &rcClient );
	rcClient.left += PANEL_WIDTH;
	
	rc.SetRect(	rcClient.left,
				rcClient.bottom - BAR_HEIGHT - m_nText - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - BAR_HEIGHT - m_nText );
	dc.FillSolidRect( rc.left, rc.top, rc.Width(), 1, GetSysColor( COLOR_BTNFACE ) );
	dc.FillSolidRect( rc.left, rc.top + 1, rc.Width(), 1, GetSysColor( COLOR_3DHIGHLIGHT ) );
	dc.FillSolidRect( rc.left, rc.bottom - 1, rc.Width(), 1, GetSysColor( COLOR_3DSHADOW ) );
	dc.FillSolidRect( rc.left, rc.top + 2, rc.Width(), rc.Height() - 3,
		GetSysColor( COLOR_BTNFACE ) );
}

/////////////////////////////////////////////////////////////////////////////
// CUnicomWnd resizing behaviour

BOOL CUnicomWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CRect rcClient, rc;
	CPoint point;

	GetCursorPos( &point );
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	rcClient.left += PANEL_WIDTH;
	
	rc.SetRect(	rcClient.left,
				rcClient.bottom - BAR_HEIGHT - m_nText - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - BAR_HEIGHT - m_nText );
	
	if ( rc.PtInRect( point ) )
	{
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_SIZENS ) );
		return TRUE;
	}
	
	return CChildWnd::OnSetCursor( pWnd, nHitTest, message );
}

void CUnicomWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rcClient, rc;
	
	GetClientRect( &rcClient );
	rcClient.left += PANEL_WIDTH;

	rc.SetRect(	rcClient.left,
				rcClient.bottom - BAR_HEIGHT - m_nText - SPLIT_SIZE,
				rcClient.right,
				rcClient.bottom - BAR_HEIGHT - m_nText );
	
	if ( rc.PtInRect( point ) )
	{
		DoSizeText(); return;
	}
	
	CChildWnd::OnLButtonDown( nFlags, point );
}

BOOL CUnicomWnd::DoSizeText()
{
	CRect rcClient;
	CPoint point;
	
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	ClipCursor( &rcClient );
	SetCapture();

	GetClientRect( &rcClient );
	
	int nOffset = 0xFFFF;
	
	while ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 )
	{
		MSG msg;
		while ( ::PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) );

		if ( ! AfxGetThread()->PumpMessage() )
		{
			AfxPostQuitMessage( 0 );
			break;
		}

		GetCursorPos( &point );
		ScreenToClient( &point );

		int nSplit = rcClient.bottom - point.y;

		if ( nOffset == 0xFFFF ) nOffset = m_nText - nSplit;
		nSplit += nOffset;

		nSplit = max( nSplit, 0 );
		nSplit = min( nSplit, rcClient.bottom - SPLIT_SIZE );

		if ( nSplit < 8 )
			nSplit = 0;
		if ( nSplit > rcClient.bottom - SPLIT_SIZE - 8 )
			nSplit = rcClient.bottom - SPLIT_SIZE;

		if ( nSplit != m_nText )
		{
			m_nText = nSplit;
			OnSize( SIZE_INTERNAL, 0, 0 );
			Invalidate();
		}
	}
	
	ReleaseCapture();
	ClipCursor( NULL );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CUnicomWnd custom message

LONG CUnicomWnd::OnTabConnect(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == (DWORD)-1 )
	{
		StopThread(); return TRUE;
	}

	StartThread();
	m_pWakeup.SetEvent();
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUnicomWnd Text operations

void CUnicomWnd::Message(int nType, LPCTSTR pszFormat, ...)
{
	if ( nType == MSG_DEBUG && ! Settings.General.Debug ) return;
	
	CString strFormat;
	va_list pArgs;
	
	va_start( pArgs, pszFormat );
	_vsntprintf( strFormat.GetBuffer(1024), 1024, pszFormat, pArgs );
	strFormat.ReleaseBuffer();
	va_end( pArgs );
	
	CTime pNow = CTime::GetCurrentTime();
	CString strLine;
	strLine.Format( _T("[%.2i:%.2i:%.2i] %s"),
		pNow.GetHour(), pNow.GetMinute(), pNow.GetSecond(), strFormat );
	m_wndText.AddLine( nType, strLine );
	
	if ( Settings.General.DebugLog ) LogMessage( strFormat );
}

void CUnicomWnd::LogMessage(LPCTSTR pszLog, BOOL bShowTime)
{
	CFile pFile;
	
	if ( pFile.Open( _T("Xunicom.log"), CFile::modeReadWrite ) )
	{
		pFile.Seek( 0, CFile::end );
	}
	else
	{
		if ( ! pFile.Open( _T("Xunicom.log"), CFile::modeWrite|CFile::modeCreate ) ) return;
	}

	if ( bShowTime )
	{
		CTime pNow = CTime::GetCurrentTime();
		CString strLine;
		
		strLine.Format( _T("[%.2i:%.2i:%.2i] %s\r\n"),
			pNow.GetHour(), pNow.GetMinute(), pNow.GetSecond(), pszLog );
		
		pFile.Write( (LPCTSTR)strLine, sizeof(TCHAR) * strLine.GetLength() );
	}
	else
	{
		pFile.Write( pszLog, sizeof(TCHAR) * _tcslen(pszLog) );
		pFile.Write( _T("\r\n"), sizeof(TCHAR) * 2 );
	}
	
	pFile.Close();
}

//////////////////////////////////////////////////////////////////////
// CUnicomWnd thread run

void CUnicomWnd::StartThread()
{
	if ( m_hThread != NULL && m_bThread ) return;
	
	m_bThread = TRUE;
	CWinThread* pThread = AfxBeginThread( ThreadStart, this, THREAD_PRIORITY_NORMAL );
	m_hThread = pThread->m_hThread;
}

void CUnicomWnd::StopThread()
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
	}
	
	if ( nAttempt == 0 )
	{
		TerminateThread( m_hThread, 0 );
		Sleep( 100 );
	}
	
	m_hThread = NULL;
}

UINT CUnicomWnd::ThreadStart(LPVOID pParam)
{
	CUnicomWnd* pClass = (CUnicomWnd*)pParam;
	pClass->OnRun();
	return 0;
}

BOOL CUnicomWnd::OnRun()
{
	CSingleLock pLock( &m_pSection );

	Settings.Unicom.BookMark = time( NULL );
	
	while ( m_bThread )
	{
		WaitForSingleObject( m_pWakeup, INFINITE );

#if 1
		if ( ! m_bThread ) break;
		pLock.Lock();
		
// 		if ( HashCallCenter() )
// 		OnBuildReport();
		
		if ( ! IsIgnoreTime() )
		{
		Message( MSG_DEBUG, "尝试上传[XXTD370C??????????_G.AVL]文件..." );
		PutToRemote( Settings.Unicom.PathUsr, "XXTD370C??????????_G" );

		Settings.Unicom.BookMark = time( NULL );
		}

		pLock.Unlock();
#endif
		
		m_wndPanel.ExecuteFlowBack( Settings.Unicom.Interval );
	}

	return TRUE;
}

BOOL CUnicomWnd::IsIgnoreTime() const
{
	CTime tNow = CTime::GetCurrentTime();

	CString strValue = Settings.Unicom.IgnoreHour;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( _ttoi(str) == tNow.GetHour() ) return TRUE;
	}
	return FALSE;
}

BOOL CUnicomWnd::HashCallCenter()
{		
	CConnection* pConnection = new CConnection;
	
	DWORD nAddress = inet_addr( "192.168.0.2" );
	pConnection->ConnectTo( (IN_ADDR*)&nAddress, 9902 );

	pConnection->m_pOutput->Print( _T("HND 9999\r\n") );
	pConnection->OnWrite(); Sleep( 150 );
	pConnection->OnRun();
	
	if ( ! pConnection->IsConnected() ) return FALSE;
	
	for ( int nChan = 122; nChan <= 151; nChan++ )
	{
		CString str;
		str.Format( "HAL %i\r\n", nChan );
		
		pConnection->m_pOutput->Print( (LPCTSTR)str );
		pConnection->OnWrite(); Sleep( 50 );
		
		pConnection->OnRun();
		if ( ! m_bThread ) break;
	}
	
	pConnection->m_pOutput->Print( "BYE\r\n" );
	pConnection->OnWrite(); Sleep( 250 );
	
	delete pConnection;
	
	return TRUE;
}

BOOL CUnicomWnd::OnBuildReport()
{
	try
	{
		CString strQuery;
		strQuery.Format( "select a.*, b.xphone as UsrNumber"
					" from XuserDetail a inner join"
					" xuser b on a.xuserbindid = b.xuserbindid inner join"
					" Xfeetable c on a.xfeetableId = c.xjsjbh" );
		CString strFilter;
		
		strFilter.Format( "c.xtalktype = 'CATK'"
					//" AND left(c.xphone,5) between '95950' and '95951'"
//					" AND c.xphone IN('95950%%', '95951%%')"
//					" AND LEFT(b.xphone,3) BETWEEN '130' AND '132'"
// 屏蔽130-133select，以加快筛选速度
//					" AND LEFT(b.xphone,3) BETWEEN '130' AND '133'"
//#ifdef _DEBUG
//					" AND a.jsjbh >= 1968255 and b.xphone='13296869428'" );
//#else

#ifdef _DEBUG
					" AND a.xtimestart > '2007-5-1' and a.xtimestart < '2007-6-1'"
					//" and xfwdphone='95950'"
					//" and xtimelength >0"
					//" and left(b.xphone,3) between '130' and '133'"
					);
#else
					" AND a.jsjbh > %i", Settings.Unicom.BookMark );
#endif

//#endif
		strQuery += _T(" WHERE ") + strFilter;
		strQuery += _T(" ORDER BY a.jsjbh");
		
#ifdef _DEBUG
		TRACE( strQuery + "\n" );
#else
		Message( MSG_DEBUG, strQuery );
#endif
		_RecordsetPtr xRecordset;
		xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		if ( ! xRecordset->adoEOF )
		{
			CFile xAvlFile;
			GetNextAvlFile( xAvlFile, 'G' );
			BuildReport( xAvlFile, xRecordset );
			
			xAvlFile.Close();
			UpdateBookmark( xRecordset, 'G' );
		}
		
		xCloseRecordset( xRecordset );
	}
	catch(_com_error e) 
	{ 
		Message( MSG_ERROR, _T("CUnicomWnd::OnBuildReport()发生错误: %s"), e.ErrorMessage() );
		return FALSE;
	}
	
	return TRUE;
}

BOOL CUnicomWnd::GetNextAvlFile(CFile& xAvlFile, CHAR cAvlType) const
{
	WIN32_FIND_DATA pFind;
	CString strPath;
	
	CTime tNow = CTime::GetCurrentTime();
	
	if ( 'C' == cAvlType )//C网
	strPath.Format( _T("%s\\Data\\XXTZ370C????%02i%02i%02i_C.AVL"),
			(LPCTSTR)Settings.General.Path,
			tNow.GetYear() % 100, tNow.GetMonth(), tNow.GetDay() );
	else
	strPath.Format( _T("%s\\Data\\XXTD370C????%02i%02i%02i_G.AVL"),
			(LPCTSTR)Settings.General.Path,
			tNow.GetYear() % 100, tNow.GetMonth(), tNow.GetDay() );

	DWORD nSeqNumb = 0;
	HANDLE hSearch = FindFirstFile( strPath, &pFind );
	
	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) continue;
			
			LPSTR pAvlFile = pFind.cFileName + 8;
			for ( DWORD nSeqId = 0; pAvlFile < pFind.cFileName + 12; pAvlFile++ )
			{
				if ( ! _istdigit( *pAvlFile ) ) break;
				nSeqId = nSeqId * 10 + (*pAvlFile) - '0';
			}
			
			if ( nSeqNumb < nSeqId ) nSeqNumb = nSeqId;
		}
		while ( FindNextFile( hSearch, &pFind ) );
		
		FindClose( hSearch );
	}

#if 1
	if ( 'C' == cAvlType )//C网
	strPath.Format( _T("%s\\XXTZ370C????%02i%02i%02i_C.AVL"),
			(LPCTSTR)Settings.General.Path,
			tNow.GetYear() % 100, tNow.GetMonth(), tNow.GetDay() );
	else
	strPath.Format( _T("%s\\XXTD370C????%02i%02i%02i_G.AVL"),
			(LPCTSTR)Settings.General.Path,
			tNow.GetYear() % 100, tNow.GetMonth(), tNow.GetDay() );
	
	hSearch = FindFirstFile( strPath, &pFind );
	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) continue;
			
			LPSTR pAvlFile = pFind.cFileName + 8;
			for ( DWORD nSeqId = 0; pAvlFile < pFind.cFileName + 12; pAvlFile++ )
			{
				if ( ! _istdigit( *pAvlFile ) ) break;
				nSeqId = nSeqId * 10 + (*pAvlFile) - '0';
			}
			
			if ( nSeqNumb < nSeqId ) nSeqNumb = nSeqId;
		}
		while ( FindNextFile( hSearch, &pFind ) );
		
		FindClose( hSearch );
	}
#endif
	
	if ( 'C' == cAvlType )//C网
	strPath.Format( _T("XXTZ370C%04i%02i%02i%02i_C.AVL"), ++ nSeqNumb,
			tNow.GetYear() % 100, tNow.GetMonth(), tNow.GetDay() );
	else
	strPath.Format( _T("XXTD370C%04i%02i%02i%02i_G.AVL"), ++ nSeqNumb,
			tNow.GetYear() % 100, tNow.GetMonth(), tNow.GetDay() );
	
	VERIFY( xAvlFile.Open(strPath, CFile::modeCreate | CFile::modeWrite) );
	xAvlFile.Seek( GetHeadLength(), CFile::begin );
	
	return TRUE;
}

int CUnicomWnd::GetHeadLength() const
{
	return 2+3+4+4+2+8+8+6+8+6+6+15+105+2;
}

BOOL CUnicomWnd::UpdateBookmark(_RecordsetPtr xRecordset, CHAR cAvlType)
{
	xRecordset->MoveLast();
	
	_variant_t TheValue = xRecordset->GetCollect( "jsjbh" );
	if ( 'C' == cAvlType )//C网
	Settings.Unicom.BookMark = TheValue.lVal;
	else //G网
	Settings.Unicom.BookMark = TheValue.lVal;
	
	Settings.Save();
	
	return TRUE;
}

BOOL CUnicomWnd::PutToRemote(LPCTSTR xPath, LPCTSTR xMask)
{
	try
	{
		CInternetSession* pInetSession = new CInternetSession(NULL, 1, PRE_CONFIG_INTERNET_ACCESS );;
		
		//pInetSession->SetOption( INTERNET_OPTION_CONNECT_TIMEOUT, 15 * 1000 );
//		pInetSession->SetOption( INTERNET_OPTION_CONNECT_BACKOFF, 1000 );
//		pInetSession->SetOption( INTERNET_OPTION_CONNECT_RETRIES, 1 );
		
//		pInetSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 3000 );
//		pInetSession->SetOption(INTERNET_OPTION_SEND_TIMEOUT, 3000 );
		
		CFtpConnection* pFtpConnection = pInetSession->GetFtpConnection(
									Settings.Unicom.Host,
									Settings.Unicom.UserId,
									Settings.Unicom.Password,
									Settings.Unicom.Port, TRUE );
		
		pFtpConnection->SetCurrentDirectory( xPath );

		PressRecord( Settings.General.Path, pFtpConnection, Settings.Unicom.Interval);
#if 0		
		CString strPath;
		WIN32_FIND_DATA pFind;
		
// 		strPath.Format( _T("%s\\%s.AVL"), (LPCTSTR)Settings.General.Path, xMask );
		strPath.Format( _T("%s\\*.*"), (LPCTSTR)Settings.General.Path, xMask );
		HANDLE hSearch = FindFirstFile( strPath, &pFind );
		
		if ( hSearch != INVALID_HANDLE_VALUE )
		{
			do
			{
				if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) continue;
				
				pFtpConnection->PutFile( Settings.General.Path + "\\" + pFind.cFileName, pFind.cFileName );
//				PutToRemote( pFtpConnection, Settings.General.Path + "\\" + pFind.cFileName, pFind.cFileName );
				MoveFile( Settings.General.Path + "\\" + pFind.cFileName,
						Settings.General.Path + _T("\\Data\\") + pFind.cFileName );
				
				Message( MSG_DEFAULT, "成功上传[%s]文件，并移动到[Data]目录", pFind.cFileName );
			}
			while ( FindNextFile( hSearch, &pFind ) );
		
			FindClose( hSearch );
		}
#endif
		pFtpConnection->Close(); delete pFtpConnection;
		pInetSession->Close(); delete pInetSession;
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage( szErr, 1024 );
		
		Message( MSG_ERROR, "CInternetException: %s", szErr );
		return FALSE;
	}
	
	return TRUE;
}

BOOL CUnicomWnd::PressRecord(LPCTSTR xPath, CFtpConnection* pFtpConnection, int nExpire)
{
	WIN32_FIND_DATA pFind;
	CString strPath;
	HANDLE hSearch;
	
	strPath.Format( _T("%s\\*.*"), xPath/*, xUserId*/ );
	Message( MSG_TEMP, "PressRecord: %s", strPath );
	
	hSearch = FindFirstFile( strPath, &pFind );
	
	if ( hSearch != INVALID_HANDLE_VALUE )
	{
		do
		{
// 			MSG msg;
// 			if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
// 			{
// 				TranslateMessage( &msg ); DispatchMessage( &msg );
// 			}
			
			if ( pFind.cFileName[0] == '.' ) continue;
			
			if ( pFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				COleDateTime xFileDate = pFind.ftLastWriteTime;
// 				xFileDate.SetDate( xFileDate.GetYear(), xFileDate.GetMonth(), xFileDate.GetDay() );
				COleDateTimeSpan xSpan = COleDateTime::GetCurrentTime() - xFileDate;
				
				Message( MSG_DEBUG, "目录[%s] -> [%04i-%02i-%02i %02i:%02i:%02i]]",
					pFind.cFileName,
					xFileDate.GetYear(), xFileDate.GetMonth(), xFileDate.GetDay(),
					xFileDate.GetHour(), xFileDate.GetMinute(), xFileDate.GetSecond());
// 				Message( MSG_DEBUG, "test上传[%s]目录，并归类到[%f]", pFind.cFileName,xSpan.GetTotalSeconds() );
// 				if ( xSpan.GetTotalDays() < 1 || xSpan.GetTotalDays() > 2 ) continue;
// 				if ( xSpan.GetDays() != 1 ) continue;
// 				if ( xSpan.GetTotalSeconds() < nExpire ) continue;
// 				if ( xSpan.GetTotalSeconds() > nExpire * 2 ) continue;
				if ( xSpan.GetTotalSeconds() > time(NULL)-Settings.Unicom.BookMark + nExpire ) continue;

				pFtpConnection->CreateDirectory(pFind.cFileName);
				pFtpConnection->SetCurrentDirectory( pFind.cFileName );

				strPath.Format( _T("%s\\%s"),
					xPath ? xPath : _T(""), pFind.cFileName );
				
				PressRecord( strPath, pFtpConnection, nExpire );
			}
			else/* if (	_tcsistr( pFind.cFileName, _T(".xml") ) != NULL )*/
			{
// 				if ( xUserId && _tcsistr( pFind.cFileName, _T("-") ) + 1 !=
// 					_tcsistr( pFind.cFileName, xUserId ) ) continue;
// 				
// 				COleDateTime xFileDate = pFind.ftCreationTime;
				COleDateTime xFileDate = pFind.ftLastWriteTime;
// 				xFileDate.SetDate( xFileDate.GetYear(), xFileDate.GetMonth(), xFileDate.GetDay() );
				COleDateTimeSpan xSpan = COleDateTime::GetCurrentTime() - xFileDate;
				
				Message( MSG_DEBUG, "文件[%s] -> [%04i-%02i-%02i %02i:%02i:%02i]]",
					pFind.cFileName,
					xFileDate.GetYear(), xFileDate.GetMonth(), xFileDate.GetDay(),
					xFileDate.GetHour(), xFileDate.GetMinute(), xFileDate.GetSecond());
				Message( MSG_DEBUG, "test上传[%s]文件，[%f] [%u]", pFind.cFileName,
					xSpan.GetTotalSeconds(),time(NULL)-Settings.Unicom.BookMark+nExpire );
// 				Message( MSG_DEFAULT, "test上传[%s]文件，并归类到[%i]", pFind.cFileName,xSpan.GetDays() );
// 				if ( xSpan.GetTotalDays() < 1 || xSpan.GetTotalDays() > 2 ) continue;
// 				if ( xSpan.GetDays() != 1 ) continue;
				if ( xSpan.GetTotalSeconds() < nExpire ) continue;
// 				if ( xSpan.GetTotalSeconds() > nExpire * 2 ) continue;
				if ( xSpan.GetTotalSeconds() > time(NULL)-Settings.Unicom.BookMark + nExpire ) continue;
				
				strPath.Format( _T("%s\\%s"),
					xPath ? xPath : _T(""), pFind.cFileName );

				pFtpConnection->PutFile( strPath, pFind.cFileName );
				Message( MSG_DEFAULT, "成功上传[%s]文件，并归类到[Data]目录", pFind.cFileName );
// 				
// 				CString xNewFile = Settings.General.PathRec + _T("\\Backup");
// 				CreateDirectory( (LPCTSTR)xNewFile, NULL );
// 				xNewFile += '\\'; xNewFile += pFind.cFileName;
// 				
// 				MoveFileEx( strPath, xNewFile, MOVEFILE_REPLACE_EXISTING );
			}
		}
		while ( FindNextFile( hSearch, &pFind ) );
		
		FindClose( hSearch );
	}
	
// 	RemoveDirectory( xPath );
	return TRUE;
}

BOOL CUnicomWnd::BuildReport(CFile& xAvlFile, _RecordsetPtr xRecordset)
{
	DWORD nAllTotalFee = 0;
DWORD nInfFee95950 = 0;
DWORD nInfFee95951 = 0;
	
	for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
	{
		int nItem = -1;
		_variant_t TheValue;
		CXMLElement* pXMLFee = NULL;

#if 1
		// 流水号
		TheValue = xRecordset->GetCollect( "xseqnumber" );
		if ( VT_NULL != TheValue.vt && TheValue != xRecordset->GetCollect( "jsjbh" ) ) continue;
		TheValue = xRecordset->GetCollect( "jsjbh" );
		DWORD xSeqNumber = TheValue.lVal;
		// 并且符合95950或95951
		TheValue = xRecordset->GetCollect( "xFwdPhone" );
		if ( VT_NULL == TheValue.vt ) continue;
		//
		{
			CString strValue = (LPCTSTR)(_bstr_t)TheValue;
			if ( strValue.CompareNoCase( "95950" ) && strValue.CompareNoCase( "95951" ) ) continue;
		}
		
		int nMapLeng = 0, nMapFee = 0;
#endif
#if 1
		// 确认计费号码符合130-133,150-153
		TheValue = xRecordset->GetCollect( "UsrNumber" );
		if ( VT_NULL == TheValue.vt ) continue;
		{
			CString strValue = (LPCTSTR)(_bstr_t)TheValue;
			strValue = strValue.Left( 3 );
			
			if ( ! (strValue >= "130" && strValue <= "133") &&
				 ! (strValue >= "150" && strValue <= "153") ) continue;
		}
#endif
#if 0
		// 确认15秒内无操作则免费(通话时长)
	// 对端号码, 取业务资费
		TheValue = xRecordset->GetCollect( "xFwdPhone" );
		if ( VT_NULL == TheValue.vt || ! _tcscmp((LPCTSTR)(_bstr_t)TheValue,"95950") )
		{	
			pXMLFee = FindProfile( m_pXML, _T("95950") );
		}
		else
		{	
			pXMLFee = FindProfile( m_pXML, _T("95951") );
			CalcMapTalking( xRecordset, xSeqNumber, &nMapLeng, &nMapFee, pXMLFee );
		}
	{
		int nSpeakLeng = 0;
		TheValue = xRecordset->GetCollect( "xTimeLength" );
		if ( VT_NULL != TheValue.vt ) nSpeakLeng = TheValue.lVal;
		
		// 18秒,避免时间误差
		if ( nSpeakLeng + nMapLeng <= 18 ) continue;
	}
	
	nMapLeng = nMapFee = 0;
#endif

////////////////////////////////		
		// 呼叫类型
		WriteAvlFile( &xAvlFile, "00", 2 );
		nItem = PrintLineText( "00", 6, nItem );
		
		// 计费号码
		TheValue = xRecordset->GetCollect( "UsrNumber" );
		if ( VT_NULL == TheValue.vt )
		{
			WriteAvlFile( &xAvlFile, "", 11 );
			nItem = PrintLineText( "", 0, nItem );
		}
		else
		if ( IsTestPhone( (LPCTSTR)(_bstr_t)TheValue ) )
		{
			continue;
		}
		else
		{
			WriteAvlFile( &xAvlFile, (LPCTSTR)(_bstr_t)TheValue, 11 );
			nItem = PrintLineText( (LPCTSTR)(_bstr_t)TheValue, 0, nItem );
		}
		
		//(...)	//MIN/IMSI号
		WriteAvlFile( &xAvlFile, "", 15 );
		//(...)	//电子序列号(ESN)
		WriteAvlFile( &xAvlFile, "", 11 );
		
		// 对端号码
BOOL bIs95950 = FALSE;
		TheValue = xRecordset->GetCollect( "xFwdPhone" );
		if ( VT_NULL == TheValue.vt || ! _tcscmp((LPCTSTR)(_bstr_t)TheValue,"95950") )
		{	
			pXMLFee = FindProfile( m_pXML, _T("95950") );
			WriteAvlFile( &xAvlFile, "95950", 21 );
			nItem = PrintLineText( "95950", 1, nItem );
		bIs95950 = TRUE;
		}
		else
		{	
			pXMLFee = FindProfile( m_pXML, _T("95951") );
			WriteAvlFile( &xAvlFile, "95951", 21 );
			nItem = PrintLineText( "95951", 1, nItem );
		}
#if 1		
		CalcMapTalking( xRecordset, xSeqNumber, &nMapLeng, &nMapFee, pXMLFee );
#endif
		
		//(...)	//临时本地目录号
		WriteAvlFile( &xAvlFile, "", 15 );
		//(...)	//第三方号码
		WriteAvlFile( &xAvlFile, "", 21 );
		
		// 通话起始日期
		TheValue = xRecordset->GetCollect( "xTimeStart" );
		if ( VT_NULL != TheValue.vt )
		{
			CString strValue;
			COleDateTime tt(TheValue);
			strValue.Format( "%04d%02d%02d", tt.GetYear(), tt.GetMonth(), tt.GetDay() );
			WriteAvlFile( &xAvlFile, strValue, 8 );
			nItem = PrintLineText( strValue, 2, nItem );
		}
		else
		{
			WriteAvlFile( &xAvlFile, "", 8 );
		}
		
		// 通话起始时间
		TheValue = xRecordset->GetCollect( "xTimeStart" );
		if ( VT_NULL != TheValue.vt )
		{
			CString strValue;
			COleDateTime tt(TheValue);
			strValue.Format( "%02d%02d%02d", tt.GetHour(), tt.GetMinute(), tt.GetSecond() );
			WriteAvlFile( &xAvlFile, strValue, 6 );
			nItem = PrintLineText( strValue, 3, nItem );
		}
		else
		{
			WriteAvlFile( &xAvlFile, "", 6 );
		}
		
		// 通话时长
		int nSpeakLeng = 0;
		TheValue = xRecordset->GetCollect( "xTimeLength" );
		if ( VT_NULL != TheValue.vt )
		{
			nSpeakLeng = TheValue.lVal;
		}
#if 1
		nSpeakLeng = GetNewLength(nSpeakLeng);
#endif
#if 0	// 配合信产部前15秒免费
		nSpeakLeng = max( 0, nSpeakLeng - 18 );
#endif
		nItem = PrintLineText( nSpeakLeng + nMapLeng, 4, nItem );
		CString strValue;
		strValue.Format( "%06i", nSpeakLeng + nMapLeng );
		WriteAvlFile( &xAvlFile, strValue, 6 );
		
		//(...)	//交换机代码
		WriteAvlFile( &xAvlFile, "", 10 );
		//本方初始位置LAC号
		WriteAvlFile( &xAvlFile, "99999", 5 );
		//本方初始位置蜂窝（基站）号CELLID
		WriteAvlFile( &xAvlFile, "99999", 5 );
		//对方初始位置LAC号
		WriteAvlFile( &xAvlFile, "99999", 5 );
		//对方初始位置蜂窝（基站）号CELLID
		WriteAvlFile( &xAvlFile, "99999", 5 );
		//(...)	//增值业务
		WriteAvlFile( &xAvlFile, "", 2 );
		//(...)	//结束原因
		WriteAvlFile( &xAvlFile, "", 1 );
		//(...)	//残缺标志
		WriteAvlFile( &xAvlFile, "", 1 );
		
		DWORD nTotalFee = 0;
		
		//基本费用(单位厘)
		if ( pXMLFee != NULL )
		{
			CString strValue;
			strValue = pXMLFee->GetAttributeValue( _T("Talk") );
			int nTalk = _ttoi( strValue );
			
			int nBaseLeng = nSpeakLeng + nMapLeng;
			strValue.Format( "%08i", (nBaseLeng+60) / 60 * nTalk * 10 );
			nTotalFee += (nBaseLeng+60) / 60 * nTalk;
			WriteAvlFile( &xAvlFile, strValue, 8 );
		}				
		else//其他SEAT
		{
			WriteAvlFile( &xAvlFile, "00000000", 8 );
		}
		
		//(...)	//基本计费时长
		WriteAvlFile( &xAvlFile, "", 6 );
		//(...)	//多费率标识
		WriteAvlFile( &xAvlFile, " ", 1 );
		//(...)	//长途费用
		WriteAvlFile( &xAvlFile, "", 8 );
		//(...)	//长途计费时长
		WriteAvlFile( &xAvlFile, "", 6 );
		//(...)	//长途多费率标识
		WriteAvlFile( &xAvlFile, "", 1 );
		
		//信息费用(单位厘)
		if ( pXMLFee != NULL )
		{
			CString strValue;
			strValue = pXMLFee->GetAttributeValue( _T("News") );
			int nNews = _ttoi( strValue );
			
			nTotalFee += (nSpeakLeng+60) / 60 * nNews + nMapFee;
if ( bIs95950) nInfFee95950 += (nSpeakLeng+60) / 60 * nNews + nMapFee;
else nInfFee95951 += (nSpeakLeng+60) / 60 * nNews + nMapFee;
			
			strValue.Format( "%08i", (nSpeakLeng+60) / 60 * nNews * 10 + nMapFee*10 );
			WriteAvlFile( &xAvlFile, strValue, 8 );
		}
		else
		{
			WriteAvlFile( &xAvlFile, "00000000", 8 );
		}
		
		//其它费
		WriteAvlFile( &xAvlFile, "00000000", 8 );
		
		nItem = PrintLineText( double(nTotalFee) / 100, 5, nItem );
		nAllTotalFee += nTotalFee;
		
		//对方所在地
		WriteAvlFile( &xAvlFile, "574", 3 );
			
		//(...)	//对方所在省
		WriteAvlFile( &xAvlFile, "", 3 );
		//(...)	//对方所在国
		WriteAvlFile( &xAvlFile, "", 3 );
		//(...)	//长途类型
		WriteAvlFile( &xAvlFile, "", 1 );
		//(...)	//长途网类型
		WriteAvlFile( &xAvlFile, "", 1 );
		//(...)	//漫游类型
		WriteAvlFile( &xAvlFile, "0", 1 );
		//(...)	//通话地
		WriteAvlFile( &xAvlFile, "", 3 );
		//(...)	//附加业务活动码
		WriteAvlFile( &xAvlFile, "", 1 );
		//(...)	//附加业务码
		WriteAvlFile( &xAvlFile, "", 2 );
		//(...)	//换行
		WriteAvlFile( &xAvlFile, "\r\n", 2 );
	}
	
	xAvlFile.SeekToBegin();
	BuildHeader( xAvlFile, xRecordset, nAllTotalFee );
Message( MSG_ERROR, "debug: 95950信息费(单位厘)-> %i", nInfFee95950 * 10);
Message( MSG_ERROR, "debug: 95951信息费(单位厘)-> %i", nInfFee95951 * 10);
	
	return TRUE;
}

BOOL CUnicomWnd::BuildHeader(CFile& xAvlFile, _RecordsetPtr xRecordset, DWORD nTotalFee)
{
	CString xFileName = xAvlFile.GetFileName();
	
	// 本地话单
	WriteAvlFile( &xAvlFile, "01", 2 );
	// 地市代码
	WriteAvlFile( &xAvlFile, "370", 3 );
	// 文件标志
	WriteAvlFile( &xAvlFile, " XXT", 4 );
	// 文件序列号
	WriteAvlFile( &xAvlFile, xFileName.Mid(8, 4), 4 );
	// 文件版本号
	WriteAvlFile( &xAvlFile, "00", 2 );
	
	// 文件产生日期
	WriteAvlFile( &xAvlFile, "20" + xFileName.Mid(12, 6), 8 );
	
	// 首次通话日期
	_variant_t TheValue;
	xRecordset->MoveFirst();
	
	TheValue = xRecordset->GetCollect( "xTimeStart" );
	if ( VT_NULL != TheValue.vt )
	{
		CString strValue;
		COleDateTime tt(TheValue);
		strValue.Format( "%04d%02d%02d", tt.GetYear(), tt.GetMonth(), tt.GetDay() );
		WriteAvlFile( &xAvlFile, strValue, 8 );
	}
	else
	{
		WriteAvlFile( &xAvlFile, "", 8 );
	}
	
	// 首次通话时间
	TheValue = xRecordset->GetCollect( "xTimeStart" );
	if ( VT_NULL != TheValue.vt )
	{
		CString strValue;
		COleDateTime tt(TheValue);
		strValue.Format( "%02d%02d%02d", tt.GetHour(), tt.GetMinute(), tt.GetSecond() );
		WriteAvlFile( &xAvlFile, strValue, 6 );
	}
	else
	{
		WriteAvlFile( &xAvlFile, "", 6 );
	}
	
	// 末次通话日期
	xRecordset->MoveLast();
	TheValue = xRecordset->GetCollect( "xTimeStart" );
	if ( VT_NULL != TheValue.vt )
	{
		CString strValue;
		COleDateTime tt(TheValue);
		strValue.Format( "%04d%02d%02d", tt.GetYear(), tt.GetMonth(), tt.GetDay() );
		WriteAvlFile( &xAvlFile, strValue, 8 );
	}
	else
	{
		WriteAvlFile( &xAvlFile, "", 8 );
	}
	// 末次通话时间
	TheValue = xRecordset->GetCollect( "xTimeStart" );
	if ( VT_NULL != TheValue.vt )
	{
		CString strValue;
		COleDateTime tt(TheValue);
		strValue.Format( "%02d%02d%02d", tt.GetHour(), tt.GetMinute(), tt.GetSecond() );
		WriteAvlFile( &xAvlFile, strValue, 6 );
	}
	else
	{
		WriteAvlFile( &xAvlFile, "", 6 );
	}
	
	//通话总记录数
	CString strValue;
	strValue.Format( "%06i", xRecordset->GetRecordCount() );
	WriteAvlFile( &xAvlFile, strValue, 6 );
	
	//总话费(单位厘)
	strValue.Format( "%015i", nTotalFee * 10 );
	WriteAvlFile( &xAvlFile, strValue, 15 );
	
	//未用
	WriteAvlFile( &xAvlFile, "", 105 );
	
	//(...)	//换行
	WriteAvlFile( &xAvlFile, "\r\n", 2 );
Message( MSG_ERROR, "debug: 总话费(单位厘)-> %i", nTotalFee * 10);
	
	return TRUE;
}

void CUnicomWnd::WriteAvlFile(CFile* pFile, LPCTSTR xField, int nLength)
{
	int nFieldLen = _tcslen( xField );
	
	pFile->Write( xField, nFieldLen );

	for ( ; nFieldLen++ < nLength; )
	{
		pFile->Write( " ", 1 );
	}
}

int CUnicomWnd::PrintLineText(LPCTSTR xText, int nSubItem, int nAtLine)
{
	if ( nAtLine == -1 )
	{
		nAtLine = m_wndList.GetItemCount();
		if ( nAtLine >= 1024 ) m_wndList.DeleteAllItems();
		m_wndList.InsertItem( nAtLine %= 1024, NULL );
	}
	
	m_wndList.SetItemText( nAtLine, nSubItem, xText );
	return nAtLine;
}

int CUnicomWnd::PrintLineText(int xValue, int nSubItem, int nAtLine)
{
	CHAR xText[16];
	_itot( xValue, xText, 10 );
	
	return PrintLineText( xText, nSubItem, nAtLine );
}

int CUnicomWnd::PrintLineText(double xValue, int nSubItem, int nAtLine)
{
	CHAR xText[16];
	_stprintf( xText, "%.2lf", xValue );
	
	return PrintLineText( xText, nSubItem, nAtLine );
}

int CUnicomWnd::GetNewLength(int nLength) const
{
#ifdef _DEBUG
return nLength;
#endif
	int nNewLength = nLength;

#if 1
	if ( nLength < 35 ) nNewLength += 2;
	else
	if ( nLength < 55 ) nNewLength += 5;
	else
	if ( nLength < 100 ) nNewLength += 20;
	else
#ifdef _12_10_ //EDIT 12/10
	if ( nLength < 150 ) nNewLength += 65;
	else
	if ( nLength < 240 ) nNewLength += 105;
	else nNewLength += (nLength/60)*30;
#else
	nNewLength += 30;
#endif

#else
	nNewLength += Settings.Unicom.TakAdjust;
#endif

	return nNewLength;
}

#if 0
BOOL CUnicomWnd::CalcMapTalking(DWORD nSeqId, int* pnLeng, int* pnFee, CXMLElement* pXMLFee)
{
	CString strQuery;
	strQuery.Format( "select * from XuserDetail"
					" where jsjbh > %i and xseqnumber = %i", nSeqId, nSeqId );
	
#ifdef _DEBUG
	TRACE( strQuery + "\n" );
#else
	Message( MSG_DEBUG, strQuery );
#endif
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	
	for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
	{
		_variant_t xFwdPhone = xRecordset->GetCollect( "xFwdPhone" );
		if ( VT_NULL == xFwdPhone.vt ) continue;
		
		_variant_t TheValue = xRecordset->GetCollect( "xTimeLength" );
		if ( VT_NULL == TheValue.vt ) continue;
		*pnLeng += TheValue.lVal;
		
		BOOL bFound = FALSE;
		CString strValue = pXMLFee->GetAttributeValue( _T("Key") );
		for ( strValue += '|' ; strValue.GetLength() ; )
		{
			CString str	= strValue.SpanExcluding( _T("|;,") );
			strValue	= strValue.Mid( str.GetLength() + 1 );
			
			CString sc	= str.SpanExcluding( _T(":") );
			str			= str.Mid( sc.GetLength() + 1 );
			
			if ( ! sc.CompareNoCase( (LPCTSTR)(_bstr_t)xFwdPhone ) )
			{
				*pnFee += (TheValue.lVal+60) / 60 * _ttoi( str );
				bFound = TRUE; break;
			}
		}
		
		if ( bFound == FALSE )
		{
			strValue = pXMLFee->GetAttributeValue( "News" );
			*pnFee += (TheValue.lVal+60) / 60 * _ttoi( strValue );
		}
	}
	
	return TRUE;
}
#else
BOOL CUnicomWnd::CalcMapTalking(_RecordsetPtr xRecordset, DWORD nSeqId, int* pnLeng, int* pnFee, CXMLElement* pXMLFee)
{
	_variant_t pBookmark = xRecordset->Bookmark;
	
	CString strQuery;
	
	strQuery.Format( "xseqnumber = %i", nSeqId );
	TRACE( strQuery + "\n" );
	
	do
	{
		xRecordset->Find( (_bstr_t)strQuery, 1, adSearchForward, xRecordset->Bookmark );
		if ( xRecordset->adoEOF ) break;
		
		_variant_t xFwdPhone = xRecordset->GetCollect( "xFwdPhone" );
		if ( VT_NULL == xFwdPhone.vt ) continue;
		
		_variant_t TheValue = xRecordset->GetCollect( "xTimeLength" );
		if ( VT_NULL == TheValue.vt ) continue;
		*pnLeng += TheValue.lVal;
		
		BOOL bFound = FALSE;
		CString strValue = pXMLFee->GetAttributeValue( _T("Key") );
		for ( strValue += '|' ; strValue.GetLength() ; )
		{
			CString str	= strValue.SpanExcluding( _T("|;,") );
			strValue	= strValue.Mid( str.GetLength() + 1 );
			
			str	+= ':';
			CString sc	= str.SpanExcluding( _T(":") );
			str			= str.Mid( sc.GetLength() + 1 );
			
			if ( ! sc.CompareNoCase( (LPCTSTR)(_bstr_t)xFwdPhone ) )
			{
				*pnFee += (TheValue.lVal+60) / 60 * _ttoi( str );
				bFound = TRUE; break;
			}
		}
		
		if ( bFound == FALSE )
		{
			strValue = pXMLFee->GetAttributeValue( "News" );
			*pnFee += (TheValue.lVal+60) / 60 * _ttoi( strValue );
		}
	}
	while ( ! xRecordset->adoEOF );
	
	xRecordset->Bookmark = pBookmark;
	return TRUE;
}
#endif

#define BUFFER_SIZE				(2*1024*1024)
BOOL CUnicomWnd::PutToRemote(CFtpConnection* pFtpConnection, LPCTSTR xLocalFile, LPCTSTR xRemoteFile)
{
	HANDLE hSource = CreateFile( xLocalFile, GENERIC_READ,
				FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hSource == INVALID_HANDLE_VALUE ) return FALSE;
	
	DWORD nSizeHigh	= 0;
	DWORD nSizeLow	= GetFileSize( hSource, &nSizeHigh );
	QWORD nSize		= (QWORD)nSizeLow + ( (QWORD)nSizeHigh << 32 );
	
	BYTE* pBuffer = new BYTE[ BUFFER_SIZE ];
	CInternetFile* pInternetFile = pFtpConnection->OpenFile( "xRemoteFile", GENERIC_WRITE );
	
	try
	{
	for ( QWORD nLength = nSize; nLength > 0; )
	{
		DWORD nBuffer	= (DWORD)min( nLength, QWORD(BUFFER_SIZE) );
		DWORD nSuccess	= 0;
		
		ReadFile( hSource, pBuffer, nBuffer, &nBuffer, NULL );
		pInternetFile->Write( pBuffer, nBuffer );
		
		nLength -= nBuffer;
	}
	
	pInternetFile->Close(); delete pInternetFile;
	CloseHandle( hSource );
	
	delete [] pBuffer;
	return TRUE;
	
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage( szErr, 1024 );
		
		Message( MSG_ERROR, "CInternetException: %s", szErr );
	}
	
	pInternetFile->Close(); delete pInternetFile;
	CloseHandle( hSource );
	
	delete [] pBuffer;
	return FALSE;
}
