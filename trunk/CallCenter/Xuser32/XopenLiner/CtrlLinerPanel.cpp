//
// CtrlLinerPanel.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlLinerPanel.h"
#include "CtrlNetworkPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CLinerPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CLinerPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CLinerPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CLinerPanel construction

CLinerPanel::CLinerPanel()
{
}

CLinerPanel::~CLinerPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLinerPanel message handlers

BOOL CLinerPanel::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd)
{
	return CTaskPanel::Create( dwStyle, rect, pParentWnd, IDC_LINER_PANEL );
}

int CLinerPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	SetOwner( GetParent() );
	SetFooter( NULL, TRUE );
	
//	m_wndVideo.Create( this, 140, _T("±æµÿ ”∆µ"), IDI_TASKPANEL );
	AddBox( &wndVideo );
	
	return 0;
}
