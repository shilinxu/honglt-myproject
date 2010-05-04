//
// CtrlHomeForm.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CtrlHomeForm.h"
#include "CoolInterface.h"
#include "Registry.h"

#include <afxpriv.h>
#include <afximpl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CHomeForm, CDialog)
BEGIN_MESSAGE_MAP(CHomeForm, CDialog)
	//{{AFX_MSG_MAP(CHomeForm)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_USER_ID, OnSelChangeUserId)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifdef _DEBUG
#define MAX_ITEM	1
#else
#define MAX_ITEM	10
#endif

//////////////////////////////////////////////////////////////////////
// CHomeForm construction

CHomeForm::CHomeForm() : CDialog( CHomeForm::IDD, NULL )
{
	//{{AFX_DATA_INIT(CHomeForm)
	//}}AFX_DATA_INIT
	
	if ( m_lpszTemplateName != NULL ) LoadDefaultCaption();
	m_brWhite.CreateSolidBrush( CCoolInterface::GetDialogBkColor() );
}

CHomeForm::~CHomeForm()
{
}

void CHomeForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHomeForm)
	DDX_Control(pDX, IDC_USER_ID, m_wndUserId);
	DDX_Control(pDX, IDC_PASSWORD, m_wndPassword);
	DDX_Control(pDX, IDC_SAVE_PASSWORD, m_wndSaveing);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CHomeForm operations

BOOL CHomeForm::LoadDefaultCaption()
{
	CDialogTemplate pTemplate;
	DLGTEMPLATE* pData;
	LPWORD pWord;

	if ( ! pTemplate.Load( m_lpszTemplateName ) ) return FALSE;

	pData = (DLGTEMPLATE*)GlobalLock( pTemplate.m_hTemplate );

	if ( ((DLGTEMPLATEEX*)pData)->signature == 0xFFFF )
	{
		pWord = (WORD*)( (DLGTEMPLATEEX*)pData + 1 );
	}
	else
	{
		pWord = (WORD*)( pData + 1 );
	}
	if ( *pWord == 0xFFFF )
	{
		pWord += 2;
	}
	else
	{
		while ( *pWord++ );
	}

	if ( *pWord == 0xFFFF )
	{
		pWord += 2;
	}
	else
	{
		while ( *pWord++ );
	}

	m_strCaption = (wchar_t*)pWord;

	GlobalUnlock( pTemplate.m_hTemplate );

	return m_strCaption.GetLength() > 0;
}

BOOL CHomeForm::Create(const RECT& rect, CWnd* pParentWnd)
{
	ASSERT_VALID(this);
	ASSERT( m_lpszTemplateName != NULL );

	CDialogTemplate pTemplate;
	LPDLGTEMPLATE pData;

	if ( ! pTemplate.Load( m_lpszTemplateName ) ) return FALSE;
	pData = (LPDLGTEMPLATE)GlobalLock( pTemplate.m_hTemplate );
	
	DWORD dwExStyle = WS_EX_LTRREADING|WS_EX_LEFT|WS_EX_RIGHTSCROLLBAR;

	if ( ((DLGTEMPLATEEX*)pData)->signature == 0xFFFF )
	{
		DLGTEMPLATEEX* pEx = (DLGTEMPLATEEX*)pData;
		pEx->style		= WS_CHILDWINDOW|WS_OVERLAPPED|DS_3DLOOK|DS_SETFONT|DS_CONTROL;
		pEx->exStyle	= dwExStyle|WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT;
	}
	else
	{
		pData->style			= WS_CHILDWINDOW|WS_OVERLAPPED|DS_3DLOOK|DS_SETFONT|DS_CONTROL;
		pData->dwExtendedStyle	= dwExStyle|WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT;
	}
	
	GlobalUnlock( pTemplate.m_hTemplate );
	CreateIndirect( pTemplate.m_hTemplate, pParentWnd );
	SetFont( &CoolInterface.m_fntNormal );

	MoveWindow( &rect );

	return ( m_hWnd != NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CHomeForm message handlers

BOOL CHomeForm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LoadHistory();
	
	m_wndUserId.SetCurSel( 0 );
	SendMessage(WM_COMMAND, MAKELONG(m_wndUserId.GetDlgCtrlID(),CBN_SELCHANGE), (LPARAM)m_hWnd);
	
	return TRUE;
}

BOOL CHomeForm::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


HBRUSH CHomeForm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH)CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
	
	if ( nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC )
	{
		pDC->SetBkColor( CoolInterface.GetDialogBkColor() );
		hbr = m_brWhite;
	}
	
	return hbr;
}

int CHomeForm::LoadHistory()
{
	CRegistry pRegistry;
	int nCookie = pRegistry.GetInt( _T("Connect"), _T("Cookie") );
	
	for ( int nItem = 0; nItem < MAX_ITEM; nItem++, nCookie -= nCookie > 0 ? 1 : -MAX_ITEM+1 )
	{
		CString strKey;
		strKey.Format( _T("usr%i"), UINT(nCookie) % MAX_ITEM );
		
		CString strValue = pRegistry.GetString( _T("Connect"), strKey );
		if ( strValue.IsEmpty() ) break;
		
		strValue	+= '|';
		CString str	= strValue.SpanExcluding( _T("|;,") );
		InsertItem( nItem, (LPCTSTR)str, nCookie );
	}
	
	return nItem;
}

void CHomeForm::SaveHistory()
{
	CString xUserId;
	m_wndUserId.GetWindowText( xUserId );
	if ( xUserId.IsEmpty() ) return;
	
	CRegistry pRegistry;
	int nCookie = pRegistry.GetInt( _T("Connect"), _T("Cookie"), -1 );
	nCookie = ++nCookie % MAX_ITEM;
	
	InsertItem( 0, xUserId, nCookie );
	
	CString strKey;
	strKey.Format( _T("usr%i"), nCookie );
	
	CString strValue = xUserId;
	if ( m_wndSaveing.GetCheck() )
	{
		CString str;
		m_wndPassword.GetWindowText( str );
		strValue += '|'; strValue += str;
	}
	
	pRegistry.SetString( _T("Connect"), strKey, strValue );
	pRegistry.SetInt( _T("Connect"), _T("Cookie"), nCookie );
}

int CHomeForm::InsertItem(int nIndex, LPCTSTR xUserId, DWORD nData)
{
	int nItem = m_wndUserId.FindString( -1, xUserId );
	if ( nItem != -1 ) RemoveItem( nItem );
	
	if ( m_wndUserId.GetCount() >= MAX_ITEM )
		RemoveItem( MAX_ITEM - 1 );
	
	nItem = m_wndUserId.InsertString( nIndex, xUserId );
	m_wndUserId.SetItemData( nItem, nData );
	
	return nItem;
}

void CHomeForm::RemoveItem(int nItem)
{
	int nIndex = m_wndUserId.GetItemData( nItem );
	m_wndUserId.DeleteString( nItem );
	
	CString strKey;
	strKey.Format( _T("usr%i"), nIndex );
	
	CRegistry pRegistry;
	pRegistry.RemoveValue( _T("Connect"), strKey );
}

void CHomeForm::OnSelChangeUserId()
{
	int nItem = m_wndUserId.GetCurSel();
	if ( nItem < 0 ) return;
	
	CString strKey;
	DWORD nIndex = m_wndUserId.GetItemData( nItem );
	strKey.Format( _T("usr%i"), nIndex );
	
	CRegistry pRegistry;
	CString strValue = pRegistry.GetString( _T("Connect"), strKey );
	strValue		+= '|';
	
	CString str		= strValue.SpanExcluding( _T("|;,") );
	strValue		= strValue.Mid( str.GetLength() + 1 );
	
	if ( strValue.IsEmpty() == FALSE )
	{
		CString xPwd= strValue.SpanExcluding( _T("|;,") );
		strValue	= strValue.Mid( xPwd.GetLength() + 1 );
		
		m_wndPassword.SetWindowText( (LPCTSTR)xPwd );
		m_wndSaveing.SetCheck( TRUE );
	}
	else
	{
		m_wndPassword.SetWindowText( _T("") );
	}
}

void CHomeForm::OnOK() 
{
	SaveHistory();
}

void CHomeForm::OnCancel()
{
}
