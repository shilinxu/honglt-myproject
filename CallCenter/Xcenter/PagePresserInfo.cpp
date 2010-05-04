//
// PagePresserInfo.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "Traffic.h"
#include "PagePresserInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CInfoPresserPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CInfoPresserPage, CSettingsPage)
	//{{AFX_MSG_MAP(CInfoPresserPage)
	ON_BN_CLICKED(IDC_USER_PRESS, OnUserPress)
	ON_BN_CLICKED(IDC_USER_VIEW, OnUserView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInfoPresserPage property page

CInfoPresserPage::CInfoPresserPage() : CSettingsPage( CInfoPresserPage::IDD )
{
	//{{AFX_DATA_INIT(CInfoPresserPage)
	//}}AFX_DATA_INIT
}

CInfoPresserPage::~CInfoPresserPage()
{
}

void CInfoPresserPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPresserPage)
	DDX_Control(pDX, IDC_USER_NUMBER, m_wndUserId);
	DDX_Control(pDX, IDC_USER_EXPIRE, m_wndExpire);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CInfoPresserPage message handlers

BOOL CInfoPresserPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndUserId.SetCurSel( 0 );
	m_wndExpire.SetCurSel( 2 );	// 3个月以上
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInfoPresserPage::OnOK()
{
	CSettingsPage::OnOK();
}

void CInfoPresserPage::OnUserPress()
{
	CString xUserId;
	m_wndUserId.GetWindowText( xUserId );
	xUserId.TrimLeft(); xUserId.TrimRight();
	if ( xUserId.IsEmpty() ) return;
	
	int nExpire = m_wndExpire.GetCurSel();
	if ( nExpire == 0 ) nExpire = 30;
	else if ( nExpire == 1 ) nExpire = 60;
	else if ( nExpire == 2 ) nExpire = 90;
	else if ( nExpire == 3 ) nExpire = 180;
	else if ( nExpire == 4 ) nExpire = 365;
	else if ( nExpire == 5 ) nExpire = -1;
	else if ( nExpire == -1 ) return;
	
	CString xExpire;
	m_wndExpire.GetWindowText( xExpire ); 
	
	CString str;
	str.Format( "是否清理\"%s\"号码的\"%s\"的所有项目?",
				xUserId, xExpire );
	if (AfxMessageBox(str, MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDCANCEL)
	{
		return;
	}
	
	CWaitCursor pCursor;
	
	if ( xUserId == "全部" ) PressRecord(NULL, nExpire);
	else PressRecord( (LPCTSTR)xUserId, nExpire );
}

BOOL CInfoPresserPage::PressRecord(LPCTSTR xUserId, int nExpire)
{
	CString strPath = Settings.General.PathUsr + _T("\\Backup");
	CreateDirectory( (LPCTSTR)strPath, NULL );
	
	CString strQuery;
	
	if ( xUserId == NULL )
	strQuery.Format( "select * from Tel_Record"
					" where datediff(day,RecDate,getdate()) > %i", nExpire );
	else
	strQuery.Format( "select * from Tel_Record"
					" where PhoneNum = '%s'"
					" and datediff(day,RecDate,getdate()) > %i", xUserId, nExpire );
	
	try
	{
		_RecordsetPtr Recordvox = xExecute(theApp.m_pConnection2, (_bstr_t)strQuery, adCmdText);
		TRACE( strQuery + "\n" );
		
		for ( ; ! Recordvox->adoEOF; Recordvox->MoveNext() )
		{
			MSG msg;
			if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg ); DispatchMessage( &msg );
			}
			
			_variant_t xPhoneNum = Recordvox->GetCollect( "PhoneNum" );
			_variant_t xCallerId = Recordvox->GetCollect( "Caller" );
			_variant_t xPCMName  = Recordvox->GetCollect( "PCMName" );
			_variant_t xjsjbh    = Recordvox->GetCollect( "inID" );
			
			strQuery.Format( "delete Tel_Record"
							" where inID = %i", xjsjbh.lVal );
			TRACE( strQuery + "\n" );
			
			xExecute(theApp.m_pConnection2, (_bstr_t)strQuery, adExecuteNoRecords);
			
			CString xUsrFile = (LPCTSTR)(_bstr_t)xPCMName;
			xUsrFile.TrimLeft(); xUsrFile.TrimRight();
			if ( Traffic.IsMusic(xUsrFile) ) continue;
			
			CString xNewFile = "";
			
			if ( xPhoneNum.vt != VT_NULL ) xNewFile += (LPCTSTR)(_bstr_t)xPhoneNum;
			xNewFile.TrimLeft(); xNewFile.TrimRight();
			xNewFile += '_';
			if ( xCallerId.vt != VT_NULL ) xNewFile += (LPCTSTR)(_bstr_t)xCallerId;
			xNewFile.TrimLeft(); xNewFile.TrimRight();
			xNewFile += '_';
			
			xNewFile += xUsrFile.Mid( xUsrFile.ReverseFind( '\\' ) + 1 );
			xNewFile  =	strPath + '\\' + xNewFile;
			
			MoveFile( xUsrFile, xNewFile );
		}
	}
	catch(_com_error e) 
	{
		CString str = "数据库操作失败: ";
		str += e.ErrorMessage();
		AfxMessageBox( str );
		return FALSE;
	}
	
	return TRUE;
}

void CInfoPresserPage::OnUserView()
{
	CString strPath = Settings.General.PathUsr + _T("\\Backup");
	
	ShellExecute( GetSafeHwnd(), _T("open"),
			(LPCTSTR)strPath, NULL, NULL, SW_SHOWNORMAL );
}
