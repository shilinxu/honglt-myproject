# Microsoft Developer Studio Project File - Name="XinnerJoin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=XinnerJoin - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XinnerJoin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XinnerJoin.mak" CFG="XinnerJoin - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XinnerJoin - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XinnerJoin - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CallCenter/Xuser32/XinnerJoin", LRCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XinnerJoin - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XinnerJoin_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\\" /I "..\Xuserface" /I "..\..\Common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 Winmm.lib Xuserface.lib /nologo /dll /machine:I386 /out:"../XinnerJoin.dll" /libpath:"..\Xuserface\Release"

!ELSEIF  "$(CFG)" == "XinnerJoin - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XinnerJoin_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\Xuserface" /I "..\..\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Winmm.lib Xuserface.lib /nologo /dll /map /debug /machine:I386 /out:"../XinnerJoin.dll" /pdbtype:sept /libpath:"..\Xuserface\Debug"

!ENDIF 

# Begin Target

# Name "XinnerJoin - Win32 Release"
# Name "XinnerJoin - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Common\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlInnerHeaderBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlInnerList.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlInnerPanel.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DlgUserMeeting.cpp
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

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WndInner.cpp
# End Source File
# Begin Source File

SOURCE=.\XinnerJoin.def
# End Source File
# Begin Source File

SOURCE=.\XinnerJoin.rc
# End Source File
# Begin Source File

SOURCE=..\..\Common\xml.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Common\buffer.h
# End Source File
# Begin Source File

SOURCE=.\CtrlInnerHeaderBar.h
# End Source File
# Begin Source File

SOURCE=.\CtrlInnerList.h
# End Source File
# Begin Source File

SOURCE=.\CtrlInnerPanel.h
# End Source File
# Begin Source File

SOURCE=.\DlgUserMeeting.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\md5.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\Packet.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\PacketBase.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WndInner.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\china.ico
# End Source File
# Begin Source File

SOURCE=.\res\dtmfbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Employee.ico
# End Source File
# Begin Source File

SOURCE=.\res\MenuBar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\PanelMark.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Default.xml
# End Source File
# Begin Source File

SOURCE=.\res\Definitions.xml
# End Source File
# Begin Source File

SOURCE=.\res\Ring.wav
# End Source File
# Begin Source File

SOURCE=.\res\signal.dtmf
# End Source File
# End Target
# End Project
