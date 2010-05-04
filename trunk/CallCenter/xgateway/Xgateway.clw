; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CGeneralSettingsPage
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "xgateway.h"
LastPage=0

ClassCount=23
Class1=CCoolBarCtrl
Class2=CCoolMenuBarCtrl
Class3=CIconButtonCtrl
Class4=CNetworkChild
Class5=CNetworkHeaderBar
Class6=CNetworkTree
Class7=CNetworkTreeBar
Class8=CTaskPanel
Class9=CTaskBox
Class10=CTextCtrl
Class11=CAboutDlg
Class12=CProfileManagerDlg
Class13=CSettingsManagerDlg
Class14=CSplashDlg
Class15=CSecureProfilePage
Class16=CGeneralSettingsPage
Class17=CWindowManager
Class18=CChildWnd
Class19=CMainWnd
Class20=CNetworkWnd
Class21=CSettingsPage
Class22=CSettingsSheet
Class23=CXgatewayApp

ResourceCount=9
Resource1=IDD_SETTINGS_NETWORK
Resource2=IDD_SETTINGS_CENTRIC
Resource3=IDD_SPLASH
Resource4=IDD_SETTINGS_GENERAL
Resource5=IDD_SETTINGS_AUDIO
Resource6=IDD_PROFILE_SECURE
Resource7=IDD_SETTINGS_VIDEO
Resource8=IDD_ABOUT
Resource9=IDR_MAINFRAME

[CLS:CCoolBarCtrl]
Type=0
BaseClass=CControlBar
HeaderFile=ctrlcoolbar.h
ImplementationFile=ctrlcoolbar.cpp

[CLS:CCoolMenuBarCtrl]
Type=0
BaseClass=CCoolBarCtrl
HeaderFile=ctrlcoolmenubar.h
ImplementationFile=ctrlcoolmenubar.cpp

[CLS:CIconButtonCtrl]
Type=0
BaseClass=CWnd
HeaderFile=ctrliconbutton.h
ImplementationFile=ctrliconbutton.cpp

[CLS:CNetworkChild]
Type=0
BaseClass=CWnd
HeaderFile=CtrlNetworkChild.h
ImplementationFile=CtrlNetworkChild.cpp
LastObject=CNetworkChild

[CLS:CNetworkHeaderBar]
Type=0
BaseClass=CCoolBarCtrl
HeaderFile=CtrlNetworkHeaderBar.h
ImplementationFile=CtrlNetworkHeaderBar.cpp

[CLS:CNetworkTree]
Type=0
BaseClass=CTreeCtrl
HeaderFile=CtrlNetworkTree.h
ImplementationFile=CtrlNetworkTree.cpp

[CLS:CNetworkTreeBar]
Type=0
BaseClass=CCoolBarCtrl
HeaderFile=CtrlNetworkTreeBar.h
ImplementationFile=CtrlNetworkTreeBar.cpp

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

[CLS:CTextCtrl]
Type=0
BaseClass=CWnd
HeaderFile=ctrltext.h
ImplementationFile=ctrltext.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=dlgabout.h
ImplementationFile=dlgabout.cpp
LastObject=CAboutDlg

[CLS:CProfileManagerDlg]
Type=0
BaseClass=CSettingsSheet
HeaderFile=DlgProfileManager.h
ImplementationFile=DlgProfileManager.cpp

[CLS:CSettingsManagerDlg]
Type=0
BaseClass=CSettingsSheet
HeaderFile=dlgsettingsmanager.h
ImplementationFile=dlgsettingsmanager.cpp

[CLS:CSplashDlg]
Type=0
BaseClass=CDialog
HeaderFile=dlgsplash.h
ImplementationFile=dlgsplash.cpp

[CLS:CSecureProfilePage]
Type=0
BaseClass=CSettingsPage
HeaderFile=PageProfileSecure.h
ImplementationFile=PageProfileSecure.cpp

[CLS:CGeneralSettingsPage]
Type=0
BaseClass=CSettingsPage
HeaderFile=pagesettingsgeneral.h
ImplementationFile=pagesettingsgeneral.cpp
LastObject=CGeneralSettingsPage

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

[CLS:CXgatewayApp]
Type=0
BaseClass=CWinApp
HeaderFile=Xgateway.h
ImplementationFile=Xgateway.cpp

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

[DLG:IDD_SPLASH]
Type=1
Class=CSplashDlg
ControlCount=0

[DLG:IDD_PROFILE_SECURE]
Type=1
Class=CSecureProfilePage
ControlCount=10
Control1=IDC_LIST,SysListView32,1350778909
Control2=IDC_STATIC,button,1342308359
Control3=IDC_STATIC,static,1342308352
Control4=IDC_HOST,combobox,1344340226
Control5=IDC_STATIC,static,1342308352
Control6=IDC_DESC,edit,1352728644
Control7=IDC_STATIC,static,1342308352
Control8=IDC_ACTION,combobox,1344339971
Control9=IDC_ADD,button,1342242816
Control10=IDC_DELETE,button,1342373888

[DLG:IDD_SETTINGS_GENERAL]
Type=1
Class=CGeneralSettingsPage
ControlCount=12
Control1=IDC_STATIC,static,1342308864
Control2=IDC_HOST,combobox,1344340226
Control3=IDC_STATIC,static,1342308864
Control4=IDC_STATIC,button,1342308359
Control5=IDC_DATABASE,combobox,1344340226
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342308864
Control8=IDC_PATH_REC,edit,1350631552
Control9=IDC_PATH_REC_BROWSE,button,1342242816
Control10=IDC_STATIC,static,1342308864
Control11=IDC_REC_FILE,edit,1350631552
Control12=IDC_REC_FILE_DEFAULT,button,1342242816

[DLG:IDD_SETTINGS_AUDIO]
Type=1
Class=?
ControlCount=5
Control1=IDC_STATIC,static,1342308864
Control2=IDC_AUDIO_INPUT,combobox,1344340227
Control3=IDC_STATIC,button,1342308359
Control4=IDC_STATIC,static,1342308864
Control5=IDC_AUDIO_OUTPUT,combobox,1344340227

[DLG:IDD_SETTINGS_VIDEO]
Type=1
Class=?
ControlCount=5
Control1=65535,static,1342308864
Control2=IDC_VIDEO_INPUT,combobox,1344340227
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

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_NETWORK_CONNECT
Command2=ID_APP_EXIT
Command3=ID_TOOLS_SETTINGS
Command4=ID_WINDOW_TOOLBAR
Command5=ID_WINDOW_STATUS_BAR
Command6=ID_APP_ABOUT
CommandCount=6

[DLG:IDD_SETTINGS_CENTRIC]
Type=1
Class=?
ControlCount=5
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308864
Control3=IDC_LOCAL,edit,1350631552
Control4=IDC_STATIC,static,1342308864
Control5=IDC_REMOTE,edit,1350631552

