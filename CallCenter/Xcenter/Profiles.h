//
// Profiles.h
//

#if !defined(AFX_PROFILES_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_)
#define AFX_PROFILES_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XML.h"
//class CXMLElement;

class AFX_EXT_CLASS CProfiles : CXMLElement
{
	friend class CUserWnd;
	
// Construction
public:
	CProfiles();
	virtual ~CProfiles();

// Attributes
protected:
	CXMLElement*	m_pXMLUser;
	CXMLElement*	m_pXMLCorp;
	CXMLElement*	m_pXMLInter;
	CXMLElement*	m_pXMLFinal;
	CXMLElement*	m_pXMLAnalog;
protected:
	CCriticalSection	m_pSection;
	CONST LPCTSTR	m_pszFile;

// Operations
public:
	BOOL			Setup();
	void			Release();
public:
	BOOL			IsDenied(CXMLElement* pXML, LPCTSTR xRemoteId) const;
	CXMLElement*	LookupKey(CXMLElement* pParent, LPCTSTR pszKey);
	CXMLElement*	LookupKey(CXMLElement* pParent, LPCTSTR pszKey, LPCTSTR pszName);
	inline CXMLElement*	operator[](LPCTSTR pszKey);
public:
	static BOOL		HavingText(LPCTSTR lpszText, LPCTSTR lpszValue);
public:
	CXMLElement*	FindCorpId(LPCTSTR xCorpId, INT xUserId);
	CXMLElement*	FindCorpId(LPCTSTR xFeeNumber);
	CXMLElement*	FindInter(INT xUserId);
	INT				FindInter(LPCTSTR xHost, LPCTSTR xField = _T("HOST"));
	CXMLElement*	FindFinal(LPCTSTR xFeeCode, CString* xModule = NULL);
	CXMLElement*	FindAnalog(INT xLinkId, LPCTSTR xLocalId = NULL);
protected:
	BOOL			IsPermitCalled(IN_ADDR* pAddress);
	BOOL			IsBlackList(LPCTSTR xRemoted, HANDLE hFile) const;
	BOOL			IsBlackList(LPCTSTR xRemoted, LPCTSTR xBlkFile) const;
	BOOL			IsPermitOut(LPCTSTR xRemoted, LPCTSTR xLimited) const;
protected:
	virtual void	AttachFrom(CXMLElement* pXML);
};

extern AFX_EXT_CLASS CProfiles Profiles;

#endif // !defined(AFX_PROFILES_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_)
