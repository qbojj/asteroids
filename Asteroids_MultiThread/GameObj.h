// GameObj.h: interface for the CGameObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOBJ_H__84F49562_6C9E_49DB_ACEB_B25FE947B652__INCLUDED_)
#define AFX_GAMEOBJ_H__84F49562_6C9E_49DB_ACEB_B25FE947B652__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PI 3.14159265358979323846

#include "StdAfx.h"
#include "DirectSound.h"
#include "ConfigDlg.h"

enum { KIND_GENERIC, KIND_ASTEROID, KIND_UFO, KIND_SHIP, KIND_BULLET, KIND_CORPSE };

class CCoord
{
public:
	CCoord();
	CCoord( const CCoord &coord );
	CCoord( const CPoint &point );
	CCoord( const double x, const double y );

	CCoord operator +( const CCoord &coord ) const { return CCoord( x + coord.x, y + coord.y ); };
	CCoord operator -( const CCoord &coord ) const { return CCoord( x - coord.x, y - coord.y ); };
	CCoord operator *( double d ) const { return CCoord( x * d, y * d ); };

	BOOL operator ==( const CCoord &coord );

	CCoord &operator +=( const CCoord &coord );
	CCoord &operator -=( const CCoord &coord );
	CCoord &operator *=( double d );

	void Rotate( const CCoord &Center, double deg );

	operator CPoint();
	
	double x, y;
};

inline CCoord GetPointAtPercent( CCoord &Start, CCoord &End, double Percent )
{
	return Start + ( End - Start ) * Percent;
};

class CGameObjsArray;

class CGameObj  
{
public:
	CGameObj();
	CGameObj( const CCoord &StartPos, const CCoord &StartVector );
	virtual ~CGameObj();

	CCoord m_oCenter;
	CCoord m_oMovementVector;

	void Init( const CCoord &StartPos, const CCoord &StartVector );

	double m_dHitboxRadius;
	
	virtual void Tick( int TimeDiff );
	void Draw( CDC *pDC, double Conv );
	virtual BOOL CheckColisions();

	virtual void DrawElement( CDC *pDC, CCoord &Center, double Conv );
	virtual void Normalize();

	void GiveAwayPrevNextObj();

	void CreateGameObj( CGameObj *pNewObj );
	CGameObjsArray *m_pList;

	int m_iScoreForDestruction;

	int m_iShield;
	bool m_bShieldLoss;

	int m_iInvisible;
	bool m_bInvisibleLoss;

	int m_iGod;
	bool m_bGodLoss;

	// 0 - enemy, 1 - player, ...
	BOOL m_iTeam;

	BOOL m_bForDestruction;

	CGameObj *m_pPrevObj;
	CGameObj *m_pNextObj;

	int		m_iKind;
};

class CAsteroid : public CGameObj
{
public:
	CAsteroid( const CCoord &StartPos, const CCoord &StartVector, int size = 3 );
	virtual ~CAsteroid();

	virtual void DrawElement( CDC *pDC, CCoord &Center, double Conv );
	virtual void Tick( int TimeDiff );
	
	//Visual Type
	int m_iType;

	int m_iSize;

	double m_dRotation;
	double m_dRotationIncr;
};

class CShip : public CGameObj
{
public:
	CShip();

	void Restart();

	virtual void Tick( int TimeDiff );
	virtual void DrawElement( CDC *pDC, CCoord &Center, double Conv );
	virtual void Normalize();

	void DrawHypSpc( CDC *pDC, double AnimationPercent, double Conv );
	CCoord m_oHypSpcNewPos;

	void Accelerate( int TimeDiff );
	void Drag( int TimeDiff );

	void Shoot( int BulletLife );
	void GenerateBullet( double directoion, int BulletLife, CCoord &Recoil );

	double m_dDirection;

	int m_iHypSpaceCooldown;

	int m_iReload;
	int m_iBullets;

	int m_iTimeToShieldReload;

	bool m_aControls[ SHIP_KEY_BIND_QTY ];

	int m_iLifes;

	CDirectSound m_sndLaser;
	CDirectSound m_sndBigLaser;
	CDirectSound m_sndHyperspace;
};

class CBullet : public CGameObj
{
public:
	CBullet( CCoord &StartPos, CCoord &StartVector, BOOL Team = 1 );

	double m_iLifespan;

	bool m_bDoNotRunNext;

	virtual void Tick( int TimeDiff );
};

class CUfo : public CGameObj
{
public:
	CUfo( const CCoord &StartPos, const CCoord &StartVector, int type );
	virtual ~CUfo();

	virtual void Tick( int TimeDiff );
	virtual void DrawElement( CDC *pDC, CCoord &Center, double Conv );

	double m_dPlayerDirection;
	int m_iReload;

	void TrackPlayer();

	void Accelerate( int TimeDiff);
	void Drag( int TimeDiff);

	void Shoot( int BulletLife );

	void RandShoot( int BulletLife );

	int m_iType;

	int m_iBoing;

	static	CDirectSound m_sndUFO;
};

class CCorpse : public CGameObj
{
public:
	int m_iLifespan;

	CCoord m_aParticleDest[ 10 ];

	CCorpse( const CCoord &StartPos, const CCoord &StartVector );

	virtual void Tick( int TimeDiff );
	virtual void DrawElement( CDC *pDC, CCoord &Center, double Conv );
};

class CShipCorpse : public CCorpse
{
public:
	CShipCorpse( const CCoord &StartPos, const CCoord &StartVector );

	int m_aShowTimes[ 5 ];

	virtual void DrawElement( CDC *pDC, CCoord &Center, double Conv );
};

#endif // !defined(AFX_GAMEOBJ_H__84F49562_6C9E_49DB_ACEB_B25FE947B652__INCLUDED_)
