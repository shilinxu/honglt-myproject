//
// CtrlAgencyPanel.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlAgencyPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CAgencyPanel, CTaskPanel)
BEGIN_MESSAGE_MAP(CAgencyPanel, CTaskPanel)
	//{{AFX_MSG_MAP(CAgencyPanel)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CAgencyPanel construction

CAgencyPanel::CAgencyPanel()
{
}

CAgencyPanel::~CAgencyPanel()
{
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyPanel message handlers

BOOL CAgencyPanel::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd)
{
	return CTaskPanel::Create( dwStyle, rect, pParentWnd, IDC_AGENCY_PANEL );
}

int CAgencyPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CTaskPanel::OnCreate( lpCreateStruct ) == -1 )return -1;
	
	SetOwner( GetParent() );
	SetFooter( NULL, TRUE );
	
//	m_wndVideo.Create( this, 140, _T("±æµÿ ”∆µ"), IDI_TASKPANEL );
	AddBox( &wndVideo );
	
	return 0;
}
