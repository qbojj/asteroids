// GameThread.h: interface for the CGameThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMETHREAD_H__D1FF7D7F_174D_4A48_8FF2_D5D649E1E73B__INCLUDED_)
#define AFX_GAMETHREAD_H__D1FF7D7F_174D_4A48_8FF2_D5D649E1E73B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GameObjsArray.h"
#include <afxmt.h>

#include "NNDataCollector.h"

class CGameThread  
{
public:
	CGameThread();
	virtual ~CGameThread();

	bool			m_bThreadExists;
	bool			m_bInside;
	HWND			m_hMessageDest;

	CGameObjsArray	*m_pGameObjsArray;
	CBitmap			*m_pGameBitmap;

	UINT			m_uiTimeDiff, m_uiGameLen;
	bool			m_bShowScore;
	bool			m_bEnemyGen;
	double			m_dConv;

	UINT			m_uiDCNum;

	CWinThread*		m_pThread;

	CNNDataCollector m_oDataCollector;
	bool			m_bLogGame;
	int				m_iTickNum;

	void StartThread();


	static UINT ThreadFunction( LPVOID pParam );
	void SimulateGame();
	void GameTick();
	void DrawGame();
	void GenerateEnemies();

	static double GetDifficulty( double Seconds );

	CDC m_oGameDC;
	CPen m_oGamePen;
	CFont m_oGameFont;

	double m_dLastConv;

	unsigned int m_uiTimeFromLastLog;
};

#endif // !defined(AFX_GAMETHREAD_H__D1FF7D7F_174D_4A48_8FF2_D5D649E1E73B__INCLUDED_)
