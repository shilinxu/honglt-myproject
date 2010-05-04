//
// PageProfileTalker.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "XML.h"

#include "PageProfileTalker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTalkerProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CTalkerProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CTalkerProfilePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTalkerProfilePage property page

CTalkerProfilePage::CTalkerProfilePage(CXMLElement* pXML)
	: CSettingsPage( CTalkerProfilePage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CTalkerProfilePage)
	//}}AFX_DATA_INIT
}

CTalkerProfilePage::~CTalkerProfilePage()
{
}

void CTalkerProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTalkerProfilePage)
	DDX_Control(pDX, IDC_FIRST_LENGTH, m_wndLeng1st);
	DDX_Control(pDX, IDC_FIRST_COST, m_wndCost1st);
	DDX_Control(pDX, IDC_FIRST_LENGTH2, m_wndLeng1st2);
	DDX_Control(pDX, IDC_FIRST_COST2, m_wndCost1st2);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTalkerProfilePage message handlers

BOOL CTalkerProfilePage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	if ( m_pXML ) LoadXML();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTalkerProfilePage::OnOK()
{
	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CTalkerProfilePage::LoadXML()
{
	CXMLElement* pXML = m_pXML->GetElementByName( "Local", TRUE );
	
	CString strValue = pXML->GetAttributeValue( "FirstLength", "60" );
	m_wndLeng1st.SetWindowText( (LPCTSTR)strValue );
	
	strValue = pXML->GetAttributeValue( "FirstCost", "0" );
	m_wndCost1st.SetWindowText( (LPCTSTR)strValue );
	
	pXML = m_pXML->GetElementByName( "Remote", TRUE );
	
	strValue = pXML->GetAttributeValue( "FirstLength", "60" );
	m_wndLeng1st2.SetWindowText( (LPCTSTR)strValue );
	
	strValue = pXML->GetAttributeValue( "FirstCost", "0" );
	m_wndCost1st2.SetWindowText( (LPCTSTR)strValue );

	return TRUE;
}

BOOL CTalkerProfilePage::EditXML() 
{
	CXMLElement* pXML = m_pXML->GetElementByName( "Local", TRUE );
	CString strValue;
	
	m_wndLeng1st.GetWindowText( strValue );
	pXML->AddAttribute( _T("FirstLength"), strValue );
	
	m_wndCost1st.GetWindowText( strValue );
	pXML->AddAttribute( _T("FirstCost"), strValue );

	pXML = m_pXML->GetElementByName( "Remote", TRUE );
	
	m_wndLeng1st2.GetWindowText( strValue );
	pXML->AddAttribute( _T("FirstLength"), strValue );
	
	m_wndCost1st2.GetWindowText( strValue );
	pXML->AddAttribute( _T("FirstCost"), strValue );

	return TRUE;
}
