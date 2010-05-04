; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CNetworkWnd
LastTemplate=CListBox
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "xuser32.h"
LastPage=0

ClassCount=15
Class1=CCoolBarCtrl
Class2=CCoolHeaderBarCtrl
Class3=CNetworkHeaderBar
Class4=CNetworkPanel
Class5=CTaskPanel
Class6=CTaskBox
Class7=CAboutDlg
Class8=CConnectDlg
Class9=CSplashDlg
Class10=CWindowManager
Class11=CChildWnd
Class12=CMainWnd
Class13=CNetworkWnd
Class14=CXuser32App

ResourceCount=10
Resource1=IDD_SETTINGS_VIDEO
Resource2=IDD_ABOUT
Resource3=IDD_SETTINGS_GENERAL
Resource4=IDD_CONNECT
Resource5=IDD_SETTINGS_AUDIO
Resource6=IDD_SPLASH
Resource7=IDD_JOINVISIT
Class15=CNetworkThumb
Resource8=IDD_SETTINGS_NETWORK
Resource9=IDD_JOINVISITOR
Resource10=IDR_MAINFRAME

[CLS:CCoolBarCtrl]
Type=0
BaseClass=CControlBar
HeaderFile=ctrlcoolbar.h
ImplementationFile=ctrlcoolbar.cpp

[CLS:CCoolHeaderBarCtrl]
Type=0
BaseClass=CCoolBarCtrl
HeaderFile=CtrlCoolHeaderBar.h
ImplementationFile=CtrlCoolHeaderBar.cpp

[CLS:CNetworkHeaderBar]
Type=0
BaseClass=CCoolBarCtrl
HeaderFile=CtrlNetworkHeaderBar.h
ImplementationFile=CtrlNetworkHeaderBar.cpp

[CLS:CNetworkPanel]
Type=0
BaseClass=CTaskPanel
HeaderFile=CtrlNetworkPanel.h
ImplementationFile=CtrlNetworkPanel.cpp

[CLS:CTaskPanel]
Type=0
BaseClass=CWnd
HeaderFile=ctrltaskpanel.h
ImplementationFile=ctrltaskpanel.cpp

[CLS:CTaskBox]
Type=0
BaseClass=CWnd
HeaderFile=ctrltaskpanel.h
ImplementationFile=ctrltaskpanel.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=DlgAbout.h
ImplementationFile=DlgAbout.cpp
LastObject=CAboutDlg

[CLS:CConnectDlg]
Type=0
BaseClass=CDialog
HeaderFile=DlgConnect.h
ImplementationFile=DlgConnect.cpp

[CLS:CSplashDlg]
Type=0
BaseClass=CDialog
HeaderFile=dlgsplash.h
ImplementationFile=dlgsplash.cpp

[CLS:CWindowManager]
Type=0
BaseClass=CWnd
HeaderFile=windowmanager.h
ImplementationFile=windowmanager.cpp

[CLS:CChildWnd]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=wndchild.h
ImplementationFile=wndchild.cpp

[CLS:CMainWnd]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=wndmain.h
ImplementationFile=wndmain.cpp

[CLS:CNetworkWnd]
Type=0
BaseClass=CChildWnd
HeaderFile=WndNetwork.h
ImplementationFile=WndNetwork.cpp
LastObject=CNetworkWnd
Filter=W
VirtualFilter=WC

[CLS:CXuser32App]
Type=0
BaseClass=CWinApp
HeaderFile=Xuser32.h
ImplementationFile=Xuser32.cpp

[DLG:IDD_ABOUT]
Type=1
Class=CAboutDlg
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342177294
Control3=IDC_TITLE,static,1342308864
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342177298
Control6=IDC_WEB,static,1342308352
Control7=IDC_MAIL,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_REGISTRY,static,1342308352

[DLG:IDD_CONNECT]
Type=1
Class=CConnectDlg
ControlCount=15
Control1=IDC_STATIC,static,1342177294
Control2=IDC_STATIC,static,1342308864
Control3=IDC_USER_ID,combobox,1344348162
Control4=IDC_STATIC,static,1342308864
Control5=IDC_PASSWORD,edit,1350631584
Control6=IDC_SAVE_PASSWORD,button,1342242819
Control7=IDC_STATIC,static,1342181382
Control8=IDRETRY,button,1342242817
Control9=IDCANCEL,button,1342242816
Control10=IDC_SETTINGS,button,1342242816
Control11=IDC_REMOTE,button,1342308359
Control12=IDC_STATIC,static,1342177792
Control13=IDC_HOST,combobox,1344340226
Control14=IDC_STATIC,static,1342177792
Control15=IDC_PORT,edit,1350639744

[DLG:IDD_SPLASH]
Type=1
Class=CSplashDlg
ControlCount=0

[DLG:IDD_SETTINGS_VIDEO]
Type=1
Class=?
ControlCount=5
Control1=65535,static,1342308864
Control2=IDC_VIDEO_INPUT,combobox,1344339971
Control3=65535,button,1342308359
Control4=IDC_VIDEO_PROPERTY,button,1342242816
Control5=IDC_VIDEO_FRAME,static,1342177287

[DLG:IDD_SETTINGS_NETWORK]
Type=1
Class=?
ControlCount=10
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308864
Control3=IDC_STATIC,static,1342308864
Control4=IDC_PORT,edit,1350639744
Control5=IDC_HOST,combobox,1344340226
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342308864
Control8=IDC_STATIC,static,1342308864
Control9=IDC_MAX_RATE,edit,1350639744
Control10=IDC_MAX_PACKET,edit,1350639744

[DLG:IDD_SETTINGS_GENERAL]
Type=1
Class=?
ControlCount=5
Control1=65535,static,1342308864
Control2=IDC_HOST,combobox,1344340226
Control3=65535,static,1342308864
Control4=65535,button,1342308359
Control5=IDC_DATABASE,combobox,1344340226

[DLG:IDD_SETTINGS_AUDIO]
Type=1
Class=?
ControlCount=5
Control1=IDC_STATIC,static,1342308864
Control2=IDC_AUDIO_INPUT,combobox,1344339971
Control3=IDC_STATIC,button,1342308359
Control4=IDC_STATIC,static,1342308864
Control5=IDC_AUDIO_OUTPUT,combobox,1344339971

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=ID_TOOLS_SETTINGS
Command3=ID_WINDOW_TOOLBAR
Command4=ID_WINDOW_STATUS_BAR
Command5=ID_HELP_UPDATE
Command6=ID_APP_ABOUT
CommandCount=6

[CLS:CNetworkThumb]
Type=0
HeaderFile=CtrlNetworkThumb.h
ImplementationFile=CtrlNetworkThumb.cpp
BaseClass=CListBox
Filter=W
LastObject=CNetworkThumb
VirtualFilter=bWC

[DLG:IDD_JOINVISIT]
Type=1
Class=?
ControlCount=11
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342177283
Control4=IDC_STATIC,static,1342308864
Control5=IDC_STATIC,static,1073745926
Control6=IDC_STATIC,static,1342308864
Control7=IDC_USER_PASS0,edit,1350639776
Control8=IDC_STATIC,static,1342308864
Control9=IDC_USER_PASS1,edit,1350639776
Control10=IDC_STATIC,static,1342308864
Control11=IDC_USER_PASS2,edit,1350639776

[DLG:IDD_JOINVISITOR]
Type=1
Class=?
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342177283
Control4=IDC_STATIC,static,1342308864
Control5=IDC_STATIC,static,1342181382
Control6=IDC_STATIC,static,1342308864
Control7=IDC_REMITE_ID,edit,1350641792
Control8=IDC_STATIC,static,1342308864
Control9=IDC_PLACEHOLDER,static,1073741831
Control10=IDC_LENGTH,static,1342308864

