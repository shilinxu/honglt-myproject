//
// CtrlNetworkChild.h
//

#if !defined(AFX_CTRLNETWORKCHILD_H__EB2670D7_3F79_442C_B4B2_11ACA8F24066__INCLUDED_)
#define AFX_CTRLNETWORKCHILD_H__EB2670D7_3F79_442C_B4B2_11ACA8F24066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXMLElement;
class CSyncLineState;

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild window

class CNetworkChild : public CWnd
{
// Construction
public:
	CNetworkChild(CXMLElement* pXML = NULL);
	virtual ~CNetworkChild();
	
	DECLARE_DYNAMIC(CNetworkChild)

// Attributes
public:
	CString			m_strChType;
protected:
	CXMLElement*	m_pXML;
	CMap<int, int&, int, int&> m_pList;

// Operations
public:
	int				GetChCount() const;
	int				GetChState(int nChan) const;
	CXMLElement*	FindXMLFirstBy(LPCTSTR pszChType) const;
protected:
	POSITION		GetIterator() const;
	CXMLElement*	GetNext(POSITION& posNext, int nChType) const;
	int				CalcRowCount(LPRECT lpRect) const;
	BOOL			IsEmbededChType(int nChType) const;
	BOOL			IsEmbededChType(LPCTSTR pszChType) const;
	CXMLElement*	FindXMLBy(int nChan, int nAtom, int* pnRow) const;
	BOOL			GetPosition(int nChan, POINT* pPoint) const;
public:
	virtual int		MapClassToXML(CXMLElement* pXML, int nChState = S_CALL_STANDBY);
	virtual LPCTSTR SetFilter(LPCTSTR pszChType);
	void			UpdateChState(CSyncLineState* pPacket);
	int				MapClassToXML(int nChType, int nChFirst, int nChLength, int nChState = S_CALL_STANDBY);
	void			UpdateChState(int nChan, int nChState);
	void			ResetMap();
protected:
	void			UpdateScroll();
	void			DoPaint(CDC* pDC, CRect& rcClient, CXMLElement* pXML);
	void			DrawChImage(CDC* pDC, int nX, int nY, int nChan, BOOL bGrid = FALSE);
	void			DrawChImage(int nChan, COLORREF crColour);
private:
	COLORREF		GetChColour(DWORD nChState) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkChild)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkChild)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	
	friend class CNetworkHeaderBar;
};

inline int CNetworkChild::GetChCount() const
{
	return m_pList.GetCount();
}

inline int CNetworkChild::GetChState(int nChan) const
{
	int nChState = S_CALL_UNAVAILABLE;
	
	m_pList.Lookup( nChan, nChState );
	return nChState;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKCHILD_H__EB2670D7_3F79_442C_B4B2_11ACA8F24066__INCLUDED_)
