# Microsoft Developer Studio Project File - Name="MakeReport" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MakeReport - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MakeReport.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MakeReport.mak" CFG="MakeReport - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MakeReport - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MakeReport - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DataCenter/MakeReport", SPBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MakeReport - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "D:\Program Files\Microsoft Visual Studio\VC98\MFC\SRC" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib Ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"../MakeReport.exe"

!ELSEIF  "$(CFG)" == "MakeReport - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "D:\Program Files\Microsoft Visual Studio\VC98\MFC\SRC" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib Ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"./MakeReport.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MakeReport - Win32 Release"
# Name "MakeReport - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\connection.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\coolmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrlcoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrlcoolmenubar.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrliconbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrltaskpanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlTelecomPanel.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ctrltext.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlUnicomPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgabout.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgprofilesmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgsettingsmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSplash.cpp
# End Source File
# Begin Source File

SOURCE=.\EDClock.cpp
# End Source File
# Begin Source File

SOURCE=.\imagefile.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeReport.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeReport.rc
# End Source File
# Begin Source File

SOURCE=.\PageProfileTelecom.cpp
# End Source File
# Begin Source File

SOURCE=.\PageProfileUnicom.cpp
# End Source File
# Begin Source File

SOURCE=.\pagesettingsgeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsTelecom.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsUnicom.cpp
# End Source File
# Begin Source File

SOURCE=.\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\skin.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\windowmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\wndchild.cpp
# End Source File
# Begin Source File

SOURCE=.\wndmain.cpp
# End Source File
# Begin Source File

SOURCE=.\wndsettingspage.cpp
# End Source File
# Begin Source File

SOURCE=.\wndsettingssheet.cpp
# End Source File
# Begin Source File

SOURCE=.\WndTelecom.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WndUnicom.cpp
# End Source File
# Begin Source File

SOURCE=.\xml.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\buffer.h
# End Source File
# Begin Source File

SOURCE=.\connection.h
# End Source File
# Begin Source File

SOURCE=.\CoolInterface.h
# End Source File
# Begin Source File

SOURCE=.\coolmenu.h
# End Source File
# Begin Source File

SOURCE=.\ctrlcoolbar.h
# End Source File
# Begin Source File

SOURCE=.\ctrlcoolmenubar.h
# End Source File
# Begin Source File

SOURCE=.\ctrliconbutton.h
# End Source File
# Begin Source File

SOURCE=.\ctrltaskpanel.h
# End Source File
# Begin Source File

SOURCE=.\CtrlTelecomPanel.h
# End Source File
# Begin Source File

SOURCE=.\ctrltext.h
# End Source File
# Begin Source File

SOURCE=.\CtrlUnicomPanel.h
# End Source File
# Begin Source File

SOURCE=.\Dlgabout.h
# End Source File
# Begin Source File

SOURCE=.\dlgprofilesmanager.h
# End Source File
# Begin Source File

SOURCE=.\dlgsettingsmanager.h
# End Source File
# Begin Source File

SOURCE=.\DlgSplash.h
# End Source File
# Begin Source File

SOURCE=.\EDClock.h
# End Source File
# Begin Source File

SOURCE=.\imagefile.h
# End Source File
# Begin Source File

SOURCE=.\MakeReport.h
# End Source File
# Begin Source File

SOURCE=.\PageProfileTelecom.h
# End Source File
# Begin Source File

SOURCE=.\PageProfileUnicom.h
# End Source File
# Begin Source File

SOURCE=.\pagesettingsgeneral.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsTelecom.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsUnicom.h
# End Source File
# Begin Source File

SOURCE=.\registry.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\skin.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\windowmanager.h
# End Source File
# Begin Source File

SOURCE=.\wndchild.h
# End Source File
# Begin Source File

SOURCE=.\wndmain.h
# End Source File
# Begin Source File

SOURCE=.\wndsettingspage.h
# End Source File
# Begin Source File

SOURCE=.\wndsettingssheet.h
# End Source File
# Begin Source File

SOURCE=.\WndTelecom.h
# End Source File
# Begin Source File

SOURCE=.\WndUnicom.h
# End Source File
# Begin Source File

SOURCE=.\xml.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\EDClock.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Hand.cur
# End Source File
# Begin Source File

SOURCE=.\Res\MakeReport.ico
# End Source File
# Begin Source File

SOURCE=.\Res\MenuBar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\PanelMark.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\products.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\SearchFolder.ico
# End Source File
# Begin Source File

SOURCE=.\Res\TaskBoxCaption.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\TaskPanel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\TaskPanelFooter.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Res\Default.xml
# End Source File
# Begin Source File

SOURCE=.\Res\Definitions.xml
# End Source File
# Begin Source File

SOURCE=.\Res\manifest.xml
# End Source File
# End Target
# End Project
