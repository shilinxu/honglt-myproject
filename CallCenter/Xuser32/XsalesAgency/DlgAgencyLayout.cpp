//
// DlgAgencyLayout.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "DlgAgencyLayout.h"

#include "Network.h"
#include "WndAgency.h"
#include "XML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAgencyLayoutDlg, CDialog)
	//{{AFX_MSG_MAP(CAgencyLayoutDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_STARTUP, OnStartup)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PACKETREQ, OnPacketReq)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgencyLayoutDlg construction

CAgencyLayoutDlg::CAgencyLayoutDlg(CXMLElement* pXML, CXMLElement* pJobs)
	: CDialog(CAgencyLayoutDlg::IDD, NULL), m_pXML( pXML ), m_pJobs( pJobs )
{
	//{{AFX_DATA_INIT(CAgencyLayoutDlg)
	//}}AFX_DATA_INIT
}
CAgencyLayoutDlg::CAgencyLayoutDlg(LPCTSTR hKey, CWnd* pParentWnd)
: CDialog(CAgencyLayoutDlg::IDD, pParentWnd), m_hKey( hKey )
{
	//{{AFX_DATA_INIT(CAgencyLayoutDlg)
	//}}AFX_DATA_INIT
}

void CAgencyLayoutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAgencyLayoutDlg)
	DDX_Control(pDX, IDC_DESC, m_wndDesc);
	DDX_Control(pDX, IDC_PHONE, m_wndPhone);
	DDX_Control(pDX, IDC_STATE, m_wndState);
	DDX_Control(pDX, IDC_REASON, m_wndReason);
	DDX_Control(pDX, IDC_STARTUP, m_wndStartup);
	DDX_Control(pDX, IDOK, m_wndOK);
	//}}AFX_DATA_MAP
}

_ConnectionPtr CAgencyLayoutDlg::GetConnectionPtr() const
{
	if ( ! m_pParentWnd ) return NULL;
	
	CAgencyWnd* pAgencyWnd = (CAgencyWnd*)m_pParentWnd;
	return pAgencyWnd->m_pConnection;
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyLayoutDlg message handlers

BOOL CAgencyLayoutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	this->m_pMainWnd = theApp.m_pMainWnd;
	if ( theApp.m_pMainWnd ) theApp.m_pMainWnd	= this;
	
	if ( ! LoadRecordset() ) EndDialog(IDCANCEL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAgencyLayoutDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	Network.HangRemote();
	theApp.m_pMainWnd = this->m_pMainWnd;
}
#if 0
BOOL CAgencyLayoutDlg::LoadRecordset()
{
	ASSERT( m_pXML != NULL && m_pJobs != NULL );
	
	m_wndDesc.SetWindowText( m_pJobs->GetElementByName( "text" )->GetValue() );
	
	m_wndPhone.SetWindowText( m_pXML->GetAttributeValue( "xphone" ) );
	
	m_wndState.SetWindowText( m_pXML->GetElementByName( "xstate", TRUE )->GetValue() );
	m_wndReason.SetWindowText( m_pXML->GetElementByName( "xreason", TRUE )->GetValue() );
	
	return TRUE;
}

void CAgencyLayoutDlg::OnOK()
{
	ASSERT( m_pXML != NULL );
	CString strValue;
	
	m_wndState.GetWindowText( strValue );
	m_pXML->GetElementByName( "xstate", TRUE )->SetValue( strValue );
	
	m_wndReason.GetWindowText( strValue );
	m_pXML->GetElementByName( "xreason", TRUE )->SetValue( strValue );
	
	CDialog::OnOK();
}
#else
BOOL CAgencyLayoutDlg::LoadRecordset()
{
	try
	{
		CString strQuery;
		strQuery.Format( "select a.*, b.xjob,b.xdesc,b.xtimeopening,b.xtimeexpired,b.xfeenumber"
			" from xsale a inner join xsaleinfo b on a.xsaleinfo = b.xsi"
			" where a.xsale = '%s'", m_hKey );
		TRACE( strQuery + "\n" );
		
		xOpenRecordset(m_pRecordset, GetConnectionPtr(), (_bstr_t)strQuery);
		//xRecordset = xExecute(GetConnectionPtr(), (_bstr_t)strQuery, adCmdText);
		
		_variant_t TheValue = m_pRecordset->GetCollect( "xdesc" );
		m_wndDesc.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
		
		TheValue = m_pRecordset->GetCollect( "xphone" );
		if ( VT_NULL != TheValue.vt ) m_wndPhone.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
		
		TheValue = m_pRecordset->GetCollect( "xstate" );
		if ( VT_NULL != TheValue.vt ) m_wndState.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
		
		TheValue = m_pRecordset->GetCollect( "xreason" );
		if ( VT_NULL != TheValue.vt ) m_wndReason.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE );
		return FALSE;
	}
	return TRUE;
}

void CAgencyLayoutDlg::OnOK()
{
	try
	{
		CString strValue;
		m_wndState.GetWindowText( strValue );
		m_pRecordset->PutCollect( "xstate", _variant_t(strValue) );
			
		m_wndReason.GetWindowText( strValue );
		m_pRecordset->PutCollect( "xreason", _variant_t(strValue) );
		
		m_pRecordset->Update();
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE );
		return;
	}
	
	CDialog::OnOK();
}
#endif

void CAgencyLayoutDlg::OnStartup()
{
	_variant_t TheValue = m_pRecordset->GetCollect( "xfeenumber" );
	CString xLocalId = (LPCTSTR)(_bstr_t)TheValue;
	
	TheValue = m_pRecordset->GetCollect( "xphone" );
	CString xRemoteId = (LPCTSTR)(_bstr_t)TheValue;
	
	Network.CallRemote( xRemoteId, xLocalId );
	
	EnableMemvar( FALSE );
}

void CAgencyLayoutDlg::EnableMemvar(BOOL bEnable)
{
	m_wndStartup.EnableWindow( bEnable );
	m_wndOK.EnableWindow( bEnable );
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyLayoutDlg custom message handlers

LONG CAgencyLayoutDlg::OnPacketReq(WPARAM wParam, LPARAM lParam)
{
	CPacket* pPacket = (CPacket*)wParam;
	
	if ( pPacket && pPacket->IsCommandId("OK") )
	{
	}
	if ( pPacket && pPacket->IsCommandId("ERR") )
	{
		int nError = ((CErrPacket*)pPacket)->m_nErrCode;
		
		CString str;
		str.Format( IDS_NETWORK_CONNECT_ERROR, nError );
		
		AfxMessageBox( str );
		EnableMemvar();
	}
	
	if ( (BOOL)lParam == TRUE )
	pPacket->Release(); return 0;
}
