//
// PageSettingsRuler.cpp
//

#include "stdafx.h"
#include "Xmaster.h"
#include "XML.h"

#include "PageSettingsRuler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CRulerSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CRulerSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CRulerSettingsPage)
	ON_CBN_EDITCHANGE(IDC_EXT_LIST, OnEditChangeExtList)
	ON_CBN_SELCHANGE(IDC_EXT_LIST, OnSelChangeExtList)
	ON_BN_CLICKED(IDC_EXT_ADD, OnExtAdd)
	ON_BN_CLICKED(IDC_EXT_REMOVE, OnExtRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRulerSettingsPage property page

CRulerSettingsPage::CRulerSettingsPage(CXMLElement* pXML)
	: CSettingsPage( CRulerSettingsPage::IDD ), m_pXML( pXML )
{
	//{{AFX_DATA_INIT(CRulerSettingsPage)
	//}}AFX_DATA_INIT
}

CRulerSettingsPage::~CRulerSettingsPage()
{
}

void CRulerSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRulerSettingsPage)
	DDX_Control(pDX, IDC_EXT_ADD, m_wndExtAdd);
	DDX_Control(pDX, IDC_EXT_REMOVE, m_wndExtRemove);
	DDX_Control(pDX, IDC_EXT_LIST, m_wndExtList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CRulerSettingsPage message handlers

BOOL CRulerSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	if ( m_pXML ) LoadXML();
	
	OnEditChangeExtList();
	OnSelChangeExtList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRulerSettingsPage::OnOK()
{
	if ( m_pXML ) EditXML();

	CSettingsPage::OnOK();
}

BOOL CRulerSettingsPage::LoadXML()
{
	CString strValue;
	CXMLElement* pXML = NULL;
	CXMLAttribute* pAttri = NULL;
	
	if ( pXML = m_pXML->GetElementByName( _T("BlackList") ) )
	{
		for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
		{
			CXMLElement* pMap = pXML->GetNextElement( pos );
			if ( ! pMap->IsNamed( _T("Map") ) ) continue;

			if ( pAttri = pMap->GetAttribute( _T("ID") ) )
			{
				strValue = pAttri->GetValue();
				m_wndExtList.AddString( strValue );
			}
		}
	}

	return TRUE;
}

BOOL CRulerSettingsPage::EditXML() 
{
	CString strValue;
	CXMLElement* pXML = NULL;
	CXMLAttribute* pAttri = NULL;
	
	if ( pXML = m_pXML->GetElementByName( _T("BlackList"), TRUE ) )
	{
		pXML->DeleteAllElements();

		for ( int nItem = 0; nItem < m_wndExtList.GetCount(); nItem++ )
		{
			m_wndExtList.GetLBText( nItem, strValue );
			CXMLElement* pMap = pXML->AddElement( _T("Map") );
			pMap->AddAttribute( _T("ID"), strValue );
		}
	}

	return TRUE;
}

void CRulerSettingsPage::OnEditChangeExtList() 
{
	m_wndExtAdd.EnableWindow( m_wndExtList.GetWindowTextLength() > 0 );
}

void CRulerSettingsPage::OnSelChangeExtList() 
{
	m_wndExtRemove.EnableWindow( m_wndExtList.GetCurSel() >= 0 );
}

void CRulerSettingsPage::OnExtAdd() 
{
	CString strExist;
	m_wndExtList.GetWindowText( strExist );

	strExist.TrimLeft(); strExist.TrimRight();
	if ( strExist.IsEmpty() ) return;

	if ( m_wndExtList.FindString( -1, strExist ) >= 0 ) return;

	m_wndExtList.AddString( strExist );
	m_wndExtList.SetWindowText( _T("") );
}

void CRulerSettingsPage::OnExtRemove() 
{
	int nItem = m_wndExtList.GetCurSel();
	if ( nItem >= 0 ) m_wndExtList.DeleteString( nItem );
	m_wndExtRemove.EnableWindow( FALSE );
}
