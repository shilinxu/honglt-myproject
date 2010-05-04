//
// CtrlNetworkPanel.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlNetworkPanel.h"
#include "Settings.h"

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

IMPLEMENT_DYNAMIC(CNetworkVideo, CTaskBox)
BEGIN_MESSAGE_MAP(CNetworkVideo, CTaskBox)
	//{{AFX_MSG_MAP(CNetworkVideo)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_STARTPREVIEW, OnStartPreview)
END_MESSAGE_MAP()

CNetworkPanel	wndPanel;

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
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkPanel custom message handlers

CTaskBox* CNetworkPanel::RenderUser(LPCTSTR xUserId, int nAtAfter)
{
	for ( POSITION posNext = GetBoxIterator(); posNext; )
	{
		CTaskBox* pWindow = GetNextBox( posNext );
		
		ASSERT_KINDOF( CNetworkVideo, pWindow );
		if ( nAtAfter-- > 0 ) continue;
		
		if ( pWindow->IsPrimary() && ! xUserId ) return pWindow;
		ASSERT( pWindow->IsPrimary() == FALSE );
		
		CString strCaption;
		pWindow->GetWindowText( strCaption );
		
		if ( ! strCaption.CompareNoCase( xUserId ) ) return pWindow;
	}
	
	CTaskBox* pWindow = new CNetworkVideo( xUserId );
	
	pWindow->Create( this, 140, xUserId, IDI_TASKPANEL );
	AddBox( pWindow );
	
	return pWindow;
}

CTaskBox* CNetworkPanel::RenderUser(LPCTSTR xUserId, LPCTSTR lpzAlias, int nAtAfter)
{
	CTaskBox* pBox = FindUser( xUserId, nAtAfter ); if ( ! pBox )
	{
		pBox = new CNetworkVideo( xUserId );
		
		pBox->Create( this, 140, lpzAlias, IDI_TASKPANEL );
		
		AddBox( pBox );
	}
	
	return pBox;
}

CTaskBox* CNetworkPanel::FindUser(LPCTSTR xUserId, int nAtAfter) const
{
	for ( POSITION posNext = GetBoxIterator(); posNext; )
	{
		CTaskBox* pWindow = GetNextBox( posNext );
		
		ASSERT_KINDOF( CNetworkVideo, pWindow );
		if ( nAtAfter-- > 0 ) continue;
		
#if 0
		CString strCaption;
		pWindow->GetWindowText( strCaption );
		
		if ( ! strCaption.CompareNoCase( xUserId ) ) return pWindow;
#else
		CNetworkVideo* pVideo = (CNetworkVideo*)pWindow;
		if ( pVideo->IsNamed(xUserId) ) return pWindow;
#endif
	}
	
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo construction

CNetworkVideo::CNetworkVideo(LPCTSTR xUserId) : CGraphRender(xUserId)
{
}

CNetworkVideo::~CNetworkVideo()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo message handlers

int CNetworkVideo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
//	SetPrimary( TRUE );
	Expand( FALSE );
	
	return 0;
}

void CNetworkVideo::OnDestroy()
{
}

void CNetworkVideo::PostNcDestroy() 
{
	CTaskBox::PostNcDestroy();
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo custom message handlers

LONG CNetworkVideo::OnStartPreview(WPARAM nPayloadMask, LPARAM lParam)
{
	if ( (RenderPayload(PayloadMask_Audio) & nPayloadMask) == RenderPayload(PayloadMask_Audio) )
	{
		RenderStream( RenderPayload(PayloadMask_Audio), (CComBSTR)Settings.Audio.Input, lParam );
	}
	if ( (RenderPayload(PayloadMask_Video) & nPayloadMask) == RenderPayload(PayloadMask_Video) )
	{
		RenderStream( RenderPayload(PayloadMask_Video), (CComBSTR)Settings.Video.Input, lParam );
	}
	
	if ( (PreviewPayload(PayloadMask_Audio) & nPayloadMask) == PreviewPayload(PayloadMask_Audio) )
	{
		if ( IsPrimary() ) RenderPreview( PreviewPayload(PayloadMask_Audio), (CComBSTR)Settings.Audio.Render );
		else RenderPreview( PreviewPayload(PayloadMask_Audio), (CComBSTR)Settings.Audio.Render, lParam );
	}
	if ( (PreviewPayload(PayloadMask_Video) & nPayloadMask) == PreviewPayload(PayloadMask_Video) )
	{
		if ( IsPrimary() ) RenderPreview( PreviewPayload(PayloadMask_Video), NULL );
		else RenderPreview( PreviewPayload(PayloadMask_Video), NULL, lParam );
	}
	{
		CRect rc;
		GetClientRect( &rc );
		
		rc.bottom = max(rc.bottom, m_nHeight);
		
		HRESULT hr = CGraphRender::StartPreview( rc, GetSafeHwnd() );
		
		Expand( SUCCEEDED(hr) );
	}
	
	return 0;
}
