//
// PageSettingsVideo.cpp
//

#include "stdafx.h"
#include "Xgateway.h"
#include "Settings.h"

#include "PageSettingsVideo.h"
#include "VideoCapture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CVideoSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CVideoSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CVideoSettingsPage)
	ON_CBN_SELCHANGE(IDC_VIDEO_INPUT, OnSelChangeDevInput)
	ON_BN_CLICKED(IDC_VIDEO_PROPERTY, OnVideoProperty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVideoSettingsPage property page

CVideoSettingsPage::CVideoSettingsPage() : CSettingsPage( CVideoSettingsPage::IDD )
{
	//{{AFX_DATA_INIT(CVideoSettingsPage)
	//}}AFX_DATA_INIT
	
	m_pPreview	= NULL;
}

CVideoSettingsPage::~CVideoSettingsPage()
{
	if ( m_pPreview ) delete m_pPreview;
}

void CVideoSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoSettingsPage)
	DDX_Control(pDX, IDC_VIDEO_FRAME, m_wndPreview);
	DDX_Control(pDX, IDC_VIDEO_PROPERTY, m_wndProperty);
	DDX_Control(pDX, IDC_VIDEO_INPUT, m_wndInput);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CVideoSettingsPage message handlers

BOOL CVideoSettingsPage::OnInitDialog() 
{
	CSettingsPage::OnInitDialog();

	EnumVideoDevices();
	LoadSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVideoSettingsPage::OnOK()
{
	EditSettings();

	CSettingsPage::OnOK();
}

BOOL CVideoSettingsPage::LoadSettings()
{
	int nItem = -1;
	CComBSTR xDisplayName = Settings.Video.DevInput;
	
	CComPtr<IBindCtx> pBindCtx = NULL;
	if ( FAILED(CreateBindCtx(0, &pBindCtx)) ) return FALSE;
	
	DWORD dwEaten;
	CComPtr<IMoniker> pMoniker = NULL;
	
	if ( SUCCEEDED(MkParseDisplayName(pBindCtx, xDisplayName, &dwEaten, &pMoniker)) )
	{
		for ( nItem = m_wndInput.GetCount() - 1; nItem >= 0; nItem-- )
		{
			LPVOID pmTemp = m_wndInput.GetItemDataPtr( nItem );
			if ( pMoniker->IsEqual( (IMoniker*)pmTemp ) == S_OK ) break;
		}
	}
	
	m_wndInput.SetCurSel( nItem == -1 ? m_wndInput.GetCount() - 1 : nItem );
	SendMessage(WM_COMMAND, MAKELONG(m_wndInput.GetDlgCtrlID(),CBN_SELCHANGE), (LPARAM)m_hWnd);
	
	return TRUE;
}

BOOL CVideoSettingsPage::EditSettings() 
{
	int nItem = m_wndInput.GetCurSel();
	if ( nItem == CB_ERR || nItem == m_wndInput.GetCount() - 1 )
	{
		Settings.Video.DevInput.Empty(); return TRUE;
	}
	
	LPOLESTR pDisplayName = NULL;
	IMoniker* pMoniker = (IMoniker*)m_wndInput.GetItemDataPtr( nItem );
	pMoniker->GetDisplayName( NULL, NULL, &pDisplayName );
	
	Settings.Video.DevInput = pDisplayName;
	CoTaskMemFree( pDisplayName );
	
	return TRUE;
}

int CVideoSettingsPage::EnumVideoDevices()
{
	CComPtr<ICreateDevEnum> icDevEnum;
	if ( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&icDevEnum ) != S_OK ) return 0;
	
	CComPtr<IEnumMoniker> ieMoniker = NULL;
	if ( icDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory,
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
	}
	
	return 0;
}

void CVideoSettingsPage::OnSelChangeDevInput() 
{
	int nItem = m_wndInput.GetCurSel();
	
	m_wndProperty.EnableWindow( nItem >= 0 && nItem < m_wndInput.GetCount() - 1 );
	
	if ( nItem == CB_ERR || nItem == m_wndInput.GetCount() - 1 )
	{
		if ( m_pPreview ) delete m_pPreview;
		m_pPreview = NULL; return;
	}
	
    IBaseFilter* ibFilter = NULL;
	IMoniker* pMoniker = (IMoniker*)m_wndInput.GetItemDataPtr( nItem );
	if ( pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&ibFilter) != S_OK ) return;
	
    StartPreview( ibFilter );
}

void CVideoSettingsPage::OnVideoProperty() 
{
	int nItem = m_wndInput.GetCurSel();
	if ( nItem == CB_ERR ) return;
	
    IBaseFilter* ibFilter = NULL;
	IMoniker* pMoniker = (IMoniker*)m_wndInput.GetItemDataPtr( nItem );
	if ( pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&ibFilter) != S_OK ) return;
	
	CComPtr<IPropertyBag> pBag = NULL;
	if ( pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, (void **)&pBag ) != S_OK ) return;
	
	CComPtr<ISpecifyPropertyPages> pSpecify;
	if ( ibFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify) != S_OK ) return;
	
	_variant_t TheValue;
	pBag->Read( L"FriendlyName", &TheValue, NULL );
	
	CAUUID cauuid;
	pSpecify->GetPages( &cauuid );
	
	OleCreatePropertyFrame( m_hWnd, 0, 0,
		(_bstr_t)TheValue, 1,
		(IUnknown **)&ibFilter, cauuid.cElems,
		(GUID *)cauuid.pElems, 0, 0, NULL );
	
	CoTaskMemFree( cauuid.pElems );
	ibFilter->Release();
}

BOOL CVideoSettingsPage::StartPreview(IBaseFilter* pCapFilter) 
{
	StopPreview();
/*	
	DWORD dwEaten;
	CComPtr<IMoniker> pMoniker = NULL;
	
	CComPtr<IBindCtx> pBindCtx = NULL;
	if ( FAILED(CreateBindCtx(0, &pBindCtx)) ) return FALSE;
	
	CComBSTR xDisplayName = Settings.Video.DevInput;
	if ( FAILED(MkParseDisplayName(pBindCtx, xDisplayName, &dwEaten, &pMoniker)) ) return FALSE;
	
	CComPtr<IBaseFilter> pCapFilter = NULL;
	if ( FAILED(pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCapFilter)) ) return FALSE;
*/	
	m_pPreview = new CVideoCapture( TRUE );
	m_pPreview->InstallFilter( pCapFilter, L"VideoCap" );
	m_pPreview->ConnectPreview( &MEDIATYPE_Video, pCapFilter );
	
	CRect rcClient ; 
	m_wndPreview.GetWindowRect( &rcClient );
	ScreenToClient( &rcClient ); rcClient.DeflateRect( 1, 1 );
	
	m_pPreview->PutIntoWindow( GetSafeHwnd(), rcClient, TRUE );
	
	return TRUE;
}

void CVideoSettingsPage::StopPreview()
{
	if ( m_pPreview ) delete m_pPreview;
}
