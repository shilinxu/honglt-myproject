; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CConnectDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "xphone.h"
LastPage=0

ClassCount=6
Class1=CCoolBarCtrl
Class2=CConnectDlg
Class3=CWindowManager
Class4=CChildWnd
Class5=CMainWnd
Class6=CXphoneApp

ResourceCount=4
Resource1=IDD_CONNECT
Resource2=IDR_MAINFRAME
Resource3=IDD_SPLASH
Resource4=IDR_TRAYICON

[CLS:CCoolBarCtrl]
Type=0
BaseClass=CControlBar
HeaderFile=ctrlcoolbar.h
ImplementationFile=ctrlcoolbar.cpp

[CLS:CConnectDlg]
Type=0
BaseClass=CDialog
HeaderFile=dlgconnect.h
ImplementationFile=dlgconnect.cpp
Filter=D
VirtualFilter=dWC
LastObject=CConnectDlg

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
LastObject=CChildWnd

[CLS:CMainWnd]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=wndmain.h
ImplementationFile=wndmain.cpp

[CLS:CXphoneApp]
Type=0
BaseClass=CWinApp
HeaderFile=xphone.h
ImplementationFile=xphone.cpp

[DLG:IDD_CONNECT]
Type=1
Class=CConnectDlg
ControlCount=2
Control1=IDC_TOOLBAR,static,1073741831
Control2=IDC_VIDEO,static,1073741831

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

[DLG:IDD_SPLASH]
Type=1
Class=?
ControlCount=0

[MNU:IDR_TRAYICON]
Type=1
Class=?
Command1=ID_TRAY_OPEN
Command2=ID_APP_EXIT
CommandCount=2

