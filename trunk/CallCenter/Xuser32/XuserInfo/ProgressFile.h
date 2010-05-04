//
// ProgressFile.h
//

#if !defined(AFX_PROGRESSFILE_H__3C4DAAC1_0A4A_4C3E_B6D9_C3C487B7EF69__INCLUDED_)
#define AFX_PROGRESSFILE_H__3C4DAAC1_0A4A_4C3E_B6D9_C3C487B7EF69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CProgressFile window

class CProgressFile : public CDialog
{
// Enumerations
public:
	typedef enum
	{
		FileCopy,
		FileDownload,
		FileUpload,
	} FileType;

// Construction
public:
	CProgressFile(CFile* pSource, CFile* pTarget, FileType nType = FileType::FileCopy);
	
	const FileType	m_nType;
	CFile*			m_pSrcFile;
	CFile*			m_pTagFile;

// Dialog Data
public:
	//{{AFX_DATA(CProgressFile)
	enum { IDD = IDD_FILE_PROGRESS };
	CAnimateCtrl	m_wndAnimate;
	CStatic			m_wndDesc;
	CProgressCtrl	m_wndProgress;
	//}}AFX_DATA

protected:
	HANDLE			m_hThread;
	volatile BOOL	m_bThread;

// Operations
protected:
	void			StartThread();
	void			StopThread();
protected:
	virtual	BOOL	OnRun();
	static	UINT	ThreadStart(LPVOID pParam);

// Overrides
public:
	//{{AFX_VIRTUAL(CProgressFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CProgressFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSFILE_H__3C4DAAC1_0A4A_4C3E_B6D9_C3C487B7EF69__INCLUDED_)
