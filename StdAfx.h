// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__65799202_7F31_4CBF_8752_D56E225A0E70__INCLUDED_)
#define AFX_STDAFX_H__65799202_7F31_4CBF_8752_D56E225A0E70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if _MSC_VER > 1200
#define WINVER 0x0501  
#define _WIN32_WINNT 0x0501 
#else
#define TVS_NOHSCROLL           0x8000 
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxmt.h>
#include <atlbase.h>
#include <math.h>
#include "cmBase.h"

#include "winver.h"
#pragma comment(lib,"version.lib")
#pragma comment(lib,"Msimg32.lib")
#undef _MP

#define APP_VERSION	_T(" v1.42")
#pragma comment(lib,"dbghelp.lib")

#if _MSC_VER > 1200
	#ifdef _DEBUG
	#pragma   comment(lib,"RKUpgradeLib_Debug_2008.lib")
	#pragma   comment(lib,"cmMfcLib_vs2008_Debug.lib")
	#else
	#pragma   comment(lib,"RKUpgradeLib_release_2008.lib")
	#pragma   comment(lib,"cmMfcLib_vs2008.lib")
	#endif
#else
	#ifdef _DEBUG
	#pragma   comment(lib,"RKUpgradeLib_Debug.lib")
	#pragma comment(lib,"cmMfcLib_Debug.lib")
	#else
	#pragma   comment(lib,"RKUpgradeLib_release.lib")
	#pragma comment(lib,"cmMfcLib.lib")
	#endif
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__65799202_7F31_4CBF_8752_D56E225A0E70__INCLUDED_)
