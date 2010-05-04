//
// DlgAgencyProperties.cpp
//

#include "stdafx.h"
#include "xmonitor.h"
#include "DlgAgencyProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAgencyPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CAgencyPropertiesDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgencyPropertiesDlg construction

CAgencyPropertiesDlg::CAgencyPropertiesDlg(LPCTSTR xsaleinfo)
	: CDialog(CAgencyPropertiesDlg::IDD, NULL), m_xsaleinfo( _tcsdup(xsaleinfo) )
{
	//{{AFX_DATA_INIT(CAgencyPropertiesDlg)
	//}}AFX_DATA_INIT
}

CAgencyPropertiesDlg::~CAgencyPropertiesDlg()
{
	free( m_xsaleinfo ); m_xsaleinfo = NULL;
}

void CAgencyPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAgencyPropertiesDlg)
	DDX_Control(pDX, IDC_EXPIRED, m_wndExpired);
	DDX_Control(pDX, IDC_OPENING, m_wndOpening);
	DDX_Control(pDX, IDC_DESC, m_wndDesc);
	DDX_Control(pDX, IDC_JOBS, m_wndJobs);
	DDX_Control(pDX, IDC_FEE_NUMBER, m_wndFeeNumber);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyPropertiesDlg message handlers

BOOL CAgencyPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( ! LoadRecordset() ) EndDialog(IDCANCEL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAgencyPropertiesDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	m_pRecordset->CancelUpdate();
}

void CAgencyPropertiesDlg::OnOK() 
{
	if ( ! SaveRecordset() ) return;
	
	CDialog::OnOK();
}

BOOL CAgencyPropertiesDlg::LoadRecordset()
{
	CString strQuery;
	
	strQuery.Format( "select * from xsaleinfo"
		" where xsi = '%s'", m_xsaleinfo );
	TRACE( strQuery + "\n" );
	
	try
	{
	xOpenRecordset(m_pRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	if ( m_pRecordset->adoEOF ) m_pRecordset->AddNew();
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xjob" );
	if ( TheValue.vt != VT_NULL ) m_wndJobs.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xfeenumber" );
	if ( TheValue.vt != VT_NULL ) m_wndFeeNumber.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xdesc" );
	if ( TheValue.vt != VT_NULL ) m_wndDesc.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xtimeopening" );
	if ( TheValue.vt != VT_NULL ) m_wndOpening.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
	
//	TheValue = m_pRecordset->GetCollect( "xuserid" );
//	if ( TheValue.vt != VT_NULL ) m_wndUserId.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
	
	TheValue = m_pRecordset->GetCollect( "xtimeexpired" );
	if ( TheValue.vt != VT_NULL ) m_wndExpired.SetTime( (COleDateTime)TheValue );
	else m_wndExpired.SetTime( COleDateTime::GetCurrentTime() + COleDateTimeSpan(365, 0, 0, 0) );
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE ); return FALSE;
		return FALSE;
	}
	
	return TRUE;
}

BOOL CAgencyPropertiesDlg::SaveRecordset()
{
	try
	{
	CString strValue;
	
	m_wndJobs.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	m_pRecordset->PutCollect( "xjob", _variant_t(strValue) );
	
	m_wndFeeNumber.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	m_pRecordset->PutCollect( "xfeenumber", _variant_t(strValue) );
	
	m_wndDesc.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	m_pRecordset->PutCollect( "xdesc", _variant_t(strValue) );
	
	m_wndOpening.GetWindowText( strValue );
	strValue.TrimLeft(); strValue.TrimRight();
	m_pRecordset->PutCollect( "xtimeopening", _variant_t(strValue) );
	
//	m_wndUserId.GetWindowText( strValue );
//	strValue.TrimLeft(); strValue.TrimRight();
//	m_pRecordset->PutCollect( "xuserid", _variant_t(strValue) );
	
	COleDateTime tt; m_wndExpired.GetTime(tt);
	m_pRecordset->PutCollect( "xtimeexpired", _variant_t(tt) );
	
	_variant_t TheValue = m_pRecordset->GetCollect( "xsi" );
	if ( VT_EMPTY == TheValue.vt )
	{
		CString strJobId; GetJobId( strJobId );
		m_pRecordset->PutCollect( "xsi", _variant_t(strJobId) );
	}
	
	m_pRecordset->Update();
	
	m_pRecordset->Resync( adAffectAll, adResyncAllValues );
	TheValue = m_pRecordset->GetCollect( "xsi" );
	
	free( m_xsaleinfo ); m_xsaleinfo = NULL;
	m_xsaleinfo = _tcsdup( (LPCTSTR)(_bstr_t)TheValue );
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE ); return FALSE;
		return FALSE;
	}
	
	return TRUE;
}

BOOL CAgencyPropertiesDlg::GetJobId(CString& strJobId)
{
	CString strQuery;
	CTime pNow = CTime::GetCurrentTime();
	
	strQuery.Format( "select max(cast(right(xsi,6) as int))"
					" from xsaleinfo where len(xsi)=16 and substring(xsi,3,8)='%.4i%.2i%.2i'",
					pNow.GetYear(), pNow.GetMonth(), pNow.GetDay() );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	_variant_t TheValue = xRecordset->Fields->GetItem( _variant_t(long(0)) )->Value;
	
	strJobId.Format( "SI%.4i%.2i%.2i%.6i", 
					pNow.GetYear(), pNow.GetMonth(), pNow.GetDay(),
					( TheValue.vt == VT_NULL ) ? 1 : TheValue.lVal + 1 );
	
	return TRUE;
}
