// GameObjsArray.h: interface for the CGameObjsArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOBJSARRAY_H__4EAF9862_9D51_4622_B16D_97F12A09DDA4__INCLUDED_)
#define AFX_GAMEOBJSARRAY_H__4EAF9862_9D51_4622_B16D_97F12A09DDA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DirectSound.h"

#define HYP_SPC_LEN 3000

#include "GameObj.h"
#include <afxtempl.h>

enum{ STATE_NORMAL, STATE_HYP_SPC };

class CGameObjsArray  
{
public:
	CGameObjsArray();
	virtual ~CGameObjsArray();

	CList< CGameObj *, CGameObj * > m_aObjects;

	bool m_bEndGame;

	int m_iScore;

	CShip m_oShip;

	void Tick( int TimeDiff /* in Ms */ );
	void Draw( CDC *pDC, CFont *font, bool ShowScore = true, double Conv = 1 );

	BOOL m_bShowCollisionBoxes;

	BOOL m_iEnemies;

	int m_iState;
	int m_iAnimCounter;

	bool m_bInGame;

	bool m_bReviving;
	int m_iTimeToRevive;

	void AddObj( CGameObj *pNewObj );

	void DeleteAll();

	void Restart();

	void SetState( int state = STATE_NORMAL );

	CDirectSound m_sndDestroy;
	CDirectSound m_sndUFODestroy;
};

#endif // !defined(AFX_GAMEOBJSARRAY_H__4EAF9862_9D51_4622_B16D_97F12A09DDA4__INCLUDED_)
