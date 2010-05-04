# Microsoft Developer Studio Project File - Name="Xgateway" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Xgateway - Win32 Debug_Best
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Xgateway.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Xgateway.mak" CFG="Xgateway - Win32 Debug_Best"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Xgateway - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Xgateway - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Xgateway - Win32 Debug_Best" (based on "Win32 (x86) Application")
!MESSAGE "Xgateway - Win32 Release_Best" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CallCenter/Xgateway", LPCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Xgateway - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Common" /I "H263Codec" /I "XvidCodec" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib Ws2_32.lib Winmm.lib Mpr.lib msacm32.lib Vfw32.lib Strmiids.lib strmbase.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"\\192.168.0.18\d$\callcenter\Xgateway.exe" /libpath:"E:\DXSDK\Samples\C++\DirectShow\BaseClasses\Release"

!ELSEIF  "$(CFG)" == "Xgateway - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Common" /I "H263Codec" /I "XvidCodec" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib Ws2_32.lib Winmm.lib Mpr.lib msacm32.lib Vfw32.lib Strmiids.lib strmbasd.lib jrtplibd.lib jthread.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"\\192.168.0.18\d$\callcenter\Xgateway.exe" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Xgateway - Win32 Debug_Best"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Xgateway___Win32_Debug_Best"
# PROP BASE Intermediate_Dir "Xgateway___Win32_Debug_Best"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xgateway___Win32_Debug_Best"
# PROP Intermediate_Dir "Xgateway___Win32_Debug_Best"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Common" /I "H263Codec" /I "XvidCodec" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Common" /I "H263Codec" /I "XvidCodec" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_BEST" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib Ws2_32.lib Winmm.lib msacm32.lib Vfw32.lib Strmiids.lib strmbasd.lib jrtplibd.lib jthread.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"\\192.168.0.18\d$\callcenter\XgatewayDebug.exe" /pdbtype:sept
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 version.lib Ws2_32.lib Winmm.lib Mpr.lib msacm32.lib Vfw32.lib Strmiids.lib strmbasd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"\\192.168.0.18\d$\callcenter\XgatewayDebug.exe" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Xgateway - Win32 Release_Best"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Xgateway___Win32_Release_Best0"
# PROP BASE Intermediate_Dir "Xgateway___Win32_Release_Best0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Xgateway___Win32_Release_Best"
# PROP Intermediate_Dir "Xgateway___Win32_Release_Best"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\Common" /I "H263Codec" /I "XvidCodec" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Common" /I "H263Codec" /I "XvidCodec" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_BEST" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 version.lib Ws2_32.lib Winmm.lib Mpr.lib msacm32.lib Vfw32.lib Strmiids.lib strmbase.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"\\192.168.0.18\d$\callcenter\Xgateway.exe" /libpath:"E:\DXSDK\Samples\C++\DirectShow\BaseClasses\Release"
# ADD LINK32 version.lib Ws2_32.lib Winmm.lib Mpr.lib msacm32.lib Vfw32.lib Strmiids.lib strmbase.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"\\192.168.0.18\d$\callcenter\XgatewayDebug.exe" /libpath:"E:\DXSDK\Samples\C++\DirectShow\BaseClasses\Release"

!ENDIF 

# Begin Target

# Name "Xgateway - Win32 Release"
# Name "Xgateway - Win32 Debug"
# Name "Xgateway - Win32 Debug_Best"
# Name "Xgateway - Win32 Release_Best"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Common\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Channel.cpp
# End Source File
# Begin Source File

SOURCE=.\ChannelUser.cpp
# End Source File
# Begin Source File

SOURCE=.\Circuitry.cpp
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

SOURCE=.\ctrlcoolmenubar.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrliconbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkChild.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkHeaderBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkTree.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkTreeBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkVideo.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrltaskpanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrltext.cpp
# End Source File
# Begin Source File

SOURCE=.\Datagram.cpp
# End Source File
# Begin Source File

SOURCE=.\Datagrams.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgabout.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProfileManager.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgsettingsmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgsplash.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\FilterGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterRtpReceiver.cpp
# End Source File
# Begin Source File

SOURCE=.\imagefile.cpp
# End Source File
# Begin Source File

SOURCE=.\Interlink.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\Neighbour.cpp
# End Source File
# Begin Source File

SOURCE=.\Network.cpp
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

SOURCE=.\PageProfileSecure.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsCentric.cpp
# End Source File
# Begin Source File

SOURCE=.\pagesettingsgeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsNetwork.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Pipeline.cpp
# End Source File
# Begin Source File

SOURCE=.\Profiles.cpp
# End Source File
# Begin Source File

SOURCE=.\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderGraph.cpp
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

SOURCE=.\transfer.cpp
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

SOURCE=.\Xgateway.cpp
# End Source File
# Begin Source File

SOURCE=.\Xgateway.rc
# End Source File
# Begin Source File

SOURCE=..\Common\xml.cpp
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

SOURCE=.\ChannelUser.h
# End Source File
# Begin Source File

SOURCE=.\Circuitry.h
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

SOURCE=.\ctrlcoolmenubar.h
# End Source File
# Begin Source File

SOURCE=.\ctrliconbutton.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkChild.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkHeaderBar.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkTree.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkTreeBar.h
# End Source File
# Begin Source File

SOURCE=.\CtrlNetworkVideo.h
# End Source File
# Begin Source File

SOURCE=.\ctrltaskpanel.h
# End Source File
# Begin Source File

SOURCE=.\ctrltext.h
# End Source File
# Begin Source File

SOURCE=.\Datagram.h
# End Source File
# Begin Source File

SOURCE=.\Datagrams.h
# End Source File
# Begin Source File

SOURCE=.\dlgabout.h
# End Source File
# Begin Source File

SOURCE=.\DlgProfileManager.h
# End Source File
# Begin Source File

SOURCE=.\dlgsettingsmanager.h
# End Source File
# Begin Source File

SOURCE=.\dlgsplash.h
# End Source File
# Begin Source File

SOURCE=..\Common\FilterGraph.h
# End Source File
# Begin Source File

SOURCE=.\FilterRtpReceiver.h
# End Source File
# Begin Source File

SOURCE=.\imagefile.h
# End Source File
# Begin Source File

SOURCE=.\Interlink.h
# End Source File
# Begin Source File

SOURCE=.\Neighbour.h
# End Source File
# Begin Source File

SOURCE=.\Network.h
# End Source File
# Begin Source File

SOURCE=..\Common\Packet.h
# End Source File
# Begin Source File

SOURCE=..\Common\PacketBase.h
# End Source File
# Begin Source File

SOURCE=..\Common\PacketCore.h
# End Source File
# Begin Source File

SOURCE=.\PageProfileSecure.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsCentric.h
# End Source File
# Begin Source File

SOURCE=.\pagesettingsgeneral.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsNetwork.h
# End Source File
# Begin Source File

SOURCE=..\Common\Pipeline.h
# End Source File
# Begin Source File

SOURCE=.\Profiles.h
# End Source File
# Begin Source File

SOURCE=.\registry.h
# End Source File
# Begin Source File

SOURCE=.\RenderGraph.h
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

SOURCE=.\transfer.h
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

SOURCE=.\Xgateway.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\Hand.cur
# End Source File
# Begin Source File

SOURCE=.\Res\Home.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Media.ico
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

SOURCE=.\res\TaskPanel.ico
# End Source File
# Begin Source File

SOURCE=.\Res\TaskPanelFooter.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Xgateway.ico
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
