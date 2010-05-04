//
// CtrlUserInfo.h
//

#if !defined(AFX_CTRLUSERINFO_H__D2EFCE64_6CC6_4F0A_868E_9262412F4891__INCLUDED_)
#define AFX_CTRLUSERINFO_H__D2EFCE64_6CC6_4F0A_868E_9262412F4891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CUserInfoForm : public CFormView
{
// Construction
protected:
	CUserInfoForm();
	virtual ~CUserInfoForm();
	DECLARE_DYNCREATE(CUserInfoForm)

// Form Data
public:
	//{{AFX_DATA(CUserInfoForm)
	enum { IDD = IDD_USER_INFO };
	//}}AFX_DATA
	
// Attributes
protected:
	
// Operations
public:
	
// Overrides
public:
	//{{AFX_VIRTUAL(CUserInfoForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CUserInfoForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CTRLUSERINFO_H__D2EFCE64_6CC6_4F0A_868E_9262412F4891__INCLUDED_)
