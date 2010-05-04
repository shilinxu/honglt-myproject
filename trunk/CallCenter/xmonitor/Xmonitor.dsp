# Microsoft Developer Studio Project File - Name="Xmonitor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Xmonitor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Xmonitor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Xmonitor.mak" CFG="Xmonitor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Xmonitor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Xmonitor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CallCenter/Xmonitor", SQBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Xmonitor - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Common" /I "..\Xcenter" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib Ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"./Xmonitor.exe"

!ELSEIF  "$(CFG)" == "Xmonitor - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Common" /I "..\Xcenter" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib Ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"./Xmonitor.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Xmonitor - Win32 Release"
# Name "Xmonitor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Common\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Channel.cpp
# End Source File
# Begin Source File

SOURCE=.\ChannelMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\ChannelSentry.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\connection.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlAgencyList.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlAgencyPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlCoolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlCoolHeaderBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlIconButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkChild.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkHeaderBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlTaskPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrltext.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAgencyImport.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAgencyProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAgencyXproperty.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProfilesManager.cpp
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

SOURCE=.\ImageFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Network.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Packet.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\PacketCore.cpp
# End Source File
# Begin Source File

SOURCE=.\PageProfileMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\pagesettingsgeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsNetwork.cpp
# End Source File
# Begin Source File

SOURCE=.\Profiles.cpp
# End Source File
# Begin Source File

SOURCE=.\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\settings.cpp
# End Source File
# Begin Source File

SOURCE=.\Skin.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\transfer.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowManager.cpp
# End Source File
# Begin Source File

SOURCE=.\WndAgency.cpp
# End Source File
# Begin Source File

SOURCE=.\WndChild.cpp
# End Source File
# Begin Source File

SOURCE=.\WndMain.cpp
# End Source File
# Begin Source File

SOURCE=.\WndNetwork.cpp
# End Source File
# Begin Source File

SOURCE=.\wndsettingspage.cpp
# End Source File
# Begin Source File

SOURCE=.\wndsettingssheet.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\xml.cpp
# End Source File
# Begin Source File

SOURCE=.\Xmonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\Xmonitor.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Common\buffer.h
# End Source File
# Begin Source File

SOURCE=.\Channel.h
# End Source File
# Begin Source File

SOURCE=.\ChannelMonitor.h
# End Source File
# Begin Source File

SOURCE=.\ChannelSentry.h
# End Source File
# Begin Source File

SOURCE=..\Common\connection.h
# End Source File
# Begin Source File

SOURCE=.\CoolInterface.h
# End Source File
# Begin Source File

SOURCE=.\CoolMenu.h
# End Source File
# Begin Source File

SOURCE=.\CtrlAgencyList.h
# End Source File
# Begin Source File

SOURCE=.\CtrlAgencyPanel.h
# End Source File
# Begin Source File

SOURCE=.\CtrlCoolBar.h
# End Source File
# Begin Source File

SOURCE=.\CtrlCoolHeaderBar.h
# End Source File
# Begin Source File

SOURCE=.\CtrlIconButton.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkChild.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkHeaderBar.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkMonitor.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkPanel.h
# End Source File
# Begin Source File

SOURCE=.\CtrlTaskPanel.h
# End Source File
# Begin Source File

SOURCE=.\ctrltext.h
# End Source File
# Begin Source File

SOURCE=.\DlgAbout.h
# End Source File
# Begin Source File

SOURCE=.\DlgAgencyImport.h
# End Source File
# Begin Source File

SOURCE=.\DlgAgencyProperties.h
# End Source File
# Begin Source File

SOURCE=.\DlgAgencyXproperty.h
# End Source File
# Begin Source File

SOURCE=.\DlgConnect.h
# End Source File
# Begin Source File

SOURCE=.\DlgProfilesManager.h
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

SOURCE=.\ImageFile.h
# End Source File
# Begin Source File

SOURCE=.\Network.h
# End Source File
# Begin Source File

SOURCE=..\Common\Packet.h
# End Source File
# Begin Source File

SOURCE=..\Common\PacketCore.h
# End Source File
# Begin Source File

SOURCE=.\PageProfileMonitor.h
# End Source File
# Begin Source File

SOURCE=.\pagesettingsgeneral.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsNetwork.h
# End Source File
# Begin Source File

SOURCE=.\Profiles.h
# End Source File
# Begin Source File

SOURCE=.\registry.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\settings.h
# End Source File
# Begin Source File

SOURCE=.\Skin.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\transfer.h
# End Source File
# Begin Source File

SOURCE=.\WindowManager.h
# End Source File
# Begin Source File

SOURCE=.\WndAgency.h
# End Source File
# Begin Source File

SOURCE=.\WndChild.h
# End Source File
# Begin Source File

SOURCE=.\WndMain.h
# End Source File
# Begin Source File

SOURCE=.\WndNetwork.h
# End Source File
# Begin Source File

SOURCE=.\wndsettingspage.h
# End Source File
# Begin Source File

SOURCE=.\wndsettingssheet.h
# End Source File
# Begin Source File

SOURCE=..\Common\xml.h
# End Source File
# Begin Source File

SOURCE=.\Xmonitor.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\Agency.ico
# End Source File
# Begin Source File

SOURCE=.\res\ChState.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\connect.bmp
# End Source File
# Begin Source File

SOURCE=.\res\EDClock.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\Home.ico
# End Source File
# Begin Source File

SOURCE=.\res\MenuBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PanelMark.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\products.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SearchFolder.ico
# End Source File
# Begin Source File

SOURCE=.\Res\TaskBoxCaption.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TaskPanel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\TaskPanelFooter.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Xmonitor.ico
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
