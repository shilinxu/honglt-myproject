//
// WndTelecom.h
//

#if !defined(AFX_WNDTELECOM_H__E7E490FD_31DE_46F8_AEF0_27DEFC5D1D6A__INCLUDED_)
#define AFX_WNDTELECOM_H__E7E490FD_31DE_46F8_AEF0_27DEFC5D1D6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndChild.h"
#include "CtrlText.h"
#include "CtrlTelecomPanel.h"
#include "CtrlCoolBar.h"

#include "DBFTableDef.h"
#include "DBFRecordset.h"

class CXMLElement;

/////////////////////////////////////////////////////////////////////////////
// CTelecomWnd window

class CTelecomWnd : public CChildWnd
{
// Construction
public:
	CTelecomWnd();
	virtual ~CTelecomWnd();
	
	DECLARE_SERIAL(CTelecomWnd)

// Attributes
public:
	CListCtrl		m_wndList;
	CTextCtrl		m_wndText;
	CTelecomPanel	m_wndPanel;
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
	BOOL			OnBuildReport(CString* pxFile = NULL);
protected:
	static	UINT	ThreadStart(LPVOID pParam);
	virtual	BOOL	OnRun();
protected:
	BOOL		BuildReport(CDBFRecordset* xRecorddbf, _RecordsetPtr xRecordset);
	BOOL		PutToRemote(LPCTSTR xPath, LPCTSTR xFile);
	BOOL		CalcMapTalking(DWORD nSeqId, int* pnLeng, int* pnFee, CXMLElement* pXMLFee);
	BOOL		CalcMapTalking(_RecordsetPtr xRecordset, DWORD nSeqId, int* pnLeng, int* pnFee, CXMLElement* pXMLFee);
protected:
	BOOL		CreateFile(LPCTSTR xFile, CDBFTableDef& xTable);
	void		AddField(CDBFTableDef* pTable, DBF_CHAR* sField, DBF_CHAR field_type, DBF_UINT char_len);
	void		AddField(CDBFTableDef* pTable, DBF_CHAR* sField, DBF_CHAR field_type, DBF_CHAR num_len, DBF_CHAR num_dec);
private:
	int			PrintLineText(LPCTSTR xText, int nSubItem, int nAtLine = -1);
	int			PrintLineText(int xValue, int nSubItem, int nAtLine = -1);
	int			PrintLineText(double xValue, int nSubItem, int nAtLine = -1);
	int			GetNewLength(int nLength) const;

// Overrides
public:
	//{{AFX_VIRTUAL(CTelecomWnd)
	virtual BOOL Create(UINT nID, LPCTSTR xCaption);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTelecomWnd)
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
#define IDC_TELECOM_LIST	1001
#define IDC_TELECOM_TEXT	1002
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDTELECOM_H__E7E490FD_31DE_46F8_AEF0_27DEFC5D1D6A__INCLUDED_)
