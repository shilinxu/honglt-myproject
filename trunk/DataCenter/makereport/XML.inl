//
// XML.inl
//

//////////////////////////////////////////////////////////////////////
// CXMLNode node type and casting access

AFX_INLINE int CXMLNode::GetType() const
{
	return m_nNode;
}

AFX_INLINE CXMLNode* CXMLNode::AsNode() const
{
	return (CXMLNode*)this;
}

AFX_INLINE CXMLElement* CXMLNode::AsElement() const
{
	return ( m_nNode == xmlElement ) ? (CXMLElement*)this : NULL;
}

AFX_INLINE CXMLAttribute* CXMLNode::AsAttribute() const
{
	return ( m_nNode == xmlAttribute ) ? (CXMLAttribute*)this : NULL;
}

//////////////////////////////////////////////////////////////////////
// CXMLNode parent access and delete

AFX_INLINE CXMLElement* CXMLNode::GetParent() const
{
	return m_pParent;
}

AFX_INLINE void CXMLNode::Delete()
{
	if ( this == NULL ) return;

	if ( m_pParent != NULL )
	{
		if ( m_nNode == xmlElement ) m_pParent->RemoveElement( (CXMLElement*)this );
		else if ( m_nNode == xmlAttribute ) m_pParent->RemoveAttribute( (CXMLAttribute*)this );
	}

	delete this;
}

//////////////////////////////////////////////////////////////////////
// CXMLNode name access

AFX_INLINE CString CXMLNode::GetName() const
{
	return m_sName;
}

AFX_INLINE void CXMLNode::SetName(LPCTSTR pszValue)
{
	m_sName = pszValue;
}

AFX_INLINE BOOL CXMLNode::IsNamed(LPCTSTR pszName) const
{
	if ( this == NULL ) return FALSE;
	return m_sName.CompareNoCase( pszName ) == 0;
}

//////////////////////////////////////////////////////////////////////
// CXMLNode value access

AFX_INLINE CString CXMLNode::GetValue() const
{
	return m_sValue;
}

AFX_INLINE void CXMLNode::SetValue(LPCTSTR pszValue)
{
	m_sValue = pszValue;
}

//////////////////////////////////////////////////////////////////////
// CXMLElement detach

AFX_INLINE CXMLElement* CXMLElement::Detach()
{
	if ( m_pParent ) m_pParent->RemoveElement( this );
	m_pParent = NULL;
	return this;
}

//////////////////////////////////////////////////////////////////////
// CXMLElement element access

AFX_INLINE CXMLElement* CXMLElement::AddElement(LPCTSTR pszName)
{
	CXMLElement* pElement = new CXMLElement( this, pszName );
	m_pElements.AddTail( pElement );
	return pElement;
}

AFX_INLINE CXMLElement* CXMLElement::AddElement(CXMLElement* pElement)
{
	if ( pElement->m_pParent ) return NULL;
	m_pElements.AddTail( pElement );
	pElement->m_pParent = this;
	return pElement;
}

AFX_INLINE int CXMLElement::GetElementCount() const
{
	return (int)m_pElements.GetCount();
}

AFX_INLINE CXMLElement* CXMLElement::GetFirstElement() const
{
	if ( this == NULL ) return NULL;
	return m_pElements.GetCount() ? (CXMLElement*)m_pElements.GetHead() : NULL;
}

AFX_INLINE POSITION CXMLElement::GetElementIterator() const
{
	return m_pElements.GetHeadPosition();
}

AFX_INLINE CXMLElement* CXMLElement::GetNextElement(POSITION& pos) const
{
	return (CXMLElement*)m_pElements.GetNext( pos );
}

AFX_INLINE CXMLElement* CXMLElement::GetElementByName(LPCTSTR pszName) const
{
	for ( POSITION pos = GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement = GetNextElement( pos );
		if ( pElement->GetName().CompareNoCase( pszName ) == 0 ) return pElement;
	}
	return NULL;
}

AFX_INLINE CXMLElement* CXMLElement::GetElementByName(LPCTSTR pszName, BOOL bCreate)
{
	for ( POSITION pos = GetElementIterator() ; pos ; )
	{
		CXMLElement* pElement = GetNextElement( pos );
		if ( pElement->GetName().CompareNoCase( pszName ) == 0 ) return pElement;
	}

	return bCreate ? AddElement( pszName ) : NULL;
}

AFX_INLINE void CXMLElement::RemoveElement(CXMLElement* pElement)
{
	POSITION pos = m_pElements.Find( pElement );
	if ( pos ) m_pElements.RemoveAt( pos );
}

//////////////////////////////////////////////////////////////////////
// CXMLElement attribute access

AFX_INLINE CXMLAttribute* CXMLElement::AddAttribute(LPCTSTR pszName, LPCTSTR pszValue)
{
	CXMLAttribute* pAttribute = GetAttribute( pszName );

	if ( ! pAttribute )
	{
		pAttribute = new CXMLAttribute( this, pszName );
		CString strName( pszName );
		strName.MakeLower();
		m_pAttributes.SetAt( strName, pAttribute );
	}

	if ( pszValue ) pAttribute->SetValue( pszValue );

	return pAttribute;
}

AFX_INLINE CXMLAttribute* CXMLElement::AddAttribute(CXMLAttribute* pAttribute)
{
	if ( pAttribute->m_pParent ) return NULL;
	CString strName( pAttribute->m_sName );
	strName.MakeLower();
	m_pAttributes.SetAt( pAttribute->m_sName, pAttribute );
	pAttribute->m_pParent = this;
	return pAttribute;
}

AFX_INLINE int CXMLElement::GetAttributeCount() const
{
	return (int)m_pAttributes.GetCount();
}

AFX_INLINE POSITION CXMLElement::GetAttributeIterator() const
{
	return m_pAttributes.GetStartPosition();
}

AFX_INLINE CXMLAttribute* CXMLElement::GetNextAttribute(POSITION& pos) const
{
	CXMLAttribute* pAttribute = NULL;
	CString strName;
	m_pAttributes.GetNextAssoc( pos, strName, XMLVOID(pAttribute) );
	return pAttribute;
}

AFX_INLINE CXMLAttribute* CXMLElement::GetAttribute(LPCTSTR pszName) const
{
	CXMLAttribute* pAttribute = NULL;
	CString strName( pszName );
	strName.MakeLower();
	return m_pAttributes.Lookup( strName, XMLVOID(pAttribute) ) ? pAttribute : NULL;
}

AFX_INLINE CString CXMLElement::GetAttributeValue(LPCTSTR pszName, LPCTSTR pszDefault) const
{
	CXMLAttribute* pAttribute = GetAttribute( pszName );
	CString strResult;
	if ( pAttribute ) strResult = pAttribute->m_sValue;
	else if ( pszDefault ) strResult = pszDefault;
	return strResult;
}

AFX_INLINE void CXMLElement::RemoveAttribute(CXMLAttribute* pAttribute)
{
	CString strName( pAttribute->m_sName );
	strName.MakeLower();
	m_pAttributes.RemoveKey( strName );
}

AFX_INLINE void CXMLElement::DeleteAttribute(LPCTSTR pszName)
{
	CXMLAttribute* pAttribute = GetAttribute( pszName );
	if ( pAttribute ) pAttribute->Delete();
}
