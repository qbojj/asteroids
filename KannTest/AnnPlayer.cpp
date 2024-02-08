// AnnPlayer.cpp: implementation of the CAnnPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AnnPlayer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnnPlayer::CAnnPlayer()
{
	m_bThreadExists = false;
}

CAnnPlayer::~CAnnPlayer()
{

}

BOOL CAnnPlayer::NewSim(CMyKann *pAnn )
{
	m_pAnn = pAnn;
	m_oReady.ResetEvent();
	m_iFitness = 0;

	if (!m_bThreadExists)
	{
		if( AfxBeginThread(ThreadFunction, this, THREAD_PRIORITY_IDLE) == NULL ) return false;
		m_bThreadExists = true;
	}

	return m_oStartThread.SetEvent();
}

UINT CAnnPlayer::ThreadFunction( LPVOID pParam )
{
	CAnnPlayer *p = (CAnnPlayer *)pParam;
	DWORD Ret;
	HANDLE a = p->m_oStartThread;

	while (g_bContinue)
	{
		Ret = WaitForSingleObject( a, 50 );

		if (Ret == WAIT_OBJECT_0)
		{
			p->GetFitness();
			p->m_oReady.SetEvent();
		}
		else if (Ret != WAIT_TIMEOUT) break;
	}

	p->m_bThreadExists = false;

	return 0;
}

void CAnnPlayer::GetFitness()
{
	m_oGameState.Restart();
	m_oGameThread.m_pGameObjsArray = &m_oGameState;
	m_oGameThread.m_bEnemyGen = true;
	m_oGameThread.m_bShowScore = true;
	m_oGameThread.m_iTickNum = 0;
	m_oGameThread.m_uiGameLen = 0;
	m_oGameThread.m_uiTimeDiff = 200; // -> 5 tps
	m_oGameState.m_bInGame = true;

	int scoreSum = 0;

	float *In = new float[INPUT_NEURONS];
	float *Out = new float[OUTPUT_NEURONS];
	int rep = 256;

	for( int i = 0; i < rep; i++ )
	{
		while (g_bContinue && !m_oGameState.m_bEndGame && m_oGameState.m_oShip.m_iLifes == 3)
		{
			//ASSERT(!m_oGameState.m_oShip.m_bForDestruction);

			
			CNNDataCollector::GetAnnInput(&m_oGameState, In);
			//const float *pControlls = kann_apply1(m_pAnn->m_pKann, In);
			m_fApply( m_pCtx, In, Out );
			for (int i = 0; i < OUTPUT_NEURONS; i++)
				m_oGameState.m_oShip.m_aControls[i] = Out[i] > .5 ? 1 : 0;

			m_oGameThread.m_uiGameLen += m_oGameThread.m_uiTimeDiff;
			if ( m_oGameState.m_iState == STATE_NORMAL ) m_oGameThread.GenerateEnemies();
			m_oGameThread.GameTick();

			//ASSERT(!m_oGameState.m_bEndGame);
		}

		scoreSum += m_oGameState.m_iScore;
		m_oGameState.Restart();
	}

	delete[] In;

	m_iFitness = scoreSum / rep;
}