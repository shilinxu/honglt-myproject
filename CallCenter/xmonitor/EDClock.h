//
// EDClock.h
//

#if !defined(AFX_EDCLOCK_H__A531CE31_A3EA_4AE3_98D6_15C545A87DA2__INCLUDED_)
#define AFX_EDCLOCK_H__A531CE31_A3EA_4AE3_98D6_15C545A87DA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEDClock : public CStatic
{
// Construction
public:
	CEDClock();
	virtual ~CEDClock();
	
	DECLARE_DYNAMIC(CEDClock)

// Attributes
public:
	enum EClockStyle { XDC_NORMAL = 1, XDC_FLOWNEXT, XDC_FLOWBACK };
protected:
	CImageList		m_pClock;
	EClockStyle		m_dwStyle;
	DWORD			m_tNavigated;

// Operations
public:
	EClockStyle		GetStyle() const;
	EClockStyle		SetStyle(EClockStyle dwStyle);
	EClockStyle		SetStyle(EClockStyle dwStyle, DWORD nFlowCount);
	int				GetFlowCount() const;
	BOOL			IsFlowOver() const;
protected:
	void			DrawNowTime(CTime& tNow);
	void			DrawFlowCount(DWORD nFlowCount);
	void			DrawClockBit(BYTE nDigit, int nPos);

// Overrides
public:
	//{{AFX_VIRTUAL(CEDClock)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CEDClock)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDCLOCK_H__A531CE31_A3EA_4AE3_98D6_15C545A87DA2__INCLUDED_)
