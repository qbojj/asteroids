// Asteroids.h : main header file for the ASTEROIDS application
//

#if !defined(AFX_ASTEROIDS_H__B660BCD2_E3A6_4986_BB57_42B02D4E2962__INCLUDED_)
#define AFX_ASTEROIDS_H__B660BCD2_E3A6_4986_BB57_42B02D4E2962__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "AsteroidsView.h"

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsApp:
// See Asteroids.cpp for the implementation of this class
//

class CAsteroidsApp : public CWinApp
{
public:
	CAsteroidsApp();
	~CAsteroidsApp();

	CAsteroidsView *m_pView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAsteroidsApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CAsteroidsApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASTEROIDS_H__B660BCD2_E3A6_4986_BB57_42B02D4E2962__INCLUDED_)
