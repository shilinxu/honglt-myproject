//
// CtrlNetworkChild.h
//

#if !defined(AFX_CTRLNETWORKCHILD_H__B851684C_A103_44F3_808E_15AE5EA58092__INCLUDED_)
#define AFX_CTRLNETWORKCHILD_H__B851684C_A103_44F3_808E_15AE5EA58092__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum
{
	Ch_Ready	= 0,
	Ch_Talking	= 29,
	Ch_Rejected	= 8,
};

class CChannel;

/////////////////////////////////////////////////////////////////////////////
// CNetworkChild window

class CNetworkChild : public CListCtrl
{
// Construction
public:
	CNetworkChild();
	virtual ~CNetworkChild();

	DECLARE_DYNAMIC(CNetworkChild)

// Attributes
protected:

// Operations
public:
	int			AddItem(CChannel* pChannel);
	BOOL		RemoveItem(CChannel* pChannel);
	int			FindItem(CChannel* pChannel) const;
	CChannel*	FindChannel(DWORD xSeqNumber) const;
protected:
	void		Clear();
protected:
	int			PrintOwner(int nItem, LPCTSTR xLocalId, LPCTSTR xUsrNumber, LPCTSTR xFeeNumber = NULL);
	int			PrintChState(CChannel* pChannel, int nChState);
	int			PrintChState(CChannel* pChannel, int nChState, DWORD nReason);
	int			PrintExpense(CChannel* pChannel, double nExpense);
	int			PrintLength(CChannel* pChannel, DWORD nLength);
	int			PrintSeqNumber(int nItem, DWORD xSeqNumber);
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
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMLVCUSTOMDRAW* pNotify, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
	friend class CNetworkWnd;
	friend class CChannel;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLNETWORKCHILD_H__B851684C_A103_44F3_808E_15AE5EA58092__INCLUDED_)
