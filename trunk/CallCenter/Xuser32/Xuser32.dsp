# Microsoft Developer Studio Project File - Name="Xuser32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=XUSER32 - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Xuser32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Xuser32.mak" CFG="XUSER32 - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Xuser32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Xuser32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CallCenter/Xuser32", HKDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Xuser32 - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Common" /I "Xuserface" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib Ws2_32.lib strmbase.lib Xuserface.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /out:"./Xuser32.exe" /libpath:"Xuserface\Release"

!ELSEIF  "$(CFG)" == "Xuser32 - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Common" /I "Xuserface" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib Ws2_32.lib strmbasd.lib Xuserface.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMTD" /out:".\Xuser32.exe" /pdbtype:sept /libpath:"Xuserface\Debug"

!ENDIF 

# Begin Target

# Name "Xuser32 - Win32 Release"
# Name "Xuser32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Common\buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\connection.cpp
# End Source File
# Begin Source File

SOURCE=.\coolinterface.cpp
# End Source File
# Begin Source File

SOURCE=.\coolmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrlcoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlCoolHeaderBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlIconCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkThumb.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrltaskpanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlTrafficPanel.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DlgAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgJoinVisitor.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\dlgsettingsmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgsplash.cpp
# End Source File
# Begin Source File

SOURCE=.\imagefile.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\neighbour.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Common\Packet.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\PacketBase.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\PacketCore.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsVideo.cpp
# End Source File
# Begin Source File

SOURCE=.\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\settings.cpp
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

SOURCE=.\WndNetwork.cpp
# End Source File
# Begin Source File

SOURCE=.\wndsettingspage.cpp
# End Source File
# Begin Source File

SOURCE=.\wndsettingssheet.cpp
# End Source File
# Begin Source File

SOURCE=.\WndTraffic.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Common\xml.cpp
# End Source File
# Begin Source File

SOURCE=.\Xuser32.cpp
# End Source File
# Begin Source File

SOURCE=.\Xuser32.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Common\buffer.h
# End Source File
# Begin Source File

SOURCE=..\Common\connection.h
# End Source File
# Begin Source File

SOURCE=.\coolinterface.h
# End Source File
# Begin Source File

SOURCE=.\coolmenu.h
# End Source File
# Begin Source File

SOURCE=.\ctrlcoolbar.h
# End Source File
# Begin Source File

SOURCE=.\CtrlCoolHeaderBar.h
# End Source File
# Begin Source File

SOURCE=.\CtrlIconCombo.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkPanel.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkThumb.h
# End Source File
# Begin Source File

SOURCE=.\ctrltaskpanel.h
# End Source File
# Begin Source File

SOURCE=.\CtrltrafficPanel.h
# End Source File
# Begin Source File

SOURCE=.\DlgAbout.h
# End Source File
# Begin Source File

SOURCE=.\DlgConnect.h
# End Source File
# Begin Source File

SOURCE=.\DlgJoinVisitor.h
# End Source File
# Begin Source File

SOURCE=.\dlgsettingsmanager.h
# End Source File
# Begin Source File

SOURCE=.\dlgsplash.h
# End Source File
# Begin Source File

SOURCE=.\imagefile.h
# End Source File
# Begin Source File

SOURCE=..\Common\md5.h
# End Source File
# Begin Source File

SOURCE=.\neighbour.h
# End Source File
# Begin Source File

SOURCE=..\Common\Packet.h
# End Source File
# Begin Source File

SOURCE=..\Common\PacketBase.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsAudio.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsVideo.h
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

SOURCE=.\WndNetwork.h
# End Source File
# Begin Source File

SOURCE=.\wndsettingspage.h
# End Source File
# Begin Source File

SOURCE=.\wndsettingssheet.h
# End Source File
# Begin Source File

SOURCE=.\WndTraffic.h
# End Source File
# Begin Source File

SOURCE=.\Xuser32.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\BannerMark.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\connect.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Connect.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Hand.cur
# End Source File
# Begin Source File

SOURCE=.\Res\Home.ico
# End Source File
# Begin Source File

SOURCE=.\Res\MenuBar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Music.ico
# End Source File
# Begin Source File

SOURCE=.\Res\PanelMark.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\products.bmp
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
# Begin Source File

SOURCE=.\res\Traffic.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Xuser32.ico
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
