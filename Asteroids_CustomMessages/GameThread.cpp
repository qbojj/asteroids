// GameThread.cpp: implementation of the CGameThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "asteroids.h"
#include "GameThread.h"

#include "Asteroidsview.h"
#include "MsgWrapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameThread::CGameThread()
{
	m_bEnemyGen = false;
	m_bShowScore = false;
	m_dConv = 1;
	m_uiTimeDiff = 0;
	m_uiGameLen = 0;
	m_uiDCNum = 0;

	m_hMessageDest = NULL;
	m_bInside = false;
	m_bThreadExists = false;

	m_pGameBitmap = NULL;
	m_pGameObjsArray = NULL;
	m_pThread = NULL;

	m_oGameDC.CreateCompatibleDC( NULL );

	m_oGameFont.CreatePointFont( round( 0.05  * 30 / 4 * GAME_HEIGHT ), "tahoma" );
	m_oGameDC.SelectObject( &m_oGameFont );
	
	m_oGamePen.CreatePen( PS_SOLID, 1, GAME_COLOR );
	m_oGameDC.SelectObject( &m_oGamePen );
	m_dLastConv = 1;

	m_oGameDC.SetBkMode( TRANSPARENT );

	m_oGameDC.SetTextAlign( TA_CENTER | TA_TOP );
	m_oGameDC.SetTextColor( RGB( 255, 255, 255 ) );

	m_oGameDC.SelectStockObject( NULL_BRUSH );
}

void CGameThread::StartThread()
{
	if( !m_bThreadExists ) m_pThread = AfxBeginThread( ThreadFunction, this );

	m_bThreadExists = true;
}

CGameThread::~CGameThread()
{
	m_oGameDC.DeleteDC();
}

UINT CGameThread::ThreadFunction( LPVOID pParam )
{
	CGameThread *pGameThreadClass = (CGameThread *)pParam;

	DWORD Result = 0;

	HANDLE handles[ 2 ];

	handles[ 0 ] = ::CreateEvent( NULL, FALSE, FALSE, START_TICK );
	handles[ 1 ] = ::CreateEvent( NULL, FALSE, TRUE , CLOSE_ALL  );

	BOOL bEndGame;
	UINT uiDCNum;
	HWND hMessageDest;

	while( 1 )
	{
		Result = WaitForMultipleObjects( 2, handles, FALSE, INFINITE );

		if( Result == WAIT_OBJECT_0 )
		{
			//TRACE("Signal received\n");
			pGameThreadClass->m_bInside = true;

			pGameThreadClass->SimulateGame();
			//PostMessage( pGameThreadClass->m_hMessageDest, WM_END_OF_TICK, pGameThreadClass->m_uiDCNum, pGameThreadClass->m_pGameObjsArray->m_bEndGame );

			hMessageDest = pGameThreadClass->m_hMessageDest;
			uiDCNum = pGameThreadClass->m_uiDCNum;
			bEndGame = pGameThreadClass->m_pGameObjsArray->m_bEndGame;

			pGameThreadClass->m_bInside = false;

			//TRACE("Before Post\n");
			CMsgWrapper::Post( hMessageDest, WM_END_OF_TICK, uiDCNum, bEndGame );
			//TRACE("After Post\n");

			//TRACE( "%d\n", pGameThreadClass->m_pGameObjsArray->m_aObjects.GetCount() );
		}
		else if( Result == WAIT_FAILED || Result == WAIT_OBJECT_0 + 1 ) break;
	}

	TRACE( "Thread exited\n" );

	pGameThreadClass->m_bThreadExists = false;
	return 0;
}

void CGameThread::SimulateGame()
{
#if LOG_TICK_LEN
	LARGE_INTEGER t1, t2, TimeElapsed, TimerFreq;

	VERIFY

	QueryPerformanceFrequency( &TimerFreq );

	QueryPerformanceCounter( &t1 );
#endif

	m_uiGameLen += m_uiTimeDiff;

	if( m_bEnemyGen && ( m_pGameObjsArray->m_iState == STATE_NORMAL ) && !m_pGameObjsArray->m_bEndGame ) GenerateEnemies();

	if( m_uiTimeDiff > 0 ) GameTick();

	DrawGame();

#if LOG_TICK_LEN
	QueryPerformanceCounter( &t2 );

	FILE *fo = fopen( "dat/log.txt", "a" );

	if( fo )
	{
		TimeElapsed.QuadPart = t2.QuadPart - t1.QuadPart;
		TimeElapsed.QuadPart *= 1000000;
		TimeElapsed.QuadPart /= TimerFreq.QuadPart;

		fprintf( fo, "%.3f ms\n", TimeElapsed.QuadPart * 0.001 );

		fclose( fo );
	}

#endif
}

void CGameThread::GameTick()
{
	if( m_pGameObjsArray->m_oShip.m_iBullets < 3 && m_pGameObjsArray->m_iScore >= 10000 * m_pGameObjsArray->m_oShip.m_iBullets * m_pGameObjsArray->m_oShip.m_iBullets )
		m_pGameObjsArray->m_oShip.SetBulletNum( m_pGameObjsArray->m_oShip.m_iBullets + 1 );

	m_pGameObjsArray->Tick( m_uiTimeDiff );
}

void CGameThread::DrawGame()
{
	CBitmap *prevBit = m_oGameDC.SelectObject( m_pGameBitmap );

	if( m_dLastConv != m_dConv )
	{
		m_oGameFont.DeleteObject();
		m_oGameFont.CreatePointFont( round( 0.05  * 30 / 4 * GAME_HEIGHT * m_dConv ), "tahoma" );

		m_dLastConv = m_dConv;
	}

	m_oGameDC.SelectObject( &m_oGameFont );
	m_oGameDC.SelectObject( &m_oGamePen );

	m_pGameObjsArray->Draw( &m_oGameDC, m_bShowScore, m_dConv );

	m_oGameDC.SelectObject( prevBit );
}

void CGameThread::GenerateEnemies()
{
	UINT Rand = Random( 0, ENEMY_GEN_MAX_RAND );

	double Level = GetDifficulty( m_uiTimeDiff * 0.001 );

	double MinForAsteroid = Level				* m_uiTimeDiff / 16.6666666;
	double MinForUfo = Level * Level * .000007	* m_uiTimeDiff / 16.6666666;

	if( Rand <= MinForAsteroid || m_pGameObjsArray->m_iEnemies == 0 )
	{
		CCoord NewObjPos;
		CCoord NewObjSpeed = CCoord( Random( -3000, 3000 ) * 0.00003, Random( -3000, 3000 ) * 0.00003 );

		if( Random( 0, 1 ) )  NewObjPos.x = Random( 0, 10000 ) % GAME_WIDTH;
		else				  NewObjPos.y = Random( 0, 10000 ) % GAME_HEIGHT;

		CGameObj *pNewObj;
		if( Rand <= MinForUfo )
		{
			pNewObj = Random( -1, 2 ) >= 0 ?	new CUfo( NewObjPos, CCoord( 0, 0 ), 1 )   : 
												new CUfo( NewObjPos, NewObjSpeed, 0 );
		}
		else pNewObj = new CAsteroid( NewObjPos, NewObjSpeed );
		
		m_pGameObjsArray->AddObj( pNewObj );
	}
}

double CGameThread::GetDifficulty( double Seconds )
{
	return 7000 + 0.1 * Seconds;
}