//
// DlgProfilesManager.h
//

#if !defined(AFX_DLGPROFILESMANAGER_H__0F4D4873_886C_4D77_8979_A8F546643384__INCLUDED_)
#define AFX_DLGPROFILESMANAGER_H__0F4D4873_886C_4D77_8979_A8F546643384__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndSettingsSheet.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CProfilesManagerDlg dialog

class CProfilesManagerDlg : public CSettingsSheet
{
// Construction
public:
	CProfilesManagerDlg();
	virtual ~CProfilesManagerDlg();

	DECLARE_DYNAMIC(CProfilesManagerDlg)

// Attributes
protected:
	CBitmap			m_bmHeader;
protected:
	CXMLElement*	m_pXML;
	LPCTSTR			m_pszXML;

// Operations
public:
	static BOOL		Run(LPCTSTR pszWindow = NULL);
	int				DoModal(LPCTSTR pszWindow = NULL);
protected:
	void			AddPage(CSettingsPage* pPage);
	void			AddGroup(CSettingsPage* pPage);
	void			AddRecursive(CMap<CSettingsPage*, CSettingsPage*, HINSTANCE, HINSTANCE>& pList,
		CXMLElement* pXML, LPCTSTR pszPath = NULL);
	virtual void	DoPaint(CDC& dc);
protected:
	BOOL			LoadXML();
	BOOL			SaveXML();
	void			CreateXML();

// Overrides
public:
	//{{AFX_VIRTUAL(CProfilesManagerDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CProfilesManagerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROFILESMANAGER_H__0F4D4873_886C_4D77_8979_A8F546643384__INCLUDED_)
