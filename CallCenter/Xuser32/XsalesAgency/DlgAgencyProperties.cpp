//
// DlgAgencyProperties.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "DlgAgencyProperties.h"
#include "WndAgency.h"
#include "XML.h"

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

CAgencyPropertiesDlg::CAgencyPropertiesDlg(CXMLElement* pXML)
	: CDialog(CAgencyPropertiesDlg::IDD, NULL), m_pXML( pXML->Clone() )
{
	//{{AFX_DATA_INIT(CAgencyPropertiesDlg)
	//}}AFX_DATA_INIT
}

CAgencyPropertiesDlg::CAgencyPropertiesDlg(LPCTSTR xKey, CWnd* pWndParent)
: CDialog(CAgencyPropertiesDlg::IDD, pWndParent), m_xKey( xKey )
{
	//{{AFX_DATA_INIT(CAgencyPropertiesDlg)
	//}}AFX_DATA_INIT
	m_pXML = NULL;
}

CAgencyPropertiesDlg::~CAgencyPropertiesDlg()
{
	if ( m_pXML ) delete m_pXML;
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
// CAgencyPropertiesDlg attributes

_ConnectionPtr CAgencyPropertiesDlg::GetConnectionPtr() const
{
	if ( ! m_pParentWnd ) return NULL;
	
	CAgencyWnd* pAgencyWnd = (CAgencyWnd*)m_pParentWnd;
	return pAgencyWnd->m_pConnection;
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyPropertiesDlg message handlers

BOOL CAgencyPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for ( CWnd* pWnd = GetWindow( GW_CHILD ); pWnd; pWnd = pWnd->GetNextWindow() )
	{
		pWnd->SendMessage( EM_SETREADONLY, TRUE );
	}
	
	if ( ! LoadRecordset() ) EndDialog(IDCANCEL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAgencyPropertiesDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}

#if 0
BOOL CAgencyPropertiesDlg::LoadRecordset()
{
	ASSERT( m_pXML != NULL );
	
	m_wndJobs.SetWindowText( m_pXML->GetAttributeValue( "job" ) );
	
	m_wndFeeNumber.SetWindowText( m_pXML->GetElementByName( "feenumber" )->GetValue() );
	m_wndDesc.SetWindowText( m_pXML->GetElementByName( "text" )->GetValue() );
	m_wndOpening.SetWindowText( m_pXML->GetElementByName( "timeopening" )->GetValue() );
	
	COleDateTime tt = COleDateTime::GetCurrentTime();
	tt.ParseDateTime( m_pXML->GetElementByName( "timeexpired" )->GetValue() );
	
	m_wndExpired.SetTime( tt );
	
	return TRUE;
}
#else
BOOL CAgencyPropertiesDlg::LoadRecordset()
{
	_ConnectionPtr pConnection = GetConnectionPtr();
	if ( pConnection == NULL ) return FALSE;
	
	try
	{
		CString strQuery;
		strQuery.Format( "select * from xsaleinfo"
			" where xsi = '%s'", m_xKey );
		TRACE( strQuery + "\n" );
		
		_RecordsetPtr xRecordset;
		xRecordset = xExecute(pConnection, (_bstr_t)strQuery, adCmdText);
		
		_variant_t TheValue = xRecordset->GetCollect( "xjob" );
		m_wndJobs.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
		
		TheValue = xRecordset->GetCollect( "xfeenumber" );
		if ( VT_NULL != TheValue.vt ) m_wndFeeNumber.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
		
		TheValue = xRecordset->GetCollect( "xdesc" );
		if ( VT_NULL != TheValue.vt ) m_wndDesc.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
		
		TheValue = xRecordset->GetCollect( "xtimeopening" );
		if ( VT_NULL != TheValue.vt ) m_wndOpening.SetWindowText( (LPCTSTR)(_bstr_t)TheValue );
		
		TheValue = xRecordset->GetCollect( "xtimeexpired" );
		m_wndExpired.SetTime( COleDateTime( TheValue ) );
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE );
		return FALSE;
	}
	
	return TRUE;
}
#endif
