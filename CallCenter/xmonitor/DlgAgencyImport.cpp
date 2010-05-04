//
// DlgAgencyImport.cpp
//

#include "stdafx.h"
#include "Xmonitor.h"
#include "DlgAgencyImport.h"

using namespace Excel ;
//using namespace Office;
//using namespace VBIDE;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAgencyImportDlg, CDialog)
	//{{AFX_MSG_MAP(CAgencyImportDlg)
	ON_BN_CLICKED(IDC_PATH_BROWSE, OnPathBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgencyImportDlg construction

CAgencyImportDlg::CAgencyImportDlg(LPCTSTR xsaleinfo)
	: CDialog(CAgencyImportDlg::IDD, NULL), m_xsaleinfo( _tcsdup(xsaleinfo) )
{
	//{{AFX_DATA_INIT(CAgencyImportDlg)
	//}}AFX_DATA_INIT
}

CAgencyImportDlg::~CAgencyImportDlg()
{
	free( m_xsaleinfo ); m_xsaleinfo = NULL;
}

void CAgencyImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAgencyImportDlg)
	DDX_Control(pDX, IDC_PATH_IMPORT, m_wndPath);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CAgencyImportDlg message handlers

void CAgencyImportDlg::OnPathBrowse() 
{
	CFileDialog dlg( TRUE, _T(".xls"), NULL, OFN_HIDEREADONLY,
		_T("Excel Files|*.xls|All Files|*.*||"), this );
	
	if ( dlg.DoModal() != IDOK ) return;
	m_wndPath.SetWindowText( dlg.GetPathName() );
}

void CAgencyImportDlg::OnOK() 
{
	CString strFile;
	m_wndPath.GetWindowText( strFile );
	
	try
	{
		CWaitCursor pCursor;
		
		if ( ! ImportFromExcel( strFile ) ) return;
	}
	catch (_com_error e)
	{
		AfxMessageBox( IDS_ERROR_DATABASE );
		return;
	}
	
	CDialog::OnOK();
}

BOOL CAgencyImportDlg::ImportFromExcel(LPCTSTR lpszFile)
{
	_ApplicationPtr pApplication;
	pApplication.CreateInstance( "Excel.Application" );
	pApplication->PutVisible( 0, VARIANT_FALSE );
	
	_WorkbookPtr pWorkbook;
	pWorkbook = pApplication->GetWorkbooks()->Open( lpszFile );
	
	CString strQuery;
	strQuery.Format( "select * from xsale where 1 = 0" );
	
	TRACE( strQuery + "\n" );
	_RecordsetPtr xRecordset;
	xOpenRecordset(xRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
	
	for ( int nSheet = 1; nSheet <= pWorkbook->Sheets->Count; nSheet++ )
	{
		_WorksheetPtr pWorksheet;
		pWorksheet = pWorkbook->Sheets->GetItem( (long)nSheet );
		
		ImportFromWorksheet( xRecordset, pWorksheet );
	}
	
	xRecordset->UpdateBatch( adAffectAll );
	return TRUE;
}

void CAgencyImportDlg::ImportFromWorksheet(_RecordsetPtr xRecordset, _WorksheetPtr pWorksheet)
{
	int nSalIdIndex = GetSalesIdIndex();
	CTime pNow = CTime::GetCurrentTime();
	RangePtr pRange = pWorksheet->GetUsedRange();
	
	for ( int nRow = 1; nRow <= pRange->GetRows()->Count; nRow++ )
	{
		xRecordset->AddNew();
		xRecordset->PutCollect( "xsaleinfo", _variant_t(m_xsaleinfo) );
		
		CString strValue;
		strValue.Format( "SAL%.4i%.2i%.2i%.6i", 
					pNow.GetYear(), pNow.GetMonth(), pNow.GetDay(), nSalIdIndex++ );
		xRecordset->PutCollect( "xsale", _variant_t(strValue) );
		
		_variant_t TheValue = pRange->GetItem( (short)nRow, (short)1 );
		xRecordset->PutCollect( "xphone", TheValue );
		
		TheValue = pRange->GetItem( (short)nRow, (short)2 );
		xRecordset->PutCollect( "xuserid", TheValue );
		
		TheValue = pRange->GetItem( (short)nRow, (short)3 );
		xRecordset->PutCollect( "xdatetime", TheValue );
		
		TheValue = pRange->GetItem( (short)nRow, (short)4 );
		xRecordset->PutCollect( "xstate", TheValue );
		
		TheValue = pRange->GetItem( (short)nRow, (short)5 );
		xRecordset->PutCollect( "xreason", TheValue );
	}
	
	xRecordset->UpdateBatch( adAffectAll );
}

int CAgencyImportDlg::GetSalesIdIndex() const
{
	CString strQuery;
	CTime pNow = CTime::GetCurrentTime();
	
	strQuery.Format( "select max(cast(right(xsale,6) as int))"
					" from xsale where len(xsale)=17 and substring(xsale,4,8)='%.4i%.2i%.2i'",
					pNow.GetYear(), pNow.GetMonth(), pNow.GetDay() );
	
	_RecordsetPtr xRecordset;
	xRecordset = xExecute(theApp.m_pConnection, (_bstr_t)strQuery, adCmdText);
	_variant_t TheValue = xRecordset->Fields->GetItem( _variant_t(long(0)) )->Value;
	
	return ( TheValue.vt == VT_NULL ) ? 1 : TheValue.lVal + 1;
}
