//
// stdafx.h
//

#if !defined(AFX_STDAFX_H__26DF58C5_8A9C_435B_8517_28961ACB4B39__INCLUDED_)
#define AFX_STDAFX_H__26DF58C5_8A9C_435B_8517_28961ACB4B39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// Configuration
//

#define WINVER			0x0500		// Windows Version
#define _WIN32_WINDOWS	0x0500		// Windows Version
#define _WIN32_WINNT	0x0500		// NT Version
#define _WIN32_IE		0x0500		// IE Version
#define _WIN32_DCOM					// DCOM
#define _AFX_NO_RICHEDIT_SUPPORT	// No RichEdit

//
// MFC
//

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxtempl.h>		// MFC templates
#include <afxmt.h>			// MFC threads
#include <afxole.h>			// MFC OLE
#include <afxdtctl.h>

//
// WIN32
//

#include <winsock2.h>		// Windows sockets V2
#include <dshow.h>
#include <atlbase.h>

#include "Resource.h"       // main symbols

//
// Utility
//

#undef IDC_HAND
#pragma comment(lib,"..\\Xuser32.lib")

class	__declspec(dllimport) CChildWnd;
class	__declspec(dllimport) CCoolBarCtrl;
class	__declspec(dllimport) CTaskPanel;
class	__declspec(dllimport) CTaskBox;

class	__declspec(dllimport) CUserChannel;

//
// 64-bit type
//

typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

//
// Tristate type
//

typedef int TRISTATE;

#define TS_UNKNOWN	0
#define TS_FALSE	1
#define TS_TRUE		2

//
// OLE DB
//

#pragma warning(disable: 4146)
#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")
#pragma warning(default: 4146)

//连接数据库
#define xConnect(/*_ConnectionPtr*/ cn, /*_bstr_t*/ bt) \
HRESULT hr_cn=S_OK;\
hr_cn=cn.CreateInstance(__uuidof(Connection));\
ASSERT(SUCCEEDED(hr_cn));\
cn->CursorLocation=adUseClient;\
hr_cn=cn->Open (bt,_bstr_t(""),_bstr_t(""),adModeUnknown);\
ASSERT(SUCCEEDED(hr_cn));

//断开数据库
#define xDisconnect(/*_ConnectionPtr*/ cn) \
cn->Close();

//打开记录集
#define xOpenRecordset(/*_RecordsetPtr*/ rs, /*_ConnectionPtr*/ cn,/*_bstr_t*/ bt) \
HRESULT hr_rs=S_OK;\
hr_rs=rs.CreateInstance(__uuidof(Recordset));\
ASSERT(SUCCEEDED(hr_rs));\
rs->CursorLocation=adUseClient;\
ASSERT(SUCCEEDED(hr_rs));\
hr_rs=rs->Open(bt,cn.GetInterfacePtr(),adOpenStatic,adLockOptimistic,adCmdText);\
ASSERT(SUCCEEDED(hr_rs));

//关闭记录集
#define xCloseRecordset(/*_RecordsetPtr*/ rs) \
if (NULL != rs)\
{rs->State ? rs->Close() : 0; rs.Release();}

//执行SQL语句
#define xExecute(/*_ConnectionPtr*/ cn,/*_bstr_t*/ bt,/*ExecuteOptionEnum*/ op) \
cn->Execute(bt,NULL,op);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__26DF58C5_8A9C_435B_8517_28961ACB4B39__INCLUDED_)
