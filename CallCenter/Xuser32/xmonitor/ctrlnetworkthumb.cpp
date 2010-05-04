//
// CtrlNetworkThumb.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlNetworkThumb.h"
#include "CtrlNetworkChild.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CNetworkThumb, CNetworkVideo)
BEGIN_MESSAGE_MAP(CNetworkThumb, CNetworkVideo)
	//{{AFX_MSG_MAP(CNetworkThumb)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CNetworkThumb construction

CNetworkThumb::CNetworkThumb(CNetworkChild* pPanel, LPCTSTR xUserId) : CNetworkVideo(xUserId)
{
	m_pPanel		= pPanel;
	
	Create( pPanel, WINDOW_HEIGHT, xUserId, IDI_TASKPANEL );
}

CNetworkThumb::~CNetworkThumb()
{
	GetPanel()->RemoveBox( this );
}

void CNetworkThumb::PostNcDestroy()
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkThumb operations

CNetworkChild* CNetworkThumb::GetPanel() const
{
	ASSERT( m_pPanel != NULL );
	ASSERT_KINDOF( CNetworkChild, m_pPanel );
	return m_pPanel;
}

CString CNetworkThumb::GetRecordFile(LPCTSTR fMask, LPCTSTR lpszPath) const
{
	CString xNewFile = CString( fMask );
	
	if ( xNewFile.Find( "<USER>" ) >= 0 )
	{
		Replace( xNewFile, "<USER>", m_pszNamed );
	}
	
	if ( xNewFile.Find( "<DATE>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%04i年%02i月%02i日",
			pTime.wYear, pTime.wMonth, pTime.wDay );
		Replace( xNewFile, "<DATE>", strValue );
	}
	
	if ( xNewFile.Find( "<TIME>" ) >= 0 )
	{
		SYSTEMTIME pTime;
		GetLocalTime( &pTime );
		
		CString strValue;
		strValue.Format( "%02i时%02i分%02i秒%03i",
			pTime.wHour, pTime.wMinute, pTime.wSecond, pTime.wMilliseconds );
		Replace( xNewFile, "<TIME>", strValue );
	}
/*	
	if ( xNewFile.Find( "<REMOTE>" ) >= 0 )
	{
		CString strValue;
		Replace( xNewFile, "<REMOTE>", xRemoteId );
	}
*/	
	for ( LPCTSTR xPath = xNewFile, xPathExt = NULL;
		  xPathExt = _tcsistr(xPath, "\\"); xPath = xPathExt + 1 )
	{
		CString strValue = xNewFile.Left(xPathExt - (LPCTSTR)xNewFile);
		CreateDirectory( CString(lpszPath) + "\\" + strValue, NULL );
	}
	
	return xNewFile;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkThumb message handlers

int CNetworkThumb::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	EnableToolTips( TRUE );
	return 0;
}

void CNetworkThumb::OnSize(UINT nType, int cx, int cy) 
{
	CNetworkVideo::OnSize( nType, cx, cy );
	
	CRect rc;
	GetClientRect( &rc );
	
	PutIntoWindow( m_hWnd, rc, TRUE );
}

void CNetworkThumb::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetPanel()->SelectTo( this );
}

void CNetworkThumb::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnLButtonDown( nFlags, point );
}

int CNetworkThumb::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	if ( ! pTI ) return 1;
	
	pTI->uFlags		= TTF_IDISHWND;
	pTI->hwnd		= GetSafeHwnd();
	pTI->uId		= (UINT)GetSafeHwnd();
	
	CString strText;
	strText.Format( "IP: %s:%i", inet_ntoa(m_pHost.sin_addr), ntohs(m_pHost.sin_port) );
	
	pTI->lpszText	= _tcsdup( strText );

	return pTI->uId;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkThumb custom message handlers

HRESULT CNetworkThumb::OnRecordFile(DWORD nPayloadMask, LPCTSTR fMask)
{
	if ( IsRecording() ) return S_OK;
	
	CRegistry pRegistry;
	CString strPath = pRegistry.GetString( "Monitor", "RecPath", "." );
	
	CString strFile = GetRecordFile( fMask, strPath );
	return RecordFile( CComBSTR(strPath+'\\'+strFile), (WORD)RecordPayload(nPayloadMask) );
}
