//
// CtrlTrafficPanel.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlTrafficPanel.h"
#include "CtrlNetworkPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTrafficPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CTrafficPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CTrafficPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CTrafficPanel construction

CTrafficPanel::CTrafficPanel()
{
}

CTrafficPanel::~CTrafficPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTrafficPanel message handlers

BOOL CTrafficPanel::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd)
{
	return CTaskPanel::Create( dwStyle, rect, pParentWnd, IDC_TRAFFIC_PANEL );
}

int CTrafficPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	SetOwner( GetParent() );
	SetFooter( NULL, TRUE );
	
//	m_wndVideo.Create( this, 140, _T("±æµÿ ”∆µ"), IDI_TASKPANEL );
	AddBox( &wndVideo );
	
	return 0;
}
