# Microsoft Developer Studio Project File - Name="H263Codec" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=H263Codec - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "H263Codec.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "H263Codec.mak" CFG="H263Codec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "H263Codec - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "H263Codec - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CallCenter/H263Codec", FEDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "H263Codec - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\H263Codec.lib"

!ELSEIF  "$(CFG)" == "H263Codec - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "H263Codec___Win32_Debug"
# PROP BASE Intermediate_Dir "H263Codec___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\H263Codc.lib"

!ENDIF 

# Begin Target

# Name "H263Codec - Win32 Release"
# Name "H263Codec - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Encoder Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Encoder\coder.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\coder.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\config.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\countbit.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\countbit.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\ctables.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\dct.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\dct.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\def.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\Global.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\huffman.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\huffman.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\libr263.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\libr263.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\mot_est.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\mot_est.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\owntypes.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\pred.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\pred.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\quant.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\quant.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\rlib.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\rlib.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\stream.cpp
# End Source File
# Begin Source File

SOURCE=.\Encoder\stream.h
# End Source File
# Begin Source File

SOURCE=.\Encoder\struct.h
# End Source File
# End Group
# Begin Group "Decoder Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Decoder\convert.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\convert.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\decdef.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\decstruct.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\DGlobal.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\DGlobal.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\GetBits.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\GetBits.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\GetBlk.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\GetBlk.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\GetHdr.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\GetHdr.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\GetPic.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\GetPic.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\GetVlc.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\GetVlc.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\Idct.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\Idct.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\Idctref.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\Idctref.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\Indices.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\Indices.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\main.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Decoder\Recon.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\Recon.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\Sac.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\Sac.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\Sactbls.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\Sactbls.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\Tmndec.cpp
# End Source File
# Begin Source File

SOURCE=.\Decoder\Tmndec.h
# End Source File
# Begin Source File

SOURCE=.\Decoder\YuvToRgb.cpp
# End Source File
# End Group
# End Target
# End Project
