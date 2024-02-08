#if !defined(AFX_MYDIALOG_H__560F644B_40DC_44A3_AB83_50CF155A2B4F__INCLUDED_)
#define AFX_MYDIALOG_H__560F644B_40DC_44A3_AB83_50CF155A2B4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyDialog dialog

class CMyDialog : public CDialog
{
// Construction
public:
	CMyDialog( UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor

	virtual LRESULT OnKickIdle( WPARAM wParam, LPARAM lParam );

// Dialog Data
	//{{AFX_DATA(CMyDialog)
	enum { IDD = 0 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMyDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDIALOG_H__560F644B_40DC_44A3_AB83_50CF155A2B4F__INCLUDED_)
