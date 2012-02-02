//
// stdafx.h
//

#if !defined(AFX_STDAFX_H__26DF58C5_8A9C_435B_8517_28961ACB4B39__INCLUDED_)
#define AFX_STDAFX_H__26DF58C5_8A9C_435B_8517_28961ACB4B39__INCLUDED_

#pragma once

#pragma warning(disable: 4267 4251)
#pragma warning(disable: 4018 4114 4146 4245)

//
// Configuration
//

#define WINVER			0x0500	// Windows Version
#define _WIN32_WINDOWS	0x0500	// Windows Version
#define _WIN32_WINNT	0x0500	// NT Version
#define _WIN32_IE		0x0500	// IE Version
#define _WIN32_DCOM				// DCOM

#define _CRT_SECURE_NO_DEPRECATE

//
// MFC
//
// 
// #include <afxwin.h>				// MFC core and standard components
// #include <afxext.h>				// MFC extensions
// #include <afxtempl.h>			// MFC templates
// #include <afxmt.h>				// MFC threads
// #include <afxole.h>				// MFC OLE

//
// WIN32
//

#include <winsock2.h>			// Windows sockets V2

#include <atlbase.h>
#include <atlcoll.h>
#include <atlstr.h>

//
// Utility
//

#include "ComService.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__26DF58C5_8A9C_435B_8517_28961ACB4B39__INCLUDED_)
