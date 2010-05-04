//
// Skin.h
//

#if !defined(AFX_SKIN_H__C788F62D_8DA3_4C93_8AA6_45145B906566__INCLUDED_)
#define AFX_SKIN_H__C788F62D_8DA3_4C93_8AA6_45145B906566__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CoolMenu.h"

class CCoolBarCtrl;
class CXMLElement;

class AFX_EXT_CLASS CSkin  
{
// Construction
public:
	CSkin();
	virtual ~CSkin();

// Operations
public:
	void	Apply();
	void	Clear();
	BOOL	LoadFromResource(HINSTANCE hInstance, UINT nResourceID);
	BOOL	LoadFromString(const CString& strXML, const CString& strPath);
	BOOL	LoadFromXML(CXMLElement* pXML, const CString& strPath);
protected:
	void	Finalise();
	void	CreateDefault();
	HBITMAP	LoadBitmap(CString& strName);

// Menus
public:
	CMenu*	GetMenu(LPCTSTR pszName);
	CMapStringToPtr	m_pMenus;
protected:
	BOOL	LoadMenus(CXMLElement* pBase);
	BOOL	LoadMenu(CXMLElement* pXML);
	BOOL	CreateMenu(CXMLElement* pXML, HMENU hMenu);
	CMenu	m_mnuDefault;
	
// Toolbars
public:
	BOOL	CreateToolBar(LPCTSTR pszName, CCoolBarCtrl* pBar);
	CMapStringToPtr m_pToolbars;
protected:
	BOOL	LoadToolbars(CXMLElement* pBase);
	BOOL	CreateToolBar(CXMLElement* pElement);

// Colour Scheme
public:
	COLORREF	m_crDialog;
	CBrush		m_brDialog;
	COLORREF	m_crPanelBack;
	CBitmap		m_bmPanelMark;

// Other
protected:
	UINT	LookupCommandID(CXMLElement* pXML, LPCTSTR pszName = NULL);
	BOOL	LoadCommandMap(CXMLElement* pBase);
	BOOL	LoadCommandImages(CXMLElement* pBase, const CString& strPath);
	BOOL	LoadCommandBitmap(CXMLElement* pBase, const CString& strPath);
};

extern AFX_EXT_CLASS CSkin Skin;

#endif // !defined(AFX_SKIN_H__C788F62D_8DA3_4C93_8AA6_45145B906566__INCLUDED_)
