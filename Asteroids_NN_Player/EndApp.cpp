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
	: CMyDialog(CEndApp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEndApp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bEvil = FALSE;
}


void CEndApp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEndApp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEndApp, CMyDialog)
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

	if( m_bEvil )
	{
		CRect rect;
		GetClientRect( &rect );

		CRect WorkArea;
		SystemParametersInfo( SPI_GETWORKAREA, NULL, (LPRECT)WorkArea, 0 );

		SetWindowPos( NULL, WorkArea.left + ( WorkArea.Width()  - rect.Width()  ) * Random( 0, 100000 ) / 100000,
							WorkArea.top  + ( WorkArea.Height() - rect.Height() ) * Random( 0, 100000 ) / 100000,
							0, 0, SWP_NOSIZE | SWP_NOZORDER );

		::SetFocus( NULL );

		return FALSE;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CEndApp::DoEvilModal()
{
	m_bEvil = TRUE;

	int res = DoModal();

	m_bEvil = FALSE;

	return res;
}