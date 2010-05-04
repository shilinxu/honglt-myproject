# Microsoft Developer Studio Project File - Name="XsalesAgency" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=XsalesAgency - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XsalesAgency.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XsalesAgency.mak" CFG="XsalesAgency - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XsalesAgency - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XsalesAgency - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CallCenter/Xuser32/XsalesAgency", HJCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XsalesAgency - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\XsalesAgency.dll ..\xusr.1234.dll > nul
# End Special Build Tool

!ELSEIF  "$(CFG)" == "XsalesAgency - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /map /debug /machine:I386 /out:"../XsalesAgency.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "XsalesAgency - Win32 Release"
# Name "XsalesAgency - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Common\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlAgencyList.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlAgencyPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAgencyLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAgencyProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\dllmain.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\md5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Packet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\PacketBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\PacketCore.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WndAgency.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\xml.cpp
# End Source File
# Begin Source File

SOURCE=.\XsalesAgency.def
# End Source File
# Begin Source File

SOURCE=.\XsalesAgency.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Common\buffer.h
# End Source File
# Begin Source File

SOURCE=.\CtrlAgencyList.h
# End Source File
# Begin Source File

SOURCE=.\CtrlAgencyPanel.h
# End Source File
# Begin Source File

SOURCE=.\DlgAgencyLayout.h
# End Source File
# Begin Source File

SOURCE=.\DlgAgencyProperties.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\Packet.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\PacketCore.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WndAgency.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\xml.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Agency.ico
# End Source File
# Begin Source File

SOURCE=.\res\BannerMark.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MenuBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PanelMark.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TaskPanel.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Default.xml
# End Source File
# Begin Source File

SOURCE=.\res\Definitions.xml
# End Source File
# End Target
# End Project
