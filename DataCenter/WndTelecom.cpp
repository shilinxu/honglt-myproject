//
// WndTelecom.cpp
//

#include "stdafx.h"
#include "MakeReport.h"
#include "WndTelecom.h"
#include "CoolInterface.h"
#include "Settings.h"
#include "Skin.h"
#include "XML.h"

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
// CTelecomWnd construction

CTelecomWnd::CTelecomWnd() : CChildWnd(FALSE)
{
	m_nText		= 102;
	m_pXML		= NULL;

	CString strCaption;
	strCaption.LoadString( IDR_TELECOMFRAME );
	
	Create( IDI_TASKPANEL, (LPCTSTR)strCaption );
	
	Setup();
}

CTelecomWnd::~CTelecomWnd()
{
	Release();
}

/////////////////////////////////////////////////////////////////////////////
// CTelecomWnd operations

BOOL CTelecomWnd::Create(UINT nID, LPCTSTR xCaption) 
{
	m_nResID = nID;
	
	return CMDIChildWnd::Create( NULL, xCaption, WS_CHILD |
		 WS_OVERLAPPEDWINDOW | WS_VISIBLE );
}

BOOL CTelecomWnd::IsTestPhone(LPCTSTR xPhone) const
{
	if ( ! Settings.Telecom.Test ) return FALSE;
	
	CString strValue;
	
	strValue = Settings.Telecom.TestPhone;
	for ( strValue += '|' ; strValue.GetLength() ; )
	{
		CString str	= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( str.GetLength() + 1 );
		
		if ( str.GetLength() > 0 && str == xPhone ) return TRUE;
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CTelecomWnd setup

BOOL CTelecomWnd::Setup()
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
		
		if ( pXMLMap = pXML->GetElementByName( _T("Telecom") ) )
		{
			m_pXML = pXMLMap->Clone();
		}
		
		delete pXML; return TRUE;
	}

	return FALSE;
}

void CTelecomWnd::Release()
{
	CSingleLock pLock( &m_pSection, TRUE );
	
	if ( m_pXML != NULL )
	{
		delete m_pXML; m_pXML = NULL;
	}
}

CXMLElement* CTelecomWnd::FindProfile(CXMLElement* pXML, LPCTSTR xID)
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
// CTelecomWnd message handlers
	
int CTelecomWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetOwner( GetParent() );
	
	// Panel
	
	m_wndPanel.Create( this );
	
	// ListCtrl
	
	m_wndList.Create( WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOLABELWRAP|WS_VSCROLL|WS_TABSTOP,
		rectDefault, this, IDC_TELECOM_LIST );
	
	m_wndList.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP );
	m_wndList.SetImageList( &CoolInterface.m_pImages, LVSIL_SMALL );
	
	m_wndList.InsertColumn( 0, _T("计费号码"), LVCFMT_LEFT, 94, -1 );
	m_wndList.InsertColumn( 1, _T("对端号码"), LVCFMT_LEFT, 94, 0 );
	m_wndList.InsertColumn( 2, _T("通话日期"), LVCFMT_LEFT, 100, 1 );
	m_wndList.InsertColumn( 3, _T("通话时间"), LVCFMT_LEFT, 100, 2 );
	m_wndList.InsertColumn( 4, _T("通话时长"), LVCFMT_RIGHT, 80, 3 );
	m_wndList.InsertColumn( 5, _T("通话费用"), LVCFMT_RIGHT, 80, 4 );

	// CtrlText

	m_wndText.Create( WS_VISIBLE, rectDefault, this, IDC_TELECOM_TEXT );
	
	// Toolbar
	
	if ( ! m_wndToolBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	m_wndToolBar.SetOwner( this );
	
	Skin.CreateToolBar( _T("CTelecomWnd"), &m_wndToolBar );
	
	m_hThread	= NULL;
	m_bThread	= FALSE;
	
	if ( Settings.General.AutoTelecom )
	m_wndPanel.ExecuteStart();

	return 0;
}

void CTelecomWnd::OnDestroy() 
{
	//m_wndPanel.m_wndTask.SendMessage( WM_COMMAND, IDC_TELECOM_CLEAR );
	StopThread();
	
	CChildWnd::OnDestroy();
}

void CTelecomWnd::OnSize(UINT nType, int cx, int cy) 
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

void CTelecomWnd::OnPaint() 
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
// CTelecomWnd resizing behaviour

BOOL CTelecomWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
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

void CTelecomWnd::OnLButtonDown(UINT nFlags, CPoint point) 
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

BOOL CTelecomWnd::DoSizeText()
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
// CTelecomWnd custom message

LONG CTelecomWnd::OnTabConnect(WPARAM wParam, LPARAM lParam)
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
// CTelecomWnd Text operations

void CTelecomWnd::Message(int nType, LPCTSTR pszFormat, ...)
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

void CTelecomWnd::LogMessage(LPCTSTR pszLog, BOOL bShowTime)
{
	CFile pFile;
	
	if ( pFile.Open( _T("XTelecom.log"), CFile::modeReadWrite ) )
	{
		pFile.Seek( 0, CFile::end );
	}
	else
	{
		if ( ! pFile.Open( _T("XTelecom.log"), CFile::modeWrite|CFile::modeCreate ) ) return;
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
	}
	
	if ( nAttempt == 0 )
	{
		TerminateThread( m_hThread, 0 );
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
	CSingleLock pLock( &m_pSection, TRUE );
	
	CString xFile;
	if ( OnBuildReport( &xFile ) )
	{
		Message( MSG_DEBUG, "尝试上传[%s]文件...", xFile );
		PutToRemote( Settings.Telecom.PathUsr, xFile );
	}
	
	m_wndPanel.PostMessage( WM_NEXTMONTH );
	m_bThread = FALSE;

	return TRUE;
}

BOOL CTelecomWnd::OnBuildReport(CString* pxFile)
{
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	
	CString xFile;
	if ( tNow.GetMonth() == 1 )
	xFile.Format( "nbzb%.4i%.2i.dbf", tNow.GetYear() - 1, 12 );
	else
	xFile.Format( "nbzb%.4i%.2i.dbf", tNow.GetYear(), tNow.GetMonth() - 1 );
	
	try
	{
		CDBFTableDef xTable;
		CreateFile( (LPCTSTR)xFile, xTable );
		
		CDBFRecordset xRecorddbf;
		xRecorddbf.Open( xFile );
		if ( pxFile ) *pxFile = xRecorddbf.GetDBFilePath();
		
		CString strQuery;
		strQuery.Format( "select a.*, substring(b.xphone,5,32) as UsrNumber"
					" from XuserDetail a inner join"
					" xuser b on a.xuserbindid = b.xuserbindid inner join"
					" Xfeetable c on a.xfeetableId = c.xjsjbh" );
		
		CString strFilter;
		strFilter.Format( "c.xtalktype = 'CATK'"
					//" AND left(c.xphone,5) between '95950' and '95951'"
//					" AND c.xphone IN('95950%%', '95951%%')"
					" AND DATEDIFF(month, xTimeStart, getdate()) = 1"
// 屏蔽05746,8,201select，以加快筛选速度
//					" AND (LEFT(b.xphone,5) BETWEEN '05746' AND '05748' OR LEFT(b.xphone,7)='0574201')"
//					" AND b.xphone IN('05746%%', '05748%%', '0574201%%')"
#ifdef _DEBUG
					" AND a.jsjbh >= 1968255 and b.xphone='057481123926'" );
#else
					" AND a.jsjbh > %i", Settings.Telecom.BookMark );
#endif
		strQuery += _T(" WHERE ") + strFilter;
		strQuery += _T(" ORDER BY a.jsjbh");
		
#ifdef _DEBUG
		TRACE( strQuery + "\n" );
#else
		Message( MSG_DEBUG, strQuery );
#endif
		_RecordsetPtr xRecordset;
		xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
		
		BuildReport( &xRecorddbf, xRecordset );
		
		xCloseRecordset( xRecordset );
		xRecorddbf.Close();
	}
	catch(_com_error e) 
	{ 
		Message( MSG_ERROR, _T("CTelecomWnd::OnBuildReport()发生错误: %s"), e.ErrorMessage() );
		return FALSE;
	}
	catch(CDBFException* e)
	{
		//e->ReportError();
		TCHAR szError[1024];
		e->GetErrorMessage( szError, 1024 );
		Message( MSG_ERROR, _T("CTelecomWnd::OnBuildReport()发生错误: %s"), szError );
		e->Delete();
		return FALSE;
	}
	
	return TRUE;
}

BOOL CTelecomWnd::PutToRemote(LPCTSTR xPath, LPCTSTR xFile)
{
	try
	{
		CString strPath = xFile;
		if ( strPath.ReverseFind( '\\' ) >= 0 )
		strPath = strPath.Right( strPath.GetLength() - strPath.ReverseFind( '\\' ) - 1 );
		
		CInternetSession* pInetSession = 
			new CInternetSession("DataCenter", 1, PRE_CONFIG_INTERNET_ACCESS);
		
		CFtpConnection* pFtpConnection = 
			pInetSession->GetFtpConnection(
				Settings.Telecom.Host,
				Settings.Telecom.UserId,
				Settings.Telecom.Password );
		
		pFtpConnection->SetCurrentDirectory( xPath );
		if ( pFtpConnection->PutFile( xFile, strPath ) )
		{
			MoveFile( xFile, Settings.General.Path + _T("\\Data\\") + strPath );
			Message( MSG_DEFAULT, "成功上传[%s]文件，并移动到[Data]目录", strPath );
		}
		else
		{
			Message( MSG_DEFAULT, "上传文件[%s]失败[%i]：%s", strPath,
					GetLastError(), theApp.GetErrorString() );
		}
		
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

BOOL CTelecomWnd::CreateFile(LPCTSTR xFile, CDBFTableDef& xTable)
{	
	if ( xTable.IsOpen() ) xTable.Close();
	xTable.CreateTable( xFile );
	
	AddField( &xTable, "CODE", FLD_TYPE_CHARACTER, 13 );
	AddField( &xTable, "CODE168", FLD_TYPE_CHARACTER, 11 );
	AddField( &xTable, "DATE", FLD_TYPE_CHARACTER, 8 );
	AddField( &xTable, "TIME", FLD_TYPE_CHARACTER, 8 );
	AddField( &xTable, "LONG", FLD_TYPE_NUMERIC, 6, 0 );
	AddField( &xTable, "PRICE", FLD_TYPE_NUMERIC, 8, 2 );
	AddField( &xTable, "AREA", FLD_TYPE_CHARACTER, 2 );
	
	xTable.UpdateTable();
	xTable.Close();
	
	return TRUE;
}

void CTelecomWnd::AddField(CDBFTableDef* pTable, DBF_CHAR* sField, DBF_CHAR field_type, DBF_UINT char_len)
{
	FIELD_REC FieldRec;
	
	ZeroMemory(&FieldRec, sizeof(FIELD_REC));
	CopyMemory(FieldRec.field_name, sField, 10);
	FieldRec.field_type = field_type;
	FieldRec.len_info.char_len = char_len;
	
	pTable->AddField( &FieldRec );
}

void CTelecomWnd::AddField(CDBFTableDef* pTable, DBF_CHAR* sField, DBF_CHAR field_type, DBF_CHAR num_len, DBF_CHAR num_dec)
{
	FIELD_REC FieldRec;
	
	ZeroMemory(&FieldRec, sizeof(FIELD_REC));
	CopyMemory(FieldRec.field_name, sField, 10);
	FieldRec.field_type = field_type;
	FieldRec.len_info.num_size.len = num_len;
	FieldRec.len_info.num_size.dec = num_dec;
	
	pTable->AddField( &FieldRec );
}

BOOL CTelecomWnd::BuildReport(CDBFRecordset* xRecorddbf, _RecordsetPtr xRecordset)
{
	for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
	{
		int nItem = -1;
		_variant_t TheValue;
		CXMLElement* pXMLFee = NULL;
			
#if 1
		// 流水号==NULL表示该流水的起始呼入
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
		// 确认计费号码符合05746,05748,0574201
		TheValue = xRecordset->GetCollect( "UsrNumber" );
		if ( VT_NULL == TheValue.vt ) continue;
		{
			CString strValue = (LPCTSTR)(_bstr_t)TheValue;
			strValue = strValue.Left( 5 );
			if ( strValue.Left(1) != "6" && strValue.Left(1) != "8" &&
				 strValue.Left(3) != "201" ) continue;
		}
#endif
		
		// 计费号码
		TheValue = xRecordset->GetCollect( "UsrNumber" );
		if ( VT_NULL == TheValue.vt )
		{
			// 新增记录
			xRecorddbf->AddNew();
			
			xRecorddbf->SetFieldValue("CODE", _variant_t());
			nItem = PrintLineText( "", 0, nItem );
		}
		else
		if ( IsTestPhone( (LPCTSTR)(_bstr_t)TheValue ) )
		{
			continue;
		}
		else
		{
			// 新增记录
			xRecorddbf->AddNew();
			
			xRecorddbf->SetFieldValue("CODE", TheValue);
			nItem = PrintLineText( (LPCTSTR)(_bstr_t)TheValue, 0, nItem );
		}
		
		// 对端号码
		TheValue = xRecordset->GetCollect( "xFwdPhone" );
		if ( VT_NULL == TheValue.vt || ! _tcscmp((LPCTSTR)(_bstr_t)TheValue,"95950") )
		{	
			pXMLFee = FindProfile( m_pXML, _T("95950") );
			xRecorddbf->SetFieldValue("CODE168", _variant_t("95950"));
			nItem = PrintLineText( "95950", 1, nItem );
		}
		else
		{	
			pXMLFee = FindProfile( m_pXML, _T("95951") );
			xRecorddbf->SetFieldValue("CODE168", _variant_t("95951"));
			nItem = PrintLineText( "95951", 1, nItem );
		}
#if 1		
		CalcMapTalking( xRecordset, xSeqNumber, &nMapLeng, &nMapFee, pXMLFee );
#endif
		
		// 通话起始日期
		TheValue = xRecordset->GetCollect( "xTimeStart" );
		if ( VT_NULL != TheValue.vt )
		{
			CString strValue;
			COleDateTime tt(TheValue);
			strValue.Format( "%04d%02d%02d", tt.GetYear(), tt.GetMonth(), tt.GetDay() );
			xRecorddbf->SetFieldValue( "DATE", _variant_t(strValue) );
			nItem = PrintLineText( strValue, 2, nItem );
		}
		
		// 通话起始时间
		TheValue = xRecordset->GetCollect( "xTimeStart" );
		if ( VT_NULL != TheValue.vt )
		{
			CString strValue;
			COleDateTime tt(TheValue);
			strValue.Format( "%02d%02d%02d", tt.GetHour(), tt.GetMinute(), tt.GetSecond() );
			xRecorddbf->SetFieldValue( "TIME", _variant_t(strValue) );
			nItem = PrintLineText( strValue, 3, nItem );
		}
		
		// 通话时长
		int nSpeakLeng = 0;
		TheValue = xRecordset->GetCollect( "xTimeLength" );
		if ( VT_NULL != TheValue.vt )
		{
			nSpeakLeng = TheValue.lVal;
		}
		
		nSpeakLeng = GetNewLength(nSpeakLeng) + nMapLeng;
		xRecorddbf->SetFieldValue("LONG", _variant_t(long(nSpeakLeng)));
		nItem = PrintLineText( nSpeakLeng, 4, nItem );
		
		DWORD nTotalFee = 0;
		
		// 基本费用
		if ( pXMLFee != NULL )
		{
			CString strValue;
			strValue = pXMLFee->GetAttributeValue( _T("Talk") );
			nTotalFee += (nSpeakLeng+60)/60 * _ttoi( strValue );
		}
		
		// 信息费用
		if ( pXMLFee != NULL )
		{
			CString strValue;
			strValue = pXMLFee->GetAttributeValue( _T("News") );
			nTotalFee += (nSpeakLeng+60)/60 * _ttoi( strValue );
			nTotalFee += nMapFee;
		}
		
		COleVariant varValue;
		varValue.Clear();
		varValue = double(nTotalFee) / 100;
		xRecorddbf->SetFieldValue("PRICE", varValue);
		nItem = PrintLineText( double(nTotalFee) / 100, 5, nItem );
		
		xRecorddbf->Update();
	}
	
	return TRUE;
}

int CTelecomWnd::PrintLineText(LPCTSTR xText, int nSubItem, int nAtLine)
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

int CTelecomWnd::PrintLineText(int xValue, int nSubItem, int nAtLine)
{
	CHAR xText[16];
	_itot( xValue, xText, 10 );
	
	return PrintLineText( xText, nSubItem, nAtLine );
}

int CTelecomWnd::PrintLineText(double xValue, int nSubItem, int nAtLine)
{
	CHAR xText[16];
	_stprintf( xText, "%.2lf", xValue );
	
	return PrintLineText( xText, nSubItem, nAtLine );
}

int CTelecomWnd::GetNewLength(int nLength) const
{
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
	nNewLength += Settings.Telecom.TakAdjust;
#endif

	return nNewLength;
}

#if 0
BOOL CTelecomWnd::CalcMapTalking(DWORD nSeqId, int* pnLeng, int* pnFee, CXMLElement* pXMLFee)
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
BOOL CTelecomWnd::CalcMapTalking(_RecordsetPtr xRecordset, DWORD nSeqId, int* pnLeng, int* pnFee, CXMLElement* pXMLFee)
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
