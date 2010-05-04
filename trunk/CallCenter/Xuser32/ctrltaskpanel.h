//
// CtrlTaskPanel.h
//

#if !defined(AFX_CTRLTASKPANEL_H__509DBC3E_C56B_4A11_993A_3F227C6D9621__INCLUDED_)
#define AFX_CTRLTASKPANEL_H__509DBC3E_C56B_4A11_993A_3F227C6D9621__INCLUDED_

#pragma once

class CTaskBox;

class CTaskPanel : public CWnd
{
// Construction
public:
	CTaskPanel();
	virtual ~CTaskPanel();

	DECLARE_DYNAMIC(CTaskPanel)

// Attributes
protected:
	CPtrList		m_pBoxes;
	CTaskBox*		m_pStretch;
	int				m_nMargin;
	int				m_nCurve;
	BOOL			m_bLayout;
protected:
	COLORREF		m_crPanelBack;
	CBitmap			m_bmWatermark;
	CBitmap			m_bmFooter;
	
// Operations
public:
	CTaskBox*		AddBox(CTaskBox* pBox, POSITION posBefore = NULL);
	void			RemoveBox(CTaskBox* pBox);
	virtual void	ClearBoxes();
public:
	void			SetStretchBox(CTaskBox* pBox);
	void			SetMargin(int nMargin, int nCurve = 2);
	void			SetWatermark(HBITMAP hBitmap);
	void			SetFooter(HBITMAP hBitmap, BOOL bDefault = TRUE);
public:
	virtual void	OnChanged();
	virtual void	Layout(CRect& rcClient);

// List Access
public:
	inline POSITION GetBoxIterator() const
	{
		return m_pBoxes.GetHeadPosition();
	}

	inline CTaskBox* GetNextBox(POSITION& pos) const
	{
		return (CTaskBox*)m_pBoxes.GetNext( pos );
	}

	inline int GetBoxCount() const
	{
		return m_pBoxes.GetCount();
	}
	
// Overrides
public:
	//{{AFX_VIRTUAL(CTaskPanel)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTaskPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CTaskBox;
};

class CTaskBox : public CWnd
{
// Construction
public:
	CTaskBox();
	virtual ~CTaskBox();

	DECLARE_DYNAMIC(CTaskBox)

// Attributes
protected:
	CBitmap			m_bmWatermark;
	CBitmap			m_bmCaptionmark;
protected:
	CTaskPanel*		m_pPanel;
	BOOL			m_bPrimary;
	BOOL			m_bOpen;
	BOOL			m_bCaptionCurve;
	int				m_nHeight;
private:
	BOOL			m_bVisible;
	BOOL			m_bHover;
	HICON			m_hIcon;
	BOOL			m_bIconDel;
	
// Operations
public:
	CTaskPanel*		GetPanel() const;
	void			SetCaption(LPCTSTR pszCaption);
	void			SetIcon(HICON hIcon, BOOL bDelete = FALSE);
	void			SetSize(int nHeight);
	void			SetPrimary(BOOL bPrimary = TRUE);
	void			SetWatermark(HBITMAP hBitmap);
	void			SetCaptionmark(HBITMAP hBitmap, BOOL bDefault = TRUE);
	void			Expand(BOOL bOpen = TRUE);
	int				GetOuterHeight() const;
protected:
	void			PaintBorders();
	void			InvalidateNonclient();
protected:
	virtual void	OnExpanded(BOOL bOpen);

// Overrides
public:
	//{{AFX_VIRTUAL(CTaskBox)
	virtual BOOL Create(CTaskPanel* pPanel, int nHeight = 0, LPCTSTR pszCaption = NULL, UINT nIDIcon = 0);
	virtual BOOL CreateEx(CWnd* pParentWnd, const RECT& rect, LPCTSTR pszCaption = NULL, UINT nIDIcon = 0);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTaskBox)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// inline
public:
	inline BOOL		IsPrimary() const
	{
		return m_bPrimary;
	}
	
	friend class CTaskPanel;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CTRLTASKPANEL_H__509DBC3E_C56B_4A11_993A_3F227C6D9621__INCLUDED_)
