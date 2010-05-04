//
// PagePresserUser.cpp
//

#include "stdafx.h"
#include "Xcenter.h"
#include "Settings.h"
#include "Traffic.h"
#include "PagePresserUser.h"
#include "Channel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CUserPresserPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CUserPresserPage, CSettingsPage)
	//{{AFX_MSG_MAP(CUserPresserPage)
	ON_BN_CLICKED(IDC_USER_PRESS, OnUserPress)
	ON_BN_CLICKED(IDC_USER_VIEW, OnUserView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUserPresserPage property page

CUserPresserPage::CUserPresserPage() : CSettingsPage( CUserPresserPage::IDD )
{
	//{{AFX_DATA_INIT(CUserPresserPage)
	//}}AFX_DATA_INIT
}

CUserPresserPage::~CUserPresserPage()
{
}

void CUserPresserPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserPresserPage)
	DDX_Control(pDX, IDC_USER_NUMBER, m_wndUserId);
	DDX_Control(pDX, IDC_USER_EXPIRE, m_wndExpire);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CUserPresserPage message handlers

BOOL CUserPresserPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();
	
	m_wndUserId.SetCurSel( 0 );
	m_wndExpire.SetCurSel( 2 );	// 3个月以上
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserPresserPage::OnOK()
{
	CSettingsPage::OnOK();
}

void CUserPresserPage::OnUserPress()
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

BOOL CUserPresserPage::PressRecord(LPCTSTR xUserId, int nExpire)
{
	CString strPath = Settings.General.PathUsr + _T("\\Backup");
#ifdef _DEBUG
	strPath.MakeLower();
	theApp.Message( MSG_TEMP, "debug: Replace [PathUsr] with [Net Path]" );
	Replace( strPath, "d:\\", "\\\\192.168.0.2\\\\d$\\" );
#endif
	CreateDirectory( (LPCTSTR)strPath, NULL );
	
	CString strQuery;
	
	if ( xUserId == NULL )
	strQuery.Format( "select * from xuservoice"
					" where datediff(day,xRecDate,getdate()) > %i", nExpire );
	else
	strQuery.Format( "select * from xuservoice"
					" where xPhoneNum = '%s'"
					" and datediff(day,xRecDate,getdate()) > %i", xUserId, nExpire );
	TRACE( strQuery + "\n" );
	
	try
	{
		_RecordsetPtr xRecordset;
		xOpenRecordset(xRecordset, theApp.m_pConnection, (_bstr_t)strQuery);
		
		for ( ; ! xRecordset->adoEOF; xRecordset->MoveNext() )
		{
			MSG msg;
			if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg ); DispatchMessage( &msg );
			}
			
			_variant_t xPhoneNum = xRecordset->GetCollect( "xPhoneNum" );
			_variant_t xCallerId = xRecordset->GetCollect( "xCaller" );
			_variant_t xPCMName  = xRecordset->GetCollect( "xPCMName" );
			_variant_t xjsjbh    = xRecordset->GetCollect( "xjsjbh" );
			
			xRecordset->Delete( adAffectCurrent );
			
			CString xUsrFile = (LPCTSTR)(_bstr_t)xPCMName;
			xUsrFile.TrimLeft(); xUsrFile.TrimRight();
			if ( Traffic.IsMusic(xUsrFile) || IsSysFile(xUsrFile) ) continue;
			
#ifdef _DEBUG
	xUsrFile.MakeLower();
	theApp.Message( MSG_TEMP, "debug: Replace [xUsrFile] with [Net Path]" );
	Replace( xUsrFile, "d:\\", "\\\\192.168.0.2\\\\d$\\" );
#endif
			
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
			
			CString xUsrPath;
			xUsrPath = xUsrFile.Left( xUsrFile.ReverseFind( '\\' ) );
			RemoveDirectory( xUsrPath );
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

void CUserPresserPage::OnUserView()
{
	CString strPath = Settings.General.PathUsr + _T("\\Backup");
	
	ShellExecute( GetSafeHwnd(), _T("open"),
			(LPCTSTR)strPath, NULL, NULL, SW_SHOWNORMAL );
}

BOOL CUserPresserPage::IsSysFile(LPCTSTR xPath) const
{
	CString xSysPath = FT_PROMPT;
	xSysPath += '\\';
	
	return ( _tcsnistr(xPath, xSysPath, xSysPath.GetLength()) != NULL );
}
