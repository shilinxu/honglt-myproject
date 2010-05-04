/*
 * dllmain.cxx
 *
 * DLL main entry point for XuserInfo.dll
 *
 */

#include "stdafx.h"
#include "WndUser.h"
#include "WindowManager.h"

#include "PacketBase.h"
#include "XML.h"

BOOL APIENTRY DllMain (HANDLE, DWORD, LPVOID)
{
    return TRUE;
}

CChildWnd* Create(CXMLElement* pXML, CWindowManager* pWindows)
{
	if ( CChildWnd* pChildWnd = pWindows->Find( RUNTIME_CLASS(CUserWnd) ) )
	{
		FreeLibrary( AfxGetResourceHandle() );
	}
	
	CChildWnd* pChildWnd = pWindows->Open( RUNTIME_CLASS(CUserWnd) );
//	((CMonitorWnd *)pChildWnd)->LayoutJobs( pXML );
	
	return pChildWnd;
}

/////////////////////////////////////////////////////////////////////////////
// User's group info

#define MIN_GROUP		4001
#define MAX_GROUP		4999

HMENU FindMenuGroup(HMENU hParent, LPCTSTR lpszMenu);
BOOL CreateMenuGroup(CXMLElement* pXML, HMENU hParent);

DWORD GetUserGroupInfo(LPCTSTR xUserId, HMENU hMenu)
{
	CXMLElement* pXML;
	DWORD nResult = CUserWnd::GetUserGroupInfo( xUserId, &pXML );
	
	if ( ! nResult && pXML != NULL )
	{
		ASSERT( pXML->IsNamed( ISyncDataView ) );
		
		CreateMenuGroup( pXML, hMenu );
		pXML->Delete();
	}
	
	return nResult;
}

BOOL CreateMenuGroup(CXMLElement* pXML, HMENU hParent)
{
	UINT nMenuId = MIN_GROUP;
	
	for ( POSITION pos = pXML->GetElementIterator() ; pos ; )
	{
		CXMLElement* pSync = pXML->GetNextElement( pos );
		if ( ! pSync->IsNamed( "SyncOrder" ) ) continue;
		
		static LPCTSTR pszGroup[] = { _T("xgroupname"), _T("xgroupnum") };
		
		CXMLAttribute* pAttri = pSync->GetAttribute( pszGroup[0] );
		if ( ! pAttri || pAttri->GetValue().IsEmpty() ) pAttri = pSync->GetAttribute( pszGroup[1] );
		
		HMENU hMenu = FindMenuGroup( hParent, pAttri->GetValue() );	if ( ! hMenu )
		{
			hMenu = CreatePopupMenu();
			AppendMenu( hParent, MF_STRING|MF_POPUP, (UINT)hMenu, pAttri->GetValue() );
		}
		
		CString strText = pSync->GetAttributeValue( _T("xname") );
		if ( ! strText.IsEmpty() ) strText += '\t';
		
		strText += pSync->GetAttributeValue( _T("xphone") );
		AppendMenu( hMenu, MF_STRING, nMenuId++, strText );
		
		if ( nMenuId > MAX_GROUP ) break;
	}
	
	return TRUE;
}

HMENU FindMenuGroup(HMENU hParent, LPCTSTR lpszMenu)
{
	for ( int nItem = GetMenuItemCount( hParent ); nItem > 0; nItem-- )
	{
		TCHAR strItem[64];
		GetMenuString( hParent, nItem - 1, strItem, 64, MF_BYPOSITION );
		
		if ( ! _tcsicmp( strItem, lpszMenu ) ) return GetSubMenu( hParent, nItem - 1 );
	}
	
	return NULL;
}
