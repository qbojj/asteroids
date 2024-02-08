// MyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "asteroids.h"
#include "MyDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "afxpriv.h"

/////////////////////////////////////////////////////////////////////////////
// CMyDialog dialog


CMyDialog::CMyDialog( UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog( nIDTemplate, pParent )
{
	//{{AFX_DATA_INIT(CMyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyDialog, CDialog)
	//{{AFX_MSG_MAP(CMyDialog)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_KICKIDLE, OnKickIdle )
END_MESSAGE_MAP()

LRESULT CMyDialog::OnKickIdle( WPARAM wParam, LPARAM lParam )
{
	return ( (CAsteroidsApp *)AfxGetApp() )->OnIdle( (LONG)lParam );
}

/////////////////////////////////////////////////////////////////////////////
// CMyDialog message handlers
