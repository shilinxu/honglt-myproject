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
#include <streams.h>        // DirectShow base class definitions

#include "Resource.h"       // main symbols

//
// Utility
//

#undef IDC_HAND
#pragma comment(lib,"..\\Xuser32.lib")

#include "Xuserface\Xuserface.h"
#define XT_EXT_CLASS __declspec(dllimport)

class	__declspec(dllimport) CChildWnd;
class	__declspec(dllimport) CCoolBarCtrl;
class	__declspec(dllimport) CRegistry;
class	__declspec(dllimport) CTaskPanel;
class	__declspec(dllimport) CTaskBox;

class	__declspec(dllimport) CUserChannel;
class	__declspec(dllimport) CFilterGraph;

//
// 64-bit type
//

typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef unsigned __int64 QWORD;

//
// Tristate type
//

typedef int TRISTATE;

#define TS_UNKNOWN	0
#define TS_FALSE	1
#define TS_TRUE		2

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__26DF58C5_8A9C_435B_8517_28961ACB4B39__INCLUDED_)
