//
// Profiles.h
//

#if !defined(AFX_PROFILES_H__302BE49B_C6DA_4783_BBD2_BC6226D3D658__INCLUDED_)
#define AFX_PROFILES_H__302BE49B_C6DA_4783_BBD2_BC6226D3D658__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXMLElement;

class CProfiles  
{
// Construction
public:
	CProfiles();
	virtual ~CProfiles();

// Attributes
protected:
	CXMLElement*	m_pXMLMonitor;
protected:
	CCriticalSection	m_pSection;

// Operations
public:
	BOOL			Setup();
	void			Release();
public:
	CXMLElement*	FindProcess(LPCTSTR xJobId, POSITION& posNext, CXMLElement** ppActionXML = NULL);
};

extern CProfiles Profiles;

#endif // !defined(AFX_PROFILES_H__302BE49B_C6DA_4783_BBD2_BC6226D3D658__INCLUDED_)
