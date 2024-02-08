// ChangeKeyBindings.cpp : implementation file
//

#include "stdafx.h"
#include "asteroids.h"
#include "ConfigDlg.h"

#include "AsteroidsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT IsJoystickControllActive( int Controll )
{
	if( Controll >= 128 || Controll == NONE ) return FALSE;

	LPDIJOYSTATE2 joy = g_oJoystick.GetJoystickStateInfo();

	if( Controll >= 0 ) return joy->rgbButtons[ Controll ];

	if( Controll <= HAT_UP && Controll >= HAT_LEFT )
	{
		if( joy->rgdwPOV[ 0 ] == -1 ) return FALSE;

		switch( Controll )
		{
		case HAT_UP:	return joy->rgdwPOV[ 0 ] >= 30000	|| joy->rgdwPOV[ 0 ] <= 6000;
		case HAT_RIGHT:	return joy->rgdwPOV[ 0 ] >= 3000	&& joy->rgdwPOV[ 0 ] <= 15000;
		case HAT_DOWN:	return joy->rgdwPOV[ 0 ] >= 12000	&& joy->rgdwPOV[ 0 ] <= 24000;
		case HAT_LEFT:	return joy->rgdwPOV[ 0 ] >= 21000	&& joy->rgdwPOV[ 0 ] <= 33000;
		}
	}

	switch( Controll )
	{
	case X_PLUS:			return  joy->lX				<= 0 ? FALSE :  joy->lX;
	case X_MINUS:			return -joy->lX				<= 0 ? FALSE : -joy->lX;
	case X_ROTATION_PLUS:	return  joy->lRx			<= 0 ? FALSE :  joy->lRx;
	case X_ROTATION_MINUS:	return -joy->lRx			<= 0 ? FALSE : -joy->lRx;
	case Y_PLUS:			return  joy->lY				<= 0 ? FALSE :  joy->lY;
	case Y_MINUS:			return -joy->lY				<= 0 ? FALSE : -joy->lY;
	case Y_ROTATION_PLUS:	return  joy->lRy			<= 0 ? FALSE :  joy->lRy;
	case Y_ROTATION_MINUS:	return -joy->lRy			<= 0 ? FALSE : -joy->lRy;
	case Z_PLUS:			return  joy->lZ				<= 0 ? FALSE :  joy->lZ;
	case Z_MINUS:			return -joy->lZ				<= 0 ? FALSE : -joy->lZ;
	case Z_ROTATION_PLUS:	return  joy->lRz			<= 0 ? FALSE :  joy->lRz;
	case Z_ROTATION_MINUS:	return -joy->lRz			<= 0 ? FALSE : -joy->lRz;
	case U_PLUS:			return  joy->rglSlider[ 0 ]	<= 0 ? FALSE :  joy->rglSlider[ 0 ];
	case U_MINUS:			return -joy->rglSlider[ 0 ]	<= 0 ? FALSE : -joy->rglSlider[ 0 ];
	case V_PLUS:			return  joy->rglSlider[ 1 ]	<= 0 ? FALSE :  joy->rglSlider[ 1 ];
	case V_MINUS:			return -joy->rglSlider[ 1 ]	<= 0 ? FALSE : -joy->rglSlider[ 1 ];
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CMyDialog(CConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDlg)
	//}}AFX_DATA_INIT

	m_iClicked = m_iLastClicked = -1;
	m_bClickedSecondary = m_bLastClickedSecondary = false;
	m_iInputMode = KEYBOARD;

	m_bFirstTime = true;

	m_bPressedOK = false;

	m_bScaleGame = m_bFullscreen = true;

	m_bAntialiasing = false;
}


void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
	DDX_Control(pDX, IDC_ANTIALIASING, m_oAntialiasing);
	DDX_Control(pDX, IDC_INPUT_DEVICES, m_oInputDevices);
	DDX_Control(pDX, IDC_FULLSCREEN, m_oFullscreen);
	DDX_Control(pDX, IDC_SCALE_GAME, m_oScaleGame );
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_BUTTON_LEFT, m_aButtons[ LEFT ][ 0 ]);
	DDX_Control(pDX, IDC_BUTTON_RIGHT, m_aButtons[ RIGHT ][ 0 ]);
	DDX_Control(pDX, IDC_BUTTON_ACCELERATE, m_aButtons[ ACC ][ 0 ]);
	DDX_Control(pDX, IDC_BUTTON_SHOOT, m_aButtons[ SHOOT ][ 0 ]);
	DDX_Control(pDX, IDC_BUTTON_HYP_SPC, m_aButtons[ HYP_SPACE ][ 0 ]);

	DDX_Control(pDX, IDC_BUTTON_LEFT2, m_aButtons[ LEFT ][ 1 ]);
	DDX_Control(pDX, IDC_BUTTON_RIGHT2, m_aButtons[ RIGHT ][ 1 ]);
	DDX_Control(pDX, IDC_BUTTON_ACCELERATE2, m_aButtons[ ACC ][ 1 ]);
	DDX_Control(pDX, IDC_BUTTON_SHOOT2, m_aButtons[ SHOOT ][ 1 ]);
	DDX_Control(pDX, IDC_BUTTON_HYP_SPC2, m_aButtons[ HYP_SPACE ][ 1 ]);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CMyDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
	ON_BN_CLICKED(IDC_BUTTON_ACCELERATE, OnButtonAccelerate)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, OnButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, OnButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_SHOOT, OnButtonShoot)
	ON_BN_CLICKED(IDC_BUTTON_SHOOT2, OnButtonShoot2)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT2, OnButtonRight2)
	ON_BN_CLICKED(IDC_BUTTON_LEFT2, OnButtonLeft2)
	ON_BN_CLICKED(IDC_BUTTON_ACCELERATE2, OnButtonAccelerate2)
	ON_BN_CLICKED(IDC_BUTTON_LEFT_ERASE, OnButtonLeftErase)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_ERASE, OnButtonRightErase)
	ON_BN_CLICKED(IDC_BUTTON_ACCELERATE_ERASE, OnButtonAccelerateErase)
	ON_BN_CLICKED(IDC_BUTTON_SHOOT_ERASE, OnButtonShootErase)
	ON_BN_CLICKED(IDC_BUTTON_HYP_SPC, OnButtonHypSpc)
	ON_BN_CLICKED(IDC_BUTTON_HYP_SPC2, OnButtonHypSpc2)
	ON_BN_CLICKED(IDC_BUTTON_HYP_SPC_ERASE, OnButtonHypSpcErase)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_INPUT_DEVICES, OnSelchangeInputDevices)
	ON_CBN_DROPDOWN(IDC_INPUT_DEVICES, OnDropdownInputDevices)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg message handlers

void CConfigDlg::CheckKeyboardInput()
{
	int Succesful = 0;

	BYTE Keyboard[ 256 ];

	if( GetKeyboardState( (BYTE *)(&Keyboard) ) )
	{
		for( int i = 0; i < 256; i++ )
		{
			if( Keyboard[ i ] & 0x80 )
			{
				Succesful = i;
				break;
			}
		}

		if( Succesful != 0 )
		{
			CString str = VKToText( Succesful );

			m_aButtonCodes[ m_iClicked ][ m_bClickedSecondary ] = Succesful;
			m_aButtons[ m_iClicked ][ m_bClickedSecondary ].SetWindowText( str );
			
			m_iClicked = -1;
			m_bClickedSecondary = false;
		}
	}
}

void CConfigDlg::CheckJoystickInput()
{
	if( g_oJoystick.PollDevice() )
	{
		int Succesful = NONE;

		for( int i = m_aJoystickButtonNames.GetSize() - 1; i > MIN_CONTROLL_VALUE; i-- )
		{ 
			if( IsJoystickControllActive( i ) )
			{
				Succesful = i;
				break;
			}
		}

		if( Succesful != NONE )
		{
			m_aJoystickCodes[ m_iClicked ][ m_bClickedSecondary ] = Succesful;
			m_aButtons[ m_iClicked ][ m_bClickedSecondary ].SetWindowText( GetButtonName( m_iClicked, m_bClickedSecondary ) );

			m_iClicked = -1;
			m_bClickedSecondary = false;
		}
	}
}

void CConfigDlg::ButtonClicked( UINT Button, bool IsSecondary )
{
	m_iLastClicked = m_iClicked;
	m_iClicked = Button;

	m_bLastClickedSecondary = m_bClickedSecondary;
	m_bClickedSecondary = IsSecondary;

	if( m_iClicked != -1 && m_iClicked == m_iLastClicked && m_bClickedSecondary == m_bLastClickedSecondary )
	{
		m_aButtons[ m_iClicked ][ m_bClickedSecondary ].SetWindowText( GetButtonName( m_iClicked, m_bClickedSecondary ) );
		m_iClicked = -1;
		return;
	}

	if( m_iLastClicked != -1 )
	{
		m_aButtons[ m_iLastClicked ][ m_bLastClickedSecondary ].SetWindowText( GetButtonName( m_iLastClicked, m_bLastClickedSecondary ) );
	}

	m_aButtons[ m_iClicked ][ m_bClickedSecondary ].SetWindowText( "Press key to detect..." );
}

CString CConfigDlg::GetButtonName( UINT Button, bool IsSecondary )
{
	switch( m_iInputMode )
	{
	case KEYBOARD:
		return VKToText( m_aButtonCodes[ Button ][ IsSecondary ] );
		break;
	case JOYSTICK:
		if( m_aJoystickCodes[ Button ][ IsSecondary ] >= 0 )
		{
			if( m_aJoystickCodes[ Button ][ IsSecondary ] >= m_aJoystickButtonNames.GetSize() )
				return "???";
			return *m_aJoystickButtonNames[ m_aJoystickCodes[ Button ][ IsSecondary ] ];
		}
		switch( m_aJoystickCodes[ Button ][ IsSecondary ] )
		{
		case NONE:				return "";
		case X_PLUS:			return "X +";
		case X_MINUS:			return "X -";
		case X_ROTATION_PLUS:	return "XR +";
		case X_ROTATION_MINUS:	return "XR -";
		case Y_PLUS:			return "Y +";
		case Y_MINUS:			return "Y -";
		case Y_ROTATION_PLUS:	return "YR +";
		case Y_ROTATION_MINUS:	return "YR -";
		case Z_PLUS:			return "Z +";
		case Z_MINUS:			return "Z -";
		case Z_ROTATION_PLUS:	return "ZR +";
		case Z_ROTATION_MINUS:	return "ZR -";
		case U_PLUS:			return "U +";
		case U_MINUS:			return "U -";
		case V_PLUS:			return "V +";
		case V_MINUS:			return "V -";
		case HAT_UP:			return "Hat Up";
		case HAT_RIGHT:			return "Hat Right";
		case HAT_DOWN:			return "Hat Down";
		case HAT_LEFT:			return "Hat Left";
		}
		break;
	}

	return "???";
}

void CConfigDlg::OnButtonLeft() 
{
	ButtonClicked( LEFT, false );
}

void CConfigDlg::OnButtonLeft2() 
{
	ButtonClicked( LEFT, true );
}

void CConfigDlg::OnButtonRight() 
{
	ButtonClicked( RIGHT, false );
}

void CConfigDlg::OnButtonRight2() 
{
	ButtonClicked( RIGHT, true );
}

void CConfigDlg::OnButtonAccelerate() 
{
	ButtonClicked( ACC, false );
}

void CConfigDlg::OnButtonAccelerate2() 
{
	ButtonClicked( ACC, true );
}

void CConfigDlg::OnButtonShoot() 
{
	ButtonClicked( SHOOT, false );
}

void CConfigDlg::OnButtonShoot2() 
{
	ButtonClicked( SHOOT, true );
}

void CConfigDlg::OnButtonHypSpc() 
{
	ButtonClicked( HYP_SPACE, false );
}

void CConfigDlg::OnButtonHypSpc2() 
{
	ButtonClicked( HYP_SPACE, true );
}

BOOL CConfigDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		if( m_iClicked != -1 && m_iInputMode == KEYBOARD )
		{
			CheckKeyboardInput();
			
			return TRUE;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_iClicked = m_iLastClicked = -1;
	m_bClickedSecondary = m_bLastClickedSecondary = false;
	
	UpdateAllButtons();

	m_oLastPrefferedDevice = m_oPrefferedDevice;

	m_bPressedOK = false;

	m_oScaleGame.SetCheck ( m_bScaleGame );
	m_oFullscreen.SetCheck( m_bFullscreen);
	m_oAntialiasing.SetCheck( m_bAntialiasing );

	g_oJoystick.SetHWND( m_hWnd );

	UpdateDevicesList();

	SetTimer( EV_CHECK_INPUT, 10, NULL );

	//::SendMessage(GetParent()->GetSafeHwnd(), WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)m_hWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDlg::EraseButtonEntry( UINT Button, bool IsSecondary /*= true*/ )
{
	if( m_iInputMode == KEYBOARD )
	{
		m_aButtonCodes[ Button ][ IsSecondary ] = 0;
	}
	else if( m_iInputMode == JOYSTICK )
	{
		m_aJoystickCodes[ Button ][ IsSecondary ] = NONE;
	}

	m_aButtons[ Button ][ IsSecondary ].SetWindowText( "" );

	m_iClicked = -1;
}

void CConfigDlg::OnButtonLeftErase() 
{
	EraseButtonEntry( LEFT );
}

void CConfigDlg::OnButtonRightErase()
{
	EraseButtonEntry( RIGHT );
}

void CConfigDlg::OnButtonAccelerateErase() 
{
	EraseButtonEntry( ACC );
}

void CConfigDlg::OnButtonShootErase() 
{
	EraseButtonEntry( SHOOT );
}

void CConfigDlg::OnButtonHypSpcErase() 
{
	EraseButtonEntry( HYP_SPACE );
}

BOOL CConfigDlg::DestroyWindow() 
{
	KillTimer( EV_CHECK_INPUT );

	for( int i = 0;i < m_aJoystickButtonNames.GetSize(); i++ ) delete m_aJoystickButtonNames[ i ];
	m_aJoystickButtonNames.RemoveAll();

	if( ! m_bPressedOK )
	{
		g_oJoystick.SetPreferredDevice( &m_oLastPrefferedDevice );
	}
	
	return CDialog::DestroyWindow();
}

void CConfigDlg::OnOK() 
{
	m_bPressedOK = true;
	
	m_bScaleGame	= m_oScaleGame.GetCheck();
	m_bFullscreen	= m_oFullscreen.GetCheck();
	m_bAntialiasing = m_oAntialiasing.GetCheck();
	
	CDialog::OnOK();
}

int CConfigDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	
	return 0;
}

void CConfigDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CConfigDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	if( nIDEvent == EV_CHECK_INPUT && m_iClicked != -1 )
	{
		switch( m_iInputMode )
		{
		case JOYSTICK: CheckJoystickInput();
			break;
		case KEYBOARD: //CheckKeyboardInput();
			break;
		}

	}
	
	CDialog::OnTimer(nIDEvent);
}

void CConfigDlg::UpdateDevicesList()
{
	g_oJoystick.Enumerate_Joysticks( TRUE );

	m_oInputDevices.ResetContent();

	m_oInputDevices.AddString( "Keyboard" );

	LPCDIDEVICEINSTANCE lpddi = g_oJoystick.GetFirstJoystickID();

	int GoodSel = -1;
 
	while(lpddi)
	{
		int x = m_oInputDevices.AddString( lpddi->tszInstanceName );

		m_oInputDevices.SetItemDataPtr( x, (void*)lpddi );

		if( lpddi->guidInstance == m_oPrefferedDevice )
			GoodSel = m_oInputDevices.GetCount() - 1;

		lpddi = g_oJoystick.GetNextJoystickID();
	}

	if( m_iInputMode == KEYBOARD ) m_oInputDevices.SetCurSel( 0 );
	else
	{
		if( GoodSel != -1 ) m_oInputDevices.SetCurSel( GoodSel );
		else m_oInputDevices.SetCurSel( min( 1, m_oInputDevices.GetCount() - 1 ) );
	}

	OnSelchangeInputDevices();
}


void CConfigDlg::OnSelchangeInputDevices() 
{
	int SelPos = m_oInputDevices.GetCurSel();

	if( SelPos == CB_ERR ) return;

	if( SelPos == 0 )
	{
		m_iInputMode = KEYBOARD;
	}
	else
	{
		for( int i = 0;i < m_aJoystickButtonNames.GetSize(); i++ ) delete m_aJoystickButtonNames[ i ];

		m_aJoystickButtonNames.RemoveAll();

		m_iInputMode = JOYSTICK;

		LPCDIDEVICEINSTANCE lpddi = (LPCDIDEVICEINSTANCE)m_oInputDevices.GetItemDataPtr( SelPos );

		if(lpddi && ((int)lpddi!=-1))
		{
			m_oPrefferedDevice = lpddi->guidInstance;

			g_oJoystick.SetPreferredDevice( &m_oPrefferedDevice );

			g_oJoystick.HowManyButtons();

			TCHAR* name = NULL;
			name = g_oJoystick.GetFirstButtonName();
 
			while(name)
			{
				m_aJoystickButtonNames.Add( new CString( name ) );

				name = g_oJoystick.GetNextButtonName();
			}
			
		}
	}

	UpdateAllButtons();
}

void CConfigDlg::UpdateAllButtons()
{
	for( int i = 0;i < KEY_BIND_QTY; i++ )
	{
		m_aButtons[ i ][ 0 ].SetWindowText( GetButtonName( i, 0 ) );
		m_aButtons[ i ][ 1 ].SetWindowText( GetButtonName( i, 1 ) );
	}

	m_iClicked = -1;
	m_bClickedSecondary = false;
}

void CConfigDlg::OnDropdownInputDevices() 
{	
	UpdateDevicesList();
}

int CConfigDlg::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::DoModal();
}
