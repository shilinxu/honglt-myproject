//
// HisReport.h
//

#if !defined(AFX_HISREPORT_H__254F6D8D_C399_474C_9980_0837C0E50C6C__INCLUDED_)
#define AFX_HISREPORT_H__254F6D8D_C399_474C_9980_0837C0E50C6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXMLElement;

class CHisReport  
{
// Construction
public:
	CHisReport();
	virtual ~CHisReport();

// Attributes
public:
	volatile BOOL	m_bEnabled;
protected:
	DWORD			m_tLastChk;
	CString			m_strFile;
	CXMLElement*	m_pXML;

// Operations
public:
	BOOL			IsEnabled() const;
	CXMLElement*	FindToday() const;
	DWORD			GetTotal() const;
	DWORD			GetToday() const;
	void			Register(int nInk = 1);
public:
	virtual BOOL	InitReport();
	virtual void	ExitReport();
	virtual BOOL	LoadReport();
	virtual BOOL	SaveReport();
	virtual BOOL	CheckOut();
protected:
	void			Create();
};

#endif // !defined(AFX_HISREPORT_H__254F6D8D_C399_474C_9980_0837C0E50C6C__INCLUDED_)
