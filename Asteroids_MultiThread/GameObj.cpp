// GameObj.cpp: implementation of the CGameObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Asteroids.h"
#include "GameObj.h"
#include "GameObjsArray.h"

#include "AsteroidsView.h"
#include "TGraphics.h"

#define BULLET_SPEED 0.3
#define DRAG 0.005

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoord::CCoord()
{
	x = 0;
	y = 0;
}

CCoord::CCoord( const CCoord &coord )
{
	x = coord.x;
	y = coord.y;
}

CCoord::CCoord( const CPoint &point )
{
	x = (double) point.x;
	y = (double) point.y;
}

CCoord::CCoord( const double x, const double y )
{
	this->x = x;
	this->y = y;
}

BOOL CCoord::operator ==( const CCoord &coord )
{
	return x == coord.x && y == coord.y;
}

CCoord &CCoord::operator +=( const CCoord &coord )
{
	x += coord.x;
	y += coord.y;
	return *this;
}

CCoord &CCoord::operator -=( const CCoord &coord )
{
	x -= coord.x;
	y -= coord.y;
	return *this;
}

CCoord &CCoord::operator *=( double d )
{
	x *= d;
	y *= d;
	return *this;
}

CCoord::operator CPoint()
{
	return CPoint( round( x ), round( y ) );
}

void CCoord::Rotate( const CCoord &Center, double deg )
{
	CCoord Dif = *this - Center;
	double Sin = sin( deg );
	double Cos = cos( deg );

	CCoord coord( Dif.x * Cos - Dif.y * Sin, Dif.y * Cos + Dif.x * Sin );

	x = Center.x + coord.x;
	y = Center.y + coord.y;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameObj::Init( const CCoord &StartPos, const CCoord &StartVector )
{
	m_oCenter = StartPos;
	m_oMovementVector = StartVector;

	m_dHitboxRadius = 10 * GAME_OBJ_SIZE_MUL;

	m_pPrevObj = NULL;
	m_pNextObj = NULL;

	m_iTeam = 0;

	m_bForDestruction = false;
	m_iScoreForDestruction = 0;

	m_iShield = 0;
	m_bShieldLoss = true;

	m_iInvisible = 0;
	m_bInvisibleLoss = true;

	m_iGod = 0;
	m_bGodLoss = true;

	m_iKind	= KIND_GENERIC;
}

CGameObj::CGameObj()
{
	Init( CCoord( 0, 0 ), CCoord( 0, 0 ) );

	m_iScoreForDestruction = 0;
}

CGameObj::CGameObj( const CCoord &StartPos, const CCoord &StartVector )
{
	Init( StartPos, StartVector );

	m_iScoreForDestruction = 0; 
}

CGameObj::~CGameObj()
{
	GiveAwayPrevNextObj();
}

void CGameObj::DrawElement( CDC *pDC, CCoord &Center, double Conv )
{
	double Radius = m_dHitboxRadius * Conv;

	DrawEllipse( pDC, round( Center.x - Radius ), round( Center.y - Radius ), round( Center.x + Radius ), round( Center.y + Radius ), GAME_COLOR );
}

void CGameObj::Draw( CDC *pDC, double Conv )
{
	if( ! m_bForDestruction )
	{
		CCoord coord = m_oCenter;

		BOOL bOutX = true;
		BOOL bOutY = true;

		if( m_oCenter.x + m_dHitboxRadius >= GAME_WIDTH )	coord.x -= GAME_WIDTH;
		else if( m_oCenter.x - m_dHitboxRadius < 0 )		coord.x += GAME_WIDTH;
		else bOutX = false;

		if( m_oCenter.y + m_dHitboxRadius >= GAME_HEIGHT )	coord.y -= GAME_HEIGHT;
		else if( m_oCenter.y - m_dHitboxRadius < 0 )		coord.y += GAME_HEIGHT;
		else bOutY = false;

		DrawElement( pDC, m_oCenter * Conv, Conv );

		if( bOutX ) DrawElement( pDC, CCoord( coord.x, m_oCenter.y ) * Conv, Conv );

		if( bOutY ) DrawElement( pDC, CCoord( m_oCenter.x, coord.y ) * Conv, Conv );

		if( bOutX && bOutY ) DrawElement( pDC, coord * Conv, Conv );
	}
}

void CGameObj::Normalize()
{
	while( m_oCenter.x >= GAME_WIDTH )  m_oCenter.x -= GAME_WIDTH;
	while( m_oCenter.x < 0 )			m_oCenter.x += GAME_WIDTH;

	while( m_oCenter.y >= GAME_HEIGHT ) m_oCenter.y -= GAME_HEIGHT;
	while( m_oCenter.y < 0 )			m_oCenter.y += GAME_HEIGHT;

	if( m_iShield < 0 ) m_iShield = 0;
	if( m_iGod < 0 ) m_iGod = 0;
	if( m_iInvisible < 0 ) m_iInvisible = 0;
}

void CGameObj::Tick( int TimeDiff )
{
	if( ! m_bForDestruction && TimeDiff > 0 )
	{
		m_oCenter += m_oMovementVector * TimeDiff;

		if( m_iShield > 0 && m_bShieldLoss ) m_iShield -= TimeDiff;

		Normalize();
	}
}

void CGameObj::GiveAwayPrevNextObj()
{
	if( m_pPrevObj )	m_pPrevObj->m_pNextObj = m_pNextObj;
	if( m_pNextObj )	m_pNextObj->m_pPrevObj = m_pPrevObj;

	m_pPrevObj = NULL;
	m_pNextObj = NULL;
}

void CGameObj::CreateGameObj( CGameObj *pNewObj )
{
	m_pList->AddObj( pNewObj );
}

BOOL CGameObj::CheckColisions()
{
	if( ! m_bForDestruction && m_iInvisible <= 0 && m_iKind != KIND_CORPSE )
	{
		CGameObj *CurrentObj = m_pNextObj;
		bool bCollision = false;

		while( CurrentObj )
		{
			if( ! CurrentObj->m_bForDestruction && CurrentObj->m_iTeam != m_iTeam && CurrentObj->m_iInvisible <= 0 && CurrentObj->m_iKind != KIND_CORPSE )
			{
				double dX = m_oCenter.x - CurrentObj->m_oCenter.x;
				double dY = m_oCenter.y - CurrentObj->m_oCenter.y;

				if( dX > GAME_WIDTH  * 0.5 )  dX -= GAME_WIDTH;
				else if( dX < - GAME_WIDTH  * 0.5 )  dX += GAME_WIDTH;

				if( dY > GAME_HEIGHT * 0.5 ) dY -= GAME_HEIGHT;
				else if( dY < - GAME_HEIGHT * 0.5 ) dY += GAME_HEIGHT;

				double MinLen = m_dHitboxRadius + CurrentObj->m_dHitboxRadius;

				if( bCollision = ( dX * dX + dY * dY <= MinLen * MinLen ) ) break;
			}

			CurrentObj = CurrentObj->m_pNextObj;
		}

		if( bCollision )
		{
			if( CurrentObj->m_iGod <= 0 )
			{
				if( CurrentObj->m_iShield )
				{
					CurrentObj->m_iShield = 0;
				}
				else
				{
					CurrentObj->m_bForDestruction = true;
				}
			}

			if( m_iGod <= 0 )
			{
				if( m_iShield )
				{
					m_iShield = 0;
				}
				else
				{
					m_bForDestruction = true;
				}
			}
		}
	}

	return m_bForDestruction;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

CShip::CShip() : CGameObj( CCoord( GAME_WIDTH * 0.5, GAME_HEIGHT * 0.5 ), CCoord( 0, 0 ) )
{
	m_dDirection = 0;
	m_dHitboxRadius = 10 * GAME_OBJ_SIZE_MUL;

	for( int i = 0; i < SHIP_KEY_BIND_QTY; i++ )
		m_aControls[ i ] = false;

	m_iReload = 0;

	m_iTeam = 1;

	m_iBullets = 1;

	m_iShield = 1;
	m_bShieldLoss = false;

	m_iTimeToShieldReload = 0;

	m_iKind	= KIND_SHIP;

	m_sndLaser.CreateFromFile( "snd\\lasersmall.wav" );

	m_sndBigLaser.CreateFromFile( "snd\\laserbig.wav" );
	m_sndBigLaser.SetVolume( -12 );

	m_sndHyperspace.CreateFromFile( "snd\\hyperspace.wav" );

	m_iHypSpaceCooldown = 0;

	m_iLifes = 3;
	m_bForDestruction = false;

	//m_bGodLoss = false;
}

void CShip::Restart()
{
	Init( CCoord( GAME_WIDTH * 0.5, GAME_HEIGHT * 0.5 ), CCoord( 0, 0 ) );

	m_dDirection = 0;

	m_iReload = 0;

	m_iTeam = 1;

	m_iBullets = 1;

	m_iShield = 1;
	m_bShieldLoss = false;

	m_iTimeToShieldReload = 0;
	m_iHypSpaceCooldown = 0;

	m_iLifes = 3;
}

void CShip::DrawElement( CDC *pDC, CCoord &Center, double Conv )
{
	if( ! m_bForDestruction )
	{
		if( m_iShield )		DrawEllipse( pDC, round( Center.x - ( m_dHitboxRadius + 3 ) * Conv ), round( Center.y - ( m_dHitboxRadius + 3 ) * Conv ), round( Center.x + ( m_dHitboxRadius + 3 ) * Conv ), round( Center.y + ( m_dHitboxRadius + 3 ) * Conv ), GAME_COLOR );

		double Radius = m_dHitboxRadius * Conv;
		double dHeadToArmAngle = PI * 15 / 18;

		CCoord head( cos( m_dDirection ) * Radius, sin( m_dDirection ) * Radius );

		CCoord RWing( cos( m_dDirection - dHeadToArmAngle ) * Radius, 
					  sin( m_dDirection - dHeadToArmAngle ) * Radius );

		CCoord LWing( cos( m_dDirection + dHeadToArmAngle ) * Radius,
					  sin( m_dDirection + dHeadToArmAngle ) * Radius );

		CPoint Points[3] = { Center + RWing, Center + head, Center + LWing };

		DrawPolygon( pDC, (CPoint *)&Points, 3, GAME_COLOR );

		if( m_aControls[ ACC ] )
		{
			CCoord FireTip( cos( m_dDirection + PI ) * Radius * 1.7, sin( m_dDirection + PI ) * Radius * 1.7 );

			Points[ 0 ] = ( Center + GetPointAtPercent( LWing, RWing, 0.3 ) );
			Points[ 1 ] = ( Center + FireTip );
			Points[ 2 ] = ( Center + GetPointAtPercent( LWing, RWing, 0.7 ) );

			DrawPolyline( pDC, (CPoint *)&Points, 3, GAME_COLOR );
		}
	}
}

#define HYP_SPC_DIVISION .30
#define NUM_OF_RINGS 50
#define RING_SPACING .02

void CShip::DrawHypSpc( CDC *pDC, double AnimationPercent, double Conv )
{
	//TRACE( "%f\n", AnimationPercent );

	double MaxRadius = 2.5 * m_dHitboxRadius;

	CCoord TL( m_oCenter.x - MaxRadius, m_oCenter.y - MaxRadius );
	CCoord BR( m_oCenter.x + MaxRadius, m_oCenter.y + MaxRadius );

	TL *= Conv;
	BR *= Conv;

	int i = 0;

	double A = AnimationPercent * ( 1 + ( NUM_OF_RINGS - 1 ) * RING_SPACING );

	for(; A >= 0 && i < NUM_OF_RINGS; A -= RING_SPACING, i++ )
	{
		if( A <= 1 )
		{
			if( A < HYP_SPC_DIVISION )
			{
				double GrowthPercent = A / HYP_SPC_DIVISION;

				CPoint TL2 = GetPointAtPercent( m_oCenter * Conv, TL, GrowthPercent );
				CPoint BR2 = GetPointAtPercent( m_oCenter * Conv, BR, GrowthPercent );

				DrawEllipse( pDC, TL2.x, TL2.y, BR2.x, BR2.y, GAME_COLOR );
			}
			else
			{
				double SlidePercent = ( A - HYP_SPC_DIVISION ) / ( 1 - HYP_SPC_DIVISION );
			
				CPoint TL2 = GetPointAtPercent( TL, m_oHypSpcNewPos * Conv, SlidePercent );
				CPoint BR2 = GetPointAtPercent( BR, m_oHypSpcNewPos * Conv, SlidePercent );

				DrawEllipse( pDC, TL2.x, TL2.y, BR2.x, BR2.y, GAME_COLOR );
			}
		}
	}
}

void CShip::Normalize()
{
	while( m_oCenter.x >= GAME_WIDTH )  m_oCenter.x -= GAME_WIDTH;
	while( m_oCenter.x < 0 )			m_oCenter.x += GAME_WIDTH;

	while( m_oCenter.y >= GAME_HEIGHT ) m_oCenter.y -= GAME_HEIGHT;
	while( m_oCenter.y < 0 )			m_oCenter.y += GAME_HEIGHT;

	while( m_dDirection >= PI * 2 )		m_dDirection -= 2 * PI;
	while( m_dDirection < 0 )			m_dDirection += 2 * PI;

	if( m_iShield < 0 ) m_iShield = 0;
	if( m_iGod < 0 ) m_iGod = 0;
	if( m_iInvisible < 0 ) m_iInvisible = 0;

	if( m_iHypSpaceCooldown < 0 ) m_iHypSpaceCooldown = 0;
}

void CShip::Tick( int TimeDiff )
{
	if( ! m_bForDestruction && TimeDiff > 0 )
	{
		m_dDirection += TimeDiff * 0.003 * ( (int)m_aControls[ RIGHT - SHIP_KEY_BIND_START ] - (int)m_aControls[ LEFT - SHIP_KEY_BIND_START ] );

		if( m_aControls[ ACC ] ) Accelerate( TimeDiff );
		Drag( TimeDiff );

		if( m_iReload > 0 )	m_iReload -= TimeDiff;

		if( m_aControls[ SHOOT ] && m_iReload <= 0 ) Shoot( max( 0, -m_iReload ) );
	
		m_oCenter += m_oMovementVector * TimeDiff;

		if( m_iShield > 0 && m_bShieldLoss )		m_iShield		-= TimeDiff;
		if( m_iGod > 0 && m_bGodLoss )				m_iGod			-= TimeDiff;
		if( m_iInvisible > 0 && m_bInvisibleLoss )	m_iInvisible	-= TimeDiff;

		if( m_iShield <= 0 )
		{
			if( m_iTimeToShieldReload <= 0 )
			{
				m_iGod = 500;

				m_iTimeToShieldReload = 20000;
			}
			else m_iTimeToShieldReload -= TimeDiff;

			if( m_iTimeToShieldReload <= 0 )
			{
				m_iTimeToShieldReload = 0;
				m_iShield = 1;
			}
		}

		if( m_iHypSpaceCooldown == 0 )
		{
			if( m_aControls[ HYP_SPACE ] )
			{
				m_sndHyperspace.Play();

				m_pList->SetState( STATE_HYP_SPC );
				m_oHypSpcNewPos.x = Random( 0, GAME_WIDTH  * 100 ) * 0.01;
				m_oHypSpcNewPos.y = Random( 0, GAME_HEIGHT * 100 ) * 0.01;

				m_iHypSpaceCooldown = 10000;
			}
		}
		else
			m_iHypSpaceCooldown -= TimeDiff;

		Normalize();
	}
}

void CShip::Accelerate( int TimeDiff )
{
	m_oMovementVector.x += cos( m_dDirection ) * 0.0003 * TimeDiff;
	m_oMovementVector.y += sin( m_dDirection ) * 0.0003 * TimeDiff;
}

void CShip::Drag( int TimeDiff )
{
	m_oMovementVector.x -= fabs( m_oMovementVector.x ) * m_oMovementVector.x * DRAG * TimeDiff;
	m_oMovementVector.y -= fabs( m_oMovementVector.y ) * m_oMovementVector.y * DRAG * TimeDiff;
}

void CShip::GenerateBullet( double direction, int BulletLife, CCoord &Recoil )
{
	CCoord BulletSpeed( cos( direction ) * BULLET_SPEED, sin( direction ) * BULLET_SPEED );

	Recoil -= BulletSpeed * 0.003;

	BulletSpeed += m_oMovementVector;

	CBullet *pNewBullet = new CBullet( m_oCenter, BulletSpeed, m_iTeam );

	CreateGameObj( pNewBullet );

	pNewBullet->Tick( BulletLife );
	pNewBullet->m_bDoNotRunNext = true;
}

#define MAX_SPREAD PI / 4
#define NORM_SPREAD PI / 32

void CShip::Shoot( int BulletLife )
{
	m_iReload = 128;

	double spread = NORM_SPREAD * ( m_iBullets - 1 ) <= MAX_SPREAD ? 
						NORM_SPREAD : MAX_SPREAD / ( m_iBullets - 1 );

	double firstBullet = m_dDirection + ( ( m_iBullets - 1 ) * 0.5 * spread );
	//double firstBullet = m_dDirection + ( ( m_iBullets - 1 ) * 0.5 * spread ) + PI;
	
	CCoord Recoil;

	for( int i = 0; i < m_iBullets; i++ )
	{
		GenerateBullet( firstBullet - i * ( spread ), BulletLife, Recoil );
	}

	m_oMovementVector += Recoil;

	if( m_iBullets > 1 ) m_sndBigLaser.Play();
	else	m_sndLaser.Play();

}

////////////////////////////////////////////////////////////////////////////////////////////////////

CBullet::CBullet( CCoord &StartPos, CCoord &StartVector, BOOL Team ) : CGameObj( StartPos, StartVector )
{
	m_dHitboxRadius = 2 * GAME_OBJ_SIZE_MUL;

	m_iLifespan = 1000;

	m_iTeam = Team;

	m_iKind = KIND_BULLET;

	m_bDoNotRunNext = false;
}

void CBullet::Tick( int TimeDiff )
{
	if( ! m_bForDestruction && TimeDiff > 0 )
	{
		if( m_bDoNotRunNext )
		{
			m_bDoNotRunNext = false;
			return;
		}

		m_oCenter.x += m_oMovementVector.x * TimeDiff;
		m_oCenter.y += m_oMovementVector.y * TimeDiff;

		Normalize();

		m_iLifespan -= TimeDiff;

		if( m_iShield > 0 && m_bShieldLoss ) m_iShield -= TimeDiff;
		
		if( m_iLifespan <= 0 ) m_bForDestruction = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

CAsteroid::CAsteroid( const CCoord &StartPos, const CCoord &StartVector, int size ) : CGameObj( StartPos, StartVector )
{
	m_iSize = size;
	m_dHitboxRadius = m_iSize * 10 * GAME_OBJ_SIZE_MUL;

	m_iKind = KIND_ASTEROID;

	m_iScoreForDestruction = size == 3 ? 20 : size == 2 ? 50 : 100; 

	m_iType = Random( 0, 2 );

	m_dRotation = 0;
	m_dRotationIncr = Random( -100, 100 ) * 0.00001;
}

CAsteroid::~CAsteroid()
{
	CreateGameObj( new CCorpse( m_oCenter, m_oMovementVector ) );

	if( m_iSize > 1 )
	{
		CreateGameObj( new CAsteroid( m_oCenter, CCoord( m_oMovementVector.x + Random( -10, 10 ) * 0.003, m_oMovementVector.y + Random( -10, 10 ) * 0.003 ), m_iSize - 1 ) );
		CreateGameObj( new CAsteroid( m_oCenter, CCoord( m_oMovementVector.x + Random( -10, 10 ) * 0.003, m_oMovementVector.y + Random( -10, 10 ) * 0.003 ), m_iSize - 1 ) );
	}
}

//AsteroidsLook [ TYPE ][ COORD ] (For 30 px radius asteroid)
const CCoord AsteroidsLook [ 3 ][ 11 ] = {  { CCoord( 7, -29 ), CCoord( 28, -8  ), CCoord( 15, -5  ), CCoord( 26, 13 ), CCoord( 15, 24 ), CCoord( 12, 12 ), CCoord( -18, 23 ), CCoord( -25, 16 ), CCoord( -29, -6 ), CCoord( -13, -9  ), CCoord( -19, -23 ) },
											{ CCoord( 2, -12 ), CCoord( 12, -27 ), CCoord( 27, -11 ), CCoord( 16, -1 ), CCoord( 28, 8  ), CCoord( 9 , 27 ), CCoord( -8 , 9  ), CCoord( -18, 23 ), CCoord( -29, 7  ), CCoord( -22, -20 ), CCoord( -9 , -29 ) },
											{ CCoord( 5, -29 ), CCoord( 9 , -15 ), CCoord( 24, -16 ), CCoord( 28, 7  ), CCoord( 11, -1 ), CCoord( 17, 23 ), CCoord( 5  , 28 ), CCoord( -6 , 19 ), CCoord( -16, 24 ), CCoord( -28, -10 ), CCoord( -17, -25 ) } };

void CAsteroid::DrawElement( CDC *pDC, CCoord &Center, double Conv )
{
	Conv *= m_dHitboxRadius / 30;

	CCoord Asteroid[ 11 ];
	CPoint Ast[ 11 ];

	for( int i = 0; i < 11; i++ )
	{
		Asteroid[ i ] = Center + AsteroidsLook[ m_iType ][ i ] * Conv;
		Asteroid[ i ].Rotate( Center, m_dRotation );
		Ast[ i ] = Asteroid[ i ];
	}

	//pDC->Polygon( (POINT *)&Ast, 11 );

	DrawPolygon( pDC, (POINT *)&Ast, 11, GAME_COLOR );
}

void CAsteroid::Tick( int TimeDiff )
{
	if( ! m_bForDestruction )
	{
		m_dRotation += m_dRotationIncr * TimeDiff;

		m_oCenter += m_oMovementVector * TimeDiff;

		Normalize();
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDirectSound CUfo::m_sndUFO;

CUfo::CUfo( const CCoord &StartPos, const CCoord &StartVector, int type ) : CGameObj( StartPos, StartVector )
{
	// type: 0 - NoMind, 1 - following
	m_dHitboxRadius = ( type == 0 ? 25 : type == 1 ? 20 : 10 ) * GAME_OBJ_SIZE_MUL;
	m_iTeam = type == 0 ? 2 : 0;

	m_iScoreForDestruction = type == 0 ? 200 : type == 1 ? 1000 : 100;

	m_iType = type;

	m_dPlayerDirection = 0;

	m_iReload = 1024;

	if( ! m_sndUFO.IsValid() ) m_sndUFO.CreateFromFile( "snd\\ufo.wav" );

	m_sndUFO.Play();

	m_iKind = KIND_UFO;
}

CUfo::~CUfo()
{
	CreateGameObj( new CCorpse( m_oCenter, m_oMovementVector ) );

	GiveAwayPrevNextObj();
}

void CUfo::DrawElement( CDC *pDC, CCoord &Center, double Conv )
{
	CCoord coord;

	Conv *= m_dHitboxRadius / 20;

	switch( m_iType )
	{
	case 1:
		/*
		pDC->MoveTo( m_oCenter );
		coord.x = cos( m_dPlayerDirection ) * ( m_dHitboxRadius * 2 );
		coord.y = sin( m_dPlayerDirection ) * ( m_dHitboxRadius * 2 );
		pDC->LineTo( m_oCenter + coord );
		*/
		//pDC->Ellipse( 
	case 0:
		double deg = m_oMovementVector.x * PI * 2;

		CCoord Cir1Center( Center.x, Center.y + Conv * 24  );
		CCoord Cir2Center( Center.x, Center.y + Conv * -17 );

		CCoord StartBody( Center.x + Conv *  22, Center.y + Conv * 4 );
		CCoord EndBody  ( Center.x + Conv * -22, Center.y + Conv * 4 );

		CCoord ElCenter  ( Center.x, Center.y + Conv * -8 );

		CCoord StartHead ( Center.x + Conv *  8, Center.y + Conv * -5 );
		CCoord EndHead   ( Center.x + Conv * -8, Center.y + Conv * -5 );

		Cir1Center	.Rotate( Center, deg );
		Cir2Center	.Rotate( Center, deg );

		StartBody	.Rotate( Center, deg );
		EndBody		.Rotate( Center, deg );

		ElCenter	.Rotate( Center, deg );
		StartHead	.Rotate( Center, deg );
		EndHead		.Rotate( Center, deg );

		int radius = 30;
		CRect Cir1( round( Cir1Center.x - Conv * radius ), round( Cir1Center.y - Conv * radius ), round( Cir1Center.x + Conv * radius ), round( Cir1Center.y + Conv * radius ) );
		CRect Cir2( round( Cir2Center.x - Conv * radius ), round( Cir2Center.y - Conv * radius ), round( Cir2Center.x + Conv * radius ), round( Cir2Center.y + Conv * radius ) );

		radius = 8;
		CRect El( round( ElCenter.x - Conv * radius ), round( ElCenter.y - Conv * radius ), round( ElCenter.x + Conv * radius ), round( ElCenter.y + Conv * radius ) );

		DrawArc( pDC, Cir1.left, Cir1.top, Cir1.right, Cir1.bottom, round(StartBody.x), round(StartBody.y), round(EndBody.x), round(EndBody.y), GAME_COLOR );
		DrawArc( pDC, Cir2.left, Cir2.top, Cir2.right, Cir2.bottom, round(EndBody.x), round(EndBody.y), round(StartBody.x), round(StartBody.y), GAME_COLOR );
		DrawArc( pDC, El.left, El.top, El.right, El.bottom, round(StartHead.x), round(StartHead.y), round(EndHead.x), round(EndHead.y), GAME_COLOR );
		//pDC->Arc( Cir1, StartBody, EndBody );
		//pDC->Arc( Cir2, EndBody, StartBody );
		//pDC->Arc( El  , StartHead , EndHead  );

		/*
		bool bState = false;
		bool bFirst = true;

		for( double d = 0; d < 2 * PI; d += PI / 12 )
		{
			coord.x = cos( d ) * ( m_dHitboxRadius * ( bState * 0.5 + 0.5 ) );
			coord.y = sin( d ) * ( m_dHitboxRadius * ( bState * 0.5 + 0.5 ) );
			
			if( bFirst ) pDC->MoveTo( m_oCenter + coord );
			else pDC->LineTo( m_oCenter + coord );


			bState = !bState;
			bFirst = false;
		}
		*/
		break;
	}
}

void CUfo::Accelerate( int TimeDiff )
{
	m_oMovementVector.x += cos( m_dPlayerDirection ) * 0.00007 * TimeDiff;
	m_oMovementVector.y += sin( m_dPlayerDirection ) * 0.00007 * TimeDiff;
}

void CUfo::Drag( int TimeDiff )
{
	m_oMovementVector.x -= fabs( m_oMovementVector.x ) * m_oMovementVector.x * DRAG * TimeDiff;
	m_oMovementVector.y -= fabs( m_oMovementVector.y ) * m_oMovementVector.y * DRAG * TimeDiff;
}

void CUfo::Shoot( int BulletLife )
{
	m_iReload = 1024;

	CCoord BulletSpeed( cos( m_dPlayerDirection ) * BULLET_SPEED, sin( m_dPlayerDirection ) * BULLET_SPEED );

	CCoord MovVecCpy = m_oMovementVector;
	m_oMovementVector -= BulletSpeed * 0.003;
	BulletSpeed += MovVecCpy;

	CBullet *pNewBullet = new CBullet( m_oCenter, BulletSpeed, m_iTeam );

	pNewBullet->m_iLifespan *= 0.75;
	CreateGameObj( pNewBullet );

	pNewBullet->Tick( BulletLife );
	pNewBullet->m_bDoNotRunNext = true;
}

void CUfo::TrackPlayer()
{
	double dX = m_pList->m_oShip.m_oCenter.x - m_oCenter.x;
	double dY = m_pList->m_oShip.m_oCenter.y - m_oCenter.y;

	if( dX > GAME_WIDTH * 0.5 )  dX -= GAME_WIDTH;
	else if( dX < - GAME_WIDTH  * 0.5 )  dX += GAME_WIDTH;

	if( dY > GAME_HEIGHT * 0.5 ) dY -= GAME_HEIGHT;
	else if( dY < - GAME_HEIGHT * 0.5 ) dY += GAME_HEIGHT;

	m_dPlayerDirection = - atan2( dX, dY ) + PI / 2;
}

void CUfo::RandShoot( int BulletLife )
{
	double direction = Random( 0, 62832 ) * 0.0001;

	m_iReload = 256;

	CCoord BulletSpeed( cos( direction ) * BULLET_SPEED, sin( direction ) * BULLET_SPEED );

	CCoord MovVecCpy = m_oMovementVector;
	m_oMovementVector -= BulletSpeed * 0.003;
	BulletSpeed += MovVecCpy;

	CBullet *pNewBullet = new CBullet( m_oCenter, BulletSpeed, m_iTeam );

	pNewBullet->m_iLifespan *= 0.75;
	CreateGameObj( pNewBullet );

	pNewBullet->Tick( BulletLife );
	pNewBullet->m_bDoNotRunNext = true;
}

void CUfo::Tick( int TimeDiff )
{
	int i = 0;
	switch( m_iType )
	{
	case 0:
		if( m_iReload > 0 )	m_iReload -= TimeDiff;
		
		if( m_iReload <= 0 ) RandShoot( max( 0, -m_iReload ) );

		m_oMovementVector += CCoord( ( Random( -300, 300 ) ) * 0.000005, ( Random( -300, 300 ) ) * 0.000005 );

		m_oCenter += m_oMovementVector * TimeDiff;
		break;
	case 1:
		if( m_pList && !m_pList->m_oShip.m_bForDestruction )
		{
			TrackPlayer();
			Accelerate( TimeDiff );

			if( m_iReload > 0 )	m_iReload -= TimeDiff;

			if( m_iReload <= 0 ) Shoot( max( 0, -m_iReload ) );
		}

		Drag( TimeDiff );

		m_oCenter += m_oMovementVector * TimeDiff;

		break;
	}

	if( m_iShield > 0 && m_bShieldLoss ) m_iShield -= TimeDiff;

	Normalize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_CORPSE_LIFESPAN 1000
CCorpse::CCorpse( const CCoord &StartPos, const CCoord &StartVector ) : CGameObj( StartPos, StartVector )
{
	m_iKind = KIND_CORPSE;

	m_iLifespan = MAX_CORPSE_LIFESPAN;

	for( int i = 0; i < 10; i++ )
	{
		m_aParticleDest[ i ].x = Random( -10000, 10000 ) * 0.01;
		m_aParticleDest[ i ].y = Random( -10000, 10000 ) * 0.01;
	}

	m_iScoreForDestruction = 0;

	m_dHitboxRadius = 1000;
}

void CCorpse::Tick(int TimeDiff)
{
	if( TimeDiff > 0 && ! m_bForDestruction )
	{
		m_iLifespan -= TimeDiff;

		if( m_iLifespan <= 0 ) m_bForDestruction = true;

		m_oCenter += m_oMovementVector * TimeDiff;
	}

	Normalize();
}

void CCorpse::DrawElement( CDC *pDC, CCoord &Center, double Conv )
{
	double Percent = 1 - m_iLifespan / (double)(MAX_CORPSE_LIFESPAN);

	for( int i = 0; i < 10; i++ )
		pDC->SetPixelV( GetPointAtPercent( m_oCenter, m_oCenter + m_aParticleDest[ i ], Percent ) * Conv, RGB( 255, 255, 255 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_SHIP_CORPSE_LIFESPAN 3000
CShipCorpse::CShipCorpse( const CCoord &StartPos, const CCoord &StartVector ) : CCorpse( StartPos, StartVector )
{
	for( int i = 0; i < 5; i++ )
	{
		m_aParticleDest[ i * 2 ].x = Random( -5000, 5000 ) * 0.01;
		m_aParticleDest[ i * 2 ].y = Random( -5000, 5000 ) * 0.01;

		m_aParticleDest[ i * 2 + 1 ].x = Random( -1000, 1000 ) * 0.01;
		m_aParticleDest[ i * 2 + 1 ].y = Random( -1000, 1000 ) * 0.01;

		m_aShowTimes[ i ] = Random( 0, MAX_SHIP_CORPSE_LIFESPAN - MAX_CORPSE_LIFESPAN );
	}

	m_iLifespan = MAX_SHIP_CORPSE_LIFESPAN;

	m_dHitboxRadius = 60;
}

void CShipCorpse::DrawElement( CDC *pDC, CCoord &Center, double Conv )
{
	double Percent = 1 - m_iLifespan / (double)(MAX_SHIP_CORPSE_LIFESPAN);

	for( int i = 0; i < 5; i++ )
	{
		if( m_iLifespan >= m_aShowTimes[ i ] )
		{
			CCoord StartPoint = GetPointAtPercent( m_oCenter, m_oCenter + m_aParticleDest[ i * 2 ], Percent ) * Conv;
			CCoord EndPoint = StartPoint + m_aParticleDest[ i * 2 + 1 ] * Conv;

			DrawLine( pDC, round( StartPoint.x ), round( StartPoint.y ), round( EndPoint.x ), round( EndPoint.y ), GAME_COLOR );
		}
	}
}