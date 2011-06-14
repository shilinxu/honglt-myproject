//
// WndUnicom.h
//

#if !defined(AFX_WNDUNICOM_H__E7E490FD_31DE_46F8_AEF0_27DEFC5D1D6A__INCLUDED_)
#define AFX_WNDUNICOM_H__E7E490FD_31DE_46F8_AEF0_27DEFC5D1D6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlText.h"
#include "CtrlUnicomPanel.h"
#include "CtrlCoolBar.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CUnicomWnd window

class CUnicomWnd : public CChildWnd
{
// Construction
public:
	CUnicomWnd();
	virtual ~CUnicomWnd();
	
	DECLARE_SERIAL(CUnicomWnd)

// Attributes
public:
	CListCtrl		m_wndList;
	CTextCtrl		m_wndText;
	CUnicomPanel	m_wndPanel;
	CCoolBarCtrl	m_wndToolBar;
protected:
	int				m_nText;
	CXMLElement*	m_pXML;
protected:
	HANDLE				m_hThread;
	volatile BOOL		m_bThread;
	CEvent				m_pWakeup;
	CCriticalSection	m_pSection;

// Operations
protected:
	BOOL			Setup();
	void			Release();
	BOOL			DoSizeText();
	void			Message(int nType, LPCTSTR pszFormat, ...);
	void			LogMessage(LPCTSTR pszLog, BOOL bShowTime = TRUE);
	CXMLElement*	FindProfile(CXMLElement* pXML, LPCTSTR xID);
	BOOL			IsTestPhone(LPCTSTR xPhone) const;
protected:
	void			StartThread();
	void			StopThread();
	BOOL			OnBuildReport();
	BOOL			HashCallCenter();
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
protected:
	BOOL		BuildReport(CFile& xAvlFile, _RecordsetPtr xRecordset);
	BOOL		BuildHeader(CFile& xAvlFile, _RecordsetPtr xRecordset, DWORD nTotalFee);
	BOOL		UpdateBookmark(_RecordsetPtr xRecordset, CHAR cAvlType);
	BOOL		PutToRemote(LPCTSTR xPath, LPCTSTR xMask = NULL);
	BOOL		PutToRemote(CFtpConnection* pFtpConnection, LPCTSTR xLocalFile, LPCTSTR xRemoteFile);
	BOOL		GetNextAvlFile(CFile& xAvlFile, CHAR cAvlType) const;
	BOOL		CalcMapTalking(DWORD nSeqId, int* pnLeng, int* pnFee, CXMLElement* pXMLFee);
	BOOL		CalcMapTalking(_RecordsetPtr xRecordset, DWORD nSeqId, int* pnLeng, int* pnFee, CXMLElement* pXMLFee);
	BOOL CUnicomWnd::PressRecord(LPCTSTR xPath, CFtpConnection* pFtpConnection, int nExpire);
private:
	void		WriteAvlFile(CFile* pFile, LPCTSTR xField, int nLength);
	int			PrintLineText(LPCTSTR xText, int nSubItem, int nAtLine = -1);
	int			PrintLineText(int xValue, int nSubItem, int nAtLine = -1);
	int			PrintLineText(double xValue, int nSubItem, int nAtLine = -1);
	DWORD		ReadTimeValue(LPCTSTR xTime) const;
	int			GetNewLength(int nLength) const;
	int			GetHeadLength() const;
	BOOL		IsIgnoreTime() const;

// Overrides
public:
	//{{AFX_VIRTUAL(CUnicomWnd)
	virtual BOOL Create(UINT nID, LPCTSTR xCaption);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUnicomWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	afx_msg LONG OnTabConnect(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
#define IDC_UNICOM_LIST	1001
#define IDC_UNICOM_TEXT	1002
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDUNICOM_H__E7E490FD_31DE_46F8_AEF0_27DEFC5D1D6A__INCLUDED_)
