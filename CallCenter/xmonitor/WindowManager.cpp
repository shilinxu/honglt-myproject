//
// WindowManager.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "WindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWindowManager, CWnd)
	//{{AFX_MSG_MAP(CWindowManager)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CWindowManager construction

CWindowManager::CWindowManager(CMDIFrameWnd* pParent)
{
	m_bIgnoreActivate	= FALSE;
	m_bClosing			= FALSE;
	
	if ( pParent ) SetOwner( pParent );
}

CWindowManager::~CWindowManager()
{
}

//////////////////////////////////////////////////////////////////////
// CWindowManager owner

void CWindowManager::SetOwner(CMDIFrameWnd* pParent)
{
	m_pParent = pParent;
	SubclassWindow( m_pParent->m_hWndMDIClient );
}

//////////////////////////////////////////////////////////////////////
// CWindowManager add and remove

void CWindowManager::Add(CChildWnd* pChild)
{
	CSingleLock pLock( &theApp.m_pSection, TRUE );
	if ( m_pWindows.Find( pChild ) == NULL ) m_pWindows.AddTail( pChild );
}

void CWindowManager::Remove(CChildWnd* pChild)
{
	CSingleLock pLock( &theApp.m_pSection, TRUE );
	POSITION pos = m_pWindows.Find( pChild );
	if ( pos ) m_pWindows.RemoveAt( pos );
}

//////////////////////////////////////////////////////////////////////
// CWindowManager get active

CChildWnd* CWindowManager::GetActive() const
{
	if ( m_hWnd == NULL ) return NULL;
	
	CWnd* pActive = m_pParent->MDIGetActive();

	if ( pActive && pActive->IsKindOf( RUNTIME_CLASS(CChildWnd) ) )
		return (CChildWnd*)pActive;
	else
		return NULL;
}

//////////////////////////////////////////////////////////////////////
// CWindowManager iteration

POSITION CWindowManager::GetIterator() const
{
	return m_pWindows.GetHeadPosition();
}

CChildWnd* CWindowManager::GetNext(POSITION& pos) const
{
	return (CChildWnd*)m_pWindows.GetNext( pos );
}

BOOL CWindowManager::Check(CChildWnd* pChild) const
{
	return m_pWindows.Find( pChild ) != NULL;
}

//////////////////////////////////////////////////////////////////////
// CWindowManager find

CChildWnd* CWindowManager::Find(CRuntimeClass* pClass, CChildWnd* pAfter, CChildWnd* pExcept)
{
	CSingleLock pLock( &theApp.m_pSection, TRUE );
	BOOL bFound = ( pAfter == NULL );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CChildWnd* pChild = GetNext( pos );

		if ( pChild == pExcept ) continue;
		else if ( bFound && ( !pClass || pChild->IsKindOf( pClass ) ) ) return pChild;
		else if ( pChild == pAfter ) bFound = TRUE;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// CWindowManager open / toggle

CChildWnd* CWindowManager::Open(CRuntimeClass* pClass, BOOL bFocus)
{
	CSingleLock pLock( &theApp.m_pSection, TRUE );
	
	CChildWnd* pChild = Find( pClass );
	
	if ( pChild && pChild->IsIconic() )
	{
		pChild->ShowWindow( SW_SHOWNORMAL );
	}
#if 0	
	if ( pChild && GetActive() == pChild )
	{
		pChild->DestroyWindow();
		return NULL;
	}
#endif
	pLock.Unlock();
	
	if ( ! pChild ) pChild = (CChildWnd*)pClass->CreateObject();
	
	if ( bFocus ) pChild->BringWindowToTop();
	
	return pChild;
}

//////////////////////////////////////////////////////////////////////
// CWindowManager find by point

CChildWnd* CWindowManager::FindFromPoint(const CPoint& point) const
{
	CPoint pt( point );
	
	ScreenToClient( &pt );
	CChildWnd* pWnd = (CChildWnd*)ChildWindowFromPoint( pt );
	
	return ( pWnd != NULL && pWnd->IsKindOf( RUNTIME_CLASS(CChildWnd) ) ) ? pWnd : NULL;
}

//////////////////////////////////////////////////////////////////////
// CWindowManager close

void CWindowManager::Close()
{
	CSingleLock pLock( &theApp.m_pSection, TRUE );
	CPtrList pClose;
	
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		pClose.AddTail( GetNext( pos ) );
	}
	
	for ( pos = pClose.GetHeadPosition() ; pos ; )
	{
		CChildWnd* pChild = (CChildWnd*)pClose.GetNext( pos );
		pChild->DestroyWindow();
	}
}

//////////////////////////////////////////////////////////////////////
// CWindowManager automatic resize

void CWindowManager::AutoResize()
{
	CChildWnd* pChild;
	CRect rcSize;
	
	GetClientRect( &rcSize );
	if ( rcSize.right < 64 || rcSize.bottom < 64 ) return;
	
	for ( pChild = (CChildWnd*)GetWindow( GW_CHILD ) ; pChild ; pChild = (CChildWnd*)pChild->GetNextWindow() )
	{
		if ( ! pChild->IsKindOf( RUNTIME_CLASS(CChildWnd) ) ) continue;
		
		CRect rcChild;
		pChild->GetWindowRect( &rcChild );
		ScreenToClient( &rcChild );
		
		if ( rcChild.right == m_rcSize.right || rcChild.bottom == m_rcSize.bottom )
		{
			if ( rcChild.right == m_rcSize.right ) rcChild.right = rcSize.right;
			if ( rcChild.bottom == m_rcSize.bottom ) rcChild.bottom = rcSize.bottom;
			
			pChild->MoveWindow( &rcChild );
		}
	}
	
	m_rcSize = rcSize;
	
	for ( pChild = (CChildWnd*)GetWindow( GW_CHILD ) ; pChild ; pChild = (CChildWnd*)pChild->GetNextWindow() )
	{
		if ( ! pChild->IsKindOf( RUNTIME_CLASS(CChildWnd) ) ) continue;
		
		CRect rcChild;
		pChild->GetWindowRect( &rcChild );
		ScreenToClient( &rcChild );
		if ( rcChild != rcSize ) pChild->MoveWindow( &rcSize );
	}
}

//////////////////////////////////////////////////////////////////////
// CWindowManager customized cascade

void CWindowManager::Cascade(BOOL bActiveOnly)
{
	CSingleLock pLock( &theApp.m_pSection, TRUE );

	CChildWnd* pActive = bActiveOnly ? GetActive() : NULL;

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CChildWnd* pChild = GetNext( pos );

		if ( ! pChild && pChild->IsWindowVisible() &&
			 ! pChild->IsIconic() && ( pChild == pActive || ! pActive ) )
		{
			CRect rcClient;
			pChild->ShowWindow( SW_RESTORE );
			pChild->GetParent()->GetClientRect( &rcClient );
			pChild->MoveWindow( &rcClient );
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CWindowManager message handlers

void CWindowManager::OnSize(UINT nType, int cx, int cy)
{
	if ( nType != 1982 ) CWnd::OnSize( nType, cx, cy );
	AutoResize();
}

BOOL CWindowManager::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect( &rc );
#if 0
	pDC->FillSolidRect( &rc, ( ! m_bClosing ) ?
		RGB( 0xBE, 0, 0 ) : GetSysColor( COLOR_APPWORKSPACE ) );
#else
	pDC->FillSolidRect( &rc, GetSysColor( COLOR_APPWORKSPACE ) );
#endif
	return TRUE;
}

void CWindowManager::OnPaint() 
{
	CPaintDC dc( this );
}
