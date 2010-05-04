; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAgencyList
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "xsalesagency.h"
LastPage=0

ClassCount=6
Class1=CAgencyList
Class2=CAgencyPanel
Class3=CAgencyVideoBox
Class4=CAgencyPropertiesDlg
Class5=CAgencyWnd
Class6=CXsalesAgencyApp

ResourceCount=2
Resource1=IDD_AGENCY_LAYOUT
Resource2=IDD_AGENCY_PROPERTIES

[CLS:CAgencyList]
Type=0
BaseClass=CListCtrl
HeaderFile=CtrlAgencyList.h
ImplementationFile=CtrlAgencyList.cpp
LastObject=CAgencyList

[CLS:CAgencyPanel]
Type=0
BaseClass=CTaskPanel
HeaderFile=CtrlAgencyPanel.h
ImplementationFile=CtrlAgencyPanel.cpp

[CLS:CAgencyVideoBox]
Type=0
BaseClass=CTaskBox
HeaderFile=CtrlAgencyPanel.h
ImplementationFile=CtrlAgencyPanel.cpp

[CLS:CAgencyPropertiesDlg]
Type=0
BaseClass=CDialog
HeaderFile=DlgAgencyProperties.h
ImplementationFile=DlgAgencyProperties.cpp

[CLS:CAgencyWnd]
Type=0
BaseClass=CChildWnd
HeaderFile=WndAgency.h
ImplementationFile=WndAgency.cpp

[CLS:CXsalesAgencyApp]
Type=0
BaseClass=CWinApp
HeaderFile=XsalesAgency.h
ImplementationFile=XsalesAgency.cpp

[DLG:IDD_AGENCY_PROPERTIES]
Type=1
Class=CAgencyPropertiesDlg
ControlCount=14
Control1=IDC_STATIC,button,1342308359
Control2=IDC_STATIC,static,1342308864
Control3=IDC_JOBS,edit,1350631552
Control4=IDC_STATIC,static,1342308864
Control5=IDC_DESC,edit,1352732676
Control6=IDC_STATIC,static,1342308864
Control7=IDC_OPENING,edit,1352732676
Control8=IDC_STATIC,static,1342308864
Control9=IDC_EXPIRED,SysDateTimePick32,1342242852
Control10=IDOK,button,1342242817
Control11=IDCANCEL,button,1342242816
Control12=IDC_STATIC,static,1342181382
Control13=IDC_STATIC,static,1342308864
Control14=IDC_FEE_NUMBER,edit,1350639744

[DLG:IDD_AGENCY_LAYOUT]
Type=1
Class=?
ControlCount=15
Control1=IDC_STATIC,static,1342308864
Control2=IDC_DESC,edit,1352734724
Control3=IDC_STATIC,static,1342181382
Control4=IDC_STATIC,button,1342308359
Control5=IDC_STATIC,static,1342308864
Control6=IDC_PHONE,edit,1350633600
Control7=IDC_STATIC,static,1342308864
Control8=IDC_REASON,edit,1350635588
Control9=IDOK,button,1342242817
Control10=IDCANCEL,button,1342242816
Control11=IDC_STATIC,static,1342308864
Control12=IDC_STATE,combobox,1344339970
Control13=IDC_STATIC,static,1342308864
Control14=IDC_PROGRESS,edit,1350633600
Control15=IDC_STARTUP,button,1342242816

