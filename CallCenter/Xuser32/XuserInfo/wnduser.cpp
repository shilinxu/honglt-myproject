//
// WndUser.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "WndUser.h"
#include "CoolInterface.h"
#include "Network.h"
#include "skin.h"
#include "XML.h"

#include "CtrlNetworkPanel.h"
#include "ProgressFile.h"
#include "DlgPassword.h"
#include "DlgGrouping.h"
#include "Settings.h"
#include <afxinet.h>

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
	ON_WM_MDIACTIVATE()
	ON_BN_CLICKED(AFX_IDW_TOOLBAR, OnToolbarReturn)
	ON_BN_DOUBLECLICKED(AFX_IDW_TOOLBAR, OnToolbarEscape)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_PLAY, OnUpdateMediaPlay)
	ON_COMMAND(ID_MEDIA_PLAY, OnMediaPlay)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_REMOVE, OnUpdateMediaPlay)
	ON_COMMAND(ID_MEDIA_REMOVE, OnMediaRemove)
	ON_COMMAND(ID_MEDIA_REFRESH, OnMediaRefresh)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_RENEW, OnUpdateMediaPlay)
	ON_COMMAND(ID_MEDIA_RENEW, OnMediaRenew)
	ON_COMMAND(ID_SEARCH_QUICK, OnMediaUpload)
	ON_COMMAND(ID_TOPIC_MAILBOX, OnTopicMailbox)
	ON_COMMAND(ID_USER_PASSWORD, OnUserPassword)
	ON_COMMAND(ID_USER_GROUP, OnUserGroup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define SIZE_INTERNAL	1982
#define PANEL_WIDTH		200
#define BAR_HEIGHT		28

#define INTERNAL_HOST	_T("\\\\192.168.0.2\\")
#define INTERNAL_ROOT	_T("Rec3")

#ifdef _BEST
#define FTP_Host		"192.168.0.2"
#define FTP_Port		50
#else
#define FTP_Host		Settings.Network.Host
#define FTP_Port		50
#endif

#define FTP_UserId		"Anonymous"
#define FTP_Password	"96031@96031.com.cn"

//////////////////////////////////////////////////////////////////////
// CUserWnd construction

CUserWnd::CUserWnd() : CChildWnd(FALSE)
{
	m_pListCtrl		= NULL;
	
	Create( IDR_MAILBOXFRAME );
}

CUserWnd::~CUserWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd message handlers

int CUserWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CChildWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	// Load Definitions
	
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFINITIONS );
	Skin.LoadFromResource( AfxGetResourceHandle(), IDR_XML_DEFAULT );
	
	// Panel
	
//	m_wndPanel.Create( WS_VISIBLE, rectDefault, this );
	
	// Header
	
	if ( ! m_wndHeaderBar.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndHeaderBar.SetBarStyle( m_wndHeaderBar.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_BOTTOM );
	
	Skin.CreateToolBar( _T("CUserWnd.Header"), &m_wndHeaderBar );
	
	CString strText;
#ifdef _BEST
	strText.LoadString( IDS_WELCOME_BEST );
#else
	strText.LoadString( IDS_WELCOME );
#endif
	m_wndHeaderBar.SetCaption( (LPCTSTR)strText );
	m_wndHeaderBar.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	
	if ( ! m_wndTime.Create( "", WS_CHILD, rectDefault, &m_wndHeaderBar, 1001 ) ) return -1;
	m_wndTime.SetFont( &CoolInterface.m_fntBold );
//	if ( ! m_wndPhone.Create( WS_CHILD|ES_AUTOHSCROLL, rectDefault, &m_wndHeaderBar, IDC_USER_NUMBER ) ) return -1;
//	m_wndPhone.SetFont( &CoolInterface.m_fntBold );
	
	// List
	
//	m_wndList.Create( WS_CHILD|WS_VISIBLE, rectDefault, this, IDC_USER_LIST );
	
	// Bottom
	
	if ( ! m_wndBottom.Create( this, WS_CHILD|WS_VISIBLE|CBRS_NOALIGN, AFX_IDW_TOOLBAR ) ) return -1;
	m_wndBottom.SetBarStyle( m_wndBottom.GetBarStyle() | CBRS_TOOLTIPS|CBRS_BORDER_TOP );
	Skin.CreateToolBar( _T("CUserWnd.Bottom"), &m_wndBottom );
	
	if ( ! m_wndFind.Create( WS_CHILD|ES_AUTOHSCROLL, rectDefault, &m_wndBottom, IDC_USER_PHONE ) ) return -1;
	m_wndFind.SetFont( &CoolInterface.m_fntNormal );
	m_wndFind.SetWindowText( Network.m_xUserId );
	
	SendMessage( WM_COMMAND, ID_TOPIC_MAILBOX );
	PostMessage( WM_TIMER, 1 );
	
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
	
	if ( rc.Width() < 32 || rc.Height() < 32 ) return;
	
	HDWP hDWP = BeginDeferWindowPos( 2 + m_pListCtrl != 0 );
	
	DeferWindowPos( hDWP, wndPanel.GetSafeHwnd(), NULL,
		rc.left, rc.top, PANEL_WIDTH, rc.Height(), SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndHeaderBar.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_pListCtrl->GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.top + BAR_HEIGHT, rc.Width() - PANEL_WIDTH,
		rc.Height() - BAR_HEIGHT * 2, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndBottom.GetSafeHwnd(), NULL,
		rc.left + PANEL_WIDTH, rc.bottom - BAR_HEIGHT, rc.Width() - PANEL_WIDTH, BAR_HEIGHT, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CUserWnd::OnToolbarReturn()
{
	if ( GetFocus() == &m_wndFind )
	{
		if ( m_wndFind.GetWindowTextLength() > 0 )
			PostMessage( WM_COMMAND, ID_SEARCH_QUICK );
	}
}

void CUserWnd::OnToolbarEscape()
{
	if ( GetFocus() == &m_wndFind )
	{
		m_wndFind.SetWindowText( _T("") );
		if ( m_pListCtrl != NULL ) m_pListCtrl->SetFocus();
	}
}

BOOL CUserWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if ( m_wndHeaderBar.m_hWnd )
	{
		if ( m_wndHeaderBar.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) return TRUE;
	}
	
	return CMDIChildWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CUserWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	if ( bActivate )
	{
		HINSTANCE hPrevInst = AfxGetResourceHandle();
		
		HINSTANCE hInstance = m_hInstance ? (HINSTANCE)m_hInstance : NULL;
		if ( hInstance ) AfxSetResourceHandle( hInstance );
		
		ApplyMenu( IDR_USERFRAME );
		AfxSetResourceHandle( hPrevInst );
		
		wndPanel.SetParent( this );
		OnSize( SIZE_INTERNAL, 0, 0 );
	}
	else
	{
		DestroyMenu();
	}
	
	CChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
}

BOOL CUserWnd::ApplyMenu(UINT nMenuId)
{
	CMenu pMenu;
	pMenu.LoadMenu( nMenuId );
	
	CString str;
	pMenu.GetMenuString( 0, str, MF_BYPOSITION );
	
	CMenu* pSubMenu = pMenu.GetSubMenu( 0 );
	m_hSubMenu = pMenu.GetSubMenu(0)->m_hMenu;
	
	CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	pMainMenu->AppendMenu( MF_POPUP|MF_STRING, (UINT)m_hSubMenu, str ); 
	
	pMainMenu->Detach();
	pMenu.Detach();
	
	AfxGetMainWnd()->DrawMenuBar();
	return TRUE;
}

void CUserWnd::DestroyMenu()
{
	CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	pMainMenu->DeleteMenu( (UINT)m_hSubMenu, MF_BYCOMMAND );
	
	pMainMenu->Detach();
	AfxGetMainWnd()->DrawMenuBar(); 
	
	m_hSubMenu = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd common timer

void CUserWnd::OnTimer(UINT nIDEvent) 
{
	COleDateTime tNow = COleDateTime::GetCurrentTime();
	m_wndTime.SetWindowText( tNow.Format( "%Y-%m-%d %H:%M:%S" ) );
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd custom message handlers

void CUserWnd::OnSyncOrderRelation()
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
#ifdef _BEST
	pXML->AddAttribute( "method", "select" );
#else
	pXML->AddAttribute( "method", "select top 10000" );
#endif
	
	CXMLElement* pSync = pXML->AddElement( "column_expression" );
	pSync->SetValue( "xPCMName, xRecDate1=convert(varchar(19),xRecDate,120), xCaller, xTheState, xjsjbh,"
		"xphonenum" );
	//	"xphonenum=case when left(xphonenum,1) in ('0','1') then '' else '0574' end + xphonenum" );
	
	pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xuservoice" );
	
	CString str;
	str.Format( "left(xphonenum,%i) = '%s'", _tcslen(Network.m_xUserId), Network.m_xUserId );
//	str.Format( "left(case when left(xphonenum,1) in ('0','1') then '' else '0574' end"
//		"+ xphonenum,%i) = '%s'", _tcslen(Network.m_xUserId), Network.m_xUserId );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	pSync = pXML->AddElement( "order_by_expression" );
	pSync->SetValue( "xRecDate desc, xjsjbh desc" );
	
	Network.SendPacket( new CSyncDataView(pXML) );
	delete pXML;
}

BOOL CUserWnd::OnSyncDataViewResp(CXMLElement* pXML)
{
#if 0
	CString strKey = pXML->GetAttributeValue( "hKey" );
	DWORD hKey = 0; _stscanf( strKey, "0x%x", &hKey );
	
	if ( hKey != (DWORD)m_hInstance ) return FALSE;
#endif
	return m_wndList.LoadRecordset( pXML );
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd menu GUI

void CUserWnd::OnTopicMailbox()
{
	if ( ! m_wndList.GetSafeHwnd() )
	{
		m_wndList.Create( WS_CHILD|WS_VISIBLE, rectDefault, this, IDC_USER_LIST );
		
		PostMessage( WM_COMMAND, ID_MEDIA_REFRESH );
	}
	
	m_pListCtrl = &m_wndList;
	OnSize( SIZE_INTERNAL, 0, 0 );
	Invalidate();
}

void CUserWnd::OnUpdateMediaPlay(CCmdUI* pCmdUI) 
{
	int nItem = m_wndList.GetNextItem( -1, LVIS_SELECTED );
	pCmdUI->Enable( nItem >= 0 );
}

void CUserWnd::OnMediaPlay()
{
	int nItem = m_wndList.GetNextItem( -1, LVIS_SELECTED );
	if ( nItem < 0 ) return;
	
	//CString strPath;
	//strPath.Format( "ftp://%s:%i/", FTP_Host, FTP_Port );
	
	CString strFile;
	strFile = m_wndList.GetItemText( nItem, 0 );
	
	Replace( strFile, "\\\\192.168.0.2\\", "" );
	Replace( strFile, "\\", "/" );
	
	CFtpConnection* pConnect;
	CInternetFile* pSrcFile = InternetOpenFile( strFile, pConnect );
	if ( ! pSrcFile || ! pConnect ) return;
	
	CFile pTagFile;
	CreateMapFile( strFile, pTagFile );
	
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	
	HINSTANCE hInstance = m_hInstance ? (HINSTANCE)m_hInstance : NULL;
	if ( hInstance ) AfxSetResourceHandle( hInstance );
	
	CProgressFile dlg( pSrcFile, &pTagFile, CProgressFile::FileDownload );
	int nResult = dlg.DoModal();

	AfxSetResourceHandle( hPrevInst );
	InternetCloseFile( pSrcFile, pConnect );
	if ( nResult != IDOK ) return;
	
	strFile = pTagFile.GetFilePath();
	pTagFile.Close();
	
	ShellExecute( NULL, _T("open"), strFile, NULL,NULL, SW_SHOW );   
}

void CUserWnd::OnMediaRemove()
{
	int nItem = m_wndList.GetNextItem( -1, LVIS_SELECTED );
	if ( nItem < 0 ) return;
	
	CString strPath = m_wndList.GetItemText( nItem, 0 );
	strPath.TrimLeft(); strPath.TrimRight();
	
	CString str;
	str.Format( "是否删除\"%s\"等号码发起的录音文件, 比如\"%s\"?",
				m_wndList.GetItemText(nItem, 3), m_wndList.GetItemText(nItem, 0) );
	if (AfxMessageBox(str, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDCANCEL)
	{
		return;
	}
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "delete" );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xuservoice" );
	
	for ( ; nItem != -1; nItem = m_wndList.GetNextItem(-1, LVIS_SELECTED) )
	{
		str.Format( "xjsjbh = %s", m_wndList.GetItemText(nItem, 5) );
		
		pSync = pXML->GetElementByName( "search_condition" );
		if ( ! pSync ) pSync = pXML->AddElement( "search_condition" );
		
		pSync->SetValue( (LPCTSTR)str );
		Network.SendPacket( new CSyncDataView( pXML ) );
		
		m_wndList.DeleteItem( nItem );
	}
	
	pXML->Delete();
}

void CUserWnd::OnMediaRefresh()
{
	CWaitCursor pCursor;
	
	OnSyncOrderRelation();
	
	CXMLElement* pXML;
	if ( ! Network.CallWorker( &pXML, INFINITE ) )
	{
		ASSERT( pXML->IsNamed( ISyncDataView ) );
		
		OnSyncDataViewResp( pXML );
		pXML->Delete();
	}
}

void CUserWnd::OnMediaRenew()
{
	int nItem = m_wndList.GetNextItem( -1, LVIS_SELECTED );
	if ( nItem < 0 ) return;
	
	CString strPath = m_wndList.GetItemText( nItem, 0 );
	strPath.TrimLeft(); strPath.TrimRight();
	
	CString str;
	str.Format( "是否重置\"%s\"等号码发起的录音文件, 比如\"%s\"?",
				m_wndList.GetItemText(nItem, 3), m_wndList.GetItemText(nItem, 0) );
	if (AfxMessageBox(str, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDCANCEL)
	{
		return;
	}
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "update" );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "xuservoice" );
	
	pSync = pXML->AddElement( "column_name" );
	pSync->SetValue( "set xRecDate = getdate()" );
#if 0	
	for ( int nFirst = nItem; nItem != -1; nItem = m_wndList.GetNextItem(nItem, LVIS_SELECTED) )
	{
		str.Format( "xjsjbh = %s", m_wndList.GetItemText(nItem, 5) );
		
		pSync = pXML->GetElementByName( "search_condition" );
		if ( ! pSync ) pSync = pXML->AddElement( "search_condition" );
		
		pSync->SetValue( (LPCTSTR)str );
		Network.SendPacket( new CSyncDataView( pXML ) );
	}
#else
	int nFirst = nItem;
	UpdateReverse( pXML, -1 );
#endif
	delete pXML;
	
	str.Format( "已成功提交\"%s\"等号码发起的录音文件, 比如\"%s\", 请稍后刷新!",
				m_wndList.GetItemText(nFirst, 3), m_wndList.GetItemText(nFirst, 0) );
	AfxMessageBox( str );
}

void CUserWnd::UpdateReverse(CXMLElement* pXML, int nItem)
{
	int nNext = m_wndList.GetNextItem( nItem, LVIS_SELECTED );
	
	if ( nNext != -1 ) UpdateReverse(pXML, nNext);
	if ( nItem == -1 ) return;
	
	CString str;
	str.Format( "xjsjbh = %s", m_wndList.GetItemText(nItem, 5) );
	
	CXMLElement* pSync = pXML->GetElementByName( "search_condition" );
	if ( ! pSync ) pSync = pXML->AddElement( "search_condition" );
	
	pSync->SetValue( (LPCTSTR)str );
	Network.SendPacket( new CSyncDataView( pXML ) );
}

void CUserWnd::OnMediaUpload()
{
	CFileDialog dlg( TRUE, _T("pcm"), _T("*.pcm"), OFN_HIDEREADONLY,
		_T("PCM Files|*.pcm|All Files|*.*||"), this );
	
	if ( dlg.DoModal() != IDOK ) return;
	
	CString xRemoteId;
	m_wndFind.GetWindowText( xRemoteId );
	
	int nLength = _tcslen( Network.m_xUserId );
	if ( xRemoteId.Left( nLength ) != Network.m_xUserId )
	{
		AfxMessageBox( "非VIP用户只限于上传于自己！");
		return;
	}
	
	CString strPath( INTERNAL_ROOT );
	strPath += '/'; strPath += Network.m_xUserId;
	
	CString strFile = InternetMapFile( strPath, dlg.GetPathName() );
	
	CFtpConnection* pConnect;
	CInternetFile* pTagFile = InternetOpenFile( strFile, pConnect, GENERIC_WRITE );
	if ( ! pTagFile || ! pConnect ) return;
	
	CFile pSrcFile;
	pSrcFile.Open( dlg.GetPathName(), CFile::modeRead );
	
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	
	HINSTANCE hInstance = m_hInstance ? (HINSTANCE)m_hInstance : NULL;
	if ( hInstance ) AfxSetResourceHandle( hInstance );
	
	CProgressFile dlg2( &pSrcFile, pTagFile, CProgressFile::FileUpload );
	int nResult = dlg2.DoModal();

	AfxSetResourceHandle( hPrevInst );
	InternetCloseFile( pTagFile, pConnect );
	if ( nResult != IDOK ) return;
	
	pSrcFile.Close();
	strPath = INTERNAL_HOST + strFile;
	Replace( strPath, "/", "\\" );
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "insert into" );
	
	CXMLElement* pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "xuservoice" );
	
	CString str;
	str.Format( "(xPCMName,xPhoneNum,xRecDate,xCaller,xTheState)"
					" values('%s','%s',getdate(),'%s','DELIVRD')",
					strPath, xRemoteId, Network.m_xUserId );
	
	pSync = pXML->AddElement( "column_name" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView( pXML ) ); delete pXML;
	
	str.Format( "已成功提交\"%s\"号码发起的录音文件\"%s\", 请稍后刷新!",
				xRemoteId, dlg.GetPathName() );
	AfxMessageBox( str );
}

void CUserWnd::OnUserPassword()
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	
	HINSTANCE hInstance = m_hInstance ? (HINSTANCE)m_hInstance : NULL;
	if ( hInstance ) AfxSetResourceHandle( hInstance );
	
	CPasswordDlg dlg;
	dlg.DoModal();
	
	AfxSetResourceHandle( hPrevInst );
}

void CUserWnd::OnUserGroup()
{
	HINSTANCE hPrevInst = AfxGetResourceHandle();
	
	HINSTANCE hInstance = m_hInstance ? (HINSTANCE)m_hInstance : NULL;
	if ( hInstance ) AfxSetResourceHandle( hInstance );
	
	CGroupingDlg dlg;
	dlg.DoModal();
	
	AfxSetResourceHandle( hPrevInst );
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd file transfer handlers

CInternetFile* CUserWnd::InternetOpenFile(LPCTSTR lpszFile, CFtpConnection*& pConnect, DWORD dwAccess)
{
	pConnect = NULL;
	CInternetSession* pInternet = NULL;
	
	try
	{
		pInternet = new CInternetSession( _T("CallCenter") );
		
		pConnect = pInternet->GetFtpConnection( FTP_Host, FTP_UserId, FTP_Password, FTP_Port, TRUE );
		
		CInternetFile* pFile = pConnect->OpenFile( lpszFile, dwAccess, FTP_TRANSFER_TYPE_BINARY );
		
		return pFile;
	}
	catch (CException* pException)
	{
		pException->ReportError();
		pException->Delete();
		
		if ( pConnect ) delete pConnect;
		if ( pInternet ) delete pInternet;
		return NULL;
	}
}

BOOL CUserWnd::InternetCloseFile(CInternetFile* pFile, CFtpConnection* pConnect)
{
	try
	{
		pFile->Close(); delete pFile;
		
		CInternetSession* pInternet = pConnect->GetSession();
		
		pConnect->Close(); delete pConnect;
		pInternet->Close(); delete pInternet;
		
		return TRUE;
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage( szErr, 1024 );
		
		AfxMessageBox( CString("CInternetException: ") + szErr );
		return FALSE;
	}
}

BOOL CUserWnd::CreateMapFile(LPCTSTR lpszFile, CFile& pFile) const
{
	CString strFile( lpszFile );
	Replace( strFile, "/", "\\" );
	
	if ( strFile.ReverseFind( '\\' ) >= 0 )
	strFile = strFile.Right( strFile.GetLength() - strFile.ReverseFind( '\\' ) - 1 );
	
	CString strPath;
	GetTempPath( MAX_PATH, strPath.GetBuffer(MAX_PATH) );
	
	strPath.ReleaseBuffer();
	strPath += strFile;
	
	return pFile.Open( strPath, CFile::modeWrite|CFile::modeCreate );
}

CString CUserWnd::InternetMapFile(LPCTSTR lpszPath, LPCTSTR lpszFile)
{
	CString strFile( lpszFile );
	
	if ( strFile.ReverseFind( '\\' ) >= 0 )
	strFile = strFile.Right( strFile.GetLength() - strFile.ReverseFind( '\\' ) - 1 );
	
	CString strPath( lpszPath );
	strPath += '/'; strPath += strFile;
	
	return strPath;
}

/////////////////////////////////////////////////////////////////////////////
// CUserWnd user group info

DWORD CUserWnd::GetUserGroupInfo(LPCTSTR xUserId, CXMLElement** ppXML, DWORD nTimeOut)
{
	CXMLElement* pXML = new CXMLElement( NULL, ISyncDataView );
	pXML->AddAttribute( "method", "select" );
	
	CXMLElement* pSync = pXML->AddElement( "column_expression" );
	pSync->SetValue( "a.xphone, a.xname, a.xgroupnum, b.xgroupname" );
	
	pSync = pXML->AddElement( "table_name" );
	pSync->SetValue( "from xphonemeetinggroupmore a"
		" inner join xphonemeetinggroup b on a.xjsjbh=b.xjsjbh"
			" and a.xgroupnum=b.xgroupnum" );
	
	CString str;
	str.Format( "a.xjsjbh in (select xjsjbh from xuser "
				"where xphone = '%s')", Network.m_xUserId );
	
	pSync = pXML->AddElement( "search_condition" );
	pSync->SetValue( (LPCTSTR)str );
	
	Network.SendPacket( new CSyncDataView(pXML) );
	delete pXML; pXML = NULL;
	
	if ( ! nTimeOut ) return 0;
	return Network.CallWorker( ppXML, nTimeOut );
}
