; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainWnd
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "xmonitor.h"
LastPage=0

ClassCount=25
Class1=CCoolBarCtrl
Class2=CCoolMenuBarCtrl
Class3=CHomePanel
Class4=CHomeTaskBox
Class5=CHomeListenBox
Class6=CIconButtonCtrl
Class7=CNetworkChild
Class8=CNetworkHeaderBar
Class9=CNetworkPanel
Class10=CNetworkTaskBox
Class11=CTaskPanel
Class12=CTaskBox
Class13=CAboutDlg
Class14=CConnectDlg
Class15=CSettingsManagerDlg
Class16=CSplashDlg
Class17=CGeneralSettingsPage
Class18=CNetworkSettingsPage
Class19=CWindowManager
Class20=CChildWnd
Class21=CMainWnd
Class22=CNetworkWnd
Class23=CSettingsPage
Class24=CSettingsSheet
Class25=CXmonitorApp

ResourceCount=11
Resource1=IDD_SETTINGS_GENERAL
Resource2=IDD_AGENCY_PROPERTIES
Resource3=IDD_SPLASH
Resource4=IDD_AGENCY_XPROPERTY
Resource5=IDR_CHTYPE
Resource6=IDD_SETTINGS_NETWORK
Resource7=IDD_ABOUT
Resource8=IDD_AGENCY_IMPORT
Resource9=IDD_CONNECT
Resource10=IDR_MAINFRAME
Resource11=IDR_AGENCY

[CLS:CCoolBarCtrl]
Type=0
BaseClass=CControlBar
HeaderFile=CtrlCoolBar.h
ImplementationFile=CtrlCoolBar.cpp

[CLS:CCoolMenuBarCtrl]
Type=0
BaseClass=CCoolBarCtrl
HeaderFile=CtrlCoolMenuBar.h
ImplementationFile=CtrlCoolMenuBar.cpp

[CLS:CHomePanel]
Type=0
BaseClass=CTaskPanel
HeaderFile=CtrlHomePanel.h
ImplementationFile=CtrlHomePanel.cpp

[CLS:CHomeTaskBox]
Type=0
BaseClass=CTaskBox
HeaderFile=CtrlHomePanel.h
ImplementationFile=CtrlHomePanel.cpp

[CLS:CHomeListenBox]
Type=0
BaseClass=CTaskBox
HeaderFile=CtrlHomePanel.h
ImplementationFile=CtrlHomePanel.cpp

[CLS:CIconButtonCtrl]
Type=0
BaseClass=CWnd
HeaderFile=CtrlIconButton.h
ImplementationFile=CtrlIconButton.cpp

[CLS:CNetworkChild]
Type=0
BaseClass=CWnd
HeaderFile=CtrlNetworkChild.h
ImplementationFile=CtrlNetworkChild.cpp

[CLS:CNetworkHeaderBar]
Type=0
BaseClass=CCoolBarCtrl
HeaderFile=CtrlNetworkHeaderBar.h
ImplementationFile=CtrlNetworkHeaderBar.cpp
LastObject=ID_TASK_AGENCY

[CLS:CNetworkPanel]
Type=0
BaseClass=CTaskPanel
HeaderFile=CtrlNetworkPanel.h
ImplementationFile=CtrlNetworkPanel.cpp

[CLS:CNetworkTaskBox]
Type=0
BaseClass=CTaskBox
HeaderFile=CtrlNetworkPanel.h
ImplementationFile=CtrlNetworkPanel.cpp

[CLS:CTaskPanel]
Type=0
BaseClass=CWnd
HeaderFile=CtrlTaskPanel.h
ImplementationFile=CtrlTaskPanel.cpp

[CLS:CTaskBox]
Type=0
BaseClass=CWnd
HeaderFile=CtrlTaskPanel.h
ImplementationFile=CtrlTaskPanel.cpp

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

[CLS:CSettingsManagerDlg]
Type=0
BaseClass=CSettingsSheet
HeaderFile=dlgsettingsmanager.h
ImplementationFile=dlgsettingsmanager.cpp

[CLS:CSplashDlg]
Type=0
BaseClass=CDialog
HeaderFile=DlgSplash.h
ImplementationFile=DlgSplash.cpp

[CLS:CGeneralSettingsPage]
Type=0
BaseClass=CSettingsPage
HeaderFile=pagesettingsgeneral.h
ImplementationFile=pagesettingsgeneral.cpp

[CLS:CNetworkSettingsPage]
Type=0
BaseClass=CSettingsPage
HeaderFile=PageSettingsNetwork.h
ImplementationFile=PageSettingsNetwork.cpp

[CLS:CWindowManager]
Type=0
BaseClass=CWnd
HeaderFile=WindowManager.h
ImplementationFile=WindowManager.cpp

[CLS:CChildWnd]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=WndChild.h
ImplementationFile=WndChild.cpp

[CLS:CMainWnd]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=WndMain.h
ImplementationFile=WndMain.cpp
LastObject=CMainWnd

[CLS:CNetworkWnd]
Type=0
BaseClass=CChildWnd
HeaderFile=WndNetwork.h
ImplementationFile=WndNetwork.cpp

[CLS:CSettingsPage]
Type=0
BaseClass=CDialog
HeaderFile=wndsettingspage.h
ImplementationFile=wndsettingspage.cpp

[CLS:CSettingsSheet]
Type=0
BaseClass=CDialog
HeaderFile=wndsettingssheet.h
ImplementationFile=wndsettingssheet.cpp

[CLS:CXmonitorApp]
Type=0
BaseClass=CWinApp
HeaderFile=Xmonitor.h
ImplementationFile=Xmonitor.cpp

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
Control8=IDOK,button,1342242817
Control9=IDCANCEL,button,1342242816
Control10=IDC_SETTINGS,button,1342242816
Control11=IDC_REMOTE,button,1342308359
Control12=IDC_STATIC,static,1342177792
Control13=IDC_HOST,combobox,1344340226
Control14=IDC_STATIC,static,1342177792
Control15=IDC_DATABASE,edit,1350631552

[DLG:IDD_SPLASH]
Type=1
Class=CSplashDlg
ControlCount=0

[DLG:IDD_SETTINGS_GENERAL]
Type=1
Class=CGeneralSettingsPage
ControlCount=5
Control1=IDC_STATIC,static,1342308864
Control2=IDC_HOST,combobox,1344340226
Control3=IDC_STATIC,static,1342308864
Control4=IDC_STATIC,button,1342308359
Control5=IDC_DATABASE,combobox,1344340226

[DLG:IDD_SETTINGS_NETWORK]
Type=1
Class=CNetworkSettingsPage
ControlCount=5
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308864
Control3=IDC_STATIC,static,1342308864
Control4=IDC_PORT,edit,1350639744
Control5=IDC_HOST,combobox,1344340226

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=ID_TASK_AGENCY
Command3=ID_TOOLS_SETTINGS
Command4=ID_WINDOW_TOOLBAR
Command5=ID_WINDOW_STATUS_BAR
Command6=ID_APP_ABOUT
CommandCount=6

[MNU:IDR_CHTYPE]
Type=1
Class=?
Command1=ID_CHTYPE_USER
Command2=ID_CHTYPE_REC
Command3=ID_CHTYPE_FAX
Command4=ID_CHTYPE_SS1
Command5=ID_CHTYPE_SS7
Command6=ID_CHTYPE_ANALOG
CommandCount=6

[DLG:IDD_AGENCY_PROPERTIES]
Type=1
Class=?
ControlCount=14
Control1=IDC_STATIC,button,1342308359
Control2=IDC_STATIC,static,1342308864
Control3=IDC_JOBS,edit,1350631552
Control4=IDC_STATIC,static,1342308864
Control5=IDC_DESC,edit,1352732676
Control6=IDC_STATIC,static,1342308864
Control7=IDC_OPENING,edit,1350635588
Control8=IDC_STATIC,static,1342308864
Control9=IDC_EXPIRED,SysDateTimePick32,1342242852
Control10=IDOK,button,1342242817
Control11=IDCANCEL,button,1342242816
Control12=IDC_STATIC,static,1342181382
Control13=IDC_STATIC,static,1342308864
Control14=IDC_FEE_NUMBER,edit,1350639744

[DLG:IDD_AGENCY_IMPORT]
Type=1
Class=?
ControlCount=7
Control1=IDC_STATIC,button,1342308359
Control2=IDC_STATIC,static,1342308864
Control3=IDC_PATH_IMPORT,edit,1350631552
Control4=IDC_PATH_BROWSE,button,1342242816
Control5=IDC_OVERWRITE,button,1476460547
Control6=IDOK,button,1342242817
Control7=IDCANCEL,button,1342242816

[DLG:IDD_AGENCY_XPROPERTY]
Type=1
Class=?
ControlCount=13
Control1=IDC_STATIC,button,1342308359
Control2=IDC_STATIC,static,1342308864
Control3=IDC_PHONE,edit,1350631552
Control4=IDC_STATIC,static,1342308864
Control5=IDC_DESC,edit,1350635588
Control6=IDC_STATIC,static,1342308864
Control7=IDC_DATETIME,SysDateTimePick32,1342242852
Control8=IDOK,button,1342242817
Control9=IDCANCEL,button,1342242816
Control10=IDC_STATIC,static,1342308864
Control11=IDC_USERID,combobox,1344340514
Control12=IDC_STATIC,static,1342308864
Control13=IDC_STATE,combobox,1344339970

[MNU:IDR_AGENCY]
Type=1
Class=?
Command1=ID_AGENCY_XPROPERTY
Command2=ID_AGENCY_XADD
Command3=ID_AGENCY_XREMOVE
Command4=ID_AGENCY_IMPORT
Command5=ID_AGENCY_EXPORT
CommandCount=5

