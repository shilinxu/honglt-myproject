//
// CtrlHomeUser.h
//

#if !defined(AFX_CTRLHOMEUSER_H__D2EFCE64_6CC6_4F0A_868E_9262412F4891__INCLUDED_)
#define AFX_CTRLHOMEUSER_H__D2EFCE64_6CC6_4F0A_868E_9262412F4891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CHomeUser : public CFormView
{
// Construction
protected:
	CHomeUser();
	virtual ~CHomeUser();
	DECLARE_DYNCREATE(CHomeUser)

// Form Data
public:
	//{{AFX_DATA(CHomeUser)
	enum { IDD = IDD_HOME_USER };
	//}}AFX_DATA
	
// Attributes
protected:
	
// Operations
public:
	
// Overrides
public:
	//{{AFX_VIRTUAL(CHomeUser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CHomeUser)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CTRLHOMEUSER_H__D2EFCE64_6CC6_4F0A_868E_9262412F4891__INCLUDED_)
