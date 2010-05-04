//
// CtrlUserPanel.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlUserPanel.h"
#include "CtrlNetworkPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CUserPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CUserPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CUserPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CUserPanel construction

CUserPanel::CUserPanel()
{
}

CUserPanel::~CUserPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CUserPanel message handlers

BOOL CUserPanel::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd)
{
	return CTaskPanel::Create( dwStyle, rect, pParentWnd, IDC_USER_PANEL );
}

int CUserPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	SetOwner( GetParent() );
	SetFooter( NULL, TRUE );
	
//	m_wndVideo.Create( this, 140, _T("±æµÿ ”∆µ"), IDI_TASKPANEL );
	AddBox( &wndVideo );
	
	return 0;
}
