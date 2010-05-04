# Microsoft Developer Studio Project File - Name="Xcenter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Xcenter - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Xcenter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Xcenter.mak" CFG="Xcenter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Xcenter - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Xcenter - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CallCenter/Xcenter", VWAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Xcenter - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib Ws2_32.lib Mpr.lib SHP_A3.lib /nologo /subsystem:windows /machine:I386 /out:"../Xcenter.exe" /libpath:"ShCti"

!ELSEIF  "$(CFG)" == "Xcenter - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib Ws2_32.lib Mpr.lib SHP_A3.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../Xcenter.exe" /pdbtype:sept /libpath:"ShCti"

!ENDIF 

# Begin Target

# Name "Xcenter - Win32 Release"
# Name "Xcenter - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Common\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\channel.cpp
# End Source File
# Begin Source File

SOURCE=.\channelclerk.cpp
# End Source File
# Begin Source File

SOURCE=.\channelfax.cpp
# End Source File
# Begin Source File

SOURCE=.\channeluser.cpp
# End Source File
# Begin Source File

SOURCE=.\channeluser2.cpp
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

SOURCE=.\ctrlclerkpanel.cpp
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

SOURCE=.\ctrliconcombo.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlMailPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrlmusicpanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrlnetworkchild.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrlnetworktree.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrlp2jobpanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlPagerPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrltaskpanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrltext.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlUserList.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrluserpanel.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgabout.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgexecute.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgpressermanager.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgprofilesmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgsettingsmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgsplash.cpp
# End Source File
# Begin Source File

SOURCE=.\EDClock.cpp
# End Source File
# Begin Source File

SOURCE=.\Handshake.cpp
# End Source File
# Begin Source File

SOURCE=.\Handshakes.cpp
# End Source File
# Begin Source File

SOURCE=.\hisreport.cpp
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

SOURCE=.\neighbour.cpp
# End Source File
# Begin Source File

SOURCE=.\network.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Packet.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\PacketCore.cpp
# End Source File
# Begin Source File

SOURCE=.\pagepresserfax.cpp
# End Source File
# Begin Source File

SOURCE=.\pagepresserinfo.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\pagepresserrec.cpp
# End Source File
# Begin Source File

SOURCE=.\pagepresseruser.cpp
# End Source File
# Begin Source File

SOURCE=.\PageProfileAnalog.cpp
# End Source File
# Begin Source File

SOURCE=.\pageprofilecorp.cpp
# End Source File
# Begin Source File

SOURCE=.\PageProfileFinal.cpp
# End Source File
# Begin Source File

SOURCE=.\PageProfileInter.cpp
# End Source File
# Begin Source File

SOURCE=.\pageprofileuser.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsCentric.cpp
# End Source File
# Begin Source File

SOURCE=.\pagesettingsgeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsMaster.cpp
# End Source File
# Begin Source File

SOURCE=.\pagesettingsmeeting.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsOptimize.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsPager.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsTrunk.cpp
# End Source File
# Begin Source File

SOURCE=.\PageSettingsUser.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Pipeline.cpp
# End Source File
# Begin Source File

SOURCE=.\profiles.cpp
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

SOURCE=.\traffic.cpp
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

SOURCE=.\wndclerk.cpp
# End Source File
# Begin Source File

SOURCE=.\WndMail.cpp
# End Source File
# Begin Source File

SOURCE=.\wndmain.cpp
# End Source File
# Begin Source File

SOURCE=.\wndmusic.cpp
# End Source File
# Begin Source File

SOURCE=.\wndnetwork.cpp
# End Source File
# Begin Source File

SOURCE=.\WndPager.cpp
# End Source File
# Begin Source File

SOURCE=.\wndsettingspage.cpp
# End Source File
# Begin Source File

SOURCE=.\wndsettingssheet.cpp
# End Source File
# Begin Source File

SOURCE=.\wnduser.cpp
# End Source File
# Begin Source File

SOURCE=.\Xcenter.cpp
# End Source File
# Begin Source File

SOURCE=.\Xcenter.rc
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

SOURCE=.\channel.h
# End Source File
# Begin Source File

SOURCE=.\channelclerk.h
# End Source File
# Begin Source File

SOURCE=.\channelfax.h
# End Source File
# Begin Source File

SOURCE=.\channeluser.h
# End Source File
# Begin Source File

SOURCE=.\channeluser2.h
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

SOURCE=.\ctrlclerkpanel.h
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

SOURCE=.\ctrliconcombo.h
# End Source File
# Begin Source File

SOURCE=.\CtrlMailPanel.h
# End Source File
# Begin Source File

SOURCE=.\ctrlmusicpanel.h
# End Source File
# Begin Source File

SOURCE=.\ctrlnetworkchild.h
# End Source File
# Begin Source File

SOURCE=.\ctrlnetworktree.h
# End Source File
# Begin Source File

SOURCE=.\ctrlp2jobpanel.h
# End Source File
# Begin Source File

SOURCE=.\CtrlPagerPanel.h
# End Source File
# Begin Source File

SOURCE=.\ctrltaskpanel.h
# End Source File
# Begin Source File

SOURCE=.\ctrltext.h
# End Source File
# Begin Source File

SOURCE=.\CtrlUserList.h
# End Source File
# Begin Source File

SOURCE=.\ctrluserpanel.h
# End Source File
# Begin Source File

SOURCE=.\dlgabout.h
# End Source File
# Begin Source File

SOURCE=.\dlgexecute.h
# End Source File
# Begin Source File

SOURCE=.\dlgpressermanager.h
# End Source File
# Begin Source File

SOURCE=.\dlgprofilesmanager.h
# End Source File
# Begin Source File

SOURCE=.\dlgsettingsmanager.h
# End Source File
# Begin Source File

SOURCE=.\dlgsplash.h
# End Source File
# Begin Source File

SOURCE=.\EDClock.h
# End Source File
# Begin Source File

SOURCE=.\Handshake.h
# End Source File
# Begin Source File

SOURCE=.\Handshakes.h
# End Source File
# Begin Source File

SOURCE=.\hisreport.h
# End Source File
# Begin Source File

SOURCE=.\imagefile.h
# End Source File
# Begin Source File

SOURCE=.\Interlink.h
# End Source File
# Begin Source File

SOURCE=.\neighbour.h
# End Source File
# Begin Source File

SOURCE=.\network.h
# End Source File
# Begin Source File

SOURCE=..\Common\Packet.h
# End Source File
# Begin Source File

SOURCE=..\Common\PacketCore.h
# End Source File
# Begin Source File

SOURCE=.\pagepresserfax.h
# End Source File
# Begin Source File

SOURCE=.\pagepresserinfo.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\pagepresserrec.h
# End Source File
# Begin Source File

SOURCE=.\pagepresseruser.h
# End Source File
# Begin Source File

SOURCE=.\PageProfileAnalog.h
# End Source File
# Begin Source File

SOURCE=.\pageprofilecorp.h
# End Source File
# Begin Source File

SOURCE=.\PageProfileFinal.h
# End Source File
# Begin Source File

SOURCE=.\PageProfileInter.h
# End Source File
# Begin Source File

SOURCE=.\pageprofileuser.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsCentric.h
# End Source File
# Begin Source File

SOURCE=.\pagesettingsgeneral.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsMaster.h
# End Source File
# Begin Source File

SOURCE=.\pagesettingsmeeting.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsMonitor.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsOptimize.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsPager.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsTrunk.h
# End Source File
# Begin Source File

SOURCE=.\PageSettingsUser.h
# End Source File
# Begin Source File

SOURCE=..\Common\Pipeline.h
# End Source File
# Begin Source File

SOURCE=.\profiles.h
# End Source File
# Begin Source File

SOURCE=.\registry.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
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

SOURCE=.\traffic.h
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

SOURCE=.\wndclerk.h
# End Source File
# Begin Source File

SOURCE=.\WndMail.h
# End Source File
# Begin Source File

SOURCE=.\wndmain.h
# End Source File
# Begin Source File

SOURCE=.\wndmusic.h
# End Source File
# Begin Source File

SOURCE=.\wndnetwork.h
# End Source File
# Begin Source File

SOURCE=.\WndPager.h
# End Source File
# Begin Source File

SOURCE=.\wndsettingspage.h
# End Source File
# Begin Source File

SOURCE=.\wndsettingssheet.h
# End Source File
# Begin Source File

SOURCE=.\wnduser.h
# End Source File
# Begin Source File

SOURCE=.\Xcenter.h
# End Source File
# Begin Source File

SOURCE=..\Common\xml.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ChanBar16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\clerk.ico
# End Source File
# Begin Source File

SOURCE=.\res\EDClock.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\home.ico
# End Source File
# Begin Source File

SOURCE=.\res\Mail.ico
# End Source File
# Begin Source File

SOURCE=.\res\menubar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\music.ico
# End Source File
# Begin Source File

SOURCE=.\res\navbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Pager.ico
# End Source File
# Begin Source File

SOURCE=.\res\panelmark.bmp
# End Source File
# Begin Source File

SOURCE=.\res\products.bmp
# End Source File
# Begin Source File

SOURCE=.\res\searchfolder.ico
# End Source File
# Begin Source File

SOURCE=.\res\taskboxcaption.bmp
# End Source File
# Begin Source File

SOURCE=.\res\taskpanel.ico
# End Source File
# Begin Source File

SOURCE=.\res\taskpanelfooter.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\user.ico
# End Source File
# Begin Source File

SOURCE=.\res\Xcenter.ico
# End Source File
# Begin Source File

SOURCE=.\res\Xcenter.rc2
# End Source File
# Begin Source File

SOURCE=.\res\XcenterDoc.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Default.xml
# End Source File
# Begin Source File

SOURCE=.\res\Definitions.xml
# End Source File
# Begin Source File

SOURCE=.\res\manifest.xml
# End Source File
# End Target
# End Project
