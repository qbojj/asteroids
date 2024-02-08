// EndApp.cpp : implementation file
//

#include "stdafx.h"
#include "asteroids.h"
#include "EndApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEndApp dialog


CEndApp::CEndApp(CWnd* pParent /*=NULL*/)
	: CDialog(CEndApp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEndApp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEndApp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEndApp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEndApp, CDialog)
	//{{AFX_MSG_MAP(CEndApp)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEndApp message handlers

void CEndApp::OnYes() 
{
	// TODO: Add your control notification handler code here
	EndDialog( IDYES );
}

void CEndApp::OnNo() 
{
	// TODO: Add your control notification handler code here
	EndDialog( IDNO );
}

BOOL CEndApp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
