; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CConfigDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "asteroids.h"
LastPage=0

ClassCount=7
Class1=CAsteroidsApp
Class2=CAboutDlg
Class3=CAsteroidsDoc
Class4=CAsteroidsView
Class5=CConfigDlg
Class6=CEndApp
Class7=CMainFrame

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME_BCK
Resource3=IDD_CONFIG
Resource4=IDR_MAINFRAME
Resource5=IDD_END_APP

[CLS:CAsteroidsApp]
Type=0
BaseClass=CWinApp
HeaderFile=Asteroids.h
ImplementationFile=Asteroids.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Asteroids.cpp
ImplementationFile=Asteroids.cpp
LastObject=CAboutDlg

[CLS:CAsteroidsDoc]
Type=0
BaseClass=CDocument
HeaderFile=AsteroidsDoc.h
ImplementationFile=AsteroidsDoc.cpp

[CLS:CAsteroidsView]
Type=0
BaseClass=CView
HeaderFile=AsteroidsView.h
ImplementationFile=AsteroidsView.cpp
Filter=C
VirtualFilter=VWC
LastObject=CAsteroidsView

[CLS:CConfigDlg]
Type=0
BaseClass=CDialog
HeaderFile=ConfigDlg.h
ImplementationFile=ConfigDlg.cpp
LastObject=CConfigDlg
Filter=D
VirtualFilter=dWC

[CLS:CEndApp]
Type=0
BaseClass=CDialog
HeaderFile=EndApp.h
ImplementationFile=EndApp.cpp
Filter=D
VirtualFilter=dWC
LastObject=CEndApp

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame
Filter=T
VirtualFilter=fWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_CONFIG]
Type=1
Class=CConfigDlg
ControlCount=36
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_BUTTON_LEFT,button,1342242816
Control4=IDC_BUTTON_LEFT2,button,1342242816
Control5=IDC_BUTTON_LEFT_ERASE,button,1342242816
Control6=IDC_BUTTON_RIGHT,button,1342242816
Control7=IDC_BUTTON_RIGHT2,button,1342242816
Control8=IDC_BUTTON_RIGHT_ERASE,button,1342242816
Control9=IDC_BUTTON_ACCELERATE,button,1342242816
Control10=IDC_BUTTON_ACCELERATE2,button,1342242816
Control11=IDC_BUTTON_ACCELERATE_ERASE,button,1342242816
Control12=IDC_BUTTON_SHOOT,button,1342242816
Control13=IDC_BUTTON_SHOOT2,button,1342242816
Control14=IDC_BUTTON_SHOOT_ERASE,button,1342242816
Control15=IDC_BUTTON_HYP_SPC,button,1342242816
Control16=IDC_BUTTON_HYP_SPC2,button,1342242816
Control17=IDC_BUTTON_HYP_SPC_ERASE,button,1342242816
Control18=IDC_STATIC,button,1342177287
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_STATIC,static,1342308352
Control26=IDC_STATIC,static,1342308352
Control27=IDC_STATIC,button,1342177287
Control28=IDC_STATIC,static,1342308352
Control29=IDC_SCALE_GAME,button,1342242819
Control30=IDC_STATIC,static,1342308352
Control31=IDC_FULLSCREEN,button,1342242819
Control32=IDC_STATIC,static,1342308352
Control33=IDC_STATIC,static,1342308352
Control34=IDC_INPUT_DEVICES,combobox,1344339971
Control35=IDC_ANTIALIASING,button,1342242819
Control36=IDC_STATIC,static,1342308352

[DLG:IDD_END_APP]
Type=1
Class=CEndApp
ControlCount=3
Control1=IDYES,button,1342242817
Control2=IDNO,button,1342242816
Control3=IDC_STATIC,static,1342308865

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_TIME_X2
Command10=ID_TIME_X0_5
Command11=ID_SHOW_COLLISION_BOXES
CommandCount=11

[ACL:IDR_MAINFRAME_BCK]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_FILE_NEW
Command3=ID_FILE_OPEN
Command4=ID_FILE_SAVE
Command5=ID_EDIT_PASTE
Command6=ID_EDIT_UNDO
Command7=ID_EDIT_CUT
Command8=ID_NEXT_PANE
Command9=ID_PREV_PANE
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_EDIT_CUT
Command13=ID_EDIT_UNDO
CommandCount=13

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_VIEW_TOOLBAR
Command11=ID_VIEW_STATUS_BAR
Command12=ID_APP_ABOUT
CommandCount=12

