//
// CtrlNetworkVideo.h
//

#if !defined(AFX_CTRLNETWORKVIDEO_H__31BD374E_A689_4334_A2A2_ED3A66B29AC3__INCLUDED_)
#define AFX_CTRLNETWORKVIDEO_H__31BD374E_A689_4334_A2A2_ED3A66B29AC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChannelUser.h"
class CNetworkChild;

class CNetworkVideo : public CWnd, public CUserChannel  
{
// Construction
public:
	CNetworkVideo(CNetworkChild* pPanel, LPCTSTR xUserId);
	virtual ~CNetworkVideo();
	DECLARE_DYNAMIC(CNetworkVideo)

// Attributes
protected:
	BOOL			m_bSelected;
	CNetworkChild*	m_pPanel;

// Operations
public:
	virtual void	Disconnect();
	CNetworkChild*	GetPanel() const;
	void			SetCaption(LPCTSTR pszCaption);
protected:
	void			PaintBorders();
	void			InvalidateNonclient();
protected:
	CNetworkVideo*	Select();
	void			Deselect();

// Overrides
public:
	//{{AFX_VIRTUAL(CNetworkVideo)
	virtual BOOL Create(CNetworkChild* pPanel, LPCTSTR pszCaption = NULL);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CNetworkVideo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	friend class CNetworkChild;
};

//
// Window Config
//

#define WINDOW_HEIGHT	90
#define WINDOW_WIDTH	120
#define CAPTION_HEIGHT	20

#endif // !defined(AFX_CTRLNETWORKVIDEO_H__31BD374E_A689_4334_A2A2_ED3A66B29AC3__INCLUDED_)
