// AsteroidsView.h : interface of the CAsteroidsView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASTEROIDSVIEW_H__B21A0D69_3B4D_45DC_AFF7_49C829FE17E9__INCLUDED_)
#define AFX_ASTEROIDSVIEW_H__B21A0D69_3B4D_45DC_AFF7_49C829FE17E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AsteroidsDoc.h"
#include "GameObj.h"
#include "GameObjsArray.h"
#include "DirectSound.h"
#include "ConfigDlg.h"
#include "EndApp.h"
#include "DIJoystick.h"
#include <rpc.h>
#include "time.h"

#include <afxmt.h>

#include "GameThread.h"

#include "math.h"

#define DI_VER 0x0700

#define BORDER_FLAGS ( WS_BORDER | WS_CAPTION )

#define EV_TICK 0
#define EV_RETRY_GAME_LOOP 1

#define CLOCK_DELAY 20
#define TPS (1000.0 / CLOCK_DELAY)
#define WANTED_RES 1

#define TIME_MEASURE_CELLS 10

#define GAME_WIDTH  ( 0x300 )
#define GAME_HEIGHT ( 0x300 )
#define GAME_ASPECT ( 1.0 * GAME_WIDTH / GAME_HEIGHT )

#define GAME_OBJ_SIZE_MUL ( GAME_WIDTH / 0x280 )

enum{ WAIT_FOR_START, COUNTDOWN, GAME, GAME_OVER, SUBMIT_RECORD };

#define BUFFERS 2

#define UPDATE_SCREEN_ON_TICK	FALSE
#define FPS_DISP				TRUE
#define LOG_TICK_LEN			FALSE

#define ASK_FOR_QUIT			TRUE

#define TIMEOUT					FALSE
#define TIMEOUT_VALUE			1576258086

#define WM_GAMETICK		( WM_USER + 1 )
#define WM_END_OF_TICK	( WM_USER + 2 )

#define ENEMY_GEN_MAX_RAND 1000000

#define GAME_COLOR RGB( 255, 255, 255 )

extern bool g_bAntialiasing;
extern CDIJoystick	g_oJoystick;

#define START_TICK	"StartTick"
#define CLOSE_ALL	"CloseAll"

//extern CEvent g_oStartTick, g_oCloseAll;

const CRect g_oStartButton	( round(0.3 * GAME_WIDTH ), round(0.7 * GAME_HEIGHT ), round(0.7 * GAME_WIDTH ) + 1, round(0.9 * GAME_HEIGHT ) + 1 );
const CRect g_oConfigButton	( round(0.75 * GAME_WIDTH), round(0.82 * GAME_HEIGHT), round(0.95 * GAME_WIDTH) + 1, round(0.9 * GAME_HEIGHT ) + 1 );
const CRect g_oEndButton	( round(0.75 * GAME_WIDTH), round(0.7 * GAME_HEIGHT ), round(0.95 * GAME_WIDTH) + 1, round(0.78 * GAME_HEIGHT) + 1 );

class CAsteroidsView : public CView
{
protected: // create from serialization only
	CAsteroidsView();
	DECLARE_DYNCREATE(CAsteroidsView)

// Attributes
public:
	CAsteroidsDoc* GetDocument();

	CGameThread m_oGameThread;

	CEvent m_oStartTick, m_oCloseAll;

	BOOL m_bScaleGame;
	BOOL m_bFullscreen;

	bool m_bCursorVisible;

	void InitStartScreen();
	void InitGame();

	void StopSim();

	bool m_bInitialized;

	// true - game; false - start screen
	bool m_bGame;
	bool m_bEnemyGen;
	bool m_bShowScore;

	void CalcSizes();

	void StartGameThread();

	void ExtractFromConfigFile();
	void InsertToConfigFile();

	double m_dConv;

	UINT m_uiState;
	bool m_bSimulateGame;

	int m_iCurrentWidth, m_iCurrentHeight;

	int m_iWindowWidth, m_iWindowHeight;

	int m_iGameWndWidth, m_iGameWndHeight;

	CDC m_oGameDC[ BUFFERS ];
	CBitmap m_oGameBitmap[ BUFFERS ];
	bool m_bCorrectSize[ BUFFERS ];

	CBitmap m_oDefaultBitmap[ BUFFERS ];

	UINT m_uiCurrentBitmap, m_uiLastGiven;

	CFont m_oBigFont[ BUFFERS ];
	CFont m_oScoreFont[ BUFFERS ];
	CFont m_oScoreboardFont[ BUFFERS ];

	CGameObjsArray *m_poGameObjs;

	int m_iTicks;

	UINT m_uiLastTick;
	UINT m_uiLastDraw;

#if FPS_DISP
	UINT m_iElapse[ TIME_MEASURE_CELLS ];
	UINT m_iSum;
	UINT m_iPos;

	UINT m_iDrawElapse[ TIME_MEASURE_CELLS ];
	UINT m_iDrawSum;
	UINT m_iDrawPos;
#endif

	UINT m_uiStartGame;

	double m_dTimeMultiplayer;

	CString m_sInitials;

	void Restart();

	bool m_bFirstDrawTime, m_bFirstTickTime;

	double m_dLevel;

	GUID m_oPrefferedDevice;
	UINT m_iInputMode;

	int m_aJoyStickActions[ KEY_BIND_QTY ][ 2 ];
	UINT m_aActions[ KEY_BIND_QTY ][ 2 ];

	CConfigDlg m_wndConfig;
	CEndApp m_wndEndApp;

	void InitConfig();
	void SaveConfigChanges();

	void StartCountDown();

	void UpdateControlls();

	void EndApp();

	void UpdateFonts( int iSet );

	//-1 - Next
	int GenerateStartScreen( int Bitmap = -1 );

	void InitBitmap( int Btmp );

	UINT NextBitmap();

	void SelectBitmap( UINT Bitmap );

	void AddCharToScbEntry( UINT nChar );

	void UpdateBorder();

// Operations
public:
	void UpdateScreen();

	UINT UpdateTickTimes();
	UINT UpdateDrawTimes();

	LRESULT OnEndOfTick( WPARAM wParam, LPARAM lParam );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAsteroidsView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAsteroidsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAsteroidsView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimeX2();
	afx_msg void OnUpdateTimeX2(CCmdUI* pCmdUI);
	afx_msg void OnTimeX05();
	afx_msg void OnUpdateTimeX05(CCmdUI* pCmdUI);
	afx_msg void OnShowCollisionBoxes();
	afx_msg void OnUpdateShowCollisionBoxes(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()


protected:

	CDirectSound m_sndMusic;
	CDirectSound m_sndGameOver;
};

#ifndef _DEBUG  // debug version in AsteroidsView.cpp
inline CAsteroidsDoc* CAsteroidsView::GetDocument()
   { return (CAsteroidsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASTEROIDSVIEW_H__B21A0D69_3B4D_45DC_AFF7_49C829FE17E9__INCLUDED_)
