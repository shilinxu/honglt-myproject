//
// CtrlNetworkChild.h
//

#if !defined(AFX_CTRLNETWORKCHILD_H__EB2670D7_3F79_442C_B4B2_11ACA8F24066__INCLUDED_)
#define AFX_CTRLNETWORKCHILD_H__EB2670D7_3F79_442C_B4B2_11ACA8F24066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// disp state definitions
//

#define DS_OTHR		(1<<11)
#define DS_FAIL		(1<<12)
#define DS_PLAY		(1<<13)
#define DS_HOOK		(1<<14)
#define DS_IDLE		(1<<15)

class CChannel;

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild window

class CNetworkChild : public CWnd
{
	friend class CNetworkWnd;
	
// Construction
public:
	CNetworkChild(int nChType = -1);
	virtual ~CNetworkChild();
	
	DECLARE_DYNAMIC(CNetworkChild)

// Attributes
public:
	int			m_nChFirst;
	int			m_nChType;
protected:
	LPBYTE		m_pChState;
	int			m_nChCount;
protected:
	int			m_nRetained;

// Operations
public:
	int			AddItem(CChannel* pChannel);
	void		SetItem(int nChan, LPCTSTR pszText, int nType);
	void		SetItem(int nChan, int nChState);
public:
	int			GetChType() const;
	int			GetChCount() const;
	int			GetChStateCount(int nChState) const;
protected:
	void		UpdateScroll();
	CChannel*	FindChannel(int nChState);
	void		DoPaint(CDC* pDC, CRect& rcClient, int nScroll);
	void		DrawChImage(CDC* pDC, int nX, int nY, int nChan, BOOL bGrid = FALSE);
	void		DrawChImage(int nChan, COLORREF crColour);
private:
	COLORREF	GetChColour(DWORD nChState) const;

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
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

inline int CNetworkChild::GetChCount() const
{
	return m_nChCount;
}

inline int CNetworkChild::GetChType() const
{
	return m_nChType;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKCHILD_H__EB2670D7_3F79_442C_B4B2_11ACA8F24066__INCLUDED_)
