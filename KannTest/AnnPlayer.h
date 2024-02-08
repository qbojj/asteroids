// AnnPlayer.h: interface for the CAnnPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANNPLAYER_H__E564F4D1_54E3_4E2F_A630_C6F06E782CFE__INCLUDED_)
#define AFX_ANNPLAYER_H__E564F4D1_54E3_4E2F_A630_C6F06E782CFE__INCLUDED_

#include "stdafx.h"
#include "..\Asteroids\Asteroids_NN\GameThread.h"
#include "MyKann.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAnnPlayer  
{
public:
	CAnnPlayer();
	virtual ~CAnnPlayer();

	BOOL NewSim(CMyKann *pAnn);

	void *m_pCtx;
	void( *m_fApply )( void *ctx, const float input[INPUT_NEURONS], float output[OUTPUT_NEURONS]);

	int m_iFitness;
	CEvent m_oReady, m_oStartThread;

	static UINT ThreadFunction( LPVOID pParam );
	void GetFitness();
protected:
	CGameThread m_oGameThread;
	CGameObjsArray m_oGameState;

	bool m_bThreadExists;
};

#endif // !defined(AFX_ANNPLAYER_H__E564F4D1_54E3_4E2F_A630_C6F06E782CFE__INCLUDED_)
