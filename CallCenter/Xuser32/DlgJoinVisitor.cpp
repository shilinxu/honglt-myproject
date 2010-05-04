//
// DlgJoinVisitor.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "DlgJoinVisitor.h"
#include "CoolInterface.h"

#include "ChannelUser.h"

#include <afxpriv.h>
#include <afximpl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CJoinVisitorDlg, CDialog)
BEGIN_MESSAGE_MAP(CJoinVisitorDlg, CDialog)
	//{{AFX_MSG_MAP(CJoinVisitorDlg)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CJoinVisitorDlg construction

CJoinVisitorDlg::CJoinVisitorDlg(CUserChannel* pChannel) : CDialog( CJoinVisitorDlg::IDD, NULL )
{
	//{{AFX_DATA_INIT(CJoinVisitorDlg)
	//}}AFX_DATA_INIT
	
	m_pChannel	= pChannel;
	
	CreateReal( CJoinVisitorDlg::IDD);
}

CJoinVisitorDlg::~CJoinVisitorDlg()
{
	if ( m_brWhite.m_hObject != NULL ) m_brWhite.DeleteObject();
}

void CJoinVisitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJoinVisitorDlg)
	DDX_Control(pDX, IDC_PLACEHOLDER, m_wndPlaceHolder);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CJoinVisitorDlg operations

BOOL CJoinVisitorDlg::CreateReal(UINT nID)
{
	LPCTSTR lpszTemplateName = MAKEINTRESOURCE( nID );

	HINSTANCE hInst		= AfxFindResourceHandle( lpszTemplateName, RT_DIALOG );
	HRSRC hResource		= ::FindResource( hInst, lpszTemplateName, RT_DIALOG );
	HGLOBAL hTemplate	= LoadResource( hInst, hResource );

	LPCDLGTEMPLATE lpDialogTemplate = (LPCDLGTEMPLATE)LockResource( hTemplate );

	BOOL bResult = CreateDlgIndirect( lpDialogTemplate, NULL, hInst );

	UnlockResource( hTemplate );

	FreeResource( hTemplate );

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CJoinVisitorDlg message handlers

BOOL CJoinVisitorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rc;
	m_wndPlaceHolder.GetClientRect( &rc );
	
	m_wndVideo.CreateEx( this, rc, "Ô¶¶ËÊÓÆµ", IDI_TASKPANEL);

	m_crWhite = CCoolInterface::GetDialogBkColor();
	m_brWhite.CreateSolidBrush( m_crWhite );
	
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	
	CenterWindow();
	ShowWindow( SW_SHOW );
	
	return TRUE;
}


void CJoinVisitorDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	m_pChannel->m_pVisitorWnd = NULL;
}

void CJoinVisitorDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}

BOOL CJoinVisitorDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect( &rc );
	
	pDC->FillSolidRect( &rc, m_crWhite );

	return TRUE;
}

HBRUSH CJoinVisitorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor( pDC, pWnd, nCtlColor );

	if ( nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC )
	{
		pDC->SetBkColor( m_crWhite );
		hbr = m_brWhite;
	}

	return hbr;
}

void CJoinVisitorDlg::OnClose()
{
	DestroyWindow();
}
