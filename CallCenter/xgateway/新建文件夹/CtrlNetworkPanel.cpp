//
// CtrlNetworkPanel.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "CoolInterface.h"
#include "CtrlNetworkPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNetworkPanel, CWnd)
BEGIN_MESSAGE_MAP(CNetworkPanel, CWnd)
	//{{AFX_MSG_MAP(CNetworkPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

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
	dwStyle |= WS_CHILD|WS_CLIPCHILDREN;
	return CWnd::Create( NULL, NULL, dwStyle, rect, pParentWnd, IDC_NETWORK_PANEL, NULL );
}

int CNetworkPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	SetOwner( GetParent() );
	return 0;
}
