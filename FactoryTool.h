// FactoryTool.h : main header file for the FACTORYTOOL application
//

#if !defined(AFX_FACTORYTOOL_H__6F25F480_E701_4477_90FD_B74910AB5C3C__INCLUDED_)
#define AFX_FACTORYTOOL_H__6F25F480_E701_4477_90FD_B74910AB5C3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFactoryToolApp:
// See FactoryTool.cpp for the implementation of this class
//

class CFactoryToolApp : public CWinApp
{
public:
	CFactoryToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFactoryToolApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFactoryToolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FACTORYTOOL_H__6F25F480_E701_4477_90FD_B74910AB5C3C__INCLUDED_)
