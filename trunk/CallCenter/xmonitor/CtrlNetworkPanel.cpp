//
// CtrlNetworkPanel.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "CoolInterface.h"
#include "CtrlNetworkPanel.h"

#include "Network.h"
#include "ChannelSentry.h"
#include "Profiles.h"
#include "Tlhelp32.h"
#include "XML.h"

#include "Psapi.h"
#pragma comment ( lib, "psapi.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CNetworkPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CNetworkPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CNetworkTaskBox, CTaskBox)
BEGIN_MESSAGE_MAP(CNetworkTaskBox, CTaskBox)
	//{{AFX_MSG_MAP(CNetworkTaskBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CNetworkMonitorBox, CTaskBox)
BEGIN_MESSAGE_MAP(CNetworkMonitorBox, CTaskBox)
	//{{AFX_MSG_MAP(CNetworkMonitorBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
//	ON_WM_PAINT()
	ON_COMMAND(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BOX_MARGIN	8

//////////////////////////////////////////////////////////////////////
// CNetworkPanel construction

CNetworkPanel::CNetworkPanel()
{
}

CNetworkPanel::~CNetworkPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkPanel message handlers

BOOL CNetworkPanel::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd)
{
	return CTaskPanel::Create( dwStyle, rect, pParentWnd, IDC_NETWORK_PANEL );
}

int CNetworkPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	SetOwner( GetParent() );
	SetFooter( NULL, TRUE );
	
	m_wndTask.Create( this, 112, _T("操作"), IDI_TASKPANEL );
	AddBox( &m_wndTask );
	
	m_wndMonitor.Create( this, 38, _T("监视"), IDI_SEARCH_FOLDER );
	AddBox( &m_wndMonitor );
	
	return 0;
}

void CNetworkPanel::ExecuteMonitor()
{
	if ( ! m_wndMonitor.GetSafeHwnd() ) return;
	m_wndMonitor.SendMessage( WM_COMMAND, IDOK );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkTaskBox construction

CNetworkTaskBox::CNetworkTaskBox()
{
	m_bPending	= FALSE;
}

CNetworkTaskBox::~CNetworkTaskBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkTaskBox message handlers

int CNetworkTaskBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetPrimary( TRUE );
	CRect rc( 0, 0, 0, 0 );
	
	m_wndChan.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWN|CBS_SORT,
		rc, this, IDC_NETWORK_CHAN );
	m_wndPlay.Create( WS_CHILD|WS_VSCROLL|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWN|CBS_AUTOHSCROLL,
		rc, this, IDC_NETWORK_PLAY );
	
	m_wndChan.SetFont( &CoolInterface.m_fntNormal );
	
	m_wndPlay.SetFont( &CoolInterface.m_fntNormal );
	m_wndPlay.AddString( "监听" );
	m_wndPlay.AddString( "录音" );
	m_wndPlay.SetCurSel( 0 );
	
	m_wndStart.Create( rc, this, IDOK );
	m_wndStart.SetWindowText( "确定" );
	m_wndStart.SetIcon( CoolInterface.m_pImages.ExtractIcon( 22 ) );
	m_wndStart.SetHandCursor( TRUE );
	
	return 0;
}

void CNetworkTaskBox::OnDestroy()
{
}

void CNetworkTaskBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 2 );
	
	DeferWindowPos( hDWP, m_wndChan, NULL, BOX_MARGIN, 27,
		cx - BOX_MARGIN * 2, 80, SWP_NOZORDER );
	
	DeferWindowPos( hDWP, m_wndPlay, NULL, BOX_MARGIN, 80,
		cx - BOX_MARGIN * 2 - 80, 80, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndStart, NULL, cx - 80, 79, 72, 24, SWP_SHOWWINDOW|SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CNetworkTaskBox::OnPaint() 
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

	rct.OffsetRect( 0, 64 - rct.top );
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

void CNetworkTaskBox::OnOK()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkMonitorBox construction

CNetworkMonitorBox::CNetworkMonitorBox()
{
}

CNetworkMonitorBox::~CNetworkMonitorBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkMonitorBox message handlers

int CNetworkMonitorBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	CRect rc( 0, 0, 0, 0 );
	
	m_wndClock.Create( WS_CHILD|WS_VISIBLE, rc, this, IDC_NETWORK_CLOCK );
	
	m_wndStart.Create( rc, this, IDOK );
	m_wndStart.SetWindowText( "执行" );
	
	int nIcon = CoolInterface.ImageForID( ID_NETWORK_CONNECT );
	m_wndStart.SetIcon( CoolInterface.m_pImages.ExtractIcon( nIcon ) );
	m_wndStart.SetHandCursor( TRUE );
	
	SetTimer( 5, 6000, NULL );
	PostMessage( WM_TIMER, 5 );
	
	return 0;
}

void CNetworkMonitorBox::OnDestroy()
{
}

void CNetworkMonitorBox::OnSize(UINT nType, int cx, int cy) 
{
	CTaskBox::OnSize( nType, cx, cy );
	
	HDWP hDWP = BeginDeferWindowPos( 2 );
	
	DeferWindowPos( hDWP, m_wndStart, NULL, BOX_MARGIN, 7, 55, 23, SWP_NOZORDER );
	DeferWindowPos( hDWP, m_wndClock, NULL, BOX_MARGIN + 61, 7, 13 * 8, 23, SWP_NOZORDER );
	
	EndDeferWindowPos( hDWP );
}

void CNetworkMonitorBox::OnPaint() 
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
	
	rct.SetRect( BOX_MARGIN + 1, BOX_MARGIN + 3, rc.right - BOX_MARGIN, BOX_MARGIN + 16 );
	pDC->ExtTextOut( rct.left, rct.top, nFlags, &rct, "运行在:", NULL );
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

LONG CNetworkMonitorBox::OnTimerOut(WPARAM wParam, LPARAM lParam)
{
	m_wndClock.SetStyle( CEDClock::EClockStyle::XDC_FLOWNEXT );
	GetPanel()->GetParent()->PostMessage( ID_TAB_CONNECT );
	
	return 0;
}

void CNetworkMonitorBox::OnTimer(UINT nIDEvent) 
{
/*	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( hSnapshot == INVALID_HANDLE_VALUE ) return;
	
	ProcessXML( hSnapshot, NULL );
	CloseHandle( hSnapshot );
	
	if ( nIDEvent == 88 )
	{
		m_wndClock.SetStyle( CEDClock::EClockStyle::XDC_FLOWNEXT );
	}
	*/
	//if ( (nIDEvent >> 4) == CEDClock::XDC_FLOWBACK ) 
	ProcessXML( NULL );
	
	CTaskBox::OnTimer( nIDEvent );
}

void CNetworkMonitorBox::OnOK()
{
	BOOL bCheck = ! m_wndStart.GetCheck();
	
	m_wndStart.SetWindowText( bCheck ? "暂停" : "执行" );
	
	int nIcon = CoolInterface.ImageForID( bCheck ? ID_NETWORK_DISCONNECT : ID_NETWORK_CONNECT );
	m_wndStart.SetIcon( CoolInterface.m_pImages.ExtractIcon( nIcon ) );
	
	if ( bCheck )
	{
		m_wndClock.SetStyle( CEDClock::EClockStyle::XDC_NORMAL );
	}
	else
	{
		m_wndClock.SetStyle( CEDClock::EClockStyle::XDC_FLOWBACK, 0 );
		
		ClearProcess( NULL );
	}
	
	m_wndStart.SetCheck( bCheck );
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkMonitorBox process

BOOL CNetworkMonitorBox::ProcessXML(HANDLE hSnapshot, POSITION posNext)
{
	CXMLElement* pXML = Profiles.FindProcess( NULL, posNext );
	if ( pXML == NULL ) return FALSE;
	
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof( PROCESSENTRY32 );
	
	for ( BOOL bRetval = Process32First(hSnapshot,&pe); bRetval; )
	{
		if ( HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID ) )
		{
			TCHAR szPath[128];
			if ( GetModuleFileNameEx( hProcess, NULL, szPath, MAX_PATH ) )
TRACE("%s\n", szPath);
			CloseHandle( hProcess );
		}
		
		pe.dwSize = sizeof( PROCESSENTRY32 );
		bRetval = Process32Next( hSnapshot, &pe );
	}
	
	if ( posNext ) return ProcessXML( hSnapshot, posNext );
	return TRUE;
}

BOOL CNetworkMonitorBox::ProcessXML(POSITION posNext)
{
	CXMLElement* pXML = Profiles.FindProcess( NULL, posNext );
	if ( pXML == NULL ) return FALSE;
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CString strValue = pXML->GetAttributeValue( "Host" );
	
	if ( CChannel* pChannel = Network.FindChannel( strValue ) )
	{
		if ( pChannel->m_hSocket == INVALID_SOCKET && pChannel->TimeOut( 60 * 1000 ) ) 
		{
			CXMLAttribute* pAttri = pXML->GetAttribute( "Retry" );
			if ( ! pAttri ) pAttri = pXML->AddAttribute( "Retry" );
			
			if ( _ttoi( pAttri->GetValue() ) >= 2 )
			{
				AlarmToShortMessage( (LPCTSTR)strValue );
				strValue = pXML->GetAttributeValue( _T("Path") );
				RestartMachine( (LPCTSTR)strValue );
				
				pXML->DeleteAttribute( "Retry" );
			}
			else
			{
				strValue.Format( "%i", _ttoi( pAttri->GetValue() ) + 1 );
				pAttri->SetValue( (LPCTSTR)strValue );
				
				pChannel->LinkRestart();
				pChannel->m_tConnected	= GetTickCount();
			}
		}
		else
		if ( pChannel->IsConnected() )
		{
			pXML->DeleteAttribute( "Retry" );
		}
	}
	else
	{
		CChannel* pChannel = new CSentryChannel( pXML );
		Network.SetChannel( pChannel );
		
		pChannel->LinkRestart();
	}
	
	if ( posNext ) return ProcessXML( posNext );
	return TRUE;
}

BOOL CNetworkMonitorBox::ClearProcess(POSITION posNext)
{
	CXMLElement* pXML = Profiles.FindProcess( NULL, posNext );
	if ( pXML == NULL ) return FALSE;
	
	CSingleLock pLock( &Network.m_pSection, TRUE );
	
	CString strValue = pXML->GetAttributeValue( "Host" );
	
	CChannel* pChannel = Network.FindChannel( strValue );
	if ( pChannel ) pChannel->Release();
	
	if ( posNext ) return ProcessXML( posNext );
	return TRUE;
}
/*
BOOL CNetworkMonitorBox::AlarmMachine(CChannel* pChannel)
{
	CXMLElement* pActionXML;
	POSITION posNext = NULL;
	
	CXMLElement* pXML = Profiles.FindProcess( pChannel->m_xJobId, posNext, &pActionXML );
	if ( pXML ) AlarmToShortMessage( pActionXML );
	
	CString strValue = m_pXML->GetAttributeValue( _T("Path") );
	return RestartMachine( (LPCTSTR)strValue );
}
*/
BOOL CNetworkMonitorBox::RestartMachine(LPCTSTR lpszAction)
{
theApp.Message(MSG_TEMP, lpszAction );
return TRUE;
	CString strPath = lpszAction;
	strPath.TrimLeft(); strPath.TrimRight();
	
	CString sCommandLine;
	if ( strPath.Find( ' ' ) > 0 )
	{
		sCommandLine = strPath.Mid( strPath.Find( ' ' ) + 1 );
		strPath = strPath.Left( strPath.Find( ' ' ) );
	}
	
	ShellExecute( NULL, _T("open"), strPath, sCommandLine, NULL, SW_SHOWNORMAL );
	return TRUE;
}

BOOL CNetworkMonitorBox::AlarmToShortMessage(LPCTSTR lpszHost)
{
	CXMLElement* pActionXML;
	POSITION posNext = NULL;
	
	CXMLElement* pXML = Profiles.FindProcess( lpszHost, posNext, &pActionXML );
	if ( pXML ) return AlarmToShortMessage( lpszHost, pActionXML );
	
	return FALSE;
}

BOOL CNetworkMonitorBox::AlarmToShortMessage(LPCTSTR lpszHost, CXMLElement* pXML)
{
	CString strPhone, strContent;
	ASSERT( pXML->IsNamed( _T("ShortMessage") ) );
	
	if ( CXMLElement* pXMLSub = pXML->GetElementByName( _T("PhoneNum") ) )
	{
		strPhone = pXMLSub->GetValue();
	}
	if ( CXMLElement* pXMLSub = pXML->GetElementByName( _T("MsgContent") ) )
	{
		strContent = pXMLSub->GetValue();
		Replace( strContent, "<HOST>", lpszHost );
	}
	
	CString	strQuery;
	
	strQuery.Format( "insert into [msgsend]..tempsm_mobile(user_mobile_number, msg_content, msg_service_id)"
					" values('%s', '%s', 'ZBK')", strPhone, strContent );
	TRACE( strQuery + "\n" );
	
	_variant_t vRowsAffected(0L);
	theApp.m_pConnection->Execute( (_bstr_t)strQuery, &vRowsAffected, adExecuteNoRecords);
theApp.Message(MSG_TEMP, strQuery );
	
	return vRowsAffected.lVal > 0;
}

