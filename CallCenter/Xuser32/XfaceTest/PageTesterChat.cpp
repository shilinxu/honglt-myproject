//
// PageTesterChat.cpp
//

#include "stdafx.h"
#include "XfaceTest.h"

#include "PageTesterChat.h"
#include "Xuserface.h"
#include "Network.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CChatTesterPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CChatTesterPage, CSettingsPage)
	//{{AFX_MSG_MAP(CChatTesterPage)
	ON_WM_TIMER()
	ON_COMMAND(IDC_CONNECT, OnConnect)
	ON_COMMAND(IDC_DIALUP, OnDialup)
	ON_COMMAND(IDC_HANGUP, OnHangup)
	ON_COMMAND(IDC_RECORD, OnRecordFile)
	ON_COMMAND(IDC_BEARING, OnPressTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define	WM_TIMERA	(WM_USER + 1)

/////////////////////////////////////////////////////////////////////////////
// CChatTesterPage property page

CChatTesterPage::CChatTesterPage() : CSettingsPage( CChatTesterPage::IDD )
{
	//{{AFX_DATA_INIT(CChatTesterPage)
	//}}AFX_DATA_INIT
}

CChatTesterPage::~CChatTesterPage()
{
}

void CChatTesterPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChatTesterPage)
	DDX_Control(pDX, IDC_PLACEHOLDER, m_wndPreview);
	DDX_Control(pDX, IDC_PHONE, m_wndPhone);
	DDX_Control(pDX, IDC_STATUS, m_wndStatus);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CChatTesterPage message handlers

BOOL CChatTesterPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndPhone.SetWindowText( "013867877813" );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChatTesterPage::OnConnect()
{
	ASSERT( Network.m_bConnected );
	
	CRect rect;
	m_wndPreview.GetWindowRect( &rect );
	
	m_wndPreview.ScreenToClient( &rect ); rect.DeflateRect( 1, 1 );

#if 1
//	XRenderStream( RenderPayload(PayloadMask_Video)|PreviewPayload(PayloadMask_Video), Network.m_xUserId );
	XRenderStream( RenderPayload(PayloadMask_Audio), Network.m_xUserId );
#else
	XRenderStream( RenderPayload(PayloadMask_Video|PayloadMask_Audio), Network.m_xUserId );
	XRenderStream( PreviewPayload(PayloadMask_Video), Network.m_xUserId );
//	XrenderStream( RenderPayload(|PayloadMask_Audio) );
#endif

	XStartPreview( rect, m_wndPreview.GetSafeHwnd(), 0 );
	
	SetTimer( WM_TIMERA, 100, TimerProcA );
}

void CChatTesterPage::OnDialup()
{
	CString strValue;
	m_wndPhone.GetWindowText( strValue );
	
	DWORD dwStyle = m_wndPhone.GetStyle();
	if ( dwStyle & ES_READONLY ) XStartTalk();
	else XDialupWith( strValue, NULL );
	m_wndStatus.SetWindowText( "正在呼叫" );
}

void CChatTesterPage::OnHangup()
{
	XLeaveTalk();
	m_wndStatus.SetWindowText( "空闲" );
	
	m_wndPhone.SetReadOnly( FALSE );
}

void CChatTesterPage::OnRecordFile()
{
	XRecordFile();
}

void CChatTesterPage::OnPressTest()
{
	if ( ! KillTimer(1) )
	SetTimer( 1, 1000, NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CChatTesterPage common timer

void CChatTesterPage::OnTimer(UINT nIDEvent) 
{
	static ii=0;
	
	PostMessage(WM_COMMAND, (ii++ % 2) ? IDC_DIALUP : IDC_HANGUP );
}

void CALLBACK CChatTesterPage::TimerProcA(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	CChatTesterPage *pView = (CChatTesterPage *) CWnd::FromHandle( hWnd );
	
	static CHAR szMsg[4096];
	
	if ( XCallWorker(szMsg, 0) ) return;
	
	CXMLElement* pXML = CXMLElement::FromBytes((BYTE*)szMsg, _tcslen(szMsg), FALSE );
	
	if ( pXML && pXML->IsNamed( ISyncHostFile ) )
	{
		pView->OnSyncHostFile( pXML );
	}
	
	pXML->Delete();
}

BOOL CChatTesterPage::OnSyncHostFile(CXMLElement* pXML)
{
	CString strMethod = pXML->GetAttributeValue("method");
	
	if ( ! strMethod.CompareNoCase( "ring" ) )
	{
		CXMLElement* pSync = pXML->GetElementByName( "RemoteId" );
		
		m_wndPhone.SetWindowText( pSync->GetValue() );
		m_wndStatus.SetWindowText( "有电话呼入" );
		m_wndPhone.SetReadOnly( TRUE );
		
		XRenderPreview(PreviewPayload(PayloadMask_Audio), Network.m_xUserId);
	}
	else if ( ! strMethod.CompareNoCase( "bye" ) )
	{
		PostMessage(WM_COMMAND, IDC_HANGUP);
	}
	
	return TRUE;
}
