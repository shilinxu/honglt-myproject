//
// Skin.cpp
//

#include "stdafx.h"
#include "Xuser32.h"
#include "CoolInterface.h"
#include "CtrlCoolBar.h"
#include "ImageFile.h"
#include "Skin.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSkin Skin;

//////////////////////////////////////////////////////////////////////
// CSkin construction

CSkin::CSkin()
{
}

CSkin::~CSkin()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CSkin apply

void CSkin::Apply()
{
	CreateDefault();
	Finalise();
}

//////////////////////////////////////////////////////////////////////
// CSkin clear

void CSkin::Clear()
{
	CString strName;
	POSITION pos;
	
	for ( pos = m_pMenus.GetStartPosition() ; pos ; )
	{
		CMenu* pMenu;
		m_pMenus.GetNextAssoc( pos, strName, (void*&)pMenu );
		delete pMenu;
	}
	
	for ( pos = m_pToolbars.GetStartPosition() ; pos ; )
	{
		CCoolBarCtrl* pBar;
		m_pToolbars.GetNextAssoc( pos, strName, (void*&)pBar );
		delete pBar;
	}
	
	m_pMenus.RemoveAll();
	m_pToolbars.RemoveAll();
	
	if ( m_brDialog.m_hObject != NULL ) m_brDialog.DeleteObject();
	if ( m_bmPanelMark.m_hObject != NULL ) m_bmPanelMark.DeleteObject();
}

//////////////////////////////////////////////////////////////////////
// CSkin root load

BOOL CSkin::LoadFromResource(HINSTANCE hInstance, UINT nResourceID)
{
	HMODULE hModule = hInstance != NULL ? (HMODULE)hInstance : GetModuleHandle( NULL );
	HRSRC hRes = FindResource( hModule, MAKEINTRESOURCE( nResourceID ), MAKEINTRESOURCE( 23 ) );
	
	if ( hRes == NULL ) return FALSE;
	
	CString strBody;
	
	DWORD nSize			= SizeofResource( hModule, hRes );
	HGLOBAL hMemory		= ::LoadResource( hModule, hRes );
	LPTSTR pszOutput	= strBody.GetBuffer( nSize + 1 );
	LPCSTR pszInput		= (LPCSTR)LockResource( hMemory );
	
	while ( nSize-- ) *pszOutput++ = *pszInput++;
	*pszOutput++ = 0;
	
	strBody.ReleaseBuffer();

	CString strPath;
	strPath.Format( _T("%lu$"), (DWORD)hModule );
	
	return LoadFromString( strBody, strPath );
}

BOOL CSkin::LoadFromString(const CString& strXML, const CString& strPath)
{
	CXMLElement* pXML = CXMLElement::FromString( strXML, TRUE );
	if ( pXML == NULL ) return FALSE;

	BOOL bSuccess = LoadFromXML( pXML, strPath );

	delete pXML;
	return bSuccess;
}

BOOL CSkin::LoadFromXML(CXMLElement* pXML, const CString& strPath)
{
	if ( ! pXML->IsNamed( _T("skin") ) ) return FALSE;
	
	BOOL bSuccess = FALSE;
	
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pSub = pXML->GetNextElement( pos );
		bSuccess = FALSE;
		
		if ( pSub->IsNamed( _T("commandImages") ) )
		{
			if ( ! LoadCommandImages( pSub, strPath ) ) break;
		}
		else if ( pSub->IsNamed( _T("commandMap") ) )
		{
			if ( ! LoadCommandMap( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("menus") ) )
		{
			if ( ! LoadMenus( pSub ) ) break;
		}
		else if ( pSub->IsNamed( _T("toolbars") ) )
		{
			if ( ! LoadToolbars( pSub ) ) break;
		}

		bSuccess = TRUE;
	}
	
	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CSkin menus

CMenu* CSkin::GetMenu(LPCTSTR pszName)
{
	CString strName( pszName );
	CMenu* pMenu = NULL;
	
	if ( m_pMenus.Lookup( strName, (void*&)pMenu ) ) return pMenu;

	for ( UINT nItem = 0 ; nItem < m_mnuDefault.GetMenuItemCount() ; nItem++ )
	{
		CString strItem;

		m_mnuDefault.GetMenuString( nItem, strItem, MF_BYPOSITION );

		if ( strItem.CompareNoCase( strName ) == 0 )
		{
			return m_mnuDefault.GetSubMenu( nItem );
		}
	}

	return NULL;
}

BOOL CSkin::LoadMenus(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );
		if ( pXML->IsNamed( _T("menu") ) && ! LoadMenu( pXML ) ) return FALSE;
	}
	
	return TRUE;
}

BOOL CSkin::LoadMenu(CXMLElement* pXML)
{
	CString strName = pXML->GetAttributeValue( _T("name") );
	if ( strName.IsEmpty() ) return FALSE;
	
	CMenu* pMenu = NULL;
	
	if ( m_pMenus.Lookup( strName, (void*&)pMenu ) )
	{
		delete pMenu;
		m_pMenus.RemoveKey( strName );
	}
	
	pMenu = new CMenu();
	
	if ( pXML->GetAttributeValue( _T("type"), _T("popup") ).CompareNoCase( _T("bar") ) == 0 )
	{
		pMenu->CreateMenu();
	}
	else
	{
		pMenu->CreatePopupMenu();
	}
	
	if ( CreateMenu( pXML, pMenu->GetSafeHmenu() ) )
	{
		m_pMenus.SetAt( strName, pMenu );
		return TRUE;
	}
	else
	{
		delete pMenu;
		return FALSE;
	}
}

BOOL CSkin::CreateMenu(CXMLElement* pRoot, HMENU hMenu)
{
	for ( POSITION pos = pRoot->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML	= pRoot->GetNextElement( pos );
		CString strText		= pXML->GetAttributeValue( _T("text") );
		
		int nAmp = strText.Find( '_' );
		if ( nAmp >= 0 ) strText.SetAt( nAmp, '&' );
		
		if ( pXML->IsNamed( _T("item") ) )
		{
			if ( UINT nID = LookupCommandID( pXML ) )
			{
				CString strKeys = pXML->GetAttributeValue( _T("shortcut") );
				
				if ( strKeys.GetLength() ) strText += '\t' + strKeys;
				
				AppendMenu( hMenu, MF_STRING, nID, strText );
			}
		}
		else if ( pXML->IsNamed( _T("menu") ) )
		{
			HMENU hSubMenu = CreatePopupMenu();
			
			if ( ! CreateMenu( pXML, hSubMenu ) )
			{
				DestroyMenu( hSubMenu );
				return FALSE;
			}
			
			AppendMenu( hMenu, MF_STRING|MF_POPUP, (UINT)hSubMenu, strText );
		}
		else if ( pXML->IsNamed( _T("separator") ) )
		{
			AppendMenu( hMenu, MF_SEPARATOR, ID_SEPARATOR, NULL );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin toolbars

BOOL CSkin::CreateToolBar(LPCTSTR pszName, CCoolBarCtrl* pBar)
{
	if ( pszName == NULL ) return FALSE;
	
	for ( CWnd* pChild = pBar->GetWindow( GW_CHILD ) ; pChild ; pChild = pChild->GetNextWindow() )
	{
		pChild->ShowWindow( SW_HIDE );
	}
	
	pBar->SetWatermark( NULL );
	pBar->Clear();
	
	CCoolBarCtrl* pBase = NULL;
	CString strName( pszName );
	
	m_pToolbars.Lookup( strName, (void*&)pBase );
	
	if ( pBase != NULL )
	{
		pBar->Copy( pBase );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CSkin::LoadToolbars(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );
		if ( pXML->IsNamed( _T("toolbar") ) && ! CreateToolBar( pXML ) ) return FALSE;
	}

	return TRUE;
}

BOOL CSkin::CreateToolBar(CXMLElement* pBase)
{
	CCoolBarCtrl* pBar = new CCoolBarCtrl();
	
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );
		
		if ( pXML->IsNamed( _T("button") ) )
		{
			if ( UINT nID = LookupCommandID( pXML ) )
			{
				CCoolBarItem* pItem = pBar->Add( nID, pXML->GetAttributeValue( _T("text") ) );
				CString strTemp = pXML->GetAttributeValue( _T("colour") );
				
				if ( strTemp.GetLength() == 6 )
				{
					int nRed, nGreen, nBlue;
					_stscanf( strTemp.Mid( 0, 2 ), _T("%x"), &nRed );
					_stscanf( strTemp.Mid( 2, 2 ), _T("%x"), &nGreen );
					_stscanf( strTemp.Mid( 4, 2 ), _T("%x"), &nBlue );
					pItem->m_crText = RGB( nRed, nGreen, nBlue );
				}
				
				strTemp = pXML->GetAttributeValue( _T("tip") );
				if ( strTemp.GetLength() ) pItem->SetTip( strTemp );
				
				strTemp = pXML->GetAttributeValue( _T("visible") );
				if ( strTemp.GetLength() ) pItem->Show( FALSE );
			}
		}
		else if ( pXML->IsNamed( _T("separator") ) )
		{
			pBar->Add( ID_SEPARATOR );
		}
		else if ( pXML->IsNamed( _T("rightalign") ) )
		{
			pBar->Add( ID_RIGHTALIGN );
		}
		else if ( pXML->IsNamed( _T("control") ) )
		{
			UINT nID, nWidth, nHeight = 0;
			CString strTemp;
			
			strTemp = pXML->GetAttributeValue( _T("id") );
			if ( _stscanf( strTemp, _T("%lu"), &nID ) == 1 )
			{
				strTemp = pXML->GetAttributeValue( _T("width") );
				
				if ( _stscanf( strTemp, _T("%lu"), &nWidth ) == 1 )
				{
					strTemp = pXML->GetAttributeValue( _T("height") );
					_stscanf( strTemp, _T("%lu"), &nHeight );
					pBar->Add( nID, nWidth, nHeight );
				}
			}
		}
		else if ( pXML->IsNamed( _T("label") ) )
		{
			CCoolBarItem* pItem = pBar->Add( 1, pXML->GetAttributeValue( _T("text") ) );
			pItem->m_crText = 0;
			pItem->SetTip( pXML->GetAttributeValue( _T("tip") ) );
		}
	}
	
	CString strName = pBase->GetAttributeValue( _T("name") );
	
	CCoolBarCtrl* pOld = NULL;
	if ( m_pToolbars.Lookup( strName, (void*&)pOld ) && pOld ) delete pOld;
	
	m_pToolbars.SetAt( strName, pBar );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin command lookup helper

UINT CSkin::LookupCommandID(CXMLElement* pXML, LPCTSTR pszName)
{
	CString strID = pXML->GetAttributeValue( pszName ? pszName : _T("id") );
	UINT nID = 0;
	
	if ( _istdigit( *(LPCTSTR)strID ) )
	{
		_stscanf( strID, _T("%lu"), &nID );
	}
	else
	{
		nID = CoolInterface.NameToID( strID );
	}
	
	return nID;
}

//////////////////////////////////////////////////////////////////////
// CSkin command map

BOOL CSkin::LoadCommandMap(CXMLElement* pBase)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );

		if (  pXML->IsNamed( _T("command") ) )
		{
			CString strTemp = pXML->GetAttributeValue( _T("code") );
			UINT nID;

			if ( _stscanf( strTemp, _T("%lu"), &nID ) != 1 )
				return FALSE;

			strTemp = pXML->GetAttributeValue( _T("id") );

			CoolInterface.NameCommand( nID, strTemp );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin command images

BOOL CSkin::LoadCommandImages(CXMLElement* pBase, const CString& strPath)
{
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );
		
		if ( pXML->IsNamed( _T("icon") ) )
		{
			UINT nID = LookupCommandID( pXML );
			if ( nID == 0 ) continue;
			
			CString strFile = strPath;
			strFile += pXML->GetAttributeValue( _T("res") );
			strFile += pXML->GetAttributeValue( _T("path") );
			
			int nPos = strFile.Find( '$' );
			HICON hIcon = NULL;
			
			if ( nPos < 0 )
			{
				if ( ExtractIconEx( strFile, 0, NULL, &hIcon, 1 ) != NULL && hIcon != NULL )
				{
					CoolInterface.AddIcon( nID, hIcon );
				}
			}
			else
			{
				HINSTANCE hInstance = NULL;
				UINT nIconID = 0;
				
				if ( _stscanf( strFile.Left( nPos ), _T("%lu"), &hInstance ) != 1 ) return TRUE;
				if ( _stscanf( strFile.Mid( nPos + 1 ), _T("%lu"), &nIconID ) != 1 ) return TRUE;
				
				hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE(nIconID), IMAGE_ICON, 16, 16, 0 );
				if ( hIcon != NULL ) CoolInterface.AddIcon( nID, hIcon );
			}
		}
		else if ( pXML->IsNamed( _T("bitmap") ) )
		{
			if ( ! LoadCommandBitmap( pXML, strPath ) ) return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CSkin::LoadCommandBitmap(CXMLElement* pBase, const CString& strPath)
{
	static LPCTSTR pszNames[] = { _T("id"), _T("id1"), _T("id2"), _T("id3"), _T("id4"), _T("id5"), _T("id6"), _T("id7"), _T("id8"), _T("id9"), NULL };
	
	CString strFile = strPath;
	strFile += pBase->GetAttributeValue( _T("id") );
	strFile += pBase->GetAttributeValue( _T("path") );
	
	HBITMAP hBitmap = LoadBitmap( strFile );
	if ( hBitmap == NULL ) return TRUE;
	
	strFile = pBase->GetAttributeValue( _T("mask") );
	COLORREF crMask = RGB( 0, 255, 0 );
	
	if ( strFile.GetLength() == 6 )
	{
		int nRed, nGreen, nBlue;
		_stscanf( strFile.Mid( 0, 2 ), _T("%x"), &nRed );
		_stscanf( strFile.Mid( 2, 2 ), _T("%x"), &nGreen );
		_stscanf( strFile.Mid( 4, 2 ), _T("%x"), &nBlue );
		crMask = RGB( nRed, nGreen, nBlue );
	}
	
	CoolInterface.ConfirmImageList();
	int nBase = ImageList_AddMasked( CoolInterface.m_pImages.m_hImageList, hBitmap, crMask );
	
	if ( nBase < 0 )
	{
		DeleteObject( hBitmap );
		return FALSE;
	}
	
	int nIndex = 0;
	
	for ( POSITION pos = pBase->GetElementIterator() ; pos ; )
	{
		CXMLElement* pXML = pBase->GetNextElement( pos );
		if ( ! pXML->IsNamed( _T("image") ) ) continue;
		
		strFile = pXML->GetAttributeValue( _T("index") );
		if ( strFile.GetLength() ) _stscanf( strFile, _T("%i"), &nIndex );
		nIndex += nBase;
		
		for ( int nName = 0 ; pszNames[ nName ] ; nName++ )
		{
			UINT nID = LookupCommandID( pXML, pszNames[ nName ] );
			if ( nID ) CoolInterface.m_pImageMap.SetAt( (LPVOID)nID, (LPVOID)nIndex );
			if ( nName && ! nID ) break;
		}
		
		nIndex -= nBase;
		nIndex ++;
	}
	
	DeleteObject( hBitmap );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CSkin default skin

void CSkin::CreateDefault()
{
	// Clear
	
	Clear();
	
	// Base UI
	
	CoolInterface.Clear();
	CoolInterface.CreateFonts();
	CoolInterface.CalculateColours( FALSE );
	
	// Colour Scheme
	
	m_crDialog					= CoolInterface.GetDialogBkColor();
	
	// Default Menu
	
	CMenu* pMenuBar = new CMenu();
	pMenuBar->LoadMenu( IDR_MAINFRAME );
	m_pMenus.SetAt( _T("CMainWnd"), pMenuBar );
	if ( m_mnuDefault.m_hMenu == NULL ) m_mnuDefault.LoadMenu( IDR_MAINFRAME );

	// Load Definitions
	
	LoadFromResource( NULL, IDR_XML_DEFINITIONS );
	LoadFromResource( NULL, IDR_XML_DEFAULT );
}

void CSkin::Finalise()
{
	m_brDialog.CreateSolidBrush( m_crDialog );
	
	m_bmPanelMark.LoadBitmap( IDB_PANEL_MARK );
}

//////////////////////////////////////////////////////////////////////
// CSkin load bitmap helper

HBITMAP CSkin::LoadBitmap(CString& strName)
{
	int nPos = strName.Find( '$' );

	if ( nPos < 0 )
	{
		CImageFile pFile(strName );
		return CImageFile::LoadBitmap(strName);
	}
	else
	{
		HINSTANCE hInstance = NULL;
		UINT nID = 0;
		
		if ( _stscanf( strName.Left( nPos ), _T("%lu"), &hInstance ) != 1 ) return NULL;
		if ( _stscanf( strName.Mid( nPos + 1 ), _T("%lu"), &nID ) != 1 ) return NULL;

		if ( LPCTSTR pszType = _tcsrchr( strName, '.' ) )
		{
			pszType ++;
			CImageFile::LoadBitmap( hInstance, nID, pszType );
		}
		else
		{
			return (HBITMAP)LoadImage( hInstance, MAKEINTRESOURCE(nID), IMAGE_BITMAP, 0, 0, 0 );
		}
	}
	return NULL;
}
