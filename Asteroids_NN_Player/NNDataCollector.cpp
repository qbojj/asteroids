
#include "stdafx.h"
#include "NNDataCollector.h"

#include "AsteroidsView.h"

CNNDataCollector::CNNDataCollector()
{
}

CNNDataCollector::~CNNDataCollector()
{
	Clear();
}

void CNNDataCollector::Clear()
{
	int i;

	for( i = 0; i < m_oData.GetSize(); i++ )
		free( m_oData[i].pData );

	m_oData.RemoveAll();
}

void CNNDataCollector::Save( int Score )
{
	if( m_oData.GetSize() == 0 ) return;

	char Name[ _MAX_PATH + 1 ];
	CTime Time = CTime::GetCurrentTime();
	sprintf( Name, ".\\dat\\Replays\\%d_%s.rep", Score, Time.Format( "%d%b%Y, %H %M'%S''" ) );

	FILE *fo = fopen( Name, "wb" );

	if( fo )
	{
		for( int i = 0; i < m_oData.GetSize(); i++ )
		{
			size_t Res1 = fwrite( &m_oData[ i ].Size , sizeof( size_t ) , 1, fo );
			size_t Res2 = fwrite(  m_oData[ i ].pData, m_oData[ i ].Size, 1, fo );

			ASSERT( Res1 && Res2 );
		}

		fclose( fo );
	}

	Clear();
}

void CNNDataCollector::Load( LPCSTR FileName )
{
	Clear();

	FILE *fo = fopen( FileName, "rb" );

	if( fo )
	{
		while( !feof( fo ) )
		{
			TickData Data;

			size_t Res1 = fread( &Data.Size, sizeof( size_t ), 1, fo );

			Data.pData = malloc( Data.Size );
			size_t Res2 = fread( Data.pData, Data.Size, 1, fo );

			//ASSERT( Res1 && Res2 );

			if( Res1 && Res2 ) m_oData.Add( Data );
			else free( Data.pData );
		}

		fclose( fo );
	}
}

void CNNDataCollector::LogMoment( CGameObjsArray *pObjsArray )
{
	ASSERT( pObjsArray );

	TickData Data;

	Data.Size = sizeof( CGameObjsArray );

	POSITION pos = pObjsArray->m_aObjects.GetHeadPosition();
	CGameObj *pObj;

	while( pos )
	{
		pObj = pObjsArray->m_aObjects.GetNext( pos );

		Data.Size += sizeof( size_t );
		Data.Size += pObj->m_iSizeOf;	
	}

	Data.pData = malloc( Data.Size );

	BYTE *pCurrent = (BYTE *) Data.pData;

	memcpy( pCurrent, pObjsArray, sizeof( CGameObjsArray ) );
	pCurrent += sizeof( CGameObjsArray );

	pos = pObjsArray->m_aObjects.GetHeadPosition();

	while( pos )
	{
		pObj = pObjsArray->m_aObjects.GetNext( pos );

		memcpy( pCurrent, &pObj->m_iSizeOf, sizeof( size_t ) );
		pCurrent += sizeof( size_t );

		memcpy( pCurrent, pObj, pObj->m_iSizeOf );
		pCurrent += pObj->m_iSizeOf;
	}

	ASSERT( pCurrent == (BYTE *)Data.pData + Data.Size );

	m_oData.Add( Data );
}

void CNNDataCollector::ImportMoment( int TickNum, CGameObjsArray *pObjsArray )
{
	ASSERT( pObjsArray );

	if( TickNum >= m_oData.GetSize() ) 
	{
		pObjsArray->m_bEndGame = true;
		return;
	}

	pObjsArray->DeleteAll();

	TickData &Data = m_oData[ TickNum ];

	BYTE *pCurrent = (BYTE *)Data.pData;

	memcpy( pObjsArray, pCurrent, sizeof( CGameObjsArray ) );
	pCurrent += sizeof( CGameObjsArray );

	memcpy( &pObjsArray->m_aObjects, &CGameObjsList(), sizeof( CGameObjsList ) );

	while( pCurrent < (BYTE *)Data.pData + Data.Size )
	{
		size_t Size;
		memcpy( &Size, pCurrent, sizeof( size_t ) );
		pCurrent += sizeof( size_t );

		CGameObj *pObj = (CGameObj *)malloc( Size );
		memcpy( pObj, pCurrent, Size );
		pCurrent += Size;

		pObj->m_pNextObj = pObj->m_pPrevObj = NULL;
		pObj->m_pList = NULL;

		pObjsArray->AddObj( pObj );
	}

	ASSERT( pCurrent == (BYTE *)Data.pData + Data.Size );
}

/*

struct DistObjShp 
{ 
	double Dist; 
	CGameObj *pObj;
};

void CNNDataCollector::GetAnnInput( CGameObjsArray *pObjsArray, float Input[ 128 ] )
{
	Input[ 0 ] = (float) pObjsArray->m_oShip.m_iShield;
	Input[ 1 ] = (float) pObjsArray->m_oShip.m_iHypSpaceCooldown;
	Input[ 2 ] = (float) pObjsArray->m_oShip.m_iReload;
	Input[ 3 ] = (float) pObjsArray->m_oShip.m_iBullets;

	Input[ 4 ] = (float) pObjsArray->m_oShip.m_oCenter.x;
	Input[ 5 ] = (float) pObjsArray->m_oShip.m_oCenter.y;

	Input[ 6 ] = (float) pObjsArray->m_oShip.m_oMovementVector.x;
	Input[ 7 ] = (float) pObjsArray->m_oShip.m_oMovementVector.y;

	//20 nearest GameObjs: 120
	CArray< DistObjShp, DistObjShp &> Objs;

	POSITION pos = pObjsArray->m_aObjects.GetHeadPosition();
	
	while( pos )
	{
		DistObjShp DOS;

		DOS.pObj = pObjsArray->m_aObjects.GetNext( pos );

		CCoord Dif = DOS.pObj->m_oCenter - pObjsArray->m_oShip.m_oCenter;

		if( Dif.x > GAME_WIDTH  * 0.5 )  Dif.x -= GAME_WIDTH;
		else if( Dif.x < - GAME_WIDTH  * 0.5 )  Dif.x += GAME_WIDTH;

		if( Dif.y > GAME_HEIGHT * 0.5 ) Dif.y -= GAME_HEIGHT;
		else if( Dif.y < - GAME_HEIGHT * 0.5 ) Dif.y += GAME_HEIGHT;

		DOS.Dist = Dif.x * Dif.x + Dif.y + Dif.y;

		Objs.Add( DOS );
	}

	qsort( Objs.GetData(), Objs.GetSize(), sizeof( DistObjShp ), cmp );

	for( int k = 0; k < 20; k++ )
	{
		if( k >= Objs.GetSize() )
		{
			int c = 8 + 6*k;

			while( c < 128 ) Input[ c++ ] = (float) 0;

			break;
		}

		CGameObj *pObj = Objs[ k ].pObj;

		int StartOfObjData = 8 + 6*k;

		// pos : 2
		double dX = pObjsArray->m_oShip.m_oCenter.x - pObj->m_oCenter.x;
		double dY = pObjsArray->m_oShip.m_oCenter.y - pObj->m_oCenter.y;

		if( dX > GAME_WIDTH * 0.5 )  dX -= GAME_WIDTH;
		else if( dX < - GAME_WIDTH  * 0.5 )  dX += GAME_WIDTH;

		if( dY > GAME_HEIGHT * 0.5 ) dY -= GAME_HEIGHT;
		else if( dY < - GAME_HEIGHT * 0.5 ) dY += GAME_HEIGHT;

		double AngleObjToShip = - atan2( dX, dY ) + PI / 2;

		double Angle1 = AngleObjToShip + PI - pObjsArray->m_oShip.m_dDirection;
		NormAngle( Angle1 );

		Input[ StartOfObjData + 0 ] = (float) Angle1;
		Input[ StartOfObjData + 1 ] = (float) sqrt( Objs[ k ].Dist );

		// speed : 2
		double SpeedAngleToShip = - atan2( pObj->m_oMovementVector.x, pObj->m_oMovementVector.y )
								- AngleObjToShip;
		NormAngle( SpeedAngleToShip );

		Input[ StartOfObjData + 2 ] = (float) SpeedAngleToShip;
		Input[ StartOfObjData + 3 ] = (float) sqrt( pObj->m_oMovementVector.x * pObj->m_oMovementVector.x +
												pObj->m_oMovementVector.y * pObj->m_oMovementVector.y );

		// kind : 1
		Input[ StartOfObjData + 4 ] = (float) pObj->m_iKind;

		// radius : 1
		Input[ StartOfObjData + 5 ] = (float) pObj->m_dHitboxRadius;
	}
}

int CNNDataCollector::cmp( const void *A, const void *B )
{
	double dif = ((DistObjShp *)A)->Dist - ((DistObjShp *)B)->Dist;

	return dif < 0 ? -1 : ( dif > 0 ? 1 : 0 );
}

void CNNDataCollector::NormAngle( double &Angle )
{
	while( Angle > PI )	 Angle -= 2*PI;
	while( Angle < -PI ) Angle += 2*PI;
}

*/

double cross( CCoord a, CCoord b )
{
	return a.x*b.y - b.x*a.y;
}

void getLine(CCoord a, CCoord b, double &_a, double &_b, double &_c)
{
    // (x - a.x) / (b.x - a.x) = (y - a.y) / (b.y - a.y) 
    _a = a.y - b.y;
    _b = b.y - a.x;
    _c = cross( a, b );
}

// distance from line threw l1 & l2 to point p
double dist(CCoord l1, CCoord l2, CCoord p)
{
    double a, b, c;
    getLine(l1, l2, a, b, c);
    return fabs(a * p.x + b * p.y + c) / sqrt(a * a + b * b);
}

void CNNDataCollector::GetAnnInput( CGameObjsArray *pObjsArray, float Input[ 128 ] )
{
	// Ship info : 10
	Input[ 0 ] = (float) pObjsArray->m_oShip.m_iShield;
	Input[ 1 ] = (float) pObjsArray->m_oShip.m_iHypSpaceCooldown;
	Input[ 2 ] = (float) pObjsArray->m_oShip.m_iReload;
	Input[ 3 ] = (float) pObjsArray->m_oShip.m_iBullets;

	Input[ 4 ] = (float) pObjsArray->m_oShip.m_oCenter.x * 2 / GAME_WIDTH  - 1; // (-1; 1)
	Input[ 5 ] = (float) pObjsArray->m_oShip.m_oCenter.y * 2 / GAME_HEIGHT - 1; // (-1; 1)

	Input[ 6 ] = (float) pObjsArray->m_oShip.m_oMovementVector.x;
	Input[ 7 ] = (float) pObjsArray->m_oShip.m_oMovementVector.y;

	Input[ 8 ] = (float) pObjsArray->m_oShip.m_dDirection;
	Input[ 9 ] = (float) pObjsArray->m_oShip.m_iInvisible;

	// Rays ( length and team ) : 59

	const int NumOfRays = 59;

	for( int k = 0; k < NumOfRays; k++ )
	{
		Input[10 + 2*k] = 1e8f; // len
		Input[11 + 2*k] = 0.f; // team;
	}

	POSITION pos = pObjsArray->m_aObjects.GetHeadPosition();
	
	while( pos )
	{
		CGameObj *pObj = pObjsArray->m_aObjects.GetNext( pos );

		CCoord Dif = pObj->m_oCenter - pObjsArray->m_oShip.m_oCenter;

		if( Dif.x > GAME_WIDTH  * 0.5 )  Dif.x -= GAME_WIDTH;
		else if( Dif.x < - GAME_WIDTH  * 0.5 )  Dif.x += GAME_WIDTH;

		if( Dif.y > GAME_HEIGHT * 0.5 ) Dif.y -= GAME_HEIGHT;
		else if( Dif.y < - GAME_HEIGHT * 0.5 ) Dif.y += GAME_HEIGHT;

		for( int i = 0; i < NumOfRays; i++ )
		{
			CCoord a( 1, 0 );
			a.Rotate( CCoord(0,0), 2*PI / NumOfRays * i + pObjsArray->m_oShip.m_dDirection );

			CCoord b = a;
			b.Rotate( CCoord(0,0), -PI/2 );

			// check if object is on correct side
			if( cross( b, Dif ) < 0 ) continue;

			double ObjDist = dist( CCoord(0,0), a, Dif );

			if( ObjDist < pObj->m_dHitboxRadius ) continue;

			double TrueDist = max( 
				0, 
				ObjDist - sqrt(pObj->m_dHitboxRadius*pObj->m_dHitboxRadius + ObjDist*ObjDist)
				);

			if( Input[ 10 + i*2 ] > TrueDist )
			{
				Input[ 10 + i*2 ] = (float)TrueDist;
				Input[ 11 + i*2 ] = (float)pObj->m_iTeam;
			}
		}
	}
}
