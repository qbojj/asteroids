// GameObjsArray.cpp: implementation of the CGameObjsArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Asteroids.h"
#include "GameObjsArray.h"
#include "AsteroidsView.h"

#include <afxtempl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameObjsArray::CGameObjsArray()
{
	m_oShip.m_pList = this;
	m_iScore = 0;

	m_bShowCollisionBoxes = false;
	m_iEnemies = 0;

	m_sndDestroy.CreateFromFile("snd\\explode.wav");
	m_sndUFODestroy.CreateFromFile("snd\\ufoend.wav");

	SetState( STATE_NORMAL );

	m_bReviving = false;
	m_iTimeToRevive = 0;

	m_bInGame = false;

	m_bEndGame = false;
}

CGameObjsArray::~CGameObjsArray()
{
	DeleteAll();
}

void CGameObjsArray::Tick( int TimeDiff )
{
	m_iAnimCounter += TimeDiff;

	switch( m_iState )
	{
	case STATE_NORMAL:
		CGameObj *obj;
		POSITION pos;
/*
		if( m_oShip.m_pNextObj == NULL && ! m_aObjects.IsEmpty() )
		{
			obj = m_aObjects.GetTail();
			m_oShip.m_pNextObj = obj;
			obj->m_pPrevObj = &m_oShip;
		}
*/
		m_oShip.Tick( TimeDiff );

		if( ! m_aObjects.IsEmpty() )
		{
			pos = m_aObjects.GetTailPosition();

			do
			{
				m_aObjects.GetPrev( pos )->Tick( TimeDiff );	
			}
			while( pos );
		}

		//Collisions && deleting flagged objs
		m_oShip.CheckColisions();

		if( ! m_aObjects.IsEmpty() )
		{
			POSITION lastPos;
			pos = m_aObjects.GetTailPosition();

			do
			{
				lastPos = pos;

				obj = m_aObjects.GetPrev( pos );
				obj->CheckColisions();	

				if( obj->m_bForDestruction )
				{
					if( obj->m_iKind == KIND_ASTEROID )
					{
						m_sndDestroy.Play();
						m_iEnemies--;
					}

					if( obj->m_iKind == KIND_UFO ) 
					{
						m_sndUFODestroy.Play();
						m_iEnemies--;
					}

					m_iScore += obj->m_iScoreForDestruction;
					delete obj;
					m_aObjects.RemoveAt( lastPos );
				}
			}
			while( pos );

			if( m_iTimeToRevive > 0 ) m_iTimeToRevive -= TimeDiff;

			if( m_bReviving && m_iTimeToRevive <= 0 && !m_bEndGame )
			{
				m_bReviving = false;
				m_iTimeToRevive = 0;

				m_oShip.m_oCenter = CCoord( GAME_WIDTH * 0.5, GAME_HEIGHT * 0.5 );
				m_oShip.m_oMovementVector = CCoord( 0, 0 );
				m_oShip.m_dDirection = 0;

				m_oShip.m_iGod = 2000;

				m_oShip.m_iShield = 1;
				m_oShip.m_iTimeToShieldReload = 0;

				m_oShip.m_bForDestruction = false;
			}

			if( m_oShip.m_bForDestruction && m_bInGame && ! m_bReviving && !m_bEndGame )
			{
				m_bReviving = true;
				m_iTimeToRevive = 5000;

				m_oShip.m_iLifes--;

				AddObj( new CShipCorpse( m_oShip.m_oCenter, m_oShip.m_oMovementVector ) );

				if( m_oShip.m_iLifes <= 0 )
				{
					m_bReviving = false;
					m_bEndGame = true;
				}
			}
		}
		break;
	case STATE_HYP_SPC:
		if( m_iAnimCounter >= HYP_SPC_LEN )
		{
			m_oShip.m_oCenter = m_oShip.m_oHypSpcNewPos;
			SetState( STATE_NORMAL );
		}
		break;
	}
}

void CGameObjsArray::Draw( CDC *pDC, bool ShowScore /*= true*/, double Conv /*= 1*/ )
{
	switch( m_iState )
	{
	case STATE_NORMAL: m_oShip.Draw( pDC, Conv );
		break;
	case STATE_HYP_SPC: m_oShip.DrawHypSpc( pDC, 1.0 * m_iAnimCounter / HYP_SPC_LEN, Conv );
		break;
	}

	if( ! m_aObjects.IsEmpty() )
	{
		//int i = 0;

		POSITION pos = m_aObjects.GetTailPosition();

		do
		{
			CGameObj *obj = m_aObjects.GetPrev( pos );

			//CString a;
			//a.Format( "%d", i );

			//pDC->TextOut( obj->m_oCenter.x * Conv, obj->m_oCenter.y * Conv, a );
			//i++;

			obj->Draw( pDC, Conv );
			if( m_bShowCollisionBoxes ) pDC->Ellipse( round( obj->m_oCenter.x - obj->m_dHitboxRadius ), round( obj->m_oCenter.y - obj->m_dHitboxRadius ), round( obj->m_oCenter.x +obj-> m_dHitboxRadius ), round( obj->m_oCenter.y + obj->m_dHitboxRadius ) );
		}
		while( pos );
	}
	
	if( ShowScore )
	{
		CString str;
		str.Format( "%d", m_iScore );

		pDC->TextOut( (int)( GAME_WIDTH * 0.5 * Conv ), 0, str );

		CShip ship;
		ship.m_dHitboxRadius *= 0.75;
		ship.m_dDirection = PI * 1.5;
		ship.m_iShield = 0;

		for( int i = 1; i <= m_oShip.m_iLifes; i++ ) ship.DrawElement( pDC, CCoord( ship.m_dHitboxRadius * i * 2 , ship.m_dHitboxRadius * 2 ) * Conv, Conv );
	}
}

void CGameObjsArray::AddObj( CGameObj *pNewObj )
{
	if( pNewObj )
	{
		pNewObj->m_pList = this;

		if( ! m_aObjects.IsEmpty() )
		{
			CGameObj *FirstObj = m_aObjects.GetHead();

			pNewObj->m_pPrevObj = FirstObj;
			FirstObj->m_pNextObj = pNewObj;
		}
		else
		{
			pNewObj->m_pPrevObj = &m_oShip;
			m_oShip .m_pNextObj = pNewObj;
		}

		m_aObjects.AddHead( pNewObj );

		if( pNewObj->m_iKind == KIND_ASTEROID || pNewObj->m_iKind == KIND_UFO ) m_iEnemies++;
	}
}

void CGameObjsArray::DeleteAll()
{
	while( !m_aObjects.IsEmpty() )	delete m_aObjects.RemoveHead();

	m_iEnemies = 0;
}

void CGameObjsArray::Restart()
{
	DeleteAll();
	m_iScore = 0;
	SetState( STATE_NORMAL );
	m_oShip.Restart();

	m_bEndGame = false;
}

void CGameObjsArray::SetState( int state /*= STATE_NORMAL*/ )
{
	m_iState = state;
	m_iAnimCounter = 0;
}