// Asteroids.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Asteroids.h"

#include "MainFrm.h"
#include "AsteroidsDoc.h"
#include "AsteroidsView.h"

#include "MsgWrapper.h"
#include "mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsApp

BEGIN_MESSAGE_MAP(CAsteroidsApp, CWinApp)
	//{{AFX_MSG_MAP(CAsteroidsApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsApp construction

CAsteroidsApp::CAsteroidsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	SetDialogBkColor( RGB( 0, 0, 0 ), RGB( 255, 255, 255 ) );

	m_pView = NULL;

	CMsgWrapper::Init();
}

CAsteroidsApp::~CAsteroidsApp()
{
	CMsgWrapper *pMsg = NULL;
	while( pMsg = CMsgWrapper::GetQueuedMessage() ) delete pMsg;

	CMsgWrapper::Close();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAsteroidsApp object

CAsteroidsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsApp initialization

BOOL CAsteroidsApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CString ExePath;

	GetModuleFileName( NULL, ExePath.GetBuffer( _MAX_PATH ), _MAX_PATH );

	ExePath.ReleaseBuffer();

	CString DirPath = ExePath.Left( ExePath.ReverseFind( '\\' ) );

	BOOL ret = SetCurrentDirectory( DirPath );

	InitCommonControls();

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Kuba Inc"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAsteroidsDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAsteroidsView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CAsteroidsApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsApp message handlers

BOOL CAsteroidsApp::OnIdle(LONG lCount) 
{	
	CMsgWrapper *pMsg;
	
	for( int i = 0; ( pMsg = CMsgWrapper::GetQueuedMessage() ) && i < 10; i++ )
	{
		switch( pMsg->msg )
		{
		case WM_END_OF_TICK:	m_pView->OnEndOfTick( pMsg->wParam, pMsg->lParam ); break;
		default:				pMsg->Forward(); break;
		}
		
		delete pMsg;
	}

	CWinApp::OnIdle(lCount);

	return TRUE;
}
