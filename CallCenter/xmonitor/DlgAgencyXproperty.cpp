//
// DlgAgencyXproperty.cpp
//

#include "stdafx.h"
#include "xmonitor.h"
#include "DlgAgencyXproperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAgencyXpropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CAgencyXpropertyDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgencyXpropertyDlg construction

CAgencyXpropertyDlg::CAgencyXpropertyDlg(LPCTSTR xsale)
	: CDialog(CAgencyXpropertyDlg::IDD, NULL), m_xsale( xsale )
{
	//{{AFX_DATA_INIT(CAgencyXpropertyDlg)
	//}}AFX_DATA_INIT
}

void CAgencyXpropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAgencyXpropertyDlg)
	DDX_Control(pDX, IDC_USERID, m_wndUserId);
	DDX_Control(pDX, IDC_DATETIME, m_wndTime);
	DDX_Control(pDX, IDC_STATE, m_wndState);
	DDX_Control(pDX, IDC_DESC, m_wndDesc);
	DDX_Control(pDX, IDC_PHONE, m_wndPhone);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyXpropertyDlg message handlers

BOOL CAgencyXpropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( ! LoadRecordset() ) EndDialog(IDCANCEL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAgencyXpropertyDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CAgencyXpropertyDlg::OnOK() 
{
	if ( ! SaveRecordset() ) return;
	
	CDialog::OnOK();
}

BOOL CAgencyXpropertyDlg::LoadRecordset()
{
	CString strQuery;
	
	strQuery.Format( "select * from xsale"
		" where xsale = '%s'", m_xsale );
	TRACE( strQuery + "\n" );
	
	try
	{
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) return FALSE;
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xphone" );
	if ( TheValue.vt != VT_NULL ) m_wndPhone.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xuserid" );
	if ( TheValue.vt != VT_NULL ) m_wndUserId.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xstate" );
	if ( TheValue.vt != VT_NULL ) m_wndState.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xreason" );
	if ( TheValue.vt != VT_NULL ) m_wndDesc.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xdatetime" );
	if ( TheValue.vt != VT_NULL ) m_wndTime.SetTime( (COleDateTime)TheValue );
	else m_wndTime.SetTime( COleDateTime(1900,1,1, 0,0,0) );
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE ); return FALSE;
		return FALSE;
	}
	
	return TRUE;
}

BOOL CAgencyXpropertyDlg::SaveRecordset()
{
	try
	{
	CString strValue;
	
	m_wndPhone.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	m_pRecordset->PutCollect( "xphone", _variant_t(strValue) );
	
	m_wndUserId.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	m_pRecordset->PutCollect( "xuserid", _variant_t(strValue) );
	
	m_wndState.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	m_pRecordset->PutCollect( "xstate", _variant_t(strValue) );
	
	m_wndDesc.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	m_pRecordset->PutCollect( "xreason", _variant_t(strValue) );
	
	COleDateTime tt; m_wndTime.GetTime(tt);
	m_pRecordset->PutCollect( "xdatetime", _variant_t(tt) );
	
	m_pRecordset->Update();
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE ); return FALSE;
		return FALSE;
	}
	
	return TRUE;
}
