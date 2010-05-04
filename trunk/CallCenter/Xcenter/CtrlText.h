#if !defined(AFX_CTRLTEXT_H__4E05259F_9244_4770_B588_FA10FAAB645E__INCLUDED_)
#define AFX_CTRLTEXT_H__4E05259F_9244_4770_B588_FA10FAAB645E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CtrlText.h : header file
//

class CTextCtrl : public CWnd
{
// Construction
public:
	CTextCtrl();
	virtual ~CTextCtrl();

// Attributes
protected:
	CPtrArray			m_pLines;
	int					m_nPosition;
	int					m_nTotal;
	CSize				m_cCharacter;
	CFont				m_pFont;
	COLORREF			m_crBackground;
	COLORREF			m_crText[5];
	BOOL				m_bProcess;
	CCriticalSection	m_pSection;
	UINT				m_nScrollWheelLines; // number of lines to scroll when the mouse wheel is rotated

// Operations
public:
	void	Add(int nType, LPCTSTR pszText);
	void	AddLine(int nType, LPCTSTR pszLine);
	void	Clear(BOOL bInvalidate = TRUE);
	CFont*	GetFont();
protected:
	void	UpdateScroll(BOOL bFull = FALSE);

// Overrides
public:
	//{{AFX_VIRTUAL(CTextCtrl)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTextCtrl)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CTextLine
{
// Construction
public:
	CTextLine(int nType, LPCTSTR pszLine);
	virtual ~CTextLine();

// Attributes
public:
	CString	m_sText;
	int*	m_pLine;
	int		m_nLine;
	int		m_nType;

// Operations
public:
	int		Process(int nWidth);
	void	Paint(CDC* pDC, CRect* pRect);
protected:
	void	AddLine(int nLength);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLTEXT_H__4E05259F_9244_4770_B588_FA10FAAB645E__INCLUDED_)
