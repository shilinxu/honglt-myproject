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
	inline int				GetType() const;
	inline CXMLNode*		AsNode() const;
	inline CXMLElement*		AsElement() const;
	inline CXMLAttribute*	AsAttribute() const;
public:
	inline CXMLElement*	GetParent() const;
	inline void			Delete();
public:
	inline CString		GetName() const;
	inline void			SetName(LPCTSTR pszValue);
	inline BOOL			IsNamed(LPCTSTR pszName) const;
	inline CString		GetValue() const;
	inline void			SetValue(LPCTSTR pszValue);
protected:
	static BOOL		ParseMatch(LPCTSTR& pszXML, LPCTSTR pszToken);
	static BOOL		ParseIdentifier(LPCTSTR& pszXML, CString& strIdentifier);
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
	CXMLElement*			Clone(CXMLElement* pParent = NULL);
	inline CXMLElement*		Detach();
public:
	inline CXMLElement*		AddElement(LPCTSTR pszName);
	inline CXMLElement*		AddElement(CXMLElement* pElement);
	inline int				GetElementCount() const;
	inline CXMLElement*		GetFirstElement() const;
	inline POSITION			GetElementIterator() const;
	inline CXMLElement*		GetNextElement(POSITION& pos) const;
	inline CXMLElement*		GetElementByName(LPCTSTR pszName) const;
	inline CXMLElement*		GetElementByName(LPCTSTR pszName, BOOL bCreate);
	inline void				RemoveElement(CXMLElement* pElement);
	void					DeleteAllElements();
public:
	inline CXMLAttribute*	AddAttribute(LPCTSTR pszName, LPCTSTR pszValue = NULL);
	inline CXMLAttribute*	AddAttribute(CXMLAttribute* pAttribute);
	inline int				GetAttributeCount() const;
	inline POSITION			GetAttributeIterator() const;
	inline CXMLAttribute*	GetNextAttribute(POSITION& pos) const;
	inline CXMLAttribute*	GetAttribute(LPCTSTR pszName) const;
	inline CString			GetAttributeValue(LPCTSTR pszName, LPCTSTR pszDefault = NULL) const;
	inline void				RemoveAttribute(CXMLAttribute* pAttribute);
	inline void				DeleteAttribute(LPCTSTR pszName);
	void					DeleteAllAttributes();
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
