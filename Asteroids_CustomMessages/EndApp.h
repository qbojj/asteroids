#if !defined(AFX_ENDAPP_H__E28BA2D3_CE47_452D_AD29_F981B899AD96__INCLUDED_)
#define AFX_ENDAPP_H__E28BA2D3_CE47_452D_AD29_F981B899AD96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EndApp.h : header file
//

#include "MyDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CEndApp dialog

class CEndApp : public CMyDialog
{
// Construction
public:
	CEndApp(CWnd* pParent = NULL);   // standard constructor

	int DoEvilModal();

// Dialog Data
	//{{AFX_DATA(CEndApp)
	enum { IDD = IDD_END_APP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEndApp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	BOOL m_bEvil;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEndApp)
	afx_msg void OnYes();
	afx_msg void OnNo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENDAPP_H__E28BA2D3_CE47_452D_AD29_F981B899AD96__INCLUDED_)