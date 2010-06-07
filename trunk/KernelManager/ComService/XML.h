//
// XML.h
//

#if !defined(AFX_XML_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_)
#define AFX_XML_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXMLNode;
class CXMLElement;
class CXMLAttribute;

class CXMLNode  
{
// Construction
public:
	CXMLNode(CXMLElement* pParent = NULL, LPCTSTR pszName = NULL);
	virtual ~CXMLNode();

// Attributes
protected:
	int				m_nNode;
	CXMLElement*	m_pParent;
	CString			m_sName;
	CString			m_sValue;

	enum { xmlNode, xmlElement, xmlAttribute };

// Operations
public:
	int				GetType() const;
	CXMLNode*		AsNode() const;
	CXMLElement*	AsElement() const;
	CXMLAttribute*	AsAttribute() const;
public:
	CXMLElement*	GetParent() const;
	void			Delete();
public:
	CString			GetName() const;
	void			SetName(LPCTSTR pszValue);
	BOOL			IsNamed(LPCTSTR pszName) const;
	CString			GetValue() const;
	void			SetValue(LPCTSTR pszValue);
protected:
	static BOOL		ParseMatch(LPCTSTR& pszXML, LPCTSTR pszToken);
	static BOOL		ParseIdentifier(LPCTSTR& pszXML, CString& strIdentifier);
	static void		ValueToUniform(LPCTSTR pszValue, CString& strXML);
#ifdef _AFX
	void			Serialize(CArchive& ar);
#endif
public:
	static CString	StringToValue(LPCTSTR& pszXML, int nLength);
	static void		ValueToString(LPCTSTR pszValue, CString& strXML);
	static void		UniformString(CString& str);

	friend class CXMLElement;
	friend class CQuerySearch;

	friend class CXMLCOM;
	
};


class CXMLElement : public CXMLNode
{
// Construction
public:
	CXMLElement(CXMLElement* pParent = NULL, LPCTSTR pszName = NULL);
	virtual ~CXMLElement();

// Attributes
protected:
#ifdef _AFX
	CPtrList m_pElements;
	CMapStringToPtr m_pAttributes;
#else
	CAtlList<CXMLElement*> m_pElements;
	CAtlMap<CString, CXMLAttribute*, CStringElementTraits<CString> > m_pAttributes;
#endif
	
// Operations
public:
	CXMLElement*	Clone(CXMLElement* pParent = NULL);
	CXMLElement*	Detach();
public:
	CXMLElement*	AddElement(LPCTSTR pszName);
	CXMLElement*	AddElement(CXMLElement* pElement);
	int				GetElementCount() const;
	CXMLElement*	GetFirstElement() const;
	POSITION		GetElementIterator() const;
	CXMLElement*	GetNextElement(POSITION& pos) const;
	CXMLElement*	GetElementByName(LPCTSTR pszName) const;
	CXMLElement*	GetElementByName(LPCTSTR pszName, BOOL bCreate);
	void			RemoveElement(CXMLElement* pElement);
	void			DeleteAllElements();
public:
	CXMLAttribute*	AddAttribute(LPCTSTR pszName, LPCTSTR pszValue = NULL);
	CXMLAttribute*	AddAttribute(CXMLAttribute* pAttribute);
	int				GetAttributeCount() const;
	POSITION		GetAttributeIterator() const;
	CXMLAttribute*	GetNextAttribute(POSITION& pos) const;
	CXMLAttribute*	GetAttribute(LPCTSTR pszName) const;
	CString			GetAttributeValue(LPCTSTR pszName, LPCTSTR pszDefault = NULL) const;
	void			RemoveAttribute(CXMLAttribute* pAttribute);
	void			DeleteAttribute(LPCTSTR pszName);
	void			DeleteAllAttributes();
public:
	CString			ToString(BOOL bHeader = FALSE, BOOL bNewline = FALSE);
	void			ToString(CString& strXML, BOOL bNewline = FALSE);
	BOOL			ParseString(LPCTSTR& strXML);
	BOOL			Equals(CXMLElement* pXML) const;
	CString			GetRecursiveWords();
	void			AddRecursiveWords(CString& strWords);
#ifdef _AFX
	void			Serialize(CArchive& ar);
#endif

	static CXMLElement* FromString(LPCTSTR pszXML, BOOL bHeader = FALSE);
	static CXMLElement* FromBytes(BYTE* pByte, DWORD nByte, BOOL bHeader = FALSE);
	static CXMLElement* FromFile(LPCTSTR pszPath, BOOL bHeader = FALSE);
	static CXMLElement* FromFile(HANDLE hFile, BOOL bHeader = FALSE);
};

class CXMLAttribute : public CXMLNode
{
// Construction
public:
	CXMLAttribute(CXMLElement* pParent, LPCTSTR pszName = NULL);
	virtual ~CXMLAttribute();

// Attributes
public:
	static LPCTSTR	xmlnsSchema;
	static LPCTSTR	xmlnsInstance;
	static LPCTSTR	schemaName;
	
// Operations
public:
	CXMLAttribute*	Clone(CXMLElement* pParent = NULL);
	void			ToString(CString& strXML);
	BOOL			ParseString(LPCTSTR& strXML);
	BOOL			Equals(CXMLAttribute* pXML) const;
#ifdef _AFX
	void			Serialize(CArchive& ar);
#endif

};

#ifdef _AFX
#define XMLVOID(x)	(void*&)(x)
#else
#define XMLVOID(x)	(x)
#endif

#include "XML.inl"


#endif // !defined(AFX_XML_H__5E273589_33B1_4782_987E_8378CDF31B68__INCLUDED_)
