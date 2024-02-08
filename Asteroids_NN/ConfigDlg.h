#if !defined(AFX_CHANGEKEYBINDINGS_H__8F455DD2_A4CD_4AC2_B801_76CDC5DF43BE__INCLUDED_)
#define AFX_CHANGEKEYBINDINGS_H__8F455DD2_A4CD_4AC2_B801_76CDC5DF43BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeKeyBindings.h : header file
//

#include "MyDialog.h"

#include "Resource.h"

#include <afxtempl.h>

enum{ KEY_BINDINGS, VISUAL, INPUT_SOURCE, NUM_OF_BLOCKS };

enum{ LEFT, RIGHT, ACC, SHOOT, HYP_SPACE, KEY_BIND_QTY };

#define SHIP_KEY_BIND_START 0
#define SHIP_KEY_BIND_QTY 5

enum{ KEYBOARD, JOYSTICK, NUM_OF_MODES };

enum{ SCALE_GAME, FULLSCREEN, ANTIALIASING, VISUAL_QTY };

enum{ INPUT_MODE, JOY_GUID, INPUT_SOURCE_QTY };

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog

enum{ NONE = -1, X_PLUS = -2, X_MINUS = -3, X_ROTATION_PLUS = -4, X_ROTATION_MINUS = -5, Y_PLUS = -6, Y_MINUS = -7, Y_ROTATION_PLUS = -8, Y_ROTATION_MINUS = -9, Z_PLUS = -10, Z_MINUS = -11, Z_ROTATION_PLUS = -12, Z_ROTATION_MINUS = -13, U_PLUS = -14, U_MINUS = -15, V_PLUS = -16, V_MINUS = -17, HAT_UP = -18, HAT_RIGHT = -19, HAT_DOWN = -20, HAT_LEFT = -21, MIN_CONTROLL_VALUE = -22 };

#define EV_CHECK_INPUT 2

UINT IsJoystickControllActive( int Controll );

class CConfigDlg : public CMyDialog
{
// Construction
public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor

	BOOL m_bScaleGame, m_bFullscreen, m_bAntialiasing;

	UINT m_aButtonCodes[ KEY_BIND_QTY ][ 2 ];
	int m_aJoystickCodes[ KEY_BIND_QTY ][ 2 ];

	UINT m_iInputMode;
	GUID m_oPrefferedDevice;

	void CheckKeyboardInput();
	void CheckJoystickInput();

protected:
	CArray< CString *, CString * > m_aJoystickButtonNames;
	GUID m_oLastPrefferedDevice;

// Dialog Data
	//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_CONFIG };
	CButton	m_oAntialiasing;
	CComboBox	m_oInputDevices;
	CButton	m_oFullscreen;
	CButton	m_oScaleGame;
	//}}AFX_DATA

	int  m_iClicked;
	bool m_bClickedSecondary;
	int  m_iLastClicked;
	bool m_bLastClickedSecondary;

	CButton m_aButtons[ KEY_BIND_QTY ][ 2 ];

	void ButtonClicked( UINT Button, bool IsSecondary );

	bool m_bFirstTime;

	void EraseButtonEntry( UINT Button, bool IsSecondary = true );

	bool m_bPressedOK;

	void UpdateDevicesList();

	CString GetButtonName( UINT Button, bool IsSecondary ); 

	void UpdateAllButtons();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDlg)
	afx_msg void OnButtonAccelerate();
	afx_msg void OnButtonLeft();
	afx_msg void OnButtonRight();
	afx_msg void OnButtonShoot();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonShoot2();
	afx_msg void OnButtonRight2();
	afx_msg void OnButtonLeft2();
	afx_msg void OnButtonAccelerate2();
	afx_msg void OnButtonLeftErase();
	afx_msg void OnButtonRightErase();
	afx_msg void OnButtonAccelerateErase();
	afx_msg void OnButtonShootErase();
	afx_msg void OnButtonHypSpc();
	afx_msg void OnButtonHypSpc2();
	afx_msg void OnButtonHypSpcErase();
	virtual void OnOK();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeInputDevices();
	afx_msg void OnDropdownInputDevices();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEKEYBINDINGS_H__8F455DD2_A4CD_4AC2_B801_76CDC5DF43BE__INCLUDED_)
