// AsteroidsView.cpp : implementation of the CAsteroidsView class
//

#include "stdafx.h"
#include "Asteroids.h"

#include "AsteroidsDoc.h"
#include "AsteroidsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CALLBACK TimerHandler(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{ 
	CAsteroidsView *pView = (CAsteroidsView *) dwUser;

	if( pView->m_bSimulateGame && ! pView->m_bInTick )
	{
		pView->m_bInTick = TRUE;

		PostMessage( pView->GetSafeHwnd(), WM_GAMETICK, (WPARAM) 0, (LPARAM) 0 );
	}

}

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsView

IMPLEMENT_DYNCREATE(CAsteroidsView, CView)

BEGIN_MESSAGE_MAP(CAsteroidsView, CView)
	//{{AFX_MSG_MAP(CAsteroidsView)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_TIME_X2, OnTimeX2)
	ON_UPDATE_COMMAND_UI(ID_TIME_X2, OnUpdateTimeX2)
	ON_COMMAND(ID_TIME_X0_5, OnTimeX05)
	ON_UPDATE_COMMAND_UI(ID_TIME_X0_5, OnUpdateTimeX05)
	ON_COMMAND(ID_SHOW_COLLISION_BOXES, OnShowCollisionBoxes)
	ON_UPDATE_COMMAND_UI(ID_SHOW_COLLISION_BOXES, OnUpdateShowCollisionBoxes)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_GAMETICK, OnGameTick )
END_MESSAGE_MAP()

CDIJoystick g_oJoystick;

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsView construction/destruction

CAsteroidsView::CAsteroidsView()
{
	TIMECAPS tc;
	timeGetDevCaps( &tc, sizeof( TIMECAPS ) );
	//m_uiResolution = min( max( WANTED_RES, tc.wPeriodMin ), tc.wPeriodMax );
	m_uiResolution = tc.wPeriodMax;
	timeBeginPeriod( m_uiResolution );

	m_bInTick = FALSE;

	m_uiState = WAIT_FOR_START;

	m_iGameWndHeight = m_iGameWndWidth = m_iWindowHeight = m_iWindowWidth = 0;

	m_uiLastTick = 0;
	m_uiLastDraw = 0;

#ifdef _DEBUG
	for( int i = 0; i < TIME_MEASURE_CELLS; i++ ) m_iElapse[ i ] = 0;
	m_iPos = 0;
	m_iSum = 0;

	for( i = 0; i < TIME_MEASURE_CELLS; i++ ) m_iDrawElapse[ i ] = 0;
	m_iDrawPos = 0;
	m_iDrawSum = 0;
#endif

	m_dTimeMultiplayer = 1;

	m_uiCurrentBitmap = 0;

	m_bFirstDrawTime = m_bFirstTickTime = true;

	m_poGameObjs = NULL;

	m_dLevel = 0;

	m_dMinForUfo = m_dMinForAsteroid = 0;

	ExtractFromConfigFile();

	m_oStartButton  = CRect( round(0.3 * GAME_WIDTH ), round(0.7 * GAME_HEIGHT ), round(0.7 * GAME_WIDTH ) + 1, round(0.9 * GAME_HEIGHT ) + 1 );
	m_oConfigButton = CRect( round(0.75 * GAME_WIDTH), round(0.82 * GAME_HEIGHT), round(0.95 * GAME_WIDTH) + 1, round(0.9 * GAME_HEIGHT ) + 1 );
	m_oEndButton    = CRect( round(0.75 * GAME_WIDTH), round(0.7 * GAME_HEIGHT ), round(0.95 * GAME_WIDTH) + 1, round(0.78 * GAME_HEIGHT) + 1 );

	m_bCursorVisible = true;

	m_dConv = 1;

	m_bInitialized = false;
}

CAsteroidsView::~CAsteroidsView()
{
	timeKillEvent( m_oTimerID );
	timeEndPeriod( m_uiResolution );
	delete m_poGameObjs;

	InsertToConfigFile();
}

BOOL CAsteroidsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsView drawing

void CAsteroidsView::UpdateScreen()
{
	UpdateDrawTimes();

	CDC *pDC = GetWindowDC();

	COLORREF backgroundColor = RGB( 0, 0, 0 );

	int XOff = max( ( m_iWindowWidth  - m_iCurrentWidth  ) / 2, 0 );
	int YOff = max( ( m_iWindowHeight - m_iCurrentHeight ) / 2, 0 );

	CRect oRect0( 0, 0, m_iWindowWidth, YOff - 1 );
	pDC->FillSolidRect( &oRect0, backgroundColor );

	CRect oRect1( 0, YOff - 1, XOff - 1, YOff + m_iCurrentHeight + 1 );
	pDC->FillSolidRect( &oRect1, backgroundColor );

	CRect oRect2( XOff + m_iCurrentWidth + 1, YOff - 1, m_iWindowWidth, YOff + m_iCurrentHeight + 1 );
	pDC->FillSolidRect( &oRect2, backgroundColor );

	CRect oRect3( 0, YOff + m_iCurrentHeight + 1, m_iWindowWidth, m_iWindowHeight );
	pDC->FillSolidRect( &oRect3, backgroundColor );

	pDC->SelectStockObject( WHITE_PEN );
	pDC->SelectStockObject( NULL_BRUSH );
	pDC->SetBkMode( TRANSPARENT );
	pDC->Rectangle( XOff - 1, YOff - 1, XOff + m_iCurrentWidth + 1, YOff + m_iCurrentHeight + 1 );

	pDC->BitBlt( XOff, YOff, m_iCurrentWidth, m_iCurrentHeight, &m_oGameDC[ m_uiCurrentBitmap ], 0, 0, SRCCOPY );

#ifdef _DEBUG
	double tps = m_iSum     > 0 ? 1000.0 * TIME_MEASURE_CELLS / (double)m_iSum : 0;
	double ups = m_iDrawSum > 0 ? 1000.0 * TIME_MEASURE_CELLS / (double)m_iDrawSum : 0;

	CGdiObject *LastObj = pDC->SelectObject( &m_oScoreFont[ m_uiCurrentBitmap ] );
	pDC->SetTextColor( RGB( 255, 255, 255 ) );

	CString str;

	str.Format( "%.2f tps", tps );
	pDC->TextOut( 10, 10, str );

	str.Format( "%.2f ups", ups );
	pDC->TextOut( 10, 60, str );

	str.Format( "%d obj(s)", m_poGameObjs ? ( m_poGameObjs->m_aObjects.GetCount() + 1 ) : 0 );
	pDC->TextOut( 10, 110, str );

	str.Format( "%g", m_dLevel );
	pDC->TextOut( 10, 160, str );

	str.Format( "%d", m_poGameObjs ? m_poGameObjs->m_iEnemies : 0 );
	pDC->TextOut( 10, 210, str );

	str.Format( "Ast/s: %.2f", 1000 * ( m_dMinForAsteroid - m_dMinForUfo ) / ENEMY_GEN_MAX_RAND / CLOCK_DELAY );
	pDC->TextOut( 10, 260, str );

	str.Format( "UFO/s: %.2f", 1000 * m_dMinForUfo / ENEMY_GEN_MAX_RAND / CLOCK_DELAY );
	pDC->TextOut( 10, 310, str );

	pDC->SelectObject( LastObj );


#endif //_DEBUG

	ReleaseDC( pDC );
}

void CAsteroidsView::OnDraw(CDC* pDC)
{
	CAsteroidsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	UpdateScreen();

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsView diagnostics

#ifdef _DEBUG
void CAsteroidsView::AssertValid() const
{
	CView::AssertValid();
}

void CAsteroidsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAsteroidsDoc* CAsteroidsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAsteroidsDoc)));
	return (CAsteroidsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsView message handlers

void CAsteroidsView::GenerateEnemies( int TimeDiff )
{
	UINT Rand = Random( 0, ENEMY_GEN_MAX_RAND );

	double Seconds = ( m_iTicks * CLOCK_DELAY ) * 0.0001;

	m_dLevel = min( 7000 + Seconds * 15 + Seconds * Seconds * 10, 100000 );

	m_dMinForAsteroid = m_dLevel * TimeDiff / 16.6666;
	m_dMinForUfo = m_dLevel * m_dLevel * 0.000007 * TimeDiff / 16.6666;

	if( Rand <= m_dMinForAsteroid || m_poGameObjs->m_iEnemies == 0 )
	{
		CCoord NewObjPos;
		CCoord NewObjSpeed = CCoord( Random( -3000, 3000 ) * 0.00003, Random( -3000, 3000 ) * 0.00003 );

		if( Random( 0, 1 ) )  NewObjPos.x = Random( 0, 10000 ) % GAME_WIDTH;
		else				  NewObjPos.y = Random( 0, 10000 ) % GAME_HEIGHT;

		CGameObj *pNewObj;
		if( Rand <= m_dMinForUfo )
		{
			pNewObj = Random( -1, 2 ) >= 0 ?	new CUfo( NewObjPos, CCoord( 0, 0 ), 1 )   : 
												new CUfo( NewObjPos, NewObjSpeed, 0 );
		}
		else pNewObj = new CAsteroid( NewObjPos, NewObjSpeed );

		
		m_poGameObjs->AddObj( pNewObj );
	}
}

void CAsteroidsView::GameTick()
{
	if( m_bGame ) UpdateControlls();

	UINT TimeDiff = (UINT)( UpdateTickTimes() * m_dTimeMultiplayer );

	if( m_bEnemyGen && m_poGameObjs->m_iState == STATE_NORMAL ) GenerateEnemies( TimeDiff );

	m_poGameObjs->Tick( TimeDiff );

	if( m_bGame && m_poGameObjs->m_iState == STATE_NORMAL ) m_iTicks++;

	if( m_poGameObjs->m_oShip.m_iBullets < 3 && m_poGameObjs->m_iScore >= 10000 * m_poGameObjs->m_oShip.m_iBullets * m_poGameObjs->m_oShip.m_iBullets )
		m_poGameObjs->m_oShip.m_iBullets++;
}

void CAsteroidsView::OnTimer(UINT nIDEvent) 
{
	switch( nIDEvent )
	{
	case EV_TICK:
		UINT NewBitmap = NextBitmap();

		CAsteroidsDoc *pDoc = GetDocument();
		
		switch( m_uiState )
		{
		case COUNTDOWN:
			if( m_iTicks >= 0 )
			{
				m_uiState = GAME;
				KillTimer( EV_TICK );
				//SetTimer( EV_TICK, 1000 / TPS, NULL );
				m_iTicks = 0;
				m_uiStartGame = timeGetTime();
				m_sndMusic.Play( 0, TRUE );

				InitGame();

				break;
			}

			m_oGameDC[ NewBitmap ].SelectObject( &m_oBigFont[ NewBitmap ] );

			if( m_iTicks >= -3 )
			{
				CString str;
				str.Format( "%d", - m_iTicks );
				m_oGameDC[ NewBitmap ].TextOut( (int)(0.5 * m_iGameWndWidth), (int)(0.44 * m_iGameWndHeight), str );
			}
			break;
		case GAME_OVER:
			StopSim();
			KillTimer( EV_TICK );

			if( pDoc->IsNewRecord( m_poGameObjs->m_iScore ) )
			{
				m_uiState = SUBMIT_RECORD;
				m_sInitials = "";

				m_oGameDC[ NewBitmap ].SelectObject( &m_oScoreFont[ NewBitmap ] );
				m_oGameDC[ NewBitmap ].TextOut( (int)(0.5 * m_iGameWndWidth), (int)(0.40 * m_iGameWndHeight), "You've got score which is in top 10" );
				m_oGameDC[ NewBitmap ].TextOut( (int)(0.5 * m_iGameWndWidth), (int)(0.45 * m_iGameWndHeight), "Enter your initials to submit score" );
				m_oGameDC[ NewBitmap ].TextOut( (int)(0.5 * m_iGameWndWidth), (int)(0.55 * m_iGameWndHeight), "_ _ _" );
			}
			else
			{
				GenerateStartScreen( NewBitmap );

				InitStartScreen();

				m_uiState = WAIT_FOR_START;
				Restart();

				InitStartScreen();
			}

			break;
		}

		m_iTicks++;

		SelectBitmap( NewBitmap );
		
		#if UPDATE_SCREEN_ON_TICK
			UpdateScreen();
		#else
			Invalidate( FALSE );
		#endif
		break;

	}
	
	CView::OnTimer(nIDEvent);
}

int CAsteroidsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CAsteroidsView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_uiState == WAIT_FOR_START )
	{
		point.x -= max( ( m_iWindowWidth  - m_iCurrentWidth  ) / 2, 0 );
		point.y -= max( ( m_iWindowHeight - m_iCurrentHeight ) / 2, 0 );

		point.x = round( point.x / m_dConv );
		point.y = round( point.y / m_dConv );

		if( m_oStartButton.PtInRect( point ) )
		{
			StartCountDown();
		}
		else if( m_oConfigButton.PtInRect( point ) )
		{
			InitConfig();
			
#if STOP_SIM_IN_DIALOG
			bool SG = m_bSimulateGame;
			m_bSimulateGame = false;
#endif
			if( m_wndConfig.DoModal() == IDOK ) SaveConfigChanges();

			g_oJoystick.SetHWND( AfxGetMainWnd()->m_hWnd );

#if STOP_SIM_IN_DIALOG
			m_bSimulateGame = SG;
#endif
		}
		else if( m_oEndButton.PtInRect( point ) )
		{
#if STOP_SIM_IN_DIALOG
			bool SG = m_bSimulateGame;
			m_bSimulateGame = false;
#endif

			if( m_wndEndApp.DoModal() == IDYES )
			{
				EndApp();
				return;
			}

#if STOP_SIM_IN_DIALOG
			m_bSimulateGame = SG;
#endif
		}
	}
	
	CView::OnLButtonDown(nFlags, point);
}

void CAsteroidsView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	CalcSizes();
}

BOOL CAsteroidsView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
	return CView::OnEraseBkgnd(pDC);
}

void CAsteroidsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( m_uiState != SUBMIT_RECORD )
	{
		if( m_uiState == WAIT_FOR_START && nChar == VK_RETURN )
		{
			StartCountDown();
		}
	}
	else
	{
		AddCharToScbEntry( nChar );
	}
	
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAsteroidsView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

LRESULT CAsteroidsView::OnGameTick( WPARAM wParam, LPARAM lParam )
{
	if( m_bSimulateGame )
	{
		GameTick();

		if( m_bGame )
		{
			if( m_poGameObjs->m_bEndGame )
			{
				if( m_uiState != GAME_OVER )
				{
					m_uiState = GAME_OVER;
					//timeKillEvent( m_oTimerID );
					m_oTimerID = 0;

					SetTimer( EV_TICK, 4000, NULL );

					m_sndMusic.Stop();
					m_sndGameOver.Play();
				}

				UINT NewBitmap = DrawGame();

				m_oGameDC[ NewBitmap ].SelectObject( &m_oBigFont[ NewBitmap ] );
				m_oGameDC[ NewBitmap ].TextOut( round( 0.5 * GAME_WIDTH * m_dConv ), round( 0.44 * GAME_HEIGHT * m_dConv ), "Game Over" );

				SelectBitmap( NewBitmap );
			}
			else SelectBitmap( DrawGame() );
		}
		else
		{
			SelectBitmap( DrawGame( GenerateStartScreen() ) );
		}

		m_bInTick = FALSE;

		#if UPDATE_SCREEN_ON_TICK
			UpdateScreen();
		#else
			Invalidate( FALSE );
		#endif
	}

	return 0;
}

UINT CAsteroidsView::UpdateTickTimes()
{
	UINT Time = timeGetTime();

	UINT TimeDiff = m_bFirstTickTime ? Time - m_uiStartGame : Time - m_uiLastTick;

	m_bFirstTickTime = false;

#ifdef _DEBUG
	m_iSum += TimeDiff - m_iElapse[ m_iPos ];
	m_iElapse[ m_iPos++ ] = TimeDiff;
	if( m_iPos >= TIME_MEASURE_CELLS ) m_iPos = 0;
#endif

	m_uiLastTick = Time;

	return TimeDiff;
}

UINT CAsteroidsView::UpdateDrawTimes()
{
	UINT Time = timeGetTime();

	UINT TimeDiff = m_bFirstDrawTime ? Time - m_uiStartGame : Time - m_uiLastDraw;

	m_bFirstDrawTime = false;

#ifdef _DEBUG
	m_iDrawSum += TimeDiff - m_iDrawElapse[ m_iDrawPos ];
	m_iDrawElapse[ m_iDrawPos++ ] = TimeDiff;
	if( m_iDrawPos >= TIME_MEASURE_CELLS ) m_iDrawPos = 0;
#endif

	m_uiLastDraw = Time;

	return TimeDiff;
}

void CAsteroidsView::OnTimeX2() 
{
	m_dTimeMultiplayer = m_dTimeMultiplayer == 2 ? 1 : 2;
}

void CAsteroidsView::OnUpdateTimeX2(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dTimeMultiplayer == 2.0 );
}

void CAsteroidsView::OnTimeX05() 
{
	m_dTimeMultiplayer = m_dTimeMultiplayer == 0.5 ? 1 : 0.5;
}

void CAsteroidsView::OnUpdateTimeX05(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dTimeMultiplayer == 0.5 );
}

void CAsteroidsView::Restart()
{
	m_poGameObjs->Restart();
	m_bInTick = FALSE;
	m_dTimeMultiplayer = 1;
	m_bFirstDrawTime = m_bFirstTickTime = true;
}

int CAsteroidsView::GenerateStartScreen( int Bitmap /*= -1*/ )
{
	if( Bitmap < 0 )
	{
		Bitmap = NextBitmap();
	}

	m_oGameDC[ Bitmap ].SetTextAlign( TA_CENTER | TA_BASELINE );
	m_oGameDC[ Bitmap ].SelectObject( &m_oBigFont[ Bitmap ] );

	m_oGameDC[ Bitmap ].TextOut( (int)(0.5 * m_iGameWndWidth), (int)(0.15 * m_iGameWndHeight), "Asteroids" );

	CRect Start = ScaleRect( m_oStartButton, m_dConv );
	m_oGameDC[ Bitmap ].Rectangle( &Start );
	m_oGameDC[ Bitmap ].TextOut( Start.CenterPoint().x, round( Start.top + Start.Height() * 0.65 ), "Start" );

	m_oGameDC[ Bitmap ].SelectObject( &m_oScoreFont[ Bitmap ] );

	CRect Conf = ScaleRect( m_oConfigButton, m_dConv );
	m_oGameDC[ Bitmap ].Rectangle( &Conf );
	m_oGameDC[ Bitmap ].TextOut( Conf.CenterPoint().x, round( Conf.top + Conf.Height() * 0.65 ), "Config" );

	CRect End = ScaleRect( m_oEndButton, m_dConv );
	m_oGameDC[ Bitmap ].Rectangle( &End );
	m_oGameDC[ Bitmap ].TextOut( End.CenterPoint().x, round( End.top + End.Height() * 0.65 ), "Exit" );

	//CRect oScoreboardRect( (int)(0.3 * GAME_WIDTH), (int)(0.2 * GAME_HEIGHT), (int)(0.7 * GAME_WIDTH) + 1, (int)(0.65 * GAME_HEIGHT) + 1 );
	//m_oGameDC[ NewBitmap ].Rectangle( &oScoreboardRect );

	m_oGameDC[ Bitmap ].SelectObject( &m_oScoreboardFont[ Bitmap ] );

	CAsteroidsDoc *pDoc = GetDocument();

	int i = 0;
	CString NameText;
	CString ScoreText;
	CString ScoreCell;
	CScoreboardEntry *pEntry;

	CRect oScoreboardRect( (int)(0.3 * m_iGameWndWidth), (int)(0.21 * m_iGameWndHeight), (int)(0.7 * m_iGameWndWidth) + 1, (int)(0.64 * m_iGameWndHeight) + 1 );
	//oScoreboardRect.SetRect( oScoreboardRect.left + (int)(0.1 * GAME_WIDTH), oScoreboardRect.top + (int)(0.05 * GAME_HEIGHT), oScoreboardRect.right - (int)(0.1 * GAME_WIDTH), oScoreboardRect.bottom - (int)(0.05 * GAME_HEIGHT) );
	//m_oGameDC[ NewBitmap ].Rectangle( oScoreboardRect );

	CRect oNameCol( oScoreboardRect.TopLeft(), CPoint( oScoreboardRect.left + (int)(0.1 * m_iGameWndWidth), oScoreboardRect.bottom ) );
	//m_oGameDC[ NewBitmap ].Rectangle( oNameCol );

	CRect oScoreCol( CPoint( oNameCol.right, oNameCol.top ), oScoreboardRect.BottomRight() );
	//m_oGameDC[ NewBitmap ].Rectangle( oScoreCol );

	if( ! pDoc->m_aScoreboard.IsEmpty() )
	{
		POSITION pos = pDoc->m_aScoreboard.GetHeadPosition();

		do
		{
			pEntry = pDoc->m_aScoreboard.GetNext( pos );

			NameText += pEntry->m_sName + "\r\n";
			ScoreCell.Format( "%d\r\n", pEntry->m_iScore );
			ScoreText += ScoreCell;
			i++;
		}
		while( pos );		
	}

	m_oGameDC[ Bitmap ].SetTextAlign( TA_CENTER );
	m_oGameDC[ Bitmap ].DrawText( NameText, &oNameCol , DT_CENTER | DT_NOCLIP );

	m_oGameDC[ Bitmap ].SetTextAlign( TA_LEFT );
	m_oGameDC[ Bitmap ].DrawText( ScoreText, &oScoreCol , DT_LEFT | DT_NOCLIP );

	m_oGameDC[ Bitmap ].SetTextAlign( TA_CENTER | TA_TOP );

	return Bitmap;
}

void CAsteroidsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( m_uiState == WAIT_FOR_START ) SelectBitmap( GenerateStartScreen() );

	Invalidate( FALSE );
}

void CAsteroidsView::OnShowCollisionBoxes() 
{
	m_poGameObjs->m_bShowCollisionBoxes = !m_poGameObjs->m_bShowCollisionBoxes;
}

void CAsteroidsView::OnUpdateShowCollisionBoxes(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_poGameObjs->m_bShowCollisionBoxes );
}

void CAsteroidsView::OnInitialUpdate() 
{
	if( !m_bInitialized )
	{
		g_oJoystick.SetHWND( AfxGetMainWnd()->m_hWnd );

		g_oJoystick.Enumerate_Joysticks( TRUE );

		LPCDIDEVICEINSTANCE lpdi = g_oJoystick.GetFirstJoystickID();

		if( lpdi )
		{
			LPCDIDEVICEINSTANCE lpFirstDi = lpdi;

			bool found = false;

			do
			{
				if( lpdi->guidInstance == m_oPrefferedDevice )
				{
					found = true;
					lpdi = NULL;
				}
				else lpdi = g_oJoystick.GetNextJoystickID();
			}
			while( lpdi );

			if( found ) g_oJoystick.SetPreferredDevice( &m_oPrefferedDevice );
			else
			{
				m_oPrefferedDevice = lpFirstDi->guidInstance;
				g_oJoystick.SetPreferredDevice( &m_oPrefferedDevice );
			}
		}
		else
		{
			m_iInputMode = KEYBOARD;
		}

		UpdateBorder();

		m_poGameObjs = new CGameObjsArray;

		m_oGamePen.CreatePen( PS_SOLID, 1, RGB( 255, 255, 255 ) );

		CDC *pDC = GetWindowDC();

		for( int i = 0; i < BUFFERS; i++ )
		{
			m_oGameDC[ i ].CreateCompatibleDC( pDC );

			InitBitmap( i );

			m_oGameDC[ i ].SelectStockObject( WHITE_PEN );
			m_oGameDC[ i ].SelectStockObject( NULL_BRUSH );

			m_oGameDC[ i ].SetBkMode( TRANSPARENT );

			m_oGameDC[ i ].SetTextAlign( TA_CENTER | TA_TOP );
			m_oGameDC[ i ].SetTextColor( RGB( 255, 255, 255 ) );
		}

		ReleaseDC( pDC );

		SelectBitmap( GenerateStartScreen() );
		
		m_uiState = WAIT_FOR_START;

		#if UPDATE_SCREEN_ON_TICK
			UpdateScreen();
		#else
			Invalidate( FALSE );
		#endif	
		
		m_sndMusic.CreateFromFile( "snd\\cosmicmusic.wav" );
		m_sndGameOver.CreateFromFile( "snd\\gameover.wav");

		InitStartScreen();
		m_oTimerID = timeSetEvent( CLOCK_DELAY, m_uiResolution, TimerHandler, (ULONG)this, TIME_PERIODIC );

		m_bInitialized = true;
	}

	CView::OnInitialUpdate();
}

void CAsteroidsView::InitConfig()
{
	for( int i = 0; i < KEY_BIND_QTY; i++ )
	{
		m_wndConfig.m_aButtonCodes[ i ][ 0 ] = m_aActions[ i ][ 0 ];
		m_wndConfig.m_aButtonCodes[ i ][ 1 ] = m_aActions[ i ][ 1 ];

		m_wndConfig.m_aJoystickCodes[ i ][ 0 ] = m_aJoyStickActions[ i ][ 0 ];
		m_wndConfig.m_aJoystickCodes[ i ][ 1 ] = m_aJoyStickActions[ i ][ 1 ];
	}

	m_wndConfig.m_bScaleGame = m_bScaleGame;
	m_wndConfig.m_bFullscreen = m_bFullscreen;

	m_wndConfig.m_oPrefferedDevice = m_oPrefferedDevice;

	m_wndConfig.m_iInputMode = m_iInputMode;
}

void CAsteroidsView::SaveConfigChanges()
{
	for( int i = 0; i < KEY_BIND_QTY; i++ )
	{
		m_aActions[ i ][ 0 ] = m_wndConfig.m_aButtonCodes[ i ][ 0 ];
		m_aActions[ i ][ 1 ] = m_wndConfig.m_aButtonCodes[ i ][ 1 ];

		m_aJoyStickActions[ i ][ 0 ] = m_wndConfig.m_aJoystickCodes[ i ][ 0 ];
		m_aJoyStickActions[ i ][ 1 ] = m_wndConfig.m_aJoystickCodes[ i ][ 1 ];
	}

	if( m_bScaleGame != m_wndConfig.m_bScaleGame )
	{
		m_bScaleGame = m_wndConfig.m_bScaleGame;
		CalcSizes();
	}

	if( m_bFullscreen != m_wndConfig.m_bFullscreen )
	{
		m_bFullscreen = m_wndConfig.m_bFullscreen;

		UpdateBorder();

		CalcSizes();
	}

	m_oPrefferedDevice = m_wndConfig.m_oPrefferedDevice;

	m_iInputMode = m_wndConfig.m_iInputMode;
}

void CAsteroidsView::ExtractFromConfigFile()
{
	m_aActions[ LEFT  ]		[ 0 ]	= 'A';
	m_aActions[ RIGHT ]		[ 0 ]	= 'D';
	m_aActions[ ACC   ]		[ 0 ]	= 'W';
	m_aActions[ SHOOT ]		[ 0 ]	= VK_SPACE;
	m_aActions[ HYP_SPACE ]	[ 0 ]	= VK_RETURN;

	m_aActions[ LEFT  ]		[ 1 ]	= VK_LEFT;
	m_aActions[ RIGHT ]		[ 1 ]	= VK_RIGHT;
	m_aActions[ ACC   ]		[ 1 ]	= VK_UP;
	m_aActions[ SHOOT ]		[ 1 ]	= VK_CONTROL;
	m_aActions[ HYP_SPACE ]	[ 1 ]	= VK_ESCAPE;

	m_aJoyStickActions[ LEFT  ]		[ 0 ]	= X_MINUS;
	m_aJoyStickActions[ RIGHT ]		[ 0 ]	= X_PLUS;
	m_aJoyStickActions[ ACC   ]		[ 0 ]	= Y_MINUS;
	m_aJoyStickActions[ SHOOT ]		[ 0 ]	= 0;
	m_aJoyStickActions[ HYP_SPACE ]	[ 0 ]	= 1;

	m_aJoyStickActions[ LEFT  ]		[ 1 ]	= Z_MINUS;
	m_aJoyStickActions[ RIGHT ]		[ 1 ]	= Z_PLUS;
	m_aJoyStickActions[ ACC   ]		[ 1 ]	= Z_ROTATION_MINUS;
	m_aJoyStickActions[ SHOOT ]		[ 1 ]	= 4;
	m_aJoyStickActions[ HYP_SPACE ]	[ 1 ]	= 5;

	m_bScaleGame	= true;
	m_bFullscreen	= true;

	m_iInputMode = KEYBOARD;
	
	FILE *of = fopen( "dat\\Config.cnf", "r" );

	if( of )
	{
		CString str;
		CString WholeText;

		char *pBuffer = str.GetBuffer( 1024 );

		while( fread( pBuffer, sizeof( char ), 1024, of ) )
		{
			WholeText += pBuffer;
		}

		fclose( of );

		int iBlock = 0;
		CString sBlock;

		while( iBlock < NUM_OF_BLOCKS && AfxExtractSubString( sBlock, WholeText, iBlock++, '~' ) )
		{
			int i = 0, Mode = 0;
			CString str0, ModeStr;

			switch( iBlock - 1 )
			{
			case KEY_BINDINGS:
				Mode = 0;
				

				while( Mode < NUM_OF_MODES && AfxExtractSubString( ModeStr, sBlock, Mode++, '|' ) )
				{
					int i = 0;

					while( i < KEY_BIND_QTY && AfxExtractSubString( str0, ModeStr, i++, ';' ) )
					{
							int j = 0;
							CString str1;

							while( j < 2 && AfxExtractSubString( str1, str0, j++, ',' ) )
							{
								int a = atoi( str1 );

								switch( Mode - 1 )
								{
								case KEYBOARD:
									m_aActions[ i - 1 ][ j - 1 ] = a;
									break;
								case JOYSTICK:
									m_aJoyStickActions[ i - 1 ][ j - 1 ] = a;
									break;
								}
							}
					}
				}
				break;
			case VISUAL:
				i = 0;

				while( i < VISUAL_QTY && AfxExtractSubString( str0, sBlock, i++, ';' ) )
				{
					switch( i - 1 )
					{
					case SCALE_GAME:
						m_bScaleGame = atoi( str0 );
						break;
					case FULLSCREEN:
						m_bFullscreen = atoi( str0 );
						break;
					}
				}
				break;
			case INPUT_SOURCE:
				i = 0;

				while( i < INPUT_SOURCE_QTY && AfxExtractSubString( str0, sBlock, i++, ';' ) )
				{
					switch( i - 1 )
					{
					case INPUT_MODE:
						m_iInputMode = atoi( str0 );
						break;
					case JOY_GUID:
						UuidFromString( (unsigned char *)str0.GetBuffer( str0.GetLength() ), &m_oPrefferedDevice );
						break;
					}
				}
				break;
			}

		}
	}
}

void CAsteroidsView::InsertToConfigFile()
{
	FILE *of = fopen( "dat\\Config.cnf", "w" );

	if( of )
	{
		CString FileStr;

		CString CellStr;

		int i;
		

		//KEY BINDINGS -> KEYBOARD
		for( i = 0; i < KEY_BIND_QTY; i++ )
		{
			CellStr.Format( "%u,%u;", m_aActions[ i ][ 0 ], m_aActions[ i ][ 1 ] );
			FileStr += CellStr;
		}
		FileStr.SetAt( FileStr.GetLength() - 1, '|' );
		
		//KEY BINDINGS -> JOYSTICK
		for( i = 0; i < KEY_BIND_QTY; i++ )
		{
			CellStr.Format( "%d,%d;", m_aJoyStickActions[ i ][ 0 ], m_aJoyStickActions[ i ][ 1 ] );
			FileStr += CellStr;
		}
		FileStr.SetAt( FileStr.GetLength() - 1 , '~' );

		//VISUAL
		CellStr.Format( "%u;%u~", m_bScaleGame, m_bFullscreen );
		FileStr += CellStr;

		//INPUT SOURCE
		CellStr.Format( "%i;", m_iInputMode  );
		FileStr += CellStr;

		_TUCHAR * guidStr;
		UuidToString( &m_oPrefferedDevice, &guidStr);

		CellStr.Format( "%s", guidStr );
		FileStr += CellStr;

		RpcStringFree( &guidStr );

		fwrite( FileStr, sizeof( char ), FileStr.GetLength(), of );

		fclose( of );
	}
}

void CAsteroidsView::StartCountDown()
{
	StopSim();

	m_uiState = COUNTDOWN;
	m_iTicks = -3;

	UINT NewBitmap = NextBitmap();

	SelectBitmap( NewBitmap );

	#if UPDATE_SCREEN_ON_TICK
		UpdateScreen();
	#else
		Invalidate( FALSE );
	#endif

	m_bCursorVisible = false;

	::SetCursor( NULL );

	SetTimer( EV_TICK, 1000, NULL );
}

void CAsteroidsView::InitGame()
{
	m_bGame = true;

	Restart();

	m_bEnemyGen = true;
	m_bShowScore = true;

	m_poGameObjs->m_bInGame = true;

	m_uiStartGame = timeGetTime();
	m_bSimulateGame = true;
}

void CAsteroidsView::InitStartScreen()
{
	m_bCursorVisible = true;
	::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_CROSS ) );

	m_bGame = false;

	Restart();
	m_poGameObjs->m_oShip.m_bForDestruction = true;

	for( int i = 0;i < 10; i++ ) m_poGameObjs->AddObj( new CAsteroid( CCoord( Random( 0, GAME_WIDTH ), Random( 0, GAME_HEIGHT ) ), CCoord( Random( -3000, 3000 ) * 0.00003, Random( -3000, 3000 ) * 0.00003 ) ) );

	m_bEnemyGen = false;
	m_bShowScore = false;

	m_poGameObjs->m_bInGame = false;

	m_uiStartGame = timeGetTime();
	m_bSimulateGame = true;
}

void CAsteroidsView::StopSim()
{
	m_bSimulateGame = false;
	m_bGame = false;
	m_poGameObjs->m_bInGame = false;
}

void CAsteroidsView::UpdateControlls()
{
	if( m_iInputMode == KEYBOARD )
	{
		BYTE Keyboard[ 256 ];

		if( GetKeyboardState( Keyboard ) )
		{
			for( int i = 0; i < SHIP_KEY_BIND_QTY; i++ )
			{
				m_poGameObjs->m_oShip.m_aControls[ i ] = 
					0x80 & Keyboard[ m_aActions[ i + SHIP_KEY_BIND_START ][ 0 ] ] || 
					0x80 & Keyboard[ m_aActions[ i + SHIP_KEY_BIND_START ][ 1 ] ];
			}
		}
	}
	else
	{
		if( g_oJoystick.PollDevice() )
		{
			for( int i = 0; i < SHIP_KEY_BIND_QTY; i++ )
			{
				m_poGameObjs->m_oShip.m_aControls[ i ] =
					IsJoystickControllActive( m_aJoyStickActions[ i + SHIP_KEY_BIND_START ][ 0 ] ) ||
					IsJoystickControllActive( m_aJoyStickActions[ i + SHIP_KEY_BIND_START ][ 1 ] );
			}
		}		
	}
}

void CAsteroidsView::EndApp()
{
	PostMessage( WM_QUIT );
	AfxGetMainWnd()->DestroyWindow();
}

BOOL CAsteroidsView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default

	if( m_bCursorVisible )
		::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_CROSS ) );
	else
		::SetCursor( NULL );

	return TRUE;
	
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CAsteroidsView::CalcSizes()
{
	CRect oRect;

	GetWindowRect( &oRect );

	m_iWindowWidth  = oRect.Width();
	m_iWindowHeight = oRect.Height();

	if( m_bScaleGame )
	{
		if( m_iWindowHeight != 0 )
		{
			if( m_iWindowHeight * GAME_ASPECT < m_iWindowWidth )
			{
				m_iGameWndHeight = m_iWindowHeight;
				m_iGameWndWidth = round( m_iWindowHeight * GAME_ASPECT );
			}
			else
			{
				m_iGameWndWidth = m_iWindowWidth;
				m_iGameWndHeight = round( m_iWindowWidth / GAME_ASPECT );
			}
		}
	}
	else
	{
		m_iGameWndWidth = GAME_WIDTH;
		m_iGameWndHeight = GAME_HEIGHT;
	}

	if( m_bInitialized ) 
	{
		for( int i = 0; i < BUFFERS; i++ )
		{
			m_bCorrectSize[ i ] = false;
		}
	}

	m_dConv = 1.0 * m_iGameWndWidth / GAME_WIDTH;
}

int CAsteroidsView::DrawGame( int Bitmap /*= -1*/ )
{
	if( Bitmap < 0 )
	{
		Bitmap = NextBitmap();
	}

	CPen *LastPen = m_oGameDC[ Bitmap ].SelectObject( &m_oGamePen );

	m_poGameObjs->Draw( &m_oGameDC[ Bitmap ], &m_oScoreFont[ Bitmap ], m_bShowScore, m_dConv );

	m_oGameDC[ Bitmap ].SelectObject( LastPen );

	return Bitmap;
}

void CAsteroidsView::InitBitmap( int Btmp )
{
	CDC *pDC = GetWindowDC();

	m_oGameBitmap[ Btmp ].DeleteObject();

	m_oGameBitmap[ Btmp ].CreateCompatibleBitmap( pDC, m_iGameWndWidth, m_iGameWndHeight );

	m_oGameDC[ Btmp ].SelectObject( &m_oGameBitmap[ Btmp ] );

	m_oGameDC[ Btmp ].SelectStockObject( SYSTEM_FONT );

	UpdateFonts( Btmp );

	m_bCorrectSize[ Btmp ] = true;

	ReleaseDC( pDC );
}

void CAsteroidsView::UpdateFonts( int iSet )
{
	m_oBigFont[ iSet ]			.DeleteObject();
	m_oScoreFont[ iSet ]		.DeleteObject();
	m_oScoreboardFont[ iSet ]	.DeleteObject();

	m_oBigFont[ iSet ]			.CreatePointFont( round( m_iGameWndHeight * 0.1   * 30 / 4 ), "tahoma", &m_oGameDC[ iSet ] );
	m_oScoreFont[ iSet ]		.CreatePointFont( round( m_iGameWndHeight * 0.05  * 30 / 4 ), "tahoma", &m_oGameDC[ iSet ] );
	m_oScoreboardFont[ iSet ]	.CreatePointFont( round( m_iGameWndHeight * 0.035 * 30 / 4 ), "tahoma", &m_oGameDC[ iSet ] );
}

UINT CAsteroidsView::NextBitmap()
{
	UINT NextBitmap = m_uiCurrentBitmap + 1 >= BUFFERS ? 0 : m_uiCurrentBitmap + 1;

	if( ! m_bCorrectSize[ NextBitmap ] )
	{
		InitBitmap( NextBitmap );
	}

	m_oGameDC[ NextBitmap ].FillSolidRect( 0, 0, m_iGameWndWidth, m_iGameWndHeight, RGB( 0, 0, 0 ) );

	return NextBitmap;
}

void CAsteroidsView::SelectBitmap( UINT Bitmap )
{
	m_uiCurrentBitmap = Bitmap;

	m_iCurrentWidth  = m_iGameWndWidth;
	m_iCurrentHeight = m_iGameWndHeight;
}

void CAsteroidsView::AddCharToScbEntry( UINT nChar )
{
	if( nChar != VK_RETURN )
	{
		int iLen = m_sInitials.GetLength();

		if( nChar >= '0' && nChar <= 'Z' && iLen < 3 )
		{
			m_sInitials += nChar;
			iLen++;
		}
		else if( nChar == VK_BACK && iLen > 0 )
		{
			m_sInitials = m_sInitials.Left( --iLen );
		}

		CString str;
		CString Cell;

		for( int i = 0; i < 3; i++ )
		{
			Cell.Format( "%c ", i < iLen ? m_sInitials[ i ] : '_' );
			str += Cell;
		}

		str.Delete( str.GetLength() - 1 );
		
		UINT NewBitmap = NextBitmap();

		m_oGameDC[ NewBitmap ].SelectObject( &m_oScoreFont[ NewBitmap ] );
		m_oGameDC[ NewBitmap ].TextOut( (int)(0.5 * m_iGameWndWidth), (int)(0.40 * m_iGameWndHeight), "You've got score which is in top 10" );
		m_oGameDC[ NewBitmap ].TextOut( (int)(0.5 * m_iGameWndWidth), (int)(0.45 * m_iGameWndHeight), "Enter your initials to submit score" );
		m_oGameDC[ NewBitmap ].TextOut( (int)(0.5 * m_iGameWndWidth), (int)(0.55 * m_iGameWndHeight), str );

		SelectBitmap( NewBitmap );

		Invalidate( FALSE );
	}
	else
	{
		CAsteroidsDoc *pDoc = GetDocument();

		pDoc->AddScoreboardEntry( new CScoreboardEntry( m_sInitials, m_poGameObjs->m_iScore ) );
		m_uiState = WAIT_FOR_START;
		Restart();

		SelectBitmap( GenerateStartScreen() );

		Invalidate( FALSE );

		//pDoc->SetModifiedFlag( TRUE );
		pDoc->m_bChanged = true;

		InitStartScreen();
	}
}

void CAsteroidsView::UpdateBorder()
{
	CWnd *pMainWnd = AfxGetMainWnd();

	pMainWnd->SetRedraw( FALSE );

	pMainWnd->ShowWindow( SW_HIDE );

	if( m_bFullscreen ) 
	{
		pMainWnd->ModifyStyle( BORDER_FLAGS, 0 );

		pMainWnd->ShowWindow( SW_MAXIMIZE );
	}
	else
	{
		pMainWnd->ModifyStyle( 0, BORDER_FLAGS );

		pMainWnd->ShowWindow( SW_MAXIMIZE );
		pMainWnd->ShowWindow( SW_RESTORE );
	}

	pMainWnd->SetRedraw( TRUE );
}
