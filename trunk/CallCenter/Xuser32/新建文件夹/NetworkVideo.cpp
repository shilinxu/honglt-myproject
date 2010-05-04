//
// NetworkVideo.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "NetworkVideo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkVideo, CWnd)
BEGIN_MESSAGE_MAP(CNetworkVideo, CWnd)
	//{{AFX_MSG_MAP(CNetworkVideo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CNetworkVideo construction

CNetworkVideo::CNetworkVideo(CVideoCapture* pCapture)
{
	m_pCapture	= pCapture;
}

CNetworkVideo::~CNetworkVideo()
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo message handlers

BOOL CNetworkVideo::Create(const RECT& rect, CWnd* pParentWnd, LPCTSTR pszCaption)
{
	if ( ! CWnd::Create( NULL, pszCaption, WS_VISIBLE|WS_CHILD,
		rect, pParentWnd, 100, NULL ) ) return FALSE;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkVideo operations

BOOL CNetworkVideo::StartPreview(IBaseFilter* pCapFilter, const RECT& rect)
{
	if ( ! m_pCapture ) return FALSE;
	
	m_pCapture->InstallFilter( pCapFilter, L"VideoCap" );
	
	m_pCapture->ConnectCapture( &MEDIATYPE_Video, pCapFilter );
	
	m_pCapture->PutIntoWindow( GetSafeHwnd(), rect, TRUE );
	return TRUE;
}

BOOL CNetworkVideo::StartPreview(LPCWSTR xDisplayName, const RECT& rect)
{
	CComPtr<IBaseFilter> pCapFilter = NULL;
//	if ( ! GetCapFilter(xDisplayName, &pCapFilter) ) return FALSE;
	
	return StartPreview( pCapFilter, rect );
}
