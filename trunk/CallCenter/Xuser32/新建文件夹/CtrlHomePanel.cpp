//
// CtrlHomePanel.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlHomePanel.h"
#include "ChannelUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CHomePanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CHomePanel, CTaskPanel)
	//{{AFX_MSG_MAP(CHomePanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CHomeVideoBox, CTaskBox)
BEGIN_MESSAGE_MAP(CHomeVideoBox, CTaskBox)
	//{{AFX_MSG_MAP(CHomeVideoBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CHomePanel construction

CHomePanel::CHomePanel()
{
}

CHomePanel::~CHomePanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHomePanel message handlers

BOOL CHomePanel::Create(CWnd* pParentWnd)
{
	return CTaskPanel::Create( WS_VISIBLE, CRect(0,0,0,0), pParentWnd, IDC_HOME_PANEL );
}

int CHomePanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	SetOwner( GetParent() );
	SetFooter( NULL, TRUE );
	
	m_wndVideo.Create( this, 140, _T("±¾µØÊÓÆµ"), IDI_TASKPANEL );
	AddBox( &m_wndVideo );
	
	return 0;
}

void CHomePanel::StartPreview(CVideoCapture* pCapture)
{
	CRect rc;
	m_wndVideo.GetClientRect( &rc );
	
	CRect rect;
	rect.SetRect( rc.left, rc.top, rc.right, rc.top + 140 );
	
	if ( pCapture->StartPreview( rect, m_wndVideo.GetSafeHwnd() ) )
		m_wndVideo.Expand( TRUE );
	else
		m_wndVideo.Expand( FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CHomeVideoBox construction

CHomeVideoBox::CHomeVideoBox()
{
}

CHomeVideoBox::~CHomeVideoBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHomeVideoBox message handlers

int CHomeVideoBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskBox::OnCreate( lpCreateStruct ) == -1 ) return -1;
	
	SetPrimary( TRUE );
	Expand( FALSE );
	
	return 0;
}

void CHomeVideoBox::OnDestroy()
{
}
