//
// PageSettingsAudio.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Settings.h"

#include "PageSettingsAudio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CAudioSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CAudioSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CAudioSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAudioSettingsPage property page

CAudioSettingsPage::CAudioSettingsPage() : CSettingsPage( CAudioSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CAudioSettingsPage)
	//}}AFX_DATA_INIT
}

CAudioSettingsPage::~CAudioSettingsPage()
{
}

void CAudioSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioSettingsPage)
	DDX_Control(pDX, IDC_AUDIO_INPUT, m_wndInput);
	DDX_Control(pDX, IDC_AUDIO_OUTPUT, m_wndOutput);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CAudioSettingsPage message handlers

BOOL CAudioSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	EnumAudioDevices();
	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAudioSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CAudioSettingsPage::LoadSettings()
{
	LoadSettings( &m_wndInput, (LPCTSTR)Settings.Audio.DevInput );
	LoadSettings( &m_wndOutput, (LPCTSTR)Settings.Audio.DevOutput );
	
	return TRUE;
}

BOOL CAudioSettingsPage::EditSettings() 
{
	EditSettings( &m_wndInput, Settings.Audio.DevInput );
	EditSettings( &m_wndOutput, Settings.Audio.DevOutput );
	
	return TRUE;
}

BOOL CAudioSettingsPage::LoadSettings(CComboBox* pCombo, LPCTSTR pDevName)
{
	int nItem = -1;
	CComBSTR xDisplayName = pDevName;
	
	CComPtr<IBindCtx> pBindCtx = NULL;
	if ( FAILED(CreateBindCtx(0, &pBindCtx)) ) return FALSE;
	
	DWORD dwEaten;
	CComPtr<IMoniker> pMoniker = NULL;
	
	if ( SUCCEEDED(MkParseDisplayName(pBindCtx, xDisplayName, &dwEaten, &pMoniker)) )
	{
		for ( nItem = pCombo->GetCount() - 1; nItem >= 0; nItem-- )
		{
			LPVOID pmTemp = pCombo->GetItemDataPtr( nItem );
			if ( pMoniker->IsEqual( (IMoniker*)pmTemp ) == S_OK ) break;
		}
	}
	
	pCombo->SetCurSel( nItem );
	SendMessage(WM_COMMAND, MAKELONG(pCombo->GetDlgCtrlID(),CBN_SELCHANGE), (LPARAM)m_hWnd);
	
	return TRUE;
}

BOOL CAudioSettingsPage::EditSettings(CComboBox* pCombo, CString& xDevName)
{
	int nItem = pCombo->GetCurSel();
	if ( nItem == CB_ERR )
	{
		xDevName.Empty(); return TRUE;
	}
	
	LPOLESTR pDisplayName = NULL;
	IMoniker* pMoniker = (IMoniker*)pCombo->GetItemDataPtr( nItem );
	pMoniker->GetDisplayName( NULL, NULL, &pDisplayName );
	
	xDevName = pDisplayName;
	CoTaskMemFree( pDisplayName );
	
	return TRUE;
}

int CAudioSettingsPage::EnumAudioDevices()
{
	CComPtr<ICreateDevEnum> icDevEnum;
	if ( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&icDevEnum ) != S_OK ) return 0;
	
	CComPtr<IEnumMoniker> ieMoniker = NULL;
	if ( icDevEnum->CreateClassEnumerator( CLSID_AudioInputDeviceCategory,
		&ieMoniker, 0 ) != S_OK || ieMoniker == NULL ) return 0;
	
	IMoniker* pMoniker = NULL;
	while ( ieMoniker->Next(1, &pMoniker, NULL) == S_OK )
	{
		CComPtr<IPropertyBag> pBag = NULL;
		if ( pMoniker->BindToStorage( 0, 0, IID_IPropertyBag,
			(void **)&pBag ) != S_OK ) continue;
		
		_variant_t TheValue;
		pBag->Read( L"FriendlyName", &TheValue, NULL );
		
		int nItem = m_wndInput.AddString( (LPCTSTR)(_bstr_t)TheValue );
		m_wndInput.SetItemDataPtr( nItem, pMoniker );
		
		nItem = m_wndOutput.AddString( (LPCTSTR)(_bstr_t)TheValue );
		m_wndOutput.SetItemDataPtr( nItem, pMoniker );
	}
	
	return 0;
}
